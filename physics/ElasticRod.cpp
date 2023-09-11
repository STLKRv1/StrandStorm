#include <ElasticRod.hpp>
#include <Logging.hpp>
#include <spdlog/fmt/fmt.h>

// Elastic rod sim constants
float ElasticRod::drag = 10.0f;
float ElasticRod::bendingStiffness = 0.003f;
float ElasticRod::friction = 0.0f;
float ElasticRod::sampledVelocityScale = 0.0001f;
float ElasticRod::maxForce = 100.f;
Vector3f ElasticRod::gravity = {0.0f, -0.25f, 0.0f};

Vector3f ElasticRod::kappaB(int i)
{
    if(i==0)
        return Vector3f::Zero();
    return (2.0f * edge(i-1).cross(edge(i))) /
           (initEdge(i).norm() * initEdge(i-1).norm() + edge(i-1).dot(edge(i)));
}

float ElasticRod::initEdgeLen(int i)
{
    return edge(i-1).norm() + edge(i).norm();
}

Vector3f ElasticRod::psiGrad(int i, int j)
{
    if (j < i-1 || j > i+1) {
        return Vector3f::Zero();
    }
    const Vector3f kb = kappaB(i);
    if (j == i - 1)
    {
        return kb / (2.0f * edge(i - 1).norm());
    }
    else if (j == i + 1)
    {
        return -kb / (2.0f * edge(i).norm());
    }
    return -kb / (2.0f * edge(i - 1).norm()) - kb / (2.0f * edge(i).norm());
}

Matrix3f ElasticRod::kappaBGrad(int i, int j)
{
    if (j < i-1 || j > i+1 || j < 0 || j >= x.size()) {
        return Matrix3f::Zero();
    }
    const Vector3f kb = kappaB(i);
    const float denom = initEdge(i-1).norm() * initEdge(i).norm() + edge(i-1).dot(edge(i));
    if (j == i - 1) {
        return (2.0f * skew(edge(i)) + kb * edge(i).transpose()) / denom;
    }
    else if (j == i + 1) {
        return (2.0f * skew(edge(i-1)) - kb * edge(i-1).transpose()) / denom;
    }
    return -((2.0f * skew(edge(i)) + kb * edge(i).transpose()) +
        (2.0f * skew(edge(i-1)) - kb * edge(i-1).transpose())) /
        denom;
}

Vector2f ElasticRod::omega(const Vector3f& kb,int i, int j) 
{
    assert(j == i-1 || j == i);
    j = std::clamp(j, 0, (int)(x.size() - 1));
    return {kb.dot(M[j].m2), -kb.dot(M[j].m1)};
}

const Matrix2f J = Rotation2Df(pi / 2.0f).matrix();

Matrix<float, 2, 3> ElasticRod::omegaGrad(int i, int j, int k)
{
    Matrix<float, 2, 3> m;
    m.setZero();
    if (k >= i-1 && k <= i+1) {
        m.row(0) = M[j].m2;
        m.row(1) = -M[j].m1;
        m *= kappaBGrad(i, k);
    }
    const Vector3f kb = kappaB(k);
    return m - J * omega(kb,k, j) * gradHolonomy(i, j).transpose();
}

Vector3f ElasticRod::gradHolonomy(int i, int j)
{
    // Vector3f gh = Vector3f::Zero();
    // for (int k = 1; k <= j; k++) {
    //     gh += psiGrad(k, i);
    // }
    if(std::abs(j-i)>1)
        return Vector3f::Zero();
    Vector3f kb = kappaB(j);
    if(j==i+1)
        return kb / (2.0f * initEdge(i).norm());
    else if(j==i-1)
        return -kb / (2.0f * initEdge(j).norm());
    else
        return -(kb/(2.0f * initEdge(i).norm())) + (kb / (2.0f * initEdge(j).norm()));
}

Vector3f ElasticRod::dEdX(int i)
{
    const Matrix2f B = Matrix2f::Identity() * this->bendingStiffness;
    Vector3f f = Vector3f::Zero();
    // i-1 instead of 1 because the terms go to 0 for k<i-1
    for (int k = std::max(i-1,1); k < x.size()-1; k++) 
    {
        Vector3f pf = Vector3f::Zero();
        const Vector3f kb = kappaB(k);
        for (int j = k-1; j <= k; j++) 
        {
            pf += omegaGrad(i, j, k).transpose() * B * (omega(kb,k, j) - omega0[k][j-k+1]);
        }
        f += pf / initEdgeLen(k);
    }
    if(f.squaredNorm() > maxForce * maxForce)
        f = f.normalized() * maxForce;
    return -f;
}

void ElasticRod::computeCosAndSin(float sqMag, float & cosPhi, float & sinPhi)
{
    cosPhi = sqrt(4.0f/(sqMag + 4.0f));
    sinPhi = sqrt(sqMag/(sqMag + 4.0f));
}

void ElasticRod::compBishopFrames()
{
    bishopFrames[0] = {u0, edge(0).cross(u0).normalized()};
    for (int i = 1; i < x.size()-1; i++) 
    {
        const Vector3f kb = kappaB(i);
        const Vector3f kbNorm = kb.normalized();
        // const float angle = 2.0f * std::atan(kb.norm() / 2.0f);
        // if(std::isnan(angle))
        //     std::cout<<"NAN";

        // AngleAxisf P_i(angle, kb.normalized());
        // Vector3f u = (P_i * bishopFrames[std::max(i-1, 0)].u).normalized();
        // Vector3f v = edge(i).cross(u).normalized();
        // bishopFrames[i] = {u, v};
        
        float cosPhi, sinPhi;
        computeCosAndSin(kb.squaredNorm(), cosPhi, sinPhi);
        if(1.0f - cosPhi < 1e-6f)
        {
            bishopFrames[i] = {bishopFrames[i-1].u, bishopFrames[i-1].v};
            continue;
        }
        Quaternion rotationQuat(cosPhi, sinPhi * kbNorm.x(), sinPhi * kbNorm.y(), sinPhi * kbNorm.z());        
        Quaternion uQuat(0.0f, bishopFrames[i-1].u.x(), bishopFrames[i-1].u.y(), bishopFrames[i-1].u.z());
        Quaternion rotationQuatConjugate = rotationQuat.conjugate();
        auto rotatedVec = rotationQuat * uQuat * rotationQuatConjugate;
        Vector3f u = Vector3f(rotatedVec.x(), rotatedVec.y(), rotatedVec.z()).normalized();
        bishopFrames[i] = {u, edge(i).cross(u).normalized()};      
    }
    // u0 = bishopFrames[0].u;
}

void ElasticRod::compMatFrames()
{
    for (int i = 0; i < x.size()-1; i++) {
        // Since theta is always zero in our case, material frame is same as bishop frame
        /* M[i] = {
            std::cos(theta[i]) * bishopFrames[i].u + std::sin(theta[i]) * bishopFrames[i].v,
            -std::sin(theta[i]) * bishopFrames[i].u + std::cos(theta[i]) * bishopFrames[i].v
        }; */
        M[i] = {
            bishopFrames[i].u,
            bishopFrames[i].v
        };
    }
}

void ElasticRod::parallelTransportFrameInTime(const Vector3f& prevEdge)
{
    const Vector3f kb = 2*prevEdge.cross(edge(0)) / (prevEdge.norm() * edge(0).norm() + prevEdge.dot(edge(0)));
    const Vector3f kbNorm = kb.normalized();
    float cosPhi, sinPhi;
    computeCosAndSin(kb.squaredNorm(), cosPhi, sinPhi);
    if(1.0f - cosPhi < 1e-6f)
    {
        u0 = edge(0).cross(u0);
        u0 = u0.cross(edge(0)).normalized();
        return;
    }
    Quaternion rotationQuat(cosPhi, sinPhi * kbNorm.x(), sinPhi * kbNorm.y(), sinPhi * kbNorm.z());        
    Quaternion uQuat(0.0f, u0.x(), u0.y(), u0.z());
    Quaternion rotationQuatConjugate = rotationQuat.conjugate();
    auto rotatedVec = rotationQuat * uQuat * rotationQuatConjugate;
    u0 = Vector3f(rotatedVec.x(), rotatedVec.y(), rotatedVec.z()).normalized();
}

Vector3f ElasticRod::edge(int i)
{
    i = std::clamp(i, 0, (int)(x.size() - 2));
    return e[i];
}

Vector3f ElasticRod::initEdge(int i)
{
    i = std::clamp(i, 0, (int)(x.size() - 2));
    return xRest[i + 1] - xRest[i];
}

Vector3f ElasticRod::force(int i)
{
    assert(i >= 1);
    Vector3f f = dEdX(i);
    assert(std::isfinite(f.norm()));
    return f;
}

void ElasticRod::init(const std::vector<glm::vec3> &verts)
{
    x.resize(verts.size(), Vector3f::Zero());
    e.resize(verts.size()-1, Vector3f::Zero());
    v.resize(verts.size(), Vector3f::Zero());
    theta.resize(verts.size(), 0.0f);
    omega0.resize(verts.size());
    bishopFrames.resize(verts.size());
    M.resize(verts.size());

    for (int i = 0; i < verts.size(); i++)  {
        x[i] = Vector3f(verts[i].x, verts[i].y, verts[i].z);
    }
    xRest = x;

    reset();
}

ElasticRod::ElasticRod(const std::vector<glm::vec3> &verts)
{
    init(verts);
}


void ElasticRod::integrateFwEuler(float dt)
{
    Vector3f prevEdge = edge(0);
    for (int i = 0; i < e.size(); i++) {
        e[i] = x[i+1] - x[i];
    }

    parallelTransportFrameInTime(prevEdge);
    compBishopFrames();
    compMatFrames();

    px = x;
    for (int i = 1; i < x.size(); i++) 
    {
        v[i] += (force(i) + gravity) * dt ;
        v[i] -= 0.5f * drag * v[i].squaredNorm() * v[i].normalized() * dt;
        x[i] += v[i] * dt;
    }
}

void ElasticRod::handleCollisions(const std::vector<std::shared_ptr<SceneObject>>& colliders)
{
    SphereCollider vertCollider(Eigen::Vector3f(0.0f, 0.0f, 0.0f), 0.0001f);
    CollisionInfo col;
    for (int i = 1; i < x.size(); i++) {
        for (const std::shared_ptr<SceneObject>& c : colliders) {
            vertCollider.center = x[i];
            if (c->collider->IsCollidingWith(vertCollider, col)) {
                x[i] = c->collider->center - col.normal * c->collider->GetBoundaryAt(x[i]);
            }
        }
    }
}

void ElasticRod::enforceConstraints(float dt, const std::vector<std::shared_ptr<SceneObject>>& colliders)
{
    handleCollisions(colliders);
    x[0] = xRest[0];
    std::vector<Vector3f> dx(x.size());
    dx[0] = Vector3f::Zero();
    for (int i = 1; i < x.size(); i++) {
        // Inextensibility: needs to be perfectly inextensible otherwise problem
        dx[i] = x[i-1] + (x[i] - x[i-1]).normalized() * initEdge(i-1).norm() - x[i];
        x[i] += dx[i];
        // v[i] -= 0.5f * drag * v[i].squaredNorm() * v[i].normalized() * dt;
    }
    for (int i = 1; i < x.size()-1; i++) {
        // Velocity must be corrected according to position correction
        v[i] = (x[i] - px[i]) / dt - 0.25f * (dx[i+1] / dt);
    }
}


void ElasticRod::setVoxelContributions(const std::shared_ptr<VoxelGrid>& voxelGrid)
{    
    Eigen::Vector3f firstVoxelCoord,localPosition;
    for (size_t ind = 1; ind < x.size(); ind++)
    {
        voxelGrid->getVoxelCoordinates(x[ind],firstVoxelCoord,localPosition);
        int numSteps = (int)(voxelGrid->voxelGridExtent/voxelGrid->voxelSize);
        for(size_t i=0;i<=1;i++)
        for(size_t j=0;j<=1;j++)
        for(size_t k=0;k<=1;k++)
        {
            Eigen::Vector3f corner = firstVoxelCoord + Eigen::Vector3f(i,j,k);
            if(corner(0)>=numSteps || corner(1)>=numSteps || corner(2)>=numSteps)
                continue;
            size_t hash = voxelGrid->getSpatialHash(corner);
            corner -= localPosition;
            corner = Eigen::Vector3f(1.0f,1.0f,1.0f) - Eigen::Vector3f(corner.array().abs()); 
            
            voxelGrid->voxelMutex.lock();            
            voxelGrid->voxelMasses[hash] += corner.prod();
            voxelGrid->voxelVelocities[hash] += corner.prod() * v[i];               
            voxelGrid->voxelMutex.unlock();
        }
    }
}


void ElasticRod::updateAllVelocitiesFromVoxels(const std::shared_ptr<VoxelGrid>& voxelGrid)
{
    Eigen::Vector3f firstVoxelCoord, localPosition, velocity;
    for (size_t i = 1; i < x.size(); i++)
    {
        voxelGrid->getVoxelCoordinates(x[i], firstVoxelCoord, localPosition);

        // Get the 8 velocities of the 8 corners of the voxel containing the sampling point
        Eigen::Vector3f corner000, corner001, corner100, corner101, corner010, corner011, corner110, corner111;
        voxelGrid->sampleVoxelVelocity(corner000, firstVoxelCoord);
        voxelGrid->sampleVoxelVelocity(corner001, firstVoxelCoord + Eigen::Vector3f(0, 0, 1));
        voxelGrid->sampleVoxelVelocity(corner100, firstVoxelCoord + Eigen::Vector3f(1, 0, 0));
        voxelGrid->sampleVoxelVelocity(corner101, firstVoxelCoord + Eigen::Vector3f(1, 0, 1));
        voxelGrid->sampleVoxelVelocity(corner010, firstVoxelCoord + Eigen::Vector3f(0, 1, 0));
        voxelGrid->sampleVoxelVelocity(corner011, firstVoxelCoord + Eigen::Vector3f(0, 1, 1));
        voxelGrid->sampleVoxelVelocity(corner110, firstVoxelCoord + Eigen::Vector3f(1, 1, 0));
        voxelGrid->sampleVoxelVelocity(corner111, firstVoxelCoord + Eigen::Vector3f(1, 1, 1));

        // Perform trilinear interpolation to get the velocity at the sampling point
        Eigen::Vector3f lp_interp1 = (1.0f - localPosition[2]) * corner000 + localPosition[2] * corner001;
        Eigen::Vector3f lp_interp2 = (1.0f - localPosition[2]) * corner100 + localPosition[2] * corner101;
        Eigen::Vector3f lp = (1.0f - localPosition[0]) * lp_interp1 + localPosition[0] * lp_interp2;

        Eigen::Vector3f up_interp1 = (1.0f - localPosition[2]) * corner010 + localPosition[2] * corner011;
        Eigen::Vector3f up_interp2 = (1.0f - localPosition[2]) * corner110 + localPosition[2] * corner111;
        Eigen::Vector3f up = (1.0f - localPosition[0]) * up_interp1 + localPosition[0] * up_interp2;

        velocity = (1.0f - localPosition[1]) * lp + localPosition[1] * up;
        velocity *= sampledVelocityScale;
        v[i] = (1-friction) * v[i] + friction * velocity;
    }
}

void ElasticRod::reset()
{
    x = xRest;
    v.assign(v.size(), Vector3f::Zero());

    for (int i = 0; i < e.size(); i++) {
        e[i] = x[i+1] - x[i];
    }
    u0 = edge(0).cross(Vector3f::UnitX()).cross(edge(0)).normalized();

    compBishopFrames();
    compMatFrames();

    // Compute initial material curvature
    omega0[0] = {Vector2f::Zero(), Vector2f::Zero()};
    for (int i = 1; i < x.size()-1; i++) 
    {
        const Vector3f kb = kappaB(i);
        for (int j = 0; j < 2; j++)
        {
            omega0[i][j] = omega(kb,i, i + j-1);
        }
    }
}

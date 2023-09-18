#include <unordered_set>
#include <Mesh.hpp>
#include <Logging.hpp>
#include <ElasticRod.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <array>

uint64_t cantor(uint32_t x, uint32_t y) {
    return ((x + y) * (x + y + 1u)) / 2u + y;
}

// --- HairMesh --------------------------------------------------------------

void HairMesh::build(const OpenGLProgram &prog)
{
    spdlog::assrt(!this->vaoInitialized, "HairMesh already built");
    this->vaoInitialized = true;
    glGenVertexArrays(1, &this->vao) $gl_chk;
    glBindVertexArray(vao) $gl_chk;
    this->vboInterp = gl::buffer(GL_ARRAY_BUFFER, numInterpVertices() * sizeof(glm::vec4));
    this->eboInterp = gl::buffer(GL_ELEMENT_ARRAY_BUFFER, numInterpElements() * sizeof(GLuint));
    this->eboTris = gl::buffer(GL_ELEMENT_ARRAY_BUFFER, tris);
    this->vboControl = gl::buffer(GL_ARRAY_BUFFER, this->controlVerts, GL_DYNAMIC_DRAW);
    this->vboTangents = gl::buffer(GL_ARRAY_BUFFER, numInterpVertices() * sizeof(glm::vec4));
    
    prog.SetAttribPointer(vboInterp, "vPos", 4, GL_FLOAT);
    prog.SetAttribPointer(vboControl, "vPos", 4, GL_FLOAT);
    prog.SetAttribPointer(vboTangents, "vTangent", 4, GL_FLOAT);
}

void HairMesh::updateBuffer()
{
    glBindVertexArray(vao) $gl_chk;
    glBindBuffer(GL_ARRAY_BUFFER, vboControl) $gl_chk;
    glBufferSubData(GL_ARRAY_BUFFER, 0, controlVerts.size() * sizeof(glm::vec4), controlVerts.data()) $gl_chk;
}

void HairMesh::loadFromFile(const std::string &modelPath, bool compNormals)
{
    spdlog::assrt(fs::exists(modelPath), "model '{}' not found", modelPath);

    // Load the mesh
    cyTriMesh mesh;
    if (!mesh.LoadFromFileObj(modelPath.c_str())) {
        spdlog::error("Failed to load mesh from file: {}", modelPath);
    }
    mesh.ComputeNormals();

    // normalize the mesh to fit in a 5x5x5 cube
    mesh.ComputeBoundingBox();
    float yFactor = mesh.GetBoundMax().y;
    yFactor = yFactor == 0.0 ? 1.0f : yFactor;
    const auto scale =  1.0f / yFactor;
    //go over the vertices and scale them
    for (int i = 0; i < mesh.NV(); i++) {
        mesh.V(i) *= scale;
    }

    // Grow the control hairs from the stored vertices
    if (controlHairDensity == 0) {
        for (int i = 0; i < (int)mesh.NV(); i++) {
            if (maxControlHairs > 0 && i >= maxControlHairs) break;
            growControlHair(glm::make_vec3(mesh.V(i)), glm::normalize(glm::make_vec3(mesh.VN(i))));
        }
    } else {
        for (int i = 0; i < mesh.NF(); i++) {
            std::array<glm::vec3, 3> v, n;
            for (size_t j = 0; j < 3; j++) {
                v[j] = glm::make_vec3(&mesh.V(mesh.F(i).v[j])[0]);
                n[j] = glm::make_vec3(&mesh.VN(mesh.FN(i).v[j])[0]);
            }
            const auto vertices = tessTriangleGrid<8>(v);
            const auto normals = tessTriangleGrid<8>(n);
            for (size_t i = 0; i < vertices.size(); i++) {
                growControlHair(vertices[i], normals[i]);
            }
        }
    }
    // Triangles
    for (int i = 0; i < mesh.NF(); i++) {
        const cy::TriMesh::TriFace& f = mesh.F(i);
        bool skip = false;
        for (int j = 0; j < 3; j++) {
            if (f.v[j] >= this->numControlHairs()) {
                skip = true;
                break;
            }
        }
        if (skip) continue;
        for (int j = 0; j < 3; j++) {
            this->tris.push_back(f.v[j]);
        }
    }

    spdlog::debug("{}: {} control hairs, {} output vertices, {} output elems, {} output hairs, {} bezier control points, {} triangles",
        modelPath, numControlHairs(), numInterpVertices(), numInterpElements(), numInterpHairs(), numControlPoints(), numTris());
}

void HairMesh::draw(const OpenGLProgram &prog)
{
    if(!show)
        return;
    glBindVertexArray(this->vao) $gl_chk;

    if (drawControlHairs) {
        prog.SetUniform("hairColor", glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), false);
        glBindBuffer(GL_ARRAY_BUFFER, this->vboControl) $gl_chk;
        glVertexAttribPointer(prog.AttribLocation("vPos"), 4, GL_FLOAT, GL_FALSE, 0, (void*)0) $gl_chk;
        for (size_t i = 0; i < numControlHairs(); i++) {
            glDrawArrays(GL_LINE_STRIP, i * controlHairLen, controlHairLen) $gl_chk;
        }
    }

    prog.SetUniform("hairColor", glm::vec4(0.57f, 0.48f, 0.0f, 0.7f), false);
    glBindBuffer(GL_ARRAY_BUFFER, this->vboInterp) $gl_chk;
    glVertexAttribPointer(prog.AttribLocation("vPos"), 4, GL_FLOAT, GL_FALSE, 0, (void*)0) $gl_chk;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->eboInterp) $gl_chk;
    glDrawElements(GL_LINES, numInterpElements(), GL_UNSIGNED_INT, nullptr) $gl_chk;
}

void HairMesh::updateFrom(const ElasticRod& rod, size_t idx)
{
    for (size_t i = 0; i < controlHairLen; i++)
    {
        Eigen::Vector3f x = rod.x[i];
        controlVerts[controlHairLen * idx + i] = glm::vec4(x[0], x[1], x[2], 1.0f);
    }
}

void HairMesh::bindToComputeShader(ComputeShader &cs) const
{
    assert(this->vaoInitialized);
    cs.assocBuffer("ControlHairs", this->vboControl);
    cs.assocBuffer("InterpPoints", this->vboInterp);
    cs.assocBuffer("Tangents", this->vboTangents);
    cs.assocBuffer("InterpIndices", this->eboInterp);
    cs.assocBuffer("TriIndices", this->eboTris);
    cs.setUniform("N", controlHairLen);
    cs.setUniform("M", subdivide);
    cs.setUniform("T", numTris());
    cs.setUniform("D", interpDensity);
    cs.setUniform("H", numControlHairs());
}

void HairMesh::growControlHair(const glm::vec3 &root, const glm::vec3 &dir)
{
    assert(dir.length() > 1e-6f);
    glm::vec3 v = root;
    for (int i = 0; i < controlHairLen; i++) {
        this->controlVerts.push_back({v, 1.0f});
        v += dir * hairGrowth + rng.vec(glm::vec3(-1.0f), glm::vec3(1.0f)) * controlHairRandomize;
    }
}

// --- SurfaceMesh -----------------------------------------------------------

void SurfaceMesh::loadFromFile(const std::string &modelPath, bool compNormals)
{
    spdlog::assrt(fs::exists(modelPath), "model '{}' not found", modelPath);

    // Load the mesh
    cyTriMesh mesh;
    if (!mesh.LoadFromFileObj(modelPath.c_str())) {
        spdlog::error("Failed to load mesh from file: {}", modelPath);
    }
    if (compNormals || mesh.NVN() == 0) {
        mesh.ComputeNormals();
    }

    // normalize the mesh to fit in a 4x4x4 cube
    mesh.ComputeBoundingBox();
    const auto scale =  1.0f / mesh.GetBoundMax().y;
    //go over the vertices and scale them
    for (int i = 0; i < mesh.NV(); i++) {
        mesh.V(i) *= scale;
    }

    // Mapping from vertex index to (normalIdx, texIdx)
    std::vector<std::tuple<int, int>> indices(mesh.NV(), {-1, -1});
    // Remapping (normalIdx, texIdx) -> new vertex index
    std::unordered_map<size_t, size_t> vertRemap({});
    // Map mixed indices to flat indices
    this->indices.resize(mesh.NF() * 3);
    this->vertices.resize(mesh.NV());
    this->normals.resize(mesh.NV());
    this->textureCoords.resize(mesh.NV());
    for (size_t i = 0; i < mesh.NF(); i++) {
        const uint32_t* normalIndices = mesh.FN(i).v;
        const uint32_t* texIndices = mesh.FT(i).v;
        const uint32_t* vertIndices = mesh.F(i).v;
        for (size_t j = 0; j < 3; j++) {
            uint32_t vertIdx = vertIndices[j];
            const uint32_t inNormalIdx = normalIndices[j];
            const uint32_t inTexIdx = texIndices[j];
            const uint32_t combinedAttrIdx = cantor(inNormalIdx, inTexIdx);
            if (vertRemap.count(combinedAttrIdx)) {
                // Vertex has already been remapped, so use the stored remapped vertex index
                vertIdx = vertRemap.at(combinedAttrIdx);
            } else {
                const int storedNormalIdx = std::get<0>(indices[vertIdx]);
                const int storedTexIdx = std::get<1>(indices[vertIdx]);
                if (storedNormalIdx != (int)inNormalIdx || storedTexIdx != (int)inTexIdx) {
                    const uint32_t oldVertIdx = vertIdx;
                    // Vertex either hasn't been stored, or hasn't been remapped
                    if (storedNormalIdx == -1 || storedTexIdx == -1) {
                        // First encounter with this vertex, store it for the first time
                        indices[vertIdx] = {inNormalIdx, inTexIdx};
                    } else {
                        // Stored index set doesn't match, so we dupe and remap
                        vertIdx = vertRemap.emplace(combinedAttrIdx, this->vertices.size()).first->second;
                        this->vertices.push_back({});
                        this->normals.push_back({});
                        this->textureCoords.push_back({});
                    }
                    this->vertices[vertIdx] = glm::make_vec3(&mesh.V(oldVertIdx)[0]);
                    this->normals[vertIdx] = glm::make_vec3(&mesh.VN(inNormalIdx)[0]);
                    this->textureCoords[vertIdx] = glm::make_vec2(&mesh.VT(inTexIdx)[0]);
                }
            }
            this->indices[i*3 + j] = vertIdx;
        }
    }
}

void SurfaceMesh::build(const OpenGLProgram &prog)
{
    spdlog::assrt(!this->vaoInitialized, "SurfaceMesh already built");
    this->vaoInitialized = true;
    glGenVertexArrays(1, &this->vao) $gl_chk;
    glBindVertexArray(vao) $gl_chk;
    this->vboVertices = gl::buffer(GL_ARRAY_BUFFER, this->vertices);
    this->vboNormals = gl::buffer(GL_ARRAY_BUFFER, this->normals);
    this->vboTexCoords = gl::buffer(GL_ARRAY_BUFFER, this->textureCoords);
    this->ebo = gl::buffer(GL_ELEMENT_ARRAY_BUFFER, this->indices);
    
    prog.SetAttribPointer(vboVertices, "vPos", 3, GL_FLOAT);
    prog.SetAttribPointer(vboNormals, "vNormal", 3, GL_FLOAT);
}

void SurfaceMesh::draw(const OpenGLProgram &prog)
{
    glBindVertexArray(this->vao) $gl_chk;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo) $gl_chk;
    glBindBuffer(GL_ARRAY_BUFFER, this->vboVertices) $gl_chk;
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, nullptr) $gl_chk;
}

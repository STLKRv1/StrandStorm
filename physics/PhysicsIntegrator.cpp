#include <PhysicsIntegrator.hpp>

void PhysicsIntegrator::Initialize()
{
    //Initialize the physics integrator here
}

void PhysicsIntegrator::Integrate()
{
    for (int i = 0; i < numSteps; i++)
        TakeStep(dt);
    //Call Event Handler to scynronize the rendering geometry with the physics
}

void PhysicsIntegrator::TakeStep(float dt)
{
    //Integrate the physics here
    printf("Integrating physics with dt = %f numSteps = %d dt*numSteps = %f\r", dt, numSteps, dt*numSteps);
}
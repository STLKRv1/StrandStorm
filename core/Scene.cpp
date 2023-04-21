#include <Scene.hpp>
#include <Renderer.hpp>

Scene::Scene()
{
    cam.pos = {0.0f, 0.0f, 2.0f};
    cam.distance = 3.0f;
}

void Scene::init(const Renderer& r)
{
    hairMesh.loadFromFile("resources/sphere.obj");
    hairMesh.build(r.hairProg);

    surfaceMesh.loadFromFile("resources/sphere.obj");
    surfaceMesh.build(r.surfaceProg);

    //set Marschner luts
    TextureParams lutParams;
    lutParams.wrapS = GL_CLAMP_TO_BORDER;
    lutParams.wrapT = GL_CLAMP_TO_BORDER;
    lutParams.internalFormat = GL_RGBA8;
    lutParams.format = GL_RGBA;
    lutParams.type = GL_FLOAT;
    hairMesh.lut0 = std::make_shared<Texture>("resources/Textures/lut0.png", GL_TEXTURE0, lutParams);
    hairMesh.lut1 = std::make_shared<Texture>("resources/Textures/lut1.png", GL_TEXTURE1, lutParams);
    lutParams.internalFormat = GL_RGB8;
    lutParams.format = GL_RGB;
    hairMesh.lut2 = std::make_shared<Texture>("resources/Textures/lut2.png", GL_TEXTURE2, lutParams);


    //set light's shadow texture
    TextureParams params;
    params.wrapS = GL_CLAMP_TO_BORDER;
    params.wrapT = GL_CLAMP_TO_BORDER;
    params.internalFormat = GL_DEPTH_COMPONENT32;
    params.format = GL_DEPTH_COMPONENT;
    params.type = GL_FLOAT;
    light.shadowTexture = std::make_shared<ShadowTexture>(
        glm::uvec2(1024, 1024), GL_TEXTURE5, 
        params);
    
    //set light's opacity shadow maps
    light.opacityShadowMaps.depthTex = std::make_shared<DepthTexture>(
        glm::uvec2(1024, 1024), GL_TEXTURE6, 
        params);
    
    params.internalFormat = GL_RGBA32F;
    params.format = GL_RGBA;
    light.opacityShadowMaps.opacitiesTex = std::make_shared<RenderedTexture>(
        glm::uvec2(1024, 1024), GL_TEXTURE10, 
        params);

    cam.orient({0.0f, 0.0f});
}

glm::mat4 Scene::Light::CalculateLightSpaceMatrix() const
{
    const glm::mat4 lightProjection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 0.01f, 10.f);
    const glm::mat4 lightView = glm::lookAt(-glm::normalize(dir) * 8.f,
                                            glm::vec3(0.0f, 0.0f, 0.0f),
                                            glm::vec3(0.0f, 0.0f, 1.0f));
    return lightProjection * lightView;
}

glm::mat4 Scene::Light::CalculateLightTexSpaceMatrix() const
{
    const glm::mat4 shadowMatrix = glm::mat4(
        0.5, 0.0, 0.0,   0.0,
        0.0, 0.5, 0.0,   0.0,
        0.0, 0.0, 0.5,   0.0,
        0.5, 0.5, 0.498, 1.0);
    return shadowMatrix * this->CalculateLightSpaceMatrix();
}

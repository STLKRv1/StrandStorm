#pragma once

#include <OpenGLProgram.hpp>
#include <cyTriMesh.h>
#include <Renderer.hpp>
#include <unordered_map>
#include <string>

class ComputeShader;
class ElasticRod;

class Mesh
{
protected:
    bool vaoInitialized = false;
    GLuint vao = GL_INVALID_INDEX;
public:
    Mesh() = default;

    virtual void loadFromFile(const std::string& modelPath, bool compNormals = true) = 0;
    virtual void build(const OpenGLProgram& prog) = 0;
    virtual void draw(const OpenGLProgram& prog) = 0;

    inline bool isVaoInitialized() const { return vaoInitialized; }
};

class HairMesh : public Mesh
{
private:
    // Random number gen for making control hairs
    RNG rng = {0};
    // Triangles for interpolating hairs
    std::vector<GLuint> tris;
    // VBO for control hairs
    GLuint vboControl = GL_INVALID_INDEX;
    // VBO for interpolated hairs
    GLuint vboInterp = GL_INVALID_INDEX;
    // VBO for interpolated hair tangents
    GLuint vboTangents = GL_INVALID_INDEX;
    // EBO for interpolated hairs
    GLuint eboInterp = GL_INVALID_INDEX;
    // EBO for triangles
    GLuint eboTris = GL_INVALID_INDEX;

    // Grow control hair from a root position and direction, adding to my vertices and indices
    void growControlHair(const glm::vec3& root, const glm::vec3& dir);
public:
    // Number of vertices in each control hair (N)
    static constexpr uint32_t controlHairLen = 30;
    // Number of subdivisions between each control hair vertex (M)
    //  Includes start vertex, so needs to be >=1
    static constexpr uint32_t subdivide = 3;
    // Number of additional hairs to interpolate across each face
    static constexpr uint32_t interpDensity = 64;
    // Hair growth amount per control vert
    static constexpr float hairGrowth = 0.05f;
    // Amount to randomly perturb control hair vertices
    static constexpr float controlHairRandomize = 0.015f;
    ///DEBUG: control hair duplication
    static constexpr uint32_t controlHairDensity = 0;
    ///DEBUG: max number of control hairs, -1 for unlimited
    static constexpr int maxControlHairs = 50;

    // Vertices for control hairs
    std::vector<glm::vec4> controlVerts;

    bool drawControlHairs = false;

    HairMesh() = default;

    void build(const OpenGLProgram& prog) override;
    void updateBuffer();
    void loadFromFile(const std::string &modelPath, bool compNormals = true) override;
    void draw(const OpenGLProgram& prog) override;
    void updateFrom(const ElasticRod& rod, size_t idx);

    void bindToComputeShader(ComputeShader& cs) const;

    // Returns number of control hairs (H)
    inline size_t numControlHairs() const {
        return controlVerts.size() / controlHairLen;
    }
    // Returns total number of triangles
    inline size_t numTris() const {
        return tris.size() / 3;
    }
    // Returns total number of interpolated hairs
    inline size_t numInterpHairs() const {
        return interpDensity * numTris() + numControlHairs();
    }
    // Number of elements in the interpolated hair buffer
    inline size_t numInterpElements() const {
        return 2 * subdivide * (controlHairLen - 1) * numInterpHairs();
    }
    // Number of interpolated vertices (elements in vboInterp)
    inline size_t numInterpVertices() const {
        return (subdivide * (controlHairLen - 1) + 1) * numInterpHairs();
    }
    // Number of bezier control points
    inline size_t numControlPoints() const {
        return (3 * controlHairLen - 2) * numControlHairs();
    }

    glm::vec4 color = glm::vec4(0.57f, 0.48f, 0.0f, 1.0f);
    glm::vec3 ambient = glm::vec3(0.1f);
    glm::vec3 specular = glm::vec3(0.50f);
    float shininess = 32.0f;

    std::shared_ptr<Texture> lut0;
    std::shared_ptr<Texture> lut1;
    std::shared_ptr<Texture> lut2;

    int shadingModel = 1;

    // Marschner LUT parameters
    float diffuseFalloff = 1.0f;
    float diffuseAzimuthFalloff = 0.4f;
    float scaleDiffuse = 1.0f; 
    float scaleR = 1.0f;
    float scaleTT = 1.0f;
    float scaleTRT = 1.0f;

    // Marschner procedural parameters
    float roughness = 20.0f;
    float shift = -5.0f;
    float refractiveIndex = 1.55f;
    float procScaleR = 30.0f;
    float procScaleTT = 10.0f;
    float procScaleTRT = 20.0f;

    bool show = true;
    bool shadowsEnable = true;
};

class SurfaceMesh : public Mesh
{
private:
    GLuint vboNormals = GL_INVALID_INDEX;
    GLuint vboTexCoords = GL_INVALID_INDEX;
    GLuint vboVertices = GL_INVALID_INDEX;
    GLuint ebo = GL_INVALID_INDEX;
    std::vector<glm::vec3> vertices;
    std::vector<GLuint> indices;
public:
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> textureCoords;
    struct Material {
        glm::vec3 ambient = glm::vec3(0.1f, 0.1f, 0.1f);
        glm::vec3 diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
        glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
        float shininess = 32.0f;
    } material;

    

    SurfaceMesh() = default;

    // Loads mesh from file
    void loadFromFile(const std::string& modelPath, bool compNormals = true) override;
    // Builds the buffers
    void build(const OpenGLProgram& prog) override;
    // Draws the hair mesh
    void draw(const OpenGLProgram& prog) override;
};

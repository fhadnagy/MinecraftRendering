#include <vector>
#include "GLUtils.hpp"
#define TW 0.0625

enum class FaceDirection {
    Front,
    Back,
    Left,
    Right,
    Top,
    Bottom
};

MeshObject<Vertex> createCubeFace(FaceDirection face, glm::vec3 relativePoint, uint8_t textureCoord);


class Chunk {
public:
    Chunk(int width, int height);

    void GenerateMeshes();

    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    int GetDepth() const { return depth; }

private:
    int width, height, depth;

    // Optional: 3D voxel data
    std::vector<uint8_t> blocks;

    // You can store mesh data here
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    void AddFace(FaceDirection face, glm::vec3 center, uint8_t textureCoord);
};

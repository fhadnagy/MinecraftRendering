#pragma once

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

class ChunkManager;

class Chunk {
public:
    Chunk();
    Chunk(int width, int height, ChunkManager* manager, int startX, int startY);
    ~Chunk();
    
    void GenerateMeshes();

    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    bool SetBlock(int x, int y, int z, uint8_t value);
    bool InChunk(int x, int y, int z);
    bool IsAir(int x, int y, int z);
    bool NeedsRender() const { return needsRender; }
    void MarkDirty() { needsRender = true; }
    void UpadteOGLObject();
    const OGLObject GetOGLObject() const { return m_mesh; }

private:
    int width, height;
    int startX, startY;
    uint8_t* blocks;
    ChunkManager* manager;
    bool needsRender = true;
    // You can store mesh data here
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    OGLObject m_mesh;
    void AddFace(FaceDirection face, glm::vec3 center, uint8_t textureCoord);
};

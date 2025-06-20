#pragma once

#include <vector>
#include "GLUtils.hpp"
#include "PerlinNoise.h"

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
    Chunk(int width, int height, std::shared_ptr<ChunkManager> manager, int startX, int startZ);
    ~Chunk();
    
    void GenerateMeshes();

    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    bool SetBlock(int x, int y, int z, uint8_t value);
    bool InChunk(int x, int y, int z);
    bool IsAir(int x, int y, int z);
    int BlockAt(int x, int y, int z);
    bool NeedsRender() const { return needsRender; }
    void MarkDirty() { needsRender = true; }
    void UpadteOGLObject();
    const OGLObject GetOGLObject() const { return m_mesh; }
    void Print();
    int Index(int x, int y, int z);
    static uint8_t const AIR = 100;
private:
    int width, height;
    int startX, startZ;
    uint8_t* blocks;
    std::weak_ptr<ChunkManager> manager;
    bool needsRender = true;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    OGLObject m_mesh;
    void AddFace(FaceDirection face, glm::vec3 center, uint8_t textureCoord);
};

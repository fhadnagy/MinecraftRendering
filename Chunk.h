#pragma once

#include <vector>
#include "GLUtils.hpp"
#include "ChunkManager.h"
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
    Chunk(int width, int height, ChunkManager* manager, int startX, int startY);
    ~Chunk();

    void GenerateMeshes();

    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    bool SetBlock(int x, int y, int z, uint8_t value);
    bool InChunk(int x, int y, int z);
    bool IsAir(int x, int y, int z);
    OGLObject GetOGLObject();

private:
    int width, height;
    int startX, startY;

    uint8_t* blocks;
    ChunkManager* manager;

    // You can store mesh data here
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    void AddFace(FaceDirection face, glm::vec3 center, uint8_t textureCoord);
};

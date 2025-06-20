#pragma once


#include <vector>
#include "GLUtils.hpp"
#include <unordered_map>
#include <utility>
#include <cmath>
#include "Chunk.h"

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        return h1 ^ (h2 << 1); // Combine hashes
    }
};



class ChunkManager {
public:
    ChunkManager(int chunkWidth, int chunkHeight, int seed);
    ~ChunkManager();

    bool IsAir(int x, int y, int z);
    bool SetBlock(int x, int y, int z, int value);
    void PrintAll();
    void GenerateOGLObjects();
    int ChunkXZ(int xz);
    int LocalXZ(int xz);
    bool ExistsChunk(int x, int z);
    void CalculateRayTrace(glm::vec3 eye, glm::vec3 direction);
    glm::ivec3 ContainingBlock(glm::vec3 pos);
    void SetBlockAtPlace(int value);

    // Optionally expose for rendering
    const std::unordered_map<std::pair<int, int>, OGLObject, pair_hash>& GetDrawingData() const {
        return drawingData;
    }
    const int GetChunkSize() const {
        return m_chunkWidth;
    }

    glm::vec3 highlightPosition = glm::vec3(-1,-1,-1);
    glm::vec3 placePosition = glm::vec3(-1, -1, -1);
    PerlinNoise perlin;

private:
    int m_chunkWidth;
    int m_chunkHeight;
    int seed;
    std::unordered_map<std::pair<int, int>, std::unique_ptr<Chunk>, pair_hash> chunks;
    std::unordered_map<std::pair<int, int>, OGLObject, pair_hash> drawingData;
    
    Chunk& GetOrCreateChunk(int chunkX, int chunkZ);
};


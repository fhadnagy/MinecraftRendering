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
    ChunkManager(int chunkWidth, int chunkHeight);

    bool IsAir(int x, int y, int z);
    bool SetBlock(int x, int y, int z, int value);

    void GenerateOGLObjects();

    // Optionally expose for rendering
    const std::unordered_map<std::pair<int, int>, OGLObject, pair_hash>& GetDrawingData() const {
        return drawingData;
    }
    const int GetChunkSize() const {
        return m_chunkWidth;
    }

private:
    int m_chunkWidth;
    int m_chunkHeight;

    std::unordered_map<std::pair<int, int>, Chunk, pair_hash> chunks;
    std::unordered_map<std::pair<int, int>, OGLObject, pair_hash> drawingData;
    
    Chunk& GetOrCreateChunk(int chunkX, int chunkZ);
};


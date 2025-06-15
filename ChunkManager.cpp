#include "ChunkManager.h"

ChunkManager::ChunkManager(int chunkWidth, int chunkHeight)
    : m_chunkWidth(chunkWidth), m_chunkHeight(chunkHeight) {
}

Chunk& ChunkManager::GetOrCreateChunk(int chunkX, int chunkZ) {
    std::pair<int, int> key = { chunkX, chunkZ };

    auto it = chunks.find(key);
    if (it != chunks.end()) {
        return it->second;
    }

    // Create and insert new Chunk
    Chunk newChunk(m_chunkWidth, m_chunkHeight, this, key.first*m_chunkWidth, key.second*m_chunkWidth);
    chunks[key] = newChunk;
    return chunks[key];
}

void ChunkManager::GenerateOGLObjects() {
    drawingData.clear();

    for (auto& [coord, chunk] : chunks) {
        chunk.GenerateMeshes();
        chunk.UpadteOGLObject();
        drawingData[coord] = chunk.GetOGLObject();  // This assumes chunk builds and returns one
    }
}


bool ChunkManager::IsAir(int x, int y, int z)
{
    if (y<0 || y>= m_chunkHeight) {
        return true;
    }
    int chunkX = x / m_chunkWidth;
    int chunkZ = z / m_chunkWidth;

    std::pair<int, int> key = { chunkX, chunkZ };

    auto it = chunks.find(key);
    if (it != chunks.end()) {
        int localX = x % m_chunkWidth;
        int localZ = z % m_chunkWidth;
        if (localX < 0) localX += m_chunkWidth;
        if (localZ < 0) localZ += m_chunkWidth;
        return it->second.IsAir(localX,y,localZ);
    }

    return false;
}

bool ChunkManager::SetBlock(int x, int y, int z, int value) {
    if (y < 0 || y >= m_chunkHeight) {
        return true;
    }
    int chunkX = x / m_chunkWidth;
    int chunkZ = z / m_chunkWidth;

    int localX = x % m_chunkWidth;
    int localZ = z % m_chunkWidth;

    if (localX < 0) localX += m_chunkWidth;
    if (localZ < 0) localZ += m_chunkWidth;

    Chunk& chunk = GetOrCreateChunk(chunkX, chunkZ);
    chunk.SetBlock(localX, y, localZ, value);
    return true;
}

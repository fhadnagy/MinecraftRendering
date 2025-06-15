#include "ChunkManager.h"

ChunkManager::ChunkManager(int chunkWidth, int chunkHeight)
    : m_chunkWidth(chunkWidth), m_chunkHeight(chunkHeight) {
}

Chunk& ChunkManager::GetOrCreateChunk(int chunkX, int chunkY) {
    std::pair<int, int> key = { chunkX, chunkY };

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
    if (z<0 || z>= m_chunkHeight) {
        return true;
    }
    int chunkX = x / m_chunkWidth;
    int chunkY = y / m_chunkHeight;

    std::pair<int, int> key = { chunkX, chunkY };

    auto it = chunks.find(key);
    if (it != chunks.end()) {
        int localX = x % m_chunkWidth;
        int localY = y % m_chunkHeight;
        return it->second.IsAir(localX,localY,z);
    }

    return false;
}

bool ChunkManager::SetBlock(int x, int y, int z, int value) {
    int chunkX = x / m_chunkWidth;
    int chunkY = y / m_chunkHeight;

    int localX = x % m_chunkWidth;
    int localY = y % m_chunkHeight;

    if (localX < 0) localX += m_chunkWidth;
    if (localY < 0) localY += m_chunkHeight;

    Chunk& chunk = GetOrCreateChunk(chunkX, chunkY);
    chunk.SetBlock(localX, localY, z, value);
    return true;
}

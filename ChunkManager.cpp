#include "ChunkManager.h"

ChunkManager::ChunkManager(int chunkWidth, int chunkHeight)
    : m_chunkWidth(chunkWidth), m_chunkHeight(chunkHeight) {
}

ChunkManager::~ChunkManager()
{
    for (auto& [coord, oglo] : drawingData) {
        CleanOGLObject(oglo);
    }
}

Chunk& ChunkManager::GetOrCreateChunk(int chunkX, int chunkZ) {
    auto key = std::make_pair(chunkX, chunkZ);
    auto it = chunks.find(key);
    if (it != chunks.end()) {
        return *(it->second);
    }

    // Create and insert new Chunk
    auto newChunk = std::make_unique<Chunk>(m_chunkWidth, m_chunkHeight, this, chunkX * m_chunkWidth, chunkZ * m_chunkWidth);
    Chunk& ref = *newChunk;

    chunks[key] = std::move(newChunk);
    //printf("New chunk(%d %d)\n", key.first, key.second);
    return ref;
}

void ChunkManager::GenerateOGLObjects() {
    //drawingData.clear();

    for (auto& [coord, chunk] : chunks) {
        if (!chunk) continue;

        if (chunk->NeedsRender()) {
            chunk->GenerateMeshes();
            chunk->UpadteOGLObject();
            drawingData[coord] = chunk->GetOGLObject();  // This assumes chunk builds and returns one
        }
    }

}

int ChunkManager::ChunkXZ(int xz)
{
    // ex: chunkwidth = 3 
    // %3       -2 -1  0 -2 -1  0  1  2  0  1  2
    // chunkxz: -2 -2 -1 -1 -1  0  0  0  1  1  1
    if (xz >= 0)
    {
        return xz / m_chunkWidth;
    }
    else {
        return xz / m_chunkWidth - ((xz % m_chunkWidth == 0) ? 0 : 1);
    }
}

int ChunkManager::LocalXZ(int xz)
{
   if (xz >= 0)
    {
        return xz % m_chunkWidth;
    }
    else {
        return (xz % m_chunkWidth + m_chunkWidth) % m_chunkWidth;
    }
}

bool ChunkManager::ExistsChunk(int x, int z)
{
    return chunks.find(std::make_pair(x, z)) != chunks.end();
}


bool ChunkManager::IsAir(int x, int y, int z)
{
    //printf("ManagerIsAir %d %d %d\n",x,y,z);
    if (y<0 || y>= m_chunkHeight) {
        //printf("y outofbound\n");
        return true;
    }
    
   
    std::pair<int, int> key = { ChunkXZ(x), ChunkXZ(z) };

    auto it = chunks.find(key);
    if (it != chunks.end()) {
        
        return it->second->IsAir(LocalXZ(x), y, LocalXZ(z));
    }
    else {
        return true;
    }

}

bool ChunkManager::SetBlock(int x, int y, int z, int value) {
    if (y < 0 || y >= m_chunkHeight) {
        return false;
    }

    int chunkX = ChunkXZ(x);
    int chunkZ = ChunkXZ(z);
    int localX = LocalXZ(x);
    int localZ = LocalXZ(z);

    Chunk& chunk = GetOrCreateChunk(chunkX, chunkZ);

    if (chunk.SetBlock(localX, y, localZ, value)) {
        chunk.MarkDirty();

        // Dirty neighbors if at border
    
        if (localX == 0 && ExistsChunk(chunkX - 1,chunkZ)) {
            GetOrCreateChunk(chunkX - 1, chunkZ).MarkDirty();
        }
        else if (localX == m_chunkWidth - 1 && ExistsChunk(chunkX + 1, chunkZ))
        {
            GetOrCreateChunk(chunkX + 1, chunkZ).MarkDirty();
        }

        if (localZ == 0 && ExistsChunk(chunkX, chunkZ - 1)) {
            GetOrCreateChunk(chunkX, chunkZ - 1).MarkDirty();
        }
        else if (localZ == m_chunkWidth - 1 && ExistsChunk(chunkX, chunkZ + 1))
        {
            GetOrCreateChunk(chunkX, chunkZ + 1).MarkDirty();
        }

        
    }

    return false;
}


void ChunkManager::PrintAll()
{
    printf("Printing \n");
    for (auto& [coord, chunk] : chunks) {
        int chunkX = coord.first * m_chunkWidth;
        int chunkZ = coord.second * m_chunkWidth;

        printf("x: %d , z: %d", chunkX, chunkZ);
        chunk->Print();
    }
}

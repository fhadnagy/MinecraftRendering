#include "ChunkManager.h"

ChunkManager::ChunkManager(int chunkWidth, int chunkHeight, int seed)
    : m_chunkWidth(chunkWidth), m_chunkHeight(chunkHeight) {
    perlin = PerlinNoise(seed);
}

ChunkManager::~ChunkManager()
{
    /*for (auto& [coord, oglo] : drawingData) {
        CleanOGLObject(oglo);
    }*/
}

Chunk& ChunkManager::GetOrCreateChunk(int chunkX, int chunkZ) {
    auto key = std::make_pair(chunkX, chunkZ);
    auto it = chunks.find(key);
    if (it != chunks.end()) {
        return *(it->second);
    }

    std::shared_ptr<ChunkManager> self = shared_from_this(); // requires inheriting std::enable_shared_from_this

    auto newChunk = std::make_unique<Chunk>(m_chunkWidth, m_chunkHeight, self, chunkX * m_chunkWidth, chunkZ * m_chunkWidth);
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

void ChunkManager::CalculateRayTrace(glm::vec3 eye, glm::vec3 direction)
{
    const float maxDistance = 25.0f;
    const float maxDistanceSq = maxDistance * maxDistance;

    glm::ivec3 currentBlock = ContainingBlock(eye);

    glm::ivec3 step = glm::ivec3(
        direction.x > 0 ? 1 : (direction.x < 0 ? -1 : 0),
        direction.y > 0 ? 1 : (direction.y < 0 ? -1 : 0),
        direction.z > 0 ? 1 : (direction.z < 0 ? -1 : 0)
    );

    glm::vec3 invDir = 1.0f / direction;

    glm::vec3 blockBoundary = glm::vec3(currentBlock) + glm::vec3(
        step.x > 0 ? 1.0f : 0.0f,
        step.y > 0 ? 1.0f : 0.0f,
        step.z > 0 ? 1.0f : 0.0f
    );


    glm::vec3 tMax = (blockBoundary - eye) * invDir;
    glm::vec3 tDelta = glm::abs(invDir);

    glm::ivec3 lastBlock = currentBlock;

    highlightPosition = glm::vec3(-1);
    placePosition = glm::vec3(-1);

    const int maxSteps = 512;

    for (int i = 0; i < maxSteps; ++i)
    {
        glm::vec3 hitPos = eye + direction * glm::min(tMax.x, glm::min(tMax.y, tMax.z));
        float distSq = glm::distance(eye, hitPos)* glm::distance(eye, hitPos);

        if (distSq > maxDistanceSq)
            break;

        if (currentBlock.y>=m_chunkHeight || currentBlock.y<0) {
            highlightPosition = glm::vec3(currentBlock);
            placePosition = glm::vec3(lastBlock);
            return;
        }

        if (!IsAir(currentBlock.x, currentBlock.y, currentBlock.z))
        {
            highlightPosition = glm::vec3(currentBlock);
            placePosition = glm::vec3(lastBlock);
            return;
        }

        lastBlock = currentBlock;
        //printf("Current: %d, %d, %d\n", lastBlock.x, lastBlock.y, lastBlock.z);

        // Advance to next voxel boundary
        if (tMax.x < tMax.y)
        {
            if (tMax.x < tMax.z)
            {
                currentBlock.x += step.x;
                tMax.x += tDelta.x;
            }
            else
            {
                currentBlock.z += step.z;
                tMax.z += tDelta.z;
            }
        }
        else
        {
            if (tMax.y < tMax.z)
            {
                currentBlock.y += step.y;
                tMax.y += tDelta.y;
            }
            else
            {
                currentBlock.z += step.z;
                tMax.z += tDelta.z;
            }
        }
    }

    // Nothing hit within Euclidean range
    highlightPosition = glm::vec3(-2);
    placePosition = glm::vec3(-1);
}

int ChunkManager::GetSelection()
{
    glm::ivec3 block = highlightPosition;
    return BlockAt(block);
}

int ChunkManager::BlockAt(glm::ivec3 pos)
{
    //printf("ManagerIsAir %d %d %d\n",x,y,z);
    if (pos.y < 0 || pos.y >= m_chunkHeight) {
        //printf("y outofbound\n");
        return Chunk::AIR;
    }


    std::pair<int, int> key = { ChunkXZ(pos.x), ChunkXZ(pos.z) };

    auto it = chunks.find(key);
    if (it != chunks.end()) {

        return it->second->BlockAt(LocalXZ(pos.x), pos.y, LocalXZ(pos.z));
    }
    else {
        return Chunk::AIR;
    }

}


glm::ivec3 ChunkManager::ContainingBlock(glm::vec3 pos)
{
    return glm::ivec3(
        static_cast<int>(std::floor(pos.x)),
        static_cast<int>(std::floor(pos.y)),
        static_cast<int>(std::floor(pos.z))
    );
}

void ChunkManager::SetBlockAtPlace(int value)
{
    if (value == Chunk::AIR) {
        if (highlightPosition.y > -1.5) {
            glm::ivec3 block = ContainingBlock(highlightPosition + glm::vec3(0.01));
            SetBlock(block.x, block.y, block.z, value);
        }
        printf("Delete\n");
    }
    else {
        if (placePosition.y > -.5) {
            glm::ivec3 block = ContainingBlock(placePosition + glm::vec3(0.01));
            SetBlock(block.x, block.y, block.z, value);
        }
    }
   

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

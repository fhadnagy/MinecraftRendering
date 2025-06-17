#include "ChunkManager.h"
#include <iostream>

MeshObject<Vertex> createCubeFace(FaceDirection face, glm::vec3 relativePoint, uint8_t textureCoord) {
	MeshObject<Vertex> mesh;
	glm::vec3 normal;
	glm::vec3 offsets[4];
	glm::vec3 center = relativePoint + glm::vec3(0.5, 0.5, 0.5);
	// Texture UV top-left
	float u = (textureCoord % 16) * TW;
	float v = (textureCoord / 16) * TW;

	glm::vec2 uv0 = { u, v };
	glm::vec2 uv1 = { u + TW, v };
	glm::vec2 uv2 = { u + TW, v + TW };
	glm::vec2 uv3 = { u, v + TW };

	float h = 1.0 / 2.0f;

	switch (face) {
	case FaceDirection::Front:
		normal = { 0, 0, 1 };
		offsets[0] = { -h, -h, h };
		offsets[1] = { h, -h, h };
		offsets[2] = { h,  h, h };
		offsets[3] = { -h,  h, h };
		break;
	case FaceDirection::Back:
		normal = { 0, 0, -1 };
		offsets[0] = { h, -h, -h };
		offsets[1] = { -h, -h, -h };
		offsets[2] = { -h,  h, -h };
		offsets[3] = { h,  h, -h };
		break;
	case FaceDirection::Left:
		normal = { -1, 0, 0 };
		offsets[0] = { -h, -h, -h };
		offsets[1] = { -h, -h,  h };
		offsets[2] = { -h,  h,  h };
		offsets[3] = { -h,  h, -h };
		break;
	case FaceDirection::Right:
		normal = { 1, 0, 0 };
		offsets[0] = { h, -h,  h };
		offsets[1] = { h, -h, -h };
		offsets[2] = { h,  h, -h };
		offsets[3] = { h,  h,  h };
		break;
	case FaceDirection::Top:
		normal = { 0, 1, 0 };
		offsets[0] = { -h, h,  h };
		offsets[1] = { h, h,  h };
		offsets[2] = { h, h, -h };
		offsets[3] = { -h, h, -h };
		break;
	case FaceDirection::Bottom:
		normal = { 0, -1, 0 };
		offsets[0] = { -h, -h, -h };
		offsets[1] = { h, -h, -h };
		offsets[2] = { h, -h,  h };
		offsets[3] = { -h, -h,  h };
		break;
	}

	mesh.vertexArray = {
		{center + offsets[0], normal, uv0},
		{center + offsets[1], normal, uv1},
		{center + offsets[2], normal, uv2},
		{center + offsets[3], normal, uv3},
	};

	mesh.indexArray = { 0, 1, 2, 2, 3, 0 };

	return mesh;
}



glm::ivec3 GetFaceOffset(FaceDirection face) {
	switch (face) {
	case FaceDirection::Front: return { 0, 0, 1 };
	case FaceDirection::Back: return { 0, 0, -1 };
	case FaceDirection::Left: return { -1, 0, 0 };
	case FaceDirection::Right: return { 1, 0, 0 };
	case FaceDirection::Top: return { 0, 1, 0 };
	case FaceDirection::Bottom: return { 0, -1, 0 };
	}
	return { 0, 0, 0 }; // Default/fallback
}

Chunk::Chunk(int sizeXZ, int height, ChunkManager* manager, int startX, int startZ)
	: width(sizeXZ), height(height), startX(startX), startZ(startZ), manager(manager) {
	blocks = new uint8_t[sizeXZ * sizeXZ * height]; 
	// Empty by default
	for (int y = 0; y < height; ++y) {
		for (int z = 0; z < width; ++z) {
			for (int x = 0; x < width; ++x) {
				blocks[Index(x, y, z)] = 0;
			}
		}
	}
}

Chunk::Chunk() : width(1), height(1), startX(0), startZ(0), manager(nullptr) {
	blocks = new uint8_t[1];
	blocks[0] = 1;
}

Chunk::~Chunk()
{
	delete blocks;
}

void Chunk::GenerateMeshes() {
	if (!needsRender)
		return;

    vertices.clear();
    indices.clear();

    for (int y = 0; y < height; ++y) {
        for (int z = 0; z < width; ++z) {
            for (int x = 0; x < width; ++x) {
                uint8_t block = blocks[Index(x, y, z)];
                if (block == 0) continue; // Air

                glm::vec3 center = glm::vec3(x, y, z);

                // For now, always show all faces
                for (int f = 0; f < 6; ++f) {
					glm::ivec3 offset = GetFaceOffset(static_cast<FaceDirection>(f));
					if (IsAir(startX + x + offset.x, y + offset.y, startZ + z + offset.z)) {
						AddFace(static_cast<FaceDirection>(f), center, block);
					}
                }
            }
        }
    }

	needsRender = false;
}

bool Chunk::SetBlock(int x, int y, int z, uint8_t value)
{
	if (!InChunk(x,y,z)){
		return false; // Out of bounds
	}

	if (blocks[Index(x, y, z)] != value) {
		blocks[Index(x, y, z)] = value;
		needsRender = true;
		return true;
	}
	
	return false;
}

bool Chunk::InChunk(int x, int y, int z)
{
	if (x < 0 || x >= width ||
		y < 0 || y >= height ||
		z < 0 || z >= width)
	{
		return false; // Out of bounds
	}
	return true;
}

bool Chunk::IsAir(int x, int y, int z)
{
	if (!InChunk(x,y,z)) {
		if (manager == NULL) {
			return true;
		}
		else{
			manager->IsAir(x, y, z);
		}
	}

	return blocks[Index(x,y,z)] == 0;
}

void Chunk::UpadteOGLObject()
{
	GenerateMeshes();
	const std::initializer_list<VertexAttributeDescriptor> vertexAttribList =
	{
		{ 0, offsetof(Vertex, position), 3, GL_FLOAT },
		{ 1, offsetof(Vertex, normal),	 3, GL_FLOAT },
		{ 2, offsetof(Vertex, texcoord), 2, GL_FLOAT },
	};
	MeshObject<Vertex> mesh;
	mesh.indexArray = indices;
	mesh.vertexArray = vertices;
	CleanOGLObject(m_mesh);
	m_mesh = CreateGLObjectFromMesh(mesh, vertexAttribList);
}

void Chunk::Print()
{
	for (size_t y = 0; y < height; y++)
	{
		printf("- - - - - -\n");
		for (size_t z = 0; z < width; z++)
		{
			for (size_t x = 0; x < width; x++)
			{
				printf("%d ", blocks[Index(x,y,z)]);
			}printf("\n");
		}
	}
}

int Chunk::Index(int x, int y, int z)
{
	return x + y * width + z * width * height;
}



void Chunk::AddFace(FaceDirection face, glm::vec3 center, uint8_t textureCoord) {
    std::vector<Vertex> faceVerts = createCubeFace(face, center, textureCoord).vertexArray;

    uint32_t startIndex = vertices.size();
    vertices.insert(vertices.end(), faceVerts.begin(), faceVerts.end());

    // Add two triangles (0,1,2 and 0,2,3) for the quad
    indices.push_back(startIndex + 0);
    indices.push_back(startIndex + 1);
    indices.push_back(startIndex + 2);
    indices.push_back(startIndex + 0);
    indices.push_back(startIndex + 2);
    indices.push_back(startIndex + 3);
}

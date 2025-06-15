#include "Chunk.h"
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


Chunk::Chunk(int sizeXZ, int height, ChunkManager* manager, int startX, int startY)
	: width(sizeXZ), height(height), startX(startX), startY(startY), manager(manager) {
	blocks = new uint8_t[sizeXZ * sizeXZ * height]; 
	// Empty by default
	for (int y = 0; y < height; ++y) {
		for (int z = 0; z < width; ++z) {
			for (int x = 0; x < width; ++x) {
				int index = x + y * width + z * width * height;
				blocks[index] = 0;
			}
		}
	}
}

Chunk::~Chunk()
{
	delete blocks;
}

void Chunk::GenerateMeshes() {
    vertices.clear();
    indices.clear();

    for (int y = 0; y < height; ++y) {
        for (int z = 0; z < width; ++z) {
            for (int x = 0; x < width; ++x) {
                int index = x + y * width + z * width * height;
                uint8_t block = blocks[index];
                if (block == 0) continue; // Air

                glm::vec3 center = glm::vec3(x, y, z);

                // For now, always show all faces
                for (int f = 0; f < 6; ++f) {
                    AddFace(static_cast<FaceDirection>(f), center, block);
                }
            }
        }
    }
}

bool Chunk::SetBlock(int x, int y, int z, uint8_t value)
{
	if (!InChunk(x,y,z)){
		return false; // Out of bounds
	}

	int index = x + y * width + z * width * height;
	blocks[index] = value;
	return true;
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
	if (!InChunk) {
		if (manager == NULL) {
			return true;
		}
		else{
			manager->IsAir(x, y, z);
		}
	}

	int index = x + y * width + z * width * height;
	return blocks[index] == 0;
}

OGLObject Chunk::GetOGLObject()
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
	return CreateGLObjectFromMesh(mesh, vertexAttribList);
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

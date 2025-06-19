#include "MyApp.h"
#include "SDL_GLDebugMessageCallback.h"
#include "GLUtils.hpp"
#include "ObjParser.h"
#include "ProgramBuilder.h"

#include <imgui.h>
#include <iostream>

CMyApp::CMyApp()
{
	chunk = new Chunk(2, 4,nullptr, 0,0);
	m_manager = new ChunkManager(16, 36);
	m_manager->SetBlock(2, 2, 2, 1);
	/*m_manager->PrintAll();
	m_manager->PrintAll();*/
	//m_manager->CalculateRayTrace(glm::vec3(0,10,0),glm::vec3(1,-1,1));

}

CMyApp::~CMyApp()
{
	delete m_manager;
	delete chunk;
}

void CMyApp::InitGeometry()
{
	chunk->UpadteOGLObject();
	m_chunk = chunk->GetOGLObject();
}


void CMyApp::Update(const SUpdateInfo& updateInfo)
{
	m_fpsCamera.Update(updateInfo.DeltaTimeInSec);
    m_ElapsedTimeInSec = updateInfo.ElapsedTimeInSec;
	m_manager->CalculateRayTrace(m_camera.GetEye(), glm::normalize(m_camera.GetAt() - m_camera.GetEye()));
}

void CMyApp::DrawObject(OGLObject& obj, const glm::mat4& world) {
	glUniformMatrix4fv(ul("world"), 1, GL_FALSE, glm::value_ptr(world));
	glUniformMatrix4fv(ul("worldInvTransp"), 1, GL_FALSE, glm::value_ptr(glm::transpose(glm::inverse(world))));
	glBindVertexArray(obj.vaoID);
	glDrawElements(GL_TRIANGLES, obj.count, GL_UNSIGNED_INT, nullptr);
}

void CMyApp::RenderGeometry()
{
	glUseProgram(m_programID);
	glUniform1i( ul("textureImage"),0);
	glBindTextureUnit( 0, m_textureAtlasID );
	glBindSampler( 0, m_SamplerID );
	glUniformMatrix4fv(ul("viewProj"), 1, GL_FALSE, glm::value_ptr(m_camera.GetViewProj()));

	
	for (const auto& [coord, oglObj] : m_manager->GetDrawingData()) {
		int chunkX = coord.first*m_manager->GetChunkSize();
		int chunkZ = coord.second * m_manager->GetChunkSize();

		OGLObject mesh = oglObj;
		glm::mat4 world = glm::translate(glm::vec3(chunkX, 0.0, chunkZ));
		DrawObject(mesh,world);
	}
	if (m_manager->highlightPosition.y > -1.5) {
		glm::vec3 direction = glm::normalize(m_camera.GetEye() - m_camera.GetAt());
		glm::ivec3 step = glm::ivec3(
			direction.x > 0 ? 1 : (direction.x < 0 ? -1 : 0),
			direction.y > 0 ? 1 : (direction.y < 0 ? -1 : 0),
			direction.z > 0 ? 1 : (direction.z < 0 ? -1 : 0)
		);
		DrawBlockHighlight(m_manager->highlightPosition+0.01f*direction, glm::vec3(1, 1, 0));
	}
	if (m_manager->placePosition.y > -.5) {
		DrawBlockHighlight(m_manager->placePosition, glm::vec3(1, 0, 0));
	}
	DrawAxes();
}


void CMyApp::RenderGUI()
{
	// ImGui DemoWindow
	//ImGui::ShowDemoWindow();

	//ImGui::SetNextWindowSize(ImVec2(455, 60), ImGuiCond_FirstUseEver);
	static float pos[3] = { 0.0f, 0.0f, 0.0f };
	static int textureIndex = 0;

	ImGui::InputFloat3("Position", pos);
	ImGui::SliderInt("Texture Index", &textureIndex, 0, 255);

	if (ImGui::Button("Place Block")) {
		printf("clicked\n");
		/*chunk->SetBlock((int)pos[0], (int)pos[1], (int)pos[2], (uint8_t)textureIndex);
		chunk->Print();
		chunk->UpadteOGLObject();
		m_chunk = chunk->GetOGLObject();*/
		m_manager->SetBlock((int)pos[0], (int)pos[1], (int)pos[2], (uint8_t)textureIndex);
		m_manager->GenerateOGLObjects();
	}

}


bool CMyApp::Init()
{
	SetupDebugCallback();

	// Set a bluish clear color
	// glClear() will use this for clearing the color buffer.
	glClearColor(0.125f, 0.25f, 0.5f, 1.0f);

	InitShaders();
	InitGeometry();
	InitTextures();
	InitFrameBufferObject();

	// Other
	glEnable(GL_CULL_FACE);	 // Enable discarding the back-facing faces.
	glCullFace(GL_BACK);     // GL_BACK: facets facing away from camera, GL_FRONT: facets facing towards the camera
	glEnable(GL_DEPTH_TEST); // Enable depth testing. (for overlapping geometry)

	// Camera
	m_camera.SetView(
		glm::vec3(0, 5, 25),// From where we look at the scene - eye
		glm::vec3(0, 0, 0),	// Which point of the scene we are looking at - at
		glm::vec3(0, 1, 0)	// Upwards direction - up
	);
	m_fpsCamera.SetCamera(&m_camera);

	return true;
}
#include "MyApp.h"
#include "SDL_GLDebugMessageCallback.h"
#include "GLUtils.hpp"
#include "ObjParser.h"
#include "ProgramBuilder.h"

#include <imgui.h>
#include <iostream>

CMyApp::CMyApp()
{
	chunk = new Chunk(10, 20);
}

CMyApp::~CMyApp()
{
	delete chunk;
}


void CMyApp::SetupDebugCallback()
{
	// Enable and set the debug callback function if we are in debug context
	GLint context_flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);
	if (context_flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
		glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, GL_DONT_CARE, 0, nullptr, GL_FALSE);
		glDebugMessageCallback(SDL_GLDebugMessageCallback, nullptr);
	}
}

void CMyApp::InitShaders()
{
	m_programID = glCreateProgram();
	ProgramBuilder{ m_programID }
		.ShaderStage(GL_VERTEX_SHADER, "Shaders/Vert_PosNormTex.vert")
		.ShaderStage(GL_FRAGMENT_SHADER, "Shaders/BasicFrag.frag")
		.Link();
	
	m_programPostprocessID = glCreateProgram();
	ProgramBuilder{ m_programPostprocessID }
		.ShaderStage(GL_VERTEX_SHADER, "Shaders/Vert_FullScreen.vert")
		.ShaderStage(GL_FRAGMENT_SHADER, "Shaders/Frag_Postprocess.frag")
		.Link();

	InitAxesShader();
}

void CMyApp::CleanShaders()
{
	glDeleteProgram(m_programID);
	glDeleteProgram(m_programPostprocessID);
	CleanAxesShader();
}

void CMyApp::InitAxesShader()
{
	m_programAxesID = glCreateProgram();
	ProgramBuilder{ m_programAxesID }
		.ShaderStage(GL_VERTEX_SHADER, "Shaders/Vert_axes.vert")
		.ShaderStage(GL_FRAGMENT_SHADER, "Shaders/Frag_PosCol.frag")
		.Link();
}

void CMyApp::CleanAxesShader()
{
	glDeleteProgram(m_programAxesID);
}

MeshObject<Vertex> createCube()
{
	MeshObject<Vertex> mesh;

	mesh.vertexArray = {
		// Front face
		{{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.5f, 0.0f}}, // Bottom-left
		{{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},  // Bottom-right
		{{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.5f}},   // Top-right
		{{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.5f, 0.5f}},  // Top-left

		// Back face
		{{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}}, // Bottom-left
		{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},// Bottom-right
		{{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}}, // Top-right
		{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},  // Top-left

		// Left face
		{{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // Bottom-left
		{{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // Bottom-right
		{{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},  // Top-right
		{{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // Top-left

		// Right face
		{{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},  // Bottom-left
		{{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // Bottom-right
		{{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},  // Top-right
		{{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},   // Top-left

		// Top face
		{{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},  // Bottom-left
		{{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},   // Bottom-right
		{{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},  // Top-right
		{{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}, // Top-left

		// Bottom face
		{{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}}, // Bottom-left
		{{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}}, // Bottom-right
		{{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},  // Top-right
		{{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}, // Top-left
	};

	mesh.indexArray = {// Front face
					   0, 1, 2, 2, 3, 0,
					   // Back face
					   4, 5, 6, 6, 7, 4,
					   // Left face
					   8, 9, 10, 10, 11, 8,
					   // Right face
					   12, 13, 14, 14, 15, 12,
					   // Top face
					   16, 17, 18, 18, 19, 16,
					   // Bottom face
					   20, 21, 22, 22, 23, 20 };

	return mesh;
}


void CMyApp::InitGeometry()
{
	const std::initializer_list<VertexAttributeDescriptor> vertexAttribList =
	{
		{ 0, offsetof(Vertex, position), 3, GL_FLOAT },
		{ 1, offsetof(Vertex, normal),	 3, GL_FLOAT },
		{ 2, offsetof(Vertex, texcoord), 2, GL_FLOAT },
	};

	// Suzanne
	MeshObject<Vertex> suzanneMeshCPU = ObjParser::parse("Assets/Suzanne.obj");
	m_Suzanne = CreateGLObjectFromMesh(suzanneMeshCPU, vertexAttribList);
	m_cube = CreateGLObjectFromMesh(createCubeFace(FaceDirection::Front, glm::vec3(0),78), vertexAttribList);
	chunk->SetBlock(0, 19, 0, 1);
	chunk->SetBlock(0, 0, 0, 250);
	chunk->SetBlock(0, 19, 9, 2);
	chunk->SetBlock(9,19,9,3);
	m_chunk = chunk->GetOGLObject();
}




void CMyApp::CleanGeometry()
{
	CleanOGLObject(m_Suzanne);
}

void CMyApp::InitTextures()
{
	glCreateSamplers( 1, &m_SamplerID );
	glSamplerParameteri( m_SamplerID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glSamplerParameteri( m_SamplerID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glSamplerParameteri( m_SamplerID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glSamplerParameteri( m_SamplerID, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	ImageRGBA metalImage = ImageFromFile( "Assets/textures.png" );

	glCreateTextures( GL_TEXTURE_2D, 1, &m_metalTextureID );
	glTextureStorage2D( m_metalTextureID, NumberOfMIPLevels( metalImage ), GL_RGBA8, metalImage.width, metalImage.height );
	glTextureSubImage2D( m_metalTextureID, 0, 0, 0, metalImage.width, metalImage.height, GL_RGBA, GL_UNSIGNED_BYTE, metalImage.data() );

	glGenerateTextureMipmap( m_metalTextureID );
}

void CMyApp::CleanTextures()
{
	glDeleteTextures(1, &m_metalTextureID);

	glDeleteSamplers( 1, &m_SamplerID );
}

void CMyApp::InitFrameBufferObject()
{
	// FBO létrehozása
	glCreateFramebuffers(1, &m_frameBufferID);
}

void CMyApp::CleanFrameBufferObject()
{
	glDeleteFramebuffers( 1, &m_frameBufferID );
}

void CMyApp::InitResolutionDependentResources( int width, int height )
{
	// Setup the texture
	// We use texture because we will sample it later in the shader
	glCreateTextures( GL_TEXTURE_2D, 1, &m_colorBufferID );
	glTextureStorage2D( m_colorBufferID, 1, GL_RGBA32F, width, height );

	glNamedFramebufferTexture(m_frameBufferID, GL_COLOR_ATTACHMENT0, m_colorBufferID, 0);

	// Setup renderbuffer
	// We use renderbuffer because it's more optimized for drawing
	// and we won't sample it later
	glCreateRenderbuffers( 1, &m_depthBufferID );
	glNamedRenderbufferStorage(m_depthBufferID, GL_DEPTH_COMPONENT24, width, height);
	
	glNamedFramebufferRenderbuffer(m_frameBufferID, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBufferID);

	// Completeness check
	GLenum status = glCheckNamedFramebufferStatus(m_frameBufferID, GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		switch (status) {
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[InitFramebuffer] Incomplete framebuffer GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT!");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[InitFramebuffer] Incomplete framebuffer GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT!");
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "[InitFramebuffer] Incomplete framebuffer GL_FRAMEBUFFER_UNSUPPORTED!");
			break;
		}
	}
}

void CMyApp::CleanResolutionDependentResources()
{
	glDeleteRenderbuffers( 1, &m_depthBufferID );
	glDeleteTextures(1, &m_colorBufferID);
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
	m_cameraManipulator.SetCamera(&m_camera);

	return true;
}

void CMyApp::Clean()
{
	CleanShaders();
	CleanGeometry();
	CleanTextures();
	CleanResolutionDependentResources();
	CleanFrameBufferObject();

}

void CMyApp::Update(const SUpdateInfo& updateInfo)
{
	m_cameraManipulator.Update(updateInfo.DeltaTimeInSec);
    m_ElapsedTimeInSec = updateInfo.ElapsedTimeInSec;
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
	glBindTextureUnit( 0, m_metalTextureID );
	glBindSampler( 0, m_SamplerID );

	glUniformMatrix4fv(ul("viewProj"), 1, GL_FALSE, glm::value_ptr(m_camera.GetViewProj()));
	
	DrawObject(m_chunk,glm::translate(glm::vec3(0.0,0.0,0.0)));
}

void CMyApp::DrawAxes()
{
	// We always want to see it, regardless of whether there is an object in front of it
	glDisable(GL_DEPTH_TEST);
	glUseProgram(m_programAxesID);

	glUniformMatrix4fv( ul("viewProj"), 1, GL_FALSE, glm::value_ptr(m_camera.GetViewProj()));
	glUniformMatrix4fv( ul("world"), 1, GL_FALSE, glm::value_ptr(glm::translate(m_camera.GetAt())));

	glDrawArrays(GL_LINES, 0, 6);
	glEnable(GL_DEPTH_TEST);
	glUseProgram(0);
}

void CMyApp::Render()
{
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	RenderGeometry();
	
	DrawAxes();
}

/*
void CMyApp::Render()
{

	// Render framebuffer

	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RenderGeometry();

	// Draw a full-screen quad and stretch the image rendered before as texture

	// Step back to default FBO- (=backbuffer)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray( 0 );

	glBindTextureUnit( 0, m_colorBufferID );
	glBindSampler( 0, m_SamplerID );

	glUseProgram(m_programPostprocessID);
	glUniform1i( ul("frameTex"),0);
	glUniform1f( ul( "filter_weight"), m_filterWeight);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//glBindTextureUnit( 0, 0 );
	glBindSampler( 0, 0 );
	glUseProgram(0);
	glBindVertexArray(0);

	DrawAxes();
}
*/

void CMyApp::RenderGUI()
{
	// ImGui DemoWindow
	//ImGui::ShowDemoWindow();

	//ImGui::SetNextWindowSize(ImVec2(455, 60), ImGuiCond_FirstUseEver);
	static int selectedFace = 0;
	static int textureIndex = 0;
	static OGLObject ogl;
	static bool initialized = false;

	bool needsUpdate = false;

	const char* faceNames[] = { "Front", "Back", "Left", "Right", "Top", "Bottom" };
	if (ImGui::Combo("Face", &selectedFace, faceNames, IM_ARRAYSIZE(faceNames))) {
		needsUpdate = true;
	}

	if (ImGui::SliderInt("Texture Coord", &textureIndex, 0, 255)) {
		needsUpdate = true;
	}

	if (!initialized || needsUpdate) {
		FaceDirection face = static_cast<FaceDirection>(selectedFace);
		uint8_t tex = static_cast<uint8_t>(textureIndex);
		auto mesh = createCubeFace(face, { 0,0,0}, tex);
		const std::initializer_list<VertexAttributeDescriptor> vertexAttribList =
		{
			{ 0, offsetof(Vertex, position), 3, GL_FLOAT },
			{ 1, offsetof(Vertex, normal),	 3, GL_FLOAT },
			{ 2, offsetof(Vertex, texcoord), 2, GL_FLOAT },
		};
		CleanOGLObject(m_cube);
		m_cube = CreateGLObjectFromMesh(mesh, vertexAttribList);
	}
}

// https://wiki.libsdl.org/SDL2/SDL_KeyboardEvent
// https://wiki.libsdl.org/SDL2/SDL_Keysym
// https://wiki.libsdl.org/SDL2/SDL_Keycode
// https://wiki.libsdl.org/SDL2/SDL_Keymod

void CMyApp::KeyboardDown(const SDL_KeyboardEvent& key)
{
	if (key.repeat == 0) // Triggers only once when held
	{
		if (key.keysym.sym == SDLK_F5 && key.keysym.mod & KMOD_CTRL) // CTRL + F5
		{
			CleanShaders();
			InitShaders();
		}
		if (key.keysym.sym == SDLK_F1) // F1
		{
			GLint polygonModeFrontAndBack[2] = {};
			// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGet.xhtml
			glGetIntegerv(GL_POLYGON_MODE, polygonModeFrontAndBack); // Query the current polygon mode. It gives the front and back modes separately.
			GLenum polygonMode = (polygonModeFrontAndBack[0] != GL_FILL ? GL_FILL : GL_LINE); // Switch between FILL and LINE
			// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glPolygonMode.xhtml
			glPolygonMode(GL_FRONT_AND_BACK, polygonMode); // Set the new polygon mode
		}
	}
	m_cameraManipulator.KeyboardDown(key);
}

void CMyApp::KeyboardUp(const SDL_KeyboardEvent& key)
{
	m_cameraManipulator.KeyboardUp(key);
}

// https://wiki.libsdl.org/SDL2/SDL_MouseMotionEvent

void CMyApp::MouseMove(const SDL_MouseMotionEvent& mouse)
{
	m_cameraManipulator.MouseMove(mouse);
}

// https://wiki.libsdl.org/SDL2/SDL_MouseButtonEvent

void CMyApp::MouseDown(const SDL_MouseButtonEvent& mouse)
{
}

void CMyApp::MouseUp(const SDL_MouseButtonEvent& mouse)
{
}

// https://wiki.libsdl.org/SDL2/SDL_MouseWheelEvent

void CMyApp::MouseWheel(const SDL_MouseWheelEvent& wheel)
{
	m_cameraManipulator.MouseWheel(wheel);
}

// New window size
void CMyApp::Resize(int _w, int _h)
{
	glViewport(0, 0, _w, _h);
	m_camera.SetAspect(static_cast<float>(_w) / _h);

	// When we resize we need to remake the framebuffer with the new size,
	// because now we want the two framebuffer (default,ours) to be the same resolution
    CleanResolutionDependentResources();
	InitResolutionDependentResources(_w, _h);
}

// Other SDL events
// https://wiki.libsdl.org/SDL2/SDL_Event

void CMyApp::OtherEvent(const SDL_Event& ev)
{
}

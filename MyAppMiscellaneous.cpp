#include "MyApp.h"
#include "SDL_GLDebugMessageCallback.h"
#include "GLUtils.hpp"
#include "ObjParser.h"
#include "ProgramBuilder.h"

#include <imgui.h>
#include <iostream>


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
	InitHighlightShaders();
}

void CMyApp::CleanShaders()
{
	glDeleteProgram(m_programID);
	glDeleteProgram(m_programPostprocessID);
	CleanAxesShader(); 
	CleanHighlightShaders();
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

void CMyApp::InitHighlightShaders()
{
	m_programBlockHighlightID = glCreateProgram();
	ProgramBuilder{ m_programBlockHighlightID }
		.ShaderStage(GL_VERTEX_SHADER, "Shaders/BlockHighLight.vert")
		.ShaderStage(GL_FRAGMENT_SHADER, "Shaders/FragBasicCol.frag")
		.Link();
}

void CMyApp::CleanHighlightShaders()
{
	glDeleteProgram(m_programBlockHighlightID);
}

void CMyApp::CleanGeometry()
{
}

void CMyApp::InitTextures()
{
	glCreateSamplers(1, &m_SamplerID);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	ImageRGBA metalImage = ImageFromFile("Assets/textures.png");

	glCreateTextures(GL_TEXTURE_2D, 1, &m_textureAtlasID);
	glTextureStorage2D(m_textureAtlasID, NumberOfMIPLevels(metalImage), GL_RGBA8, metalImage.width, metalImage.height);
	glTextureSubImage2D(m_textureAtlasID, 0, 0, 0, metalImage.width, metalImage.height, GL_RGBA, GL_UNSIGNED_BYTE, metalImage.data());

	glGenerateTextureMipmap(m_textureAtlasID);
}

void CMyApp::CleanTextures()
{
	glDeleteTextures(1, &m_textureAtlasID);

	glDeleteSamplers(1, &m_SamplerID);
}

void CMyApp::InitFrameBufferObject()
{
	// FBO létrehozása
	glCreateFramebuffers(1, &m_frameBufferID);
}

void CMyApp::CleanFrameBufferObject()
{
	glDeleteFramebuffers(1, &m_frameBufferID);
}

void CMyApp::InitResolutionDependentResources(int width, int height)
{
	// Setup the texture
	// We use texture because we will sample it later in the shader
	glCreateTextures(GL_TEXTURE_2D, 1, &m_colorBufferID);
	glTextureStorage2D(m_colorBufferID, 1, GL_RGBA32F, width, height);

	glNamedFramebufferTexture(m_frameBufferID, GL_COLOR_ATTACHMENT0, m_colorBufferID, 0);

	// Setup renderbuffer
	// We use renderbuffer because it's more optimized for drawing
	// and we won't sample it later
	glCreateRenderbuffers(1, &m_depthBufferID);
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
	glDeleteRenderbuffers(1, &m_depthBufferID);
	glDeleteTextures(1, &m_colorBufferID);
}

void CMyApp::Clean()
{
	CleanShaders();
	CleanGeometry();
	CleanTextures();
	CleanResolutionDependentResources();
	CleanFrameBufferObject();

}

void CMyApp::DrawAxes()
{
	// We always want to see it, regardless of whether there is an object in front of it
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE); // Required

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glUseProgram(m_programAxesID);
	glDrawArrays(GL_POINTS, 0, 1);
	glDisable(GL_PROGRAM_POINT_SIZE); // Required
	glEnable(GL_DEPTH_TEST);
	glUseProgram(0);
}

void CMyApp::DrawBlockHighlight(glm::vec3 pos, glm::vec3 col)
{
	// We always want to see it, regardless of whether there is an object in front of it

	//glDisable(GL_DEPTH_TEST);

	glUseProgram(m_programBlockHighlightID);
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glUniformMatrix4fv(ul("viewProj"), 1, GL_FALSE, glm::value_ptr(m_camera.GetViewProj()));
	glUniformMatrix4fv(ul("world"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
	glUniform3fv(ul("minCorner"), 1, glm::value_ptr(pos));
	glUniform3fv(ul("colorIN"), 1, glm::value_ptr(col));

	glDrawArrays(GL_LINES, 0, 24);
	glUseProgram(0);
	//glEnable(GL_DEPTH_TEST);
}


void CMyApp::Render()
{

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RenderGeometry();
	//DrawAxes();

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
	m_fpsCamera.KeyboardDown(key);
}

void CMyApp::KeyboardUp(const SDL_KeyboardEvent& key)
{
	m_fpsCamera.KeyboardUp(key);
}

// https://wiki.libsdl.org/SDL2/SDL_MouseMotionEvent

void CMyApp::MouseMove(const SDL_MouseMotionEvent& mouse)
{
	m_fpsCamera.MouseMove(mouse);
}

// https://wiki.libsdl.org/SDL2/SDL_MouseButtonEvent

void CMyApp::MouseDown(const SDL_MouseButtonEvent& mouse)
{
	if (!mouseDown)
	{
		if (mouse.button == SDL_BUTTON_RIGHT)
		{
			m_manager->SetBlockAtPlace(m_blockId);
			m_manager->GenerateOGLObjects();
		}
		else if (mouse.button == SDL_BUTTON_LEFT)
		{
			m_manager->SetBlockAtPlace(Chunk::AIR);
			m_manager->GenerateOGLObjects();
		}
		else if (mouse.button == SDL_BUTTON_MIDDLE)
		{
			m_blockId = m_manager->GetSelection();
		}

		mouseDown = true;
	}
}


void CMyApp::MouseUp(const SDL_MouseButtonEvent& mouse)
{
	mouseDown = false;
}

// https://wiki.libsdl.org/SDL2/SDL_MouseWheelEvent

void CMyApp::MouseWheel(const SDL_MouseWheelEvent& wheel)
{
	m_fpsCamera.MouseWheel(wheel);
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




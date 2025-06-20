#pragma once

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

// GLEW
#include <GL/glew.h>

// SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

// Utils
#include "GLUtils.hpp"
#include "Camera.h"
#include "CameraManipulator.h"
#include "FPSCameraManipulator.h"
#include "ChunkManager.h"


struct SUpdateInfo
{
	float ElapsedTimeInSec = 0.0f;	// Elapsed time since start of the program
	float DeltaTimeInSec = 0.0f;	// Elapsed time since last update
};

class CMyApp
{
public:
	CMyApp();
	~CMyApp();

	bool Init();
	void Clean();

	void Update(const SUpdateInfo&);
	void Render();
	void RenderGUI();

	void KeyboardDown(const SDL_KeyboardEvent&);
	void KeyboardUp(const SDL_KeyboardEvent&);
	void MouseMove(const SDL_MouseMotionEvent&);
	void MouseDown(const SDL_MouseButtonEvent&);
	void MouseUp(const SDL_MouseButtonEvent&);
	void MouseWheel(const SDL_MouseWheelEvent&);
	void Resize(int, int);

	void OtherEvent(const SDL_Event&);
protected:
	void SetupDebugCallback();
	void RenderGeometry();
	
	// Variables
	float m_ElapsedTimeInSec = 0.0f;

	glm::mat4 m_suzanneWorldTransform[ 9 ] = {};
	bool mouseDown = false;
	// Camera
	Camera m_camera;
	//CameraManipulator m_fpsCamera;
	FPSCameraManipulator m_fpsCamera;
	int m_blockId=237;
	// OpenGL

	void DrawAxes();

	void DrawBlockHighlight(glm::vec3 pos, glm::vec3 col);

	// Shader variables
	GLuint m_programID = 0;			// Shader of the objects
	GLuint m_programAxesID = 0;		// Program showing X,Y,Z directions
	GLuint m_programBlockHighlightID = 0;		// Program showing X,Y,Z directions
	GLuint m_programPostprocessID = 0; // Postprocess program

	float m_filterWeight = 0;

	// Light source
	glm::vec4 m_lightPosition = glm::vec4(0.0,1.5,0.0,1.0);
	glm::vec3 m_La = glm::vec3(0.0, 0.0, 0.0);	// Ambient
	glm::vec3 m_Ld = glm::vec3(1.0, 1.0, 1.0);	// Diffuse
	glm::vec3 m_Ls = glm::vec3(0.5);  	        // Specular (OFF)

	// Shader initialization and termination
	void InitShaders();
	void CleanShaders();
	void InitAxesShader();
	void CleanAxesShader();
	void InitHighlightShaders();
	void CleanHighlightShaders();
	void DrawObject(OGLObject& obj, const glm::mat4& world);

	// Geometry variables
	
	std::shared_ptr<ChunkManager> m_manager;

	// Geometry initialization and termination
	void InitGeometry();
	void CleanGeometry();

	// Texture variables
	GLuint m_SamplerID = 0;

	GLuint m_textureAtlasID = 0;

	// Texture initialization and termination
	void InitTextures();
	void CleanTextures();

	// Framebuffer variables
	GLuint m_frameBufferID = 0;
	GLuint m_colorBufferID = 0;
	GLuint m_depthBufferID = 0;

	// Framebuffer initialization and termination
	void InitFrameBufferObject();
	void CleanFrameBufferObject();
	void InitResolutionDependentResources(int, int);
	void CleanResolutionDependentResources();
};

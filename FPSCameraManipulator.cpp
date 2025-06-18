#include "FPSCameraManipulator.h"

#include "Camera.h"

#include <SDL2/SDL.h>

FPSCameraManipulator::FPSCameraManipulator()
{
}

FPSCameraManipulator::~FPSCameraManipulator()
{
}

void FPSCameraManipulator::SetCamera(Camera* _pCamera)
{
	m_pCamera = _pCamera;

	if (!m_pCamera) return;

	// A kezdeti g�mbkoordin�t�k be�ll�t�sa. 
	m_center = m_pCamera->GetAt();
	glm::vec3 ToAim = m_center - m_pCamera->GetEye();

	m_distance = glm::length(ToAim);

	m_u = atan2f(ToAim.z, ToAim.x);
	m_v = acosf(ToAim.y / m_distance);

	m_worldUp = m_pCamera->GetWorldUp();
}

void FPSCameraManipulator::Update(float _deltaTime)
{
	if (!m_pCamera) return;

	// Friss�tj�k a kamer�t a Model param�terek alapj�n. 

	// Az �j n�z�si ir�nyt a g�mbi koordin�t�k alapj�n sz�moljuk ki. 
	glm::vec3 lookDirection(cosf(m_u) * sinf(m_v),
		cosf(m_v),
		sinf(m_u) * sinf(m_v));
	// Az �j kamera poz�ci�t a n�z�si ir�ny �s a t�vols�g alapj�n sz�moljuk ki. 
	glm::vec3 eye = m_center - m_distance * lookDirection;

	// Az �j felfel� ir�ny a vil�g felfel�vel legyen azonos. 
	glm::vec3 up = m_pCamera->GetWorldUp();

	// Az �j jobbra ir�nyt a n�z�si ir�ny �s a felfel� ir�ny keresztszorzat�b�l sz�moljuk ki. 
	glm::vec3 right = glm::normalize(glm::cross(lookDirection, up));

	// Az �j el�re ir�nyt a felfel� �s jobbra ir�nyok keresztszorzat�b�l sz�moljuk ki. 
	glm::vec3 forward = glm::cross(up, right);

	// Az �j elmozdul�sat a kamera mozg�s ir�ny�nak �s sebess�g�nek a seg�ts�g�vel sz�moljuk ki. 
	glm::vec3 deltaPosition = (m_goForward * forward + m_goRight * right + m_goUp * up) * m_speed * _deltaTime;

	// Az �j kamera poz�ci�t �s n�z�si c�l poz�ci�t be�ll�tjuk. 
	eye += deltaPosition;
	m_center += deltaPosition;

	// Friss�tj�k a kamer�t az �j poz�ci�val �s n�z�si ir�nnyal. 
	m_pCamera->SetView(eye, m_center, m_worldUp);
}


void FPSCameraManipulator::KeyboardDown(const SDL_KeyboardEvent& key)
{
	switch (key.keysym.sym)
	{
	case SDLK_LSHIFT:
	case SDLK_RSHIFT:
		if (key.repeat == 0) m_speed /= 4.0f;
		break;
	case SDLK_w:
		m_goForward = 1;
		break;
	case SDLK_s:
		m_goForward = -1;
		break;
	case SDLK_a:
		m_goRight = -1;
		break;
	case SDLK_d:
		m_goRight = 1;
		break;
	case SDLK_e:
		m_goUp = 1;
		break;
	case SDLK_q:
		m_goUp = -1;
		break;
	}
}

void FPSCameraManipulator::KeyboardUp(const SDL_KeyboardEvent& key)
{

	switch (key.keysym.sym)
	{
	case SDLK_LSHIFT:
	case SDLK_RSHIFT:
		m_speed *= 4.0f;
		break;
	case SDLK_w:
	case SDLK_s:
		m_goForward = 0;
		break;
	case SDLK_a:
	case SDLK_d:
		m_goRight = 0;
		break;
	case SDLK_q:
	case SDLK_e:
		m_goUp = 0;
		break;
	}
}


void FPSCameraManipulator::MouseMove(const SDL_MouseMotionEvent& mouse)
{
	if (mouse.state & SDL_BUTTON_LMASK)
	{
		float du = mouse.xrel / 100.0f;
		float dv = mouse.yrel / 100.0f;

		m_u += du;
		m_v = glm::clamp<float>(m_v + dv, 0.1f, 3.1f);
	}
	if (mouse.state & SDL_BUTTON_RMASK)
	{
		m_distance *= pow(0.9f, mouse.yrel / 50.0f);
	}
}

void FPSCameraManipulator::MouseWheel(const SDL_MouseWheelEvent& wheel)
{
	m_distance *= powf(0.9f, static_cast<float>(wheel.y));
}

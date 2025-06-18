#pragma once

#include <glm/glm.hpp>

class Camera;

struct SDL_KeyboardEvent;
struct SDL_MouseMotionEvent;
struct SDL_MouseWheelEvent;

class FPSCameraManipulator
{
public:
	FPSCameraManipulator();

	~FPSCameraManipulator();

	void SetCamera(Camera* _pCamera);
	void Update(float _deltaTime);

	inline void  SetSpeed(float _speed) { m_speed = _speed; }
	inline float GetSpeed() const noexcept { return m_speed; }

	void KeyboardDown(const SDL_KeyboardEvent& key);
	void KeyboardUp(const SDL_KeyboardEvent& key);
	void MouseMove(const SDL_MouseMotionEvent& mouse);
	void MouseWheel(const SDL_MouseWheelEvent& wheel);

private:
	Camera* m_pCamera = nullptr;

	// Az u koordin�ta a g�mbkoordin�ta p�ros (u,v) r�sze, ami a
	// n�z�si ir�nyt adja meg az m_eye poz�ci�b�l.
	float	m_u = 0.0f;

	// Az v koordin�ta a g�mbkoordin�ta p�ros (u,v) r�sze, ami a
	// n�z�si ir�nyt adja meg az m_eye poz�ci�b�l.
	float	m_v = 0.0f;

	// A t�vols�g a n�z�si c�l poz�ci�ja �s a kamera poz�ci�ja k�z�tt. 
	float	m_distance = 0.0f;

	// A k�z�ppontja a k�r modellnek. 
	glm::vec3 m_eye_pos = glm::vec3(0.0f);

	// A vil�g felfele vektora a kamer�nak. 
	glm::vec3 m_worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	// A kamera mozg�s�nak a sebess�ge. 
	float m_speed = 16.0f;

	// A k�l�nb�z� ir�nyokba mutat� mozg�s indik�torok. 
	float	m_goForward = 0.0f;
	float	m_goRight = 0.0f;
	float   m_goUp = 0.0f;
};

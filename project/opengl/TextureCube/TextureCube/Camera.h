#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "vmath.h"

using namespace vmath;

class Camera
{
public:
	Camera()
	{
		
	}

	void init(vec3 &position, vec3 &up, float pitch, float yaw)
	{
		_pitch = pitch;
		_yaw = yaw;
		_position = position;
		_up = up;
		_mouseSensitivity = 0.35f;

		updateCameraMatrix();
	}

	void updateCameraMatrix()
	{
		_front = vec3(cosf(radians(_pitch))*cosf(radians(_yaw)), sinf(radians(_pitch)), cosf(radians(_pitch))*sinf(radians(_yaw)));
		_z = normalize(-_front);
		_x = cross(_up, _z);
		_y = cross(_z, _x);

		mat4 rot = mat4(vec4(_x, 0.0f), vec4(_y, 0.0f), vec4(_z, 0.0f), vec4(0.0f, 0.0f, 0.0f, 1.0f));
		mat4 trans = translate(-_position);
		_viewMatrix = rot.transpose() * trans;
	}

	mat4 & getViewMatrix()
	{
		return _viewMatrix;
	}

	void update(float time)
	{
		GLfloat speed = 5.0f * time;
		if (GetAsyncKeyState('W') && 0x8000)
		{
			if (GetAsyncKeyState(VK_SHIFT) && 0x8000)
			{
				_position += speed * _y;
			}
			else
			{
				_position += -speed * _z;
			}
		}
		if (GetAsyncKeyState('S') && 0x8000)
		{
			if (GetAsyncKeyState(VK_SHIFT) && 0x8000)
			{
				_position += -speed * _y;
			}
			else
			{
				_position += speed * _z;
			}
		}
		if (GetAsyncKeyState('A') && 0x8000)
		{
			_position += -speed * _x;
		}
		if (GetAsyncKeyState('D') && 0x8000)
		{
			_position += speed *_x;
		}
		updateCameraMatrix();
	}

	void mouseRightButtonDown(int x, int y)
	{
		_preX = x;
		_preY = y;
		_bRotate = true;
	}

	void mouseRightButtonUp(int x, int y)
	{
		_bRotate = false;
	}

	void mouseMove(int x, int y)
	{
		if (!_bRotate)
		{
			return;
		}

		int deltaX = x - _preX;
		int deltaY = y - _preY;

		_pitch += -deltaY * _mouseSensitivity;
		if (_pitch < -80.0f)
		{
			_pitch = -80.0f;
		}
		else if (_pitch > 80.0f)
		{
			_pitch = 80.0f;
		}

		_yaw += deltaX * _mouseSensitivity;

		_preX = x;
		_preY = y;

		//char szLog[1024];
		//sprintf(szLog, "_pitch = %.2f, _yaw = %.2f\n", _pitch, _yaw);
		//OutputDebugString(szLog);

		updateCameraMatrix();
	}

private:
	vec3 _front;
	vec3 _x, _y, _z;
	vec3 _position;
	vec3 _up;
	float _pitch, _yaw;
	mat4 _viewMatrix;
	int _preX, _preY;
	float _mouseSensitivity;
	bool _bRotate;
};

#endif

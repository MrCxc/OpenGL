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

	void init(vec3 &position, vec3 &front, vec3 &up)
	{
		_position = position;
		_z = normalize(-front);
		_up = up;
	}

	void updateCameraMatrix()
	{
		_x = normalize(cross(_up, _z));
		_y = normalize(cross(_z, _x));

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
		if (GetAsyncKeyState(VK_UP) && 0x8000)
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
		if (GetAsyncKeyState(VK_DOWN) && 0x8000)
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
		if (GetAsyncKeyState(VK_LEFT) && 0x8000)
		{
			_position += -speed * _x;
		}
		if (GetAsyncKeyState(VK_RIGHT) && 0x8000)
		{
			_position += speed *_x;
		}
		updateCameraMatrix();
	}

private:
	vec3 _x, _y, _z;
	vec3 _position;
	vec3 _up;
	mat4 _viewMatrix;
};

#endif

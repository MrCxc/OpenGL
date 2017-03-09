#include <Windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <math.h>
#include <stdio.h>
#include <gl/glew.h>
#include <gl/wglew.h>

#include "vmath.h"
#include "Camera.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")

struct Vertex
{
	GLfloat position[3];
	GLfloat color[3];
}; 

GLuint vao, vbo, ebo;
GLuint vs, fs, program;
//模型变换矩阵, 相机变换矩阵，投影变换矩阵
vmath::Tmat4<GLfloat> modelMatrix, viewMatrix, projMatrix;
GLfloat rot;
Camera cmr;

void windowResize(int width, int height)
{
	glViewport(0, 0, width, height);
}

void initGL(void)
{
	glShadeModel(GL_SMOOTH);
	//glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	Vertex cubeVertices[24] = {
		//front
		{
			-0.5f, 0.5f, 0.5f,
			1.0f, 0.0f, 0.0f
		},
		{
			-0.5f, -0.5f, 0.5f,
			0.0f, 1.0f, 0.0f
		},
		{
			0.5f, -0.5f, 0.5f,
			0.0f, 0.0f, 1.0f
		},
		{
			0.5f, 0.5f, 0.5f,
			1.0f, 1.0f, 1.0f
		},
		//back
		{
			0.5f, 0.5f, -0.5f,
			1.0f, 0.0f, 0.0f
		},
		{
			0.5f, -0.5f, -0.5f,
			0.0f, 1.0f, 0.0f
		},
		{
			-0.5f, -0.5f, -0.5f,
			0.0f, 0.0f, 1.0f
		},
		{
			-0.5f, 0.5f, -0.5f,
			1.0f, 1.0f, 1.0f
		},
		//left
		{
			-0.5f, 0.5f, -0.5f,
			1.0f, 0.0f, 0.0f
		},
		{
			-0.5f, -0.5f, -0.5f,
			0.0f, 1.0f, 0.0f
		},
		{
			-0.5f, -0.5f, 0.5f,
			0.0f, 0.0f, 1.0f
		},
		{
			-0.5f, 0.5f, 0.5f,
			1.0f, 1.0f, 1.0f
		},
		//right
		{
			0.5f, 0.5f, 0.5f,
			1.0f, 0.0f, 0.0f
		},
		{
			0.5f, -0.5f, 0.5f,
			0.0f, 1.0f, 0.0f
		},
		{
			0.5f, -0.5f, -0.5f,
			0.0f, 0.0f, 1.0f
		},
		{
			0.5f, 0.5f, -0.5f,
			1.0f, 1.0f, 1.0f
		},
		//top
		{
			-0.5f, 0.5f, -0.5f,
			1.0f, 0.0f, 0.0f
		},
		{
			-0.5f, 0.5f, 0.5f,
			0.0f, 1.0f, 0.0f
		},
		{
			0.5f, 0.5f, 0.5f,
			0.0f, 0.0f, 1.0f
		},
		{
			0.5f, 0.5f, -0.5f,
			1.0f, 1.0f, 1.0f
		},
		//bottom
		{
			-0.5f, -0.5f, 0.5f,
			1.0f, 0.0f, 0.0f
		},
		{
			-0.5f, -0.5f, -0.5f,
			0.0f, 1.0f, 0.0f
		},
		{
			0.5f, -0.5f, -0.5f,
			0.0f, 0.0f, 1.0f
		},
		{
			0.5f, -0.5f, 0.5f,
			1.0f, 1.0f, 1.0f
		},
	};

	GLuint cubeIndices[36] = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4,
		8, 9, 10, 10, 11, 8,
		12, 13, 14, 14, 15, 12,
		16, 17, 18, 18, 19, 16,
		20, 21, 22, 22, 23, 20,
	};

	//顶点布局
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)* 24, cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLuint), cubeIndices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//创建着色器
	const GLchar *vertexShader = " \
				#version 330 core\n \
				layout(location = 0) in vec3 a_position; \n\
				layout(location = 1) in vec3 a_color;\n\
				out vec3 v_color;\n\
				uniform mat4 u_MVPMatrix;\
				void main()\n \
				{ \n\
					gl_Position = u_MVPMatrix * vec4(a_position, 1.0); \n\
					v_color = a_color;\n \
				}\n";

	const GLchar *fragmentShader = " \
				#version 330 core\n \
				in vec3 v_color;\n \
				out vec4 out_color;\n \
				void main()\n \
				{ \n\
					out_color = vec4(v_color, 1.0);\n \
				} \n";

	
	vs = glCreateShader(GL_VERTEX_SHADER);
	GLint len = strlen(vertexShader);
	glShaderSource(vs, 1, &vertexShader, &len);
	glCompileShader(vs);
	GLint success;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar info[1024];
		glGetShaderInfoLog(vs, 1024, NULL, info);
		OutputDebugString("vsError:");
		OutputDebugString(info);
		OutputDebugString("\n");

	}

	fs = glCreateShader(GL_FRAGMENT_SHADER);
	len = strlen(fragmentShader);
	glShaderSource(fs, 1, &fragmentShader, &len);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar info[1024];
		glGetShaderInfoLog(fs, 1024, NULL, info);
		OutputDebugString("fsError:");
		OutputDebugString(info);
		OutputDebugString("\n");
	}

	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		GLchar info[1024];
		glGetProgramInfoLog(program, 1024, NULL, info);
		OutputDebugString("programError:");
		OutputDebugString(info);
		OutputDebugString("\n");
	}
	
	cmr.init(vmath::vec3(0.0f, 20.0f, 20.0f), vmath::vec3(0.0f, -1.0f, -1.0f), vmath::vec3(0.0f, 1.0f, 0.0f));

	//初始化矩阵
	projMatrix = vmath::perspective(60, 800.0f / 600.0f, 0.1f, 128.0f);
	viewMatrix = vmath::lookat(vmath::vec3(0.0f, 0.0f, 20.0f), vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3(0.0f, 1.0f, 0.0f));
}

void update(float time)
{
	cmr.update(time);
}

void render(float time)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//绑定着色器到渲染管线
	glUseProgram(program);
	//设置MVPMatrix
	GLuint location = glGetUniformLocation(program, "u_MVPMatrix");
	rot += time * 20.0f;
	modelMatrix = vmath::translate(0.0f, 0.0f, 0.0f) * vmath::rotate(rot, 0.0f, 1.0f, 0.0f) * vmath::scale(1.0f, 1.0f, 1.0f);
	viewMatrix = cmr.getViewMatrix();
	vmath::Tmat4<GLfloat> MVPMatrix = projMatrix * viewMatrix * modelMatrix;
	glUniformMatrix4fv(location, 1, GL_FALSE, MVPMatrix);
	//绑定VAO（已经设置好顶点布局的数组对象）

	/*
	location = glGetAttribLocation(program, "a_color");
	char szLocation[128];
	memset(szLocation, 0, sizeof(szLocation));
	sprintf(szLocation, "location = %d\n", location);
	OutputDebugString(szLocation);
	*/

	glBindVertexArray(vao);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	//渲染命令
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);


	location = glGetUniformLocation(program, "u_MVPMatrix");
	modelMatrix = vmath::translate(0.0f, 0.0f, 0.0f) * vmath::rotate(rot, 0.0f, 1.0f, 0.0f) * vmath::scale(3.0f, 3.0f, 3.0f);
	MVPMatrix = projMatrix * viewMatrix * modelMatrix;
	glUniformMatrix4fv(location, 1, GL_FALSE, MVPMatrix);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);

	glUseProgram(0);
	glBindVertexArray(0);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wndcls;
	wndcls.cbSize = sizeof(WNDCLASSEX);
	wndcls.cbClsExtra = 0;
	wndcls.cbWndExtra = 0;
	wndcls.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndcls.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndcls.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndcls.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndcls.hInstance = hInstance;
	wndcls.lpfnWndProc = WindowProc;
	wndcls.lpszClassName = "GLWindowClass";
	wndcls.lpszMenuName = NULL;
	wndcls.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

	if (!RegisterClassEx(&wndcls))
	{
		MessageBox(NULL, "RegisterClassEx failed!", "GLWindow", MB_OK);
		exit(1);
	}

	HWND hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, "GLWindowClass", "GLWindow", WS_OVERLAPPEDWINDOW,
		0, 0, 800, 600, NULL, NULL, hInstance, NULL);

	UpdateWindow(hWnd);
	ShowWindow(hWnd, nCmdShow);

	HDC hdc = GetDC(hWnd);
	int nPixelFormat;

	PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR),
		1, //版本号，总设为1
		PFD_DRAW_TO_WINDOW | //支持窗口
		PFD_SUPPORT_OPENGL | //支持OpenGL
		PFD_DOUBLEBUFFER, //支持双缓存
		PFD_TYPE_RGBA, //RGBA颜色模式
		32, //32位颜色模式
		0, 0, 0, 0, 0, 0, //忽略颜色为，不使用
		0, //无alpha缓存
		0, //忽略偏移位
		0, //无累积缓存
		0, 0, 0, 0, //忽略累积位
		24, //24位z-buffer（z缓存）大小
		8, //8位模板缓存
		0, //无辅助缓存
		PFD_MAIN_PLANE, //主绘制平面
		0, //保留的数据项
		0, 0, 0 }; //忽略层面掩模
	nPixelFormat = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, nPixelFormat, &pfd);

	HGLRC hrc = wglCreateContext(hdc);
	wglMakeCurrent(hdc, hrc);

	glewInit();

	initGL();

	MSG msg = { 0 };
	DWORD preTick = GetTickCount();
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			//render;
			DWORD tick = GetTickCount();
			float time = (tick - preTick) / 1000.0f;
			preTick = tick;
			update(time);
			render(time);
			SwapBuffers(hdc);
		}
	}

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hrc);
	ReleaseDC(hWnd, hdc);

	return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	DWORD width, height;
	switch (uMsg)
	{
	case WM_CREATE:
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_SIZE:
		width = LOWORD(lParam);
		height = HIWORD(lParam);
		windowResize(width, height);
		return 0;
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

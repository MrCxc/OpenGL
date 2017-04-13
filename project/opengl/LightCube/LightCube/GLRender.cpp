#include <Windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <math.h>
#include <stdio.h>
#include <gl/glew.h>
#include <gl/wglew.h>
#include <gl/SOIL.h>

#include "vmath.h"
#include "Camera.h"

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "SOIL.lib")

struct Vertex
{
	GLfloat position[3];
	GLfloat color[3];
	GLfloat texcoord[2];
	GLfloat normal[3];
}; 

GLuint vao, vbo, ebo;
GLuint vs, fs, program;
//模型变换矩阵, 相机变换矩阵，投影变换矩阵
vmath::Tmat4<GLfloat> modelMatrix, viewMatrix, projMatrix;
GLfloat rot;
Camera cmr;
GLuint tex;
GLfloat elapsedTime;
GLfloat uOffset, vOffset;

void windowResize(int width, int height)
{
	glViewport(0, 0, width, height);
	projMatrix = vmath::perspective(60, width*1.0f / height, 0.1f, 128.0f);
}

void mouseRightButtonDown(int x, int y)
{
	//ShowCursor(false);
	cmr.mouseRightButtonDown(x, y);
}

void mouseRightButtonUp(int x, int y)
{
	//ShowCursor(true);
	cmr.mouseRightButtonUp(x, y);
}

void mouseMove(int x, int y)
{
	cmr.mouseMove(x, y);
}

void initGL(void)
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_TEXTURE);
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	Vertex cubeVertices[24] = {
		//front
		{
			-0.5f, 0.5f, 0.5f,
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f,
			0.0f, 0.0f, 1.0f
		},
		{
			-0.5f, -0.5f, 0.5f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 0.0f, 1.0f
		},
		{
			0.5f, -0.5f, 0.5f,
			0.0f, 0.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f, 1.0f
		},
		{
			0.5f, 0.5f, 0.5f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f,
			0.0f, 0.0f, 1.0f
		},
		//back
		{
			0.5f, 0.5f, -0.5f,
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f,
			0.0f, 0.0f, -1.0f
		},
		{
			0.5f, -0.5f, -0.5f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 0.0f, -1.0f
		},
		{
			-0.5f, -0.5f, -0.5f,
			0.0f, 0.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 0.0f, -1.0f
		},
		{
			-0.5f, 0.5f, -0.5f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f,
			0.0f, 0.0f, -1.0f
		},
		//left
		{
			-0.5f, 0.5f, -0.5f,
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f,
			-1.0f, 0.0f, 0.0f
		},
		{
			-0.5f, -0.5f, -0.5f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f,
			-1.0f, 0.0f, 0.0f
		},
		{
			-0.5f, -0.5f, 0.5f,
			0.0f, 0.0f, 1.0f,
			1.0f, 0.0f,
			-1.0f, 0.0f, 0.0f
		},
		{
			-0.5f, 0.5f, 0.5f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f,
			-1.0f, 0.0f, 0.0f
		},
		//right
		{
			0.5f, 0.5f, 0.5f,
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 0.0f, 0.0f
		},
		{
			0.5f, -0.5f, 0.5f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f,
			1.0f, 0.0f, 0.0f
		},
		{
			0.5f, -0.5f, -0.5f,
			0.0f, 0.0f, 1.0f,
			1.0f, 0.0f,
			1.0f, 0.0f, 0.0f
		},
		{
			0.5f, 0.5f, -0.5f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f,
			1.0f, 0.0f, 0.0f
		},
		//top
		{
			-0.5f, 0.5f, -0.5f,
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f,
			0.0f, 1.0f, 0.0f
		},
		{
			-0.5f, 0.5f, 0.5f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 1.0f, 0.0f
		},
		{
			0.5f, 0.5f, 0.5f,
			0.0f, 0.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 1.0f, 0.0f
		},
		{
			0.5f, 0.5f, -0.5f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f,
			0.0f, 1.0f, 0.0f
		},
		//bottom
		{
			-0.5f, -0.5f, 0.5f,
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f,
			0.0f, -1.0f, 0.0f
		},
		{
			-0.5f, -0.5f, -0.5f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, -1.0f, 0.0f
		},
		{
			0.5f, -0.5f, -0.5f,
			0.0f, 0.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, -1.0f, 0.0f
		},
		{
			0.5f, -0.5f, 0.5f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f,
			0.0f, -1.0f, 0.0f
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
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

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
				layout(location = 2) in vec2 a_texcoord0;\
				layout(location = 3) in vec3 a_normal;\
				out vec3 v_color;\n\
				out vec2 v_texcoord0;\
				out vec3 v_normal;\
				out vec3 v_positionInWorld;\
				uniform mat4 u_MVPMatrix;\
				uniform mat4 u_ModelMatrix;\
				void main()\n \
				{ \n\
					gl_Position = u_MVPMatrix * vec4(a_position, 1.0); \n\
					v_positionInWorld = (u_ModelMatrix * vec4(a_position, 1.0)).xyz;\
					v_normal = mat3(transpose(inverse(u_ModelMatrix))) * a_normal;\
					v_color = a_color;\n \
					v_texcoord0 = a_texcoord0;\
				}\n";

	const GLchar *fragmentShader = " \
				#version 330 core\n \
				in vec3 v_color;\n \
				in vec2 v_texcoord0;\
				in vec3 v_normal;\
				in vec3 v_positionInWorld;\
				uniform vec3 u_lightPosition;\
				uniform vec3 u_viewPosition;\
				uniform sampler2D u_texture0;\
				uniform vec2 u_uvOffset;\
				uniform float u_sinTime;\
				out vec4 out_color;\n \
				void main()\n \
				{ \n\
					vec3 lightDir = normalize(vec3(1, 1, 1));\
					vec3 viewDir = normalize(u_viewPosition - v_positionInWorld);\
					vec3 normal = normalize(v_normal);\
					vec3 reflectDir = reflect(-lightDir, normal);\
					vec3 lightColor = vec3(1.0);\
					vec4 tex_color = texture(u_texture0, vec2(v_texcoord0.x, 1.0-v_texcoord0.y));\
					vec3 ambient = vec3(0.3) * tex_color.rgb;\
					vec3 diffuse = max(dot(lightDir, normal),0) * lightColor * tex_color.rgb;\
					//vec3 specular = pow(max(dot(reflectDir, viewDir), 0), 128) * lightColor * vec3(1.0f);\n\
					vec3 half = normalize(lightDir + viewDir);\
					vec3 specular = pow(max(dot(half, normal), 0), 128) * lightColor * vec3(1.0f);\
					out_color =  vec4(ambient + diffuse + specular, 1.0);\n \
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
	
	cmr.init(vmath::vec3(0.0f, 20.0f, 20.0f), vmath::vec3(0.0f, 1.0f, 0.0f), -45.0f, -90.0f);

	//初始化矩阵
	projMatrix = vmath::perspective(60, 800.0f / 600.0f, 0.1f, 128.0f);
	viewMatrix = vmath::lookat(vmath::vec3(0.0f, 0.0f, 20.0f), vmath::vec3(0.0f, 0.0f, 0.0f), vmath::vec3(0.0f, 1.0f, 0.0f));

	int texw, texh;
	unsigned char* imageData = SOIL_load_image("Resources/jx3.jpg", &texw, &texh, NULL, SOIL_LOAD_RGBA);

	glGenTextures(1, &tex);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texw, texh, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
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
	modelMatrix = vmath::translate(0.0f, 0.0f, 0.0f) * vmath::rotate(rot, 0.0f, 1.0f, 0.0f) * vmath::scale(5.0f, 5.0f, 5.0f);
	viewMatrix = cmr.getViewMatrix();
	vmath::Tmat4<GLfloat> MVPMatrix = projMatrix * viewMatrix * modelMatrix;
	glUniformMatrix4fv(location, 1, GL_FALSE, MVPMatrix);

	elapsedTime += time;
	GLfloat sinTime = abs(sinf(elapsedTime*3));
	location = glGetUniformLocation(program, "u_sinTime");
	glUniform1f(location, sinTime);

	//uniform vec3 u_lightPosition; \
	//	uniform vec3 u_viewPosition;
		//uniform mat4 u_ModelMatrix;

	location = glGetUniformLocation(program, "u_lightPosition");
	glUniform3f(location, 10.0f, 10.0f, 10.0f);
	
	location = glGetUniformLocation(program, "u_viewPosition");
	glUniform3fv(location, 1, cmr.getPosition());

	location = glGetUniformLocation(program, "u_ModelMatrix");
	glUniformMatrix4fv(location, 1, GL_FALSE, modelMatrix);

	uOffset += 1.0f * time;
	vOffset += 0.0f * time;
	if (uOffset > 1.0f) uOffset -= 1.0f;
	if (vOffset > 1.0f) vOffset -= 1.0f;
	location = glGetUniformLocation(program, "u_uvOffset");
	glUniform2f(location, uOffset, vOffset);

	location = glGetUniformLocation(program, "u_texture0");
	glUniform1i(location, 0);



	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	//绑定VAO（已经设置好顶点布局的数组对象）
	glBindVertexArray(vao);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	//渲染命令
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);

	glUseProgram(0);
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
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
	DWORD x, y;
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
	
	case WM_MOUSEMOVE:
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);
		mouseMove(x, y);
		return 0;

	case WM_RBUTTONDOWN:
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);
		mouseRightButtonDown(x, y);
		return 0;

	case WM_RBUTTONUP:
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);
		mouseRightButtonUp(x, y);
		return 0;

	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

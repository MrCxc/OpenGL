#include <Windows.h>
#include <mmsystem.h>
#include <math.h>
#include <stdio.h>
#include <gl/glew.h>

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

void initGL(void)
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	Vertex verts[3] =
	{
		{
			-0.5f, -0.5f, 0.0f,
			1.0f, 0.0f, 0.0f
		},
		{
			0.5f, -0.5f, 0.0f,
			0.0f, 1.0f, 0.0f
		},
		{
			0.0f, 0.5f, 0.0f,
			0.0f, 0.0f, 1.0f
		},
	};

	//顶点布局
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)* 3, verts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	GLuint ibo[] = { 0, 1, 2 };
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(GLuint), ibo, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	//创建着色器
	const GLchar *vertexShader = " \
				#version 330 core\n \
				layout(location = 0) in vec3 a_position; \n\
				layout(location = 1) in vec3 a_color;\n\
				out vec3 v_color;\n\
				void main()\n \
				{ \n\
					gl_Position = vec4(a_position, 1.0); \n\
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
}

void render(float time)
{
	glClear(GL_COLOR_BUFFER_BIT);

	//绑定着色器到渲染管线
	glUseProgram(program);
	//绑定VAO（已经设置好顶点布局的数组对象）
	glBindVertexArray(vao);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	//渲染命令
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, NULL);

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
			float time = GetTickCount() / 1000.0f;
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

	switch (uMsg)
	{
	case WM_CREATE:
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

#include <Windows.h>
#include <mmsystem.h>
#include <math.h>
#include <gl/glew.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")

void initGL(void)
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT, GL_FILL);
}

void render(float time)
{
	glClearColor(0.5f + 0.5f*sinf(time), 0.5f + 0.5f*cosf(time), 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
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
	wndcls.lpszClassName = L"GLWindowClass";
	wndcls.lpszMenuName = NULL;
	wndcls.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

	if (!RegisterClassEx(&wndcls))
	{
		MessageBox(NULL, L"RegisterClassEx failed!", L"GLWindow", MB_OK);
		exit(1);
	}
	
	HWND hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"GLWindowClass", L"GLWindow", WS_OVERLAPPEDWINDOW,
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

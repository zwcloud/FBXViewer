#include "stdafx.h"
#include "Windowsx.h"
#include "FBXViewer.h"
#include "GraphicsDevice.h"
#include "D3D/SkinnedMesh.h"
#include "D3D/TMesh.h"
#include "D3D/Camera.h"
#include "D3D/Axis.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;								// 当前实例
HWND hWnd;                                      // 主窗口句柄
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名
HANDLE HTimer;                                  // 时钟
unsigned int FrameCount;                        // 帧计数
// 输入的相关参数
bool Pause = false, bStepMode = true;
bool bStepForward = false;
// 图形设备
GraphicsDevice* pGDevice;

// Camera相关
D3DXMATRIX identity, view, proj, viewproj;
D3DXMATRIX fixedView;

// 需要绘制的Mesh
StaticMesh::AxisMesh axis;
StaticMesh::CubeMesh cube;
SkinnedMesh skinMesh;
TMesh::MeshType meshType = TMesh::RIGHTHANDED_ZUP;

//鼠标输入相关参数
D3DXVECTOR2 lastCursorPos(0.0f, 0.0f);
D3DXVECTOR2 currentCursorPos(0.0f, 0.0f);
bool g_bMouseTrack = false;
bool dragging = false;
bool hovering = false;
bool moving = false;

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void Update( unsigned int _dt );
void Render( unsigned int _dt );
void Destroy();

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 在此放置代码。
    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));
	HACCEL hAccelTable;
    DWORD time = 0;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_FBXVIEWER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FBXVIEWER));

	// 主消息循环:
	while (msg.message!=WM_QUIT)
    {
        //处理Windows消息
        if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
        {
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            //使用WaitableTimer来控制帧
            DWORD result = MsgWaitForMultipleObjects(
                1,
                &HTimer,
                FALSE,INFINITE,QS_ALLEVENTS);
            DebugAssert(result!=WAIT_FAILED, "MsgWaitForMultipleObjects等待失败: %d", GetLastError());

            if(GetFocus()!=hWnd || Pause)    //失去焦点、暂停则跳过此帧
            {
                continue;
            }

            switch (result)
            {
            case WAIT_OBJECT_0:
                {
                    if (!bStepMode)
                    {
                        FrameCount++;
                    }
                    else    //逐帧单步模式
                    {
                        if (bStepForward)
                        {
                            FrameCount++;
                            bStepForward = false;
                        }
                        Update(FrameCount);
                        Render(FrameCount);
                    }
                }
                break;
            default:
                break;
            }//END switch
        }
	}
	return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
//  注释:
//
//    仅当希望
//    此代码与添加到 Windows 95 中的“RegisterClassEx”
//    函数之前的 Win32 系统兼容时，才需要此函数及其用法。调用此函数十分重要，
//    这样应用程序就可以获得关联的
//    “格式正确的”小图标。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FBXVIEWER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_FBXVIEWER);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // 将实例句柄存储在全局变量中
    
    //创建并将主窗口句柄存储在全局变量中
    hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
       CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
    
    if (!hWnd)
    {
       return FALSE;
    }
    
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);


    /*
     timer
    */
    //创建WaitableTimer
    HTimer = CreateWaitableTimer(NULL,FALSE,NULL);
    DebugAssert(NULL!=HTimer, "CreateWaitableTimer 失败: %d", GetLastError());
    //初始化WaitableTimer
    LARGE_INTEGER liDueTime;
    liDueTime.QuadPart = -1i64;   //1秒后开始计时
    SetWaitableTimer(HTimer, &liDueTime, 100, NULL, NULL, 0);  //周期200ms = 0.2s
    FrameCount = 0;

    /*
        GraphicsDevice
    */
    pGDevice = GraphicsDevice::getInstance(hWnd);
    pGDevice->BuildViewports();
    /*
        Camera
    */
    InitCamera();
    cube.SetVertexData(0);
    cube.Create(pGDevice->m_pD3DDevice);
    /*
        Axis
    */
	axis.SetVertexData(0);  //参数实际上不起作用 TODO: 改进设计
    axis.Create(pGDevice->m_pD3DDevice);
    //axis.CreateXYZ(pGDevice->m_pD3DDevice);
    //axis.UpdateXYZ(fixedEyePoint);
    /*
        读取fbx，加载Skinned mesh
    */
    // 获取输出文件路径 测试用
    char fileSrc[MAX_PATH];
    GetTestFileName(fileSrc);
    skinMesh.Load(fileSrc, pGDevice->m_pD3DDevice);
    /*
        Matrix
    */
    D3DXMatrixIdentity(&identity);    
    D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 4.0f,
        (float)pGDevice->mDefaultViewport.Width / (float)pGDevice->mDefaultViewport.Height, 1.0f, 1000.0f);    
    return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
        break;
    case WM_MOUSEMOVE:
        moving = true;
        hovering = false;
        lastCursorPos = currentCursorPos;
        currentCursorPos.x = (float)GET_X_LPARAM(lParam);
		currentCursorPos.y = (float)GET_Y_LPARAM(lParam);
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(TRACKMOUSEEVENT);
        tme.dwFlags = TME_HOVER;
        tme.dwHoverTime = 100;
        tme.hwndTrack = hWnd;
        TrackMouseEvent(&tme);
        break;
    case WM_LBUTTONDOWN:
        lastCursorPos = currentCursorPos;
		currentCursorPos.x = (float)GET_X_LPARAM(lParam);
		currentCursorPos.y = (float)GET_Y_LPARAM(lParam);
        if (lastCursorPos!=currentCursorPos)
        {
            dragging = true;
        }
        else
        {
            dragging = false;
        }
        break;
    case WM_MOUSEHOVER:
        lastCursorPos = currentCursorPos;
		currentCursorPos.x = (float)GET_X_LPARAM(lParam);
		currentCursorPos.y = (float)GET_Y_LPARAM(lParam);
        hovering = true;
        moving = false;
        break;
    case WM_KEYDOWN:
        switch(wParam)
        {
        case VK_ESCAPE:    //按Esc退出
            PostQuitMessage(0);
            break;
        case 'P': //按'P'暂停/恢复
            Pause = Pause ? false : true;
            break;
        //camera 操作
        case 'W':
            MoveCameraForward(3.0f);
            break;
        case 'S':
            MoveCameraBackward(3.0f);
            break;
        case 'A':
            MoveCameraLeft(3.0f);
            break;
        case 'D':
            MoveCameraRight(3.0f);
            break;
        case 'R':
            ResetCamera();
            break;
        //参考：http://forums.codeguru.com/showthread.php?302925-Where-is-Page-Up-Down-Accelerator-Key&p=981700#post981700
        //Page Up : VK_PRIOR
        //Page Down : VK_NEXT
        case VK_PRIOR:
            MoveCameraUpward(3.0f);
            break;
        case VK_NEXT:
            MoveCameraDownward(3.0f);
            break;
        case 'Q':
            RotateCameraHorizontally(-D3DX_PI/45);
            break;
        case 'E':
            RotateCameraHorizontally(D3DX_PI/45);
            break;
        case VK_HOME:
            RotateCameraVertically(-D3DX_PI/45);
            break;
        case VK_END:
            RotateCameraVertically(D3DX_PI/45);
            break;
        case VK_SPACE:
            bStepForward = true;
            break;
        }
        break;
	case WM_DESTROY:
        Destroy();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


void Update( unsigned int _dt )
{
    //Update camera    
    D3DXMatrixLookAtLH(&view, &eyePoint, &lookAt, &up);
    D3DXMatrixLookAtLH(&fixedView, &fixedEyePoint, &fixedLookAt, &fixedUp);
    cube.Update(currentCursorPos, identity, fixedView, pGDevice->m_matCubeProj);
    if (dragging)
    {
        //拖动时进行旋转
        D3DXVECTOR2 mousePosDetla(currentCursorPos-lastCursorPos);
        RotateCameraHorizontally(D3DX_PI/4*mousePosDetla.x);
        RotateCameraVertically(D3DX_PI/4*mousePosDetla.y);
    }
    axis.Update();
    //axis.UpdateXYZ(fixedEyePoint);
    
    skinMesh.Update(identity, _dt);
}


void Render( unsigned int _dt )
{
    //DebugPrintf("Frame count: %d\n", FrameCount);
    HRESULT hr = S_OK;

    //清除视区
    pGDevice->Clear();

    //开始场景
    pGDevice->BeginScene();

    //Render skinmesh
    skinMesh.Render(pGDevice->m_pD3DDevice, identity, view, proj, eyePoint);

    //Render cube
    {
        pGDevice->SetViewport(pGDevice->mCubeViewport);
        cube.SetConstants(pGDevice->m_pD3DDevice, identity, fixedView, pGDevice->m_matCubeProj);
        cube.Draw(pGDevice->m_pD3DDevice);
        if (hovering || moving)
        {
            //将cube所在的viewport的位置(top-left)处的屏幕坐标变换到(0,0)
            D3DXVECTOR2 tmp(currentCursorPos);
            tmp.x -= pGDevice->mCubeViewport.X;
            tmp.y -= pGDevice->mCubeViewport.Y;
            cube.DrawRay(tmp, identity, fixedView, pGDevice->m_matCubeProj);
        }
        pGDevice->ResetViewport();
    }

    //Render axis
    {
        pGDevice->SetViewport(pGDevice->mAxisViewport);
        axis.SetConstants(pGDevice->m_pD3DDevice, identity, fixedView, pGDevice->m_matAxisProj);
        axis.Draw(pGDevice->m_pD3DDevice);
        //axis.DrawXYZ();
        pGDevice->ResetViewport();
	}

    //结束场景
    pGDevice->EndScene();

    //显示场景
    pGDevice->Present();
}

void Destroy()
{
    axis.Destroy();
    cube.Destroy();
    skinMesh.Destroy();

    GraphicsDevice::ReleaseInstance();
}
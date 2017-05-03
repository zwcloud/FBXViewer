#include "stdafx.h"
#include "Windows.h"
#include "graphics/GraphicsDevice.h"
#include "graphics/SkinnedMeshRenderer.h"
#include "graphics/Mesh.h"
#include "graphics/Camera.h"
#include "graphics/Axis.h"
#include "graphics/Material.h"
#include "graphics\RenderSettings.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;
HWND hWnd;
TCHAR szTitle[MAX_LOADSTRING] = __TEXT("Title");
TCHAR szWindowClass[MAX_LOADSTRING] = __TEXT("MyWindowClass");
HANDLE HTimer;// Frame signal timer
unsigned int FrameCount;
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
Mesh* targetMesh;
Skeleton* targetSkeleton;
Animation* targetAnimation;
Material* targetMaterial;//TODO load this from file
SkinnedMeshRenderer skinMeshRenderer;

//鼠标输入相关参数
D3DXVECTOR2 lastCursorPos(0.0f, 0.0f);
D3DXVECTOR2 currentCursorPos(0.0f, 0.0f);
bool g_bMouseTrack = false;
bool dragging = false;
bool hovering = false;
bool moving = false;

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
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

    MSG msg;
    ZeroMemory(&msg, sizeof(MSG));
    DWORD time = 0;

	MyRegisterClass(hInstance);

	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	while (msg.message!=WM_QUIT)
    {
        if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            DWORD result = MsgWaitForMultipleObjects(
                1,
                &HTimer,
                FALSE,INFINITE,QS_ALLEVENTS);
            DebugAssert(result!=WAIT_FAILED, "MsgWaitForMultipleObjects failed: %d", GetLastError());

            if(GetFocus()!=hWnd || Pause)    //lose focu or paused, ignore this frame
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
                        Update(FrameCount);
                        Render(FrameCount);
                    }
                    else
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

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;
    hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
       CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
    DebugAssert(hWnd != NULL, "CreateWindow Failed.");

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    //Timer
    HTimer = CreateWaitableTimer(NULL,FALSE,NULL);
    DebugAssert(NULL!=HTimer, "CreateWaitableTimer failed: %d", GetLastError());
    LARGE_INTEGER liDueTime;
    liDueTime.QuadPart = -1i64;   //1秒后开始计时
    SetWaitableTimer(HTimer, &liDueTime, 100, NULL, NULL, 0);  //周期200ms = 0.2s
    FrameCount = 0;

    //GraphicsDevice
    pGDevice = GraphicsDevice::getInstance(hWnd);
    pGDevice->BuildViewports();

    //Camera
    InitCamera();
    cube.SetVertexData();
    cube.Create(pGDevice->m_pD3DDevice);

    //Axis
	axis.SetVertexData();
    axis.Create(pGDevice->m_pD3DDevice);

	//Load mesh from file
    char filePath[MAX_PATH];
    GetTestFileName(filePath);// get testing file path
    MeshUtil::LoadMeshFromFile(filePath, targetMesh, targetSkeleton, targetAnimation);
    MeshUtil::Create(*targetMesh, pGDevice->m_pD3DDevice);
    targetMaterial = MaterialUtil::CreateMaterial(pGDevice->m_pD3DDevice, "<nodiffusemap>");
    skinMeshRenderer.Load(targetSkeleton, targetAnimation, targetMaterial);

    //Set up initial projection matrix
    D3DXMatrixIdentity(&identity);
    D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 4.0f,
        (float)pGDevice->mDefaultViewport.Width / (float)pGDevice->mDefaultViewport.Height, 1.0f, 1000.0f);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
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
		currentCursorPos.x = (float)GET_X_LPARAM(lParam);
		currentCursorPos.y = (float)GET_Y_LPARAM(lParam);
        lastCursorPos = currentCursorPos;
        dragging = true;
        break;
    case WM_LBUTTONUP:
        dragging = false;
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
        case VK_ESCAPE://Esc : quit
            PostQuitMessage(0);
            break;
        case VK_OEM_3://~
            bStepMode = !bStepMode;
            break;
        case '1':
        {
            bool& showMesh = RenderSettings::getInstance().ShowMesh();
            showMesh = !showMesh;
        }
            break;
        case '2':
        {
            bool& showSkeleton = RenderSettings::getInstance().ShowSkeleton();
            showSkeleton = !showSkeleton;
        }
            break;
        case 'P':
            Pause = !Pause;
            break;
        //camera
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
        case VK_PRIOR://Page Up : VK_PRIOR
            MoveCameraUpward(3.0f);
            break;
        case VK_NEXT://Page Down : VK_NEXT
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

void Update( unsigned int _dt )
{
    //Update camera
    D3DXMatrixLookAtLH(&view, &eyePoint, &lookAt, &up);
    D3DXMatrixLookAtLH(&fixedView, &fixedEyePoint, &fixedLookAt, &fixedUp);
    if (dragging)
    {
        //drag rotating
        D3DXVECTOR2 mousePosDetla(currentCursorPos-lastCursorPos);
        RotateCameraHorizontally(D3DX_PI/50*mousePosDetla.x);
        RotateCameraVertically(D3DX_PI/50*mousePosDetla.y);
    }

    //Update scene objects
    cube.Update(currentCursorPos, identity, fixedView, pGDevice->m_matCubeProj);
    axis.Update();
    skinMeshRenderer.Update(identity, _dt);
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
    skinMeshRenderer.Render(pGDevice->m_pD3DDevice, targetMesh, identity, view, proj, eyePoint);

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
    skinMeshRenderer.Destroy();

    GraphicsDevice::ReleaseInstance();
}
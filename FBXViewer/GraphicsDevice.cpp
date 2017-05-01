#include "stdafx.h"
#include "GraphicsDevice.h"

GraphicsDevice* GraphicsDevice::instance = NULL;

GraphicsDevice::GraphicsDevice(HWND hwnd): m_hWnd(hwnd)
{
    HRESULT hr =  S_OK;

    hr = CoInitialize(0);
    DebugAssert(SUCCEEDED(hr), "CoInitialize失败");

    //获取窗口大小
    GetWindowRect(m_hWnd, &WindowRect);

    //获取客户区大小
    GetClientRect(m_hWnd, &ClientRect);
    mClientSize.width = ClientRect.right - ClientRect.left;
    mClientSize.height = ClientRect.bottom - ClientRect.top;

    //获取Direct3D9接口
    m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
    DebugAssert(m_pD3D!=NULL, "Direct3DCreate9失败");

    m_d3ddm.Width = (UINT)mClientSize.width;     //屏幕高度和客户区高度一致
    m_d3ddm.Height = (UINT)mClientSize.height;   //屏幕宽度和客户区宽度一致
    ZeroMemory(&m_d3dpp, sizeof(D3DPRESENT_PARAMETERS));
    m_d3dpp.Windowed = TRUE;    //窗口模式

    //获取显示器的显示模式
    hr = m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &m_d3ddm);
    DebugAssert(SUCCEEDED(hr), "GetAdapterDisplayMode失败");

    //检查是否支持Z-Buffer
    hr = m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
                                          m_d3ddm.Format,
                                          D3DUSAGE_DEPTHSTENCIL,
                                          D3DRTYPE_SURFACE,
                                          D3DFMT_D24S8);
    DebugAssert(SUCCEEDED(hr), "显卡不支持Z-Buffer");

    // 检查是否支持multi-sampling
    D3DMULTISAMPLE_TYPE mst = D3DMULTISAMPLE_2_SAMPLES;
    DWORD quality = 0;
    // Is it supported for the back buffer format?
    if(SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(
        D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_A8R8G8B8,
        true, mst, &quality)))
    {
        // Is it supported for the depth format?
        if(SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType(
            D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_D24S8,
            true, mst, &quality)))
        {
            m_d3dpp.MultiSampleType = mst;
            m_d3dpp.MultiSampleQuality = quality - 1;
        }
    }

    m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;   //back buffer呈现之前将被噪声数据填充
    m_d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
    m_d3dpp.EnableAutoDepthStencil = TRUE;
    m_d3dpp.AutoDepthStencilFormat  = D3DFMT_D24S8;

    //创建设备接口IDirect3DDevice9
    hr = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL, m_hWnd,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &m_d3dpp, &m_pD3DDevice);
    DebugAssert(SUCCEEDED(hr), "CreateDevice失败");

    //设置viewport
    //默认设置viewport为整个客户区
    mDefaultViewport.X = 0;
    mDefaultViewport.Y = 0;
    mDefaultViewport.Width = mClientSize.width;
    mDefaultViewport.Height = mClientSize.height;
    mDefaultViewport.MinZ = 0;
    mDefaultViewport.MaxZ = 1;
    hr = m_pD3DDevice->SetViewport(&mDefaultViewport);
    DebugAssert(SUCCEEDED(hr), "SetViewport失败");

    //填充图形
    hr = m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    DebugAssert(SUCCEEDED(hr), "SetRenderState D3DRS_FILLMODE D3DFILL_SOLID失败");
}

GraphicsDevice::~GraphicsDevice(void)
{
    CoUninitialize();
}

void GraphicsDevice::BuildViewports()
{
    //设置Axis显示区域
    mAxisViewport.X = 20;
    mAxisViewport.Y = mClientSize.height - 120;
    mAxisViewport.Width = 100;
    mAxisViewport.Height = 100;
    mAxisViewport.MinZ = 0.0f;
    mAxisViewport.MaxZ = 1.0f;
    D3DXMatrixPerspectiveFovLH(&m_matAxisProj, D3DX_PI / 4.0f,
        (float)mAxisViewport.Width / (float)mAxisViewport.Height, 1.0f, 1000.0f);

    //设置Cube显示区域
    mCubeViewport.X = mClientSize.width - 220;
    mCubeViewport.Y = 20;
    mCubeViewport.Width = 200;
    mCubeViewport.Height = 200;
    mCubeViewport.MinZ = 0.0f;
    mCubeViewport.MaxZ = 1.0f;
    D3DXMatrixPerspectiveFovLH(&m_matCubeProj, D3DX_PI / 4.0f,
        (float)mCubeViewport.Width / (float)mCubeViewport.Height, 1.0f, 1000.0f);
}

//清除视区
void GraphicsDevice::Clear()
{
	HRESULT hr = m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(255,200,200,200), 1.0f, 0);
    DebugAssert(SUCCEEDED(hr), "Clear失败");
}

void GraphicsDevice::BeginScene()
{
    //开始场景
    HRESULT hr = m_pD3DDevice->BeginScene();
    DebugAssert(SUCCEEDED(hr), "BeginScene失败");
}

void GraphicsDevice::EndScene()
{
    //结束场景
    HRESULT hr = m_pD3DDevice->EndScene();
    DebugAssert(SUCCEEDED(hr), "EndScene失败");
}

void GraphicsDevice::Present()
{
    //显示场景
    HRESULT hr = m_pD3DDevice->Present(NULL, NULL, NULL, NULL);
    DebugAssert(SUCCEEDED(hr), "Present失败");
}

void GraphicsDevice::SetViewport( const D3DVIEWPORT9& viewport )
{
    HRESULT hr = m_pD3DDevice->SetViewport(&viewport);
    DebugAssert(SUCCEEDED(hr), "SetViewport失败");
}

void GraphicsDevice::ResetViewport()
{
    HRESULT hr = m_pD3DDevice->SetViewport(&mDefaultViewport);
    DebugAssert(SUCCEEDED(hr), "SetViewport失败");
}



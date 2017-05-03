#pragma once
#include "stdafx.h"

class GraphicsDevice
{
private:
    GraphicsDevice(void){}
    void operator = (GraphicsDevice&) = delete;

    static GraphicsDevice* instance;
    RECT WindowRect;
    RECT ClientRect;
    HWND m_hWnd;

public:
    GraphicsDevice(HWND hwnd);
    ~GraphicsDevice(void);
    static GraphicsDevice* getInstance(HWND hwnd)   //创建图形设备时
    {
        if(instance == NULL)
        {
            instance = new GraphicsDevice(hwnd);
        }
        return instance;
    }
    static GraphicsDevice* getInstance()    //后续访问图形设备
    {
        return instance;
    }
    static void ReleaseInstance()
    {
        SAFE_RELEASE(instance->m_pD3DDevice);
        SAFE_RELEASE(instance->m_pD3D);
        delete instance;
    }

    struct ClientSize{LONG width; LONG height;} mClientSize;
    D3DVIEWPORT9 mDefaultViewport;
    D3DVIEWPORT9 mAxisViewport, mCubeViewport;

    D3DXMATRIX m_matAxisProj, m_matCubeProj;

    D3DDISPLAYMODE m_d3ddm;
    D3DPRESENT_PARAMETERS m_d3dpp;

    IDirect3D9* m_pD3D;
    IDirect3DDevice9* m_pD3DDevice;
    void BuildViewports();
    void SetViewport(const D3DVIEWPORT9& viewport);
    void ResetViewport();

    void Clear();
    void BeginScene();
    void EndScene();
    void Present();

};

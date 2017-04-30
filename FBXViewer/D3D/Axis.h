#pragma once
#include "SMesh.h"

namespace StaticMesh
{
    class AxisMesh : public SMesh
    {
    private:
        AxisMesh(AxisMesh&);
        void operator = (AxisMesh&);
        //XYZ Quad Mesh
        PTVertex XQuad[6];
        PTVertex YQuad[6];
        PTVertex ZQuad[6];
        //XYZ Quad Texture
        IDirect3DTexture9* pXTexture;
        IDirect3DTexture9* pYTexture;
        IDirect3DTexture9* pZTexture;
    public:
        std::vector<PCVertex> mVerticesData;    //顶点数据

        AxisMesh(void);
        ~AxisMesh(void);

        virtual void SetVertexData();

        void CreateXYZ(IDirect3DDevice9* pDevice);
        void UpdateXYZ(const D3DXVECTOR3& eyePoint);
        void DrawXYZ();
        void DestroyXYZ();
    };
}
#pragma once
#include <vector>
#include "fbxsdk.h"
#include "FbxExtractor.h"
#include "engine\render\render_pipleline.h"
#include "engine\entity\entity_skinmesh.h"
#include "engine\entity\entity_staticmesh.h"

struct SkeletonVertex
{
    D3DXVECTOR3 Pos;
    D3DXCOLOR	Color;

    static const GX_VERTEX_BIND VtxBind[2];
    SkeletonVertex(D3DXVECTOR3 pos, D3DXCOLOR col)
    {
        Pos = pos;
        Color = col;
    }
};

class ConstBinder
{
    // global constant
    IGXConst* hViewport; // x = width of Viewport, y = height of Viewport
    IGXConst* hMatWorld; // World transform matrix
    IGXConst* hMatViewProj; // View * Proj transform matrix
public:
    ConstBinder()
    {
        hViewport = NULL;
    }

    void ApplyEnv(const GX::RenderEnv* _pEnv)
    {
        GX::Vec4 v4;
        if( hViewport )
        {
            int _x = 0, _y = 0;
            unsigned int _w = 0, _h = 0;
            GetDevice()->GetViewport(_x, _y, _w, _h);
            v4.Set((float)_x, (float)_y, (float)_w, (float)_h);
            hViewport->SetVector( v4, 1);
        }
        if (hMatWorld)
        {
            hMatWorld->SetMatrix(GX::Mat44::IDENTITY, 1, false);
        }
        if (hMatViewProj)
        {
            hMatWorld->SetMatrix(_pEnv->ViewProjMat, 1, false);
        }
    }

    void ApplyInst()
    {

    }

    void Bind( IGXShader* _pShader )
    {
        hViewport = _pShader->GetConst("Viewport");
        hMatWorld = _pShader->GetConst("MatWorld");
        hMatViewProj = _pShader->GetConst("MatViewProj");
    }
};

#define USE_STATICMESH
#undef USE_STATICMESH
class ContentCache
{
private:
    ContentCache(ContentCache&);
    void operator = (ContentCache&);

public:
    FbxExtractor* extracter;
#ifdef USE_STATICMESH
    std::vector<GX::Entity_StaticMesh*> Meshes;
#else
    std::vector<GX::Entity_SkinMesh*> Meshes;
#endif
    IGXVB* pVB;
    IGXShader* pShader;
    ConstBinder constBinder;
    IGXRenderState* pRS;
    std::vector<SkeletonVertex> SkeletonPositons;

    bool Ready;

    ContentCache(void);

    ~ContentCache(void);

    bool Create(const char* szFbxSrc);

    void Regist(GX::RenderPipleline& render);

    void Draw(const GX::RenderEnv* _pEnv);

    void Destroy();
};




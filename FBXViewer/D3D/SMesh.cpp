#include "stdafx.h"
#include "SMesh.h"


namespace StaticMesh
{
D3DVERTEXELEMENT9 PCDecl[] = {
    {0, offsetof(PCVertex,Pos), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
    {0, offsetof(PCVertex,Color), D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
    D3DDECL_END()
};

D3DVERTEXELEMENT9 PTDecl[] = {
    {0, offsetof(PTVertex,Pos), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
    {0, offsetof(PTVertex,TC), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
    D3DDECL_END()
};

D3DVERTEXELEMENT9 PCTDecl[] = {
    {0, offsetof(PCTVertex,Pos), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
    {0, offsetof(PCTVertex,Color), D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
    {0, offsetof(PCTVertex,TC), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
    D3DDECL_END()
};


SMesh::SMesh( void ) : mPrimitiveType(D3DPT_TRIANGLELIST), pVB(0),pVD(0),pIB(0),pTexture(0),pVS(0),pPS(0),pCTVS(0),pCTPS(0)
{
    material.DiffuseMap = "<nodiffusemap>";
}

SMesh::~SMesh( void )
{

}

void SMesh::SetConstants( IDirect3DDevice9* pDevice,
    const D3DXMATRIX& matWorld, const D3DXMATRIX& matView, const D3DXMATRIX& matProj)
{
    //common
    {
        //matWorld matView matProj
        D3DXHANDLE hMatWorld = pCTVS->GetConstantByName(NULL, "matWorld");
        D3DXHANDLE hMatView = pCTVS->GetConstantByName(NULL, "matView");
        D3DXHANDLE hMatProj = pCTVS->GetConstantByName(NULL, "matProj");

        pCTVS->SetMatrix(pDevice, hMatWorld, &matWorld);
        pCTVS->SetMatrix(pDevice, hMatView, &matView);
        pCTVS->SetMatrix(pDevice, hMatProj, &matProj);
    }
    //vertex shader constants
    //none

    //pixel shader constants
    //none
}

bool SMesh::Create( IDirect3DDevice9* pDevice )
{
    //检查数据，若不通过说明SMesh::SetVertexData()没有设置好数据
    switch(mVertexType)
    {
    case PC:
        DebugAssert(Positions.size()!=0, "缺少位置数据\n");
        DebugAssert(Colors.size()!=0, "缺少颜色数据\n");
        break;
    case PT:
        DebugAssert(Positions.size()!=0, "缺少位置数据\n");
        DebugAssert(UVs.size()!=0, "缺少纹理坐标数据\n");
        break;
    case PCT:
        DebugAssert(Positions.size()!=0, "缺少位置数据\n");
        DebugAssert(Colors.size()!=0, "缺少颜色数据\n");
        DebugAssert(UVs.size()!=0, "缺少纹理坐标数据\n");
        break;
    default:
        DebugAssert(false, "不可能的顶点类型\n");
        break;
    }
    switch(mPrimitiveType)
    {
    case D3DPT_LINELIST:
        DebugAssert(IndexBuf.size() == nPrimitive*2, "索引数据有误\n");
        break;
    case D3DPT_TRIANGLELIST:
        DebugAssert(IndexBuf.size() == nPrimitive*3, "索引数据有误\n");
    default:
        break;
    }

    HRESULT hr = S_FALSE;
    //skinned mesh
    {
        //index
        {
            UINT lSize = 0;
            switch(mPrimitiveType)
            {
            case D3DPT_LINELIST:
                lSize = nPrimitive*2*sizeof(WORD);
                break;
            case D3DPT_TRIANGLELIST:
                lSize = nPrimitive*3*sizeof(WORD);
            default:
                break;
            }
            V(pDevice->CreateIndexBuffer(lSize, D3DUSAGE_WRITEONLY,
                D3DFMT_INDEX16, D3DPOOL_MANAGED, &pIB, 0));
            WORD* data = 0;
            V(pIB->Lock(0, 0, (void**)&data, 0));
            memcpy(data, &IndexBuf[0], lSize);
            V(pIB->Unlock());
        }
        //vertex
        {
            DebugAssert(Positions.size()==nVertex, "nVertex和Positon的数量不符合\n");
            UINT lSize = 0;
            switch(mVertexType)
            {
            case PC:
                {
                    lSize = nVertex * sizeof(PCVertex);
                    V(pDevice->CreateVertexBuffer(lSize, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &pVB, 0));
                    PCVertex* v = 0;
                    V(pVB->Lock(0, 0, (void**)&v, 0));
                    for(DWORD i = 0; i < nVertex; ++i)
                    {
                        v[i].Pos = Positions.at(i);
                        v[i].Color = Colors.at(i);
                    }
                    V(pVB->Unlock());
                }
                break;
            case PT:
                {
                    lSize = nVertex * sizeof(PTVertex);
                    V(pDevice->CreateVertexBuffer(lSize, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &pVB, 0));
                    PTVertex* v = 0;
                    V(pVB->Lock(0, 0, (void**)&v, 0));
                    for(DWORD i = 0; i < nVertex; ++i)
                    {
                        v[i].Pos = Positions.at(i);
                        v[i].TC = UVs.at(i);
                    }
                    V(pVB->Unlock());
                }
                break;
            case PCT:
                {
                    lSize = nVertex * sizeof(PCTVertex);
                    V(pDevice->CreateVertexBuffer(lSize, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &pVB, 0));
                    PCTVertex* v = 0;
                    V(pVB->Lock(0, 0, (void**)&v, 0));
                    for(DWORD i = 0; i < nVertex; ++i)
                    {
                        v[i].Pos = Positions.at(i);
                        v[i].Color = Colors.at(i);
                        v[i].TC = UVs.at(i);
                    }
                    V(pVB->Unlock());
                }
                break;
            default:
                break;
            }
        }
        //vertex decl
        {
            switch(mVertexType)
            {
            case PC:
                V(pDevice->CreateVertexDeclaration( PCDecl , &pVD ));
                break;
            case PT:
                V(pDevice->CreateVertexDeclaration( PTDecl , &pVD ));
                break;
            case PCT:
                V(pDevice->CreateVertexDeclaration( PCTDecl , &pVD ));
                break;
            default:
                break;
            }
        }

        //material: diffuse texture
        {
            if (material.DiffuseMap!="<nodiffusemap>")
            {
                hr =D3DXCreateTextureFromFileEx(pDevice, material.DiffuseMap.c_str(),
                    0, 0, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
                    NULL, NULL, &pTexture);
                if (FAILED(hr))
                {
                    DebugPrintf("Diffuse texture: %s 创建失败\n", material.DiffuseMap.c_str());
                }
            }
            else//创建1×1的Texture
            {
                hr = pDevice->CreateTexture( 1 , 1 , 0 , 0 , D3DFMT_A8R8G8B8 , D3DPOOL_MANAGED , &pTexture , NULL ); 
                D3DLOCKED_RECT lr;
                hr = pTexture->LockRect( 0 , &lr , 0 , D3DLOCK_NOSYSLOCK );
                if( SUCCEEDED( hr ) )
                {
                    *(DWORD*)lr.pBits = MAKEFOURCC( rand() & 255 , rand() & 255 , rand() & 255 , 255 );
                    hr = pTexture->UnlockRect( 0 );
                }
                else
                {
                    DebugPrintf("1×1 Dummy texture 创建失败\n");
                }
            }
        }

        //shader
        char* vertexShaderPathForPC = "shader/static.vsh";
        char* pixelShaderPathForPC = "shader/static.psh";
        char* vertexShaderPathForPT = "shader/staticTextured.vsh";
        char* pixelShaderPathForPT = "shader/staticTextured.psh";
        char* vertexShaderPathForPCT = "shader/staticColorTextured.vsh";
        char* pixelShaderPathForPCT = "shader/staticColorTextured.psh";
        char* vertexShaderPath = NULL;
        char* pixelShaderPath = NULL;
        switch(mVertexType)
        {
        case PC:
            vertexShaderPath = vertexShaderPathForPC;
            pixelShaderPath = pixelShaderPathForPC;
            break;
        case PT:
            vertexShaderPath = vertexShaderPathForPT;
            pixelShaderPath = pixelShaderPathForPT;
            break;
        case PCT:
            vertexShaderPath = vertexShaderPathForPCT;
            pixelShaderPath = pixelShaderPathForPCT;
            break;
        default:
            break;
        }
        ID3DXBuffer* errorBuf;
        ID3DXBuffer* ShaderBuf;
        //vertex shader
        hr = D3DXCompileShaderFromFile(vertexShaderPath, NULL, NULL, "main", "vs_3_0", NULL,  &ShaderBuf, &errorBuf, &pCTVS);
        if ( FAILED( hr ) && errorBuf != 0 && errorBuf->GetBufferPointer() != 0 )
        {
            OutputDebugStringA((char*)errorBuf->GetBufferPointer());
            SAFE_RELEASE(ShaderBuf);
            SAFE_RELEASE(errorBuf);
            return false;
        }
        V(pDevice->CreateVertexShader((DWORD*)ShaderBuf->GetBufferPointer(), &pVS));
        SAFE_RELEASE(ShaderBuf);
        SAFE_RELEASE(errorBuf);

        //pixel shader
        hr = D3DXCompileShaderFromFile(pixelShaderPath, NULL, NULL, "main", "ps_3_0", NULL,  &ShaderBuf, &errorBuf, &pCTPS);
        if ( FAILED( hr ) && errorBuf != 0 && errorBuf->GetBufferPointer() != 0 )
        {
            OutputDebugStringA((char*)errorBuf->GetBufferPointer());
            SAFE_RELEASE(ShaderBuf);
            SAFE_RELEASE(errorBuf);
            return false;
        }
        V(pDevice->CreatePixelShader((DWORD*)ShaderBuf->GetBufferPointer(), &pPS));
        SAFE_RELEASE(ShaderBuf);
        SAFE_RELEASE(errorBuf);
    }
    return true;
}

void SMesh::Draw( IDirect3DDevice9* pDevice)
{
    HRESULT hr = S_FALSE;
    pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

    V(pDevice->SetVertexShader(pVS));
    V(pDevice->SetPixelShader(pPS));

    V(pDevice->SetVertexDeclaration(pVD));
    V(pDevice->SetIndices(pIB));

    switch(mVertexType)
    {
    case PC:
        V(pDevice->SetStreamSource(0, pVB, 0, sizeof(PCVertex)));
        break;
    case PT:
        V(pDevice->SetStreamSource(0, pVB, 0, sizeof(PTVertex)));
        break;
    case PCT:
        V(pDevice->SetStreamSource(0, pVB, 0, sizeof(PCTVertex)));
        break;
    default:
        break;
    }
    if (material.DiffuseMap!="<nodiffusemap>")
    {
        V(pDevice->SetTexture(0, pTexture));
    }
    V(pDevice->DrawIndexedPrimitive(mPrimitiveType, 0, 0, nVertex, 0, nPrimitive));

    V(pDevice->SetVertexShader( NULL ));
    V(pDevice->SetPixelShader( NULL ));

}

void SMesh::Destroy()
{
    SAFE_RELEASE(pVB);
    SAFE_RELEASE(pVD);
    SAFE_RELEASE(pIB);
    SAFE_RELEASE(pTexture);

    SAFE_RELEASE(pVS);
    SAFE_RELEASE(pPS);
    SAFE_RELEASE(pCTVS);
    SAFE_RELEASE(pCTPS);
}

bool SMesh::Update()
{
    return true;
}

}
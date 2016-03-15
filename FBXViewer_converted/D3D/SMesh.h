#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <vector>


namespace StaticMesh
{
    //顶点定义类型
    typedef enum {PC, PT, PCT} VertexType;

    //顶点定义
    struct PCVertex //PC
    {
        D3DXVECTOR3 Pos;
        D3DCOLOR    Color;
    };

    struct PTVertex //PT
    {
        D3DXVECTOR3 Pos;
        D3DXVECTOR2 TC;
    };

    struct PCTVertex //PCT
    {
        D3DXVECTOR3 Pos;
        D3DCOLOR    Color;
        D3DXVECTOR2 TC;
    };

    class SMesh
    {
    private:
        SMesh(SMesh&);
        void operator = (SMesh&);

        struct Matrial
        {
            std::string DiffuseMap; //漫反射贴图（纹理贴图）路径
        };

        IDirect3DIndexBuffer9*          pIB;
        IDirect3DVertexDeclaration9*    pVD;
        IDirect3DTexture9*              pTexture;

        IDirect3DVertexShader9*         pVS;
        IDirect3DPixelShader9*          pPS;
        ID3DXConstantTable*             pCTVS;
        ID3DXConstantTable*             pCTPS;

    public:
        IDirect3DVertexBuffer9*         pVB;
        D3DPRIMITIVETYPE mPrimitiveType;
        unsigned int nVertex;
        unsigned int nPrimitive;

        std::vector<WORD>           IndexBuf;  // index
        std::vector<D3DXVECTOR3>    Positions; // positions
        std::vector<D3DCOLOR>       Colors;    // color
        std::vector<D3DXVECTOR2>    UVs;       // texture coordinates

        //材质
        Matrial material;

        SMesh(void);
        virtual ~SMesh(void);

        std::string mName;

        VertexType mVertexType;

        virtual void SetVertexData(int meshType) = 0;

        void SetConstants( IDirect3DDevice9* pDevice,
            const D3DXMATRIX& matWorld, const D3DXMATRIX& matView, const D3DXMATRIX& matProj);

        bool Create(IDirect3DDevice9* pDevice);

        virtual bool Update();

        void Draw(IDirect3DDevice9* pDevice);

        void Destroy();

    };


}

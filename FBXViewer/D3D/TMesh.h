#pragma once
#include <d3d9.h>
#include <vector>

//顶点定义
struct Vertex
{
    D3DXVECTOR3 Pos;    //0 + 12
    D3DXVECTOR3 Normal; //12 + 12
    D3DXVECTOR2 TC;     //24 + 8
    D3DXVECTOR4 BoneIndices;    //32 + 16
    D3DXVECTOR3 BoneWeights;    //48 + 12
};

class Skeleton;
class TMesh
{
private:
    TMesh(TMesh&);
    void operator = (TMesh&);

    struct Matrial
    {
        D3DXCOLOR Diffuse;
        D3DXCOLOR Ambient;
        D3DXCOLOR Specular;

        std::string DiffuseMap; //漫反射贴图（纹理贴图）路径
    };

    //光照相关
    D3DXCOLOR Ambient;      //环境光颜色
    D3DXCOLOR Diffuse;      //散射光颜色
    D3DXCOLOR Specular;     //镜面光颜色
    float SpecularPower;    //镜面光指数

    D3DXVECTOR3 LightPos;       //点光源位置
    D3DXVECTOR3 Attenuation012; //光照衰减系数


    D3DXVECTOR3 EyePosition;

    IDirect3DIndexBuffer9*          pIB;
    IDirect3DVertexBuffer9*         pVB;
    IDirect3DVertexDeclaration9*    pVD;
    IDirect3DTexture9*              pTexture;

    IDirect3DVertexShader9*         pVS;
    IDirect3DPixelShader9*          pPS;
    ID3DXConstantTable*             pCTVS;
    ID3DXConstantTable*             pCTPS;

public:
    bool m_bStatic; //是否为静态mesh，注：静态mesh无骨骼（skinning）信息
    unsigned int nVertices;
    unsigned int nFaces;
    std::vector<WORD> IndexBuf;             // index

    std::vector<D3DXVECTOR3> Positions;     //positions
    std::vector<D3DXVECTOR2> UVs;           //texture coordinates (only one now)
    std::vector<D3DXVECTOR3> Normals;       //normals
    std::vector<D3DXVECTOR3> Tangents;      //tangents
    std::vector<D3DXVECTOR3> Binormals;     //binormals

    //蒙皮骨骼信息
    std::vector<D3DXVECTOR4> BoneIndices;   //骨头索引
    std::vector<D3DXVECTOR3> BoneWeights;   //骨骼权重，第4个权重由1-前3个得到

    //材质
    Matrial material;

    TMesh(void);
    ~TMesh(void);

    std::string mName;

    void SetSkinnedConstants( IDirect3DDevice9* pDevice,
        const D3DXMATRIX& matWorld, const D3DXMATRIX& matViewProj, const D3DXVECTOR3& eyePoint,
        const D3DXMATRIX* matBone, unsigned int nBones);

    bool Create(IDirect3DDevice9* pDevice);

    bool Update(IDirect3DDevice9* pDevice, const D3DXMATRIX* matBone, unsigned int nBones);

    void Draw(IDirect3DDevice9* pDevice);

    void Destroy();
    
    /*
        默认情况下TMesh获得的模型是在右手系下、Z向上、Y向内、X向右的（和3DSMAX中相同）
        Convert方法对TMesh进行转换，使其转换为在左手系下、Y向上、Z向内、X向右的（和D3D默认的相同）
        注意：①现在（2014年9月16日10:07:01）仅针对静态模型，而且仅对顶点坐标和索引进行变换，
              ②转换后Normal、Tangent、Binormal已失去意义，因为模型的顶点坐标发生了变化，所以需要重新计算
              ③对UVs、BoneIndices、BoneWeights没有影响
    */
    enum MeshType{RIGHTHANDED_ZUP, LEFTHANDED_YUP} mMeshType;
    void Convert(MeshType targetType = LEFTHANDED_YUP);

    //Debug functions
    void Dump(unsigned int n);
};


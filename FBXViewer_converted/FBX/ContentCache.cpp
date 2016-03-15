#include "stdafx.h"
#include <stddef.h>
#include <vector>


#include "ContentCache.h"
#include "FBXCommon.h"
#include "FbxExtractor.h"
#include "PGE_File.h"

const char* EXPORT_FILE_PATH = "user\\fbx\\1_Exported.fbx";
const char* TEXTURE_FILE_PATH = "user\\textures\\br_agathion_tiger_grandma_sp_rotated.jpg";

const GX_VERTEX_BIND SkeletonVertex::VtxBind[2] = 
{
    { 0 , sizeof(SkeletonVertex), offsetof( SkeletonVertex , Pos ) , GX_FLOAT3 , GX_POSITION0 },
    { 0 , sizeof(SkeletonVertex), offsetof( SkeletonVertex , Color) , GX_FLOAT4 , GX_COLOR0 },
};

const static D3DVERTEXELEMENT9 SkeletonDecl[] =
{
    { 0 , offsetof( SkeletonVertex , Pos )  , D3DDECLTYPE_FLOAT3 , D3DDECLMETHOD_DEFAULT , D3DDECLUSAGE_POSITION , 0 } ,
    { 0 , offsetof( SkeletonVertex , Color ), D3DDECLTYPE_FLOAT4 , D3DDECLMETHOD_DEFAULT , D3DDECLUSAGE_COLOR	 , 0 } ,
    D3DDECL_END()
};


ContentCache::ContentCache()
{
    extracter = NULL;

    Meshes.resize(0);

    pVB = NULL;
    pShader = NULL;
    pRS = NULL;

    Ready = false;
}

ContentCache::~ContentCache()
{
    Destroy();
}

bool ContentCache::Create(const char* szFbxSrc)
{
    IGXDevice* pDev = GetDevice();

    pShader = GX::GetShader( "ContentCache.vs" , "ContentCache.ps" );
    if( pShader )
        constBinder.Bind( pShader );

    pRS = GX::GetRenderState( "ContentCache.rs" );

    if( pShader == NULL || pRS == NULL )
        return false;


    //从FBX文件中提取信息，准备PMF/PSF文件数据
    extracter = new FbxExtractor();

    //写入PMF/PSF文件
    const char* resourcePath = GetResPath();
    std::string destAbsPath;
    std::string characterMeshPath = "D:\\char\\char\\Bin\\resource\\cha\\special\\ttt\\mesh\\";
    std::string characterSkeletonPath = "D:\\char\\char\\Bin\\resource\\cha\\special\\ttt\\skeleton\\";
    // save skeleton to psf file
    const char* destPSF = "dest.psf";   //destination psf file path
    destAbsPath = resourcePath; destAbsPath.append(destPSF);
    CreatePSFFile(extracter->m_pSkeleton, destAbsPath.c_str());
    CopyFile(destAbsPath.c_str(), (characterSkeletonPath + "dest.psf").c_str(), FALSE);

    // save mesh to pmf file
    unsigned int pmfFileCount = extracter->MeshFiles.size();
    for (unsigned int i=0; i<pmfFileCount; i++)
    {
        const PMF_File& pmfFile = *extracter->MeshFiles.at(i);
        destAbsPath = resourcePath + pmfFile.name + ".pmf";  //将pmf文件存储在资源文件夹根目录
        CreatePMFFile(pmfFile.data, pmfFile.head, destAbsPath.c_str());
        CopyFile(destAbsPath.c_str(), (characterMeshPath+pmfFile.name+".pmf").c_str(), FALSE);
        //Create GX Skinned Mesh from pmf file

#ifdef USE_STATICMESH
        GX::Entity_StaticMesh* pMesh = new GX::Entity_StaticMesh;
        pMesh->WMat.Identity(); //对于Entity_StaticMesh，若不设置，绘制出的mesh会乱七八糟
#else
        GX::Entity_SkinMesh* pMesh = new GX::Entity_SkinMesh;
#endif
        bool bResult = pMesh->Create(pmfFile.name + ".pmf", 0, "test/dest_mtl.json", destPSF);
        //Meshes.push_back(pMesh);
    }
    
    // set Skeleton data for testing
    {
        unsigned int nBones = extracter->m_pSkeleton.NumBones();
        for (unsigned int i=0; i<nBones; i++)
        {
            D3DXMATRIX& w1 = extracter->m_pSkeleton.GetBoneMatrix(i);
            D3DXMATRIX& w2 = extracter->m_pSkeleton.GetParentBoneMatrix(i);
    
            if (D3DXMatrixIsIdentity(&w2))  //index为i的bone为根骨骼
            {
                continue;
            }
    
            //Extract translation
            D3DXVECTOR3 thisBone = D3DXVECTOR3(w1(3, 0), w1(3, 1), w1(3, 2));
            D3DXVECTOR3 ParentBone = D3DXVECTOR3(w2(3, 0), w2(3, 1), w2(3, 2));
    
            if(D3DXVec3Length(&(thisBone - ParentBone)) < 100.0f)
            {
                SkeletonPositons.push_back(SkeletonVertex(ParentBone, 0xffff0000));
                SkeletonPositons.push_back(SkeletonVertex(thisBone,   0xff00ff00));
            }
        }
        pVB = pDev->CreateVB(&SkeletonPositons[0], sizeof(SkeletonVertex)*SkeletonPositons.size());
        DebugAssert(pVB!=NULL, "创建VB失败\n");
    }

    SAFE_DELETE(extracter);
    Ready = true;
    return true;
}

void ContentCache::Regist(GX::RenderPipleline& render)
{
    unsigned int meshCount = Meshes.size();
    for (unsigned int i=0; i<meshCount; i++)
    {
        render.Regist(Meshes.at(i), 0);
    }
}

void ContentCache::Draw(const GX::RenderEnv* _pEnv)
{
    IGXDevice* pDev = GetDevice();

    pShader->Apply();
    constBinder.ApplyEnv(_pEnv);
    pRS->Apply();
    unsigned int PrimitiveCount = SkeletonPositons.size();

    constBinder.ApplyInst();
    const UInt32 nVtxBind = sizeof( SkeletonVertex::VtxBind ) / sizeof( SkeletonVertex::VtxBind[0] );
    DebugAssert(2 == nVtxBind, "nVtxBind!=2\n");
    GX_VERTEX_BIND vtxbind[2];
    memcpy(vtxbind, SkeletonVertex::VtxBind, sizeof( SkeletonVertex::VtxBind));
    vtxbind[0].pVB = vtxbind[1].pVB = pVB;
    pShader->BindVertexBuffer(vtxbind, nVtxBind);
    pDev->DrawPrimitive(GX_LINES, PrimitiveCount);

}

void ContentCache::Destroy()
{
    unsigned int meshCount = Meshes.size();
    for (unsigned int i=0; i<meshCount; i++)
    {
        SAFE_DELETE(Meshes.at(i));
    }
    Meshes.resize(0);

    SAFE_RELEASE(pVB);
}

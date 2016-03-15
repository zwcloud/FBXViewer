#pragma once
#include <vector>
#include <map>
#include <Windows.h>
#include <fbxsdk.h>
#include <d3d9.h>
#include <d3dx9.h>

class TMesh;
class Skeleton;
class Animation;
/*
一个fbx文件包含
1个或n个Mesh、仅1个骨骼框架、仅1个动画序列
*/
class FbxExtractor
{
public:
    FbxExtractor(const char* src);
    ~FbxExtractor();

    inline std::vector<TMesh*> GetMeshes() const {return Meshes;}

    inline Skeleton* GetSkeleton() const {return m_pSkeleton;}

    inline Animation* GetAnimation() const {return m_pAnimation;}

private:
    //Mesh 列表
    std::vector<TMesh*> Meshes;
    std::vector<FbxMesh*> FbxMeshes;

    //唯一的骨骼
    Skeleton* m_pSkeleton;

    //动画数据
    Animation* m_pAnimation;

    // FBX SDK objects
    FbxScene* fbxScene;
    FbxManager* lSdkManager;

private:
    FbxExtractor( const FbxExtractor& );
    FbxExtractor& operator = (const FbxExtractor& );
    //Mesh
    bool isUVExist(const D3DXVECTOR2& uv);
    bool SplitVertexForUV(TMesh* pMesh);
    TMesh* ExtractStaticMesh(FbxMesh* lMesh);
    bool ExtractWeight(TMesh* pMesh, FbxMesh* pFbxMesh);
    //Bone
    void ExtractHierarchy();
    void ExtractNode(FbxNode* pNode, int parentID);
    int ExtractBone(FbxNode* pNode, int parentID);
    void ComputeSkeletonMatrix();
    //Animation
    void ExtractAnimation();
    void ExtractCurve(unsigned int boneIndex, FbxAnimLayer* pAnimLayer);

    void DumpBone(unsigned int boneID, bool printT, bool printR) const;
};
#pragma once
#include <vector>
#include <map>
#include <Windows.h>
#include <fbxsdk.h>
#include <d3d9.h>
#include <d3dx9.h>

class Mesh;
class Bone;
class Skeleton;
class Animation;
/*
一个fbx文件包含
1个或n个Mesh、仅1个骨骼框架、仅1个动画序列
*/
class FbxExtractor
{
public:
    FbxExtractor();
    ~FbxExtractor();

    void DoExtract(const char * src);

    inline std::vector<Mesh*> GetMeshes() const {return Meshes;}

    inline Skeleton* GetSkeleton() const {return m_pSkeleton;}

    inline Animation* GetAnimation() const {return m_pAnimation;}

private:
    //Mesh 列表
    std::vector<Mesh*> Meshes;
    std::map<Mesh*, FbxMesh*> FbxMeshMap;
    std::map<Bone*, FbxNode*> FbxNodeMap;

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
    bool SplitVertexForUV(Mesh* pMesh);
    Mesh* ExtractStaticMesh(FbxMesh* lMesh);
    bool ExtractWeight(Mesh* pMesh, FbxMesh* pFbxMesh);
    //Bone
    void ExtractHierarchy();
    void ExtractNode(FbxNode* pNode, int parentID);
    Bone* ExtractBone(FbxNode* pNode, int parentID);
    //Animation
    void ExtractAnimation();
    void ExtractCurve(Bone* bone, FbxAnimLayer* pAnimLayer);

    void DumpBone(const Bone* bone, bool printT = true, bool printR = true, bool printS = true) const;
};
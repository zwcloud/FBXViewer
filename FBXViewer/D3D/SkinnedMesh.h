#pragma once
#include <vector>


class FbxExtractor;
class TMesh;
class Skeleton;
class Animation;

class SkinnedMesh
{
public:
    SkinnedMesh(void);
    ~SkinnedMesh(void);

    //根骨骼的世界坐标
    D3DXMATRIX mMatWorld;

    FbxExtractor* m_pFbxExtractor;
    std::vector<TMesh*> mMeshes;
    Skeleton* m_pSkeleton;
    Animation* m_pAnimation;

    //骨骼矩阵
    std::vector<D3DXMATRIX> mBoneCurrentMat;
    unsigned int m_nBone;

    void Load(const char* fbxSrc, IDirect3DDevice9* pDevice);

    void Update(D3DXMATRIX matWorld, unsigned int time);

    void Render(IDirect3DDevice9* pDevice,
        const D3DXMATRIX& matWorld,  const D3DXMATRIX& matView, const D3DXMATRIX& matProj, const D3DXVECTOR3& eyePoint);

    void Destroy();

private:
    void SetBoneMatPtr();

    void UpdateAnimation( D3DXMATRIX matWorld, unsigned int time );
    void SetPose( D3DXMATRIX matWorld, unsigned int time );
    void UpdateOffsetMatrix();

    //Debug
    struct SkeletonVertex
    {
        D3DXVECTOR3 Pos;
        D3DCOLOR Color;
        SkeletonVertex(){}
        SkeletonVertex(D3DXVECTOR3 pos, D3DCOLOR col)
        {
            Pos = pos;
            Color = col;
        }
    };

    D3DMATERIAL9 boneMeshMaterial;

    std::vector<SkeletonVertex> mBonePositions;
    void BuildBoneMesh();
    void RenderBoneMesh(IDirect3DDevice9* pDevice,
        const D3DXMATRIX& matWorld,  const D3DXMATRIX& matView, const D3DXMATRIX& matProj);
    void UpdateBoneMesh( const D3DXMATRIX& matWorld, unsigned int time );
};


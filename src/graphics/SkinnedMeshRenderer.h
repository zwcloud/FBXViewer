#pragma once
#include <vector>

class Skeleton;
class Animation;
class Material;
class Mesh;
class SkinnedMeshRenderer
{
public:
    SkinnedMeshRenderer(void);
    ~SkinnedMeshRenderer(void);

    Material* material;

    Skeleton* m_pSkeleton;
    Animation* m_pAnimation;

    std::vector<D3DXMATRIX> mBoneCurrentMat;
    unsigned int m_nBone;

    void Load(Skeleton* skeleton, Animation* animation, Material* material);

    void Update(unsigned int time);

    void Render(IDirect3DDevice9* pDevice,
        Mesh* mesh,
        const D3DXMATRIX& matWorld,  const D3DXMATRIX& matView, const D3DXMATRIX& matProj, const D3DXVECTOR3& eyePoint);

private:

    void SetBoneMatPtr();

    void UpdateAnimation(unsigned int time);

    #pragma region Debug
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
    void UpdateBoneMesh(unsigned int time );
    #pragma endregion
};

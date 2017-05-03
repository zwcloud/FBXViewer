#include "stdafx.h"
#include "SkinnedMeshRenderer.h"
#include "Mesh.h"
#include "Skeleton.h"
#include "Animation.h"
#include "common\Profile.h"
#include "Material.h"
#include "Vertex.h"
#include "RenderSettings.h"

SkinnedMeshRenderer::SkinnedMeshRenderer( void ) :
    m_pSkeleton(NULL),
    m_pAnimation(NULL),
    m_nBone(0)
{
    D3DXMatrixIdentity(&mMatWorld);

    ZeroMemory(&boneMeshMaterial, sizeof(D3DMATERIAL9));
    boneMeshMaterial.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    boneMeshMaterial.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
}

SkinnedMeshRenderer::~SkinnedMeshRenderer( void )
{

}

void SkinnedMeshRenderer::Destroy()
{
}

void SkinnedMeshRenderer::Load(Skeleton* skeleton, Animation* animation, Material* material)
{
    this->material = material;
    this->m_pSkeleton = skeleton;
    this->m_pAnimation = animation;
    SetBoneMatPtr();
    //Update(mMatWorld, 0U);
    BuildBoneMesh();
}

void SkinnedMeshRenderer::SetBoneMatPtr()
{
    unsigned int nBones = m_pSkeleton->NumBones();
    DebugAssert(nBones<=MAX_BONE_COUNT, "Number of bones exceeds 50.\n");
    mBoneCurrentMat.resize(nBones);
    m_nBone = nBones;
}

void SkinnedMeshRenderer::Update( D3DXMATRIX matWorld, unsigned int time )
{
    UpdateAnimation(matWorld, time);
    UpdateBoneMesh(matWorld, time);
}

void SkinnedMeshRenderer::UpdateAnimation( D3DXMATRIX matWorld, unsigned int time )
{
    SetPose(matWorld, time);
}

void SkinnedMeshRenderer::SetPose( D3DXMATRIX matWorld, unsigned int time )
{
    mMatWorld = matWorld;
    unsigned int lDuration = m_pAnimation->GetDuration();
    unsigned int currentFrame = (unsigned int)time%lDuration;
    //DebugPrintf("Frame count: %d | Duration: %d | Current animation frame: %d\n", time, lDuration, currentFrame);
    for (unsigned int i=0; i<m_nBone; i++)
    {
        Bone* pBone = m_pSkeleton->GetBone(i);
        D3DXMATRIX mat = m_pAnimation->GetFrame(i, currentFrame);
        memcpy(&pBone->matOffset, &mat, sizeof(D3DXMATRIX));   //get offsetMatrix at time 'time'
    }
}

void SkinnedMeshRenderer::Render(IDirect3DDevice9* pDevice,
    Mesh* mesh,
    const D3DXMATRIX& matWorld, const D3DXMATRIX& matView, const D3DXMATRIX& matProj, const D3DXVECTOR3& eyePoint)
{
    if (RenderSettings::getInstance().ShowMesh())
    {
        D3DXMATRIX matViewProj;
        D3DXMatrixMultiply(&matViewProj, &matView, &matProj);
        unsigned int nBones = m_pSkeleton->NumBones();
        //set current bone matrix
        for (unsigned int i=0; i<nBones; i++)
        {
            Bone* pBone = m_pSkeleton->GetBone(i);
            D3DXMATRIX matInverse;
            D3DXMatrixInverse(&matInverse, NULL, &pBone->matBone);
            D3DXMatrixMultiply(&mBoneCurrentMat[i], &matInverse, &pBone->matOffset);
        }
        //设置Constants，绘制Mesh
        MaterialUtil::SetConstants(pDevice, this->material, matWorld, matViewProj, eyePoint, &mBoneCurrentMat[0], m_nBone);
        {
            HRESULT hr = S_FALSE;

            IDirect3DTexture9*& texture = material->Texture();
            IDirect3DVertexShader9*& pVS = material->VS();
            IDirect3DPixelShader9*& pPS = material->PS();
            ID3DXConstantTable*& pCTVS = material->VSConstantTable();
            ID3DXConstantTable*& pCTPS = material->PSConstantTable();

            int nFaces = mesh->nFaces;
            int nVertices = mesh->nVertices;
            IDirect3DIndexBuffer9*& pIB = mesh->pIB;
            IDirect3DVertexBuffer9*& pVB = mesh->pVB;
            IDirect3DVertexDeclaration9*& pVD = mesh->pVD;

            /* 默认情况下Mesh模型是在右手系下、Z向上、Y向内、X向右的（和3DSMAX中相同）, 且以逆序作为正面 */
            pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

            #define USE_ALPHATEST
            #ifdef USE_ALPHATEST
            pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
            pDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)200);
            pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
            #else
            pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
            pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
            pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
            #endif

            V(pDevice->SetVertexShader(pVS));
            V(pDevice->SetPixelShader(pPS));

            V(pDevice->SetVertexDeclaration(pVD));
            V(pDevice->SetIndices(pIB));
            V(pDevice->SetStreamSource(0, pVB, 0, sizeof(Vertex)));
            V(pDevice->SetTexture(0, texture));
            V(pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, nVertices, 0, nFaces));

            V(pDevice->SetTexture(0, 0));

            #ifdef USE_ALPHATEST
            pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
            #else
            pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
            #endif
        }
    }

    if (RenderSettings::getInstance().ShowSkeleton())
    {
        RenderBoneMesh(pDevice, matWorld, matView, matProj);
    }
}

#pragma region Debug

void SkinnedMeshRenderer::BuildBoneMesh()
{
    unsigned int nBones = m_pSkeleton->NumBones();
    for (unsigned int i=0; i<nBones; i++)
    {
        Bone* pBone = m_pSkeleton->GetBone(i);
        int lParentBoneID = pBone->mParentId;
        if (lParentBoneID == -1)    //index为i的bone为根骨骼
        {
            //D3DXMATRIX& w = pBone->matBone*pBone->matOffset;
            //DebugPrintf("Root Bone \"%s\" positon at time %d : (%.3f, %.3f, %.3f)\n",
            //    pBone->mName.c_str(), time,
            //    w._41, w._42, w._43);
            continue;
        }
        Bone* pParentBone = m_pSkeleton->GetBone(lParentBoneID);
        D3DXMATRIX& w1 = pBone->matBone;
        D3DXMATRIX& w2 = pParentBone->matBone;

        //Extract translation
        D3DXVECTOR3 thisBone = D3DXVECTOR3(w1(3, 0), w1(3, 1), w1(3, 2));
        D3DXVECTOR3 ParentBone = D3DXVECTOR3(w2(3, 0), w2(3, 1), w2(3, 2));

        D3DXVECTOR3 offset = (thisBone - ParentBone);
        if(D3DXVec3Length(&offset) < 100.0f)
        {
            mBonePositions.push_back(SkeletonVertex(ParentBone, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f)));
            mBonePositions.push_back(SkeletonVertex(thisBone,   D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f)));
        }
    }
}

void SkinnedMeshRenderer::UpdateBoneMesh( const D3DXMATRIX& matWorld, unsigned int time )
{
    mBonePositions.resize(0);
    unsigned int nBones = m_pSkeleton->NumBones();
    for (unsigned int i=0; i<nBones; i++)
    {
        Bone* pBone = m_pSkeleton->GetBone(i);
        int lParentBoneID = pBone->mParentId;
        if (lParentBoneID == -1)    //index为i的bone为根骨骼
        {
            //D3DXMATRIX& w = pBone->matBone*pBone->matOffset;
            //DebugPrintf("Root Bone \"%s\" positon at time %d : (%.3f, %.3f, %.3f)\n",
            //    pBone->mName.c_str(), time,
            //    w._41, w._42, w._43);
            continue;
        }
        D3DXMATRIX w1, w2;
        Bone* pParentBone = m_pSkeleton->GetBone(lParentBoneID);
        w1 = pBone->matOffset;
        w2 = pParentBone->matOffset;

        //Extract translation
        D3DXVECTOR3 thisBone = D3DXVECTOR3(w1(3, 0), w1(3, 1), w1(3, 2));
        D3DXVECTOR3 ParentBone = D3DXVECTOR3(w2(3, 0), w2(3, 1), w2(3, 2));

        D3DXVECTOR3 offset = (thisBone - ParentBone);
        if(D3DXVec3Length(&offset) < 100.0f)
        {
            mBonePositions.push_back(SkeletonVertex(ParentBone, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f)));
            mBonePositions.push_back(SkeletonVertex(thisBone,   D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f)));
        }
        //DebugPrintf("Bone \"%s\" positon at time %d : (%.3f, %.3f, %.3f)\n",
        //    pBone->mName.c_str(), time,
        //    thisBone.x, thisBone.y, thisBone.z);
    }
    //DebugPrintf("---------------------------\n");
}

void SkinnedMeshRenderer::RenderBoneMesh(IDirect3DDevice9* pDevice,
    const D3DXMATRIX& matWorld, const D3DXMATRIX& matView, const D3DXMATRIX& matProj)
{
    HRESULT hr;
    unsigned int nBonePositions = mBonePositions.size();

    V(pDevice->SetTransform(D3DTS_WORLD, &mMatWorld));
    V(pDevice->SetTransform(D3DTS_VIEW, &matView));
    V(pDevice->SetTransform(D3DTS_PROJECTION, &matProj));
    V(pDevice->SetRenderState(D3DRS_LIGHTING, FALSE));
    V(pDevice->SetRenderState(D3DRS_ZENABLE, FALSE));
    V(pDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE));
    V(pDevice->SetMaterial(&boneMeshMaterial));
    V(pDevice->SetTexture(0,0));
    V(pDevice->DrawPrimitiveUP(D3DPT_LINELIST, nBonePositions/2, &mBonePositions[0], sizeof(SkeletonVertex)));
    V(pDevice->SetFVF(NULL));
    V(pDevice->SetRenderState(D3DRS_ZENABLE, TRUE));
    V(pDevice->SetRenderState(D3DRS_LIGHTING, TRUE));
}

#pragma endregion

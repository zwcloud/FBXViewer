#include "stdafx.h"
#include <d3d9.h>
#include "common\Profile.h"
#include "FBX\FbxExtractor.h"
#include "Mesh.h"
#include "Vertex.h"

const D3DVERTEXELEMENT9 SkinnedDecl[] = {
    { 0, offsetof(Vertex,Pos), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, offsetof(Vertex,Normal), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
    { 0, offsetof(Vertex,TC), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
    { 0, offsetof(Vertex,BoneIndices), D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0 },
    { 0, offsetof(Vertex,BoneWeights), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },
    D3DDECL_END()
};

namespace MeshUtil
{
    void LoadMeshFromFile(const char * filePath, Mesh*& mesh, Skeleton*& skeleton, Animation*& animation)
    {
        DebugPrintf("Extracting data from FBX file<%s>\n", filePath);
        Profile::Start("Extract data");
        FbxExtractor extracter;
        extracter.DoExtract(filePath);
        Profile::End();

        std::vector<Mesh*> meshes = extracter.GetMeshes();
        mesh = meshes[0];
        skeleton = extracter.GetSkeleton();
        animation = extracter.GetAnimation();
        //TODO extract material
    }

    bool Create(Mesh& mesh, IDirect3DDevice9* pDevice)
    {
        HRESULT hr = S_FALSE;

        int nFaces = mesh.nFaces;
        int nVertices = mesh.nVertices;
        const std::vector<WORD>& IndexBuf = mesh.IndexBuf;
        const std::vector<D3DXVECTOR3>& Positions = mesh.Positions;
        const std::vector<D3DXVECTOR2>& UVs = mesh.UVs;
        const std::vector<D3DXVECTOR3>& Normals = mesh.Normals;
        const std::vector<D3DXVECTOR4>& BoneIndices = mesh.BoneIndices;
        const std::vector<D3DXVECTOR3>& BoneWeights = mesh.BoneWeights;

        IDirect3DIndexBuffer9*& pIB = mesh.pIB;
        IDirect3DVertexBuffer9*& pVB = mesh.pVB;
        IDirect3DVertexDeclaration9*& pVD = mesh.pVD;

        //index
        {
            UINT lSize = nFaces * 3 * sizeof(WORD);
            V(pDevice->CreateIndexBuffer(lSize, D3DUSAGE_WRITEONLY,
                D3DFMT_INDEX16, D3DPOOL_MANAGED, &pIB, 0));
            WORD* data = 0;
            V(pIB->Lock(0, 0, (void**)&data, 0));
            memcpy(data, &IndexBuf[0], lSize);
            V(pIB->Unlock());
        }
        //vertex
        {
            DebugAssert(Positions.size() == nVertices, "Positions.size() != nVertices\n");
            UINT lSize = nVertices * sizeof(Vertex);
            V(pDevice->CreateVertexBuffer(lSize, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &pVB, 0));
            Vertex* v = 0;
            V(pVB->Lock(0, 0, (void**)&v, 0));
            for (DWORD i = 0; i < nVertices; ++i)
            {
                v[i].Pos = Positions.at(i);
                v[i].Normal = Normals.at(i);
                v[i].TC = UVs.at(i);
                v[i].BoneIndices = BoneIndices.at(i);
                v[i].BoneWeights = BoneWeights.at(i);
            }
            V(pVB->Unlock());
        }
        //vertex declaration
        {
            V(pDevice->CreateVertexDeclaration(SkinnedDecl, &pVD));
        }
        return true;
    }

    void Dump(Mesh& mesh, unsigned int n)
    {
        const std::string& mName = mesh.mName;
        const std::vector<WORD>& IndexBuf = mesh.IndexBuf;
        const std::vector<D3DXVECTOR3>& Positions = mesh.Positions;
        const std::vector<D3DXVECTOR2>& UVs = mesh.UVs;
        const std::vector<D3DXVECTOR3>& Normals = mesh.Normals;

        DebugPrintf("--------------Mesh %s-------------\n", mName.c_str());
        for (unsigned int i = 0; i<n * 3; i += 3)
        {
            D3DXVECTOR3 v0 = Positions[IndexBuf.at(i)];
            D3DXVECTOR3 v1 = Positions[IndexBuf.at(i + 1)];
            D3DXVECTOR3 v2 = Positions[IndexBuf.at(i + 2)];

            D3DXVECTOR2 uv0 = UVs[IndexBuf.at(i)];
            D3DXVECTOR2 uv1 = UVs[IndexBuf.at(i + 1)];
            D3DXVECTOR2 uv2 = UVs[IndexBuf.at(i + 2)];

            D3DXVECTOR3 n0 = Normals[IndexBuf.at(i)];
            D3DXVECTOR3 n1 = Normals[IndexBuf.at(i + 1)];
            D3DXVECTOR3 n2 = Normals[IndexBuf.at(i + 2)];

            DebugPrintf("Triangle %d\n"
                "    %d(%.3f, %.3f, %.3f)\n"
                "    %d(%.3f, %.3f, %.3f)\n"
                "    %d(%.3f, %.3f, %.3f)\n",
                i / 3,
                IndexBuf.at(i), v0.x, v0.y, v0.z,
                IndexBuf.at(i + 1), v1.x, v1.y, v1.z,
                IndexBuf.at(i + 2), v2.x, v2.y, v2.z);
            DebugPrintf("TexCood \n"
                "    (%.3f, %.3f)\n"
                "    (%.3f, %.3f)\n"
                "    (%.3f, %.3f)\n",
                uv0.x, uv0.y,
                uv1.x, uv1.y,
                uv2.x, uv2.y);
            DebugPrintf("Normal \n"
                "    (%.3f, %.3f, %.3f)\n"
                "    (%.3f, %.3f, %.3f)\n"
                "    (%.3f, %.3f, %.3f)\n",
                n0.x, n0.y, n0.z,
                n1.x, n1.y, n1.z,
                n2.x, n2.y, n2.z);
        }
        DebugPrintf("----------------------------------\n");
    }

    //Apply bone matrices to mesh: software skinning
    bool Update(Mesh& mesh, IDirect3DDevice9* pDevice, const D3DXMATRIX* matBone, unsigned int nBones)
    {
        HRESULT hr = S_FALSE;

        const int nVertices = mesh.nVertices;
        const std::vector<WORD>& IndexBuf = mesh.IndexBuf;
        const std::vector<D3DXVECTOR3>& Positions = mesh.Positions;
        const std::vector<D3DXVECTOR2>& UVs = mesh.UVs;
        const std::vector<D3DXVECTOR3>& Normals = mesh.Normals;
        const std::vector<D3DXVECTOR4>& BoneIndices = mesh.BoneIndices;
        const std::vector<D3DXVECTOR3>& BoneWeights = mesh.BoneWeights;
        IDirect3DVertexBuffer9*& pVB = mesh.pVB;

        //vertex
        {
            UINT lSize = nVertices * sizeof(Vertex);
            Vertex* v = 0;
            D3DXVECTOR4 lTmp;
            V(pVB->Lock(0, 0, (void**)&v, 0));
            for (DWORD i = 0; i < nVertices; ++i)
            {
                D3DXVECTOR3 Pos(0.0f, 0.0f, 0.0f);
                D3DXVECTOR3 Normal(0.0f, 0.0f, 0.0f);
                D3DXVECTOR3 diff;

                D3DXMATRIX mat0 = matBone[(unsigned int)BoneIndices[i].x];
                D3DXMATRIX mat1 = matBone[(unsigned int)BoneIndices[i].y];
                D3DXMATRIX mat2 = matBone[(unsigned int)BoneIndices[i].z];
                D3DXMATRIX mat3 = matBone[(unsigned int)BoneIndices[i].w];

                D3DXVec3Transform(&lTmp, &Positions.at(i), &mat0);
                diff = D3DXVECTOR3(lTmp.x, lTmp.y, lTmp.z) - Positions.at(i);
                Pos += BoneWeights[i].x*diff;
                D3DXVec3Transform(&lTmp, &Normals.at(i), &mat0);
                diff = D3DXVECTOR3(lTmp.x, lTmp.y, lTmp.z) - Positions.at(i);
                Normal += BoneWeights[i].x*diff;

                D3DXVec3Transform(&lTmp, &Positions.at(i), &mat1);
                diff = D3DXVECTOR3(lTmp.x, lTmp.y, lTmp.z) - Positions.at(i);
                Pos += BoneWeights[i].y*diff;
                D3DXVec3Transform(&lTmp, &Normals.at(i), &mat1);
                diff = D3DXVECTOR3(lTmp.x, lTmp.y, lTmp.z) - Positions.at(i);
                Normal += BoneWeights[i].y*diff;

                D3DXVec3Transform(&lTmp, &Positions.at(i), &mat2);
                diff = D3DXVECTOR3(lTmp.x, lTmp.y, lTmp.z) - Positions.at(i);
                Pos += BoneWeights[i].z*diff;
                D3DXVec3Transform(&lTmp, &Normals.at(i), &mat2);
                diff = D3DXVECTOR3(lTmp.x, lTmp.y, lTmp.z) - Positions.at(i);
                Normal += BoneWeights[i].z*diff;

                float finalWeight;
                if (AlmostZero(BoneWeights[i].x) && AlmostZero(BoneWeights[i].y) && AlmostZero(BoneWeights[i].z))
                {
                    finalWeight = 0.0f;
                }
                else
                {
                    finalWeight = 1.0f - BoneWeights[i].x - BoneWeights[i].y - BoneWeights[i].z;
                }

                D3DXVec3Transform(&lTmp, &Positions.at(i), &mat3);
                diff = D3DXVECTOR3(lTmp.x, lTmp.y, lTmp.z) - Positions.at(i);
                Pos += finalWeight*diff;
                D3DXVec3Transform(&lTmp, &Normals.at(i), &mat3);
                diff = D3DXVECTOR3(lTmp.x, lTmp.y, lTmp.z) - Positions.at(i);
                Normal += finalWeight*diff;
                //
                if (Pos != D3DXVECTOR3(0.0f, 0.0f, 0.0f))
                {
                    v[i].Pos = Positions.at(i) + Pos;
                }
                if (Normal != D3DXVECTOR3(0.0f, 0.0f, 0.0f))
                {
                    v[i].Normal = Normals.at(i) + Normal;
                    D3DXVec3Normalize(&v[i].Normal, &v[i].Normal);
                }
                v[i].TC = UVs.at(i);
                v[i].BoneIndices = BoneIndices.at(i);
                v[i].BoneWeights = BoneWeights.at(i);
            }
            V(pVB->Unlock());
        }
        return true;
    }

}
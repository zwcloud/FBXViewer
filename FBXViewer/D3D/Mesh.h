#pragma once
#include <d3d9.h>
#include <vector>
#include <d3dx9.h>
#include "Skeleton.h"
#include "Animation.h"

class Mesh
{
public:
    bool m_bStatic;
    std::string mName;
    unsigned int nVertices;
    unsigned int nFaces;

    // index buffer
    std::vector<WORD> IndexBuf;             // index

    // vertex buffer (per vertex data)
    std::vector<D3DXVECTOR3> Positions;     //positions
    std::vector<D3DXVECTOR2> UVs;           //texture coordinates (only one now)
    std::vector<D3DXVECTOR3> Normals;       //normals
    std::vector<D3DXVECTOR3> Tangents;      //tangents(not used)
    std::vector<D3DXVECTOR3> Binormals;     //binormals(not used)

    //skin mesh data
    std::vector<D3DXVECTOR4> BoneIndices;   //bone index
    std::vector<D3DXVECTOR3> BoneWeights;   //bone weights for this vertex, forth weight can be calculated from first three weights.

    //D3D9 objects
    IDirect3DIndexBuffer9*          pIB;
    IDirect3DVertexBuffer9*         pVB;
    IDirect3DVertexDeclaration9*    pVD;
};

namespace MeshUtil
{
    void LoadMeshFromFile(const char * filePath, Mesh*& mesh, Skeleton*& skeleton, Animation*& animation);

    bool Create(Mesh& mesh, IDirect3DDevice9* pDevice);

    void Dump(Mesh& mesh, unsigned int n);

    bool Update(Mesh& mesh, IDirect3DDevice9* pDevice, const D3DXMATRIX* matBone, unsigned int nBones);
}

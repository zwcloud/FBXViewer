#pragma once

struct Vertex
{
    D3DXVECTOR3 Pos;    //0 + 12
    D3DXVECTOR3 Normal; //12 + 12
    D3DXVECTOR2 TC;     //24 + 8
    D3DXVECTOR4 BoneIndices;    //32 + 16
    D3DXVECTOR3 BoneWeights;    //48 + 12
};
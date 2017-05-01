#pragma once
class Material
{
    //lighting
    D3DXCOLOR ambient;
    D3DXCOLOR diffuse;
    D3DXCOLOR specular;

    //texture
    const char* diffuseMap;
    IDirect3DTexture9* texture;

    //shader
    IDirect3DVertexShader9* pVS;
    IDirect3DPixelShader9* pPS;
    ID3DXConstantTable* pCTVS;
    ID3DXConstantTable* pCTPS;

public:
    Material();
    ~Material();

    D3DXCOLOR& Ambient() { return ambient; }
    D3DXCOLOR& Diffuse() { return diffuse; }
    D3DXCOLOR& Specular() { return specular; }
    const char* DiffuseMap() { return diffuseMap; }
    IDirect3DTexture9*& Texture() { return texture; }
    IDirect3DVertexShader9*& VS() { return pVS; }
    IDirect3DPixelShader9*& PS() { return pPS; }
    ID3DXConstantTable*& VSConstantTable() { return pCTVS; }
    ID3DXConstantTable*& PSConstantTable() { return pCTPS; }

};

namespace MaterialUtil
{
    Material* CreateMaterial(IDirect3DDevice9* pDevice, const char* diffuseMapPath);

    void DestroyMaterial(Material* material);

    void SetConstants(IDirect3DDevice9* pDevice, Material* material,
        const D3DXMATRIX& matWorld, const D3DXMATRIX& matViewProj, const D3DXVECTOR3& eyePoint,
        const D3DXMATRIX* pMatBone, unsigned int nBones);
}

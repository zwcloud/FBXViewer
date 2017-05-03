#include "stdafx.h"
#include "Material.h"
#include "RenderSettings.h"

Material::Material()
{
    diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

    diffuseMap = "<nodiffusemap>";
}


Material::~Material()
{
}

namespace MaterialUtil
{
    Material* CreateMaterial(IDirect3DDevice9* pDevice, const char* diffuseMapPath)
    {
        HRESULT hr;

        Material* material = new Material;
        const char* diffuseMap = material->DiffuseMap();
        IDirect3DTexture9*& texture = material->Texture();
        IDirect3DVertexShader9*& pVS = material->VS();
        IDirect3DPixelShader9*& pPS = material->PS();
        ID3DXConstantTable*& pCTVS = material->VSConstantTable();
        ID3DXConstantTable*& pCTPS = material->PSConstantTable();

        //Texture
        {
            if (strcmp(diffuseMapPath, "<nodiffusemap>") != 0)
            {
                hr = D3DXCreateTextureFromFileEx(pDevice, diffuseMap,
                    0, 0, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
                    NULL, NULL, &texture);
                DebugAssert(SUCCEEDED(hr), "D3DXCreateTextureFromFileEx failed for %s \n", diffuseMap);
            }
            else//1¡Á1 dummy texture
            {
                hr = pDevice->CreateTexture(1, 1, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &texture, NULL);
                D3DLOCKED_RECT lr;
                hr = texture->LockRect(0, &lr, 0, D3DLOCK_NOSYSLOCK);
                DebugAssert(SUCCEEDED(hr), "D3DXCreateTextureFromFileEx failed for 1¡Á1 Dummy texture\n");
                *(DWORD*)lr.pBits = MAKEFOURCC(rand() & 255, rand() & 255, rand() & 255, 255);
                hr = texture->UnlockRect(0);
            }
        }

        //Shader
        {
            ID3DXBuffer* errorBuf;
            ID3DXBuffer* ShaderBuf;

            //vertex shader
            hr = D3DXCompileShaderFromFile("resources/shader/skinned.vsh", NULL, NULL, "main", "vs_3_0", NULL, &ShaderBuf, &errorBuf, &pCTVS);
            if (FAILED(hr) && errorBuf != 0 && errorBuf->GetBufferPointer() != 0)
            {
                OutputDebugStringA((char*)errorBuf->GetBufferPointer());
                SAFE_RELEASE(ShaderBuf);
                SAFE_RELEASE(errorBuf);
                return false;
            }
            V(pDevice->CreateVertexShader((DWORD*)ShaderBuf->GetBufferPointer(), &pVS));
            SAFE_RELEASE(ShaderBuf);
            SAFE_RELEASE(errorBuf);

            //pixel shader
            hr = D3DXCompileShaderFromFile("resources/shader/skinned.psh", NULL, NULL, "main", "ps_3_0", NULL, &ShaderBuf, &errorBuf, &pCTPS);
            if (FAILED(hr) && errorBuf != 0 && errorBuf->GetBufferPointer() != 0)
            {
                OutputDebugStringA((char*)errorBuf->GetBufferPointer());
                SAFE_RELEASE(ShaderBuf);
                SAFE_RELEASE(errorBuf);
                return false;
            }
            V(pDevice->CreatePixelShader((DWORD*)ShaderBuf->GetBufferPointer(), &pPS));
            SAFE_RELEASE(ShaderBuf);
            SAFE_RELEASE(errorBuf);
        }

        return material;
    }

    void DestroyMaterial(Material* material)
    {
        IDirect3DTexture9*& texture = material->Texture();
        IDirect3DVertexShader9*& pVS = material->VS();
        IDirect3DPixelShader9*& pPS = material->PS();
        ID3DXConstantTable*& pCTVS = material->VSConstantTable();
        ID3DXConstantTable*& pCTPS = material->PSConstantTable();

        SAFE_RELEASE(texture);
        SAFE_RELEASE(pVS);
        SAFE_RELEASE(pPS);
        SAFE_RELEASE(pCTVS);
        SAFE_RELEASE(pCTPS);
    }

    void SetConstants(IDirect3DDevice9 * pDevice, Material * material,
        const D3DXMATRIX & matWorld, const D3DXMATRIX & matViewProj, const D3DXVECTOR3 & eyePoint,
        const D3DXMATRIX * pMatBone, unsigned int nBones)
    {
        IDirect3DVertexShader9*& pVS = material->VS();
        IDirect3DPixelShader9*& pPS = material->PS();
        ID3DXConstantTable*& pCTVS = material->VSConstantTable();
        ID3DXConstantTable*& pCTPS = material->PSConstantTable();

        D3DXCOLOR materialAmbient = material->Ambient();
        D3DXCOLOR materialDiffuse = material->Diffuse();
        D3DXCOLOR materialSpecular = material->Specular();

        RenderSettings& renderSettings = RenderSettings::getInstance();
        D3DXCOLOR Ambient = renderSettings.Ambient();
        D3DXCOLOR Diffuse = renderSettings.Diffuse();
        D3DXCOLOR Specular = renderSettings.Specular();
        float SpecularPower = renderSettings.SpecularPower();
        D3DXVECTOR3 LightPos = renderSettings.LightPos();
        D3DXVECTOR3 Attenuation012 = renderSettings.Attenuation012();

        //common
        {
            //matWorld matView matProj
            D3DXHANDLE hMatWorld = pCTVS->GetConstantByName(NULL, "matWorld");
            D3DXHANDLE hMatView = pCTVS->GetConstantByName(NULL, "matViewProj");
            D3DXHANDLE hMatWorldInverseTranspose = pCTVS->GetConstantByName(NULL, "matWorldInverseTranspose");
            D3DXHANDLE hEyePosition = pCTVS->GetConstantByName(NULL, "gEyePosition");
            D3DXHANDLE hPointLightPosition = pCTVS->GetConstantByName(NULL, "gPointLightPosition");

            pCTVS->SetMatrix(pDevice, hMatWorld, &matWorld);
            pCTVS->SetMatrix(pDevice, hMatView, &matViewProj);
            pCTVS->SetVector(pDevice, hPointLightPosition, &D3DXVECTOR4(eyePoint, 0));
            pCTVS->SetVector(pDevice, hEyePosition, &D3DXVECTOR4(eyePoint, 0.0f));

            D3DXMATRIX worldInverseTranspose;
            D3DXMatrixInverse(&worldInverseTranspose, 0, &matWorld);
            D3DXMatrixTranspose(&worldInverseTranspose, &worldInverseTranspose);
            pCTVS->SetMatrix(pDevice, hMatWorldInverseTranspose, &worldInverseTranspose);
        }

        //light
        //Ambient
        D3DXHANDLE hAmbient = pCTVS->GetConstantByName(NULL, "gAmbient");
        pCTVS->SetVector(pDevice, hAmbient,
            &D3DXVECTOR4(Ambient.r, Ambient.g, Ambient.b, Ambient.a));
        D3DXHANDLE hMaterialAmbient = pCTVS->GetConstantByName(NULL, "gMaterialAmbient");
        pCTVS->SetVector(pDevice, hMaterialAmbient,
            &D3DXVECTOR4(materialAmbient.r, materialAmbient.g, materialAmbient.b, materialAmbient.a));

        //Diffuse
        D3DXHANDLE hDiffuse = pCTVS->GetConstantByName(NULL, "gDiffuse");
        pCTVS->SetVector(pDevice, hDiffuse,
            &D3DXVECTOR4(Diffuse.r, Diffuse.g, Diffuse.b, Diffuse.a));
        D3DXHANDLE hMaterialDiffuse = pCTVS->GetConstantByName(NULL, "gMaterialDiffuse");
        pCTVS->SetVector(pDevice, hMaterialDiffuse,
            &D3DXVECTOR4(materialDiffuse.r, materialDiffuse.g, materialDiffuse.b, materialDiffuse.a));

        //Specular
        D3DXHANDLE hSpecular = pCTVS->GetConstantByName(NULL, "gSpecular");
        pCTVS->SetVector(pDevice, hSpecular,
            &D3DXVECTOR4(Specular.r, Specular.g, Specular.b, Specular.a));
        D3DXHANDLE hMaterialSpecular = pCTVS->GetConstantByName(NULL, "gMaterialSpecular");
        pCTVS->SetVector(pDevice, hMaterialSpecular,
            &D3DXVECTOR4(materialSpecular.r, materialSpecular.g, materialSpecular.b, materialSpecular.a));
        D3DXHANDLE hSpecularPower = pCTVS->GetConstantByName(NULL, "gSpecularPower");
        pCTVS->SetValue(pDevice, hSpecularPower, &SpecularPower, sizeof(float));

        //Point light
        D3DXHANDLE hPointLightPosition = pCTVS->GetConstantByName(NULL, "gPointLightPosition");
        pCTVS->SetVector(pDevice, hPointLightPosition, &D3DXVECTOR4(LightPos, 0));
        D3DXHANDLE hAttenuation012 = pCTVS->GetConstantByName(NULL, "gAttenuation012");
        pCTVS->SetVector(pDevice, hAttenuation012, &D3DXVECTOR4(Attenuation012, 0));

        //Bone Matrices
        {
            D3DXHANDLE hMatrixPalette = pCTVS->GetConstantByName(NULL, "MatrixPalette");
            pCTVS->SetMatrixArray(pDevice, hMatrixPalette, pMatBone, nBones);
        }
    }

}
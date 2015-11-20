#include "stdafx.h"
#include "TMesh.h"
#include "Skeleton.h"

D3DVERTEXELEMENT9 SkinnedDecl[] = {
    {0, offsetof(Vertex,Pos), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
    {0, offsetof(Vertex,Normal), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
    {0, offsetof(Vertex,TC), D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
    {0, offsetof(Vertex,BoneIndices), D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0},
    {0, offsetof(Vertex,BoneWeights), D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0},
    D3DDECL_END()
};


TMesh::TMesh( void ) : m_bStatic(true),mMeshType(RIGHTHANDED_ZUP),pVB(0),pVD(0),pIB(0),pTexture(0),pVS(0),pPS(0),pCTVS(0),pCTPS(0)
{
    Ambient = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);
    Diffuse = D3DXCOLOR(0.5f, 0.7f, 0.0f, 1.0f);
    Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    SpecularPower = 5.0f;

    LightPos = D3DXVECTOR3(0.0f, 5.0f, 0.0f);
    Attenuation012 = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

    material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    material.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    material.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

    material.DiffuseMap = "<nodiffusemap>";

    EyePosition = D3DXVECTOR3();
}

TMesh::~TMesh( void )
{

}

void TMesh::SetSkinnedConstants( IDirect3DDevice9* pDevice,
    const D3DXMATRIX& matWorld, const D3DXMATRIX& matViewProj, const D3DXVECTOR3& eyePoint,
    const D3DXMATRIX* pMatBone, unsigned int nBones)
{
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
        pCTVS->SetVector(pDevice, hPointLightPosition, &D3DXVECTOR4(eyePoint,0));
        pCTVS->SetVector(pDevice, hEyePosition, &D3DXVECTOR4(eyePoint, 0.0f));

        D3DXMATRIX worldInverseTranspose;
        D3DXMatrixInverse(&worldInverseTranspose, 0, &matWorld);
        D3DXMatrixTranspose(&worldInverseTranspose, &worldInverseTranspose);
        pCTVS->SetMatrix(pDevice, hMatWorldInverseTranspose, &worldInverseTranspose);
    }
    //vertex shader constants
    //light
    //Ambient
    D3DXHANDLE hAmbient = pCTVS->GetConstantByName(NULL, "gAmbient");
    pCTVS->SetVector(pDevice, hAmbient,
        &D3DXVECTOR4(Ambient.r, Ambient.g, Ambient.b, Ambient.a));
    D3DXHANDLE hMaterialAmbient = pCTVS->GetConstantByName(NULL, "gMaterialAmbient");
    pCTVS->SetVector(pDevice, hMaterialAmbient,
        &D3DXVECTOR4(material.Ambient.r,material.Ambient.g,material.Ambient.b,material.Ambient.a));
    //Diffuse
    D3DXHANDLE hDiffuse = pCTVS->GetConstantByName(NULL, "gDiffuse");
    pCTVS->SetVector(pDevice, hDiffuse,
        &D3DXVECTOR4(Diffuse.r, Diffuse.g, Diffuse.b, Diffuse.a));
    D3DXHANDLE hMaterialDiffuse = pCTVS->GetConstantByName(NULL, "gMaterialDiffuse");
    pCTVS->SetVector(pDevice, hMaterialDiffuse,
        &D3DXVECTOR4(material.Diffuse.r, material.Diffuse.g,material.Diffuse.b,material.Diffuse.a));
    //Specular
    D3DXHANDLE hSpecular = pCTVS->GetConstantByName(NULL, "gSpecular");
    pCTVS->SetVector(pDevice, hSpecular,
        &D3DXVECTOR4(Specular.r, Specular.g, Specular.b, Specular.a));
    D3DXHANDLE hMaterialSpecular = pCTVS->GetConstantByName(NULL, "gMaterialSpecular");
    pCTVS->SetVector(pDevice, hMaterialSpecular,
        &D3DXVECTOR4(material.Specular.r,material.Specular.g,material.Specular.b,material.Specular.a));
    D3DXHANDLE hSpecularPower = pCTVS->GetConstantByName(NULL, "gSpecularPower");
    pCTVS->SetValue(pDevice, hSpecularPower, &SpecularPower, sizeof(float));
    //Point light
    D3DXHANDLE hPointLightPosition = pCTVS->GetConstantByName(NULL, "gPointLightPosition");
    pCTVS->SetVector(pDevice, hPointLightPosition, &D3DXVECTOR4(LightPos,0));
    D3DXHANDLE hAttenuation012 = pCTVS->GetConstantByName(NULL, "gAttenuation012");
    pCTVS->SetVector(pDevice, hAttenuation012, &D3DXVECTOR4(Attenuation012,0));

    //Bone Matrices
    {
        D3DXHANDLE hMatrixPalette = pCTVS->GetConstantByName(NULL, "MatrixPalette");
        pCTVS->SetMatrixArray(pDevice, hMatrixPalette, pMatBone, nBones);
    }

    //pixel shader constants
    //none
}

bool TMesh::Create( IDirect3DDevice9* pDevice )
{
    HRESULT hr = S_FALSE;
    //skinned mesh
    {
        //index
        {
            UINT lSize = nFaces*3*sizeof(WORD);
            V(pDevice->CreateIndexBuffer(lSize, D3DUSAGE_WRITEONLY,
                D3DFMT_INDEX16, D3DPOOL_MANAGED, &pIB, 0));
            WORD* data = 0;
            V(pIB->Lock(0, 0, (void**)&data, 0));
            memcpy(data, &IndexBuf[0], lSize);
            V(pIB->Unlock());
        }
        //vertex
        {
            DebugAssert(Positions.size()==nVertices, "nVertices和Positon的数量不符合\n");
            UINT lSize = nVertices * sizeof(Vertex);
            V(pDevice->CreateVertexBuffer(lSize, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &pVB, 0));
            Vertex* v = 0;
            V(pVB->Lock(0, 0, (void**)&v, 0));
            for(DWORD i = 0; i < nVertices; ++i)
            {
                v[i].Pos = Positions.at(i);
                v[i].Normal = Normals.at(i);
                v[i].TC = UVs.at(i);
                v[i].BoneIndices = BoneIndices.at(i);
                v[i].BoneWeights = BoneWeights.at(i);
            }
            V(pVB->Unlock());
        }
        //vertex decl
        {
            V(pDevice->CreateVertexDeclaration( SkinnedDecl , &pVD ));
        }

        //material: diffuse texture
        {
            if (material.DiffuseMap!="<nodiffusemap>")
            {
                hr =D3DXCreateTextureFromFileEx(pDevice, material.DiffuseMap.c_str(),
                    0, 0, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
                    NULL, NULL, &pTexture);
                if (FAILED(hr))
                {
                    DebugPrintf("Diffuse texture: %s 创建失败\n", material.DiffuseMap.c_str());
                }
            }
            else//创建1×1的Texture
            {
                hr = pDevice->CreateTexture( 1 , 1 , 0 , 0 , D3DFMT_A8R8G8B8 , D3DPOOL_MANAGED , &pTexture , NULL ); 
                D3DLOCKED_RECT lr;
                hr = pTexture->LockRect( 0 , &lr , 0 , D3DLOCK_NOSYSLOCK );
                if( SUCCEEDED( hr ) )
                {
                    *(DWORD*)lr.pBits = MAKEFOURCC( rand() & 255 , rand() & 255 , rand() & 255 , 255 );
                    hr = pTexture->UnlockRect( 0 );
                }
                else
                {
                    DebugPrintf("1×1 Dummy texture 创建失败\n");
                }
            }
        }

        //shader
        ID3DXBuffer* errorBuf;
        ID3DXBuffer* ShaderBuf;
        //vertex shader
        hr = D3DXCompileShaderFromFile("shader/skinned.vsh", NULL, NULL, "main", "vs_3_0", NULL,  &ShaderBuf, &errorBuf, &pCTVS);
        if ( FAILED( hr ) && errorBuf != 0 && errorBuf->GetBufferPointer() != 0 )
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
        hr = D3DXCompileShaderFromFile("shader/skinned.psh", NULL, NULL, "main", "ps_3_0", NULL,  &ShaderBuf, &errorBuf, &pCTPS);
        if ( FAILED( hr ) && errorBuf != 0 && errorBuf->GetBufferPointer() != 0 )
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
    return true;
}

void TMesh::Draw( IDirect3DDevice9* pDevice)
{
    HRESULT hr = S_FALSE;
    /*
        默认情况下TMesh获得的模型是在右手系下、Z向上、Y向内、X向右的（和3DSMAX中相同）
        且以逆序作为正面
    */
    pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

#define USE_ALPHATEST
//#undef USE_ALPHATEST
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
    V(pDevice->SetTexture(0, pTexture));
    V(pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, nVertices, 0, nFaces));

    V(pDevice->SetTexture(0, 0));

#ifdef USE_ALPHATEST
    pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
#else
    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
#endif
}

void TMesh::Destroy()
{
    SAFE_RELEASE(pVB);
    SAFE_RELEASE(pVD);
    SAFE_RELEASE(pIB);
    SAFE_RELEASE(pTexture);

    SAFE_RELEASE(pVS);
    SAFE_RELEASE(pPS);
    SAFE_RELEASE(pCTVS);
    SAFE_RELEASE(pCTPS);
}

void TMesh::Dump( unsigned int n )
{
    DebugPrintf("--------------Mesh %s-------------\n", mName.c_str());
    for (unsigned int i=0; i<n*3; i+=3)
    {
        D3DXVECTOR3 v0 = Positions[IndexBuf.at(i)];
        D3DXVECTOR3 v1 = Positions[IndexBuf.at(i+1)];
        D3DXVECTOR3 v2 = Positions[IndexBuf.at(i+2)];

        D3DXVECTOR2 uv0 = UVs[IndexBuf.at(i)];
        D3DXVECTOR2 uv1 = UVs[IndexBuf.at(i+1)];
        D3DXVECTOR2 uv2 = UVs[IndexBuf.at(i+2)];

        D3DXVECTOR3 n0 = Normals[IndexBuf.at(i)];
        D3DXVECTOR3 n1 = Normals[IndexBuf.at(i+1)];
        D3DXVECTOR3 n2 = Normals[IndexBuf.at(i+2)];


        DebugPrintf("Triangle %d\n"
            "    %d(%.3f, %.3f, %.3f)\n"
            "    %d(%.3f, %.3f, %.3f)\n"
            "    %d(%.3f, %.3f, %.3f)\n",
            i/3,
            IndexBuf.at(i), v0.x, v0.y, v0.z,
            IndexBuf.at(i+1), v1.x, v1.y, v1.z,
            IndexBuf.at(i+2), v2.x, v2.y, v2.z);
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

bool TMesh::Update( IDirect3DDevice9* pDevice, const D3DXMATRIX* matBone, unsigned int nBones)
{
    HRESULT hr = S_FALSE;
    //vertex
    {
        UINT lSize = nVertices * sizeof(Vertex);
        Vertex* v = 0;
        D3DXVECTOR4 lTmp;
        V(pVB->Lock(0, 0, (void**)&v, 0));
        for(DWORD i = 0; i < nVertices; ++i)
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

            float finalWeight = 1.0f - BoneWeights[i].x - BoneWeights[i].y - BoneWeights[i].z;
            D3DXVec3Transform(&lTmp, &Positions.at(i), &mat3);
            diff = D3DXVECTOR3(lTmp.x, lTmp.y, lTmp.z) - Positions.at(i);
            Pos += finalWeight*diff;
            D3DXVec3Transform(&lTmp, &Normals.at(i), &mat3);
            diff = D3DXVECTOR3(lTmp.x, lTmp.y, lTmp.z) - Positions.at(i);
            Normal += finalWeight*diff;
            //
            if (Pos!=D3DXVECTOR3(0.0f, 0.0f, 0.0f))
            {
                v[i].Pos = Positions.at(i) + Pos;
            }
            if (Normal!=D3DXVECTOR3(0.0f, 0.0f, 0.0f))
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

void TMesh::Convert( MeshType targetType )
{
    if (mMeshType == RIGHTHANDED_ZUP && targetType == RIGHTHANDED_ZUP)   //这种情况下无需转换
    {
        DebugPrintf("无需转换\n");
        return;
    }
    /*
    对静态模型进行如下转换
        右手系下、Z向上、Y向内、X向右的
                    =>
        左手系下、Y向上、Z向内、X向右的（并且模型在转换后有正确的朝向）
    需要进行以下转换：
    ①Z坐标取反
    ②三角形顶点顺序取反，即索引值每3个逆序
    ③（调整朝向）模型绕+X轴顺时针旋转90°
    */
    //①Z坐标取反
    for (unsigned int i=0; i<nVertices; ++i)
    {
        Positions.at(i).z = -Positions.at(i).z;
    }
#if 0
    //这样计算法向量是没有意义的，因为法向量数据TMesh::Normals并不和索引index一一对应，
    //而是和顶点Position一一对应
    //0.计算每个face的新法向量(标准：3DSMAX以逆时针为正向)
    D3DXVECTOR3* faceNormalNew = new D3DXVECTOR3[nFaces];
    for (unsigned int i=0; i<nFaces; i+=3)
    {
        WORD index0 = IndexBuf.at(3*i);
        WORD index1 = IndexBuf.at(3*i+1);
        WORD index2 = IndexBuf.at(3*i+2);
        D3DXVECTOR3 v0 = Positions[index0];
        D3DXVECTOR3 v1 = Positions[index1];
        D3DXVECTOR3 v2 = Positions[index2];
        D3DXVec3Cross(&faceNormalNew[i], &(v1-v0), &(v2-v0));
    }
#endif
    //②三角形顶点顺序取反
    for (unsigned int i=0; i<nFaces; ++i)  //
    {
        WORD index0 = IndexBuf.at(3*i);
        WORD index1 = IndexBuf.at(3*i+1);
        WORD index2 = IndexBuf.at(3*i+2);
        IndexBuf.at(3*i) = index2;
        IndexBuf.at(3*i+2) = index0;
    }
    //③模型绕+X轴顺时针旋转90°
    D3DXMATRIX lRotation90;
    D3DXMatrixRotationX(&lRotation90, D3DX_PI/2);
    D3DXVECTOR4 tmp;
    for (unsigned int i=0; i<nVertices; ++i)
    {
        D3DXVec3Transform(&tmp, &Positions.at(i), &lRotation90);
        Positions.at(i) = D3DXVECTOR3(tmp.x, tmp.y, tmp.z);
    }
}

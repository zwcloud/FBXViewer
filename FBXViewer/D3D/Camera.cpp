#include "stdafx.h"
#include "Camera.h"
#include "GraphicsDevice.h"
#include "TMesh.h"

D3DXVECTOR3 eyePoint;
D3DXVECTOR3 lookAt;
D3DXVECTOR3 up;

//以下为常量，代表初始camera的参数
D3DXVECTOR3 orginalEyePoint;
D3DXVECTOR3 orginalLookAt;
D3DXVECTOR3 orginalUp;

//camera只包含旋转变换的参数
D3DXVECTOR3 fixedEyePoint;
D3DXVECTOR3 fixedLookAt;
D3DXVECTOR3 fixedUp;
//以下为常量，代表初始只包含旋转变换camera的参数
D3DXVECTOR3 orginalFixedEyePoint;
D3DXVECTOR3 orginalFixedLookAt;
D3DXVECTOR3 orginalFixedUp;

namespace
{
    bool IsD3DXVECTOR3Equal(const D3DXVECTOR3& v0, const D3DXVECTOR3& v1)
    {
        return floatEqual(v0.x,v1.x,0.0001f)
            && floatEqual(v0.y,v1.y,0.0001f)
            && floatEqual(v0.z,v1.z,0.0001f);
    }
}
void InitCamera()
{    
    orginalFixedEyePoint = fixedEyePoint = orginalEyePoint = eyePoint = D3DXVECTOR3(30.0f, -40.0f, 20.0f);    
	orginalFixedLookAt = fixedLookAt = orginalLookAt = lookAt = D3DXVECTOR3(0.0f, 0.0f, 0.0f);    
    orginalFixedUp = fixedUp = orginalUp = up = D3DXVECTOR3(0.0f, 0.0f, 1.0f);   //z轴正向为上方向    
}

void ResetCamera()
{
    eyePoint = orginalEyePoint;
    lookAt = orginalLookAt;
    up = orginalUp;

    fixedEyePoint = orginalFixedEyePoint;
    fixedLookAt = orginalFixedLookAt;
    fixedUp = orginalFixedUp;
}

//将Camera向所看方向移动一段距离
void MoveCameraForward(float distance)
{
    //所看方向的方向向量
    D3DXVECTOR3 vForward = lookAt - eyePoint;
    D3DXVec3Normalize(&vForward, &vForward);
    eyePoint += distance*vForward;
    lookAt += distance*vForward;
}

//将Camera向所看方向的反方向移动一段距离
void MoveCameraBackward(float distance)
{
    MoveCameraForward(-distance);
}

void MoveCameraLeft(float distance)
{
    D3DXVECTOR3 vForward = lookAt - eyePoint;
    D3DXVec3Normalize(&vForward, &vForward);

    D3DXVECTOR3 vLeft;
    D3DXVec3Cross(&vLeft, &up, &vForward);

    D3DXVec3Normalize(&vLeft,&vLeft);
    eyePoint += distance*vLeft;
    lookAt += distance*vLeft;
}

void MoveCameraRight(float distance)
{
    MoveCameraLeft(-distance);
}

void MoveCameraUpward(float distance)
{
    eyePoint += distance*up;
    lookAt += distance*up;
}

void MoveCameraDownward(float distance)
{
    eyePoint -= distance*up;
    lookAt -= distance*up;
}

void RotateCameraHorizontally(float radian)
{
    /*
        常规变换
    */
    //将eyePoint绕lookAt点旋转
    D3DXMATRIX rotation;
    //D3DXMatrixRotationAxis:
    // D3DXMATRIX * D3DXMatrixRotationAxis(
    //__inout  D3DXMATRIX *pOut,
    //    __in     const D3DXVECTOR3 *pV,
    //    __in     FLOAT Angle
    //    );
    //其功能是生成绕原点处的方向向量为pV的轴旋转角度Angle的旋转变换矩阵
    //所以在绕非原点旋转时需要先将目标点的旋转变换为绕原点旋转
    D3DXMatrixRotationAxis(&rotation, &up, radian);
    D3DXVECTOR4 tmp;
    D3DXVec3Transform(&tmp, &(eyePoint-lookAt), &rotation);
    eyePoint = lookAt + D3DXVECTOR3(tmp.x, tmp.y, tmp.z);

    /*
        fixed 变换
        */
    {
        //将eyePoint绕lookAt点旋转
        D3DXMATRIX rotation;
        D3DXMatrixRotationAxis(&rotation, &fixedUp, radian);
        D3DXVECTOR4 tmp;
        D3DXVec3Transform(&tmp, &(fixedEyePoint-fixedLookAt), &rotation);
        fixedEyePoint = fixedLookAt + D3DXVECTOR3(tmp.x, tmp.y, tmp.z);
    }
}

void RotateCameraVertically(float radian)
{
    /*
        常规变换
    */
    {
        D3DXVECTOR3 vForward = lookAt - eyePoint;
        D3DXVec3Normalize(&vForward, &vForward);

        D3DXVECTOR3 vLeft;
        D3DXVec3Cross(&vLeft, &up, &vForward);
        D3DXVec3Normalize(&vLeft, &vLeft);

        D3DXMATRIX rotation;
        D3DXMatrixRotationAxis(&rotation, &vLeft, radian);
        D3DXVECTOR4 tmp;
        D3DXVec3Transform(&tmp, &(eyePoint-lookAt), &rotation);
        eyePoint = lookAt + D3DXVECTOR3(tmp.x, tmp.y, tmp.z);

        //竖直方向旋转时，up方向随之变化，需要更新
        vForward = lookAt - eyePoint;
        D3DXVec3Normalize(&vForward, &vForward);
        D3DXVec3Cross(&up, &vForward, &vLeft);
        D3DXVec3Normalize(&up, &up);
    }
    
    /*
        fixed 变换
    */
    {
        D3DXVECTOR3 vForward = fixedLookAt - fixedEyePoint;
        D3DXVec3Normalize(&vForward, &vForward);

        D3DXVECTOR3 vLeft;
        D3DXVec3Cross(&vLeft, &fixedUp, &vForward);
        D3DXVec3Normalize(&vLeft, &vLeft);

        D3DXMATRIX rotation;
        D3DXMatrixRotationAxis(&rotation, &vLeft, radian);
        D3DXVECTOR4 tmp;
        D3DXVec3Transform(&tmp, &(fixedEyePoint-fixedLookAt), &rotation);
        fixedEyePoint = fixedLookAt + D3DXVECTOR3(tmp.x, tmp.y, tmp.z);

        //竖直方向旋转时，up方向随之变化，需要更新
        vForward = fixedLookAt - fixedEyePoint;
        D3DXVec3Normalize(&vForward, &vForward);
        D3DXVec3Cross(&fixedUp, &vForward, &vLeft);
        D3DXVec3Normalize(&fixedUp, &fixedUp);
    }
}


namespace StaticMesh
{

    CubeMesh::CubeMesh( void )
    {

    }

    CubeMesh::~CubeMesh( void )
    {

    }
    
	void CubeMesh::SetVertexData(int meshType)
    {
        mName = "Cube";
        mPrimitiveType = D3DPT_TRIANGLELIST;
        mVertexType = PCT;
        nVertex = 36;
        nPrimitive = 12;

        material.DiffuseMap = "box.png";

        //准备数据
        D3DXVECTOR3 Origin = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
        D3DXVECTOR3 lPos[8];
        WORD* l_pIndex = NULL;
        D3DXVECTOR2* l_pUV = NULL;
        lPos[0] = Origin + 4.0*D3DXVECTOR3(-1.0f, -1.0f, 1.0f);
	    lPos[1] = Origin + 4.0*D3DXVECTOR3(-1.0f, 1.0f, 1.0f);
	    lPos[2] = Origin + 4.0*D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	    lPos[3] = Origin + 4.0*D3DXVECTOR3(1.0f, -1.0f, 1.0f);
	    lPos[4] = Origin + 4.0*D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
	    lPos[5] = Origin + 4.0*D3DXVECTOR3(-1.0f, 1.0f, -1.0f);
	    lPos[6] = Origin + 4.0*D3DXVECTOR3(1.0f, 1.0f, -1.0f);
	    lPos[7] = Origin + 4.0*D3DXVECTOR3(1.0f, -1.0f, -1.0f);
	    WORD lIndex[] = {
	        2,1,0, 0,3,2,   //上 可见   正面 逆时针
	        4,5,6, 4,6,7,   //下 不可见 反面 顺时针
	        0,4,3, 3,4,7,   //前 可见   正面 逆时针
	        1,2,5, 2,6,5,   //后 不可见 反面 顺时针
	        0,1,4, 4,1,5,   //左 不可见 反面 顺时针
	        2,3,7, 2,7,6,   //右 可见   正面 逆时针
        };
	    l_pIndex = lIndex;
#define v2 D3DXVECTOR2
	    D3DXVECTOR2 lUV[] = 
	        {
		        //上
		        v2(0.666f,0.333f),v2(0.333f,0.333f),v2(0.333f,0.666f),
		        v2(0.333f,0.666f),v2(0.666f,0.666f),v2(0.666f,0.333f),
		        //下
		        v2(0.666f,0.666f),v2(0.666f,1.000f),v2(1.000f,1.000f),
		        v2(0.666f,0.666f),v2(1.000f,1.000f),v2(1.000f,0.666f),
		        //前
		        v2(0.333f,0.666f),v2(0.333f,1.000f),v2(0.666f,0.666f),
		        v2(0.666f,0.666f),v2(0.333f,1.000f),v2(0.666f,1.000f),
		        //后
		        v2(0.333f,0.333f),v2(0.666f,0.333f),v2(0.333f,0.000f),
		        v2(0.666f,0.333f),v2(0.666f,0.000f),v2(0.333f,0.000f),
		        //左
		        v2(0.333f,0.666f),v2(0.333f,0.333f),v2(0.000f,0.666f),
		        v2(0.000f,0.666f),v2(0.333f,0.333f),v2(0.000f,0.333f),
		        //右
		        v2(0.666f,0.333f),v2(0.666f,0.666f),v2(1.000f,0.666f),
		        v2(0.666f,0.333f),v2(1.000f,0.666f),v2(1.000f,0.333f),
	        };
	    l_pUV = lUV;
#undef v2
	    //PCT型顶点
        Positions.resize(nPrimitive*3);
        Colors.resize(nPrimitive*3);
        UVs.resize(nPrimitive*3);
        IndexBuf.resize(nPrimitive*3);
        for (size_t i=0; i<nPrimitive*3; i++)
        {
            Positions[i] = lPos[l_pIndex[i]];
            UVs[i] = l_pUV[i];
            IndexBuf[i] = i;
            Colors[i] = 0xFFFFFFFF;
        }

    }

    D3DXVECTOR3 p;
    bool CubeMesh::Update(const D3DXVECTOR2& currentMousePos,
        const D3DXMATRIX& matWorld, const D3DXMATRIX& matView, const D3DXMATRIX& matProj)
    {
        GraphicsDevice* pGDevice = GraphicsDevice::getInstance();
        IDirect3DDevice9* pDevice = pGDevice->m_pD3DDevice;
        //将屏幕坐标进行变换，变换到相对于mCubeViewport的原点的坐标
        int x = currentMousePos.x - pGDevice->mCubeViewport.X;
        int y = currentMousePos.y - pGDevice->mCubeViewport.Y;
        //获取World中的射线
        Ray ray = CalcPickingRay(x, y,
            pGDevice->mCubeViewport, matView, matProj );
        //检测射线和所有三角形的相交情况
        HRESULT hr = S_FALSE;
        UINT lSize = nPrimitive * 3 * sizeof(PCTVertex);
        PCTVertex* vertex = 0;
        V(pVB->Lock(0, 0, (void**)&vertex, 0));
        FaceType lTargetFaceType = FaceType::None;  //最近的Cube面
        float lMinDistance = FLT_MAX;               //最近的距离
        for(int i = 0; i < nPrimitive * 3; i+=6)  //步进6是为了遍历一个矩形的两个三角形
        {
            FaceType lFaceType = FaceType(i/6);
#ifdef DEBUG_CUBE
            switch(lFaceType)
            {
            case Forward:
                DebugPrintf("前");
                break;
            case Backward:
                DebugPrintf("后");
                break;
            case Upward:
                DebugPrintf("上");
                break;
            case Downward:
                DebugPrintf("下");
                break;
            case Left:
                DebugPrintf("左");
                break;
            case Right:
                DebugPrintf("右");
                break;
            default:
                break;
            }
#endif
            PCTVertex& pt0 = vertex[IndexBuf.at(i)];
            PCTVertex& pt1 = vertex[IndexBuf.at(i+1)];
            PCTVertex& pt2 = vertex[IndexBuf.at(i+2)];
            D3DXVECTOR3 v0(pt0.Pos);
            D3DXVECTOR3 v1(pt1.Pos);
            D3DXVECTOR3 v2(pt2.Pos);
            PCTVertex& pt3 = vertex[IndexBuf.at(i+3)];
            PCTVertex& pt4 = vertex[IndexBuf.at(i+4)];
            PCTVertex& pt5 = vertex[IndexBuf.at(i+5)];
            D3DXVECTOR3 v3(pt3.Pos);
            D3DXVECTOR3 v4(pt4.Pos);
            D3DXVECTOR3 v5(pt5.Pos);
            //若与该三角形相交则加深其所在Cube面颜色 TODO:遍历完后取最近的Face
            float t =0.0f,u=0.0f,v=0.0f;
            if (Ray::Intersect(ray.Origin, ray.Direction, v0, v1, v2, &t, &u, &v)
                || Ray::Intersect(ray.Origin, ray.Direction, v3, v4, v5, &t, &u, &v))
            {
#ifdef DEBUG_CUBE
                DebugPrintf("√\n");
#endif
                if (t < lMinDistance)
                {
                    lMinDistance = t;
                    lTargetFaceType = lFaceType;
                }
                p = ray.Origin + t*ray.Direction;
                pt0.Color = D3DCOLOR_ARGB(0xff,0xcc,0xcc,0xcc);
                pt1.Color = D3DCOLOR_ARGB(0xff,0xcc,0xcc,0xcc);
                pt2.Color = D3DCOLOR_ARGB(0xff,0xcc,0xcc,0xcc);
                pt3.Color = D3DCOLOR_ARGB(0xff,0xcc,0xcc,0xcc);
                pt4.Color = D3DCOLOR_ARGB(0xff,0xcc,0xcc,0xcc);
                pt5.Color = D3DCOLOR_ARGB(0xff,0xcc,0xcc,0xcc);
            }
            else
            {
#ifdef DEBUG_CUBE
                DebugPrintf("\n");
#endif
                pt0.Color = 0xFFFFFFFF;
                pt1.Color = 0xFFFFFFFF;
                pt2.Color = 0xFFFFFFFF;
                pt3.Color = 0xFFFFFFFF;
                pt4.Color = 0xFFFFFFFF;
                pt5.Color = 0xFFFFFFFF;
            }
        }
        V(pVB->Unlock());
        return true;
    }

    Ray CubeMesh::CalcPickingRay(int x, int y,
        const D3DVIEWPORT9& viewport, const D3DXMATRIX& matView, const D3DXMATRIX& matProj)
    {
        D3DXVECTOR3 vPickRayDir;
        D3DXVECTOR3 vPickRayOrig;
        D3DXVECTOR3 v;
        v.x = ( ( ( 2.0f * x ) / viewport.Width ) - 1 ) / matProj._11;
        v.y = -( ( ( 2.0f * y ) / viewport.Height ) - 1 ) / matProj._22;
        v.z = 1.0f;    //>???

        // Get the inverse view matrix
        D3DXMATRIX m;
        if(!D3DXMatrixInverse( &m, NULL, &matView ))
        {
            DebugAssert(false, "inverse failed");
        }
         
        // Transform the screen space Pick ray into 3D space
        vPickRayDir.x = v.x * m._11 + v.y * m._21 + v.z * m._31;
        vPickRayDir.y = v.x * m._12 + v.y * m._22 + v.z * m._32;
        vPickRayDir.z = v.x * m._13 + v.y * m._23 + v.z * m._33;
        vPickRayOrig.x = m._41;
        vPickRayOrig.y = m._42;
        vPickRayOrig.z = m._43;

        Ray ray;
        ray.Origin = vPickRayOrig;
        ray.Direction = vPickRayDir;
        return ray;
    }
    void CubeMesh::DrawRay(const D3DXVECTOR2& screenPos,
        const D3DXMATRIX& matWorld, const D3DXMATRIX& matView, const D3DXMATRIX& matProj)
    {
        HRESULT hr = S_FALSE;
        GraphicsDevice* pGDevice = GraphicsDevice::getInstance();
        IDirect3DDevice9* pDevice = pGDevice->m_pD3DDevice;
        Ray ray = CalcPickingRay(screenPos.x, screenPos.y,
            pGDevice->mCubeViewport, matView, matProj );
        PCVertex rayLine[] = {
            {D3DXVECTOR3(0.0f,0.0f,0.0f), D3DCOLOR_ARGB(255,255,0,0)},
            {ray.Origin + 1000*ray.Direction, D3DCOLOR_ARGB(255,255,0,0)},
        };

        PCVertex intersectPoint[] = {
            {p, D3DCOLOR_ARGB(255,0,0,255)},
            {p+D3DXVECTOR3(0.5f,0.0f,0.0f),  D3DCOLOR_ARGB(255,0,0,255)},
            {p+D3DXVECTOR3(0.0f,0.5f,0.0f),  D3DCOLOR_ARGB(255,0,0,255)},
            {p+D3DXVECTOR3(0.0f,0.0f,0.5f),  D3DCOLOR_ARGB(255,0,0,255)},
            {p+D3DXVECTOR3(-0.5f,0.0f,0.0f), D3DCOLOR_ARGB(255,0,0,255)},
            {p+D3DXVECTOR3(0.0f,-0.5f,0.0f), D3DCOLOR_ARGB(255,0,0,255)},
            {p+D3DXVECTOR3(0.0f,0.0f,-0.5f), D3DCOLOR_ARGB(255,0,0,255)},
        };

        pGDevice->SetViewport(pGDevice->mCubeViewport);
        pDevice->SetVertexShader(NULL);
        pDevice->SetPixelShader(NULL);
        V(pDevice->SetTransform(D3DTS_WORLD, &matWorld));
        V(pDevice->SetTransform(D3DTS_VIEW, &matView));
        V(pDevice->SetTransform(D3DTS_PROJECTION, &matProj));
        V(pDevice->SetRenderState(D3DRS_LIGHTING, FALSE));
        V(pDevice->SetRenderState(D3DRS_ZENABLE, FALSE));
        V(pDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE));
        V(pDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, rayLine, sizeof(PCVertex)));
        V(pDevice->DrawPrimitiveUP(D3DPT_POINTLIST, 7, intersectPoint, sizeof(PCVertex)));
        V(pDevice->SetFVF(NULL));
        V(pDevice->SetRenderState(D3DRS_ZENABLE, TRUE));
        V(pDevice->SetRenderState(D3DRS_LIGHTING, TRUE));
        pGDevice->ResetViewport();
    }

}

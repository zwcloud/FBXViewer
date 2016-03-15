#pragma once
#include "SMesh.h"

extern D3DXVECTOR3 eyePoint;
extern D3DXVECTOR3 lookAt;
extern D3DXVECTOR3 up;

extern D3DXVECTOR3 fixedEyePoint;
extern D3DXVECTOR3 fixedLookAt;
extern D3DXVECTOR3 fixedUp;

void InitCamera();
void ResetCamera();
void MoveCameraForward(float distance);
void MoveCameraBackward(float distance);
void MoveCameraLeft(float distance);
void MoveCameraRight(float distance);
void MoveCameraUpward(float distance);
void MoveCameraDownward(float distance);
void RotateCameraHorizontally(float radian);
void RotateCameraVertically(float radian);

namespace StaticMesh
{
    class CubeMesh : public SMesh
    {
    private:
        struct CubeFace
        {
            WORD Upward[2];
            WORD Downward[2];
            WORD Forward[2];
            WORD Backward[2];
            WORD Left[2];
            WORD Right[2];
        };
        typedef enum
        {
            Upward,     //0
            Downward,   //1
            Forward,    //2
            Backward,   //3
            Left,       //4
            Right,      //5
            None,
        } FaceType;
        CubeFace mCubeFace;
        const float mEdgeLenth;
        
        CubeMesh(CubeMesh&);
        void operator = (CubeMesh&);
        
        Ray CalcPickingRay(int x, int y,
            const D3DVIEWPORT9& viewport, const D3DXMATRIX& matView, const D3DXMATRIX& matProj);
    public:
        CubeMesh(void);
        ~CubeMesh(void);

		void SetVertexData(int meshType) override;

        virtual bool Update(const D3DXVECTOR2& currentMousePos,
            const D3DXMATRIX& matWorld, const D3DXMATRIX& matView, const D3DXMATRIX& matProj);

        void DrawRay(const D3DXVECTOR2& screenPos,
            const D3DXMATRIX& matWorld, const D3DXMATRIX& matView, const D3DXMATRIX& matProj);

    };
}


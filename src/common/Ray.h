#pragma once
#include <d3d9.h>
#include <d3dx9.h>

struct Ray
{
    D3DXVECTOR3 Origin;
    D3DXVECTOR3 Direction;
    Ray() :Origin(0.0f, 0.0f, 0.0f), Direction(0.0f, 0.0f, 1.0f) {};
    ~Ray() {};
    static bool Intersect(const D3DXVECTOR3& orig, const D3DXVECTOR3& dir,
        D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2,
        FLOAT* t, FLOAT* u, FLOAT* v)
    {
        // Find vectors for two edges sharing vert0
        D3DXVECTOR3 edge1 = v1 - v0;
        D3DXVECTOR3 edge2 = v2 - v0;

        // Begin calculating determinant - also used to calculate U parameter
        D3DXVECTOR3 pvec;
        D3DXVec3Cross(&pvec, &dir, &edge2);

        // If determinant is near zero, ray lies in plane of triangle
        FLOAT det = D3DXVec3Dot(&edge1, &pvec);

        D3DXVECTOR3 tvec;
        if (det > 0)
        {
            tvec = orig - v0;
        }
        else
        {
            tvec = v0 - orig;
            det = -det;
        }

        if (det < 0.0001f)
            return FALSE;

        // Calculate U parameter and test bounds
        *u = D3DXVec3Dot(&tvec, &pvec);
        if (*u < 0.0f || *u > det)
            return FALSE;

        // Prepare to test V parameter
        D3DXVECTOR3 qvec;
        D3DXVec3Cross(&qvec, &tvec, &edge1);

        // Calculate V parameter and test bounds
        *v = D3DXVec3Dot(&dir, &qvec);
        if (*v < 0.0f || *u + *v > det)
            return FALSE;

        // Calculate t, scale parameters, ray intersects triangle
        *t = D3DXVec3Dot(&edge2, &qvec);
        FLOAT fInvDet = 1.0f / det;
        *t *= fInvDet;
        *u *= fInvDet;
        *v *= fInvDet;

        return TRUE;
    }


};

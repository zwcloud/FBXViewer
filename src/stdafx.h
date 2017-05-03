// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: 在此处引用程序需要的其他头文件

#include <vector>
#include <string>
#include <map>
#include <string>
#include <set>
#include <list>
#include <algorithm>

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <float.h>
#include <limits.h>

#define D3D_DEBUG_INFO
#include <d3d9.h>
#include <d3dx9.h>

#include <vld.h>

#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))

#pragma region debug functions
void _DebugAssert(bool expression, const char *Text, ...);
void _DebugAssert(bool expression, const wchar_t *Text, ...);
void DebugPrintf(const char *Text, ...);
void DebugPrintf(const wchar_t *Text, ...);
const void GetTestFileName(char* buffer);
const void GetTestFileNameW(wchar_t* buffer);

#define DebugAssert(condition,text,...) if (!(condition)){_DebugAssert((condition), (text), __VA_ARGS__); DebugBreak();}
#pragma endregion

#pragma region 字符串转换
//转换多字节字符格式字符串str为宽字符格式字符串
//参数：
//str 待转换的多字节字符格式字符串
//返回值：转换后的宽字符格式字符串
std::wstring CStr2WStr(std::string str);

//转换宽字符格式字符串str为多字节字符格式字符串
std::string WStr2CStr(const std::wstring &str);
#pragma endregion

#pragma region 浮点数比较
bool floatEqual(float a, float b, float epsilon);
bool floatGreaterThan(float a, float b, float epsilon);
bool floatLessThan(float a, float b, float epsilon);
bool AlmostZero(float a, float epsilon = 0.001f);
#pragma endregion

#if defined(DEBUG) || defined(_DEBUG)
#ifndef V
#define V(x) { hr = (x); if( FAILED(hr) ) { DebugAssert( "error: %s(%d): %s hr=0x%x\n", __FILE__, (DWORD)__LINE__, #x, hr ); } }
#endif
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#endif    
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif


#ifndef MAX_BONE_COUNT
#define MAX_BONE_COUNT 50
#endif

float TriangleArea(const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2)
{
    float area = 0.5f*((v0.x*v1.y-v1.x*v0.y)+(v1.x*v2.y-v2.x*v1.y)+(v2.x*v0.y-v0.x*v2.y));
    return fabs(area);
}
//参考：http://stackoverflow.com/a/20861130
bool PointInTriangle(const D3DXVECTOR2& p, const D3DXVECTOR2& p0, const D3DXVECTOR2& p1, const D3DXVECTOR2& p2)
{
    float s = p0.y * p2.x - p0.x * p2.y + (p2.y - p0.y) * p.x + (p0.x - p2.x) * p.y;
    float t = p0.x * p1.y - p0.y * p1.x + (p0.y - p1.y) * p.x + (p1.x - p0.x) * p.y;

    if ((s < 0) != (t < 0))
        return false;

    float A = -p1.y * p2.x + p0.y * (p2.x - p1.x) + p0.x * (p1.y - p2.y) + p1.x * p2.y;
    if (A < 0.0)
    {
        s = -s;
        t = -t;
        A = -A;
    }
    return s > 0 && t > 0 && (s + t) < A;
}
struct Ray
{
    D3DXVECTOR3 Origin;
    D3DXVECTOR3 Direction;
    Ray():Origin(0.0f,0.0f,0.0f), Direction(0.0f,0.0f,1.0f){};
    ~Ray(){};
    static bool Intersect( const D3DXVECTOR3& orig, const D3DXVECTOR3& dir,
        D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2,
        FLOAT* t, FLOAT* u, FLOAT* v )
    {
        // Find vectors for two edges sharing vert0
        D3DXVECTOR3 edge1 = v1 - v0;
        D3DXVECTOR3 edge2 = v2 - v0;

        // Begin calculating determinant - also used to calculate U parameter
        D3DXVECTOR3 pvec;
        D3DXVec3Cross( &pvec, &dir, &edge2 );

        // If determinant is near zero, ray lies in plane of triangle
        FLOAT det = D3DXVec3Dot( &edge1, &pvec );

        D3DXVECTOR3 tvec;
        if( det > 0 )
        {
            tvec = orig - v0;
        }
        else
        {
            tvec = v0 - orig;
            det = -det;
        }

        if( det < 0.0001f )
            return FALSE;

        // Calculate U parameter and test bounds
        *u = D3DXVec3Dot( &tvec, &pvec );
        if( *u < 0.0f || *u > det )
            return FALSE;

        // Prepare to test V parameter
        D3DXVECTOR3 qvec;
        D3DXVec3Cross( &qvec, &tvec, &edge1 );

        // Calculate V parameter and test bounds
        *v = D3DXVec3Dot( &dir, &qvec );
        if( *v < 0.0f || *u + *v > det )
            return FALSE;

        // Calculate t, scale parameters, ray intersects triangle
        *t = D3DXVec3Dot( &edge2, &qvec );
        FLOAT fInvDet = 1.0f / det;
        *t *= fInvDet;
        *u *= fInvDet;
        *v *= fInvDet;

        return TRUE;
    }


};

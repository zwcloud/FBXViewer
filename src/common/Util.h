#pragma once
#include <d3d9.h>
#include <d3dx9.h>

#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))

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

#pragma region math

bool floatEqual(float a, float b, float epsilon);
bool floatGreaterThan(float a, float b, float epsilon);
bool floatLessThan(float a, float b, float epsilon);
bool AlmostZero(float a, float epsilon = 0.001f);

float TriangleArea(const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2);

bool PointInTriangle(const D3DXVECTOR2& p, const D3DXVECTOR2& p0, const D3DXVECTOR2& p1, const D3DXVECTOR2& p2);

#pragma endregion
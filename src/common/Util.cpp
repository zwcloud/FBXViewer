#include "stdafx.h"
#include "Util.h"

#pragma region debug functions
void _DebugAssert(bool expression, const char *Text, ...)
{
    if (expression)  //检查表达式是否为真，若为真则不引发DebugUtility::Assert
    {
        return;
    }
    char CaptionText[12];
    char ErrorText[2048];
    va_list valist;

    strcpy_s(CaptionText, 11, "Error");

    // Build variable text buffer
    va_start(valist, Text);
    vsprintf_s(ErrorText, 1900, Text, valist);
    va_end(valist);

    // Display the message box
    MessageBoxA(NULL, ErrorText, CaptionText, MB_OK | MB_ICONEXCLAMATION);
    OutputDebugStringA(ErrorText);
}

void _DebugAssert(bool expression, const wchar_t *Text, ...)
{
    if (expression)  //检查表达式是否为真，若为真则不引发DebugUtility::Assert
    {
        return;
    }
    wchar_t CaptionText[12];
    wchar_t ErrorText[2048];
    va_list valist;

    wcscpy_s(CaptionText, 11, L"Error");

    // Build variable text buffer
    va_start(valist, Text);
    vswprintf_s(ErrorText, 2000, Text, valist);
    va_end(valist);

    // Display the message box
    MessageBoxW(NULL, ErrorText, CaptionText, MB_OK | MB_ICONEXCLAMATION);
    OutputDebugStringW(ErrorText);

    DebugBreak();
}

void DebugPrintf(const char *Text, ...)
{
    char _Text[2048];
    va_list valist;

    // Build variable text buffer
    va_start(valist, Text);
    vsprintf_s(_Text, 2000, Text, valist);
    va_end(valist);

    // write to debug console
    OutputDebugStringA(_Text);
}

void DebugPrintf(const wchar_t *Text, ...)
{
    wchar_t _Text[2048];
    va_list valist;

    // Build variable text buffer
    va_start(valist, Text);
    vswprintf_s(_Text, 2000, Text, valist);
    va_end(valist);

    // write to debug console
    OutputDebugStringW(_Text);
}

#define TEST_FILE_PATH "resources\\inputFile.txt"
const void GetTestFileName(char* buffer)
{
    FILE* fp = fopen(TEST_FILE_PATH, "r");
    DebugAssert(NULL != fp, "open file %s failed\n", TEST_FILE_PATH);
    fgets(buffer, MAX_PATH, fp);
    fclose(fp);
}

const void GetTestFileNameW(wchar_t* buffer)
{
    FILE* fp = fopen(TEST_FILE_PATH, "r");
    DebugAssert(NULL != fp, "open file %s failed\n", TEST_FILE_PATH);
    fgetws(buffer, MAX_PATH, fp);
    fclose(fp);
}
#pragma endregion

#pragma region string
//转换多字节字符格式字符串str为宽字符格式字符串
//参数：
//str 待转换的多字节字符格式字符串
//返回值：转换后的宽字符格式字符串
std::wstring CStr2WStr(std::string str)
{
    int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, NULL);
    wchar_t* buffer = new wchar_t[len + 1];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, len);
    buffer[len] = L'\0';
    std::wstring return_value = std::wstring(buffer);
    delete[] buffer;
    return return_value;
}

//转换宽字符格式字符串str为多字节字符格式字符串
std::string WStr2CStr(const std::wstring &str)
{
    //获取缓冲区的大小，并申请空间，缓冲区大小是按字节计算的
    int len = WideCharToMultiByte(CP_ACP, 0, str.c_str(), str.size(), NULL, 0, NULL, NULL);
    char *buffer = new char[len + 1];
    WideCharToMultiByte(CP_ACP, 0, str.c_str(), str.size(), buffer, len, NULL, NULL);
    buffer[len] = '\0';
    //删除缓冲区并返回值
    std::string return_value(buffer);
    delete[] buffer;
    return return_value;
}
#pragma endregion

#pragma region math
bool floatEqual(float a, float b, float epsilon)
{
    return fabs(a - b) <= ((fabs(a) > fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

bool floatGreaterThan(float a, float b, float epsilon)
{
    return (a - b) > ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

bool floatLessThan(float a, float b, float epsilon)
{
    return (b - a) > ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

bool AlmostZero(float a, float epsilon/* = 0.001f*/)
{
    return floatEqual(a, 0, epsilon);
}

float TriangleArea(const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2)
{
    float area = 0.5f*((v0.x*v1.y - v1.x*v0.y) + (v1.x*v2.y - v2.x*v1.y) + (v2.x*v0.y - v0.x*v2.y));
    return fabs(area);
}

//http://stackoverflow.com/a/20861130
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

#pragma endregion
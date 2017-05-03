#include "stdafx.h"
#include "Log.h"
#include <stdarg.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

void Log(const char *Text)
{
    Logger::WriteMessage(Text);
}
void Log(const wchar_t *Text)
{
    Logger::WriteMessage(Text);
}

void LogFormat(const char *Text, ...)
{
    char _Text[2048];
    va_list valist;

    // Build variable text buffer
    va_start(valist, Text);
    vsprintf_s(_Text, 2000, Text, valist);
    va_end(valist);

    // write to test log
    Logger::WriteMessage(_Text);
}

void LogFormat(const wchar_t *Text, ...)
{
    wchar_t _Text[2048];
    va_list valist;

    // Build variable text buffer
    va_start(valist, Text);
    vswprintf_s(_Text, 2000, Text, valist);
    va_end(valist);

    // write to test log
    Logger::WriteMessage(_Text);
}
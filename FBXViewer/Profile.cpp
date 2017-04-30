#include "stdafx.h"
#include "Windows.h"
#include "Profile.h"

Profile* Profile::Instance = new Profile();

void Profile::Start(const char* name)
{
	QueryPerformanceFrequency(&Instance->Frequency);
	QueryPerformanceCounter(&Instance->StartingTime);
	Instance->name = name;
}

void Profile::End()
{
	QueryPerformanceCounter(&Instance->EndingTime);
	Instance->ElapsedMicroseconds.QuadPart = Instance->EndingTime.QuadPart - Instance->StartingTime.QuadPart;
	double diff = (Instance->ElapsedMicroseconds.QuadPart * 1000000 / Instance->Frequency.QuadPart) / 1000.0;
	DebugPrintf("[Profile] <%s> uses %.3f ms",  Instance->name, diff);
}

Profile::Profile(){}

Profile::~Profile(){}
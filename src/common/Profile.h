#pragma once

class Profile
{
	static Profile* Instance;

	LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
	LARGE_INTEGER Frequency;
	const char* name;

public:
	static void Start(const char* name);// start a profile
	static void End();// end a profile

public:
	Profile();
	~Profile();
};


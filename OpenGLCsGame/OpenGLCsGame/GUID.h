#pragma once
class GUID
{
private:
	static unsigned long long _Uid;
public:
	static unsigned long long GetUid(); 
};


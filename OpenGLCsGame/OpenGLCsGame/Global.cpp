#include "Global.h"
Global* Global::Instace = nullptr;
Global::Global()
{
	Global::Instace = this;
}

Global* Global::Init()
{
	return new Global();
}

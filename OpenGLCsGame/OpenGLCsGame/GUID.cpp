#include "GUID.h"

unsigned long long GUID::_Uid = 1;
unsigned long long GUID::GetUid()
{
    return _Uid++;
}

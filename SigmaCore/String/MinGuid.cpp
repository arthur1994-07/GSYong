#include "stdafx.h"
#include "./MinGuid.h"


#include "../DebugInclude.h"

namespace sc
{

MGUID::MGUID()
{
    Reset();
}

MGUID::MGUID(const MGUID& id)
{
    (*this)=id;
}

MGUID::MGUID(
    ULONG _Data1,
    USHORT _Data2,
    USHORT _Data3,
    BYTE _BData0,
    BYTE _BData1,
    BYTE _BData2,
    BYTE _BData3,
    BYTE _BData4,
    BYTE _BData5,
    BYTE _BData6,
    BYTE _BData7)
{
    Data1 = _Data1;
    Data2 = _Data2;
    Data3 = _Data3;

    Data4[0] = _BData0;
    Data4[1] = _BData1;
    Data4[2] = _BData2;
    Data4[3] = _BData3;
    Data4[4] = _BData4;
    Data4[5] = _BData5;
    Data4[6] = _BData6;
    Data4[7] = _BData7;
}

MGUID& MGUID::operator=(const MGUID& rhs)
{
    Data1 = rhs.Data1;
    Data2 = rhs.Data2;
    Data3 = rhs.Data3;

    Data4[0] = rhs.Data4[0];
    Data4[1] = rhs.Data4[1];
    Data4[2] = rhs.Data4[2];
    Data4[3] = rhs.Data4[3];
    Data4[4] = rhs.Data4[4];
    Data4[5] = rhs.Data4[5];
    Data4[6] = rhs.Data4[6];
    Data4[7] = rhs.Data4[7];

    return (*this);
}

bool MGUID::operator==(const MGUID &rhs) const
{
    if (Data1 == rhs.Data1 &&
        Data2 == rhs.Data2 &&
        Data3 == rhs.Data3 &&
        Data4[0] == rhs.Data4[0] &&
        Data4[1] == rhs.Data4[1] &&
        Data4[2] == rhs.Data4[2] &&
        Data4[3] == rhs.Data4[3] &&
        Data4[4] == rhs.Data4[4] &&
        Data4[5] == rhs.Data4[5] &&
        Data4[6] == rhs.Data4[6] &&
        Data4[7] == rhs.Data4[7])
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool MGUID::operator!=(const MGUID &rhs) const
{
    return !((*this)==rhs);
}

bool MGUID::operator<(const MGUID &rhs) const
{
    if (Data1 != rhs.Data1)
        return Data1 < rhs.Data1;
    else if (Data2 != rhs.Data2)
        return Data2 < rhs.Data2;
    else if (Data3 != rhs.Data3)
        return Data3 < rhs.Data3;
    else if (Data4[0] != rhs.Data4[0])
        return Data4[0] < rhs.Data4[0];
    else if (Data4[1] != rhs.Data4[1])
        return Data4[1] < rhs.Data4[1];
    else if (Data4[2] != rhs.Data4[2])
        return Data4[2] < rhs.Data4[2];
    else if (Data4[3] != rhs.Data4[3])
        return Data4[3] < rhs.Data4[3];
    else if (Data4[4] != rhs.Data4[4])
        return Data4[4] < rhs.Data4[4];
    else if (Data4[5] != rhs.Data4[5])
        return Data4[5] < rhs.Data4[5];
    else if (Data4[6] != rhs.Data4[6])
        return Data4[6] < rhs.Data4[6];
    else if (Data4[7] != rhs.Data4[7])
        return Data4[7] < rhs.Data4[7];

    return false;
}

void MGUID::Reset()
{
    Data1 = 0;
    Data2 = 0;
    Data3 = 0;
    Data4[0] = 0;
    Data4[1] = 0;
    Data4[2] = 0;
    Data4[3] = 0;
    Data4[4] = 0;
    Data4[5] = 0;
    Data4[6] = 0;
    Data4[7] = 0;
}

} // namespace sc

#include "pch.h"

#include "ISerialFile.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

ISerialFile::ISerialFile()
{
}

ISerialFile::~ISerialFile()
{
}

BOOL ISerialFile::OpenFile(const ic::FOPENTYPE _Type, const char* _szFileName, bool bencode)
{
    FOPENTYPE emType = static_cast<FOPENTYPE>(_Type);
    return SerialFile::OpenFile( emType, _szFileName, bencode );
}

void ISerialFile::CloseFile()
{
    SerialFile::CloseFile();
}

BOOL ISerialFile::SetOffSet(long _OffSet)
{
    return SerialFile::SetOffSet(_OffSet);
}

long ISerialFile::GetfTell()
{
    return SerialFile::GetfTell();
}

BOOL ISerialFile::BeginBlock()
{
    return SerialFile::BeginBlock();
}

BOOL ISerialFile::EndBlock()
{
    return SerialFile::EndBlock();
}

// ------- 저장하기.
BOOL ISerialFile::operator << (short Value)
{
    return SerialFile::operator << (Value);
}

BOOL ISerialFile::operator << (int Value)
{
    return SerialFile::operator << (Value);
}

BOOL ISerialFile::operator << (WORD Value)
{
    return SerialFile::operator << (Value);
}

BOOL ISerialFile::operator << (DWORD Value)
{
    return SerialFile::operator << (Value);
}

BOOL ISerialFile::operator << (UINT Value)
{
    return SerialFile::operator << (Value);
}

BOOL ISerialFile::operator << (float Value)
{
    return SerialFile::operator << (Value);
}

BOOL ISerialFile::operator << (double Value)
{
    return SerialFile::operator << (Value);
}	

BOOL ISerialFile::operator << (BYTE Value)
{
    return SerialFile::operator << (Value);
}

BOOL ISerialFile::operator << (char Value)
{
    return SerialFile::operator << (Value);
}

BOOL ISerialFile::operator << (bool Value)
{
    return SerialFile::operator << (Value);
}

BOOL ISerialFile::operator << (const std::string& str)
{
    return SerialFile::operator << (str);
}

BOOL ISerialFile::operator << (LONGLONG Value)
{
    return SerialFile::operator << (Value);
}

BOOL ISerialFile::operator << (const D3DVECTOR& Value)
{
    return SerialFile::operator << (Value);
}

BOOL ISerialFile::operator << (const D3DCOLORVALUE& Value)
{
    return SerialFile::operator << (Value);
}

BOOL ISerialFile::operator << (const D3DBLEND& Value)
{
    return SerialFile::operator << (Value);
}

// ------- 불러오기.
BOOL ISerialFile::operator >> (short& Value)
{
    return SerialFile::operator >> (Value);
}

BOOL ISerialFile::operator >> (int& Value)
{
    return SerialFile::operator >> (Value);
}

BOOL ISerialFile::operator >> (WORD& Value)
{
    return SerialFile::operator >> (Value);
}

BOOL ISerialFile::operator >> (DWORD& Value)
{
    return SerialFile::operator >> (Value);
}

BOOL ISerialFile::operator >> (UINT& Value)
{
    return SerialFile::operator >> (Value);
}

BOOL ISerialFile::operator >> (float& Value)
{
    return SerialFile::operator >> (Value);
}

BOOL ISerialFile::operator >> (double& Value)
{
    return SerialFile::operator >> (Value);
}

BOOL ISerialFile::operator >> (BYTE& Value)
{
    return SerialFile::operator >> (Value);
}

BOOL ISerialFile::operator >> (char& Value)
{
    return SerialFile::operator >> (Value);
}

BOOL ISerialFile::operator >> (std::string& str)
{
    return SerialFile::operator >> (str);
}

BOOL ISerialFile::operator >> (bool& Value)
{
    return SerialFile::operator >> (Value);
}

BOOL ISerialFile::operator >> (LONGLONG& Value)
{
    return SerialFile::operator >> (Value);
}

BOOL ISerialFile::operator >> (D3DVECTOR &Value)
{
    return SerialFile::operator >> (Value);
}

BOOL ISerialFile::operator >> (D3DCOLORVALUE &Value)
{
    return SerialFile::operator >> (Value);
}

BOOL ISerialFile::operator >> (D3DBLEND &Value)
{
    return SerialFile::operator >> (Value);
}

std::auto_ptr< ic::SerialFileInterface > ISerialFile_Func()
{
    return std::auto_ptr< ic::SerialFileInterface >( new ISerialFile );
}
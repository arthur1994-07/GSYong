#pragma once

#include "../../InternalCommonLib/Interface/SerialFileInterface.h"
#include "../../SigmaCore/File/SerialFile.h"

class ISerialFile : public sc::SerialFile, public ic::SerialFileInterface
{
public:
    virtual BOOL OpenFile(const ic::FOPENTYPE _Type, const char* _szFileName, bool bencode=false);
    virtual void CloseFile();
    virtual BOOL SetOffSet(long _OffSet);
    virtual long GetfTell();
    virtual BOOL BeginBlock();
    virtual BOOL EndBlock();

    // ------- 저장하기.
public:
    virtual BOOL operator << (short Value);
    virtual BOOL operator << (int Value);
    virtual BOOL operator << (WORD Value);
    virtual BOOL operator << (DWORD Value);
    virtual BOOL operator << (UINT Value);
    virtual BOOL operator << (float Value);
    virtual BOOL operator << (double Value);	
    virtual BOOL operator << (BYTE Value);
    virtual BOOL operator << (char Value);
    virtual BOOL operator << (bool Value);
    virtual BOOL operator << (const std::string& str);
    virtual BOOL operator << (LONGLONG Value);
    virtual BOOL operator << (const D3DVECTOR& Value);
    virtual BOOL operator << (const D3DCOLORVALUE& Value);
    virtual BOOL operator << (const D3DBLEND& Value);

    // ------- 불러오기.
public:
    virtual BOOL operator >> (short& Value);
    virtual BOOL operator >> (int& Value);
    virtual BOOL operator >> (WORD& Value);
    virtual BOOL operator >> (DWORD& Value);
    virtual BOOL operator >> (UINT& Value);
    virtual BOOL operator >> (float& Value);
    virtual BOOL operator >> (double& Value);
    virtual BOOL operator >> (BYTE& Value);
    virtual BOOL operator >> (char& Value);
    virtual BOOL operator >> (std::string& str);
    virtual BOOL operator >> (bool& Value);
    virtual BOOL operator >> (LONGLONG& Value);
    virtual BOOL operator >> (D3DVECTOR &Value);
    virtual BOOL operator >> (D3DCOLORVALUE &Value);
    virtual BOOL operator >> (D3DBLEND &Value);

public:
    ISerialFile();
    virtual ~ISerialFile();
};

std::auto_ptr< ic::SerialFileInterface > ISerialFile_Func();
// by ���
//
#pragma once

#include "../../SigmaCore/File/SerialMemory.h"

class SerialMemoryEngine : public sc::SerialMemory
{
public:
    SerialMemoryEngine();
    SerialMemoryEngine(const char *_szZipFileName, const char *_szFileName, bool bencode = false);
    virtual ~SerialMemoryEngine();

    // ------- �ҷ�����.
public:
	virtual BOOL operator >> ( D3DVECTOR &Value );
	virtual BOOL operator >> ( D3DCOLORVALUE &Value );
	virtual BOOL operator >> ( D3DBLEND &Value );
};
#include "pch.h"
#include "../../SigmaCore/Encrypt/CompByte.h"
#include "../../SigmaCore/File/SFileSystem.h"
#include "../../SigmaCore/Compress/Unzipper.h"
#include "../../SigmaCore/File/SFileSystem.h"

#include "./SerialMemoryEngine.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

SerialMemoryEngine::SerialMemoryEngine(void) 
	: sc::SerialMemory()
{
}

SerialMemoryEngine::SerialMemoryEngine(const char *_szZipFileName, const char *_szFileName, bool bencode) 
    : sc::SerialMemory(_szZipFileName, _szFileName, bencode)
{
}

SerialMemoryEngine::~SerialMemoryEngine(void)
{
}

BOOL SerialMemoryEngine::operator >> ( D3DVECTOR &Value )
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL SerialMemoryEngine::operator >> ( D3DCOLORVALUE &Value )
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

BOOL SerialMemoryEngine::operator >> ( D3DBLEND &Value )
{
	GASSERT(m_OpenType==FOT_READ);
	int Num = 0;

	Num = (int)read ( &Value, sizeof(Value) );

	return TRUE;
}

#include "pch.h"
#include "./EncryptDataDef.h"

#include "../../SigmaCore/Util/minTea.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace ENCRYPT_DATA
{
	static sc::minTea s_Tea;

	void InitKey()
	{
		const unsigned char szKey[ TEA_KEY_LENGTH + 1 ] = {
            0x3F, 0x14, 0x03, 0x47, 0xA2, 0x94, 0xBF, 0xF0,
            0xFF, 0x39, 0x81, 0xCA, 0xC3, 0x75, 0x84, 0x49, 0x00 };

		s_Tea.setKey( (char*)szKey );
	}

    bool Encrypt( char* pBuffer, int nSize )
    {
        return s_Tea.encrypt( pBuffer, nSize );
    }

    bool Decrypt( char* pBuffer, int nSize )
    {
        return s_Tea.decrypt( pBuffer, nSize );
    }
}
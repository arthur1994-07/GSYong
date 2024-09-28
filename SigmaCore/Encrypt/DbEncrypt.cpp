#include "StdAfx.h"
#include "DbEncrypt.h"

// ----------------------------------------------------------------------------
#include "../DebugInclude.h"
// ----------------------------------------------------------------------------

namespace sc
{
    DbEncrypt::DbEncrypt()
        : RC5EncryptA(std::string("MooHanDoJun"))
    {
    }

    DbEncrypt::~DbEncrypt()
    {
    }
}
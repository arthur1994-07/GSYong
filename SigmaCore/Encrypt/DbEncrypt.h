#ifndef _SC_DB_ENCRYPT_H_
#define _SC_DB_ENCRYPT_H_

#pragma once

#include "RC5Encrypt.h"

namespace sc
{
    class DbEncrypt : public RC5EncryptA
    {
    public:
        DbEncrypt();
        virtual ~DbEncrypt();
    };
}

#endif // _SC_DB_ENCRYPT_H_
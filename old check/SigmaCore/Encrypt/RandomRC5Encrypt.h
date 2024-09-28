#ifndef _SC_RANDOM_RC5_ENCRYPT_H_
#define _SC_RANDOM_RC5_ENCRYPT_H_

#pragma once

namespace sc
{
    class RC5EncryptA;

    class RandomRC5
    {
    public:
        enum { DEFAULT_KEY_LENGTH=7, };

    public:
        //! Key �� �����ʾƵ� �ڵ����� �����ȴ�.
        RandomRC5(const std::string& Key, size_t KeyLength=DEFAULT_KEY_LENGTH);
        ~RandomRC5();

    protected:
        std::string m_Key;
        RC5EncryptA* m_pRc5;

    public:        
        std::string GetKey() const;
        std::string GetEncrypt(const std::string& strSrc);
        std::string GetDecrypt(const std::string& strSrc);
    };
}

#endif // _SC_RANDOM_RC5_ENCRYPT_H_
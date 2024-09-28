#ifndef _RC5_ENCRYPT_H_
#define _RC5_ENCRYPT_H_

#pragma once

#include <string>

//#include "../../XLib_CryptoPP54/aes.h"
//#include "../../XLib_CryptoPP54/blowfish.h"
//#include "../../XLib_CryptoPP54/camellia.h"
//#include "../../XLib_CryptoPP54/cast.h"
//#include "../../XLib_CryptoPP54/des.h"
//#include "../../XLib_CryptoPP54/gost.h"
//#include "../../XLib_CryptoPP54/idea.h"
//#include "../../XLib_CryptoPP54/mars.h"
//#include "../../XLib_CryptoPP54/rc2.h"
#include "../../=CryptoPP/rc5.h"
//#include "../../XLib_CryptoPP54/rc6.h"
//#include "../../XLib_CryptoPP54/Rijndael.h"
//#include "../../XLib_CryptoPP54/safer.h"
//#include "../../XLib_CryptoPP54/serpent.h"
//#include "../../XLib_CryptoPP54/shacal2.h"
//#include "../../XLib_CryptoPP54/shark.h"
//#include "../../XLib_CryptoPP54/skipjack.h"
//#include "../../XLib_CryptoPP54/tea.h"
//#include "../../XLib_CryptoPP54/3way.h"
//#include "../../XLib_CryptoPP54/twofish.h"

// Cipher Modes
//
// #define CIPHER_MODE CBC_CTS_Mode
#define CIPHER_MODE CBC_Mode
// #define CIPHER_MODE CFB_FIPS_Mode
// #define CIPHER_MODE CFB_Mode
// #define CIPHER_MODE CTR_Mode
// #define CIPHER_MODE ECB_Mode
// #define CIPHER_MODE OFB_Mode

// Ciphers
//
//#define CIPHER AES
// #define CIPHER Blowfish
// #define CIPHER BTEA
// #define CIPHER Camellia
// #define CIPHER CAST128
// #define CIPHER CAST256
// #define CIPHER DES
// #define CIPHER DES_EDE2
// #define CIPHER DES_EDE3
// #define CIPHER DES_XEX3
// #define CIPHER GOST
// #define CIPHER IDEA
// #define CIPHER MARS
// #define CIPHER RC2
#define CIPHER RC5
// #define CIPHER RC6
// #define CIPHER Rijndael
// #define CIPHER SAFER_K
// #define CIPHER SAFER_SK
// #define CIPHER Serpent
// #define CIPHER SHACAL2
// #define CIPHER SHARK
// #define CIPHER SKIPJACK
// #define CIPHER ThreeWay
// #define CIPHER Twofish
// #define CIPHER XTEA

namespace sc
{
	class RC5Encrypt
	{
	public:
		RC5Encrypt(const std::wstring& Password);
		virtual ~RC5Encrypt();

	protected:
		byte m_key[CryptoPP::CIPHER::DEFAULT_KEYLENGTH];
		byte m_iv[CryptoPP::CIPHER::BLOCKSIZE];

	public:
		bool encrypt(const std::wstring& strSrc, std::wstring& strTarget);
		bool decrypt(const std::wstring& strSrc, std::wstring& strTarget);
		bool decrypt(const std::string& strSrc, std::string& strTarget);
		std::wstring getEncrypt(const std::wstring& strSrc);
		std::wstring getDecrypt(const std::wstring& strSrc);
		std::string  getDecryptA(const std::string& strSrc);
	};

	class RC5EncryptA
	{
	public:
		RC5EncryptA(const std::string& Password);
		RC5EncryptA(const std::wstring& Password);
		virtual ~RC5EncryptA();

	protected:
		byte m_key[CryptoPP::CIPHER::DEFAULT_KEYLENGTH];
		byte m_iv[CryptoPP::CIPHER::BLOCKSIZE];

	public:
		bool encrypt(const std::string& strSrc, std::string& strTarget);
		bool decrypt(const std::string& strSrc, std::string& strTarget);

		std::string getEncrypt(const std::string& strSrc);
		std::string getDecrypt(const std::string& strSrc);
	};

}

#endif 
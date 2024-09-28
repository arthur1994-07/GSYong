#include "stdafx.h"

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

#include <iostream>
// Crypto++ Includes
#include "../../=CryptoPP/modes.h" // xxx_Mode< >
#include "../../=CryptoPP/filters.h" // StringSource and StreamTransformation
#include "../../=CryptoPP/hex.h"
#include "../String/StringUtil.h"
#include "../Hash/MinMd5.h"

#include "RC5Encrypt.h"

// ----------------------------------------------------------------------------
#include "../DebugInclude.h"
// ----------------------------------------------------------------------------

namespace sc
{

RC5Encrypt::RC5Encrypt( const std::wstring& Password )
{
	CString strMd5;
	MinMd5 Md5;
	Md5.getMd5( CString( Password.c_str() ), strMd5 );

	// Key and IV setup
	::memcpy( m_key, strMd5.GetBuffer(), CryptoPP::CIPHER::DEFAULT_KEYLENGTH );
	::memset( m_iv, 0x01, CryptoPP::CIPHER::BLOCKSIZE );
    strMd5.ReleaseBuffer();
}

RC5Encrypt::~RC5Encrypt()
{
}

std::wstring RC5Encrypt::getEncrypt( const std::wstring& strSrc )
{
	std::wstring strTemp;
	encrypt( strSrc, strTemp );
	return strTemp;
}

std::wstring RC5Encrypt::getDecrypt( const std::wstring& strSrc )
{
	std::wstring strTemp;
	decrypt( strSrc, strTemp );
	return strTemp;
}

std::string RC5Encrypt::getDecryptA( const std::string& strSrc )
{
	std::string strTemp;
	decrypt( strSrc, strTemp );
	return strTemp;
}

bool RC5Encrypt::encrypt( const std::wstring& strSrc, std::wstring& strTarget )
{
	try
	{		
        std::string PlainText = sc::string::unicodeToAnsi( strSrc.c_str(), (int) strSrc.size() );

		// Cipher Text Sink
		std::string CipherText;

		// Encryptor
		CryptoPP::CIPHER_MODE<CryptoPP::CIPHER>::Encryption	Encryptor( m_key, sizeof(m_key), m_iv );

		// Encryption
		CryptoPP::StringSource(
			PlainText,
			true,
			new CryptoPP::StreamTransformationFilter(
			Encryptor,
			new CryptoPP::StringSink( CipherText ) ) // StreamTransformationFilter
			); // StringSource

        std::string HexCode = sc::string::HexEncodeA( (BYTE*) CipherText.c_str(), CipherText.size() );
        strTarget = sc::string::ansiToUnicode( HexCode.c_str(), (int) HexCode.size() );

		return true;
	}
	catch( CryptoPP::Exception& e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}
	catch(...)
	{
		std::cerr << "Unknown Error" << std::endl;
		return false;
	}
}

bool RC5Encrypt::decrypt( const std::wstring& strSrc, std::wstring& strTarget )
{
	try
	{
		std::string strSrcA = sc::string::unicodeToAnsi( strSrc.c_str(), (int) strSrc.size() );

		// Cipher Text Sink
		std::string CipherText = sc::string::HexDecodeA( strSrcA );

		// Recovered Text Sink
		std::string RecoveredText;

		// Decryptor
		CryptoPP::CIPHER_MODE<CryptoPP::CIPHER>::Decryption	Decryptor( m_key, sizeof(m_key), m_iv );

		// Decryption
		CryptoPP::StringSource(
			CipherText,
			true,
			new CryptoPP::StreamTransformationFilter(
			Decryptor,
			new CryptoPP::StringSink( RecoveredText ) ) // StreamTransformationFilter
			); // StringSource

		strTarget = sc::string::ansiToUnicode( RecoveredText.c_str(), (int) RecoveredText.size() );

		return true;
	}
	catch( CryptoPP::Exception& e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}
	catch(...)
	{
		std::cerr << "Unknown Error" << std::endl;
		return false;
	}  
}

bool RC5Encrypt::decrypt( const std::string& strSrc, std::string& strTarget )
{
	try
	{
		// Cipher Text Sink
		std::string CipherText = sc::string::HexDecodeA( strSrc );

		// Recovered Text Sink
		std::string RecoveredText;

		// Decryptor
		CryptoPP::CIPHER_MODE<CryptoPP::CIPHER>::Decryption	Decryptor( m_key, sizeof(m_key), m_iv );

		// Decryption
		CryptoPP::StringSource(
			CipherText,
			true,
			new CryptoPP::StreamTransformationFilter(
			Decryptor,
			new CryptoPP::StringSink( RecoveredText ) ) // StreamTransformationFilter
			); // StringSource

		strTarget = RecoveredText;

		return true;
	}
	catch( CryptoPP::Exception& e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}
	catch(...)
	{
		std::cerr << "Unknown Error" << std::endl;
		return false;
	}  
}

RC5EncryptA::RC5EncryptA( const std::string& Password )
{	
	std::string Md5 = sc::string::getMd5A(Password);

	// Key and IV setup
	::memcpy( m_key, Md5.c_str(), CryptoPP::CIPHER::DEFAULT_KEYLENGTH );
	::memset( m_iv, 0x01, CryptoPP::CIPHER::BLOCKSIZE );
}

RC5EncryptA::RC5EncryptA( const std::wstring& Password )
{
	std::string Md5 = sc::string::getMd5A( sc::string::unicodeToUtf8(Password) );

	// Key and IV setup
	::memcpy( m_key, Md5.c_str(), CryptoPP::CIPHER::DEFAULT_KEYLENGTH );
	::memset( m_iv, 0x01, CryptoPP::CIPHER::BLOCKSIZE );
}

RC5EncryptA::~RC5EncryptA()
{
}

bool RC5EncryptA::encrypt( const std::string& strSrc, std::string& strTarget )
{
	try
	{		
		std::string PlainText = strSrc;

		// Cipher Text Sink
		std::string CipherText;

		// Encryptor
		CryptoPP::CIPHER_MODE<CryptoPP::CIPHER>::Encryption	Encryptor( m_key, sizeof(m_key), m_iv );

		// Encryption
		CryptoPP::StringSource(
			PlainText,
			true,
			new CryptoPP::StreamTransformationFilter(
			Encryptor,
			new CryptoPP::StringSink( CipherText ) ) // StreamTransformationFilter
			); // StringSource

        std::string HexCode = sc::string::HexEncodeA( (BYTE*) CipherText.c_str(), CipherText.size() );
		strTarget = HexCode;

		return true;
	}
	catch( CryptoPP::Exception& e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}
	catch(...)
	{
		std::cerr << "Unknown Error" << std::endl;
		return false;
	}
}

std::string RC5EncryptA::getEncrypt( const std::string& strSrc )
{
	std::string strTemp;
	encrypt( strSrc, strTemp );
	return strTemp;
}

std::string RC5EncryptA::getDecrypt( const std::string& strSrc )
{
	std::string strTemp;
	decrypt( strSrc, strTemp );
	return strTemp;
}

bool RC5EncryptA::decrypt( const std::string& strSrc, std::string& strTarget )
{
	try
	{
		std::string strSrcA = strSrc;

		// Cipher Text Sink
		std::string CipherText = sc::string::HexDecodeA( strSrcA );

		// Recovered Text Sink
		std::string RecoveredText;

		// Decryptor
		CryptoPP::CIPHER_MODE<CryptoPP::CIPHER>::Decryption	Decryptor( m_key, sizeof(m_key), m_iv );

		// Decryption
		CryptoPP::StringSource(
			CipherText,
			true,
			new CryptoPP::StreamTransformationFilter(
			Decryptor,
			new CryptoPP::StringSink( RecoveredText ) ) // StreamTransformationFilter
			); // StringSource

		strTarget = RecoveredText;

		return true;
	}
	catch( CryptoPP::Exception& e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}
	catch(...)
	{
		std::cerr << "Unknown Error" << std::endl;
		return false;
	}  
}

}
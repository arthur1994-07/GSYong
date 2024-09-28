#pragma once

#include "../../=CryptoPP/aes.h"
#include "../../=CryptoPP/dh.h"
#include "../../=CryptoPP/modes.h"
#include "../Util/Lock.h"

namespace sc
{

/**
 * \date 2007-04-01 Jgkim
 * \author Jgkim
 * \par license
 * Copyright (c) Min Communications. All rights reserved.
 */
class Aes
{
public:
	enum AES{
		SHA256_DIGESTSIZE = CryptoPP::SHA256::DIGESTSIZE,
		AES_BLOCKSIZE     = CryptoPP::AES::BLOCKSIZE,
		AES_KEYSIZE       = 32, // 32 * 8 = 256 bit 암호화...
	};

public:
	static DefaultLock m_DigestLock;
	static void CalculateDigest(byte* digest, const byte* input, size_t length);
	
public:
	Aes();
	Aes(BYTE* pBuffer);
	virtual ~Aes();

	//! server 의 public key 를 가져온다.
	//! 이 key 는 client 에 전송해야 한다.
	//! \param pPubKey key 를 받을 버퍼 32 byte 이상
	//! \param nKeyLength 공개키의 크기를 돌려준다.
	void getServerPubKey(BYTE* pPubKey, int& nKeyLength OUT);

	//! client 의 public key 와 IV 테이블을 돌려준다.
	//! 이 key 와 IV 테이블을 서버로 전송한다.
	//! \param pPubKey public key 와 iv 테이블을 받을 버퍼 34 byte 이상
	//! \param nKeyLength public key 와 IV 테이블의 크기
	void getClientPubKey(BYTE* pPubKey, int& nKeyLength OUT);
	
	//! client 에서 받은 publickey 와 iv 테이블을 서버에
	//! 세팅한다...
	//! pBuffer
	bool setClientPubKey(BYTE* pBuffer IN);

	//! client 와 server 가 public key 와 iv 테이블을 교환했다면
	//! 버퍼를 암호화 할 수 있다.
	//! \param pBuffer 암호화할 버퍼
	//! \nLength 암호화할 버퍼의 길이.
	bool encrypt(BYTE* pBuffer, int nLength);

	//! client 와 server 가 public key 와 iv 테이블을 교환했다면
	//! 버퍼를 복호하 할 수 있다.
	//! \param pBuffer 복호화할 버퍼
	//! \nLength 복호화할 버퍼의 길이.
	bool decrypt( BYTE* pBuffer, int nLength );

	// test code
	CryptoPP::SecByteBlock* getSecretKey() { return m_pSecretKey; }
	CryptoPP::DH* getDH() { return m_pDh; }
	byte* getIV() { return m_iv; }

protected:
	CryptoPP::DH* m_pDh;
	CryptoPP::Integer m_iPrime;
	CryptoPP::Integer m_iGenerator;
	CryptoPP::SecByteBlock* m_pPrivKey; //! 개인키
	CryptoPP::SecByteBlock* m_pPubKey; //! 공개키
	CryptoPP::SecByteBlock* m_pSecretKey; //! 암호키

	//! SHA-256 방식의 hash를 암호키로 사용한다.
	//! Diffie-Hellman session key
	CryptoPP::SecByteBlock* m_pSessionKey;

	//! 암호화 복호화에 사용할 IV 테이블
	//! IV 테이블이 server/client 가 같아야 된다.
	byte m_iv[AES_BLOCKSIZE];

	//! public key 와 iv table 교환이 모두 끝나면
	//! server/client 모두 true 로 세팅된다.
	bool m_bInit;

	std::string m_ErrorMsg;

public:
	std::string getLastError() { return m_ErrorMsg; }
};

} // namespace sc

/*
Example)

sc::Aes AesServer;

void AesTest()
{	
	///////////////////////////////////////////////////////////////////////
	// Server	
	BYTE pubkeyServer[256]={0};
	int nKeyLength1=0;
	AesServer.getServerPubKey( pubkeyServer, nKeyLength1 );

	// Server->Client send pubkey, iPrime, iGenerator

	///////////////////////////////////////////////////////////////////////
	// Client
	CORE_COMMON::Aes AesClient(pubkeyServer);
	BYTE pubkeyClient[256]={0};
	int nKeyLength2=0;
	AesClient.getClientPubKey( pubkeyClient, nKeyLength2 );

	// Client->Server send pubkey

	///////////////////////////////////////////////////////////////////////
	// Server
	AesServer.setClientPubKey( pubkeyClient );


	// check secret key
	//if ( memcmp(
	//	  AesClient.getSecretKey()->begin(),
	//	  AesServer.getSecretKey()->begin(),
	//	  AesServer.getDH()->AgreedValueLength() ) != 0
	//   )
	//{
	//	// wrong secret key
	//	int a=1+1;
	//}

	//// check iv
	//if ( memcmp(
	//	  AesClient.getIV(),
	//	  AesServer.getIV(),
	//	  CryptoPP::AES::BLOCKSIZE ) != 0
	//   )
	//{
	//	int a=1+1;
	//}
	
	BYTE RandomByte[100];
	CryptoPP::AutoSeededRandomPool arngA;
	arngA.GenerateBlock( RandomByte, 100 );

	BYTE RandomByte1[100];
	memcpy( RandomByte1, RandomByte, 100 );

	// Encrypt
	AesClient.encrypt( (BYTE*) RandomByte, 100 );
	// Decrypt
	AesServer.decrypt( (BYTE*) RandomByte, 100 );

	if ( memcmp(
		  RandomByte,
		  RandomByte1,
		  100 ) != 0	   
	   )
	{
		// not same ...
		MessageBox( _T("ERROR") );
	}
}
*/
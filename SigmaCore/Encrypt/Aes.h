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
		AES_KEYSIZE       = 32, // 32 * 8 = 256 bit ��ȣȭ...
	};

public:
	static DefaultLock m_DigestLock;
	static void CalculateDigest(byte* digest, const byte* input, size_t length);
	
public:
	Aes();
	Aes(BYTE* pBuffer);
	virtual ~Aes();

	//! server �� public key �� �����´�.
	//! �� key �� client �� �����ؾ� �Ѵ�.
	//! \param pPubKey key �� ���� ���� 32 byte �̻�
	//! \param nKeyLength ����Ű�� ũ�⸦ �����ش�.
	void getServerPubKey(BYTE* pPubKey, int& nKeyLength OUT);

	//! client �� public key �� IV ���̺��� �����ش�.
	//! �� key �� IV ���̺��� ������ �����Ѵ�.
	//! \param pPubKey public key �� iv ���̺��� ���� ���� 34 byte �̻�
	//! \param nKeyLength public key �� IV ���̺��� ũ��
	void getClientPubKey(BYTE* pPubKey, int& nKeyLength OUT);
	
	//! client ���� ���� publickey �� iv ���̺��� ������
	//! �����Ѵ�...
	//! pBuffer
	bool setClientPubKey(BYTE* pBuffer IN);

	//! client �� server �� public key �� iv ���̺��� ��ȯ�ߴٸ�
	//! ���۸� ��ȣȭ �� �� �ִ�.
	//! \param pBuffer ��ȣȭ�� ����
	//! \nLength ��ȣȭ�� ������ ����.
	bool encrypt(BYTE* pBuffer, int nLength);

	//! client �� server �� public key �� iv ���̺��� ��ȯ�ߴٸ�
	//! ���۸� ��ȣ�� �� �� �ִ�.
	//! \param pBuffer ��ȣȭ�� ����
	//! \nLength ��ȣȭ�� ������ ����.
	bool decrypt( BYTE* pBuffer, int nLength );

	// test code
	CryptoPP::SecByteBlock* getSecretKey() { return m_pSecretKey; }
	CryptoPP::DH* getDH() { return m_pDh; }
	byte* getIV() { return m_iv; }

protected:
	CryptoPP::DH* m_pDh;
	CryptoPP::Integer m_iPrime;
	CryptoPP::Integer m_iGenerator;
	CryptoPP::SecByteBlock* m_pPrivKey; //! ����Ű
	CryptoPP::SecByteBlock* m_pPubKey; //! ����Ű
	CryptoPP::SecByteBlock* m_pSecretKey; //! ��ȣŰ

	//! SHA-256 ����� hash�� ��ȣŰ�� ����Ѵ�.
	//! Diffie-Hellman session key
	CryptoPP::SecByteBlock* m_pSessionKey;

	//! ��ȣȭ ��ȣȭ�� ����� IV ���̺�
	//! IV ���̺��� server/client �� ���ƾ� �ȴ�.
	byte m_iv[AES_BLOCKSIZE];

	//! public key �� iv table ��ȯ�� ��� ������
	//! server/client ��� true �� ���õȴ�.
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
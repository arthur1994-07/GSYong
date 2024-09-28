#include "stdafx.h"
#include "../../=CryptoPP/osrng.h"
#include "../String/StringUtil.h"
#include "./Aes.h"

#include "../DebugInclude.h"

namespace sc
{

// 아무리 봐도 CryptoPP CalculateDigest 는 Thread Safe 하지 않다.
// 그래서 Lock 처리 한다.
sc::DefaultLock sc::Aes::m_DigestLock;

void Aes::CalculateDigest(byte *digest, const byte *input, size_t length)
{
    m_DigestLock.lockOn();
    CryptoPP::SHA256().CalculateDigest(digest, input, length);
    m_DigestLock.lockOff();
}

Aes::Aes()
    : m_pDh(NULL)
    , m_pPrivKey(NULL)
    , m_pPubKey(NULL)
    , m_pSecretKey(NULL)
    , m_pSessionKey(NULL)
    , m_bInit(false)
{
    ::memset( m_iv, 0, AES_BLOCKSIZE );

    CryptoPP::AutoSeededRandomPool arngA;
    CryptoPP::RandomNumberGenerator& rngA = *dynamic_cast<CryptoPP::RandomNumberGenerator *>(&arngA); 
    m_pDh = new CryptoPP::DH( rngA, 128 );

    // Extract the prime and base. These values could also have been hardcoded 
    // in the application
    m_iPrime     = m_pDh->GetGroupParameters().GetModulus();
    m_iGenerator = m_pDh->GetGroupParameters().GetSubgroupGenerator();

    // 개인키
    m_pPrivKey = new CryptoPP::SecByteBlock( m_pDh->PrivateKeyLength() );
    // 공개키
    m_pPubKey  = new CryptoPP::SecByteBlock( m_pDh->PublicKeyLength() );
    // 암호키
    m_pSecretKey = new CryptoPP::SecByteBlock( m_pDh->AgreedValueLength() );

    // Generate a pair of integers for Alice. The public integer is forwarded to Bob.
    m_pDh->GenerateKeyPair( rngA, m_pPrivKey->m_ptr, m_pPubKey->m_ptr );
}

Aes::Aes(BYTE* pBuffer)
    : m_pDh(NULL)
    , m_pPrivKey(NULL)
    , m_pPubKey(NULL)
    , m_pSecretKey(NULL)
    , m_pSessionKey(NULL)
    , m_bInit(false)
{
    try
    {
        ::memset( m_iv, 0, AES_BLOCKSIZE );

        // +-------------------------------------------------------------------------+
        // | PubKeySize | PubKey | iPrimeSize | iPrime | iGeneratorSize | iGenerator |
        // +-------------------------------------------------------------------------+
        int nPos=0;

        // PubKeySize
        BYTE PubKeySize=0;
        memcpy( &PubKeySize, pBuffer+nPos, sizeof(BYTE) );
        nPos += sizeof(BYTE);
        if ( PubKeySize < 0 || PubKeySize > AES_KEYSIZE )
            return;

        // PubKey
        BYTE PubKey[AES_KEYSIZE] = {0};
        memcpy( PubKey, pBuffer+nPos, PubKeySize );
        nPos += PubKeySize;

        CryptoPP::SecByteBlock PubKeyServer( PubKey, PubKeySize );

        // iPrimeSize
        BYTE iPrimeSize=0;
        memcpy( &iPrimeSize, pBuffer+nPos, sizeof(BYTE) );
        nPos += sizeof(BYTE);
        if ( iPrimeSize < 0 || iPrimeSize > AES_KEYSIZE )
            return;

        // iPrime
        BYTE iPrime[AES_KEYSIZE] = {0};
        memcpy( iPrime, pBuffer+nPos, iPrimeSize );
        nPos += iPrimeSize;

        // iGeneratorSize
        BYTE iGeneratorSize=0;
        memcpy( &iGeneratorSize, pBuffer+nPos, sizeof(BYTE) );
        nPos += sizeof(BYTE);
        if ( iGeneratorSize < 0 || iGeneratorSize > 1 )
            return;

        // iGenerator
        BYTE iGenerator[AES_KEYSIZE] = {0};
        memcpy( iGenerator, pBuffer+nPos, iGeneratorSize );
        nPos += iGeneratorSize;

        for (int nLoop=0; nLoop<iPrimeSize; ++nLoop)
        {
            m_iPrime.SetByte(nLoop, iPrime[nLoop]);
        }

        for (int nLoop=0; nLoop<iGeneratorSize; ++nLoop)
        {
            m_iGenerator.SetByte(nLoop, iGenerator[nLoop]);
        }

        CryptoPP::AutoSeededRandomPool arngA;
        CryptoPP::RandomNumberGenerator& rngA = *dynamic_cast<CryptoPP::RandomNumberGenerator *>(&arngA); 
        m_pDh = new CryptoPP::DH( m_iPrime, m_iGenerator );

        // 개인키
        m_pPrivKey = new CryptoPP::SecByteBlock( m_pDh->PrivateKeyLength() );
        // 공개키
        m_pPubKey  = new CryptoPP::SecByteBlock( m_pDh->PublicKeyLength() );
        // 암호키
        m_pSecretKey = new CryptoPP::SecByteBlock( m_pDh->AgreedValueLength() );

        // Generate a pair of integers for Alice. The public integer is forwarded to Bob.
        m_pDh->GenerateKeyPair( rngA, m_pPrivKey->m_ptr, m_pPubKey->m_ptr );

        if ( false == m_pDh->Agree( m_pSecretKey->m_ptr, m_pPrivKey->m_ptr, PubKeyServer ) )
        {
            return;
        }

        //int aesKeyLength = SHA256_DIGESTSIZE; // 32 bytes = 256 bit key
        //int defBlockSize = AES_BLOCKSIZE;

        // Calculate a SHA-256 hash
        SAFE_DELETE( m_pSessionKey );	
        m_pSessionKey = new CryptoPP::SecByteBlock( SHA256_DIGESTSIZE );
        //CryptoPP::SHA256().CalculateDigest( m_pSessionKey->m_ptr, m_pSecretKey->m_ptr, m_pSecretKey->size() );
        CalculateDigest( m_pSessionKey->m_ptr, m_pSecretKey->m_ptr, m_pSecretKey->size() );

        // Generate random IV
        arngA.GenerateBlock( m_iv, AES_BLOCKSIZE );
        m_bInit = true;
    }
    catch ( CryptoPP::Exception const& e )
    {
        m_ErrorMsg = std::string(e.what());
        m_bInit = false;
    }
    catch(...)
    {
        m_ErrorMsg = "AES Unknown Error";
        m_bInit = false;
    }
}

Aes::~Aes()
{	
    SAFE_DELETE( m_pDh );
    SAFE_DELETE( m_pPrivKey );
    SAFE_DELETE( m_pPubKey );
    SAFE_DELETE( m_pSecretKey );
    SAFE_DELETE( m_pSessionKey );
}

bool Aes::setClientPubKey(BYTE* pBuffer)
{
    try
    {
        // +---------------------+-------------------+
        // | PubKeySize | PubKey | IV size | IV Data |
        // +---------------------+-------------------+
        int nPos=0;

        // PubKeySize
        BYTE PubKeySize=0;
        memcpy( &PubKeySize, pBuffer+nPos, sizeof(BYTE) );
        nPos += sizeof(BYTE);
        if ( PubKeySize<0 || PubKeySize>AES_KEYSIZE )
            return false;

        // PubKey
        BYTE PubKey[AES_KEYSIZE] = {0};
        memcpy( PubKey, pBuffer+nPos, PubKeySize );
        nPos += PubKeySize;

        CryptoPP::SecByteBlock PubKeyServer( PubKey, PubKeySize );
        if (false == m_pDh->Agree( m_pSecretKey->m_ptr, m_pPrivKey->m_ptr, PubKeyServer ) )
            return false;

        // IV size
        BYTE ivSize=0;
        memcpy( &ivSize, pBuffer+nPos, sizeof(BYTE) );
        nPos += sizeof(BYTE);
        if ( ivSize<0 || ivSize>AES_BLOCKSIZE )
            return false;

        // IV Data
        memcpy( m_iv, pBuffer+nPos, ivSize );
        nPos += ivSize;

        // session key
        //int aesKeyLength = SHA256_DIGESTSIZE; // 32 bytes = 256 bit key
        //int defBlockSize = AES_BLOCKSIZE;

        // Calculate a SHA-256 hash over the Diffie-Hellman session key
        SAFE_DELETE( m_pSessionKey )		
            m_pSessionKey = new	CryptoPP::SecByteBlock( SHA256_DIGESTSIZE );
        //CryptoPP::SHA256().CalculateDigest( m_pSessionKey->m_ptr, m_pSecretKey->m_ptr, m_pSecretKey->size() );
        CalculateDigest( m_pSessionKey->m_ptr, m_pSecretKey->m_ptr, m_pSecretKey->size() );

        m_bInit = true;
        return true;
    }
    catch ( CryptoPP::Exception const& e )
    {
        m_ErrorMsg = std::string(e.what());
        m_bInit = false;
        return false;
    }
    catch(...)
    {
        m_ErrorMsg = "AES Unknown Error";
        m_bInit = false;
        return false;
    }
}

void Aes::getClientPubKey(BYTE* pPubKey, int& nKeyLength)
{
    int nPos=0;

    // +---------------------+-------------------+
    // | PubKeySize | PubKey | IV size | IV Data |
    // +---------------------+-------------------+
    // 1 + 16 + 1 + 16

    // PubKeySize
    BYTE PubKeySize = (BYTE) m_pPubKey->SizeInBytes();
    memcpy( pPubKey+nPos, &PubKeySize, sizeof(BYTE) );
    nPos += sizeof(BYTE); // 1 byte

    // PubKey
    memcpy( pPubKey+nPos, m_pPubKey->m_ptr, PubKeySize );
    nPos += PubKeySize; // 16 byte

    // IV Size
    BYTE ivSize = AES_BLOCKSIZE;
    memcpy( pPubKey+nPos, &ivSize, sizeof(BYTE) );
    nPos += sizeof(BYTE); // 1 byte

    // IV Data
    memcpy( pPubKey+nPos, m_iv, AES_BLOCKSIZE );
    nPos += AES_BLOCKSIZE; // 16 byte

    nKeyLength = nPos;
}

void Aes::getServerPubKey(BYTE* pPubKey, int& nKeyLength)
{
    int nPos=0;

    // +-------------------------------------------------------------------------+
    // | PubKeySize | PubKey | iPrimeSize | iPrime | iGeneratorSize | iGenerator |
    // +-------------------------------------------------------------------------+

    // PubKeySize
    BYTE PubKeySize = (BYTE) m_pPubKey->SizeInBytes();
    memcpy( pPubKey+nPos, &PubKeySize, sizeof(BYTE) );
    nPos += sizeof(BYTE);

    // PubKey
    memcpy( pPubKey+nPos, m_pPubKey->m_ptr, PubKeySize );
    nPos += PubKeySize; // 16 byte	

    // iPrimeSize
    unsigned int nCount = m_iPrime.ByteCount(); // 16 byte
    memcpy( pPubKey+nPos, &nCount, sizeof(BYTE) );
    nPos += sizeof(BYTE);

    // iPrime
    for (unsigned int nLoop=0; nLoop<nCount; ++nLoop)
    {
        byte _byte = m_iPrime.GetByte(nLoop);
        memcpy( pPubKey+nPos, (const void*) &_byte , sizeof(byte) );
        nPos += sizeof(byte);
    }

    // iGeneratorSize
    unsigned int nCount2 = m_iGenerator.ByteCount(); // 1 byte
    memcpy( pPubKey+nPos, &nCount2, sizeof(BYTE) );
    nPos += sizeof(BYTE);

    // iGeneratorSize
    for (unsigned int nLoop=0; nLoop<nCount2; ++nLoop)
    {
        byte _byte = m_iGenerator.GetByte(nLoop);
        memcpy( pPubKey+nPos, (const void*) &_byte , sizeof(byte) );
        nPos += sizeof(byte);
    }

    nKeyLength = nPos;
    // 16 + 1 + 16 + 1 + 1 + 1 = 36 byte
}

bool Aes::encrypt( BYTE* pBuffer, int nLength )
{
    try
    {
        if ( m_pSessionKey == NULL || m_bInit == false )
            return false;

        CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption cfbEncryption(
            m_pSessionKey->m_ptr,
            SHA256_DIGESTSIZE,
            m_iv );

        cfbEncryption.ProcessData(
            (byte*) pBuffer,
            (byte*) pBuffer,
            nLength );
        return true;
    }
    catch ( CryptoPP::Exception const& e )
    {
        m_ErrorMsg = std::string(e.what());
        return false;
    }
    catch(...)
    {
        m_ErrorMsg = "AES Unknown Error";
        return false;
    }
}

bool Aes::decrypt( BYTE* pBuffer, int nLength )
{
    try
    {
        if ( m_pSessionKey == NULL || m_bInit == false )
            return false;

        CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption cfbDecryption(
            m_pSessionKey->m_ptr,
            SHA256_DIGESTSIZE,
            m_iv );

        cfbDecryption.ProcessData(
            (byte*) pBuffer,
            (byte*) pBuffer,
            nLength );
        return true;
    }
    catch ( CryptoPP::Exception const& e )
    {
        m_ErrorMsg = std::string(e.what());
        return false;
    }
    catch(...)
    {
        m_ErrorMsg = "AES Unknown Error";
        return false;
    }
}

} // namespace sc
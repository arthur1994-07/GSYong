#include "pch.h"
#include "GlobalAuthManager.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../SigmaCore/String/StringFormat.h"

namespace sc
{
// namespace sc START
CGlobalAuthManager::CGlobalAuthManager(void) : RC5EncryptA(std::string("mincoms"))
{
	m_FileName = getAppPath();
	m_FileName += "\\sa.dat";

	m_InternalFileName = getAppPath();
	m_InternalFileName += "\\country.dat";
}

CGlobalAuthManager::~CGlobalAuthManager(void)
{
}


BOOL CGlobalAuthManager::IsExistUniqKey()
{
	CFileException ex;
	CFile LoadFile;

	if ( FALSE == LoadFile.Open( m_FileName.c_str(), CFile::modeRead, &ex ) )
		return FALSE;

	return TRUE;
}

BOOL CGlobalAuthManager::CreateDefaultUniqKey()
{
	CFileException ex;
	CFile SaveFile;

	if ( FALSE == SaveFile.Open( m_FileName.c_str(), CFile::modeCreate | CFile::modeWrite, &ex ) )
	{
		SaveFile.Close();
		return FALSE;
	}

	m_authData.strUniqKey = "mincoms";

	std::string encryptStr;

	if ( false == encrypt( m_authData.strUniqKey, encryptStr ) )
		return FALSE;	
	
	int strLength = (int) encryptStr.length();

	SaveFile.Write( &strLength, sizeof(strLength) );
	SaveFile.Write( encryptStr.c_str(), sizeof(char)*(strLength+1) );
	SaveFile.Close();

	return TRUE;
}

BOOL CGlobalAuthManager::ReadUniqKey()
{
	CFileException ex;
	CFile LoadFile;

	if( !IsExistUniqKey() )
	{
		return CreateDefaultUniqKey();
	}

	if ( FALSE == LoadFile.Open( m_FileName.c_str(), CFile::modeRead, &ex ) )
		return FALSE;

	int strLength=0;
	char uniqKey[1024] = {0};

	LoadFile.Read( &strLength, sizeof(strLength) );
	LoadFile.Read( uniqKey, sizeof(char)*strLength );

	if ( false == decrypt( std::string(uniqKey), m_authData.strUniqKey ) )
		return FALSE;	

	return TRUE;
}

std::string CGlobalAuthManager::CombineAuthData()
{
	return sc::string::format( "%1%,%2%,%3%,%4%,%5%", m_authData.nServiceProvider, m_authData.nServerType, m_authData.strIP, m_authData.nPort, m_authData.strUniqKey );
}

std::string CGlobalAuthManager::CombineCertificationData()
{
	return sc::string::format( "%1%,%2%,%3%,%4%,%5%,%6%", m_authData.nServiceProvider, m_authData.nServerType, m_authData.strIP, m_authData.nPort, m_authData.strNewUniqKey, m_authData.nCertification );
}

std::string CGlobalAuthManager::DecryptAuthData(std::string strEncryptedAuthData)
{
	std::string strDecryptedData;

	if ( FALSE == decrypt( strEncryptedAuthData, strDecryptedData ) )
		return NULL;

	return strDecryptedData;
}

std::string CGlobalAuthManager::EncryptAuthData(std::string strDecryptedAuthData)
{
	std::string strEncryptedData;

	if ( FALSE == encrypt( strDecryptedAuthData, strEncryptedData ) )
		return NULL;

	return strEncryptedData;
}


BOOL CGlobalAuthManager::DecryptAuthData()
{
	if ( FALSE == decrypt( m_authData.strEncryptedAuthData, m_authData.strDecryptedAuthData ) )
		return FALSE;

	return TRUE;
}

BOOL CGlobalAuthManager::EncryptAuthData()
{
	if ( FALSE == encrypt( m_authData.strDecryptedAuthData, m_authData.strEncryptedAuthData ) )
		return FALSE;

	return TRUE;
}

BOOL CGlobalAuthManager::WriteUniqKey()
{
	CFileException ex;
	CFile SaveFile;

	if ( FALSE == SaveFile.Open( m_FileName.c_str(), CFile::modeCreate | CFile::modeWrite, &ex ) )
	{
		SaveFile.Close();
		return FALSE;
	}

	std::string encryptStr;
	
	if ( false == encrypt( m_authData.strUniqKey, encryptStr ) )
		return FALSE;	
	
	int strLength = (int) encryptStr.length();

	SaveFile.Write( &strLength, sizeof(strLength) );
	SaveFile.Write( encryptStr.c_str(), sizeof(char)*(strLength+1) );
	SaveFile.Close();

	return TRUE;
}

BOOL CGlobalAuthManager::ParsingAuthData(std::string authData)
{
	std::vector<std::string> splitVector;	
	boost::split( splitVector, authData, boost::algorithm::is_any_of( "," ) );

	m_authData.nServiceProvider = boost::lexical_cast<int> (splitVector[0]);
	m_authData.nServerType = boost::lexical_cast<int> (splitVector[1]);
	m_authData.strIP = splitVector[2];
	m_authData.nPort = boost::lexical_cast<int> (splitVector[3]);
	m_authData.strUniqKey = splitVector[4];

	return TRUE;
}

BOOL CGlobalAuthManager::ParsingRecvAuthData(std::string authData)
{
	std::vector<std::string> splitVector;	
	boost::split( splitVector, authData, boost::algorithm::is_any_of( "," ) );

	m_authData.nServiceProvider = boost::lexical_cast<int> (splitVector[0]);
	m_authData.nServerType = boost::lexical_cast<int> (splitVector[1]);
	m_authData.strIP = splitVector[2];
	m_authData.nPort = boost::lexical_cast<int> (splitVector[3]);
	m_authData.strUniqKey = splitVector[4];
	m_authData.nCertification = boost::lexical_cast<int> (splitVector[5]);

	return TRUE;
}

BOOL CGlobalAuthManager::ParsingRecvAuthData()
{
	std::vector<std::string> splitVector;	
	boost::split( splitVector, m_authData.strDecryptedAuthData, boost::algorithm::is_any_of( "," ) );

	m_authData.nServiceProvider = boost::lexical_cast<int> (splitVector[0]);
	m_authData.nServerType = boost::lexical_cast<int> (splitVector[1]);
	m_authData.strIP = splitVector[2];
	m_authData.nPort = boost::lexical_cast<int> (splitVector[3]);
	m_authData.strUniqKey = splitVector[4];
	m_authData.nCertification = boost::lexical_cast<int> (splitVector[5]);

	return TRUE;
}

BOOL CGlobalAuthManager::IsExistInternalKey()
{
	CFileException ex;
	CFile LoadFile;

	if ( FALSE == LoadFile.Open( m_InternalFileName.c_str(), CFile::modeRead, &ex ) )
		return FALSE;

	return TRUE;
}


BOOL CGlobalAuthManager::CreateInternalKey()
{
	CFileException ex;
	CFile SaveFile;

	if ( FALSE == SaveFile.Open( m_InternalFileName.c_str(), CFile::modeCreate | CFile::modeWrite, &ex ) )
	{
		SaveFile.Close();
		return FALSE;
	}

	// 키 생성 로직
	CTime curTime = CTime::GetCurrentTime();
	std::string strInternalKey = sc::string::format("%1%,%2%,%3%", curTime.GetYear(), curTime.GetMonth(), curTime.GetDay());
	
	std::string encryptStr;

	if ( false == encrypt( strInternalKey, encryptStr ) )
		return FALSE;	
	
	int strLength = (int) encryptStr.length();

	SaveFile.Write( &strLength, sizeof(strLength) );
	SaveFile.Write( encryptStr.c_str(), sizeof(char)*(strLength+1) );
	SaveFile.Close();

	return TRUE;
}

BOOL CGlobalAuthManager::CertificationInternalUniqKey()
{
	CFileException ex;
	CFile LoadFile;

	if( !IsExistInternalKey() )
	{
		CreateInternalKey();
	}

	if ( FALSE == LoadFile.Open( m_InternalFileName.c_str(), CFile::modeRead, &ex ) )
		return FALSE;

	int strLength=0;
	char internalKey[1024] = {0};

	LoadFile.Read( &strLength, sizeof(strLength) );
	LoadFile.Read( internalKey, sizeof(char)*strLength );

	std::string strInternalKey;

	if ( false == decrypt( std::string(internalKey), strInternalKey ) )
		return FALSE;	

	// 키 확인 로직

	//////////////////////////////////
	std::vector<std::string> splitVector;	
	boost::split( splitVector, strInternalKey, boost::algorithm::is_any_of( L"," ) );

	CTime curTime = CTime::GetCurrentTime();

	CTime expiredTime( atoi(splitVector[0].c_str()), atoi(splitVector[1].c_str()), atoi(splitVector[2].c_str()), 0, 0, 0 );
	expiredTime += CTimeSpan(30,0,0,0);

	if( curTime > expiredTime )
		return FALSE;

	return TRUE;
}

// namespace sc END
}
#include "StdAfx.h"
#include "./DepthWithTextToFile.h"

#include "../DebugInclude.h"

namespace
{
	const UINT MAX_LENGTH = 20480;
}

CDepthWithTextToFile::CDepthWithTextToFile()
	: m_pFile( NULL )

	, m_nDepth( 0 )
	, m_strDepth( _T( "" ) )

	, m_bCharacterPredeclaredEntity( false )
{
}

CDepthWithTextToFile::~CDepthWithTextToFile()
{
	Close();
}

bool CDepthWithTextToFile::Open(
	const std::string& strPath,
	const std::string& strMode,
	const std::string& strDepth,
	bool bCharacterPredeclaredEntity )
{
	Close();

	if( strPath.empty() )
		return false;

	_tfopen_s( &m_pFile, strPath.c_str(), strMode.c_str() );

	if( !m_pFile )
		return false;

	m_nDepth = 0;
	m_strDepth = strDepth;

	m_bCharacterPredeclaredEntity = bCharacterPredeclaredEntity;

	return true;
}

void CDepthWithTextToFile::Close()
{
	if( !m_pFile )
		return;

	fclose( m_pFile );
	m_pFile = NULL;
}

void CDepthWithTextToFile::Begin( const TCHAR* pFormat, ... )
{
	if( !pFormat )
		return;

	WriteDepth();

	TCHAR szBuffer[ MAX_LENGTH ] = _T( "" );

	va_list argList;
	va_start( argList, pFormat );
	::StringCbVPrintf( szBuffer, MAX_LENGTH, pFormat, argList );
	va_end( argList );

	//_ftprintf_s( m_pFile, _T( "%s\n" ), szBuffer );

	WCHAR szBufferW[ MAX_LENGTH ] = L"";
	MultiByteToWideChar( CP_ACP, 0, szBuffer, MAX_LENGTH, szBufferW, MAX_LENGTH );
	fwprintf_s( m_pFile, L"%s\n", szBufferW );

	AddDepth();
}

void CDepthWithTextToFile::Write( const TCHAR* pFormat, ... )
{
	if( !pFormat )
		return;

	WriteDepth();

	TCHAR szBuffer[ MAX_LENGTH ] = _T( "" );

	va_list argList;
	va_start( argList, pFormat );
	::StringCbVPrintf( szBuffer, MAX_LENGTH, pFormat, argList );
	va_end( argList );

	if( m_bCharacterPredeclaredEntity )
		CharacterPredeclaredEntity( szBuffer, MAX_LENGTH );

	//_ftprintf_s( m_pFile, _T( "%s\n" ), szBuffer );

	WCHAR szBufferW[ MAX_LENGTH ] = L"";
	MultiByteToWideChar( CP_ACP, 0, szBuffer, MAX_LENGTH, szBufferW, MAX_LENGTH );
	fwprintf_s( m_pFile, L"%s\n", szBufferW );
}

void CDepthWithTextToFile::End( const TCHAR* pFormat, ... )
{
	if( !pFormat )
		return;

	SubDepth();

	WriteDepth();

	TCHAR szBuffer[ MAX_LENGTH ] = _T( "" );

	va_list argList;
	va_start( argList, pFormat );
	::StringCbVPrintf( szBuffer, MAX_LENGTH, pFormat, argList );
	va_end( argList );

	//_ftprintf_s( m_pFile, _T( "%s\n" ), szBuffer );

	WCHAR szBufferW[ MAX_LENGTH ] = L"";
	MultiByteToWideChar( CP_ACP, 0, szBuffer, MAX_LENGTH, szBufferW, MAX_LENGTH );
	fwprintf_s( m_pFile, L"%s\n", szBufferW );
}

void CDepthWithTextToFile::WriteDepth()
{
	if( m_nDepth <= 0 )
		return;

	if( m_strDepth.empty() )
		return;

	std::string strBuffer;

	for( UINT i=0; i<m_nDepth; ++i )
		strBuffer.append( m_strDepth );

	//_ftprintf_s( m_pFile, _T( "%s" ), strBuffer.c_str() );

	WCHAR szBufferW[ MAX_LENGTH ] = L"";
	MultiByteToWideChar( CP_ACP, 0, strBuffer.c_str(), (int)strBuffer.length(), szBufferW, MAX_LENGTH );
	fwprintf_s( m_pFile, L"%s", szBufferW );
}

void CDepthWithTextToFile::CharacterPredeclaredEntity( TCHAR* pText, UINT nLength )
{
	const INT CPE_COUNT = 5;
	const std::string CPE_A[] = { _T( "&" ), _T( "<" ), _T( ">" ), _T( "\"" ), _T( "'" ) };
	const std::string CPE_B[] = { _T( "&amp;" ), _T( "&lt;" ), _T( "&gt;" ), _T( "&quot;" ), _T( "&apos;" ) };

	if( !pText )
		return;

	CString strText = pText;
	if( strText.IsEmpty() )
		return;

	INT iKey = strText.Find( '>' );
	if( iKey <= 0 || iKey >= strText.GetLength() )
		return;

	CString strLeft = strText.Left( iKey + 1 );
	strText = strText.Right( strText.GetLength() - strLeft.GetLength() );

	iKey = strText.ReverseFind( '<' );
	if( iKey <= 0 || iKey >= strText.GetLength() )
		return;

	CString strRight = strText.Right( strText.GetLength() - iKey );
	strText = strText.Left( strText.GetLength() - strRight.GetLength() );

	for( INT i=0; i<CPE_COUNT; ++i )
		strText.Replace( CPE_A[ i ].c_str(), CPE_B[ i ].c_str() );

	CString strCombine = strLeft + strText + strRight;
	StringCchCopy( pText, nLength, strCombine.GetString() );
}
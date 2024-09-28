#include "pch.h"

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "../=ZipArchive/zlib/zlib.h"

#include "oAuth/oauth.h"

#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Util/MinMessageBox.h"
#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../SigmaCore/Util/DateTime.h"
#include "../../SigmaCore/Log/LogMan.h"

#include "../../RanLogic/Character/GLCharData.h"

#include "../Database/DbManager.h"
#include "../Database/DBAction/DbActionGame.h"
#include "../Character/GLChar.h"

#include "./CharacterBackup.h"
#include "../../SigmaCore/DebugInclude.h"

// Compress a STL string using zlib with given compression level and return the binary data.
std::string compress_string( const std::string& str, int compressionlevel = Z_BEST_COMPRESSION )
{
	z_stream zs;	// z_stream is zlib's control structure
	memset( &zs, 0, sizeof( zs ) );

	if ( deflateInit( &zs, compressionlevel ) != Z_OK )
		return std::string( "" );

	zs.next_in = ( Bytef* )str.data();
	zs.avail_in = str.size();	// set the z_stream's input

	int ret;
	char outbuffer[65536];
	std::string outstring;

	// retrieve the compressed bytes blockwise
	do
	{
		zs.next_out = reinterpret_cast< Bytef* >( outbuffer );
		zs.avail_out = sizeof( outbuffer );

		ret = deflate( &zs, Z_FINISH );

		if ( outstring.size() < zs.total_out )
		{
			// append the block to the output string
			outstring.append( outbuffer, zs.total_out - outstring.size() );
		}
	} while ( ret == Z_OK );

	deflateEnd( &zs );

	if ( ret != Z_STREAM_END )
	{
		// an error occurred that was not EOF
		return std::string( "" );
	}

	return outstring;
}

// Decompress an STL string using zlib and return the original data.
std::string decompress_string( const std::string& str )
{
	z_stream zs;	// z_stream is zlib's control structure
	memset( &zs, 0, sizeof( zs ) );

	if ( inflateInit( &zs ) != Z_OK )
		return std::string( "" );

	zs.next_in = ( Bytef* )str.data();
	zs.avail_in = str.size();

	int ret;
	char outbuffer[65536];
	std::string outstring;

	// get the decompressed bytes blockwise using repeated calls to inflate
	do
	{
		zs.next_out = reinterpret_cast< Bytef* >( outbuffer );
		zs.avail_out = sizeof( outbuffer );

		ret = inflate( &zs, 0 );

		if ( outstring.size() < zs.total_out )
		{
			outstring.append( outbuffer, zs.total_out - outstring.size() );
		}
	} while ( ret == Z_OK );

	inflateEnd( &zs );

	if (ret != Z_STREAM_END)
	{
		// an error occurred that was not EOF
		return std::string( "" );
	}

	return outstring;
}

const std::string strCharacterBackupCreateQuery = "\
												  CREATE TABLE IF NOT EXISTS CharacterBackup( \
												  ChaNum INTEGER NOT NULL, \
												  UserNum INTEGER NOT NULL, \
												  ChaData TEXT NOT NULL, \
												  PRIMARY KEY( ChaNum ) ); \
												  CREATE UNIQUE INDEX IF NOT EXISTS Idx_CharacterBackup_ChaNum ON CharacterBackup ( ChaNum );";
const std::string strCharacterBackupInsertQuery = "INSERT INTO CharacterBackup( ChaNum, UserNum, ChaData ) VALUES( ?, ?, ? );";
const std::string strCharacterBackupUpdateQuery = "\
												  UPDATE CharacterBackup \
												  SET ChaData = ? \
												  WHERE ChaNum = ?;";
const std::string strCharacterBackupSelectAllQuery = "SELECT ChaNum, UserNum, ChaData FROM CharacterBackup;";
const std::string strCharacterBackupSelectQuery = "SELECT ChaData FROM CharacterBackup WHERE ChaNum = %1%;";
const std::string strCharacterBackupDeleteQuery = "DELETE FROM CharacterBackup WHERE ChaNum = %1%;";

CharacterBackup::CharacterBackup( db::IDbManager* pDBMan )
: m_stmtForCharacterInsert( NULL )
, m_stmtForCharacterUpdate( NULL )
, m_pAdoMan( pDBMan )
{
	open();
}

CharacterBackup::~CharacterBackup()
{
	close();
}

int CharacterBackup::SaveCharacterInSqlite( SCHARDATA2* pCharacter )
{
	// 서버가 종료되면서 sqlite 에 캐릭터가 저장이 된다.
	// SCHARDATA2 serialize 해서 text 로 저장이 된다.
	// 기존에 캐릭터 저장할 때 되는 출석부, 기여도(캐시서버로 메시지 전송후 캐시서버에서 저장)등은 저장하지 않는다.
	if ( NULL == pCharacter )
	{
		return eChaIsNull;
	}

	// 이미 존재하면 update, 아니면 insert
	std::string strSelectQuery( 
		sc::string::format( 
		strCharacterBackupSelectQuery, 
		pCharacter->CharDbNum() ) );

	SQLite::TablePtr pCharacterBackupTable( m_Character.QuerySQL2( strSelectQuery.c_str() ) );
	if ( !pCharacterBackupTable.m_pTable )
	{
		return eNullTablePointer;
	}

	SQLite::Table& rCharacterBackupTable = pCharacterBackupTable();
	int nRowCount = rCharacterBackupTable.GetRowCount();
	if ( nRowCount == 0 )
	{
		std::stringstream ss( std::ios_base::in | std::ios_base::out | std::ios_base::binary );
		boost::archive::binary_oarchive oa( ss );
		oa << ( *pCharacter );
		std::string strCharacter = ss.str();
		std::string strCompressed = compress_string( strCharacter );
		if ( strCompressed.empty() )
		{
			sc::writeLogError( "GLCharacterBackup SaveCharacterInSqlite(i) compress_string error." );
			return eCompress;
		}
		std::string strToBase64Encoded = oauth_encode_base64( strCompressed.size(), ( const unsigned char* )strCompressed.c_str() );

		sqlite3_bind_int( m_stmtForCharacterInsert, 1, pCharacter->CharDbNum() );
		sqlite3_bind_int( m_stmtForCharacterInsert, 2, pCharacter->GetUserID() );
		sqlite3_bind_text( m_stmtForCharacterInsert, 3, strToBase64Encoded.c_str(), strToBase64Encoded.size(), SQLITE_TRANSIENT );
		sqlite3_step( m_stmtForCharacterInsert );
		sqlite3_reset( m_stmtForCharacterInsert );

		sc::writeLogInfo( sc::string::format( "chanum %1% saved(insert) in sqlite successfully.", pCharacter->CharDbNum() ) );
	}
	else
	{
		std::stringstream ss( std::ios_base::in | std::ios_base::out | std::ios_base::binary );
		boost::archive::binary_oarchive oa( ss );
		oa << ( *pCharacter );
		std::string strCharacter = ss.str();
		std::string strCompressed = compress_string( strCharacter );
		if ( strCompressed.empty() )
		{
			sc::writeLogError( "GLCharacterBackup SaveCharacterInSqlite(u) compress_string error." );
			return eCompress;
		}
		std::string strToBase64Encoded = oauth_encode_base64( strCompressed.size(), ( const unsigned char* )strCompressed.c_str() );
		sqlite3_bind_text( m_stmtForCharacterUpdate, 1, strToBase64Encoded.c_str(), strToBase64Encoded.size(), SQLITE_TRANSIENT );
		sqlite3_bind_int( m_stmtForCharacterUpdate, 2, pCharacter->CharDbNum() );
		sqlite3_step( m_stmtForCharacterUpdate );
		sqlite3_reset( m_stmtForCharacterUpdate );

		sc::writeLogInfo( sc::string::format( "chanum %1% saved(update) in sqlite successfully.", pCharacter->CharDbNum() ) );
	}

	return eSuccess;
}

int CharacterBackup::SaveCharacterInGameDatabase()
{
	if ( NULL == m_pAdoMan )
	{
		return eDBManNull;
	}

	// SELECT ChaNum, UserNum, ChaData FROM CharacterBackup;
	std::vector< int > vecDelete;
	sqlite3_stmt* stmt = NULL;
	int nResult = sqlite3_prepare_v2( m_Character.GetPtr(), strCharacterBackupSelectAllQuery.c_str(), -1, &stmt, NULL );
	if ( SQLITE_OK == nResult )
	{
		while ( 1 )
		{
			if (  SQLITE_ROW == sqlite3_step( stmt ) )
			{
				int ChaNum = sqlite3_column_int( stmt, 0 );
				int UserNum = sqlite3_column_int( stmt, 1 );
				std::string strChadata( ( char* )sqlite3_column_text( stmt, 2 ) );
				unsigned char* pBuff = ( unsigned char* )malloc( strChadata.size()+1 );
				memset( pBuff, 0, strChadata.size()+1 );
				oauth_decode_base64( pBuff, strChadata.c_str() );
				std::string strToBase64Decoded( ( char* ) pBuff, strChadata.size()+1 );
				std::string strDecompressed = decompress_string( strToBase64Decoded );
				free( pBuff );
				if ( strDecompressed.empty() )
				{
					sc::writeLogError( "GLCharacterBackup SaveCharacterInGameDatabase decompress_string error." );
					continue;
				}
				std::stringstream ss2( std::ios_base::in | std::ios_base::out | std::ios_base::binary );
				ss2.write( strDecompressed.c_str(), strDecompressed.size() );
				boost::archive::binary_iarchive ia( ss2 );
				SCHARDATA2 sChadata;
				ia >> ( sChadata );

				// DbAction 으로 처리하지 않는다. 서버가 시작될 때 하는 것이기 때문에 다 끝날 때까지 기다리고 다음 작업 진행되도록 한다.
				if ( sc::db::DB_OK == m_pAdoMan->SaveCharacter( &sChadata ) )
				{
					vecDelete.push_back( ChaNum );
					m_SavedUserNum.push_back( static_cast< DWORD >( UserNum ) );
				}
			}
			else
			{
				break;
			}
		}
		sqlite3_finalize( stmt );
	}

	// 정상처리된 캐릭터는 삭제한다.
	int nDeleteSize = static_cast< int >( vecDelete.size() );
	for ( int loop = 0; loop < nDeleteSize; loop++ )
	{
		std::string strDeleteQuery( sc::string::format( strCharacterBackupDeleteQuery, vecDelete[loop] ) );
		executeSQL( strDeleteQuery );
	}

	if ( nDeleteSize > 0 )
	{
		std::string strMessage( 
			sc::string::format( "GLCharacterBackup SaveCharacterInGameDatabase total %1% characters saved in game database successfully.", nDeleteSize ) );
		sc::writeLogInfo( strMessage );
	}

	return eSuccess;
}

int CharacterBackup::open()
{
	std::string strFile = sc::getAppPath();
	strFile.append( "\\character_backup.db3" );
	int nResult = m_Character.Open( strFile.c_str() );
	if ( SQLITE_OK != nResult )
	{
		std::string strMessage( 
			sc::string::format( "GLCharacterBackup open Open critical error: %1%", m_Character.GetLastError() ) );
		sc::ErrorMessageBox( strMessage, false );
		return eOpen;
	}

	nResult = executeSQL( strCharacterBackupCreateQuery );
	if ( eSuccess != nResult )
	{
		return nResult;
	}

	nResult = sqlite3_prepare_v2( m_Character.GetPtr(), strCharacterBackupInsertQuery.c_str(), -1, &m_stmtForCharacterInsert, NULL );
	if ( nResult != SQLITE_OK )
	{
		std::string strMessage( 
			sc::string::format( "GLCharacterBackup open sqlite3_prepare_v2(i) error: %1%", m_Character.GetLastError() ) );
		sc::writeLogError( strMessage );
		return ePrepare;
	}

	nResult = sqlite3_prepare_v2( m_Character.GetPtr(), strCharacterBackupUpdateQuery.c_str(), -1, &m_stmtForCharacterUpdate, NULL );
	if ( nResult != SQLITE_OK )
	{
		std::string strMessage( 
			sc::string::format( "GLCharacterBackup open sqlite3_prepare_v2(u) error: %1%", m_Character.GetLastError() ) );
		sc::writeLogError( strMessage );
		return ePrepare;
	}

	return eSuccess;
}

void CharacterBackup::close()
{
	sqlite3_finalize( m_stmtForCharacterInsert );
	sqlite3_finalize( m_stmtForCharacterUpdate );

	if ( m_Character.IsOpen() )
	{
		m_Character.Close();
	}
}

int CharacterBackup::executeSQL( const std::string& strQuery )
{
	if ( strQuery.empty() )
	{
		return eStringEmpty;
	}

	int nResult = SQLITE_OK;
	nResult = m_Character.ExecuteSQL( strQuery );
	if ( SQLITE_OK != nResult )
	{
		std::string strMessage( 
			sc::string::format( 
			"GLCharacterBackup execute error expression : %1% error : %2%", 
			strQuery.c_str(), m_Character.GetLastError() ) );
		sc::writeLogError( strMessage );
		return eExecute;
	}

	return eSuccess;
}


/**
 * \date	2012/03/12
 * \author	Jun-Hyuk Choi
 */
#pragma once

#include <string>
#include <boost/functional/hash.hpp>

#include "../../SigmaCore/Database/Sqlite/SQLite.h"

#include "../../RanLogic/Msg/ReferCharMsg.h"
#include "../../RanLogic/GLogicEx.h"

#include "../Widget/GLWidgetScript.h"

class GLGaeaClient;

namespace ReferChar
{
	enum
	{
		EViewMax = 100,
	};

	typedef std::vector< std::string > NameList;

	class CClient
	{
	private:
		GLGaeaClient* m_pGaeaClient;

		FLOAT m_fDelay;
		bool m_bComplete;
		int m_nReferServer;
		GLCHARLOGIC m_sData;
		SCharInfoAddition m_sDataAddition;

		boost::hash< std::string > m_hashString;
		SQLite::Database m_cSQLite;

	public:
		CClient( GLGaeaClient* pGaeaClient );
		~CClient();

	public:
		void NotifyPublic( bool bPublic );
		bool Request( int nServer, const std::string& strName );
		bool Select( int nServer );
		bool Refresh();
		void Update( FLOAT fElapsedTime );

		void GetNameList( int nServer, NameList& vecList );
		LuaTable GetNameListForWidget( int nServer );
		GLCHARLOGIC* GetCompleteData();
		SCharInfoAddition* GetCompleteDataAddition();

		LuaTable GetInfoData();
		LuaTable GetCharInfoDetailAddonInfo();
		LuaTable GetSkillData( int nPage );
		LuaTable GetSkillTabPoint();
		LuaTable GetPutOnItem( EMSLOT emSlot );
		LuaTable GetPutOnItems();

		void GetSkillIndexRange(EMCHARCLASS _Class, int &_begin, int &_End);
		void ReqReferChar( const char* szTarName );

		EMCHARINDEX GetCharClass() { return m_sData.m_CHARINDEX; }

	public:
		void ReceiveMsgFb( NET_MSG_GENERIC* pMsg );
		void ReceiveMsgPuton( NET_MSG_GENERIC* pMsg );
		void ReceiveMsgSkill( NET_MSG_GENERIC* pMsg );
		void ReceiveMsgInfo( NET_MSG_GENERIC* pMsg );

	private:
		void SQLiteOpen( const std::string& strPath );
		void SQLiteClose();

		void SQLiteVersion();
		void SQLiteGarbage();

		void SQLiteCreate();
		void SQLiteCreateVersion();
		void SQLiteCreateInfo();
		void SQLiteCreatePuton();
		void SQLiteCreateSkill();

		void SQLiteInsert( int nServer, const GLCHARLOGIC& sData, const SCharInfoAddition& sDataAddition );
		void SQLiteInsertInfo( int nServer, const GLCHARLOGIC& sData, const SCharInfoAddition& sDataAddition );
		void SQLiteInsertPuton( int nServer, const GLCHARLOGIC& sData );
		void SQLiteInsertSkill( int nServer, const GLCHARLOGIC& sData );

		void SQLiteDelete( int nServer, const std::string& strName );
		void SQLiteDelete( int nServer, size_t nNameHash );

		void SQLiteSelectNameList( int nServer, NameList& vecList );

		bool SQLiteSelectData( int nServer, GLCHARLOGIC& sData, SCharInfoAddition& sDataAddition );
		bool SQLiteSelectDataInfo( int nServer, GLCHARLOGIC& sData, SCharInfoAddition& sDataAddition );
		void SQLiteSelectDataPuton( int nServer, GLCHARLOGIC& sData );
		void SQLiteSelectDataSkill( int nServer, GLCHARLOGIC& sData );
	};
}
#pragma once

#include <map>
#include <boost/serialization/map.hpp>

#include "./GLQuest.h"
//#include "../GLogicData.h"


namespace se {
    class ByteStream;
}

class GLQUESTPROG;

class GLQuestPlay
{
public:
	enum { VERSION = 0x0001, };

	typedef std::map<DWORD,GLQUESTPROG*>	MAPQUEST;
	typedef MAPQUEST::iterator				MAPQUEST_ITER;
	typedef MAPQUEST::const_iterator		MAPQUEST_CITER;

	typedef std::multimap<DWORD, DWORD >		QUESTMOB_MAP;
	typedef QUESTMOB_MAP::iterator				QUESTMOB_MAP_ITER;

	typedef std::multimap<DWORD, DWORD >		QUESTITEMUSE_MAP;
	typedef QUESTITEMUSE_MAP::iterator			QUESTITEMUSE_MAP_ITER;

	//typedef std::multimap<DWORD, DWORD >		QUESTITEMGRINDING_MAP;
	//typedef QUESTITEMGRINDING_MAP::iterator		QUESTITEMGRINDING_MAP_ITER;

protected:
	MAPQUEST		m_mapQuestProc;
	MAPQUEST		m_mapQuestEnd;

protected:
	QUESTMOB_MAP	m_mapQuestMob;
	QUESTITEMUSE_MAP	m_mapQuestItemUse;
	//QUESTITEMGRINDING_MAP m_mapQuestItemGrinding;

public:
	GLQuestPlay::MAPQUEST& GetQuestProc ()	{ return m_mapQuestProc; }
	GLQuestPlay::MAPQUEST& GetQuestEnd ()	{ return m_mapQuestEnd; }
	
public:
	GLQuestPlay::QUESTMOB_MAP& GetQuestMobProc ()	{ return m_mapQuestMob; }
	GLQuestPlay::QUESTITEMUSE_MAP& GetQuestItemUseProc ()	{ return m_mapQuestItemUse; }
	//GLQuestPlay::QUESTITEMUSE_MAP& GetQuestItemGrindingProc ()	{ return m_mapQuestItemGrinding; }

public:
	bool GetReqREADING ( DWORD &dwNID );
	void ResetReqREADING ( DWORD dwNID );

public:
	GLQUESTPROG* FindProc ( DWORD dwQuestID );
	GLQUESTPROG* FindEnd ( DWORD dwQuestID );

public:
	//! Quest 세팅, bCheck 가 true 이면 data 에 없는 퀘스트는 삭제된다.
    bool InsertProc(const GLQUESTPROG& sQuestProg, bool bCheck);
	bool InsertProc( 
		const GLQUESTPROG& sQuestProg, 
		bool bCheck, 
		std::multimap< DWORD, SINVENITEM_SAVE >& mapItems, 
		std::vector< sc::MGUID >& vecDeleteItems );
	bool DeleteProc(DWORD dwQuestID);

	bool InsertProcForMigration( const GLQUESTPROG& sQuestProg, bool bCheck );

public:
	void InsertMobProc(const GLQUESTPROG& sQuestProg);
	void DeleteMobProc(DWORD dwQuestID);

public:
	void InsertItemUseProc(const GLQUESTPROG& sQuestProg);
	void DeleteItemUseProc(DWORD dwQuestID);

public:
	//void InsertItemGrindingProc(const GLQUESTPROG& sQuestProg);
	//void DeleteItemGrindingProc(DWORD dwQuestID);

public:
    //! Quest 세팅, bCheck 가 true 이면 data 에 없는 퀘스트는 삭제된다.
	bool InsertEnd(const GLQUESTPROG& sQuestProg, bool bCheck);
	bool InsertEnd( 
		const GLQUESTPROG& sQuestProg, 
		bool bCheck, 
		std::multimap< DWORD, SINVENITEM_SAVE >& mapItems, 
		std::vector< sc::MGUID >& vecDeleteItems );
	bool DeleteEnd(DWORD dwQuestID);

public:
	HRESULT FrameMove(float fTime, float fElapsedTime);

public:
	void DeleteAll();

public:
	GLQuestPlay& operator= (GLQuestPlay& value);

public:
	DWORD		GetDataSize();
	BOOL		GetData( se::SerializeWritableData& data );
	bool		SET_BYBUFFER(se::ByteStream& ByteStream) const;
    //! Quest 세팅, bCheck 가 true 이면 data 에 없는 퀘스트는 삭제된다.
	bool		GET_BYBUFFER(se::ByteStream& ByteStream, bool bCheck);

	bool GET_BYBUFFER_FOR_MIGRATION( se::ByteStream& ByteStream, bool bCheck );

	bool GET_BYBUFFER_NEW( 
		se::ByteStream& ByteStream, 
		bool bCheck, 
		std::multimap< DWORD, SINVENITEM_SAVE >& mapProcItems, 
		std::multimap< DWORD, SINVENITEM_SAVE >& mapEndItems, 
		std::vector< sc::MGUID >& vecDeleteItems );
	BOOL GET_ITEM_BY_VECTOR( std::vector< SINVENITEM_SAVE >& vecItem );

public:
	GLQuestPlay();
	~GLQuestPlay();

private:
	friend class boost::serialization::access;
	template <typename Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & m_mapQuestProc;
		ar & m_mapQuestEnd;
		ar & m_mapQuestMob;
		ar & m_mapQuestItemUse;
	}
};

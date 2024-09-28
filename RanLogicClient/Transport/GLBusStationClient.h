#pragma once

#include "../../RanLogic/NpcTalk/NpcTalkMan.h"
#include "../../RanLogic/Transport/GLBusList.h"
#include "../RanLogicClient/Widget/GLWidgetScript.h"

class GLBusStationClient
{
public:
	static GLBusStationClient& GetInstance ();

protected:
	GLBusStationClient(void);	
	GLBusStationClient( const GLBusStationClient& value);	
	virtual ~GLBusStationClient(void);

public:
	const bool OpenBusStation(	/*const SNATIVEID& sNPCID,*/
								const DWORD dwNPCGlobalID,
								const DWORD dwTalkID,
								std::tr1::shared_ptr<SNpcTalk> spNpcTalk,
								bool bToolMode = false );
	lua_tinker::table	GetMapInfo(void);
	const DWORD			GetMapInfoSize(void);
	const bool			ReqBusStation( const DWORD dwStationID );
	const bool			IsUseable();
private:
	//std::tr1::shared_ptr<SNpcTalk> m_spNpcTalk;
	GLBusList		m_glBusList;
	DWORD			m_dwNPCGlobalID;
	EMCROW			m_eCrow;
};

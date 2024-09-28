#pragma once

#include "../RanLogicClient/Widget/GLWidgetScript.h"

class GLTaxiStationClient
{
public:
	static GLTaxiStationClient& GetInstance ();

protected:
	GLTaxiStationClient(void);	
	GLTaxiStationClient( const GLTaxiStationClient& value);	
	virtual ~GLTaxiStationClient(void);	

public:
	const bool			OpenTaxiCard( WORD wPosX, WORD wPosY );
	lua_tinker::table	GetMapInfo(void);
	const int			GetMapInfoSize(void) const;
	const LONGLONG		GetCalcTaxiCharge( int nSelectMap, int nSelectStop ) const;

	const bool			ReqTaxiStation( int nSelectMap, int nSelectStop ) const;

	void SetInvenPos ( WORD wPosX, WORD wPosY ) { m_wPosX = wPosX; m_wPosY = wPosY; return; }
private:
	DWORD				m_wPosX;
	DWORD				m_wPosY;

};

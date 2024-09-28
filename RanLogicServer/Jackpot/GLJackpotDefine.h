#pragma once

#include <vector>

#include "../../EngineLib/G-Logic/TypeDefine.h"

class GLAgentServer;

struct JACKPOT_ITEM
{
	SNATIVEID RequiredLevel;
	SNATIVEID ItemID;
	DWORD GameMoney;
	long PopCount;
	bool bMain;
	__time64_t ExpireTime;

	JACKPOT_ITEM()
		: RequiredLevel( SNATIVEID( false ) )
		, ItemID( SNATIVEID( false ) )
		, GameMoney( 0 )
		, PopCount( 0 )
		, bMain( true )
	{
		CTime TempTime = CTime::GetCurrentTime();
		CTimeSpan TimeSpan = CTimeSpan( 0, 0, 5, 0 );
		TempTime += TimeSpan;
		ExpireTime = TempTime.GetTime();
	}
};

struct JACKPOT
{
	float DelaySeconds;
	float Timer;
	SNATIVEID ItemID;
	SNATIVEID RequiredLevel;
	DWORD GameMoney;
	bool FromFileLoad;	// gm command 와 구분 용도, gm command 로 생긴 것이라면 false
	bool bMain;

	JACKPOT()
		: DelaySeconds( 86400.0f )
		, Timer( 0.0f )
		, ItemID( SNATIVEID( false ) )
		, RequiredLevel( SNATIVEID( false ) )
		, GameMoney( 0 )
		, FromFileLoad( true )
		, bMain( true )
	{
	}
};

class GLJackpotMgr
{
public:
	GLJackpotMgr( GLAgentServer* pServer );
	virtual ~GLJackpotMgr();

public:
	bool Load( const std::string& strFileName );

public:
	void AddUserDefinedJackpot( JACKPOT& Jackpot );
	void DeleteUserDefinedJackpot();
	void SetUse( bool value ) { m_Use = value; }

public:
	void FrameMove( float ElapsedAppTime );

protected:
	bool loadCommon();
	bool loadServer();
	void adjustTimer( float& Delay, float& Timer );

protected:
	std::string m_strFilePath;

protected:
	GLAgentServer* m_pServer;
	bool m_Use;
	std::vector< JACKPOT > m_vecJackpot;

};

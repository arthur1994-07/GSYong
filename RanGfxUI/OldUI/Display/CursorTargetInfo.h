#pragma	once

#include "../../../EngineLib/GUInterface/BasicVarTextBox.h"
#include "../../../RanLogic/NpcTalk/NpcTalkControl.h"
#include "../OldInterface.h"

class GLGaeaClient;
class GLCharacter;
class ClientActor;
class OldInterface;

class CCursorTargetInfo : public CBasicVarTextBox
{
protected:
	GLGaeaClient* m_pGaeaClient;
    OldInterface* m_pInterface;
	GLCharacter* m_pChar;
	ClientActor* m_pActor;

public:
	CCursorTargetInfo( GLGaeaClient* pGaeaClient, OldInterface* pInterface, EngineDeviceMan* pEngineDevice );

public:
	void RightBottomGap();
	void UpdateInfo();
	void ResetPosition();

private:
	void UpdateInfoPc();

	void UpdateInfoNpc();
	void AddInfoNpcBasic( CNpcTalkControl::NPCMAP* pTalkMap );
	void AddInfoNpcQuest( CNpcTalkControl::NPCMAP* pTalkMap );

	void UpdateInfoMob();

private:
	void AddNamePublic( DWORD nColor );
	void AddNamePrivate( DWORD nColor );
	void AddNameCondition( DWORD nColorPublic, DWORD nColorPrivate );
	void AddSchool( DWORD nColor );
	void AddClass( DWORD nColor );
	void AddParty( DWORD nColor );
	void AddClub( DWORD nColor );
	void AddHp( DWORD nColor );
	void AddShiftClick( DWORD nColor );
};


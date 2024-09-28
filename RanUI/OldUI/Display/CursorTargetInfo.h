#pragma	once

#include "../../../EngineLib/GUInterface/BasicVarTextBox.h"
#include "../../../RanLogic/NpcTalk/NpcTalkControl.h"
#include "../../InnerInterface.h"

class GLGaeaClient;
class GLCharacter;
class ClientActor;

class CCursorTargetInfo : public CBasicVarTextBox
{
protected:
	GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;
	GLCharacter* m_pChar;
	ClientActor* m_pActor;

public:
	CCursorTargetInfo( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

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

class MyCursorTargetInfo : public ICursorTargetInfo, private CCursorTargetInfo
{
public:
	MyCursorTargetInfo( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

public:
	virtual void CreateUIWindowAndRegisterOwnership();

	virtual void UpdateInfo();
};
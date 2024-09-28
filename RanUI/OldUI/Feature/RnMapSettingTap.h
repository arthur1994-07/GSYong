


#pragma once


#include "../Util/UIGroupHelper.h"
#include "RnMapHelper.h"

class GLGaeaClient;
class CInnerInterface;
class EngineDeviceMan;
class RnOldLargeMapWindow;



class RnMapSettingTap : public CUIGroupHelper , public RnMapPrivate::ISettingInterface
{
	enum
	{
		CHECKBOX_VIEW_ALL_SIGN = 1,
		CHECKBOX_GATE_SIGN,
		CHECKBOX_NORMAL_NPC_SIGN,
		CHECKBOX_PLACE_NAME_SIGN,
		CHECKBOX_BUS_STATION_SIGN,
		CHECKBOX_LOCKER_MANAGER_SIGN,
		CHECKBOX_START_POSITION_SIGN,
		CHECKBOX_START_CLUB_MANAGER_SIGN,
		CHECKBOX_POSTBOX_SIGN,
		CHECKBOX_PARTY_PLAYER_SIGN,
	
		CHECKBOX_ACTION_SIGN,
		CHECKBOX_AUTO_RIDE_VEHICLE_SIGN,
	};

public:
	RnMapSettingTap(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);

	void CreateSubControl();

	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual HRESULT Render( LPDIRECT3DDEVICEQ pd3dDevice );
	
	//////////////////////////////////////////////////////////////////////////

	virtual bool IsVisibleGate();
	virtual bool IsVisibleNormalNpc(); 
	virtual bool IsVisiblePlaceName(); 
	virtual bool IsVisibleBusStation(); 
	virtual bool IsVisibleLockerManager(); 
	virtual bool IsVisibleStartPosition(); 
	virtual bool IsVisibleClubManager(); 
	virtual bool IsVisiblePostBox();
	virtual bool IsVisiblePartyMember();

	virtual bool IsAutoRideVehicle();

private:



	CCheckBox* m_pCheckBoxViewAllSign;
	CCheckBox* m_pCheckBoxGate;
	CCheckBox* m_pCheckBoxNormalNPC;
	CCheckBox* m_pCheckBoxPlaceName;
	CCheckBox* m_pCheckBoxBusStation;
	CCheckBox* m_pCheckBoxLockerManager;
	CCheckBox* m_pCheckBoxStartPosition;
	CCheckBox* m_pCheckBoxClubManager;
// 	CCheckBox* m_pCheckBoxIteamSearch;
// 	CCheckBox* m_pCheckBoxItemMix;
	CCheckBox* m_pCheckBoxPostBox;
	CCheckBox* m_pCheckBoxPartyMember;

	CCheckBox* m_pCheckBoxActionAllSign;
	CCheckBox* m_pCheckBoxAutoRideVehicle;

};


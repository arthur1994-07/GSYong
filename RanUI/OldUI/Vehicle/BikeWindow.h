#pragma	once

#include "../Util/UIWindowEx.h"
#include "../../../RanLogic/Item/GLItem.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_MOUSEIN_BIKE_WEARSLOT = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class CInnerInterface;
class CBasicProgressBar;
class CBasicTextBox;
class CItemImage;
class CBasicVarTextBox;
class CInnerInterface;
class GLGaeaClient;

class CBikeWindow : public CUIWindowEx
{
	enum
	{
		SLOT_PARTS_IMAGE_TYPE0 = ET_CONTROL_NEXT,
		SLOT_PARTS_IMAGE_TYPE1,
		SLOT_PARTS_IMAGE_TYPE2,
		SLOT_PARTS_IMAGE_TYPE3,
		SLOT_PARTS_IMAGE_TYPE4,
		SLOT_PARTS_IMAGE_TYPE5,
				
		VEHICLE_BATTERY_TEXT,

		BIKE_BOOST_OVER,

		SLOT_IMAGE_SIZE = 6
	};

	DWORD m_dwMasterID;

	CBasicTextBox* m_pVehicleParts;
	CBasicTextBox* m_pVehicleName;
	CBasicTextBox* m_pVehicleType; 
	CUIControl*	m_pMouseOver;

	CUIControl* m_pSlotBack[VEHICLE_TYPE_SIZE];
	CUIControl*	m_pSlotDisplay[SLOT_IMAGE_SIZE];
	CItemImage*	m_pSlotImage[SLOT_IMAGE_SIZE];
	SITEMCUSTOM	m_ItemCustomArray[SLOT_IMAGE_SIZE];

	CBasicTextBox *	m_pBatteryText;

	CBasicProgressBar * m_pBattery;

	UIGUID	m_RollOverID;

	CUIControl* m_pIconOver;
	CBasicTextBox* m_pExplain;
	CBasicTextBox* m_pLearning; 
	CBasicTextBox* m_pBoostText;
	CUIControl* m_pOver;
	
	CBasicVarTextBox*	m_pInfo;

public:
	CBikeWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CBikeWindow ();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void CreateSubControl ();



private:
	CBasicTextBox * CreateTextBox( char* szKeyword, CD3DFontPar* pFont );
	CUIControl*	CreateControl( const char* szControl, WORD wAlignFlag = UI_FLAG_DEFAULT, UIGUID WndID = NO_ID );
	CItemImage*	CreateItemImage( const char* szControl, UIGUID ControlID );

public:
	virtual	void SetVisibleSingle ( BOOL bVisible );
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void LoadItem(int nIndex, const SITEMCUSTOM& ref_sItemCustom);
	void UnLoadItem(int nIndex);
	SITEMCUSTOM& GetItem(int nIndex) { return m_ItemCustomArray[nIndex]; }

public:
	DWORD GetMasterID() { return m_dwMasterID; }
	void SetVehicleInfo ( DWORD dwMasterID )	{ m_dwMasterID = dwMasterID; }

private:
	int m_nBatteryBack;

	void SetName( const std::string & strName );
	void SetType( const std::string & strType );
	void SetBattery( int nNOW );

public:
	EMSUIT IMAGE2EMSLOT ( UIGUID ControlID );

};
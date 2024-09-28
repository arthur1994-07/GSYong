#pragma	once

#include "../Util/UIGroupHelper.h"
#include "../../../RanLogic/Item/GLItem.h"
#include "../Feature/CostumColorInfo.h"
#include "../Util/TapSelectorEx.h"

class CharacterView;

class GLGaeaClient;
class EngineDeviceMan;
class CItemImage;
class CInnerInterface;

class RnCharacterWearWindow : public CUIGroupHelper , private CTapSelectorEx<DEFAULT_RADIO_BUTTON>
{
public:
	RnCharacterWearWindow(GLGaeaClient* pGaeaClient,CInnerInterface* pInterface,EngineDeviceMan* pEngineDevice);
	void CreateSubControl();

	void DeleteUIMeshData();

	void UpdateCharacterView();

	void SetArmSwapTabButton();
private:

	virtual void EventSelectedTap(UIGUID controlID);

	//////////////////////////////////////////////////////////////////////////
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	//////////////////////////////////////////////////////////////////////////
//	CItemImage* CreateItemImage( const char* szControl, UIGUID ControlID );
	virtual GLGaeaClient* GetGaeaClient() { return m_pGaeaClient; }

	CCostumColorInfo* CreateColorInfo( const char* szControl, UIGUID ControlID );
	void CreateSlotHelper(int index,const std::string& ItemImageKey, UIGUID ItemImageID,UIGUID ItemColorID);
	EMSLOT IMAGE2EMSLOT( int nIndex );
	SITEMCUSTOM& GetItem( int nIndex );
	void LoadItem( int nIndex, SITEMCUSTOM& ref_sItemCustom );
	void UnLoadItem( int nIndex );

	bool IsExtreme();



private:
	enum
	{
		ITEM_IMAGE_HEAD = NO_ID + 1,
		ITEM_IMAGE_UPPER,
		ITEM_IMAGE_WAIST,
		ITEM_IMAGE_LOWER,
		ITEM_IMAGE_HAND,
		ITEM_IMAGE_FOOT,
		ITEM_IMAGE_LEFT_ACCESSORY,
		ITEM_IMAGE_NECK,
		ITEM_IMAGE_WRIST,
		ITEM_IMAGE_LEFT_FINGER,
		ITEM_IMAGE_RIGHT_FINGER,
		ITEM_IMAGE_EARRINGS,
		ITEM_IMAGE_DECORATION,
		ITEM_IMAGE_RIGHT_ACCESSORY,
		ITEM_IMAGE_LEFT_WEAPON,
		ITEM_IMAGE_RIGHT_WEAPON,
		ITEM_IMAGE_LEFT_WEAPON_S,
		ITEM_IMAGE_RIGHT_WEAPON_S,
		ITEM_IMAGE_VEHICLE,
		ITEM_IMAGE_SIZE,


		// 차후 추가될 내용
// 

//		ITEM_IMAGE_SIZE,
// 
		//////////////////////////////////////////////////////////////////////////
		ITEM_COLOR_HEAD,
		ITEM_COLOR_UPPER,
		ITEM_COLOR_WAIST,
		ITEM_COLOR_LOWER,
		ITEM_COLOR_HAND,
		ITEM_COLOR_FOOT,
		ITEM_COLOR_LEFT_ACCESSORY,
		ITEM_COLOR_NECK,
		ITEM_COLOR_WRIST,
		ITEM_COLOR_LEFT_FINGER,
		ITEM_COLOR_RIGHT_FINGER,
		ITEM_COLOR_LEFT_EARRING,
		ITEM_COLOR_RIGHT_EARRING,
		ITEM_COLOR_RIGHT_ACCESSORY,
		ITEM_COLOR_LEFT_WEAPON,
		ITEM_COLOR_RIGHT_WEAPON,
		ITEM_COLOR_LEFT_WEAPON_S,
		ITEM_COLOR_RIGHT_WEAPON_S,
		ITEM_COLOR_VEHICLE,
		ITEM_COLOR_SIZE,

		//////////////////////////////////////////////////////////////////////////

		CHARACTER_RENDER_VIEW,
		RADIO_EXTREME_FIRST,
		RADIO_EXTREME_SECOND,
	};

	GLGaeaClient*			m_pGaeaClient;
	CInnerInterface*		m_pInterface;
	CharacterView*			m_pCharacterView;




	// 기존 코드를 가져온다
	CUIControl*	m_pSlotDisplay[ITEM_IMAGE_SIZE];
	CItemImage*	m_pItemImage[ITEM_IMAGE_SIZE];
	SITEMCUSTOM	m_ItemCustomArray[ITEM_IMAGE_SIZE];
	CCostumColorInfo*	m_pColorInfo[ITEM_IMAGE_SIZE]; 
	D3DXVECTOR2	m_ColorInfoPos[ITEM_IMAGE_SIZE]; 

	CBasicTextBox* m_pItemGradeText[ITEM_IMAGE_SIZE ];

	UIRECT m_pItemImageLocalPosRect[ITEM_IMAGE_SIZE];
	UIRECT m_pItemImageGlobalPosRect[ITEM_IMAGE_SIZE];

	CUIControl*		m_pMouseOver;
};
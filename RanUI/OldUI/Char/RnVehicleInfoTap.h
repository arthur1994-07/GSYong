
#pragma	once

#include "../../../EngineLib/G-Logic/TypeDefine.h"
#include "../Util/UIGroupHelper.h"

#include "../../../RanLogic/Item/GLItemDef.h"
#include "../../../RanLogic/Item/GLItemCustom.h"
#include "../../../RanLogic/Item/GLItemVehecle.h"


#include "../Util/TapSelectorEx.h"


class GLGaeaClient;
class CInnerInterface;
class EngineDeviceMan;

class CharacterView;
class CItemImage;
class CBasicTextBox;
class CCheckBox;

class RnVehicleInfoTap : public CUIGroupHelper , public CTapSelectorEx<DEFAULT_RADIO_BUTTON>
{
public:
	RnVehicleInfoTap(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	void CreateSubControl();

	void DeleteUIMeshData();
	
	virtual	void SetVisibleSingle ( BOOL bVisible );

	void SetStorageItemToVehicleTapWindow(const std::vector<DWORD>& vehicleIDVec);

private:
	enum POINT_SHOP_RADIO{ POINT_SHOP_BOARD_RADIO,POINT_SHOP_BIKE_RADIO,POINT_SHOP_PARTS_RADIO,POINT_SHOP_BATTERY_RADIO };


	static const int MY_VEHICLE_SLOT_COUNT = 7;
	static const int PARTS_SLOT_COUNT = 6;
	static const int POINT_SHOP_SLOT_COUNT = 8;

	static const int INVALID_INDEX = -1;
	enum
	{
		VEHICLE_RENDER_VIEW = 1,
		MY_SLOT,
		MY_LIST_SLOT_1,
		MY_LIST_SLOT_2,
		MY_LIST_SLOT_3,
		MY_LIST_SLOT_4,
		MY_LIST_SLOT_5,
		MY_LIST_SLOT_6,
		MY_LIST_SLOT_7,
		MY_LIST_PRE_BUTTON,
		MY_LIST_NEXT_BUTTON,


		VEHICLE_QUESTION_BUTTON,
		PARTS_SLOT_1,
		PARTS_SLOT_2,
		PARTS_SLOT_3,
		PARTS_SLOT_4,
		PARTS_SLOT_5,
		PARTS_SLOT_6,

		CHECKBOX_ANIMATION,

		POINT_SHOP_BOARD_RADIO_BUTTON,
		POINT_SHOP_BIKE_RADIO_BUTTON,
		POINT_SHOP_PARTS_RADIO_BUTTON,
		POINT_SHOP_BATTERY_RADIO_BUTTON,
		
		POINT_SHOP_LIST_SLOT_1,
		POINT_SHOP_LIST_SLOT_2,
		POINT_SHOP_LIST_SLOT_3,
		POINT_SHOP_LIST_SLOT_4,
		POINT_SHOP_LIST_SLOT_5,
		POINT_SHOP_LIST_SLOT_6,
		POINT_SHOP_LIST_SLOT_7,
		POINT_SHOP_LIST_SLOT_8,

		POINT_SHOP_LIST_PRE_BUTTON,
		POINT_SHOP_LIST_NEXT_BUTTON,
		
		POINT_SHOP_BUY_BUTTON,

	};

	void UpdateCharacterView(bool isForce);


	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

	void ShowMyVehicleList();

private:
	virtual  void EventSelectedTap( UIGUID ControlID );

	void CreateControlHelper(CItemImage*& vehicleImage,const std::string& key,DWORD boardColor,UIGUID id);
	void CreatePageHelper(const std::string& prePageKey,UIGUID prePageID,
		const std::string& nextPageKey,UIGUID nextPageID,
		const std::string& pageTextKey,CBasicTextBox*& pageText);
	void ShowPointShopVehicleList(UIGUID radio);


	void UpdateMyVehicleListPage();
	void UpdatePointShopVehicleListPage();
	void UpdateVehicleListPageHelper(CBasicTextBox* pageTextBox,int currentPage,int MaxSlot,
		CItemImage** ImageList,const std::vector<SNATIVEID>& vehicleVec);


	int GetMyListCurrentPage();
	int GetPointShopCurrentPage();

	bool PlusPage(int& curPage,std::vector<SNATIVEID>& itemCacheVec,int maxSize);
	bool MinusPage(int& curPage);

	SNATIVEID GetSelectedVehicleFromList();


	void ClearOverImage(bool myVehicleList = true,bool PointShopVehicleList = true);

	//////////////////////////////////////////////////////////////////////////
	void UpdateVehicleParts();

	bool IsVehicleItem(const SNATIVEID& id);
//	int GetPartsSlotSize();

	VEHICLE_TYPE GetVehicleType();
	EMSUIT IMAGE2EMSLOT(UIGUID ControlID);
	SITEMCUSTOM GetPartsItem(int parts);

	void LoadItem(int nIndex, const SITEMCUSTOM& ref_sItemCustom);
	void UnLoadItem(int nIndex);
	CItemImage* CreateItemImage( const char* szControl, UIGUID ControlID );

	SNATIVEID GetSelectedPointShopItemFromList();

	void ShowPointShop(SNATIVEID id);
private:
	GLGaeaClient*    m_pGaeaClient;
	CInnerInterface* m_pInterface;


	CItemImage*      m_MyVehicleImage;
	CItemImage*      m_MyVehicleImageList[MY_VEHICLE_SLOT_COUNT];
	CItemImage*      m_PointShopVehicleImageList[POINT_SHOP_SLOT_COUNT];
	

	CharacterView*   m_pCharacterView;

	std::vector<SNATIVEID> m_StorageVehicleVec;
	std::vector<SNATIVEID> m_myVehicleListCacheVec;

	std::vector<SNATIVEID> m_PointShopVehicleListCacheVec;

	int m_MyListCurrentPageNum;
	CBasicTextBox* m_MyListCurrentPageNumText;

	int m_PointShopListCurrentPageNum;
	CBasicTextBox* m_PointShopListCurrentPageNumText;
	//////////////////////////////////////////////////////////////////////////

	CCheckBox*     m_MovingAnimationCheckBox;
	CBasicTextBox* m_MovingAnimationText;


	CBasicTextBox* m_pVehiclePartsSpeedText;
	CBasicTextBox* m_pVehicleNameText;
	CBasicTextBox* m_pVehicleBatteryText;
	CBasicTextBox* m_pVehicleBoostText;
	CUIControl*    m_pBoosterIcon;
	

	CUIControl*	m_pMouseOver;
	CUIControl*	m_pSlotDisplay[PARTS_SLOT_COUNT];
	CItemImage*	m_pSlotImage[PARTS_SLOT_COUNT];
	SITEMCUSTOM	m_ItemCustomArray[PARTS_SLOT_COUNT];

	
	RnButton*  m_pPointShopBuyButton;

};


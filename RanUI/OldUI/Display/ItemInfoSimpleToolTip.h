#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogic/Item/GLItemCustom.h"
#include "../Util/UIGroupHelper.h"

class TemporaryItemPointer{
public:
	TemporaryItemPointer(const SITEMCUSTOM& ItemCustom) : m_ItemCustom(ItemCustom){}
	const SITEMCUSTOM& m_ItemCustom;
};

class CItemImage;
class CBasicButton;
class CBasicTextBox;

class CInnerInterface;
class EngineDeviceMan;
class ItemInfoSimpleToolTip : public CUIGroupHelper
{
	enum
	{
		ITEM_IMAGE = NO_ID + 1,
	};
public:
	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg) override;
	virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl) override;

	void CreateSubControl(void);

	const bool SetItemInformation(const SITEMCUSTOM& ItemCustom);	

private:
	void SetItemImage(const SITEM* const pItemData);
	void SetItemName(const SITEMCUSTOM& ItemCustom, const SITEM* const pItemData);
	void SetItemInfo(const SITEMCUSTOM& ItemCustom, const SITEM* const pItemData);
		inline void SetItemInfo_School(const DWORD dwReqSchool, const int nIndex);

		inline void SetItemInfo_Class(const SITEMCUSTOM& ItemCustom, const DWORD dwReqCharClass, const int nIndex);
			inline void SetItemInfo_Class_Costume(const DWORD dwReqCharClass, const SITEMCUSTOM& ItemCustom, const int nIndex);
			inline void SetItemInfo_Class_Normal(const DWORD dwReqCharClass, const SITEMCUSTOM& ItemCustom, const int nIndex);

		inline void SetItemInfo_Sex(const SITEMCUSTOM& ItemCustom, const DWORD dwReqCharClass, const int nIndex);
			inline void SetItemInfo_Sex_Costume(const DWORD dwReqCharClass, const SITEMCUSTOM& ItemCustom, const int nIndex);
			inline void SetItemInfo_Sex_Normal(const DWORD dwReqCharClass, const SITEMCUSTOM& ItemCustom, const int nIndex);
		inline void SetItemInfo_Condition(const SITEM* const pItemData, const int nIndex);
			inline const bool SetItemInfo_Condition_Level(const DWORD dwReqLevelDW, const DWORD dwReqLeveUP, const int nIndex);
			inline void SetItemInfo_Condition_Status(const SCHARSTATS& rItemStats, bool bExistFrontValue, const int nIndex);	

public:
	ItemInfoSimpleToolTip(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~ItemInfoSimpleToolTip(void);

private:
	GLGaeaClient* m_pGaeaClient;
	CInnerInterface* m_pInterface;

	CItemImage* m_pItemImage;
	CBasicTextBox* m_pItemName;
	CBasicTextBox* m_pItemInfo;	

	SITEMCUSTOM m_ItemCustom;
	D3DXVECTOR2 m_vMousePosition;

	DWORD m_bNotAvailable; // 착용 불가능 플래그;
};
#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogicClient/PointShop/PointShopClient.h"

class CInnerInterface;
class CBasicTextBox;

class CPointShopCategory : public CUIGroup
{
private:
    enum
    {
        CATEGORY_BACK = NO_ID + 1,
        CATEGORY_CHECK= NO_ID + 2,
        CATEGORY_UNCHECK = NO_ID + 3,
        CATEGORY_CHECK_BACK = NO_ID + 4,
    };

public:
    CPointShopCategory(CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual	~CPointShopCategory();

protected:
    CInnerInterface* m_pInterface;

private:
    CUIControl* m_pCategoryCheck;
    CUIControl* m_pCategoryUnCheck;
    CBasicTextBox* m_pText;

private:
    bool m_bCheck;
    bool m_bEnableAll; // 카테고리 전체 선택/해제 기능을 위한 변수.
    bool m_bForceChecked; // 이름 검색으로 체크 되었는지 확인하기 위한 변수.

public:
    void CreateSubControl ();

    void SetCategoryData( std::string& strName, int nCount );
    bool IsCheck() { return m_bCheck; }

    void SetEnableCategoryAll( bool _bEnable );

    void Check();
    void UnCheck();

    void SetForceChecked( bool _bForceChecked ) { m_bForceChecked = _bForceChecked; }
    bool IsForceChecked() { return m_bForceChecked; }

private:
    void PressCheckBox();

public:
    virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
};
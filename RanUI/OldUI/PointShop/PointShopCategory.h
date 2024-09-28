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
    bool m_bEnableAll; // ī�װ� ��ü ����/���� ����� ���� ����.
    bool m_bForceChecked; // �̸� �˻����� üũ �Ǿ����� Ȯ���ϱ� ���� ����.

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
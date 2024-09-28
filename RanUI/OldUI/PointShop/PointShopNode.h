#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../EngineLib/G-Logic/TypeDefine.h"

class GLGaeaClient;
class CInnerInterface;

class CBasicLineBoxSmart;
class CItemImage;
class CBasicTextBox;

class CPointShopNode : public CUIGroup
{
private:
    enum
    {
        POINT_SHOP_BACK = NO_ID + 1,
        POINT_SHOP_IMAGE = NO_ID + 2,
    };

public:
    CPointShopNode(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual	~CPointShopNode();

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

private:
    CBasicLineBoxSmart* m_pBack;
    CItemImage* m_pImage;
    CBasicTextBox* m_pName;
    CBasicTextBox* m_pPrice;

private:
    SNATIVEID m_sID;
    std::string m_Name;
    __int64 m_Price;

public:
    void CreateSubControl ();

    void SetItem( SNATIVEID _sID, std::string _strName, __int64 _Price );
    void ResetItem();

    SNATIVEID GetItemID() { return m_sID; }
    std::string GetItemName() { return m_Name; }
    __int64 GetItemPrice() { return m_Price; }

public:
    virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
};
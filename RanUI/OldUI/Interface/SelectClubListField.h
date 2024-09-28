#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class GLGaeaClient;
class CInnerInterface;

class CBasicTextBox;
class CCheckBoxEx;

class CSelectClubListField : public CUIGroup
{
private:
    enum
    {

        BACK = NO_ID + 1,
        TEXT = BACK + 1,
        ORDER = BACK + 2,
        LIST_TEXT = BACK + 3,
    };

public:
    CSelectClubListField(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual	~CSelectClubListField();

protected:
    GLGaeaClient* m_pGaeaClient;
    COuterInterface* m_pInterface;

public:
    virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
    void CreateSubControl( const char* szBackKeyWord, const char* szTextKeyword, const char* szOrderKeyword, const char* szListKeyword );

    void SetFieldType( wb::search::EM_ORDER_TYPE OrderType );
    void SetFiledText( const char* strFiledText );

    void ClearList();
    void AddList( std::string& strText );

    void SetCurListLine( int nPos );
    int GetVisibleListLine();

    wb::search::EM_ORDER_TYPE GetFieldType();
    wb::search::EM_ORDER_ASC_DSC GetFieldOrder();
    void SetFieldOrder( wb::search::EM_ORDER_ASC_DSC Order );
    void ResetOrder();

    void SetSelectPos( int nIndex );

private:
    wb::search::EM_ORDER_TYPE m_Type;

private:
    CBasicTextBox* m_pText;
    CCheckBox* m_pOrder;
    CUIControl* m_pSelect;
    CBasicTextBoxEx* m_pListText;
};
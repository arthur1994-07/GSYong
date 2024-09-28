#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class GLGaeaClient;
class CInnerInterface;

class CBasicTextBox;
class CCheckBoxEx;

class CWbClubInfoField : public CUIGroup
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
    CWbClubInfoField(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual	~CWbClubInfoField();

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

public:
    virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
    void CreateSubControl( const char* szBackKeyWord, const char* szTextKeyword, const char* szOrderKeyword, const char* szListKeyword, int nAlign );

    void SetFiledText( const char* strFiledText );

    void ClearList();
    void AddList( std::string& strText, DWORD dwID, const D3DCOLOR TextColor = NS_UITEXTCOLOR::WHITE );

    void SetCurListLine( int nPos );
    int GetVisibleListLine();

    void SetFieldOrder( club::ALIGN_ORDER_DES_ASC Order );
    club::ALIGN_ORDER_DES_ASC GetFieldOrder();
    void ResetOrder();

    void SetSelectPos( int nIndex );
    int GetSelectPos();

    const DWORD GetTextData( int nIndex );
    bool GetSelectPart( int nIndex, UIRECT& rcOut );

private:
    CBasicTextBox* m_pText;
    CCheckBox* m_pOrder;
    CUIControl* m_pSelect;
    CBasicTextBoxEx* m_pListText;
};
#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../InnerInterface.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의

class CInnerInterface;
class CSmartComboBoxRollOver;

class CChatTypeList: public CUIGroup
{
private:
    enum
    {
        CHAT_COMBOBOX = NO_ID + 1,
    };

public:
    CChatTypeList( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual	~CChatTypeList();

public:
    virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID cID, DWORD dwMsg );

public:
    void CreateSubControl();

private:
    void InitChatTypeList ();

private:
    CSmartComboBoxRollOver* m_pComboBox;

protected:
    CInnerInterface* m_pInterface;
};

class MyChatTypeList : public IChatTypeList, private CChatTypeList
{
public:
    MyChatTypeList( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyChatTypeList()   {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();
    virtual void SetUIGlobalPos( const UIRECT& rcPosition );
    virtual const UIRECT& GetUIGlobalPos();
};

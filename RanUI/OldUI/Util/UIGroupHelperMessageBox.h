#pragma	once

#include "./UIGroupHelper.h"
#include "./MessageBox.h"

class CMessageBox;


class CUIGroupHelperMessageBox : public CUIGroupHelper
{
protected:
    enum
    {
        ID_MESSAGEBOX = NO_ID + 1,
        ID_NEXT = ID_MESSAGEBOX +  1,
    };
public:
    CUIGroupHelperMessageBox(EngineDeviceMan* pEngineDevice);
    void                CreateMessageBox();
    void                OpenMessageBox( const std::string& strTitle, const std::string& strMessage, const CMessageBox::BOXTYPE eBoxType = CMessageBox::TYPE_OKCANCEL, unsigned int nIdentity = 0  );
    void                OpenMessageBox( const CMessageBox::BOXTITLE eBoxTitle, const std::string& strMessage, const CMessageBox::BOXTYPE eBoxType = CMessageBox::TYPE_OKCANCEL, unsigned int nIdentity = 0 );
    
    void                OpenMessageBoxMultiLine( const CMessageBox::BOXTITLE eBoxTitle, const std::vector<std::string>& vecMessage, const CMessageBox::BOXTYPE eBoxType = CMessageBox::TYPE_OKCANCEL, unsigned int nIdentity = 0 );
    void                OpenMessageBoxMultiLine( const std::string& strTitle, const std::vector<std::string>& vecMessage, const CMessageBox::BOXTYPE eBoxType = CMessageBox::TYPE_OKCANCEL, unsigned int nIdentity = 0 );

    virtual	void        SetVisibleSingle( BOOL bVisible );
    void                CloseMessageBox();

protected:
    
	virtual void		Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void        TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    virtual void        TranslateMessageBox( unsigned int nIdentity, DWORD dwMsg ) = 0;

    BOOL                IsOpenMessageBox();
    BOOL                IsOnMouse();
    std::string         GetInputString();

private:
    CMessageBox*        m_pMessageBox;
    bool                bOnMouse;
};
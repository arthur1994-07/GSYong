#pragma	once

#include "../Util/UIOuterWindow.h"

class COuterInterface;
class GLGaeaClient;

class CBasicLineBoxSmart;
class CBasicTextBox;
class RnButton;

class CSelectCountryConfirmModal : public CUIOuterWindow
{
private:
    enum
    {
        OK_BUTTON = ET_CONTROL_NEXT + 1,
        CANCEL_BUTTON = ET_CONTROL_NEXT + 2,
    };

public:
    enum EM_STATE
    {
        EM_BEFORE_SELECT,
        EM_AFTER_SELECT,
        EM_ESSENTIAL_SELECT_CLUB,
        EM_NONESSENTIAL_SELECT_CLUB,
    };

public:
    CSelectCountryConfirmModal(GLGaeaClient* pGaeaClient, COuterInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual	~CSelectCountryConfirmModal();

public:
    virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
    virtual void SetFocusControl();

public:
    void CreateSubControl();

    void SetBeforeSelectState();
    void SetAfterSelectState( const wb::COUNTRY_INFO& CountryInfo );
    void SetEssentialSelectClub();
    void SetNonEssentialSelectClub();

private:
    void SetMainText( const char* strMainText1, const char* strMainText2 );
    void SetOKButtonText( const char* strOKButtonText );
    void SetCancelButtonText( const char* strCancelButtonText );

private:
    GLGaeaClient* m_pGaeaClient;

    CBasicTextBox* m_pMainText;
    RnButton* m_pOKButton;
    RnButton* m_pCancelButton;

private:
    EM_STATE m_emState;
    wb::COUNTRY_INFO m_CountyInfo;
};
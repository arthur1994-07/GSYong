#pragma	once

#include "ModalWindow.h"
#include "../../InnerInterface.h"

class CInnerInterface;

class CModalWindowInner : public CModalWindow
{

public:
	CModalWindowInner( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CModalWindowInner();

public:
	virtual	void	Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

protected:
	CInnerInterface* m_pInterface;

};

class MyModalWindow : public IModalWindowInner, private CModalWindowInner
{
public:
    MyModalWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyModalWindow() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual void SetHide( BOOL bHide );
    virtual void DoModal( const std::string& strText, int nModalTitle, int nModalType,
        UIGUID CallerID );
    virtual void DoModalTipBox ( const std::string& strText, const std::string& strTip,
        int nModalTitle, int nModalType, UIGUID CallerID );
    virtual void SetModalData ( int nData1, int nData2 );
    virtual void SetModalData( const D3DXVECTOR3& vData );
    virtual void GetModalData( int* pnData1, int* pnData2 );
    virtual void GetModalData( D3DXVECTOR3* vData );
    virtual bool IsUseModaless();
    virtual UI::String	GetEditString();
    virtual UIGUID GetCallerID();
};

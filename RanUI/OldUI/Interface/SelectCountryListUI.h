#pragma	once

#include "../Util/UIOuterWindow.h"

#include "../../../RanLogicClient/Country/GLCountryManClient.h"

class COuterInterface;
class GLGaeaClient;

class CSelectCountryListComp;
class CBasicTextButton;

const DWORD UIMSG_MOUSEIN_SELECT_COUNTRY_LIST_UI = UIMSG_USER1;

class CSelectCountryListUI : public CUIOuterWindow
{
private:
	enum
	{
		SELECT_COUNTRY_LIST_OK_BUTTON = ET_CONTROL_NEXT + 1,
		SELECT_COUNTRY_LIST_CANCEL_BUTTON,
	};

public:
	CSelectCountryListUI (
		GLGaeaClient* pGaeaClient,
		COuterInterface* pInterface,
		EngineDeviceMan* pEngineDevice );
	virtual	~CSelectCountryListUI ();

public:
	void CreateSubControl ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

	virtual	void SetVisibleSingle( BOOL bVisible );

public:
	void UpdateList ( const Country::CONTINENT_ID dwCurContinentID );

	const Country::COUNTRY_ID GetSelectedCountryID () const;
	const std::string& GetSelectedCountryName () const;

private:
	CSelectCountryListComp* m_pCountryList;

	CBasicTextButton* m_pButtonOK;
	CBasicTextButton* m_pButtonCancel;

protected:
	GLGaeaClient* m_pGaeaClient;
};
#pragma	once

#include "../Util/UIGroupHelper.h"

#include "../../../RanLogic/Country/GLCountryDefine.h"

class COuterInterface;
class GLGaeaClient;

class CBasicTextButton;
class CSwapImage;

const DWORD UIMSG_MOUSEIN_SELECT_COUNTRYUI = UIMSG_USER1;

class CSelectCountryUI : public CUIGroupHelper
{
private:
	enum
	{
		SELECT_COUNTRY_BUTTON = NO_ID + 1,
	};

public:
	CSelectCountryUI (
		GLGaeaClient* pGaeaClient,
		COuterInterface* pInterface,
		EngineDeviceMan* pEngineDevice );
	virtual	~CSelectCountryUI ();

public:
	void CreateSubControl ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

	virtual	void SetVisibleSingle( BOOL bVisible );

private:
	CBasicTextButton* m_pSelectButton;
	CSwapImage* m_pSelectImage;

	bool m_bSelectedCountry;

	Country::SCOUNTRY_INFO m_sCountryInfo;
	
protected:
	COuterInterface* m_pInterface;
	GLGaeaClient* m_pGaeaClient;
};
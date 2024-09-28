#pragma	once

#include "../Util/UIOuterWindow.h"

#include "../../../RanLogicClient/Country/GLCountryManClient.h"

class COuterInterface;
class GLGaeaClient;

class CSelectContinentListComp;

const DWORD UIMSG_MOUSEIN_SELECT_CONTINENT_LIST_UI = UIMSG_USER1;

class CSelectContinentListUI : public CUIOuterWindow
{
public:
	CSelectContinentListUI (
		GLGaeaClient* pGaeaClient,
		COuterInterface* pInterface,
		EngineDeviceMan* pEngineDevice );
	virtual	~CSelectContinentListUI ();

public:
	void CreateSubControl ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

	virtual	void SetVisibleSingle( BOOL bVisible );

public:
	void UpdateList ();

	const Country::CONTINENT_ID GetSelectedContinentID () const;

private:
	CSelectContinentListComp* m_pContinentList;

protected:
	GLGaeaClient* m_pGaeaClient;
};
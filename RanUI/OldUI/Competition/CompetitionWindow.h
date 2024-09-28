#pragma	once

#include "../Util/UIWindowObjectController.h"
#include "../../../EngineLib/GUInterface/InterfaceBaseDefine.h"
#include "../../InnerInterface.h"

class CInnerInterface;
class GLGaeaClient;
class CBasicLineBoxSmart;
class CBasicTextButton;
class CBasicTextBox;
class CCompetitionObject;
class CCompetitionPage;

class CCompetitionWindow : public CUIWindowObjectController
{
protected:
	enum COMPETITION_ID
	{
		COMPETITION_WINDOW_NONE = NO_ID,
		COMPETITION_WINDOW_OBJECT,

		COMPETITION_WINDOW_PAGE_CTF,
		COMPETITION_WINDOW_PAGE_PVE,
		COMPETITION_WINDOW_PAGE_TOURNAMENT,
	};

public:
	enum COMPETITION_PAGE_INDEX
	{
		COMPETITION_PAGE_CTF,
		COMPETITION_PAGE_PVE,
		COMPETITION_PAGE_TOURNAMENT,
		COMPETITION_PAGE_QUANTITY,
	};

public:
	CCompetitionWindow ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CCompetitionWindow ();

public:
	virtual void Update				( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );	
	virtual	void TranslateUIMessage	( UIGUID ControlID, DWORD dwMsg );

public:
	void CreateSubControl ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	const bool IsOpen();
	void	   Open();
	void	   Close();
	void	   Update(int nPage);

protected:
	CBasicLineBoxSmart* CreateLineBox ( const std::string& strControlName, const std::string& strTexInfo, const UIGUID cID=NO_ID );

private:
	CCompetitionObject* m_pUIObject;

private:
	CCompetitionPage* m_pPages[COMPETITION_PAGE_QUANTITY];

protected:
	GLGaeaClient* m_pGaeaClient;
};

class MyCompetitionWindow : public ICompetitionWindow, private CCompetitionWindow
{
public:
	MyCompetitionWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~MyCompetitionWindow() {};

public:
	virtual void CreateUIWindowAndRegisterOwnership( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual const bool IsOpen();
	virtual void Open ();
	virtual void Close ();
	virtual void Update(int nPage);
	virtual const UIRECT& GetGlobalPos();    
};

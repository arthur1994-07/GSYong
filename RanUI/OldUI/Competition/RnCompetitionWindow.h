#pragma	once

#include "../Util/UIWindowObjectController.h"
#include "../../../EngineLib/GUInterface/InterfaceBaseDefine.h"
#include "../../InnerInterface.h"

class CInnerInterface;
class GLGaeaClient;
class CBasicLineBoxSmart;
class CBasicTextButton;
class CBasicTextBox;
class RnCompetitionObject;
class CCompetitionPage;
class RnCompetitionInstanceTypeMain;
class CBasicScrollBarEx;
class CTipBox;
class RnCTFPage;
class RnWailBuinding;

const int DWORD_MAX = 0xffffffffUL;

enum EM_INSTANCE_TYPE_MAIN
{
	EM_INSTANCE_TYPE_MAIN_NULL,
	EM_INSTANCE_TYPE_MAIN_PVP,
	EM_INSTANCE_TYPE_MAIN_PVE,
	EM_INSTANCE_TYPE_MAIN_SIZE,
};

enum EM_INSTANCE_TYPE_SUB
{
	EM_INSTANCE_TYPE_SUB_NULL,
	EM_INSTANCE_TYPE_SUB_CTF,
	EM_INSTANCE_TYPE_SUB_CDM,
	EM_INSTANCE_TYPE_SUB_WAIL,
};

struct SINSTANCE_NAME_ID
{
	std::string strInstanceName;
	DWORD dwInstanceID;
	DWORD dwMaxJoinCount;
	SINSTANCE_NAME_ID()
		: strInstanceName("")
		, dwInstanceID(DWORD_MAX)
		, dwMaxJoinCount(0)
	{
	}
};

struct SINSTANCE_TYPE
{
	std::string strManTypeName;
	std::string strSubTypeName;
	SINSTANCE_NAME_ID sInstanceNameId;
	SINSTANCE_TYPE()
		: strManTypeName("")
		, strSubTypeName("")
	{
	}
};

struct SINSTANCE_TYPE_SUB
{
	SINSTANCE_TYPE sInstanceType;
	CBasicTextBox* pInstanceSubTypeName;
	std::vector<SINSTANCE_NAME_ID> vecInstanceNameId;
	DWORD dwUIID;

	SINSTANCE_TYPE_SUB()
		: pInstanceSubTypeName(NULL)
		, dwUIID(DWORD_MAX)
	{
	}
};

class RnCompetitionWindow : public CUIWindowObjectController
{

public:

	enum EM_INSTANCE_TYPE // 인던 대분류 PVP,PVE
	{
		EM_INSTANCE_TYPE_PVP	= 0,
		EM_INSTANCE_TYPE_PVE	= 1,
		EM_INSTANCE_TYPE_SIZE	= 2,
	};

	enum COMPETITION_ID // UI ID
	{
		COMPETITION_WINDOW_NONE = NO_ID,
		COMPETITION_WINDOW_OBJECT,
		COMPETITION_WINDOW_COMMON,

		COMPETITION_WINDOW_PAGE_CTF,
		COMPETITION_WINDOW_PAGE_PVE,
		COMPETITION_WINDOW_PAGE_CDM,

		INSTANCE_LIST_SELECT,

		INSTANCE_TYPE, 

		INSTANCE_TYPE_SUB	= INSTANCE_TYPE + 100,
	};
 
	enum COMPETITION_PAGE_INDEX // 인던 소분류 점령전, 무한의재단
	{
		// PVP
		EM_CAPTURE_THE_FIELD_PAGE,
		EM_CLUB_DEATH_MATH,

		EM_INSTANCE_TYPE_ALL_SIZE,//모든 인던 개수
	};

public:
	RnCompetitionWindow ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~RnCompetitionWindow ();

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
	void	   UpDateWdilBuinding();

protected:
	CBasicLineBoxSmart* CreateLineBox ( const std::string& strControlName, const std::string& strTexInfo, const UIGUID cID=NO_ID );
	CBasicTextBox*	CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR dwColor, int nAlign, UIGUID nWndID = NO_ID );

	void UpdateReportPosControl();
	void ReCalcReportTotalLine();

    void InitializeMatchingInfo();

private:
	RnCompetitionObject* m_pUIObject;
private:
	CCompetitionPage*	m_pPages[EM_INSTANCE_TYPE_ALL_SIZE];
	CBasicScrollBarEx*	m_pReportListScroll;

	std::vector<RnCompetitionInstanceTypeMain*> m_vecInstanceTypeMain;
	std::vector<SINSTANCE_TYPE_SUB> m_vecInstanceTypeSub;

	RnCTFPage*	m_pRnCTFPage;
	RnWailBuinding* m_pRnWailBuinding;

	static const int nMaxReportSpecAct;
	CTipBox*    			m_pSelect;

	int     m_nReportScrollPos;
	int     m_nReportSelect;

	CBasicTextBox* m_pSelectSubType;

public:
	//Rn CDM
	void SetCDMMapinfo(std::string	strMapName,
								int nTimeLimit, 
								int nPlayerLimite, 
								int nLevelLimit );
	void SetCDMMapImageName(std::string strMapImageName);
	void SetCDMBattelState(int nStartTime, int nEndTime);

protected:
	GLGaeaClient* m_pGaeaClient;
public:
	void SetInstanceList(SINSTANCE_TYPE sInstanceType);
};

class MyRnCompetitionWindow : public IRnCompetitionWindow, private RnCompetitionWindow
{
public:
	MyRnCompetitionWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~MyRnCompetitionWindow() {};

public:
	virtual void CreateUIWindowAndRegisterOwnership( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual const bool IsOpen();
	virtual void Open ();
	virtual void Close ();
	virtual void Update(int nPage);
	virtual const UIRECT& GetGlobalPos();    
	virtual void SetCDMMapinfo(std::string	strMapName,
										int nTimeLimit, 
										int nPlayerLimite, 
										int nLevelLimit );
	virtual void SetCDMMapImageName(std::string strMapImageName);
	virtual void SetCDMBattelState(int nStartTime, int nEndTime);
	virtual void UpDateWdilBuinding();
};

#pragma	once

#include "../Util/UIWindowEx.h"

#include "../../InnerInterface.h"
#include "../../../RanLogic/Tournament/GLTournamentClientInfo.h"

class CInnerInterface;
class GLGaeaClient;
class CBasicLineBoxSmart;
class CBasicTextButton;
class CBasicTextBox;
class CCompetitionObject;
class CSwapImage;

class TournamentMoreInfo : public CUIWindowEx
{

protected:
	enum
	{
		COMPETITION_TOP_RANKING_BUTTON = ET_CONTROL_NEXT + 1,
		TOURNAMENT_MINFO_LEFT_BUTTON,
		TOURNAMENT_MINFO_RIGHT_BUTTON,
	};

public:
	TournamentMoreInfo(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~TournamentMoreInfo();

protected:
	struct RANKSLOT : public CUIGroup
	{
		RANKSLOT (GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
			: CUIGroup(pEngineDevice)
			, pGAEACLIENT(pGaeaClient)
			, pTEXTLEVEL(NULL)
			, pTEXTNAME(NULL)
			, pIMAGECLASS(NULL)
			, bENABLE(false)
		{
		}

		GLGaeaClient*  pGAEACLIENT;
		std::string	   strNAME;
		CBasicTextBox* pTEXTLEVEL;
		CBasicTextBox* pTEXTNAME;
		CSwapImage*	   pIMAGECLASS;
		bool		   bENABLE;

		void CreateSubControl( TournamentMoreInfo* pBase );
		void Reset();

		void SetMyRank();
		void SetEnable( const bool bEnable );
		void SetData( const MatchingSystem::GroupInforEployee* sResultData );

		void SetTextPosition( CBasicTextBox* pTextBox, const UIRECT& rcBase );
		void SetImagePosition( CSwapImage* pImage, const UIRECT& rcBase, const UIRECT& rcParent );
		CBasicTextBox* CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID=NO_ID );
	};

	typedef std::vector<RANKSLOT*> RANKSLOTVEC;
	typedef RANKSLOTVEC::iterator  RANKSLOTVEC_ITER;

protected:
	static const UINT nMAX_SLOT = 10;

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void CreateSubControl ();
	void Update();
	void SetPage(DWORD dwPageNum);

public:
	void SetCTFType( const PVP::ENUM_CTF_TYPE emType ) { m_emCTFType = emType; }

	const UIRECT& GetBaseRectLevel()		{ return m_pTextBase_Level->GetLocalPos(); }
	const UIRECT& GetBaseRectClass()		{ return m_pTextBase_Class->GetLocalPos(); }
	const UIRECT& GetBaseRectName()			{ return m_pTextBase_Name->GetLocalPos(); }

private:
	CBasicTextBox* m_pTextBase_Class;
	CBasicTextBox* m_pTextBase_Name;
	CBasicTextBox* m_pTextBase_Level;

	CBasicTextBox* m_pTextBase_PageCount;

	RnButton* m_RnButton_LeftBox;
	RnButton* m_RnButton_RightBox;

	CBasicTextButton* m_pCloseButton;

	RANKSLOTVEC m_vecSlot;

private:
	PVP::ENUM_CTF_TYPE m_emCTFType;

protected:    
	GLGaeaClient* m_pGaeaClient;

};

class MyTournamentMoreInfoWindow: public ITournamentMoreInfoWindow, private TournamentMoreInfo
{
public:
	MyTournamentMoreInfoWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~MyTournamentMoreInfoWindow() {};

public:

	virtual void OnCreate();
	virtual void OnClose() {};
	virtual void OnVisible(bool visible);
	virtual bool GetVisible() {return false;}
	virtual void SetPage(DWORD dwPageNum);
	virtual void OnFrameMove() {};
	virtual const UIRECT& GetGlobalPos();   
};

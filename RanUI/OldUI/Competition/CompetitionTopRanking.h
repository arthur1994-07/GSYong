#pragma	once

#include "../Util/UIWindowEx.h"

#include "../../InnerInterface.h"
#include "../../../RanLogic/GLCompetition.h"

class CInnerInterface;
class GLGaeaClient;
class CBasicLineBoxSmart;
class CBasicTextButton;
class CBasicTextBox;
class CCompetitionObject;
class CSwapImage;

class CCompetitionTopRanking : public CUIWindowEx
{

protected:
	enum
	{
		COMPETITION_TOP_RANKING_BUTTON = ET_CONTROL_NEXT + 1,
	};

public:
	CCompetitionTopRanking(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CCompetitionTopRanking();

protected:
	struct RANKSLOT : public CUIGroup
	{
		RANKSLOT (GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
			: CUIGroup(pEngineDevice)
			, pGAEACLIENT(pGaeaClient)
			, pTEXTRANK(NULL)
			, pTEXTNAME(NULL)
			, pIMAGESCHOOL(NULL)
			, pIMAGECLASS(NULL)
			, bMYRANK(false)
			, bENABLE(false)
		{
		}

		GLGaeaClient*  pGAEACLIENT;
		std::string	   strNAME;
		CBasicTextBox* pTEXTRANK;
		CBasicTextBox* pTEXTNAME;
		CSwapImage*	   pIMAGESCHOOL;
		CSwapImage*	   pIMAGECLASS;
		bool		   bMYRANK;
		bool		   bENABLE;

		void CreateSubControl( CCompetitionTopRanking* pBase );
		void Reset();

		void SetMyRank();
		void SetEnable( const bool bEnable );
		void SetData( const UINT nRank, const PVP::CTF_RANKER_BASIC& sResultData );

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
	virtual void SetVisibleSingle( BOOL bVisible );

public:
	void CreateSubControl ();
	void Update();

public:
	void SetCTFType( const PVP::ENUM_CTF_TYPE emType ) { m_emCTFType = emType; }

	const UIRECT& GetBaseRectRank()			{ return m_pTextBase_Rank->GetLocalPos(); }
	const UIRECT& GetBaseRectSchool()		{ return m_pTextBase_School->GetLocalPos(); }
	const UIRECT& GetBaseRectClass()		{ return m_pTextBase_Class->GetLocalPos(); }
	const UIRECT& GetBaseRectName()			{ return m_pTextBase_Name->GetLocalPos(); }

	void SetTextTime( const CString strTime );
	void ClearTimeText();

private:
	CBasicTextBox* m_pTextBase_Rank;
	CBasicTextBox* m_pTextBase_School;
	CBasicTextBox* m_pTextBase_Class;
	CBasicTextBox* m_pTextBase_Name;
	CBasicTextBox* m_pText_Time;

	CBasicTextButton* m_pCloseButton;

	RANKSLOTVEC m_vecSlot;

private:
	PVP::ENUM_CTF_TYPE m_emCTFType;

protected:    
	GLGaeaClient* m_pGaeaClient;

};

class MyCompetitionTopRanking: public ICompetitionTopRanking, private CCompetitionTopRanking
{
public:
	MyCompetitionTopRanking( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~MyCompetitionTopRanking() {};

public:
	virtual void CreateUIWindowAndRegisterOwnership();
	virtual void Open ( const PVP::ENUM_CTF_TYPE emType );
	virtual void Close ();
	virtual void SetTextTime( const CString strTime );

};

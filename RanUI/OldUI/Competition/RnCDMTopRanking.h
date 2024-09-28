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

class RnCDMTopRanking : public CUIWindowEx
{

protected:
	enum
	{
		RNCDM_TOP_RANKING_BUTTON = ET_CONTROL_NEXT + 1,
	};

public:
	RnCDMTopRanking(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~RnCDMTopRanking();

protected:
	struct RANKSLOT : public CUIGroup
	{
		RANKSLOT (GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
			: CUIGroup(pEngineDevice)
			, pGAEACLIENT(pGaeaClient)
			, pTEXTRANK(NULL)
			, pTEXTCLUBNAME(NULL)
			, pTEXTMASTERNAME(NULL)
			//, pIMAGESCHOOL(NULL)
			, pIMAGECLUBMARK(NULL)
			, pIMAGECLASS(NULL)
			, bMYRANK(false)
			, bENABLE(false)
			, m_dwClubDbnum(-1)
		{
		}

		GLGaeaClient*  pGAEACLIENT;
		std::string	   strNAME;
		CBasicTextBox* pTEXTRANK;
		CBasicTextBox* pTEXTCLUBNAME;
		CBasicTextBox* pTEXTMASTERNAME;
		//CSwapImage*	   pIMAGESCHOOL;
		CUIControl*    pIMAGECLUBMARK;
		CSwapImage*	   pIMAGECLASS;
		bool		   bMYRANK;
		bool		   bENABLE;
		DWORD		   m_dwClubDbnum;

		void CreateSubControl( RnCDMTopRanking* pBase );
		void Reset();

		void SetMyRank();
		void SetEnable( const bool bEnable );
		void SetData( const DWORD nRank, 
					  DWORD dwClubDbNum, 
					  std::string strClubName, 
					  std::string strClubMasterName,
					  DWORD dwClubMarkVer );
		virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );

		void SetTextPosition( CBasicTextBox* pTextBox, const UIRECT& rcBase );
		void SetImagePosition( CSwapImage* pImage, const UIRECT& rcBase, const UIRECT& rcParent );
		void SetImagePosition( CUIControl* pImage, const UIRECT& rcBase, const UIRECT& rcParent );
		CBasicTextBox* CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID=NO_ID );
	};

	typedef std::vector<RnCDMTopRanking::RANKSLOT*> RANKSLOTVEC;
	typedef RANKSLOTVEC::iterator  RANKSLOTVEC_ITER;

protected:
	static const UINT nMAX_SLOT = 10;

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void CreateSubControl ();
	void SetCDMTopList( std::vector<DWORD> vecClubId,
						LONGLONG lnStartTime,
						LONGLONG lnEndTime,
						std::vector<DWORD> vecRank );
	void SetCDMProgessTime(LONGLONG lnStartTime, LONGLONG lnEndTime);

public:
	const UIRECT& GetBaseRectRank()			{ return m_pTextBase_Rank->GetLocalPos(); }
	const UIRECT& GetBaseRectCLubName()		{ return m_pTextBase_ClubMark_Name->GetLocalPos(); }
	//const UIRECT& GetBaseRectClass()		{ return m_pTextBase_Class->GetLocalPos(); }
	const UIRECT& GetBaseRectName()			{ return m_pTextBase_ClubMasterName->GetLocalPos(); }
	void OpenCDMTopRanking ();
	void SetClubMasterClubName(	std::vector<std::string> vecMasterName, std::vector<std::string> vecClubName );

private:
	CBasicTextBox* m_pTextBase_Rank;
	CBasicTextBox* m_pTextBase_ClubMark_Name;
	CBasicTextBox* m_pTextBase_Class;
	CBasicTextBox* m_pTextBase_ClubMasterName;
	CBasicTextBox* m_pText_Time;

	CBasicTextButton* m_pCloseButton;

	RANKSLOTVEC m_vecSlot;

	std::vector<DWORD> m_vecClubId;
	LONGLONG m_lnStartTime;
	LONGLONG m_lnEndTime;
	std::vector<DWORD> m_vecRank;
	std::vector<std::string> m_vecMasterName;
	std::vector<std::string> m_vecClubName;

private:    
	GLGaeaClient* m_pGaeaClient;

};

class MyRnCDMTopRanking: public IRnCDMTopRanking, private RnCDMTopRanking
{
public:
	MyRnCDMTopRanking( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~MyRnCDMTopRanking() {};

public:
	virtual void CreateUIWindowAndRegisterOwnership();
	virtual void OpenCDMTopRanking ();
	virtual void Close ();
	virtual void SetCDMTopList(std::vector<DWORD> vecClubId,
								LONGLONG lnStartTime,
								LONGLONG lnEndTime,
								std::vector<DWORD> vecRank );
	virtual void SetClubMasterClubName(	std::vector<std::string> vecMasterName,
										std::vector<std::string> vecClubName );

};

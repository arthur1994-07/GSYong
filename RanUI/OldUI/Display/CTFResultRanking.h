#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
//#include "../../../RanLogicClient/InterfaceBaseDefine.h"
#include "../../../EngineLib/GUInterface/InterfaceBaseDefine.h"

#include <vector>

class EngineDeviceMan;
class CInnerInterface;
class GLGaeaClient;
class CBasicLineBoxSmart;
class CBasicTextButton;
class CBasicTextBox;
class CBasicScrollBarEx;
class CSwapImage;
class CD3DFontPar;

namespace PVP
{
	struct SCTFRESULTDATA;
};

class CCTFResultRanking : public CUIGroup
{
public:
	enum EMMAINSORTTYPE
	{
		EMMAINSORTTYPE_TOTAL = 0,
		EMMAINSORTTYPE_SM,
		EMMAINSORTTYPE_HA,
		EMMAINSORTTYPE_BH,
	};

protected:
	enum
	{
		CTF_RESULT_RANK_NONE = NO_ID + 1,
		CTF_RESULT_RANK_SCROLLBAR,
	};

protected:
	struct RANKSLOT : public CUIGroup
	{
		RANKSLOT (GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
			: CUIGroup(pEngineDevice)
			, pGAEACLIENT(pGaeaClient)
			, pTEXTRANK(NULL)
			, pTEXTNAME(NULL)
			, pTEXTKILL(NULL)
			, pTEXTDEATH(NULL)
			, pTEXTDAMAGE(NULL)
			, pTEXTHEAL(NULL)
			, pTEXTREBIRTH(NULL)
			, pTEXTCONTRIBUTION(NULL)
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
		CBasicTextBox* pTEXTDAMAGE;
		CBasicTextBox* pTEXTHEAL;
		CBasicTextBox* pTEXTKILL;
		CBasicTextBox* pTEXTDEATH;
		CBasicTextBox* pTEXTREBIRTH;
		CBasicTextBox* pTEXTCONTRIBUTION;
		CSwapImage*	   pIMAGESCHOOL;
		CSwapImage*	   pIMAGECLASS;
		bool		   bMYRANK;
		bool		   bENABLE;

		void CreateSubControl( CCTFResultRanking* pBase );
		void Reset();

		void SetMyRank();
		void SetEnable( const bool bEnable );
		void SetData( const UINT nRank, const PVP::SCTFRESULTDATA* pResultData );

		void SetTextPosition( CBasicTextBox* pTextBox, const UIRECT& rcBase );
		void SetImagePosition( CSwapImage* pImage, const UIRECT& rcBase, const UIRECT& rcParent );
		CBasicTextBox* CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID=NO_ID );

	};

	typedef std::vector<RANKSLOT*> RANKSLOTVEC;
	typedef RANKSLOTVEC::iterator  RANKSLOTVEC_ITER;

protected:
	static const UINT nMAX_SLOT = 10;

public:
	CCTFResultRanking ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CCTFResultRanking ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void CreateSubControl ();
	void Update();

public:
	void ResetScrollBar();

public:
	void Update_SortTotal();
	void Update_SortSM();
	void Update_SortHA();
	void Update_SortBH();

protected:
	CBasicTextBox* CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID=NO_ID );
	CUIControl*	CreateColorBackgroundControl ( char* szControlKeyword, const D3DCOLOR& cColor, int nAlign=UI_FLAG_DEFAULT );

public:
	const UIRECT& GetBaseRectRank()			{ return m_pTextBase_Rank->GetLocalPos(); }
	const UIRECT& GetBaseRectSchool()		{ return m_pTextBase_School->GetLocalPos(); }
	const UIRECT& GetBaseRectClass()		{ return m_pTextBase_Class->GetLocalPos(); }
	const UIRECT& GetBaseRectName()			{ return m_pTextBase_Name->GetLocalPos(); }
	const UIRECT& GetBaseRectDamage()		{ return m_pTextBase_Damage->GetLocalPos(); }
	const UIRECT& GetBaseRectHeal()			{ return m_pTextBase_Heal->GetLocalPos(); }
	const UIRECT& GetBaseRectKill()			{ return m_pTextBase_Kill->GetLocalPos(); }
	const UIRECT& GetBaseRectDeath()		{ return m_pTextBase_Death->GetLocalPos(); }
	const UIRECT& GetBaseRectRebirth()		{ return m_pTextBase_Rebirth->GetLocalPos(); }
	const UIRECT& GetBaseRectContribution() { return m_pTextBase_Contribution->GetLocalPos(); }

private:
	CBasicTextBox* m_pTextBase_Rank;
	CBasicTextBox* m_pTextBase_School;
	CBasicTextBox* m_pTextBase_Class;
	CBasicTextBox* m_pTextBase_Name;
	CBasicTextBox* m_pTextBase_Damage;
	CBasicTextBox* m_pTextBase_Heal;
	CBasicTextBox* m_pTextBase_Kill;
	CBasicTextBox* m_pTextBase_Death;
	CBasicTextBox* m_pTextBase_Rebirth;
	CBasicTextBox* m_pTextBase_Contribution;

	RANKSLOTVEC m_vecSlot;
	RANKSLOT*	m_pMyRank;

	CBasicScrollBarEx* m_pScroll;

private:
	UINT m_nScrollPos;
	EMMAINSORTTYPE m_emSortType;

protected:
	GLGaeaClient*	 m_pGaeaClient;
	CInnerInterface* m_pInterface;

};

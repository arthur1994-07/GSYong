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
class CBasicButton;
class CItemImageEx;

namespace InstanceSystem
{
	struct RNCDM_RANK_SLOT;
};

using namespace InstanceSystem;

const int nRewardItem = 4;

class RnCDMRankList : public CUIGroup
{
public:
	enum EMMAINSORTTYPE
	{
		EMMAINSORTTYPE_TOTAL = 0,
	};

protected:
	enum
	{
		RNCDM_RESULT_RANK_NONE = NO_ID + 1,
		RNCDM_RESULT_RANK_SCROLLBAR,
		RNCDM_INFOBUTTON,
	};

protected:
	struct RANKSLOT : public CUIGroup
	{
		RANKSLOT (CInnerInterface* pInterface, GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
			: CUIGroup(pEngineDevice)
			, pInterface(pInterface)
			, pGAEACLIENT(pGaeaClient)
			, pTEXTRANK(NULL)
			, pTEXTMASTERNAME(NULL)
			, pTEXTGETPOINT(NULL)
			, pTEXTREBIRTHNUM(NULL)
			, pTEXTKILLNUM(NULL)
			, pTEXTDEATHNUM(NULL)
			, pIMAGECLUBMARK(NULL)
			, bMYRANK(false)
			, bENABLE(false)
			, m_nSlotIndex(0)
		{
			for (int i = 0; i < nRewardItem; i ++ )
			{
				pIMAGEREWARDITEM[i] = NULL;
			}
		}
		int m_nSlotIndex;
		CInnerInterface* pInterface;
		GLGaeaClient*	pGAEACLIENT;
		std::string		strNAME;
		CBasicTextBox*	pTEXTRANK;
		CBasicTextBox*	pTEXTCLUBNAME;
		CBasicTextBox*	pTEXTMASTERNAME;
		CBasicButton*	pINFO;		
		CBasicTextBox*	pTEXTGETPOINT;
		CBasicTextBox*	pTEXTKILLNUM;
		CBasicTextBox*	pTEXTDEATHNUM;
		CBasicTextBox*	pTEXTREBIRTHNUM;
		CUIControl*		pIMAGECLUBMARK;
		CUIControl*		pIMAGEREWARDITEM[nRewardItem];
		bool			bMYRANK;
		bool			bENABLE;
		int				nPosX;
		int				nPosY;

		void CreateSubControl( RnCDMRankList* pBase );
		virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
		virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
		void Reset();

		void SetMyRank();
		void SetEnable( const bool bEnable );
		void SetData( const UINT nRank, InstanceSystem::RNCDM_RANK_SLOT* pResultData );
		void SetMyClubImage( const DWORD dwClubDbNum, const DWORD dwVer );

		void SetTextPosition( CBasicTextBox* pTextBox, const UIRECT& rcBase );
		void SetImagePositionCenterY( CItemImageEx* pImage, const UIRECT& rcBase, const UIRECT& rcParent );
		void SetImagePositionCenterY( CSwapImage* pImage, const UIRECT& rcBase, const UIRECT& rcParent );
		void SetImagePositionCenterY( CUIControl* pImage, const UIRECT& rcBase, const UIRECT& rcParent );
		void SetButtonPositionCenterY( CBasicButton* pButton, const UIRECT& rcBase, const UIRECT& rcParent );
		CBasicTextBox* CreateStaticControl(const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID=NO_ID );

	};

	typedef std::vector<RANKSLOT*> RANKSLOTVEC;
	typedef RANKSLOTVEC::iterator  RANKSLOTVEC_ITER;

protected:
	static const UINT nMAX_SLOT = 10;

public:
	RnCDMRankList ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice, std::vector<InstanceSystem::RNCDM_RANK_SLOT>* pvecRnCdmRankSlot );
	virtual	~RnCDMRankList ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void CreateSubControl ();
	void UpdateList();

public:
	void ResetScrollBar();

public:
	void Update_SortTotal();

protected:
	CBasicTextBox* CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, int nAlign, const UIGUID& cID=NO_ID );
	CUIControl*	CreateColorBackgroundControl ( char* szControlKeyword, const D3DCOLOR& cColor, int nAlign=UI_FLAG_DEFAULT );

public:
	const UIRECT& GetBaseRectRank()			{ return m_pTextBase_Rank->GetLocalPos(); }
	const UIRECT& GetBaseRectClubName()		{ return m_pTextBase_Club_Name->GetLocalPos(); }
	const UIRECT& GetBaseRectClubMaster()	{ return m_pTextBase_Club_Master->GetLocalPos(); }
	const UIRECT& GetBaseRectGetPoint()		{ return m_pTextBase_Get_Point->GetLocalPos(); }
	const UIRECT& GetBaseRectKillNum()		{ return m_pTextBase_Kill_Num->GetLocalPos(); }
	const UIRECT& GetBaseRectDeathNum()		{ return m_pTextBase_Death_Num->GetLocalPos(); }
	const UIRECT& GetBaseRectRebirth()		{ return m_pTextBase_Rebirth->GetLocalPos(); }
	const UIRECT& GetBaseRectReward()		{ return m_pTextBase_Reward->GetLocalPos(); }

private:
	CBasicTextBox* m_pTextBase_Rank;
	CBasicTextBox* m_pTextBase_Club_Name;
	CBasicTextBox* m_pTextBase_Club_Master;
	CBasicTextBox* m_pTextBase_Get_Point;
	CBasicTextBox* m_pTextBase_Kill_Num;
	CBasicTextBox* m_pTextBase_Death_Num;
	CBasicTextBox* m_pTextBase_Rebirth;
	CBasicTextBox* m_pTextBase_Reward;

	RANKSLOTVEC m_vecSlot;
	RANKSLOT*	m_pMyRank;

	CBasicScrollBarEx* m_pScroll;

private:
	UINT m_nScrollPos;
	EMMAINSORTTYPE m_emSortType;

protected:
	GLGaeaClient*	 m_pGaeaClient;
	CInnerInterface* m_pInterface;
public:
	std::vector<InstanceSystem::RNCDM_RANK_SLOT>* m_pvecRnCdmRankSlot;
	void ClearCdmRankList();

};

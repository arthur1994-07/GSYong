#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../EngineLib/G-Logic/GLDefine.h"
//#include "../../../RanLogicClient/InterfaceBaseDefine.h"
#include "../../../EngineLib/GUInterface/InterfaceBaseDefine.h"

#include "../../GfxInterface.h"
#include "../../Display/GfxDisplay.h"
#include "../OldInterface.h"

class CD3DFontPar;
class CBasicLineBoxEx;
class CBasicTextBox;
class CBasicProgressBar;
//class CAuthenticatorCTFDisplay;
class GLGaeaClient;
class OldInterface;

class CTargetInfoDisplay : public CUIGroup
{
//public:
//	enum
//	{
//		nSCHOOLMARK = 3,
//		nPARTYMARK = 5,
//		NODATA = UINT_MAX
//	};

public:
	enum EMINFOTYPE
	{
		EMINFOTYPE_NULL = 0,
		EMINFOTYPE_NORMAL,
		EMINFOTYPE_DETAIL,
		EMINFOTYPE_CTF_AUTH,
	};

public:
	CTargetInfoDisplay(GLGaeaClient* pGaeaClient, OldInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CTargetInfoDisplay();

protected:
    OldInterface* m_pInterface;

public:
	void	CreateSubControl();
	CUIControl*	CreateControl( const char* szControl, WORD wAlignFlag );

public:
    void	SetMouseOverTargetInfo( const STARGETID& sTargetID, float fPercent, CString strName, D3DCOLOR dwColor ); /*, int nSchoolMark = NODATA, int nPartyMark = NODATA );*/
    void    SetMouseOverAuthenticatorInfo( const STARGETID& sTargetID, float fPercentSM, float fPercentHA, float fPercentBH, ENUM_SCHOOL emAuthSchool );
	void	SetSelectTargetInfo( const STARGETID& sTargetID, float fPercent, CString strName, D3DCOLOR dwColor );
	void	SetSelectAuthenticatorInfo( const STARGETID& sTargetID, float fPercentSM, float fPercentHA, float fPercentBH, ENUM_SCHOOL emAuthSchool );

public:
	const bool IsMouseOverVisible() { return (m_sMouseOver.m_emType != EMINFOTYPE_NULL); }
	void SetMouseOverVisibleOff();
	const CString& GetMouseOverTargetName() const { return m_sMouseOver.m_strName; }

	const UIRECT& GetMouseOverGlobalPos();
	void SetMouseOverGlobalPos( const D3DXVECTOR2& vPos );

public:
	void SetSelectVisibleOff();

	const UIRECT& GetSelectGlobalPos();
	void SetSelectGlobalPos( const D3DXVECTOR2& vPos );

public:
	const bool CheckPriority  ( const UI::EMTARGETINFO_TYPE emInfoType, const STARGETID& sTargetID );
	const bool UpdatePriority ( const UI::EMTARGETINFO_TYPE emInfoType, const STARGETID& sTargetID );

private:
	struct SINFO
	{
		GLGaeaClient* m_pGaeaClient;
		EMINFOTYPE m_emType;
		EMCROWCOLORTYPE m_emColorType;
		CString	m_strName;
		UIRECT m_rcPos;
		bool m_bCTFVisible;

		//! Ÿ�� ǥ��(�Ϲ�)
#ifndef DEF_IMPROVE_DISPLAY_NAME
		CBasicLineBoxEx*    m_pNormalLineBox;
#endif
		CBasicTextBox*		m_pNormalNameBox;
		CBasicProgressBar*	m_pNormalHP[EMCROWCOLORTYPE_SIZE];
		CUIControl*			m_pNormalNameBoxDummy;
		CUIControl*			m_pNormalBase;

		//! Ÿ�� ǥ��(����)
#ifndef DEF_IMPROVE_DISPLAY_NAME
		CBasicLineBoxEx*    m_pDetailLineBox;
#endif
		CBasicTextBox*		m_pDetailNameBox;
		CBasicProgressBar*	m_pDetailHP[EMCROWCOLORTYPE_SIZE];
		CUIControl*			m_pDetailNameBoxDummy;
		CUIControl*			m_pDetailBase;


		//! CTF ������ ǥ��
		//CAuthenticatorCTFDisplay* m_pCTF;

		//! Ÿ��
		STARGETID m_sTargetID;

		SINFO( GLGaeaClient* pGaeaClient )
			: m_pGaeaClient(pGaeaClient)
			, m_emType(EMINFOTYPE_NULL)
			, m_emColorType(EMCROWCOLORTYPE_NULL)
#ifndef DEF_IMPROVE_DISPLAY_NAME
			, m_pNormalLineBox(NULL)
#endif
			, m_pNormalNameBox(NULL)
			, m_pNormalNameBoxDummy(NULL)
			, m_pNormalBase(NULL)
#ifndef DEF_IMPROVE_DISPLAY_NAME
			, m_pDetailLineBox(NULL)
#endif
			, m_pDetailNameBox(NULL)
			, m_pDetailNameBoxDummy(NULL)
			, m_pDetailBase(NULL)
			, m_bCTFVisible( false )
		//	, m_pCTF(NULL)
		{
			memset(m_pNormalHP, 0, sizeof(m_pNormalHP));
			memset(m_pDetailHP, 0, sizeof(m_pDetailHP));
		}

		void CreateSubControl( CUIGroup* pParent, CD3DFontPar* pFont, EngineDeviceMan* pEngineDevice );

		void SetVisibleOff();

		void SetNormalVisible( const STARGETID& sTargetID );
		void SetDetailVisible( const STARGETID& sTargetID );
		void SetCTFVisible( const STARGETID& sTargetID );

		void SetGlobalPos ( const D3DXVECTOR2& vPos );
		void SetNonBoundaryCheck ();

		void SetNormalAlignControl( const UIRECT& rcOldParent, const UIRECT& rcNewParent );
		void SetDetailAlignControl( const UIRECT& rcOldParent, const UIRECT& rcNewParent );
	};
	
	SINFO m_sMouseOver;
	SINFO m_sSelect;

private:
	CD3DFontPar*	m_pFont8;
	GLGaeaClient*	m_pGaeaClient;

//private:
//	CUIControl*		m_pSchoolMark[nSCHOOLMARK];
//	CUIControl*		m_pPartyMark[nPARTYMARK];
};


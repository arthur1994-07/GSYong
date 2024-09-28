#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogicClient/Crow/GLCrowRenList.h"

class CBasicTextBox;
class CBasicLineBoxEx;
class CSwapImage;
class GLGaeaClient;
class CInnerInterface;
class CBasicProgressBar;

class CNameDisplay : public CUIGroup
{
public:
	enum
	{
		nSCHOOLMARK		= 3,
		nPARTYMARK		= 5,
        nSkillBuff		= 4,
        nSNSMARK		= 2,
	};

public:
	CNameDisplay(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CNameDisplay ();

public:
	void CreateSubControl ();

//public:
//	void SetNameColor ( const D3DCOLOR& dwColor );
//	void SetClubNameColor ( const D3DCOLOR& dwColor );

public:
	void SetName ( const CString& strName, const D3DCOLOR& dwColor, WORD wSchoolMark = NODATA, DISP_PARTY emPartyMark = NODATA, DWORD dwSNSFlag = 0 );
	void SetClubName ( const CString& strName, const D3DCOLOR& dwColor );
	void SetOwnerName( const CString& strName, const D3DCOLOR& dwColor );
    void SetTitleName( const CString& strTitle, const D3DCOLOR& dwColor );
	void SetBuffIcon( int nIndex, bool bVisible );
    void SetHP ();
	void SetShowCountryMark ( bool bShowCountry );

	bool IsMemPool()					{ return m_bUsedMemPool; }
	void SetMemPool()					{ m_bUsedMemPool = true; }

	void INITIALIZE();

protected:
    CUIControl* CreateControl ( const char* szControl, WORD wAlignFlag );

public:
	virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );

private:
    CBasicTextBox*		m_pNickNameBox;
	CBasicTextBox*		m_pNameBox;
    CBasicTextBox*      m_pTitleBox;
#ifndef DEF_IMPROVE_DISPLAY_NAME
	CBasicLineBoxEx*	m_pNameLineBox;
    CBasicLineBoxEx*	m_pNameLineBoxCD;
#endif
    CBasicProgressBar*	m_pMiniHP;
	
	UIRECT				m_rcNameBox; // MEMO : static 변수는...?

private:
	CUIControl*			m_pSchoolMark[nSCHOOLMARK];
	CUIControl*			m_pPartyMark[nPARTYMARK];
	CSwapImage*			m_pClubMarkBackground;
	CUIControl*			m_pClubMark;
	CSwapImage*			m_pCountryMark;
    CUIControl*			m_pBuffIcon[nSkillBuff];
    CUIControl*			m_pSNSMark[nSNSMARK];

	UIRECT				m_rcSchoolMark[nSCHOOLMARK];
    UIRECT				m_rcPartyMark[nPARTYMARK];
    UIRECT				m_rcSNSMark[nSNSMARK];
	
private:
	bool				m_bDISP;
	CROWREN				m_sDISP_INFO;
	bool				m_bUsedMemPool; // MEMO : 메모리 풀을 사용하기 위한 플래그]
	bool				m_bClub;
    bool                m_bTitle;
	bool				m_bShowCountry;

public:
	bool DIFFERENT ( const CROWREN &sINFO );
	void UPDATE ( const CROWREN &sINFO );
	void SET_INFO ( const CROWREN &sINFO, const TCHAR *szOwnerName );
	void SET_DISP ( bool bDISP )						{ m_bDISP = bDISP; }

public:
	DWORD GETCOUNT ()			{ return m_sDISP_INFO.m_dwCOUNT; }
	bool GET_DISP ()			{ return m_bDISP; }
	EMCROW GETCROW ()			{ return m_sDISP_INFO.m_emCROW; }
	DWORD GETCTRLID()			{ return m_sDISP_INFO.m_dwID; }
	const CROWREN* GET_INFO ()	{ return &m_sDISP_INFO; }
	
public:
	BOOL IsRELAY ()				{ return NULL != (m_sDISP_INFO.m_dwTYPE&(TYPE_TARGET|TYPE_OURS)); }
	BOOL IsTARGET ()			{ return NULL != (m_sDISP_INFO.m_dwTYPE&(TYPE_TARGET)); }
	BOOL IsClubTARGET ()		{ return NULL != (m_sDISP_INFO.m_dwTYPE&(TYPE_CLUBBATTLE)); }
	BOOL IsPMARKET ()			{ return m_sDISP_INFO.m_bPMARKET; }
	BOOL IsCDCERTIFY ()			{ return NULL != (m_sDISP_INFO.m_dwTYPE&(TYPE_CLUBCD)); }

protected:
	GLGaeaClient*    m_pGaeaClient;
    CInnerInterface* m_pInterface;
};

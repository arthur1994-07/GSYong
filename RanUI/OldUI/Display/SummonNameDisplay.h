#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogicClient/Crow/GLCrowRenList.h"

class CBasicTextBox;
class CBasicLineBoxEx;
class CBasicProgressBar;
class GLSummonClient;
class GLGaeaClient;

class CSummonNameDisplay : public CUIGroup
{

public:
	CSummonNameDisplay(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice);
	virtual	~CSummonNameDisplay();

private:
    GLGaeaClient* m_pGaeaClient;
    CBasicTextBox*		m_pNameBox;
    CBasicLineBoxEx*	m_pNameLineBox;
    CBasicProgressBar*	m_pHP;
    UIRECT				m_rcNameBox; // MEMO : static 변수는...?

private:
    BOOL				m_bSimple;

private:
    bool				m_bDISP;
    DWORD               m_dwCOUNT;
    std::tr1::shared_ptr<GLSummonClient> m_spSummon;
    DWORD               m_dwGlobID;

private:
    DISP_PARTY          m_emPARTY;
    DWORD		        m_dwTYPE;
    DWORD               m_dwCOLOR;
    D3DXVECTOR3         m_vPOS;
    char                m_strSummonName[CHAR_SZNAME];
    char                m_strOwnerName [CHAR_SZNAME];

private:
    //CROWREN				m_sDISP_INFO;
    bool				m_bUsedMemPool; // MEMO : 메모리 풀을 사용하기 위한 플래그

public:
	void CreateSubControl ();

	//public:
	//	void SetNameColor ( const D3DCOLOR& dwColor );
	//	void SetClubNameColor ( const D3DCOLOR& dwColor );

public:
	BOOL SetSummmon(std::tr1::shared_ptr<GLSummonClient> spSummon);

	bool IsMemPool()  { return m_bUsedMemPool; }
	void SetMemPool() { m_bUsedMemPool = true; }

protected:
	void SetName      ( const CString& strName, const D3DCOLOR& dwColor );
	void SetOwnerName ( const CString& strName, const D3DCOLOR& dwColor );

protected:
	CUIControl*        CreateControl ( const char* szControl, WORD wAlignFlag );

public:
	virtual HRESULT    Render ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	void                 INITIALIZE();

	void                 UPDATE      ( DWORD _dwCOUNT );
	void                 RESIZE      ();

public:
    bool                 DIFFERENT(std::tr1::shared_ptr<GLSummonClient> spSummon);
	void                 SET_DISP    ( bool bDISP )	    { m_bDISP   = bDISP;    }

public:
	DWORD                GETCOUNT    ()	        { return m_dwCOUNT; }
	bool                 GET_DISP    ()	        { return m_bDISP; }
	DWORD                GETCTRLID   ()	        { return m_dwGlobID; }
	const D3DXVECTOR3&   GETPOSITION ()         { return m_vPOS;     }

public:
	/*
	BOOL           IsRELAY ()			{ return NULL != (m_sDISP_INFO.m_dwTYPE&(TYPE_TARGET|TYPE_OURS)); }
	BOOL           IsTARGET ()			{ return NULL != (m_sDISP_INFO.m_dwTYPE&(TYPE_TARGET)); }
	BOOL           IsPMARKET ()			{ return m_sDISP_INFO.m_bPMARKET; }
	BOOL           IsCDCERTIFY ()		{ return NULL != (m_sDISP_INFO.m_dwTYPE&(TYPE_CLUBCD)); }
	*/
};

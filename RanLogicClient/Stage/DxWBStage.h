#pragma once

//#include "../../EngineLib/GUInterface/UIDataType.h"
//#include "../../EngineLib/DxLand/DxLandMan.h"
//
#include "../../RanLogic/GLogicData.h"
//
//#include "../Char/DxCharSham.h"
#include "./DxStage.h"


class OutInterfaceBase;
class GLGaeaClient;
class DxGlobalStage;


class DxWBStage : public DxStage
{
public:

    DxWBStage(DxGlobalStage* pGlobalStage, GLGaeaClient* pGaeaClient);
    virtual ~DxWBStage(void);

protected:
    DxGlobalStage* m_pGlobalStage;
    GLGaeaClient* m_pGaeaClient;

public:
	BOOL GetUsedMaterialSystem();

	void MsgProcess ( NET_MSG_GENERIC* nmg );

	HRESULT FrameMove( float fTime, float fElapsedTime, BOOL bNotRendering );

	virtual	HRESULT Render ();
};

//
//class DxWBStage : public DxStage
//{
//public:
//
//    DxWBStage(DxGlobalStage* pGlobalStage, GLGaeaClient* pGaeaClient, OutInterfaceBase* pInterface);
//    virtual ~DxWBStage(void);
//
//protected:
//    DxGlobalStage* m_pGlobalStage;
//    GLGaeaClient* m_pGaeaClient;
//	D3DCAPSQ			m_d3dCaps;
//	D3DSURFACE_DESC		m_d3dsdBackBuffer;
//
//	float				m_fElapsedTime;
//
//	DxLandMan			m_LandCharSlt;
//	//DxLandMan			m_LandCharSlt_s01;
//	//DxLandMan			m_LandCharSlt_s02;
//	//DxLandMan			m_LandCharSlt_s03;
//	DxLandMan			m_LandLogin;
//
//	//	케릭터 선택 stage로 이동이 가능할려면 서버에 있는 모든 케릭터 정보를 수신해야한다.
//	int					m_nStartCharNum;			//	서버의 생성된 케릭 숫자.
//	int					m_nStartCharLoad;			//	클라이언트에 로드된 케릭 숫자. ( 정보 수신된. )
//
//	int					m_nChaSNum;					//	생성된 케릭터 숫자. ( 종전 케릭 수 + 새로 생성된 케릭 수 )
//	int					m_nChaIDs[MAX_SERVERCHAR];
//
//	DWORD				m_dwCharSelect;
//	SCHARINFO_LOBBY		m_CharInfo[MAX_SERVERCHAR];
//
//	int					m_nCharStart;
//	DxCharSham			m_CharSham[MAX_SELECTCHAR];
//	DxCharSham			m_NewCharSham;
//
//	GLCHAR_NEWINFO		m_sCharNewInfo;
//
//	float				m_fShiftTime;
//	BOOL				m_bShiftButton;
//	BOOL				m_bShiftLBDown;
//	BOOL				m_bShiftRBDown;
//
//	BOOL				m_bUseInterface;
//	BOOL				m_bRenderSkip;
//	float				m_fRenderSkipTime;
//
//	bool				m_bStyle;
//	D3DXVECTOR3			m_vDir;
//
//    OutInterfaceBase*    m_pOutInterface;
//
//    D3DXVECTOR3 m_vCharPos[ MAX_SELECTCHAR ];
//
//#ifndef INCLUDE_SCALEFORM
//	UIGUID				m_nControlIDBack;		// 이전 Stage를 가지고 있는다. 스테이지 변화를 감지하기 위함.
//#else
//	int					m_nStageBG;
//	bool				m_bChangedStage;
//#endif
//
//	DWORD				m_dwCharIDForWorldBattle;
//
//
//public:
//	BOOL				m_bGameJoin;				// 여러번 서버에 조인 메세지를 보내지 않기 위한 플래그
//
//private:
//
//	TextureResource m_textureRes;
//	CString				m_strTextureName;
//	UIRECT				m_rcVertex;
//	UIRECT				m_rcTexture;
//	TEXTUREVERTEX		m_Vertices[4];
//
//private:
//	std::set<float>	m_setCheckGpuElapsedTime;
//	float			m_fCheckGpuTimeADD;
//	int				m_nCheckGpuCountADD;
//
//private:
//
//	void SetVertexPos ( TEXTUREVERTEX VertexArray[6], float LEFT, float TOP, float SIZEX, float SIZEY );
//	void SetTexturePos ( TEXTUREVERTEX VertexArray[6], float LEFT, float TOP, float SIZEX, float SIZEY, float TEX_SIZEX, float TEX_SIZEY );
//	HRESULT	CreateVB ( TEXTUREVERTEX VertexArray[6] );
//	
////protected:
////	DxLandMan* GetCharNewLand ();
//
//public:
//	EMCHARINDEX GetCreateClass ()				{ return m_sCharNewInfo.m_emIndex; }
//	WORD GetCreateSchool ()						{ return m_sCharNewInfo.m_wSchool; }
//
//	const GLCHAR_NEWINFO& GetNewCharInfo ()		{ return m_sCharNewInfo; }
//	const int GetChaSNum () const				{ return m_nChaSNum; }
//
//	void  SetStyleStep( bool bStyle );
//	void  RotateChar( bool bLeft );
//
//protected:
//	void ShiftChar( float fElapsedTime );
//	BOOL ShiftRightChar();
//	BOOL ShiftLeftChar();
//	void ShiftCharInfo();
//
//	void InitRenderSkip();
//
//public:	
//	//void ToCameraPos ( int n );
//
//	void SelCharClass ( WORD nIndex );
//	void SelCharSchool ( WORD nIndex );
//
//	void SelCharFace ( WORD nIndex )			{ m_sCharNewInfo.m_wFace = nIndex; }
//	void SelCharHair ( WORD nIndex )			{ m_sCharNewInfo.m_wHair = nIndex; }
//	void SelCharHairColor ( WORD wColor )		{ m_sCharNewInfo.m_wHairColor = wColor; }
//
//	WORD ShiftCharClass ( EMSHIFT emShift, WORD wClassMax );
//	void ShiftCharSchool ( EMSHIFT emShift );
//	void ShiftCharFace ( EMSHIFT emShift );
//	void ShiftCharHair ( EMSHIFT emShift );
//
//	void DelChar ( int nCharID );
//
//	BOOL ShiftCharIndex(INT nIndex );
//
//	void ShiftLBDown()							{ m_bShiftLBDown = TRUE; }
//	void ShiftRBDown()							{ m_bShiftRBDown = TRUE; }
//	BOOL IsShiftLeft()							{ return (m_nCharStart-MAX_SELECTCHAR)>=0;}
//	BOOL IsShiftRight()							{ return (m_nCharStart+MAX_SELECTCHAR)<m_nChaSNum;}
//
//	INT GetPageNum()							{ return (m_nCharStart/MAX_SELECTCHAR)+1; }
//	
//	void SelectCharacter( DWORD dwCharSelect );
//
//public:
//	SCHARINFO_LOBBY* GetCharInfo( INT nIndex );
//	SCHARINFO_LOBBY* GetSelectCharInfo();
//	DWORD IsCharSelect()						{ return (m_dwCharSelect!=MAX_SELECTCHAR); }
//	BOOL IsStartReady ()						{ return m_nStartCharNum==m_nStartCharLoad; }
//
//#ifdef INCLUDE_SCALEFORM
//	void SetChangedStage( bool bChanged )		{ m_bChangedStage = bChanged; }
//#endif
//
//public:
//	void OnInitLogin ();
//
//public:
//	virtual HRESULT OneTimeSceneInit ( HWND hWnd, WORD wWidth, WORD wHeight, const char* szAppPath );
//	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
//	virtual HRESULT RestoreDeviceObjects ();
//	virtual HRESULT InvalidateDeviceObjects ();
//	virtual HRESULT DeleteDeviceObjects ();
//	virtual HRESULT FinalCleanup();
//
//protected:
//	void UpdateSelSkinChar ( DxSkinChar* pSkinObj, EMCHARINDEX emIndex, WORD wFace, WORD wHair, WORD wHairColor );
//	HRESULT DxEnvironmentRender( DxLandMan* pWLD );
//	HRESULT MaterialSystemStretchRect( DxLandMan* pWLD );
//
//	void RenderWorldCharEffect();
//
//	void DxRenderDefferedOpaque( DxLandMan* pWLD );
//	void DxRenderDefferedSoftAlpha( DxLandMan* pWLD );
//
//public:
//	virtual HRESULT FrameMove ( float fTime, float fElapsedTime, BOOL bNotRendering );
//	virtual HRESULT Render ();
//
//	virtual BOOL	GetUsedMaterialSystem();
//
//public:
//	virtual void MsgProcess(NET_MSG_GENERIC* nmg);
//
//    void MsgLobbyCharInfoAC( NET_MSG_GENERIC* nmg );
//	void MsgLobbyCharInfoACEnd( NET_MSG_GENERIC* nmg );
//
//    virtual HRESULT ReSizeWindow ( WORD wWidth, WORD wHeight );
//};
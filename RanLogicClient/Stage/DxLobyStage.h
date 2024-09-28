#pragma once

#include "../../EngineLib/GUInterface/UIDataType.h"
#include "../../EngineLib/DxLand/DxLandMan.h"

#include "../../RanLogic/GLogicData.h"

#include "../Char/DxCharSham.h"
#include "./DxStage.h"

struct TEXTUREVERTEX;



struct GLCHAR_NEWINFO
{
	char				m_szName[CHAR_SZNAME];		//	�̸�. (����)

	EMCHARINDEX			m_emIndex;					//	����. (����)
	WORD				m_wSchool;					//	�п�.
	WORD				m_wHair;					//	�Ӹ�ī��.
	WORD				m_wHairColor;				//  �Ӹ�����
	WORD				m_wFace;					//	�󱼸��.
	WORD				m_wSex;						//  ����

	GLCHAR_NEWINFO () :
		m_emIndex(GLCI_NUM_ACTOR),
		m_wSchool(0),
		m_wHair(0),
		m_wHairColor(0),
		m_wSex(0),
		m_wFace(0)
	{
	}

	EMCHARCLASS GetClass ()		{ return CharIndexToClass(m_emIndex); }

	bool CHECKVALID () const
	{
		if ( m_emIndex>=GLCI_NUM_ACTOR )	return false;
		return true;
	}

	void RESET ()
	{
		m_emIndex = (GLCI_NUM_ACTOR);
		m_wSchool = (0);
		m_wHair = (0);
		m_wFace = (0);
		m_wHairColor = 0;
		m_wSex = 0;
	}
};

//	-------------------- [ CUSTOM VERTEX ���� ] --------------------
const	DWORD TEXTUREVERTEXFVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
struct TEXTUREVERTEX
{
	union
	{
		struct { D3DXVECTOR4 vPos; };
		struct { float x, y, z, rhw; };
	};

	DWORD Diffuse;

	union
	{
		struct { D3DXVECTOR2 vUV; };
		struct { float tu, tv; };
	};

	TEXTUREVERTEX () :
		x(0.0f),
		y(0.0f),
		z(0.0f),
		rhw(1.0f),
		Diffuse(0xFFFFFFFF),
		tu(0.0f),
		tv(0.0f)
	{		
	}

	TEXTUREVERTEX ( float _x, float _y, float _tu, float _tv ) :
		x(_x),
		y(_y),
		z(1.0f),
		rhw(1.0f),
		Diffuse(0xFFFFFFFF),
		tu(_tu),
		tv(_tv)
	{		
	}

	void	SetPos ( float _x, float _y )
	{
		x = _x;
		y = _y;
	}

	void	SetTexPos ( float _tu, float _tv )
	{
		tu = _tu;
		tv = _tv;
	}
};

class GLGaeaClient;
class DxGlobalStage;

class DxLobyStage : public DxStage
{
public:
	enum EMSHIFT
	{
		EM_BEGIN,
		EM_PREV,
		EM_NEXT,
	};

public:
    DxLobyStage(DxGlobalStage* pGlobalStage, GLGaeaClient* pGaeaClient);
    virtual ~DxLobyStage(void);

protected:
    DxGlobalStage* m_pGlobalStage;
    GLGaeaClient* m_pGaeaClient;
	D3DCAPSQ			m_d3dCaps;
	D3DSURFACE_DESC		m_d3dsdBackBuffer;

	float				m_fElapsedTime;

	boost::shared_ptr<DxLandMan>			m_spLandCharSlt;
	//DxLandMan			m_LandCharSlt_s01;
	//DxLandMan			m_LandCharSlt_s02;
	//DxLandMan			m_LandCharSlt_s03;
	boost::shared_ptr<DxLandMan>			m_spLandLogin;

	//	�ɸ��� ���� stage�� �̵��� �����ҷ��� ������ �ִ� ��� �ɸ��� ������ �����ؾ��Ѵ�.
	int					m_nStartCharNum;			//	������ ������ �ɸ� ����.
	int					m_nStartCharLoad;			//	Ŭ���̾�Ʈ�� �ε�� �ɸ� ����. ( ���� ���ŵ�. )

	int					m_nChaSNum;					//	������ �ɸ��� ����. ( ���� �ɸ� �� + ���� ������ �ɸ� �� )
	int					m_nChaIDs[MAX_SERVERCHAR];

	DWORD				m_dwCharSelect;
	SCHARINFO_LOBBY		m_CharInfo[MAX_SERVERCHAR];

	int					m_nCharStart;
	DxCharSham			m_CharSham[MAX_SELECTCHAR];
	DxCharSham			m_NewCharSham;

	GLCHAR_NEWINFO		m_sCharNewInfo;

	float				m_fShiftTime;
	BOOL				m_bShiftButton;
	BOOL				m_bShiftLBDown;
	BOOL				m_bShiftRBDown;

	BOOL				m_bUseInterface;
	BOOL				m_bRenderSkip;
	float				m_fRenderSkipTime;

	bool				m_bStyle;
	D3DXVECTOR3			m_vDir;

    D3DXVECTOR3 m_vCharPos[ MAX_SELECTCHAR ];

	int					m_nStageBG;
	bool				m_bChangedStage;

	DWORD				m_dwCharIDForWorldBattle;


public:
	BOOL				m_bGameJoin;				// ������ ������ ���� �޼����� ������ �ʱ� ���� �÷���

private:

	TextureResource m_textureRes;
	CString				m_strTextureName;
	UIRECT				m_rcVertex;
	UIRECT				m_rcTexture;
	TEXTUREVERTEX		m_Vertices[4];

private:
	std::set<float>	m_setCheckGpuElapsedTime;
	float			m_fCheckGpuTimeADD;
	int				m_nCheckGpuCountADD;

private:

	void SetVertexPos ( TEXTUREVERTEX VertexArray[6], float LEFT, float TOP, float SIZEX, float SIZEY );
	void SetTexturePos ( TEXTUREVERTEX VertexArray[6], float LEFT, float TOP, float SIZEX, float SIZEY, float TEX_SIZEX, float TEX_SIZEY );
	HRESULT	CreateVB ( TEXTUREVERTEX VertexArray[6] );
	
//protected:
//	DxLandMan* GetCharNewLand ();

public:
	EMCHARINDEX GetCreateClass ()				{ return m_sCharNewInfo.m_emIndex; }
	WORD GetCreateSchool ()						{ return m_sCharNewInfo.m_wSchool; }

	const GLCHAR_NEWINFO& GetNewCharInfo ()		{ return m_sCharNewInfo; }
	const int GetChaSNum () const				{ return m_nChaSNum; }

	void  SetStyleStep( bool bStyle );
	void  RotateChar( bool bLeft, float fDegree = 1.0f );

protected:
	void ShiftChar( float fElapsedTime );
	BOOL ShiftRightChar();
	BOOL ShiftLeftChar();
	void ShiftCharInfo();

	void InitRenderSkip();

public:	
	//void ToCameraPos ( int n );

	void SelCharClass ( WORD nIndex );
	void SelCharSchool ( WORD nIndex );

	void SelCharFace ( WORD nIndex )			{ m_sCharNewInfo.m_wFace = nIndex; }
	void SelCharHair ( WORD nIndex )			{ m_sCharNewInfo.m_wHair = nIndex; }
	void SelCharHairColor ( WORD wColor )		{ m_sCharNewInfo.m_wHairColor = wColor; }

	WORD ShiftCharClass ( EMSHIFT emShift, WORD wClassMax );
	void ShiftCharSchool ( EMSHIFT emShift );
	void ShiftCharFace ( EMSHIFT emShift );
	void ShiftCharHair ( EMSHIFT emShift );

	void DelChar ( int nCharID );

	BOOL ShiftCharIndex(INT nIndex );

	void ShiftLBDown()							{ m_bShiftLBDown = TRUE; }
	void ShiftRBDown()							{ m_bShiftRBDown = TRUE; }
	BOOL IsShiftLeft()							{ return (m_nCharStart-MAX_SELECTCHAR)>=0;}
	BOOL IsShiftRight()							{ return (m_nCharStart+MAX_SELECTCHAR)<m_nChaSNum;}

	INT GetPageNum()							{ return (m_nCharStart/MAX_SELECTCHAR)+1; }
	
	void SelectCharacter( DWORD dwCharSelect );

public:
	SCHARINFO_LOBBY* GetCharInfo( INT nIndex );
	SCHARINFO_LOBBY* GetSelectCharInfo();
	DWORD IsCharSelect()						{ return (m_dwCharSelect!=MAX_SELECTCHAR); }
	BOOL IsStartReady ()						{ return m_nStartCharNum==m_nStartCharLoad; }
	DWORD GetCharSelectIndex() { return (m_dwCharSelect + m_nCharStart); }

	void SetChangedStage( bool bChanged )		{ m_bChangedStage = bChanged; }

public:
	void OnInitLogin ();

public:
	virtual HRESULT OneTimeSceneInit ( HWND hWnd, WORD wWidth, WORD wHeight, const char* szAppPath );
	virtual HRESULT InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT RestoreDeviceObjects ();
	virtual HRESULT InvalidateDeviceObjects ();
	virtual HRESULT DeleteDeviceObjects ();
	virtual HRESULT FinalCleanup();

protected:
	void UpdateSelSkinChar ( DxSkinChar* pSkinObj, EMCHARINDEX emIndex, WORD wFace, WORD wHair, WORD wHairColor );
	HRESULT DxEnvironmentRender( DxLandMan* pWLD );
	HRESULT MaterialSystemStretchRect( DxLandMan* pWLD );

	void RenderWorldCharEffect();

	void DxRenderDefferedOpaque( DxLandMan* pWLD );
	void DxRenderDefferedSoftAlpha( DxLandMan* pWLD );

public:
	virtual HRESULT FrameMove ( float fTime, float fElapsedTime, BOOL bNotRendering );
	virtual HRESULT Render ();

	virtual BOOL	GetUsedMaterialSystem();

public:
	virtual void MsgProcess(NET_MSG_GENERIC* nmg);

    void MsgLobbyCharInfoAC( NET_MSG_GENERIC* nmg );
	void MsgLobbyCharInfoACEnd( NET_MSG_GENERIC* nmg );

    virtual HRESULT ReSizeWindow ( WORD wWidth, WORD wHeight );
	void GetNewCharShanScreenRect(RECT& rc);
};

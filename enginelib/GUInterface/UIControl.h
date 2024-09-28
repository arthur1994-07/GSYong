#pragma	once

#include "../../SigmaCore/gassert.h"
#include "../../EngineLib/DxTools/TextureManager.h"

#include "UIDataType.h"
#include "UIControlDefine.h"
#include "UIControlMessage.h"
#include "UIDebugSet.h"

class EngineDeviceMan;

const int VERTEX_SIZE = 4;
const int INDEX_SIZE = 6;
const int TEXTURE_NAME_SIZE = 64;

struct INTERFACE_CFG
{
	int nControlType;
	CString strParentName;

	UIRECT rcControlPos;
	WORD wAlignFlag;
	D3DXVECTOR4 vBorder;

	float X_Size;
	float Y_Size;
	CString strTextureName;
	UIRECT rcTexturePos;

	INTERFACE_CFG()
		: nControlType( -1 )

		, wAlignFlag( 0 )
		, vBorder( 0.0f, 0.0f, 0.0f, 0.0f )

		, X_Size( 0.0f )
		, Y_Size( 0.0f )
	{
	}
};

class CUIControl
{
	// __UI_DEGUG__ 는 단순히 디버그용도만으로 쓰이질 않는걸 확인  의미 없으므로 제거하자 
//#ifdef	__UI_DEGUG__
private:
	std::string m_strControlName;

public:
	void SetControlNameEx( const TCHAR* strControlName )	{ m_strControlName = strControlName; }
	TSTRING GetControlNameEx() { return m_strControlName; }
//#endif	//	__UI_DEGUG__

public:
	enum
	{
		INVISIBLE_MODE	= 0,
		FADEIN_MODE		= 1,
		FADEOUT_MODE	= 2,
		VISIBLE_MODE	= 3
	};

	enum
	{
		DEFAULT_XSIZE = 1024,
		DEFAULT_YSIZE = 768,
	};

	enum
	{
		FLOW_BAR_RIGHT	= 0,
		FLOW_BAR_LEFT	= 1,
		FLOW_BAR_UP		= 2,
		FLOW_BAR_DOWN	= 3
	};

public:
    CUIControl(EngineDeviceMan* pEngineDevice);
    virtual ~CUIControl();

protected:
    EngineDeviceMan* m_pEngineDevice;

public:
    void SetUseRender( bool bUseRender )	{ m_bUseRender = bUseRender; }
	void SetUseRender( BOOL bUseRender )	{ m_bUseRender = bUseRender ? true : false; }
	void SetNonBoundaryCheck( const bool bEnable ) { m_bCheckNonBoundary = bEnable; }

    bool IsUseRender()						{ return m_bUseRender; }
    BOOL IsNoRender()						{ return !m_bVisible || !m_bUseRender; }

    EngineDeviceMan* GetEngineDevice() const { return m_pEngineDevice; }

public:
	void Create( UIGUID WndID,	const char* szControlKeyword, WORD wAlignFlag = UI_FLAG_DEFAULT )
	{
		CreateSub(NULL,szControlKeyword,wAlignFlag,WndID);
	}

	virtual void CreateSub( CUIControl* pParent, const char* szControlKeyword, WORD wAlignFlag = UI_FLAG_DEFAULT, UIGUID WndID = NO_ID );

	// Note : 아래 UI 생성 함수는 XML로 부터 정렬 플래그를 가져온다.
	//
	void CreateEx ( UIGUID WndID,	const char* szControlKeyword )
	{
		CreateSub(NULL,szControlKeyword,USHRT_MAX,WndID);
	}
	void CreateSubEx ( CUIControl* pParent, const char* szControlKeyword, UIGUID WndID = NO_ID )
	{
		CreateSub(pParent,szControlKeyword,USHRT_MAX,WndID);
	}

public:
	UIRECT AlignMainControl( const UIRECT& rcPos );
	virtual void AlignMainControlEX( 
		const UIRECT& rcLastParentPos, 
		const UIRECT& rcNewParentPos );
	virtual	void AlignSubControl( const UIRECT& rcParentOldPos, const UIRECT& rcParentNewPos );
	virtual	void AlignSubControlEX(
		const UIRECT& rcLastParentPos, 
		const UIRECT& rcNewParentPos);
    virtual void ReSizeControl( const UIRECT& rcParentOldPos, const UIRECT& rcParentNewPos );
    virtual void ResetControl( const UIRECT& rcParentPos, const char* szControlKeyword );

public:
	void SetTexturePos( const UIRECT& rcTexPos );
	void SetTexturePos( int wIndex, const D3DXVECTOR2& vPos );
	void SetTexture( LPDIRECT3DTEXTUREQ pTexture )	{ m_pTexture = pTexture; }
	LPDIRECT3DTEXTUREQ GetTexture() { return m_pTexture; }

	int	GetTextureSizeWidth()						{ return m_nTextureSizeWidth;}
	int	GetTextureSizeHeight()						{ return m_nTextureSizeHeight;}
	void SetTextureSize(int nWidth, int nHeight);
	void SetTextureSizeWidth(int nWidth)			{ m_nTextureSizeWidth = nWidth;}
	void SetTextureSizeHeight(int nHeight)			{ m_nTextureSizeHeight = nHeight;}

	const UIRECT& GetTexturePos() const			{ return m_rcTexurePos; }

	void SetFadeTime( float fFadeLimitTime )	{ m_fFadeLimitTime = fFadeLimitTime; }
	float GetFadeElapsedTime()					{ return m_fFadeElapsedTime; }

public:
	void	SetBeginAlpha ( DWORD dwAlpha )		{ m_dwBeginAlpha = dwAlpha; }

public:
	void SetGlobalPos( int wIndex, const D3DXVECTOR2& vPos ) { m_UIVertex[wIndex] = vPos; }
	virtual void SetAlignedGlobalPos( const UIRECT& rcPos );
	virtual void SetGlobalPos( const UIRECT& rcPos );
	virtual void SetGlobalPos( const D3DXVECTOR2& vPos );
		
	void SetLocalPos( const UIRECT& rcLocalPosNew ) { m_rcLocalPos = rcLocalPosNew;	}
    void SetLocalPos( const D3DXVECTOR2& vPos );
    void AddLocalPos( const D3DXVECTOR2& vPos );

	const UIRECT& GetGlobalPos() const		{ return m_rcGlobalPos; }
	const UIRECT& GetLocalPos () const		{ return m_rcLocalPos; }

    virtual void AddPosition ( const D3DXVECTOR2& vPos );

public:
    void SetBorder( const D3DXVECTOR4& vBorder );
    void SetBorder( const UIRECT& rcBorder ) { m_rcBorder = rcBorder; };
    const UIRECT& GetBorder() const { return m_rcBorder; }

public:
	void SetProtectSizePoint ();
	void SetProtectSizePoint ( const D3DXVECTOR2& vSIZE );
	const D3DXVECTOR2& GetProtectSize() const { return m_vProtectSize; }

private:
	void CheckProtectSize ();
	void CheckBoundary( bool bMouseAligned = false );

public:
	UIVERTEX* GetUIVertex(void) { return m_UIVertex; }
	const D3DCOLOR&	GetDiffuse() const		{ return m_UIVertex[0].GetDiffuse(); }
	virtual void SetDiffuse(D3DCOLOR _diffuse);

public:
	virtual void SetTransparentOption ( BOOL bTransparency );
	BOOL GetTransparentOption()				{ return m_bTransparency; }

public:
	void SetTextureName( const CString& strTextureName );
	const CString& GetTextureName() const	{ return m_strTextureName; }

public:
	void SetParent( CUIControl* pParent )	{ m_pParent = pParent; }
	CUIControl* GetParent()					{ return m_pParent; }
	CUIControl*	GetTopParent();

public:
	HRESULT	LoadTexture ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT UnLoadTexture ();

public:
	// 메세지 관련 함수
	DWORD GetMessageEx ()					{ return m_dwMsg; }
	void AddMessageEx ( DWORD dwMsg )		{ m_dwMsg |= dwMsg; }
	void SetMessageEx ( DWORD dwMsg )		{ m_dwMsg = dwMsg; }
	virtual void ResetMessageEx ()			{ m_dwMsg = 0; }

	// 마우스 메세지를 받을 필요없는 컨트롤에 SetNoMessage(true)를 세팅한다.
	virtual void SetNoMessage( BOOL bNoMessage)				{ m_bNoMSG = bNoMessage; }
	BOOL IsNoMessage()								{ return m_bNoMSG; }

public:
	virtual	void SetVisibleSingle ( BOOL bVisible );
	BOOL IsVisible() const { return m_bVisible; }

	virtual	BOOL SetFadeIn ();
	virtual	BOOL SetFadeOut();
	virtual	BOOL SetFade ( BOOL bFadeIn );

	BOOL	IsFading();	
	float	GetVisibleRate()		{ return m_fVisibleRate; }

	virtual void Reset();
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg ) {}
	virtual void ResizeWindow(const UIRECT& rcNew);
	virtual void ResizeWindow(const D3DXVECTOR2& vNew);
	
	void SetControlSize( int nWidth, int nHeight);
	BOOL GetBoundary()							{ return m_bCheckBound; }
	void SetBoundary(BOOL bBound)				{ m_bCheckBound = bBound; }

public:
	virtual HRESULT OneTimeSceneInit()											{ return S_OK; }
	virtual HRESULT InitDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT InvalidateDeviceObjects()									{ return S_OK; }
	virtual HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
	virtual HRESULT DeleteDeviceObjects();							
	virtual HRESULT Render( LPDIRECT3DDEVICEQ pd3dDevice );			
	virtual HRESULT FrameMove(LPDIRECT3DDEVICEQ pd3dDevice,float fElapsedTime)	{ return S_OK; }
    virtual HRESULT FinalCleanup()												{ return S_OK; }
    virtual HRESULT InfoFileLoad () { return S_OK; }
    virtual HRESULT InfoFileSave () { return S_OK; }

	virtual void ActiveUIMessage() { }
	virtual void InactiveUIMessage() { }

public:
	inline BOOL	MouseUpdate( int PosX, int PosY );

protected:
	void	FadeUpdate(float fElapsedTime);

private:
	void	SetVertexPos();
	//UIRECT	GetAlignPos ( UIRECT Pos );

public:
	virtual void SetDiffuseAlpha ( WORD wAlpha );
	void	     SetVertexPos( float z );

public:
	void SetAlignFlag( WORD wFlag )		{ m_wAlignFlag = wFlag; CHECK_ALIGN_FLAG( m_wAlignFlag ); }
	void AddAlignFlag( WORD wFlag )		{ m_wAlignFlag |= wFlag; CHECK_ALIGN_FLAG( m_wAlignFlag ); }
	void ResetAlignFlag()				{ m_wAlignFlag = UI_FLAG_DEFAULT;	}
	WORD GetAlignFlag()					{ return m_wAlignFlag; }

public:
	// 마우스 메세지를 받을 필요없는 컨트롤에 SetNoUpdate(true)를 세팅한다.
	void SetNoUpdate( BOOL bNoUpdate )	{ m_bNO_UPDATE = bNoUpdate; }
	BOOL IsNoUpdate()					{ return !m_bVisible || m_bNO_UPDATE; }

public:
	void SetWndID( UIGUID WndID )		{ m_WndID = WndID; }
	UIGUID GetWndID()					{ return m_WndID; }

	// 무조건 콘트롤을 받고 싶을 경우(맵)
	void SetAllMessage( BOOL bAllWaysMessage)		{ m_bALLWAYS_MSG = bAllWaysMessage; }
	BOOL IsAllMessage()								{ return m_bALLWAYS_MSG; }

	void SetResolutionBack( long lResolution )	{ m_lResolutionBack = lResolution; }
	long GetResolutionBack()						{ return m_lResolutionBack; }
	
    void ChangeFontSize( WORD& wX_RES, WORD& wY_RES, WORD& wX_RES_BACK, WORD& wY_RES_BACK );
    void ChangeFontSize( const DWORD dwHeight );
	void ChangeSizeWindow();


	virtual void SetChangeSize( BOOL bChange )	{ m_bChangeSize = bChange; }
	BOOL GetChangeSize()				{ return m_bChangeSize; }

	CD3DFontPar* GetFont() { return m_pFont; }
	virtual void SetFont ( CD3DFontPar* pFont );

	void SetFlowBar(const UIRECT& rcOriginPos, const UIRECT& rcOriginTexturePos );
	void ResetFlowBar();
	BOOL IsFlowBar() { return m_bFlowBar; }

	void SetFlowBarType( int nType ) { m_iFlowBarType = nType; } // nType = 차오르는 방향. default = 오른쪽방향. by luxes.

	void FlowBarUpdate( float fPercent ); // fPercent = 0.0 ~ 1.0. by luxes.

	virtual void Refresh() {}

protected:
	UIRECT	m_rcGlobalPos;	
	UIRECT	m_rcLocalPos;
    UIRECT	m_rcBorder;
	D3DXVECTOR2	m_vProtectSize;
	UIRECT	m_rcTexurePos;

	UIVERTEX	m_UIVertex[VERTEX_SIZE];

	LPDIRECT3DTEXTUREQ	m_pTexture;
    TextureResource m_textureRes;
	CString				m_strTextureName;
	int					m_nTextureSizeWidth;
	int					m_nTextureSizeHeight;

	float	m_fFadeLimitTime;
	float	m_fFadeElapsedTime;

	BOOL	m_bTransparency;	
	float	m_fVisibleRate;

	bool	m_bUseRender;
	bool	m_bCheckNonBoundary;
	BOOL	m_bCheckProtectSize;

	DWORD	m_dwMsg;		//	메시지
	BOOL	m_bNO_UPDATE;
	BOOL	m_bCheckBound;			// MEMO : 콘트롤이 갇혀있는지 아닌지
	BOOL	m_bALLWAYS_MSG;
	BOOL	m_bNoMSG;

private:
	UIGUID	m_WndID;		//	컨트롤 ID,Unique ID
	CUIControl*	m_pParent;
	WORD	m_wAlignFlag;
	BOOL	m_bVisible;
	int		m_VisibleState;
	DWORD	m_dwBeginAlpha;

protected:
	BOOL m_bExclusiveControl; // MEMO : 단독 컨트롤

	LONG m_lResolutionBack;
	BOOL m_bChangeSize;
	
	CD3DFontPar* m_pFont;
	FLOAT m_fDefaultFontSize;

	BOOL m_bFlowBar;	// 텍스쳐 위치를 변경하는 progressive bar 사용 여부. by luxes.
	int  m_iFlowBarType; // progressive bar 차오르는 방향. by luxes.
	UIRECT m_rcOriginPos; // 변경되기 전 ui rect. by luxes.
	UIRECT m_rcOriginTexturePos; // 변결됭지 전 texture rect. by luxes.

public:
	virtual void SetExclusiveControl()				{ m_bExclusiveControl = TRUE; }
	virtual void ResetExclusiveControl()	{ m_bExclusiveControl = FALSE; }
	BOOL IsExclusiveSelfControl()			{ return m_bExclusiveControl; }
	virtual BOOL IsExclusiveControl()		{ return m_bExclusiveControl; }

protected:
	BOOL m_bFocusControl; // MEMO : 포커스 컨트롤

public:
	virtual void SetFocusControl()			{ m_bFocusControl = TRUE; }
	virtual void ResetFocusControl()		{ m_bFocusControl = FALSE; }
	virtual BOOL IsFocusControl()			{ return m_bFocusControl; }

protected:
	INT m_nFoldID; // MEMO : 중복된 컨트롤의 그룹, 0이면 그룹이 아니다.

public:
	VOID SetFoldID( INT nID )				{ GASSERT( nID > 0 ); m_nFoldID = nID; }
	INT GetFoldID()							{ return m_nFoldID; }

public:
	/**
		특정 컨트롤의 포커싱,마우스이벤트 가능 유무를 설정 한다.
		이는 특정 컨트롤에 한해서만 모달처럼 작동 되게 할 때 사용 될 수 있다.
	 */
	void SetLock(bool bLock) { m_bLock = bLock; }
	bool IsLock() { return m_bLock; }

protected:
	bool m_bLock;
};

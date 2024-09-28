//	BasicScrollThumbFrame 클래스
//
//	최초 작성자 : 성기엽
//	이후 수정자 :
//	로그
//		[2003.11.26]
//			@ 최초 제작

#pragma	once

#include "UIGroup.h"

class	CBasicScrollThumb;

class CBasicScrollThumbFrame : public CUIGroup
{
public:
static const float	fZERO_PERCENT;
static const float	fHUNDRED_PERCENT;
static const int    nSCROLL_MOVE_LINE_DEFAULT;

private:
static const float	fMIN_THUMBSIZE;
static const int	nDEFAULT_VIEWPERPAGE;
static const int	nDEFAULT_MAXPOS;

private:
	enum	//	기본 윈도우 ID ( 외부로 공개되지 않음 )
	{
		ET_CONTROL_THUMB = NO_ID + 1,
	};

public:
	CBasicScrollThumbFrame (EngineDeviceMan* pEngineDevice);
	virtual	~CBasicScrollThumbFrame ();

public:
	void	MoveUp ();
	void	MoveDown ();
	
	void	MoveLeft ()							{ MoveUp(); }
	void	MoveRight ()						{ MoveDown(); }

public:	
	void	CreateThumb ( char* szThumb, char* szBodyValue, char* szUpValue, char* szDnValue, const bool& bVERTICAL );

public:
	void	SetState ( int nTotal, int nViewPerPage );
	void	GetState ( int& RnTotal, int& RnViewPerPage );

private:
	void	VALIDTHUMBMOVE ();

	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	virtual void	SetGlobalPos ( const UIRECT& rcPos );
	virtual void	SetGlobalPos ( const D3DXVECTOR2& vPos );
	virtual	void	AlignSubControl ( const UIRECT& rcParentOldPos, const UIRECT& rcParentNewPos );
	virtual HRESULT RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice );

public:
	float	GetPercent ( void )					{ return m_fPercent; }
	void	SetPercent ( float fPercent );

	int     GetPosition();
public:
	BOOL	IsMoreOnePage ( void )				{ return m_bMoreOnePage; }
	int		GetViewPerPage ( void )				{ return m_nViewPerPage; }
	int		GetTotal ( void )					{ return m_nMAXPOS; }

	const bool IsVERTICAL() const				{ return m_bVERTICAL; }

    void    SetScrollParent( CUIControl* pControl );

    void SetScrollMoveLine( int nScrollMoveLine );

private:
	int		m_nMAXPOS;

	float	m_fMINPERCENT;
	float	m_fMAXPERCENT;

	int		m_nViewPerPage;
	BOOL	m_bMoreOnePage;

private:
	float	m_fPercent;

    int     m_nNumScrollMoveLine; // 한 번에 스크롤할 줄의 수.

private:
	BOOL		m_bFirstGap;
	D3DXVECTOR2	m_vGap;

	int			m_PosX;
	int			m_PosY;

private:
    //  마우스 휠이 동작할 부모 컨트롤 지정
    bool            m_bScrollParent;
    CUIControl*     m_pScrollParent;

protected:
	bool		m_bVERTICAL;

private:
	CBasicScrollThumb*	m_pThumb;
};
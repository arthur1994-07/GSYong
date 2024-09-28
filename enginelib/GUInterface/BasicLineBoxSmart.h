#pragma	once

#include "./UIGroup.h"
#include "InterfaceCfgXml.h"

class CSwapImage;

// Note : 라인박스를 쉽게 만들어줌
//
class CBasicLineBoxSmart : public CUIGroup
{

public:
	struct UITEXSIZERECT
	{
		float POSX;
		float POSY;
		float SIZEX;
		float SIZEY;

		UITEXSIZERECT()
			: POSX  (0.0f)
			, POSY  (0.0f)
			, SIZEX (0.0f)
			, SIZEY (0.0f)
		{
		}

		UITEXSIZERECT( UIRECT& rcInTex, const float fTexSizeX, const float fTexSizeY )
		{
			Set ( rcInTex, fTexSizeX, fTexSizeY );
		}

		void Set ( UIRECT& rcInTex, const float fTexSizeX, const float fTexSizeY )
		{
			const float fMicroX = 0.25f/fTexSizeX;
			const float fMicroY = 0.25f/fTexSizeY;

			POSX  = (rcInTex.left-fMicroX) * fTexSizeX;
			POSY  = (rcInTex.top -fMicroY) * fTexSizeY;
			SIZEX = rcInTex.sizeX*fTexSizeX;
			SIZEY = rcInTex.sizeY*fTexSizeY;
		}

		void Get ( UIRECT& rcOutTex, const float fTexSizeX, const float fTexSizeY )
		{
			const float fMicroX = 0.25f/fTexSizeX;
			const float fMicroY = 0.25f/fTexSizeY;

			rcOutTex.left	= POSX/fTexSizeX+fMicroX;
			rcOutTex.top	= POSY/fTexSizeY+fMicroY;
			rcOutTex.sizeX  = SIZEX/fTexSizeX;
			rcOutTex.sizeY  = SIZEY/fTexSizeY;
			rcOutTex.right	= rcOutTex.left + rcOutTex.sizeX;
			rcOutTex.bottom = rcOutTex.top  + rcOutTex.sizeY;
		}
	};

public:
	CBasicLineBoxSmart (EngineDeviceMan* pEngineDevice);
	virtual	~CBasicLineBoxSmart ();

public:
	void    CreateSubControl( const char* szTextureInfoControl );   // 구형 UIXML
private:
    void    CreateControl();

public:
	void	SetBackgroundImage		( const char* szControlKeyword );
	void	SetBackgroundImageEx	( const char* szControlKeyword, const UINT32 nTexPosX, const UINT32 nTexPosY, const UINT32 nTexWidth, const UINT32 nTexHeight );
	void	SetBackgroundView		( const BOOL  bEnable );

public:
	virtual void    SetLocalPos(const UIRECT& Pos);
	virtual void	SetLocalPos( const D3DXVECTOR2& vPos );
	virtual void    SetGlobalPos(const UIRECT& Pos);
    virtual void    SetGlobalPos(const D3DXVECTOR2& vPos);
    void    SetTexturePos( const UIRECT& rcTexPos );
    void    SetTexturePos(int index, const D3DXVECTOR2& vPos);

private:
    void    Refresh();
	void	LineControlSet ( CUIControl* pControl, UITEXSIZERECT& InPos, UITEXSIZERECT& InLocalTexRect, UITEXSIZERECT& InGlobalTexRect, const float fTexSizeX, const float fTexSizeY );

private:
    UITextureList::SControl m_sControl;
	CSwapImage* m_pImage_Background;
    CUIControl* m_pLineBody;
    CUIControl* m_pLineTop;
    CUIControl* m_pLineRightTop;
    CUIControl* m_pLineRight;
    CUIControl* m_pLineRightBootm;
    CUIControl* m_pLineBottom;
    CUIControl* m_pLineLeftBottom;
    CUIControl* m_pLineLeft;
    CUIControl* m_pLineLeftTop;
};


//------------------------------------------------------------------------------------------------------
//	by luxes.
//------------------------------------------------------------------------------------------------------

#pragma	once

#include "../EngineLib/GUInterface/UIGroup.h"

class	CTipBox : public CUIGroup
{
public:
	enum
	{
		LEFT_TOP = 1,
		TOP,
		RIGHT_TOP,
		RIGHT,
		RIGHT_BOTTOM,
		BOTTOM,
		LEFT_BOTTOM,
		LEFT
	};

public:
	CTipBox (EngineDeviceMan* pEngineDevice);
	virtual	~CTipBox ();

public:
	void CreateTipBox( char* szBoxControl );
	void CreateMouseTipBox( char* szBoxControl );
	void CreateTipBoxNoBody( char* szBoxControl );
	void CreatePracticeModeBox( char* szBoxControl );
	void CreateNoLineBox( char* szBoxControl );
    void CreateQuestList( char* szBoxControl );
    void CreateQuestSelect( char* szBoxControl );

public:
	void	SetUseRenderLine ( int nLine, BOOL bUseRender );

protected:
	CUIControl*		CreateControl ( char* szControl, WORD wAlignFlag );

protected:
	void	ResizeControl ( char* szBoxControl );
	void	CreateLineImage ( char* szLeftTop, char* szTop, char* szRightTop, char* szRight,
		char* szRightBootm, char* szBottom, char* LeftBotoom, char* szLeft );
	void	CreateBodyImage ( char* szBody );

public:
	void	ResetAlignFlagLine ();
	void	SetAlignFlagLine ( int nLine, WORD wAlignFlag );

public:
	virtual void ResizeWindow(const UIRECT& rcNew) override;

public:
	void SetCalcResizeWindow( BOOL bCalc )	{ m_bCalcResizeWindow = bCalc; }

protected:
	CUIControl*		m_pLeftTop;
	CUIControl*		m_pTop;
	CUIControl*		m_pRightTop;
	CUIControl*		m_pRight;
	CUIControl*		m_pRightBottom;
	CUIControl*		m_pBottom;
	CUIControl*		m_pLeftBottom;
	CUIControl*		m_pLeft;
	CUIControl*		m_pBody;

	BOOL			m_bCalcResizeWindow;
};
#pragma once

#include "../Util/UIGroupHelper.h"

class CInnerInterface;
class EngineDeviceMan;

class CBasicTextBox;

class CMoveableTextBox : public CUIGroupHelper
{
public:
	enum EMOVEABLE_TEXT_TYPE
	{
		MOVE_LEFT_TYPE,
		MOVE_RIGHT_TYPE
	};

public:
	CMoveableTextBox( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~CMoveableTextBox();

public:
	virtual void CreateSub( CUIControl* pParent, const char* szControlKeyword, WORD wAlignFlag = UI_FLAG_DEFAULT, UIGUID WndID = NO_ID );
	virtual void CreateSubControl( const char* szControlKeyword, CD3DFontPar* pFont, int nAlign );
	virtual void SetVisibleSingle( BOOL bVisible );

protected:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:
	// 텍스트 세팅 ( 세팅된 텍스트는 다음 애니메이션에 갱신된다 );
	void SetText( const std::string& text );
	void SetTextColor( const D3DCOLOR& textColor );
	void SetTextSpeed( float textSpeed );
	void SetMoveableType( EMOVEABLE_TEXT_TYPE type );

	// 텍스트 애니메이션을 초기화하고 텍스트를 바로 세팅한다;
	void SetTextWithResetAnimation( const std::string& text );

	// 텍스트 애니메이션 초기화;
	// 바로 적용하고 싶을 때 애니메이션을 초기화 해준다;
	void ResetTextAnimation();

	// 애니메이션 처리;
	void StartAnimation();
	void StopAnimation();

private:
	void ChangeText();				// 변경될 텍스트로 변경;
	void UpdateText();				// 텍스트 길이 갱신;
	void UpdateTextString();		// 텍스트 스트링 이동;

private:
	CInnerInterface* m_pInterface;
	CBasicTextBox* m_pDescText;

	std::string m_originString;				// 원본 텍스트;
	std::string m_strDesc;					// 변경 텍스트;
	std::string m_changeString;				// 변경될 텍스트 ( 다음 애니메이션에 갱신됨 );
	D3DCOLOR m_textColor;					// 텍스트 색상;

	bool m_bAniDesc;						// 애니메이션;
	bool m_bOverTextLength;					// 텍스트 길이가 긴지 여부;
	unsigned int m_nAniDescCur;				// 현재 위치;
	unsigned int m_nAniDescMax;				// 최대 위치;
	unsigned int m_nStrCur;					// 현재 길이;
	unsigned int m_nStrMax;					// 최대 길이;
	float m_fAniTime;						// 시간;

	float m_fDESC_ANIMATION_SPEED_DELAY;	// 텍스트 스피드;

	EMOVEABLE_TEXT_TYPE m_moveableType;		// 이동 타입;
};
#pragma once

#include <queue>

#ifdef DrawText
#undef DrawText
#endif

typedef std::vector< Ptr< GFx::DrawText > >		VEC_GFX_TEXT;
typedef VEC_GFX_TEXT::iterator					VEC_GFX_TEXT_ITER;
typedef std::queue< Ptr< GFx::DrawText > >		QUE_DISABLE_TEXT;

class GfxDrawText
{
public :
	GfxDrawText( GFx::DrawTextManager* pDTM );
	~GfxDrawText();

private :
	GFx::DrawTextManager*				m_pDrawTextMan;		// 텍스트 매니저
	GFx::DrawTextManager::TextParams	m_TextParam;		// 기본설정 텍스트 파라미터
	GFx::DrawText::Filter				m_TextFilter;		// 기본설정 텍스트 필터 

	VEC_GFX_TEXT			m_vecText;			// 생성된 텍스트 보관용
	QUE_DISABLE_TEXT		m_queDisableText;	// 사용안하는 텍스트 보관 큐

public :
	// 텍스트 추가
	GFx::DrawText* AddText();
	// 텍스트 박스 사이즈 계산
	void CalcSize( const char* szText, float& fSizeX, float& fSizeY, bool bMultiline, float fFontSize = 12.0f );

public :
	// 텍스트 프로퍼티 설정
	void SetProperty( GFx::DrawText* pText, const char* szText, float fPosX, float fPosY, float fSizeX, float fSizeY,
		DWORD dwColor = 0xFFFFFFFF, DWORD dwBgColor = 0x00000000, bool bMultiLine = false,
		float fFontSize = 12.0f, bool bUnderLine = false ); 
	// 배경색 설정
	void SetBgColor( GFx::DrawText* pText, DWORD dwColor );
	// 텍스트 색 설정
	void SetTextColor( GFx::DrawText* pText, DWORD dwColor, int nStart = 0, int nEnd = -1 );
	// 폰트 사이즈 설정
	void SetFontSize( GFx::DrawText* pText, float fSize, int nStart = 0, int nEnd = -1 );
	// 멀티라인 여부 설정
	void SetMultiLine( GFx::DrawText* pText, bool bMultiLine );
	// 텍스트 설정
	void SetText( GFx::DrawText* pText, const char* szText );
	// visible 설정
	void SetVisible( GFx::DrawText* pText, bool bVisible );
	// 언더라인 여부 설정
	void SetUnderLine( GFx::DrawText* pText, bool bUnderLine, int nStart = 0, int nEnd = -1 );
	// 위치 설정
	void SetPosition( GFx::DrawText* pText, float fPosX, float fPosY, float fSizeX, float fSizeY );

	// 텍스트 박스 사이즈 (성능이 좋지 않다. 되도록이면 안쓰도록 한다.)
	void GetSize( GFx::DrawText* pText, float& fSizeX, float& fSizeY );
	// 텍스트
	const char* GetText( GFx::DrawText* pText );
};

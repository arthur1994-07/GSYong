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
	GFx::DrawTextManager*				m_pDrawTextMan;		// �ؽ�Ʈ �Ŵ���
	GFx::DrawTextManager::TextParams	m_TextParam;		// �⺻���� �ؽ�Ʈ �Ķ����
	GFx::DrawText::Filter				m_TextFilter;		// �⺻���� �ؽ�Ʈ ���� 

	VEC_GFX_TEXT			m_vecText;			// ������ �ؽ�Ʈ ������
	QUE_DISABLE_TEXT		m_queDisableText;	// �����ϴ� �ؽ�Ʈ ���� ť

public :
	// �ؽ�Ʈ �߰�
	GFx::DrawText* AddText();
	// �ؽ�Ʈ �ڽ� ������ ���
	void CalcSize( const char* szText, float& fSizeX, float& fSizeY, bool bMultiline, float fFontSize = 12.0f );

public :
	// �ؽ�Ʈ ������Ƽ ����
	void SetProperty( GFx::DrawText* pText, const char* szText, float fPosX, float fPosY, float fSizeX, float fSizeY,
		DWORD dwColor = 0xFFFFFFFF, DWORD dwBgColor = 0x00000000, bool bMultiLine = false,
		float fFontSize = 12.0f, bool bUnderLine = false ); 
	// ���� ����
	void SetBgColor( GFx::DrawText* pText, DWORD dwColor );
	// �ؽ�Ʈ �� ����
	void SetTextColor( GFx::DrawText* pText, DWORD dwColor, int nStart = 0, int nEnd = -1 );
	// ��Ʈ ������ ����
	void SetFontSize( GFx::DrawText* pText, float fSize, int nStart = 0, int nEnd = -1 );
	// ��Ƽ���� ���� ����
	void SetMultiLine( GFx::DrawText* pText, bool bMultiLine );
	// �ؽ�Ʈ ����
	void SetText( GFx::DrawText* pText, const char* szText );
	// visible ����
	void SetVisible( GFx::DrawText* pText, bool bVisible );
	// ������� ���� ����
	void SetUnderLine( GFx::DrawText* pText, bool bUnderLine, int nStart = 0, int nEnd = -1 );
	// ��ġ ����
	void SetPosition( GFx::DrawText* pText, float fPosX, float fPosY, float fSizeX, float fSizeY );

	// �ؽ�Ʈ �ڽ� ������ (������ ���� �ʴ�. �ǵ����̸� �Ⱦ����� �Ѵ�.)
	void GetSize( GFx::DrawText* pText, float& fSizeX, float& fSizeY );
	// �ؽ�Ʈ
	const char* GetText( GFx::DrawText* pText );
};

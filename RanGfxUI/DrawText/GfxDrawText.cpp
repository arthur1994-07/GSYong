#include "StdAfx.h"
#include "GfxDrawText.h"

#include "../../SigmaCore/String/StringUtil.h"

GfxDrawText::GfxDrawText( GFx::DrawTextManager* pDTM )
:	m_pDrawTextMan( pDTM )
{
	m_vecText.reserve( 100 );

	m_TextParam = m_pDrawTextMan->GetDefaultTextParams();
	m_TextParam.FontName = "$NormalFont";
	m_TextParam.TextColor = Color( 0xFFFFFFFF );
	m_TextParam.Multiline = false;
	m_TextParam.WordWrap = false;
	m_TextParam.FontSize = 12.0f;
	m_pDrawTextMan->SetDefaultTextParams( m_TextParam );

	m_TextFilter.SetFilterType( GFx::DrawText::Filter_Glow );
	m_TextFilter.Glow.BlurX = 1;
	m_TextFilter.Glow.BlurY = 1;
	m_TextFilter.Glow.Strength = 1000;
	m_TextFilter.Glow.Color = Color( 0, 0, 0, 255 ).ToColor32();
}

GfxDrawText::~GfxDrawText()
{
	m_vecText.clear();
}

GFx::DrawText* GfxDrawText::AddText()
{
	if( false == m_queDisableText.empty() )
	{
		GFx::DrawText* pText = m_queDisableText.front();
		m_queDisableText.pop();

		return pText;
	}

	std::string strText = " ";

	Ptr< GFx::DrawText > pNewText = *m_pDrawTextMan->CreateText( 
		strText.c_str(), RectF( 0.0f, 0.0f, 1.0f, 1.0f ), &m_TextParam );
	pNewText->SetAAMode( GFx::DrawText::AA_Readability );
	pNewText->SetAlignment( GFx::DrawText::Align_Center );
	pNewText->SetVAlignment( GFx::DrawText::VAlign_Center );
	pNewText->SetFilters( &m_TextFilter );

	m_vecText.push_back( pNewText );

	return pNewText;
}

void GfxDrawText::CalcSize( const char* szText, float& fSizeX, float& fSizeY, bool bMultiline, float fFontSize )
{
	std::string strText = sc::string::ansi_to_utf8( std::string( szText ) );

	GFx::DrawTextManager::TextParams params = m_pDrawTextMan->GetDefaultTextParams();
	params.Multiline = bMultiline;
	params.FontSize = fFontSize;

	SizeF sizeText = m_pDrawTextMan->GetTextExtent( strText.c_str(), 0, &params );
	fSizeX = sizeText.Width;
	fSizeY = sizeText.Height;
}

void GfxDrawText::SetProperty( GFx::DrawText* pText, const char* szText, float fPosX, float fPosY, float fSizeX, float fSizeY,
				 DWORD dwColor, DWORD dwBgColor, bool bMultiLine, float fFontSize, bool bUnderLine )
{
	std::string strText = sc::string::ansi_to_utf8( std::string( szText ) );

	pText->SetText( strText.c_str() );
	pText->SetRect( RectF( fPosX, fPosY, fPosX + fSizeX, fPosY + fSizeY ) );
	pText->SetColor( Color( dwColor ) );
	pText->SetBackgroundColor( Color( dwBgColor ) );
	pText->SetMultiline( bMultiLine );
	pText->SetFontSize( fFontSize );
	pText->SetUnderline( bUnderLine );

	pText->SetVisible( true );
}

void GfxDrawText::SetBgColor( GFx::DrawText* pText, DWORD dwColor )
{
	pText->SetBackgroundColor( Color( dwColor ) );
}

void GfxDrawText::SetTextColor( GFx::DrawText* pText, DWORD dwColor, int nStart, int nEnd )
{
	pText->SetColor( Color( dwColor ), nStart, nEnd );
}

void GfxDrawText::SetFontSize( GFx::DrawText* pText, float fSize, int nStart, int nEnd )
{
	pText->SetFontSize( fSize, nStart, nEnd );
}

void GfxDrawText::SetMultiLine( GFx::DrawText* pText, bool bMultiLine )
{
	pText->SetMultiline( bMultiLine );
}

void GfxDrawText::SetText( GFx::DrawText* pText, const char* szText )
{
	std::string strText = sc::string::ansi_to_utf8( std::string( szText ) );
	pText->SetText( strText.c_str() );
}

void GfxDrawText::SetVisible( GFx::DrawText* pText, bool bVisible )
{
	pText->SetVisible( bVisible );

	if( false == bVisible )
		m_queDisableText.push( pText );
}

void GfxDrawText::SetUnderLine( GFx::DrawText* pText, bool bUnderLine, int nStart, int nEnd )
{
	pText->SetUnderline( bUnderLine, nStart, nEnd );
}

void GfxDrawText::SetPosition( GFx::DrawText* pText, float fPosX, float fPosY, float fSizeX, float fSizeY )
{
	pText->SetRect( RectF( fPosX, fPosY, fPosX + fSizeX, fPosY + fSizeY ) );
}

void GfxDrawText::GetSize( GFx::DrawText* pText, float& fSizeX, float& fSizeY )
{
	SizeF sizeText = m_pDrawTextMan->GetTextExtent( pText->GetText(), 0 );
	fSizeX = sizeText.Width;
	fSizeY = sizeText.Height;
}

const char* GfxDrawText::GetText( GFx::DrawText* pText )
{
	return pText->GetText().ToCStr();
}
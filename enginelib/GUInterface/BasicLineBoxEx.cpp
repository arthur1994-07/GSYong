#include "pch.h"
#include "BasicLineBoxEx.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CBasicLineBoxEx::CBasicLineBoxEx(EngineDeviceMan* pEngineDevice)
    : CBasicLineBox(pEngineDevice)
{
}

CBasicLineBoxEx::~CBasicLineBoxEx ()
{
}

void CBasicLineBoxEx::CreateBaseBoxStorageCard ( char* szBoxControl )
{
	CreateBodyImage ( "BASIC_LINE_BOX_EX_BODY_STORAGE_CARD" );
	CreateLineImage ( "BASIC_LINE_BOX_EX_LEFT_STORAGE_CARD", "BASIC_LINE_BOX_EX_RIGHT_STORAGE_CARD", "BASIC_LINE_BOX_EX_TOP_STORAGE_CARD", "BASIC_LINE_BOX_EX_BOTTOM_STORAGE_CARD" );
	CreateLineImageEx ( "BASIC_LINE_BOX_EX_LEFTTOP_STORAGE_CARD", "BASIC_LINE_BOX_EX_LEFTBOTTOM_STORAGE_CARD", "BASIC_LINE_BOX_EX_RIGHTTOP_STORAGE_CARD", "BASIC_LINE_BOX_EX_RIGHTBOTTOM_STORAGE_CARD" );
	ResizeControl ( szBoxControl );	
}

void CBasicLineBoxEx::CreateBaseBoxESCMenu ( char* szBoxControl )
{
	CreateBodyImage ( "BASIC_LINE_BOX_EX_BODY_ESC" );
	CreateLineImage ( "BASIC_LINE_BOX_EX_LEFT_ESC", "BASIC_LINE_BOX_EX_RIGHT_ESC", "BASIC_LINE_BOX_EX_TOP_ESC", "BASIC_LINE_BOX_EX_BOTTOM_ESC" );
	CreateLineImageEx ( "BASIC_LINE_BOX_EX_LEFTTOP_ESC", "BASIC_LINE_BOX_EX_LEFTBOTTOM_ESC", "BASIC_LINE_BOX_EX_RIGHTTOP_ESC", "BASIC_LINE_BOX_EX_RIGHTBOTTOM_ESC" );
	ResizeControl ( szBoxControl );	
}

void CBasicLineBoxEx::CreateBaseCostumeTitleBox ( char* szBoxControl )
{	
	CreateLineImage ( "BASIC_LINE_BOX_EX_LEFT_ESC", "BASIC_LINE_BOX_EX_RIGHT_ESC", "BASIC_LINE_BOX_EX_TOP_ESC", "BASIC_LINE_BOX_EX_BOTTOM_ESC" );
	CreateLineImageEx ( "BASIC_LINE_BOX_EX_LEFTTOP_ESC", "BASIC_LINE_BOX_EX_LEFTBOTTOM_ESC", "BASIC_LINE_BOX_EX_RIGHTTOP_ESC", "BASIC_LINE_BOX_EX_RIGHTBOTTOM_ESC" );

	ResizeControl ( szBoxControl );	
}

void CBasicLineBoxEx::CreateBaseCostumeTitleBoxII( char* szBoxControl, char* szBody, char* szLineLeft, char* szLineRight )
{
	m_pLeft   =  CreateControl ( szLineLeft , UI_FLAG_YSIZE );
	m_pRight  = CreateControl ( szLineRight, UI_FLAG_RIGHT | UI_FLAG_YSIZE );

	m_pTop    = NULL;
	m_pBottom = NULL;

	m_pLeftTop     = NULL;
	m_pLeftBottom  = NULL;
	m_pRIghtTop    = NULL;
	m_pRightBottom = NULL;

	//CreateLineImage ( "BASIC_LINE_BOX_EX_LEFT1_ESC", "BASIC_LINE_BOX_EX_RIGHT1_ESC", "BASIC_LINE_BOX_EX_TOP1_ESC", "BASIC_LINE_BOX_EX_BOTTOM1_ESC" );
	//CreateLineImageEx ( "BASIC_LINE_BOX_EX_LEFTTOP1_ESC", "BASIC_LINE_BOX_EX_LEFTBOTTOM1_ESC", "BASIC_LINE_BOX_EX_RIGHTTOP1_ESC", "BASIC_LINE_BOX_EX_RIGHTBOTTOM1_ESC" );

	ResizeControl ( szBoxControl );	
}

void CBasicLineBoxEx::CreateBaseBoxVarTextBox ( char* szBoxControl )
{
	CreateBodyImage ( "BASIC_LINE_BOX_EX_BODY_VARTEXT" );
	CreateLineImage ( "BASIC_LINE_BOX_EX_LEFT_VARTEXT", "BASIC_LINE_BOX_EX_RIGHT_VARTEXT", "BASIC_LINE_BOX_EX_TOP_VARTEXT", "BASIC_LINE_BOX_EX_BOTTOM_VARTEXT" );
	CreateLineImageEx ( "BASIC_LINE_BOX_EX_LEFTTOP_VARTEXT", "BASIC_LINE_BOX_EX_LEFTBOTTOM_VARTEXT", "BASIC_LINE_BOX_EX_RIGHTTOP_VARTEXT", "BASIC_LINE_BOX_EX_RIGHTBOTTOM_VARTEXT" );
	ResizeControl ( szBoxControl );	
}

void CBasicLineBoxEx::CreateBaseBoxTargetInfo ( char* szBoxControl )
{
	CreateBodyImage ( "BASIC_LINE_BOX_EX_BODY_TARGETINFO" );
	CreateLineImage ( "BASIC_LINE_BOX_EX_LEFT_TARGETINFO", "BASIC_LINE_BOX_EX_RIGHT_TARGETINFO", "BASIC_LINE_BOX_EX_TOP_TARGETINFO", "BASIC_LINE_BOX_EX_BOTTOM_TARGETINFO" );
	CreateLineImageEx ( "BASIC_LINE_BOX_EX_LEFTTOP_TARGETINFO", "BASIC_LINE_BOX_EX_LEFTBOTTOM_TARGETINFO", "BASIC_LINE_BOX_EX_RIGHTTOP_TARGETINFO", "BASIC_LINE_BOX_EX_RIGHTBOTTOM_TARGETINFO" );
	ResizeControl ( szBoxControl );	
}

void	CBasicLineBoxEx::CreateBaseBoxSimpleHP ( char* szBoxControl )
{
	CreateBodyImage ( "BASIC_LINE_BOX_EX_BODY_SIMPLEHP" );
	CreateLineImage ( "BASIC_LINE_BOX_EX_LEFT_SIMPLEHP", "BASIC_LINE_BOX_EX_RIGHT_SIMPLEHP", "BASIC_LINE_BOX_EX_TOP_SIMPLEHP", "BASIC_LINE_BOX_EX_BOTTOM_SIMPLEHP" );
	CreateLineImageEx ( "BASIC_LINE_BOX_EX_LEFTTOP_SIMPLEHP", "BASIC_LINE_BOX_EX_LEFTBOTTOM_SIMPLEHP", "BASIC_LINE_BOX_EX_RIGHTTOP_SIMPLEHP", "BASIC_LINE_BOX_EX_RIGHTBOTTOM_SIMPLEHP" );
	ResizeControl ( szBoxControl );	
}

void CBasicLineBoxEx::CreateBaseBoxName ( char* szBoxControl )
{
	CreateBodyImage ( "BASIC_LINE_BOX_EX_BODY_NAME" );
	CreateLineImage ( "BASIC_LINE_BOX_EX_LEFT_NAME", "BASIC_LINE_BOX_EX_RIGHT_NAME", "BASIC_LINE_BOX_EX_TOP_NAME", "BASIC_LINE_BOX_EX_BOTTOM_NAME" );
	CreateLineImageEx ( "BASIC_LINE_BOX_EX_LEFTTOP_NAME", "BASIC_LINE_BOX_EX_LEFTBOTTOM_NAME", "BASIC_LINE_BOX_EX_RIGHTTOP_NAME", "BASIC_LINE_BOX_EX_RIGHTBOTTOM_NAME" );
	ResizeControl ( szBoxControl );	
}

void CBasicLineBoxEx::CreateBaseBoxNameCD ( char* szBoxControl )
{
	CreateBodyImage ( "BASIC_LINE_BOX_EX_BODY_NAME_CD" );
	CreateLineImage ( "BASIC_LINE_BOX_EX_LEFT_NAME", "BASIC_LINE_BOX_EX_RIGHT_NAME", "BASIC_LINE_BOX_EX_TOP_NAME", "BASIC_LINE_BOX_EX_BOTTOM_NAME" );
	CreateLineImageEx ( "BASIC_LINE_BOX_EX_LEFTTOP_NAME", "BASIC_LINE_BOX_EX_LEFTBOTTOM_NAME", "BASIC_LINE_BOX_EX_RIGHTTOP_NAME", "BASIC_LINE_BOX_EX_RIGHTBOTTOM_NAME" );
	ResizeControl ( szBoxControl );	
}

void CBasicLineBoxEx::CreateBaseBoxChat ( char* szBoxControl )
{
	CreateBodyImage ( "BASIC_LINE_BOX_EX_BODY_CHAT" );
	CreateLineImage ( "BASIC_LINE_BOX_EX_LEFT_CHAT", "BASIC_LINE_BOX_EX_RIGHT_CHAT", "BASIC_LINE_BOX_EX_TOP_CHAT", "BASIC_LINE_BOX_EX_BOTTOM_CHAT" );
	CreateLineImageEx ( "BASIC_LINE_BOX_EX_LEFTTOP_CHAT", "BASIC_LINE_BOX_EX_LEFTBOTTOM_CHAT", "BASIC_LINE_BOX_EX_RIGHTTOP_CHAT", "BASIC_LINE_BOX_EX_RIGHTBOTTOM_CHAT" );
	ResizeControl ( szBoxControl );	
}

void CBasicLineBoxEx::CreateLineImageEx ( char* szLeftTop, char* szLeftBottom, char* szRightTop, char* szRIghtBottom )
{
	m_pLeftTop = CreateControl ( szLeftTop, UI_FLAG_DEFAULT );
	m_pLeftBottom = CreateControl ( szLeftBottom, UI_FLAG_BOTTOM );
	m_pRIghtTop = CreateControl ( szRightTop, UI_FLAG_RIGHT );
	m_pRightBottom = CreateControl ( szRIghtBottom, UI_FLAG_BOTTOM | UI_FLAG_RIGHT );
}
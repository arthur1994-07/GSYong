#include "StdAfx.h"
#include "UIEditModal.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CUIEditModal::CUIEditModal(EngineDeviceMan* pEngineDevice, CInterfaceAdmin* pInterfaceAdmin)
    : CUIEditGroup(pEngineDevice, pInterfaceAdmin)
    , m_pLeft( NULL )
    , m_pRight( NULL )
    , m_pTop( NULL )
    , m_pBottom( NULL )
    , m_pLeftTop( NULL )
    , m_pRightTop( NULL )
    , m_pLeftBottom( NULL )
    , m_pRightBottom( NULL )
    , m_pCenter( NULL )
{
}

CUIEditModal::~CUIEditModal ()
{
}

CUIControl*	CUIEditModal::CreateControl ( TCHAR* szControl, WORD wAlignFlag )
{
	CUIControl* pControl = new CUIControl(m_pEngineDevice);
	pControl->CreateSub ( this, szControl);
	RegisterControl ( pControl );
	return pControl;
}

void CUIEditModal::CreateWindowControl (const TCHAR* szControl)
{
	m_pCenter = CreateControl ( _T("EXPAND_DIALOG_BODY"), UI_FLAG_LEFT |UI_FLAG_YSIZE );
	m_pLeft = CreateControl ( _T("EXPAND_DIALOG_LEFT"), UI_FLAG_LEFT |UI_FLAG_YSIZE );
	m_pRight = CreateControl ( _T("EXPAND_DIALOG_RIGHT"), UI_FLAG_RIGHT | UI_FLAG_YSIZE );
	m_pTop = CreateControl ( _T("EXPAND_DIALOG_TOP"), UI_FLAG_TOP | UI_FLAG_XSIZE );
	m_pBottom = CreateControl ( _T("EXPAND_DIALOG_BOTTOM"), UI_FLAG_BOTTOM | UI_FLAG_XSIZE );
	m_pLeftTop = CreateControl ( _T("EXPAND_DIALOG_LEFT_TOP"), UI_FLAG_TOP | UI_FLAG_LEFT );
	m_pRightTop = CreateControl ( _T("EXPAND_DIALOG_RIGHT_TOP"), UI_FLAG_TOP | UI_FLAG_RIGHT ); 
	m_pLeftBottom = CreateControl ( _T("EXPAND_DIALOG_LEFT_BOTTOM"), UI_FLAG_BOTTOM | UI_FLAG_LEFT );
	m_pRightBottom = CreateControl ( _T("EXPAND_DIALOG_RIGHT_BOTTOM"), UI_FLAG_BOTTOM | UI_FLAG_RIGHT );

	CUIEditGroup::CreateWindowControl(szControl);
	ResizeControl( szControl );
}

void CUIEditModal::ResizeControl(const TCHAR* szControl)
{	
	CUIEditGroup::ResizeControl( szControl );

	CUIControl TempControl(m_pEngineDevice);
	TempControl.Create ( 1, szControl );
	UIRECT rcParentNewPos = TempControl.GetLocalPos();

	ResizeWindow(rcParentNewPos);
}

void CUIEditModal::ResizeWindow(const UIRECT& rcNew)
{
	int nMinSizeX = m_pLeftTop->GetGlobalPos().sizeX + m_pRightBottom->GetGlobalPos().sizeX;
	int nMinSizeY = m_pLeftTop->GetGlobalPos().sizeY + m_pRightBottom->GetGlobalPos().sizeY;

	//rcNew.sizeX = max( nMinSizeX, rcNew.sizeX );
	//rcNew.sizeY = max( nMinSizeY, rcNew.sizeY );

	CUIEditGroup::ResizeWindow(rcNew);


	UIRECT rcParentNewPos;

	rcParentNewPos.sizeX	= rcNew.sizeX;
	rcParentNewPos.sizeY	= rcNew.sizeY;
	rcParentNewPos.left		= 0;
	rcParentNewPos.top		= 0;
	rcParentNewPos.right	= rcParentNewPos.left + rcParentNewPos.sizeX;
	rcParentNewPos.bottom	= rcParentNewPos.top + rcParentNewPos.sizeY;

	{
		UIRECT rcSetPos;

		rcSetPos.sizeX		= m_pLeft->GetLocalPos().sizeX;
		rcSetPos.sizeY		= max( 0, rcParentNewPos.sizeY - (m_pLeftTop->GetLocalPos().sizeY + m_pLeftBottom->GetLocalPos().sizeY) );

		rcSetPos.left		= rcParentNewPos.left;
		rcSetPos.top		= rcParentNewPos.top + m_pLeftTop->GetLocalPos().sizeY;
		rcSetPos.right		= rcSetPos.left + rcSetPos.sizeX;
		rcSetPos.bottom		= rcSetPos.top + rcSetPos.sizeY;

		m_pLeft->SetLocalPos(rcSetPos);
	}

	{
		UIRECT rcSetPos;

		rcSetPos.sizeX		= m_pRight->GetLocalPos().sizeX;
		rcSetPos.sizeY		= max( 0,rcParentNewPos.sizeY - (m_pRightTop->GetLocalPos().sizeY + m_pRightBottom->GetLocalPos().sizeY) );

		rcSetPos.left		= max( 0,rcParentNewPos.right - rcSetPos.sizeX);
		rcSetPos.top		= rcParentNewPos.top + m_pRightTop->GetLocalPos().sizeY;
		rcSetPos.right		= rcSetPos.left + rcSetPos.sizeX;
		rcSetPos.bottom		= rcSetPos.top + rcSetPos.sizeY;

		m_pRight->SetLocalPos(rcSetPos);
	}

	{
		UIRECT rcSetPos;

		rcSetPos.sizeX		= max( 0,rcParentNewPos.sizeX - (m_pLeftTop->GetLocalPos().sizeX + m_pRightTop->GetLocalPos().sizeX) );
		rcSetPos.sizeY		= m_pTop->GetLocalPos().sizeY;

		rcSetPos.left		= rcParentNewPos.left + m_pLeftTop->GetLocalPos().sizeX;
		rcSetPos.top		= rcParentNewPos.top;
		rcSetPos.right		= rcSetPos.left + rcSetPos.sizeX;
		rcSetPos.bottom		= rcSetPos.top + rcSetPos.sizeY;

		m_pTop->SetLocalPos(rcSetPos);
	}

	{
		UIRECT rcSetPos;

		rcSetPos.sizeX		= max( 0, rcParentNewPos.sizeX - (m_pLeftBottom->GetLocalPos().sizeX + m_pRightBottom->GetLocalPos().sizeX) );
		rcSetPos.sizeY		= m_pBottom->GetLocalPos().sizeY;

		rcSetPos.left		= rcParentNewPos.left + m_pLeftBottom->GetLocalPos().sizeX;
		rcSetPos.top		= max( 0,rcParentNewPos.bottom - rcSetPos.sizeY);
		rcSetPos.right		= rcSetPos.left + rcSetPos.sizeX;
		rcSetPos.bottom		= rcSetPos.top + rcSetPos.sizeY;

		m_pBottom->SetLocalPos(rcSetPos);
	}

	{
		UIRECT rcSetPos;

		rcSetPos.sizeX		= m_pLeftTop->GetLocalPos().sizeX;
		rcSetPos.sizeY		= m_pLeftTop->GetLocalPos().sizeY;

		rcSetPos.left		= rcParentNewPos.left;
		rcSetPos.top		= rcParentNewPos.top;
		rcSetPos.right		= rcSetPos.left + rcSetPos.sizeX;
		rcSetPos.bottom		= rcSetPos.top + rcSetPos.sizeY;

		m_pLeftTop->SetLocalPos(rcSetPos);
	}

	{
		UIRECT rcSetPos;

		rcSetPos.sizeX		= m_pRightTop->GetLocalPos().sizeX;
		rcSetPos.sizeY		= m_pRightTop->GetLocalPos().sizeY;

		rcSetPos.left		= max( 0,rcParentNewPos.right - rcSetPos.sizeX);
		rcSetPos.top		= rcParentNewPos.top;
		rcSetPos.right		= rcSetPos.left + rcSetPos.sizeX;
		rcSetPos.bottom		= rcSetPos.top + rcSetPos.sizeY;

		m_pRightTop->SetLocalPos(rcSetPos);
	}

	{
		UIRECT rcSetPos;

		rcSetPos.sizeX		= m_pLeftBottom->GetLocalPos().sizeX;
		rcSetPos.sizeY		= m_pLeftBottom->GetLocalPos().sizeY;

		rcSetPos.left		= rcParentNewPos.left;
		rcSetPos.top		= max( 0,rcParentNewPos.bottom - rcSetPos.sizeY);
		rcSetPos.right		= rcSetPos.left + rcSetPos.sizeX;
		rcSetPos.bottom		= rcSetPos.top + rcSetPos.sizeY;

		m_pLeftBottom->SetLocalPos(rcSetPos);
	}

	{
		UIRECT rcSetPos;

		rcSetPos.sizeX		= m_pRightBottom->GetLocalPos().sizeX;
		rcSetPos.sizeY		= m_pRightBottom->GetLocalPos().sizeY;

		rcSetPos.left		= max( 0,rcParentNewPos.right - rcSetPos.sizeX);
		rcSetPos.top		= max( 0,rcParentNewPos.bottom - rcSetPos.sizeY);
		rcSetPos.right		= rcSetPos.left + rcSetPos.sizeX;
		rcSetPos.bottom		= rcSetPos.top + rcSetPos.sizeY;

		m_pRightBottom->SetLocalPos(rcSetPos);
	}

	{
		UIRECT rcSetPos;

		rcSetPos.sizeX		= m_pTop->GetLocalPos().sizeX;
		rcSetPos.sizeY		= m_pLeft->GetLocalPos().sizeY;

		rcSetPos.left		= m_pLeftTop->GetLocalPos().right;
		rcSetPos.top		= m_pLeftTop->GetLocalPos().bottom;
		rcSetPos.right		= rcSetPos.left + rcSetPos.sizeX;
		rcSetPos.bottom		= rcSetPos.top + rcSetPos.sizeY;

		m_pCenter->SetLocalPos(rcSetPos);
	}
}

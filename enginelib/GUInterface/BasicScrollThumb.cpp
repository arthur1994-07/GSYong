#include "pch.h"
#include "BasicScrollThumb.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CBasicScrollThumb::CBasicScrollThumb (EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
{
}

CBasicScrollThumb::~CBasicScrollThumb ()
{
}

void CBasicScrollThumb::CreateSkin ( char* szBodyValue, char* szUpValue, char* szDnValue, const bool& bVERTICAL )
{
	if ( bVERTICAL )
	{
		CUIControl* pBody = new CUIControl(m_pEngineDevice);
		pBody->CreateSub ( this, szBodyValue, UI_FLAG_YSIZE );
		pBody->SetControlNameEx ( "��ũ�ѹٵ� | UI_FLAG_YSIZE_PROPORTION" );
		RegisterControl ( pBody );

		CUIControl* pUp = new CUIControl(m_pEngineDevice);	
		pUp->CreateSub ( this, szUpValue );	
		pUp->SetControlNameEx ( "��ũ�Ѿ��̹���" );
		RegisterControl ( pUp );

		CUIControl* pDn = new CUIControl(m_pEngineDevice);
		pDn->CreateSub ( this, szDnValue, UI_FLAG_BOTTOM );
		pDn->SetControlNameEx ( "��ũ�Ѵٿ��̹��� | UI_FLAG_BOTTOM" );
		RegisterControl ( pDn );
	}
	else
	{
		CUIControl* pBody = new CUIControl(m_pEngineDevice);
		pBody->CreateSub ( this, szBodyValue, UI_FLAG_XSIZE );
		pBody->SetControlNameEx ( "��ũ�ѹٵ� | UI_FLAG_XSIZE_PROPORTION" );
		RegisterControl ( pBody );

		CUIControl* pUp = new CUIControl(m_pEngineDevice);	
		pUp->CreateSub ( this, szUpValue );	
		pUp->SetControlNameEx ( "��ũ�Ѿ��̹���" );
		RegisterControl ( pUp );

		CUIControl* pDn = new CUIControl(m_pEngineDevice);
		pDn->CreateSub ( this, szDnValue, UI_FLAG_RIGHT );
		pDn->SetControlNameEx ( "��ũ�Ѵٿ��̹��� | UI_FLAG_BOTTOM" );
		RegisterControl ( pDn );
	}
}
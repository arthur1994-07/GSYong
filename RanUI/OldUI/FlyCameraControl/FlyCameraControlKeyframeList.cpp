
#include "StdAfx.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/BasicScrollBarEx.h"
#include "../../EngineLib/GUInterface/BasicScrollThumbFrame.h"
#include "../../InnerInterface.h"
#include "../../RanLogicClient/FlyCameraControl/GLFlyCameraControl.h"
#include "../../RanLogicClient/GLGaeaClient.h"

#include "../Util/UIGroupHelper.h"
#include "../Util/RnButton.h"
#include "../Util/RnEditBox.h"
#include "./FlyCameraControlUIDefine.h"
#include "./FlyCameraControlKeySlot.h"
#include "./FlyCameraControlKeyframeList.h"

FlyCameraControlKeyframeList::FlyCameraControlKeyframeList(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
: CUIGroupHelper(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pKeyFrameListBox(NULL)
, m_pListScrollBar(NULL)
, m_nSelectIndex(-1)
{
	ZeroMemory(m_pKeySlot, sizeof(m_pKeySlot) );
}

void FlyCameraControlKeyframeList::CreateSubControl(void)
{
	CreateLineBox("FLYCAMERACONTROL_KEYFRAMELIST_LINEBOX", "PARTY_INFO_LINEBOX_TEXTUREINFO");
	
	// List Scroll
	m_pListScrollBar = new CBasicScrollBarEx(m_pEngineDevice);
	m_pListScrollBar->CreateSub( this
		, "BASIC_SCROLLBAR"
		, UI_FLAG_RIGHT | UI_FLAG_YSIZE
		, IDD_FLYCAMERACONTROLLIST_SCROLLBAR );
	m_pListScrollBar->CreateBaseScrollBar( "FLYCAMERACONTROL_LIST_SCROLL" );
	m_pListScrollBar->GetThumbFrame()->SetScrollParent( this );
	m_pListScrollBar->GetThumbFrame()->SetScrollMoveLine( 1 );
	RegisterControl ( m_pListScrollBar );

	// Slot;
	for ( int i = 0; i < LIST_VISIBLE_LINE; ++i )
	{
		CString strKeyword;
		strKeyword.Format("FLYCAMERACONTROL_LIST_KEYSLOTBOX_%d", i);
		// AddSlot(i);
		m_pKeySlot[i] = new FlyCameraControlKeySlot(m_pEngineDevice, i);
		m_pKeySlot[i]->CreateSub(this
			, strKeyword.GetString()
			, UI_FLAG_DEFAULT
			, IDD_FLYCAMERACONTROLLIST_LIST_SLOT_0 + i);
		m_pKeySlot[i]->CreateSubControl();
		RegisterControl(m_pKeySlot[i]);
	}
}

void FlyCameraControlKeyframeList::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	CUIGroupHelper::TranslateUIMessage(ControlID, dwMsg);

	switch ( ControlID )
	{
	case IDD_FLYCAMERACONTROLLIST_LIST_SLOT_0:
	case IDD_FLYCAMERACONTROLLIST_LIST_SLOT_1:
	case IDD_FLYCAMERACONTROLLIST_LIST_SLOT_2:
	case IDD_FLYCAMERACONTROLLIST_LIST_SLOT_3:
	case IDD_FLYCAMERACONTROLLIST_LIST_SLOT_4:
	case IDD_FLYCAMERACONTROLLIST_LIST_SLOT_5:
	case IDD_FLYCAMERACONTROLLIST_LIST_SLOT_6:
	case IDD_FLYCAMERACONTROLLIST_LIST_SLOT_7:
	case IDD_FLYCAMERACONTROLLIST_LIST_SLOT_8:
	case IDD_FLYCAMERACONTROLLIST_LIST_SLOT_9:
		{
			if ( dwMsg & FLYCAMERA_UI_MSG_DEFINE::dwFLYCAMERA_UIMSG_KEY_SELECT )
			{
				FlyCameraControlKeySlot* const pSlot = static_cast<FlyCameraControlKeySlot*>(this->FindControl(ControlID));
				m_nSelectIndex = pSlot->GetSlotIndex();
				m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_SET_CURRENTKEYFRAME, m_nSelectIndex);
				AddMessageEx(FLYCAMERA_UI_MSG_DEFINE::dwFLYCAMERA_UIMSG_DETAILPAGE_ACTIVITY|UIMSG_TOTOPPARENT);
			}
		}
		break;
	}
}

void FlyCameraControlKeyframeList::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	{
		UpdateList();
	}
}

void FlyCameraControlKeyframeList::UpdateList(void)
{
	// List Scroll Update;
	{
		const unsigned int nTotalLine = GLFlyCameraControl::GetInstance().GetKeyFrameSize(),//GLFlyCameraControl::GetInstance().GetKeySize(),
			nMoveableLine = nTotalLine - LIST_VISIBLE_LINE;
		// 전체 라인, 한번에 보이는 라인;
		m_pListScrollBar->GetThumbFrame()->SetState(nTotalLine, LIST_VISIBLE_LINE);
		CBasicScrollThumbFrame* const pThumbFrame = m_pListScrollBar->GetThumbFrame();
		const float fViewPosPercent = pThumbFrame->GetPercent();

		unsigned int nVisiblePos = (unsigned int)floor(fViewPosPercent*nMoveableLine);		
		if ( nVisiblePos < 0 )
			nVisiblePos = 0;

		for ( int i = 0; i < LIST_VISIBLE_LINE; ++i )
		{
			const unsigned int nRelativePos = nVisiblePos + i;
			if ( nRelativePos < nTotalLine )
			{
				const unsigned int nAtIndex = GLFlyCameraControl::GetInstance().GetAtIndex(nRelativePos);
				if ( nAtIndex != -1 )
				{
					m_pKeySlot[i]->SetKeySlotInfo(nAtIndex, nAtIndex == m_nSelectIndex);
					m_pKeySlot[i]->SetVisibleSingle(TRUE);
				}
			}
			else
			{
				m_pKeySlot[i]->SetVisibleSingle(FALSE);
			}
		}
	}
}

void FlyCameraControlKeyframeList::DelSlot(void)
{

}
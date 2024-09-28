
#include "StdAfx.h"

#include "../../InnerInterface.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../RanLogic/FlyCamera/FlyCameraDefine.h"

#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../Util/UIGroupHelper.h"

#include "./FlyCameraControlUIDefine.h"
#include "./FlyCameraControlKeyframeList.h"
#include "./FlyCameraControlKeyframeTimeLine.h"
#include "./FlyCameraControlDetailKeyControl.h"
#include "./FlyCameraControlKeyframePage.h"

FlyCameraControlKeyframePage::FlyCameraControlKeyframePage(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
: CUIGroupHelper(pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pFlyCameraKeyFrameList(NULL)
, m_pFlyCameraKeyTimeLine(NULL)
{

}

void FlyCameraControlKeyframePage::CreateSubControl(void)
{
	// Keyframe List;
	{
		m_pFlyCameraKeyFrameList = new FlyCameraControlKeyframeList(m_pGaeaClient, m_pEngineDevice);
		m_pFlyCameraKeyFrameList->CreateSub( this
			, "FLYCAMERACONTROL_KEYFRAMELIST_MAN"
			, UI_FLAG_DEFAULT
			, IDD_FLYCAMERACONTROL_KEYFRAME_LIST);
		m_pFlyCameraKeyFrameList->CreateSubControl();
	}

	// Keyframe TimeLine;
	{
		m_pFlyCameraKeyTimeLine = new FlyCameraControlKeyframeTimeLine(m_pEngineDevice, m_pGaeaClient);
		m_pFlyCameraKeyTimeLine->CreateSub( this
			, "FLYCAMERACONTROL_KEYFRAMETIMELINE_MAN"
			, UI_FLAG_DEFAULT
			, IDD_FLYCAMERACONTROL_KEYFRAME_TIMELINE);
		m_pFlyCameraKeyTimeLine->CreateSubControl();
	}

	// Detail Key Control
	{
		m_pFlyCameraKeyDetailControl = new FlyCameraControlDetailKeyControl(m_pGaeaClient, m_pEngineDevice);
		m_pFlyCameraKeyDetailControl->CreateSub(this
			, "FLYCAMERACONTROL_KEYCONTROLDETAIL_MAN"
			, UI_FLAG_DEFAULT
			, IDD_FLYCAMERACONTROL_DETAILKEY_CONTROL);
		m_pFlyCameraKeyDetailControl->CreateSubControl();
		m_pFlyCameraKeyDetailControl->SetVisibleSingle(FALSE);
	}

	RegisterControl(m_pFlyCameraKeyFrameList);
	RegisterControl(m_pFlyCameraKeyTimeLine);
	RegisterControl(m_pFlyCameraKeyDetailControl);
}

void FlyCameraControlKeyframePage::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	CUIGroupHelper::TranslateUIMessage(ControlID, dwMsg);

	switch ( ControlID )
	{
	case IDD_FLYCAMERACONTROL_KEYFRAME_LIST:	
		{
			if ( dwMsg & FLYCAMERA_UI_MSG_DEFINE::dwFLYCAMERA_UIMSG_DETAILPAGE_ACTIVITY )
			{
				m_pFlyCameraKeyDetailControl->SetVisibleSingle(TRUE);
				m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_SET_CURRENTKEYFRAME, m_pFlyCameraKeyFrameList->GetSelectIndex());
				m_pFlyCameraKeyDetailControl->SetDetailSlot(m_pFlyCameraKeyFrameList->GetSelectIndex());
			}
		}
		break;
	case IDD_FLYCAMERACONTROL_KEYFRAME_TIMELINE:
		{
			if ( dwMsg & FLYCAMERA_UI_MSG_DEFINE::dwFLYCAMERA_UIMSG_KEY_ADD )
			{
				m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_ADD_KEYFRAMESLOT, m_pFlyCameraKeyTimeLine->GetCurrentKey());
			}
			else if ( dwMsg & FLYCAMERA_UI_MSG_DEFINE::dwFLYCAMERA_UIMSG_KEY_DEL )
			{
				m_pGaeaClient->FlyCamera_AddMsg(FlyCameraControl::EMFLYCAMERA_DEL_KEYFRAMESLOT, m_pFlyCameraKeyFrameList->GetSelectIndex());
			}
		}
		break;
	}
}

void FlyCameraControlKeyframePage::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	CUIGroupHelper::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);
}

const D3DXVECTOR3& FlyCameraControlKeyframePage::GetLastInputPos(void)
{
	return m_pFlyCameraKeyDetailControl->GetLastInputPos();
}
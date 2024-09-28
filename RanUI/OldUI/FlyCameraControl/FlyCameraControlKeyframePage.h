
#pragma once

class GLGaeaClient;
class FlyCameraControlKeyframeList;
class FlyCameraControlKeyframeTimeLine;
class FlyCameraControlDetailKeyControl;

class FlyCameraControlKeyframePage : public CUIGroupHelper
{
private:
	enum /*UIID*/{
		IDD_FLYCAMERACONTROL_KEYFRAME_LIST = NO_ID + 1,
		IDD_FLYCAMERACONTROL_KEYFRAME_TIMELINE,
		IDD_FLYCAMERACONTROL_DETAILKEY_CONTROL,
	};

	GLGaeaClient*	m_pGaeaClient;
	FlyCameraControlKeyframeList*		m_pFlyCameraKeyFrameList;
	FlyCameraControlKeyframeTimeLine*	m_pFlyCameraKeyTimeLine;
	FlyCameraControlDetailKeyControl*	m_pFlyCameraKeyDetailControl;

public:
	FlyCameraControlKeyframePage(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice);
	virtual ~FlyCameraControlKeyframePage() {}

public:
	void CreateSubControl(void);
	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg) override;
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl) override;

public:
	// 외부 참조용 FlyCam -> Innerinterface -> FlycameraUI;
	const D3DXVECTOR3& GetLastInputPos(void);
};

#pragma once

class GLGaeaClient;
class FlyCameraControlPage;
class FlyCameraControlKeyframePage;

class FlyCameraControlWindow : public CUIWindowEx, IFlyCameraControlWindow
{
private:
	enum /*UIID*/{
		IDD_FLYCAMERACONTROL_WINDOW_MOVEROTATE_PAGE = ET_CONTROL_NEXT,//NO_ID + 1,
		IDD_FLYCAMERACONTROL_WINDOW_KEYFRAME_PAGE,
	};
	GLGaeaClient*			m_pGaeaClient;
	FlyCameraControlPage*	m_pFlyCameraControlPage;
	FlyCameraControlKeyframePage*	m_pFlyCameraKeyframePage;

public:
	FlyCameraControlWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~FlyCameraControlWindow(void){};

public:
	virtual void CreateUIWindowAndRegisterOwnership(void) override;
	virtual void SetVisibleSingle(BOOL bVisible) override;
	virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl) override;
	virtual void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg ) override;

	// 외부 참조용 FlyCam -> Innerinterface -> FlycameraUI;
	const D3DXVECTOR3& GetLastInputPos(void);
private:
	void CreateSubControl(void);
};
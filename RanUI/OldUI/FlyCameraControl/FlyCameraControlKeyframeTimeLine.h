
#pragma once

class GLGaeaClient;
class RnButton;
class CUIEditBoxMan;
class CBasicTextBoxEx;
class CBasicProgressBar;
class RnProgressBar;

class FlyCameraControlKeyframeTimeLine : public CUIGroupHelper
{
private:
	enum /*UIID*/{
		IDD_FLYCAMERATIMELINE_FPS_EDITBOX = NO_ID + 1,
		IDD_FLYCAMERATIMELINE_KEY_EDITBOX,
		IDD_FLYCAMERATIMELINE_TOTAL_EDITBOX,
		IDD_FLYCAMERATIMELINE_ADD_BUTTON,
		IDD_FLYCAMERATIMELINE_DEL_BUTTON,
		IDD_FLYCAMERATIMELINE_PLAY_BUTTON,
		IDD_FLYCAMERATIMELINE_STOP_BUTTON,
		IDD_FLYCAMERATIMELINE_REWIND_BUTTON,

		IDD_FLYCAMERATIMELINE_TIMELINE_PROGRESSBAR,
	};

	enum /**/{
		EDITBOX_COUNT = 3,
	};

private:
	GLGaeaClient*	m_pGaeaClient;
	
	CUIEditBoxMan*	m_pEdtMan;
	
	RnButton*	m_pBtnKeyAdd;
	RnButton*	m_pBtnKeyDel;
	RnButton*	m_pBtnPlay;
	RnButton*	m_pBtnStop;
	RnButton*	m_pBtnRewind;

	RnProgressBar*	m_pProgTimeLine;

	int m_nFPS;
	int m_nCurrentKeyFrame;
	int m_nTotalKeyFrame;

public:
	FlyCameraControlKeyframeTimeLine(EngineDeviceMan* pEngineDevice, GLGaeaClient* pGaeaClient);
	virtual ~FlyCameraControlKeyframeTimeLine(void) {}

public:
	void CreateSubControl(void);
	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg) override;
	virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl) override;

	const int GetCurrentKey(void) {return m_nCurrentKeyFrame;}
public:
	bool _ExpansionEditBoxMsgCheck(UIGUID uiID);
};
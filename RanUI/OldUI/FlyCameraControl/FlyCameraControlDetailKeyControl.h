
#pragma once

class GLGaeaClient;
class CUIEditBoxMan;
class RnButton;
class CBasicTextBoxEx;
class CSmartComboBoxBasicEx;

namespace FlyCameraControl{
	enum EMFLYCAMERA_MOVETO_TYPE;
	enum EMFLYCAMERA_LOOKAT_TYPE;
}

class FlyCameraControlDetailKeyControl : public CUIGroupHelper
{
private:
	enum /*UIID*/{
		IDD_FLYCAMERADETAILCTRL_MOVETO_TYPE_COMBO = NO_ID + 1,
		IDD_FLYCAMERADETAILCTRL_LOOKAT_TYPE_COMBO,
		IDD_FLYCAMERADETAILCTRL_MOVETO_EXPLAIN_TEXT,
		IDD_FLYCAMERADETAILCTRL_LOOKAT_EXPLAIN_TEXT,
		IDD_FLYCAMERADETAILCTRL_MOVETO_VALUE_EDIT,
		IDD_FLYCAMERADETAILCTRL_LOOKAT_VALUE_EDIT,

		IDD_FLYCAMERADETAILCTRL_SAVE_BTN,
	};

	enum /**/{
		FLYCAMERADETAILCTRL_EDITSIZE = 2,
	};
	GLGaeaClient*			m_pGaeaClient;
	
	FlyCameraControl::EMFLYCAMERA_MOVETO_TYPE m_emMovetoType;
	FlyCameraControl::EMFLYCAMERA_LOOKAT_TYPE m_emLookatType;

	CSmartComboBoxBasicEx* m_CbxMovetoType;

	CBasicTextBox*	m_pTbxMovetoExplain;
	CBasicTextBox*	m_pTbxLookatExplain;

	CUIEditBoxMan*		m_pEdtMan;
	RnButton*			m_pBtnSave;

	// 외부 사용값;
private:
	D3DXVECTOR3 m_vLastInputPos;

public:
	FlyCameraControlDetailKeyControl(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice);
	virtual ~FlyCameraControlDetailKeyControl(void){}

public:
	void CreateSubControl(void);
	virtual void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg) override;
	virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl) override;

public:
	void SetDetailSlot( const int nSetIndex );
	// 외부 참조용 FlyCam -> Innerinterface -> FlycameraUI;
	const D3DXVECTOR3& GetLastInputPos(void)	{ return m_vLastInputPos; }

private:
	void InitCombo(void);
	void InitMovetoType(void);
	void InitLookatType(void);
	void UpdateCombo(void);
};
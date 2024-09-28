#pragma once

#include "OddEvenWindowDefine.h"

class OddEvenWindow : public IOddEvenWindow, public CUIWindowEx
{
	enum
	{
		ODDEVEN_BEGIN_BUTTON = ET_CONTROL_NEXT,
		ODDEVEN_REBEGIN_BUTTON,
		ODDEVEN_ROLL_BUTTON,
		ODDEVEN_RECEIVE_BUTTON,
		ODDEVEN_EXIT_BUTTON, 
		ODDEVEN_ODD_BUTTON,
		ODDEVEN_EVEN_BUTTON,
		ODDEVEN_MONEY_BUTTON,	

		ODDEVEN_REWARD_LOSE, // ��;
		ODDEVEN_REWARD_1,
		ODDEVEN_REWARD_2,
		ODDEVEN_REWARD_3,
		ODDEVEN_REWARD_4,
		ODDEVEN_REWARD_NSIZE = (ODDEVEN_REWARD_4 - ODDEVEN_REWARD_LOSE + 1),

		ODDEVEN_REWARD_ITEM_LOSE = ODDEVEN_REWARD_4 + 1,
		ODDEVEN_REWARD_ITEM_1,
		ODDEVEN_REWARD_ITEM_2,
		ODDEVEN_REWARD_ITEM_3,
		ODDEVEN_REWARD_ITEM_4,
	};

	enum
	{
		ODDEVEN_YESNO_LABEL,
		ODDEVEN_READY_LABEL,
		ODDEVEN_SELECT_LABEL,				
		ODDEVEN_WIN_LABEL,
		ODDEVEN_LOSE_LABEL,
		ODDEVEN_REWARD_LABEL,
		ODDEVEN_LABEL_NSIZE,
	};
public:
	virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl) override;
	virtual void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg) override;
	virtual void SetVisibleSingle(BOOL bVisible) override;
	virtual void CreateUIWindowAndRegisterOwnership(void) override;
	virtual void SetVisible(const bool bVisible) override;

public:
	virtual HRESULT Render(LPDIRECT3DDEVICEQ pd3dDevice) override;
	virtual HRESULT InitDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice) override;
	virtual HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICEQ pd3dDevice) override;
	virtual HRESULT InvalidateDeviceObjects(void) override;	

private: // �ֻ��� render;
	inline void _renderSetRenderState(LPDIRECT3DDEVICEQ pd3dDevice, RenderState& _sRenderState);
	inline void _renderRenderItem(LPDIRECT3DDEVICEQ pd3dDevice);
	inline void _renderRecoverRenderState(LPDIRECT3DDEVICEQ pd3dDevice, const RenderState& _sRenderState);
	inline const HRESULT _renderRenderDiceTable(LPDIRECT3DDEVICEQ pd3dDevice);

private:	
	void _updateButtons(const MiniGame::GLOddEvenClient* const pGame);
	void _updateLabel(const MiniGame::GLOddEvenClient* const pGame);
	void _updateGuideText(const MiniGame::GLOddEvenClient* const pGame);
	void _updateStage(const MiniGame::GLOddEvenClient* const pGame, const float fElapsedTime);

	void _translateUIMessageBeginButton(void);
	void _translateUIMessageRollButton(void);
	void _translateUIMessageReceiveButton(void);
	void _translateUIMessageExitButton(void);
	void _translateUIMessageOddButton(void);
	void _translateUIMessageEvenButton(void);	
	void _translateUIMessageRewardButton(const DWORD nRewardIndex);

public:
	const OddEvenWindow& operator =(const OddEvenWindow&){}
	OddEvenWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual ~OddEvenWindow(void);

private:
	CInnerInterface* const m_pInterface;
	GLGaeaClient* const m_pGaeaClient;
	MiniGame::GLManagerClient* const m_pMiniGameManager;

	D3DSURFACE_DESC		m_d3dBackBufferDesc;	
	LPDIRECT3DSURFACEQ	m_pTargetSurface;
	LPDIRECT3DSURFACEQ	m_pTargetZBuffer;
	LPDIRECT3DTEXTUREQ	m_pTargetTexture;	
	D3DMATERIAL9			m_Material;
	D3DLIGHT9					m_Light[3];
	D3DXMATRIXA16			m_mView;
	D3DXMATRIXA16			m_mProjection;
	UIVERTEX					m_TargetUIVertex[VERTEX_SIZE];
	DxSimMeshResource		m_DimMeshDice[2];
	MiniGame::IDice*			m_pDice; // �ֻ��� ���� ��ü;
	float							m_fUHalfSize[2];
	float							m_fHalfSize[2];

	CUIControl* m_pDiceTable; // �ֻ��� ���̺� �ڽ�;
	CUIControl* m_pLabel[ODDEVEN_LABEL_NSIZE]; // ��;
	RnButton* m_pBeginButton; // ���� ��ư;
	RnButton* m_pRebeginButton; // ���� ���� ��ư;
//	RnButton* m_pRollButton; // ������ ��ư;
	RnButton* m_pReceiveButton; // ���� �ޱ� ��ư;
	RnButton* m_pExitButton; // ������ ��ư;	

	CBasicButton* m_pOddButton; // Ȧ�� ��ư;
	CBasicButton* m_pEvenButton; // ¦�� ��ư;

	CBasicTextBox* m_pGuideText;
	CBasicTextBox* m_pMoneyText;
	CBasicTextBox* m_pProcessText[4]; // �غ�, ����, ������, ����;
	CBasicTextBox* m_pInformationText;
	CBasicTextBox* m_pProgressText;

	RnProgressBar* m_pProgressBar;

	CBasicButton* m_pRewardCloseBox[ODDEVEN_REWARD_NSIZE];
	CUIControl* m_pRewardOpenBox[ODDEVEN_REWARD_NSIZE][2];
	CUIControl* m_pRewardOpenSelectBox[ODDEVEN_REWARD_NSIZE];
	CItemImage* m_pItemImage[ODDEVEN_REWARD_NSIZE];

	DWORD m_nRequestRewardCount;
	bool m_bLock;
	bool m_bSelectReward;
	float m_fCurrentTime;
	float m_fLastRequestTime;
	float m_fResultElapsedTime;
	float m_fSelectRewardAnimateElapsedTime;
	float m_fSelectRewardElapsedTime;
};
#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class	CInnerInterface;
class	CBasicTextBox;
class	CBasicButton;
class	CD3DFontPar;
class	CBasicComboBox;
class	CBasicComboBoxRollOver;

class	CBasicGamePlayOption : public CUIGroup
{
protected:
	enum
	{
		HWOPTION_GAMEPLAY_CONFT_BUTTON = NO_ID + 1,		// '�ڵ�' ��� �ź�
		HWOPTION_GAMEPLAY_TRADE_BUTTON,					// '�ڵ�' �ŷ� �ź�
		HWOPTION_GAMEPLAY_PARTY_BUTTON,					// '�ڵ�' ��Ƽ �ź�
		HWOPTION_GAMEPLAY_SIMPLEHP_BUTTON,				// �̴� HP ǥ��
		HWOPTION_GAMEPLAY_FRIEND_BUTTON,				// �ڵ� ģ�� ��û �ź�
		HWOPTION_GAMEPLAY_DEFAULTPOS_BUTTON,			// ����Ʈ ��ġ�̵�
		HWOPTION_GAMEPLAY_MOVABLE_ON_CHAT_BUTTON,		// ä��â �������� �����̱�
		HWOPTION_GAMEPLAY_SHOW_TIP_BUTTON,				// �� �����ֱ�
		HWOPTION_GAMEPLAY_FORCED_ATTACK_BUTTON,			// ���� ����
		HWOPTION_GAMEPLAY_NAME_DISPLAY_BUTTON,			// �̸� �׻� ǥ��
		HWOPTION_GAMEPLAY_SIMPLE_COMBO_OPEN,			// �ֺ� ĳ���� �ܼ�ȭ �޺�
		HWOPTION_GAMEPLAY_SIMPLE_COMBO_ROLLOVER,		// �ֺ� ĳ���� �ܼ�ȭ �޺�
		HWOPTION_GAMEPLAY_SIMPLE_SUMMON_NAME,			// ��ȯ�� �̸� �ܼ�ȭ
        HWOPTION_GAMEPLAY_AUTO_CROW_HPBAR,				// ���� HP�� �ڵ� ǥ��
		HWOPTION_GAMEPLAY_SKILLTYPE_COMBO_OPEN,			// ��ų Ÿ�� UI
		HWOPTION_GAMEPLAY_SKILLTYPE_COMBO_ROLLOVER,		// ��ų Ÿ�� UI
		HWOPTION_GAMEPLAY_VEHICLE_BUTTON,				// �ڵ� ž�¿�û �ź�
		HWOPTION_GAMEPLAY_CHAR_INFO_PRIVATE,			// �� ���� �����
		HWOPTION_GAMEPLAY_CHAR_INFO_ADDITION,			// �߰� ���� ����
		HWOPTION_GAMEPLAY_GROUP_CHAT_BUTTON,			// '�ڵ�' �׷�ä�� �ź�
		HWOPTION_GAMEPLAY_AUTOREBIRTH,					// �ڵ� ��Ȱ�� ��Ȱ�ϱ��;
	};

public:
	CBasicGamePlayOption(CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CBasicGamePlayOption ();

public:
	void	CreateSubControl ();

private:
	CBasicButton*	CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID );
	CBasicTextBox*	CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR D3DCOLOR, int nAlign );

public:
	virtual	void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle ( BOOL bVisible );

public:
	void	LoadCurrentOption();

private:
	void	LoadConft ();
	void	LoadTrade ();
	void	LoadParty ();
	void	LoadSimpleHP ();
	void	LoadFriend ();
	void	LoadMovableOnChat ();
	void	LoadShowTip ();
	void	LoadForcedAttack ();
	void	LoadNameDisplay ();
	void	LoadAroundSimple ();
	void	LoadSimpleSummonName ();
	void	LoadSkillTypeUI();
	void	LoadVehicle();
    void    LoadAutoCrowHPBar();
	void	LoadCharInfoPrivate();
	void	LoadCharInfoAddition();
	void	LoadGroupChatInvite();
	void	LoadRebirthOption();

private:
	CD3DFontPar*	m_pFont9;	
	UIGUID			m_RollOverID;
	BOOL			m_bFirstLBUP;

private:
	CBasicButton*		m_pConftButton;
	CBasicButton*		m_pTradeButton;
	CBasicButton*		m_pPartyButton;
	CBasicButton*		m_pSimpleHPButton;
	CBasicButton*		m_pFriendButton;
	CBasicButton*		m_pMovableOnChatButton;
	CBasicButton*		m_pShowTipButton;
	CBasicButton*		m_pForcedAttackButton;
	CBasicButton*		m_pNameDisplayButton;
    CBasicButton*		m_pSimpleSummonNameButton;
    CBasicButton*		m_pAutoCrowHPBarButton;
	CBasicButton*		m_pVehicleButton;
	CBasicButton*		m_pCharInfoPrivateButton;
	CBasicButton*		m_pCharInfoAdditionButton;
	CBasicButton*		m_pGroupChatInvateButton;
	CBasicButton*		m_pAutoRebirthButton;

	CBasicComboBox*			m_pComboBoxSimpleOpen;
	CBasicComboBoxRollOver*	m_pComboBoxSimpleRollOver;

	CBasicComboBox*			m_pComboSkillTypeOpen;
	CBasicComboBoxRollOver*	m_pComboSkillTypeRollOver;

public:
	BOOL m_bConft;
	BOOL m_bTrade;
	BOOL m_bParty;
	BOOL m_bSimpleHP;
	BOOL m_bFriend;
	BOOL m_bVehicle;
	BOOL m_bMovableOnChat;
	BOOL m_bShowTip;
	BOOL m_bForcedAttack;
	BOOL m_bNameDisplay;
    BOOL m_bSimpleSummonName;
    BOOL m_bAutoCrowHPBar;
	DWORD m_dwAroundDetail;
	DWORD m_dwSkillTypeUI;
	BOOL m_bCharInfoPrivate;
	BOOL m_bCharInfoAddition;
	BOOL m_bGroupChatInvite;
	BOOL m_bAutoRebirth;

protected:
	CInnerInterface* m_pInterface;

};
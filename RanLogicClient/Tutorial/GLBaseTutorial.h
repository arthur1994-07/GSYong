#pragma once

#include "../Char/GLCharacter.h"
#include "../../RanLogic/Character/GLCharData.h"

// Tutorial Sound files load.
// const std::string TUTO_SOUNDS[ ] = {
// GLBaseTutorial �� m_TutorialSound �� �Ű�����.
//

class GLGaeaClient;

class GLBaseTutorial
{
public:
    enum MAIN_STEP
    {
        INTRODUCE_STEP = 1,
        EXPLAIN_INTERFACE_STEP = 2,
        MOUSECONTROL_AND_MOVE_STEP = 3,
        QUICKSLOT_ACTIONSLOT_STEP = 4,
        ATTACK_STEP = 5,
        QUEST_AND_HUNT_STEP = 6,
        LEVEL_UP_STEP = 7,
        QUEST_WINDOW_STEP = 8,
    };

    enum SUB_STEP
    {
        ONE_ = 1, TWO_ = 2, THREE_ = 3, FOUR_ = 4, FIVE_ = 5, SIX_ = 6, SEVEN_ = 7,
        EIGHT_ = 8, NINE_ = 9, TEN_ = 10, ELEVEN_ = 11, TWELVE_ = 12, THIRTEEN_ = 13,
        FOURTEEN_ = 14, FIFTEEN_ = 15, SIXTEEN_ = 16,
    };

public:
	GLBaseTutorial(GLGaeaClient* pGaeaClient);
    virtual ~GLBaseTutorial();

private:
    GLGaeaClient* m_pGaeaClient;

	bool m_bTutorial;		// Ʃ�丮�� ���������� ���� üũ.
	bool m_bControlMouse;	// Ʃ�丮�� �� ���콺 ��Ʈ�� �������� üũ.
	bool m_bMove;			// Ʃ�丮�� �� �̵� �������� üũ.
	GLCharacter* m_pGLChar;		//

	D3DXVECTOR3 m_vCameraPos;		// Ʃ�丮�� �� ī�޶� default ��ġ.
	float		m_fCameraLenght;	// Ʃ�丮�� �� ī�޶� default ��.
	float		m_fThetaX;			// Ʃ�丮�� �� ī�޶� default X�� ȸ�� ����.
	float		m_fThetaY;			// Ʃ�丮�� �� ī�޶� default Y�� ȸ�� ����.

	bool		m_bLevelUp;			// Ʃ�丮�� �� ������ Ȯ�� �Լ�.

    std::vector<std::string> m_TutorialSound;

public:
	// step �� �ʿ��� ������.
	D3DXVECTOR3		m_vStepFivePos;		// Ʃ�丮�� �� ����5 ��ġ.
	float			m_fLMouseMoveCheck, m_fRMouseMoveCheck;
	float			m_fZoomIn, mfZoomOut;
	bool			m_bClickMove[ 2 ];
	D3DXVECTOR3		m_vClickMoveTargetPos[ 2 ];	// Ʃ�丮�� �� Ŭ���� ���� ���� ����.
	int				m_iMinimapTargetX;
	int				m_iMinimapTargetY;
	D3DXVECTOR3		m_vMinimapTargetPos; // Ʃ�丮�� �� �̴ϸ� Ŭ���� ���� ���� ����.
	SNATIVEID		m_sHoldSkill;		// �տ� ��� �ִ� ��ų Ȯ���ϱ� ����.
	D3DXVECTOR3		m_vCameraRotate;	// ī�޶� ȸ�� ����.
	float			m_fCameraZoom;		// ī�޶� �� ��, �ƿ� ����.
	int				m_iHitCount;		// ���� HIT ī��Ʈ.
	SNATIVEID		m_sItemID;			// �����ؾ� �� ������.
	SNATIVEID		m_sSkillID[ 2 ];	// �����ؾ� �� ��ų.
	bool			m_bUseSkill[ 2 ];	// ����� ��ų.
	SNATIVEID		m_sUseSkillID;		// ����� ��ų.
	SNATIVEID		m_sMokgacID;		// ��ƾ� �� ��.
	SNATIVEID		m_sMobID;			// ��ƾ� �� ��.
	BOOL			m_bStep9UI;			// ����9���� UI ������ ����.
	bool			m_bFreeStep;
	bool			m_bDrop;			// ����� ������ �Ǵ� ���� UI ǥ�� ����.
	D3DXVECTOR3		m_vDropPos;			// ����� ������ �Ǵ� ���� UI ǥ���ϴ� ��ǥ.
	DWORD			m_dwQuest;			// ���� ����Ʈ.
	DWORD			m_dwQuestUI;		// UI ǥ�ÿ� �ʿ��� ����Ʈ.
	bool			m_bUICheck;			// ���ܿ� �ʿ��� UI�� �����ִ��� Ȯ��.

public: // public for test.
	int m_dwMainStep;	// Ʃ�丮�� ���� ���� ���ܴܰ�.
	int m_dwSubStep;	// Ʃ�丮�� ���� ���� ���ܴܰ�.

public:
	HRESULT InitData();
	void FrameMove( float fElapsedTime, GLCharacter* pGLChar );
    size_t GetSoundSize() const { return m_TutorialSound.size(); }
    void StopSound();
    void PlaySound(DWORD Index);
    void CreateSound();

	HRESULT StartTutorialInClient( GLCharacter* pGLChar );
	bool IsTutorial();
	void SkipTutorial();

	bool IsCanMoveInTutorial();
	bool IsCanMouseControlInTutorial();
	bool IsSaveCharInTutorial();
	bool IsFreeStep() { return m_bFreeStep; }
	void SetFreeStep( bool bFree ) { m_bFreeStep = bFree; }

	HRESULT SetTutorialStep( int dwMainStep, int dwSubStep );
	HRESULT NextMainStep();
	HRESULT NextSubStep();

	int GetCurrentMainStep();
	int GetCurrentSubStep();

	D3DXVECTOR3 GetDefaultCameraPos();
	float GetDefaultCameraLength();
	float GetDefaultCameraThetaX();
	float GetDefaultCameraThetaY();
	D3DXVECTOR3 GetStep5Pos();

	void CheckAttackForTutorial( const EMCROW emCrow, const DWORD dwID );
	bool IsLevelUpOne() { return m_bLevelUp; }

    bool IsIntroduceStep();
    bool IsExplainInterfaceStep();
    bool IsMouseContNMoveStep();
    bool IsQuickNActionSlotStep();
    bool IsAttackStep();
    bool IsQuestNHuntStep();
    bool IsLevelUpStep();
    bool IsQuestWindowStep();

    bool IsSubOneStep();
    bool IsSubTwoStep();
    bool IsSubThreeStep();
    bool IsSubFourStep();
    bool IsSubFiveStep();
    bool IsSubSixStep();
    bool IsSubSevenStep();
    bool IsSubEightStep();
    bool IsSubNineStep();
    bool IsSubTenStep();
    bool IsSubElevenStep();
    bool IsSubTwelveStep();
    bool IsSubThirteenStep();
    bool IsSubFourteenStep();
    bool IsSubFifteenStep();
    bool IsSubSixteenStep();
};
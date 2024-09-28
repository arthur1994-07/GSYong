#pragma once

#include "../Char/GLCharacter.h"
#include "../../RanLogic/Character/GLCharData.h"

// Tutorial Sound files load.
// const std::string TUTO_SOUNDS[ ] = {
// GLBaseTutorial 의 m_TutorialSound 로 옮겨졌음.
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

	bool m_bTutorial;		// 튜토리얼 진행중인지 상태 체크.
	bool m_bControlMouse;	// 튜토리얼 중 마우스 컨트롤 가능한지 체크.
	bool m_bMove;			// 튜토리얼 중 이동 가능한지 체크.
	GLCharacter* m_pGLChar;		//

	D3DXVECTOR3 m_vCameraPos;		// 튜토리얼 용 카메라 default 위치.
	float		m_fCameraLenght;	// 튜토리얼 용 카메라 default 줌.
	float		m_fThetaX;			// 튜토리얼 용 카메라 default X축 회전 정도.
	float		m_fThetaY;			// 튜토리얼 용 카메라 default Y축 회전 정도.

	bool		m_bLevelUp;			// 튜토리얼 용 레벨업 확인 함수.

    std::vector<std::string> m_TutorialSound;

public:
	// step 때 필요한 변수들.
	D3DXVECTOR3		m_vStepFivePos;		// 튜토리얼 용 스텝5 위치.
	float			m_fLMouseMoveCheck, m_fRMouseMoveCheck;
	float			m_fZoomIn, mfZoomOut;
	bool			m_bClickMove[ 2 ];
	D3DXVECTOR3		m_vClickMoveTargetPos[ 2 ];	// 튜토리얼 용 클릭을 통한 도달 지점.
	int				m_iMinimapTargetX;
	int				m_iMinimapTargetY;
	D3DXVECTOR3		m_vMinimapTargetPos; // 튜토리얼 용 미니맵 클릭을 통한 도달 지점.
	SNATIVEID		m_sHoldSkill;		// 손에 들고 있는 스킬 확인하기 위함.
	D3DXVECTOR3		m_vCameraRotate;	// 카메라 회전 정도.
	float			m_fCameraZoom;		// 카메라 줌 인, 아웃 정도.
	int				m_iHitCount;		// 목각인 HIT 카운트.
	SNATIVEID		m_sItemID;			// 장착해야 할 아이템.
	SNATIVEID		m_sSkillID[ 2 ];	// 장착해야 할 스킬.
	bool			m_bUseSkill[ 2 ];	// 사용한 스킬.
	SNATIVEID		m_sUseSkillID;		// 사용한 스킬.
	SNATIVEID		m_sMokgacID;		// 잡아야 할 몹.
	SNATIVEID		m_sMobID;			// 잡아야 할 몹.
	BOOL			m_bStep9UI;			// 스텝9에서 UI 보여짐 여부.
	bool			m_bFreeStep;
	bool			m_bDrop;			// 드랍된 아이템 또는 돈에 UI 표시 여부.
	D3DXVECTOR3		m_vDropPos;			// 드랍된 아이템 또는 돈에 UI 표시하는 좌표.
	DWORD			m_dwQuest;			// 진행 퀘스트.
	DWORD			m_dwQuestUI;		// UI 표시에 필요한 퀘스트.
	bool			m_bUICheck;			// 스텝에 필요한 UI가 열려있는지 확인.

public: // public for test.
	int m_dwMainStep;	// 튜토리얼 진행 메인 스텝단계.
	int m_dwSubStep;	// 튜토리얼 진행 서브 스텝단계.

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
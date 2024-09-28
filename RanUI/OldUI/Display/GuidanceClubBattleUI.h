#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogic/Msg/GLContrlInstanceContentsMsg.h"
#include "../../InnerInterface.h"

class CBasicProgressBar;
class GLGaeaClient;
class CInnerInterface;

class GuidanceAuthProgressUI : public CUIGroup
{
private:
    enum
    {
        CONFT_CONFIRM = NO_ID + 1,
    };

public:
    GuidanceAuthProgressUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual	~GuidanceAuthProgressUI();

public:
    void CreateSubControl ();

    virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl ) override;
    void SetAuthTime(const float _fAuthTime, const float _fCurrentTime);

protected:
    float fCurrentTime; // 현재 인증 진행 시간;
    float fAuthTime;    // 인증 완료 시간;

    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;
    CBasicProgressBar* m_pProgress;
};

class GuidanceProgressInfoUI : public CUIGroup
{
public:
    GuidanceProgressInfoUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual	~GuidanceProgressInfoUI();    

    void CreateSubControl ();

    void SetTopParam(const int _iParam1, const int _iParam2);
    void SetMiddleParam(const int _iParam1);
    void SetBottomParam(const float _fParam1);

    virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl ) override;
protected:
    CBasicTextBox* m_pTopText;      // 상단 텍스트 : 성문 선도 클럽 결정전 진행중;
    CBasicTextBox* m_pMiddleText;   // 중단 텍스트 : 목표 점수 :;
    CBasicTextBox* m_pBottomText;   // 하단 텍스트 : 남은 시간 :;

    CUIControl* m_pBackGround; // 배경 화면;

    float m_fRemainTime;  // 남은 시간 : 초단위 ( 80 => 1:20 );

    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;
    CBasicProgressBar* m_pProgress;
};

class GuidanceRankInfoUI : public CUIGroup
{
public:
    GuidanceRankInfoUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~GuidanceRankInfoUI();

    void CreateSubControl ();
    void SetRankInfo(GLMSG::NET_GUIDANCE_RANK_VEC& _RankInfo);
    void SetRankMyInfo(const int _iRank, const int _iPoint);

    virtual	void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl ) override;
    virtual void SetVisibleSingle( BOOL bVisible ) override;
protected:
    std::vector<DWORD> clubRank;
    CBasicTextBox* m_pClubRankText;    // 순위 텍스트;
    CBasicTextBox* m_pClubNameText;     // 클럽이름 텍스트;
    CBasicTextBox* m_pClubPointText;    // 점수 텍스트;
    CBasicTextBox* m_pMyRankText;       // 내 클럽 순위 텍스트;
    CBasicTextBox* m_pMyNameText;       // 내 클럽 이름 텍스트;
    CBasicTextBox* m_pMyPointText;      // 내 클럽 점수 텍스트;

    CUIControl* m_pBackGround;  // 배경 화면;

    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;
    CBasicProgressBar* m_pProgress;
};












class MyGuidanceAuthProgressUI : public IGuidanceAuthProgressUI, public GuidanceAuthProgressUI
{
public:
    MyGuidanceAuthProgressUI( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyGuidanceAuthProgressUI() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual void SetAuthTime(const bool _bVisible, const float _fAuthTime, const float _fCurrentTime = 0.0f);
};

class MyGuidanceProgressInfoUI : public IGuidanceProgressInfoUI, public GuidanceProgressInfoUI
{
public:
    MyGuidanceProgressInfoUI(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual ~MyGuidanceProgressInfoUI() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual void SetProgressInfo(const NET_MSG_GENERIC* _pMessag);
};

class MyGuidanceRankInfoUI: public IGuidanceRankInfoUI, public GuidanceRankInfoUI
{
public:
    MyGuidanceRankInfoUI(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual ~MyGuidanceRankInfoUI() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual void SetRankInfo(const NET_MSG_GENERIC* _pMessag);
    virtual void SetRankMyInfo(const NET_MSG_GENERIC* _pMessag);
};
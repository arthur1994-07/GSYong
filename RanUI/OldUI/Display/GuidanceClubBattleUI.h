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
    float fCurrentTime; // ���� ���� ���� �ð�;
    float fAuthTime;    // ���� �Ϸ� �ð�;

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
    CBasicTextBox* m_pTopText;      // ��� �ؽ�Ʈ : ���� ���� Ŭ�� ������ ������;
    CBasicTextBox* m_pMiddleText;   // �ߴ� �ؽ�Ʈ : ��ǥ ���� :;
    CBasicTextBox* m_pBottomText;   // �ϴ� �ؽ�Ʈ : ���� �ð� :;

    CUIControl* m_pBackGround; // ��� ȭ��;

    float m_fRemainTime;  // ���� �ð� : �ʴ��� ( 80 => 1:20 );

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
    CBasicTextBox* m_pClubRankText;    // ���� �ؽ�Ʈ;
    CBasicTextBox* m_pClubNameText;     // Ŭ���̸� �ؽ�Ʈ;
    CBasicTextBox* m_pClubPointText;    // ���� �ؽ�Ʈ;
    CBasicTextBox* m_pMyRankText;       // �� Ŭ�� ���� �ؽ�Ʈ;
    CBasicTextBox* m_pMyNameText;       // �� Ŭ�� �̸� �ؽ�Ʈ;
    CBasicTextBox* m_pMyPointText;      // �� Ŭ�� ���� �ؽ�Ʈ;

    CUIControl* m_pBackGround;  // ��� ȭ��;

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
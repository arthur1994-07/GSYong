#pragma	once

#include "../Util/UIWindowEx.h"
#include "../../../RanLogic/Msg/GLContrlInstanceContentsMsg.h"
#include "../../../EngineLib/GUInterface/UIControlMessage.h"
#include "../../../EngineLib/GUInterface/InterfaceBaseDefine.h"
#include "../../../Enginelib/GUInterface/BasicLineBoxSmart.h"

#include "../../InnerInterface.h"

class CInnerInterface;
class GLGaeaClient;
class CBasicTextButton;
class CBasicTextBox;
class CBasicScrollBarEx;
class CSwapImage;

using namespace InstanceSystem;

// 결과 보상 툴팁 윈도우;
class RnCompetitionResultRewardTooltip : public CBasicLineBoxSmart
{
    enum
    {
        EM_MAX_REWARD = 5,
    };
protected:
    CUIControl*     pDummyTooltip;  // 더미 툴팁;

    CBasicTextBox*  pDummyTextTitle;  // 기여도;
    CBasicTextBox*  pDummyItemTitle;  // 기여도;
    CBasicTextBox*  pDummyBuffTitle;  // 기여도;
    CBasicTextBox*  pDummyText;  // 기여도;
    CBasicTextBox*  pDummyItem;  // 기여도;
    CBasicTextBox*  pDummyBuff;  // 기여도;

    CBasicTextBox*  pContributionTitleTextBox;
    CBasicTextBox*  pExpTitleTextBox;
    CBasicTextBox*  pMoneyTitleTextBox;
    CBasicTextBox*  pItemTitleTextBox;
    CBasicTextBox*  pBuffTitleTextBox;

    CBasicTextBox*  pContributionPointTextBox;  // 기여도;
    CBasicTextBox*  pExpTextBox;                // 경험치;
    CBasicTextBox*  pMoneyTextBox;              // 게임머니;
    CUIControl*     pItemImage[EM_MAX_REWARD];  // 아이템;
	CBasicTextBox*  pItemCount[EM_MAX_REWARD];  // 아이템개수;
    CUIControl*     pBuffImage[EM_MAX_REWARD];  // 버프;

    int             emCurrentTitleType;

public:
    RnCompetitionResultRewardTooltip(EngineDeviceMan* pEngineDevice)
        : CBasicLineBoxSmart(pEngineDevice)
        , pDummyTooltip(NULL)
        , pDummyTextTitle(NULL)
        , pDummyItemTitle(NULL)
        , pDummyBuffTitle(NULL)
        , pDummyText(NULL)
        , pDummyItem(NULL)
        , pDummyBuff(NULL)
        , pContributionTitleTextBox(NULL)
        , pExpTitleTextBox(NULL)
        , pMoneyTitleTextBox(NULL)
        , pItemTitleTextBox(NULL)
        , pBuffTitleTextBox(NULL)
        , pContributionPointTextBox(NULL)
        , pExpTextBox(NULL)
        , pMoneyTextBox(NULL)

    {
    }
    virtual ~RnCompetitionResultRewardTooltip()
    {
    }

public:
    void CreateSubControl( const char* szTextureInfoControl );
    void SetTitleInfo(const int emTitleType);
    void SetInfo(const RnCompetitionResultContentRewardObj* _pRewardObj);
};

// 결과 항목;
class RnCompetitionResultObjUI
{
public:
    CUIGroup* pUI;
    int emItemType;
	int iParam;	// UI에서 특정 인자가 필요할 때 사용 ( 현재 클럽마크 UI에서 클럽마크DbNum을 저장하는데 사용 );
};


// 결과 내용 슬롯;
class RnCompetitionResultInfoSlot  : public CUIGroup
{
    enum
    {
        RNCOMPETITION_RESULT_REWARD_BUTTON = NO_ID + 1,
    };
    GLGaeaClient* pGaeaClient;
    const bool bMyInfoSlot;
public:
    std::vector<RnCompetitionResultObjUI> vecContentUI; // 결과 목록 내용 UI; (텍스트,이미지 등);
    std::vector<RnCompetitionResultObjUI> vecContentRewardUI; // 결과 보상 목룍 내용 UI;

    const RnCompetitionResultContent* pContentData;  // 결과 내용 데이터;

public:
    RnCompetitionResultInfoSlot(GLGaeaClient* _pGaeaClient, EngineDeviceMan* pEngineDevice, const bool _bMyInfoSlot = false)
        : CUIGroup(pEngineDevice)
        , pGaeaClient(_pGaeaClient)
        , bMyInfoSlot(_bMyInfoSlot)
        , pContentData(NULL)
    {
    }
    virtual ~RnCompetitionResultInfoSlot()
    {
    }

public:
    virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice );

    void CreateSubControl ( const RnCompetitionResultTitle& _sResultTab, const unsigned int _nSlotNumber );
    void SetInfo( const RnCompetitionResultContent& _sResultContent );
};


// 탭 버튼;
class RnCompetitionResultTabButton  : public CUIGroup
{
#define RNCOMPETITION_RESULT_TAB_BUTTON_MESSAGE UIMSG_USER1
    enum
    {
        RNCOMPETITION_RESULT_TAB_BUTTON = NO_ID + 1,
    };

    CBasicTextBox* pTabNameTextBox;  // 탭 제목 UI;
    CBasicLineBoxSmart* pActiveBox;
    CBasicLineBoxSmart* pInactiveBox;

    bool                ENABLE;
public:
    RnCompetitionResultTabButton(EngineDeviceMan* pEngineDevice)
        : CUIGroup(pEngineDevice)
        , pTabNameTextBox(NULL)
        , pActiveBox(NULL)
        , pInactiveBox(NULL)
    {
    }

    virtual ~RnCompetitionResultTabButton()
    {
    }

public:
    virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
    void CreateSubControl ( const unsigned int _nTabNum, std::string& strTabName );

    void SetEnable( const bool bEnable );
};


// 탭 내용 윈도우;
class RnCompetitionResultTabWindow  : public CUIGroup
{
protected:
    enum
    {
        RESULT_INFO_MAX_SLOT = 10,
    };
    GLGaeaClient* pGaeaClient;
public:
    std::vector<CBasicTextBox*> vecTitleNameTextBox;  // 결과 제목 텍스트 UI;
    std::vector<RnCompetitionResultInfoSlot*> vecInfoSlot;  // 결과 내용 UI;
    RnCompetitionResultInfoSlot* myInfoSlot;  // 내 결과 내용 UI;

    const RnCompetitionResultTab* pTabData; // 해당 탭 데이터;
    int nRewardTitleNum;  // 보상의 제목 순번;

    std::vector<unsigned int> sortList; // 결과 정렬을 위한 리스트;
private:
    CBasicLineBoxSmart* pLineWindowBox;
    CBasicScrollBarEx* pScrollBar;
    CUIControl* pScrollBarBackground;

    unsigned int currentScrollPos;

public:
    RnCompetitionResultTabWindow(GLGaeaClient* _pGaeaClient, EngineDeviceMan* pEngineDevice)
        : CUIGroup(pEngineDevice)
        , pGaeaClient(_pGaeaClient)
        , myInfoSlot(NULL)
        , pTabData(NULL)
        , pLineWindowBox(NULL)
        , pScrollBar(NULL)
        , pScrollBarBackground(NULL)
        , nRewardTitleNum(-1)
        , currentScrollPos(0)
    {
    };

    virtual ~RnCompetitionResultTabWindow()
    {
    }

public:
    void CreateSubControl ( const RnCompetitionResultTab& _sResultTab );
    virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    void UpdateInfo();
    void UpdateInfoSlot(const unsigned int _nCurrentScoroll);

private:
    void SortInfo();
    const bool CompareInfo(const unsigned int _Info1, const unsigned int _Info2);
};



// 전장 결과 UI;
class RnCompetitionResultWindow : public CUIWindowEx
{
protected:
    enum
    {
        RNCOMPETITION_RESULT_INFO  = ET_CONTROL_NEXT,
        RNCOMPETITION_RESULT_CLOSE_BUTTON,
        RNCOMPETITION_RESULT_TAB_BUTTON_START,
    };
private:
    RnCompetotionResultInfo sResultInfo;

    std::vector<RnCompetitionResultTabButton*> vecTabButton;  // 전장 결과 탭 버튼;
    std::vector<RnCompetitionResultTabWindow*> vecResultTab;  // 전장 결과 탭 윈도우;
    CBasicTextBox* pText_Time;  // 보상/강제 나가기 시간;
    CBasicTextButton* pCloseButton;  // 결과 닫기 버튼;

    DWORD nCloseButtonParam;  // 결과 닫기 버튼 클릭시 커스텀 메시지에 보낼 파라미터 (0일 경우 사용 안함);

    RnCompetitionResultRewardTooltip* pRewardTooltip;  // 전장 보상 툴팁;

protected:
    GLGaeaClient* pGaeaClient;
    CUIControl* pDummyBody;
    CUIControl* pBackgroundHead;
    CUIControl* pBackgroundTail;

    unsigned int nCurrentTabNum;  // 현재 선택된 결과 탭 번호;

public:
    RnCompetitionResultWindow ( GLGaeaClient* _pGaeaClient, CInnerInterface* _pInterface, EngineDeviceMan* _pEngineDevice )
        : CUIWindowEx(_pInterface, _pEngineDevice)
        , pGaeaClient(_pGaeaClient)
        , nCurrentTabNum(-1)
        , nCloseButtonParam(0)
        , pDummyBody(NULL)
        , pBackgroundHead(NULL)
        , pBackgroundTail(NULL)
        , pRewardTooltip(NULL)
        , pText_Time(NULL)
        , pCloseButton(NULL)
    {
    }

public:
    virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
    virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
    virtual void SetVisibleSingle( BOOL bVisible );

public:
    void CreateSubControl();

protected:
    void CreateResultUI();

    const bool msgProc_RnCompetitionResult(const NET_MSG_GENERIC* _pMessage);

public:
    void UpdateTime();  // 보상/강제 나가기 시간 업데이트;
    void SelectResultTab( const unsigned int _nTabNum ); // 결과 탭 선택;
    void ShowRnCompetitionRewardTooltop(const int emTitleType, const RnCompetitionResultContentRewardObj* _pRewardOjb);
};




//-------------------------------------------------------------------------------------------------

class MyRnCompetitionResultWindow : public IRnCompetitionResultWindow, private RnCompetitionResultWindow
{
public:
    MyRnCompetitionResultWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyRnCompetitionResultWindow() {};

public:
    virtual void CreateUIWindowAndRegisterOwnership( LPDIRECT3DDEVICEQ pd3dDevice );

    virtual const bool msgProc_RnCompetitionResult(const NET_MSG_GENERIC* _pMessage);
    virtual void ShowRnCompetitionRewardTooltop(const int emTitleType, const RnCompetitionResultContentRewardObj* pReward);
};
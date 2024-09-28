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

// ��� ���� ���� ������;
class RnCompetitionResultRewardTooltip : public CBasicLineBoxSmart
{
    enum
    {
        EM_MAX_REWARD = 5,
    };
protected:
    CUIControl*     pDummyTooltip;  // ���� ����;

    CBasicTextBox*  pDummyTextTitle;  // �⿩��;
    CBasicTextBox*  pDummyItemTitle;  // �⿩��;
    CBasicTextBox*  pDummyBuffTitle;  // �⿩��;
    CBasicTextBox*  pDummyText;  // �⿩��;
    CBasicTextBox*  pDummyItem;  // �⿩��;
    CBasicTextBox*  pDummyBuff;  // �⿩��;

    CBasicTextBox*  pContributionTitleTextBox;
    CBasicTextBox*  pExpTitleTextBox;
    CBasicTextBox*  pMoneyTitleTextBox;
    CBasicTextBox*  pItemTitleTextBox;
    CBasicTextBox*  pBuffTitleTextBox;

    CBasicTextBox*  pContributionPointTextBox;  // �⿩��;
    CBasicTextBox*  pExpTextBox;                // ����ġ;
    CBasicTextBox*  pMoneyTextBox;              // ���ӸӴ�;
    CUIControl*     pItemImage[EM_MAX_REWARD];  // ������;
	CBasicTextBox*  pItemCount[EM_MAX_REWARD];  // �����۰���;
    CUIControl*     pBuffImage[EM_MAX_REWARD];  // ����;

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

// ��� �׸�;
class RnCompetitionResultObjUI
{
public:
    CUIGroup* pUI;
    int emItemType;
	int iParam;	// UI���� Ư�� ���ڰ� �ʿ��� �� ��� ( ���� Ŭ����ũ UI���� Ŭ����ũDbNum�� �����ϴµ� ��� );
};


// ��� ���� ����;
class RnCompetitionResultInfoSlot  : public CUIGroup
{
    enum
    {
        RNCOMPETITION_RESULT_REWARD_BUTTON = NO_ID + 1,
    };
    GLGaeaClient* pGaeaClient;
    const bool bMyInfoSlot;
public:
    std::vector<RnCompetitionResultObjUI> vecContentUI; // ��� ��� ���� UI; (�ؽ�Ʈ,�̹��� ��);
    std::vector<RnCompetitionResultObjUI> vecContentRewardUI; // ��� ���� �� ���� UI;

    const RnCompetitionResultContent* pContentData;  // ��� ���� ������;

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


// �� ��ư;
class RnCompetitionResultTabButton  : public CUIGroup
{
#define RNCOMPETITION_RESULT_TAB_BUTTON_MESSAGE UIMSG_USER1
    enum
    {
        RNCOMPETITION_RESULT_TAB_BUTTON = NO_ID + 1,
    };

    CBasicTextBox* pTabNameTextBox;  // �� ���� UI;
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


// �� ���� ������;
class RnCompetitionResultTabWindow  : public CUIGroup
{
protected:
    enum
    {
        RESULT_INFO_MAX_SLOT = 10,
    };
    GLGaeaClient* pGaeaClient;
public:
    std::vector<CBasicTextBox*> vecTitleNameTextBox;  // ��� ���� �ؽ�Ʈ UI;
    std::vector<RnCompetitionResultInfoSlot*> vecInfoSlot;  // ��� ���� UI;
    RnCompetitionResultInfoSlot* myInfoSlot;  // �� ��� ���� UI;

    const RnCompetitionResultTab* pTabData; // �ش� �� ������;
    int nRewardTitleNum;  // ������ ���� ����;

    std::vector<unsigned int> sortList; // ��� ������ ���� ����Ʈ;
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



// ���� ��� UI;
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

    std::vector<RnCompetitionResultTabButton*> vecTabButton;  // ���� ��� �� ��ư;
    std::vector<RnCompetitionResultTabWindow*> vecResultTab;  // ���� ��� �� ������;
    CBasicTextBox* pText_Time;  // ����/���� ������ �ð�;
    CBasicTextButton* pCloseButton;  // ��� �ݱ� ��ư;

    DWORD nCloseButtonParam;  // ��� �ݱ� ��ư Ŭ���� Ŀ���� �޽����� ���� �Ķ���� (0�� ��� ��� ����);

    RnCompetitionResultRewardTooltip* pRewardTooltip;  // ���� ���� ����;

protected:
    GLGaeaClient* pGaeaClient;
    CUIControl* pDummyBody;
    CUIControl* pBackgroundHead;
    CUIControl* pBackgroundTail;

    unsigned int nCurrentTabNum;  // ���� ���õ� ��� �� ��ȣ;

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
    void UpdateTime();  // ����/���� ������ �ð� ������Ʈ;
    void SelectResultTab( const unsigned int _nTabNum ); // ��� �� ����;
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
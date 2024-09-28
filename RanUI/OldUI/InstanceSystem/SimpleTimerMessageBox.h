#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../Util/UIWindowEx.h"
#include "../../InnerInterface.h"

class CInnerInterface;
class CBasicTextBox;
class CBasicLineBoxSmart;
class CBasicTextBox;
class CBasicTextButton;
class EngineDeviceMan;

// 간단한 타이머 UI ( 단독적인 용도보다는 부모윈도우에 자식컨트롤로 사용. 타이머 시간이벤트 메시지를 부모윈도우에 전달 );
class SimpleProgressTimer : public CUIGroup
{
public:
    SimpleProgressTimer(EngineDeviceMan* pEngineDevice);
    virtual ~SimpleProgressTimer();


    enum
    {
        // 타이머 시간 오버시 메시지
        PT_TIMEOVER = 0x01000000,
    };

    // 타이머 생성 : 로컬 x,y 좌표에 생성됨
    virtual void CreateSubControl();

    // 타이머 시작 (재호출시 초기화됨) 단위는 초;
    void Start(float fTimer = 0);

    virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl);

protected:
    CBasicLineBoxSmart* m_pGauge;	//@ 타이머 게이지;
    CBasicLineBoxSmart* m_pBlank;	//@ 타이머 게이지 배경;
    CBasicTextBox*      m_pText;	//@ 남은 시간 표시 텍스트 박스;
    float               m_fTimer;	//@ 총 시간;
    float               m_fCur;		//@ 남은 시간;

};



// 간단한 제한시간 메시지 박스;
class SimpleTimerMessageBox : public CUIWindowEx
{
public:
    SimpleTimerMessageBox(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual ~SimpleTimerMessageBox();

    enum
    {
        CONTROL_ID_NONE = CUIWindowEx::ET_CONTROL_NEXT + 1,
        CONTROL_ID_TIMER,
        CONTROL_ID_BUTTON_OK,
        CONTROL_ID_BUTTON_CANCEL,
    };

    enum
    {
        EM_MESSAGE_OK = 0,
        EM_MESSAGE_CANCEL,
        EM_MESSAGE_TIMEOVER,
    };

    virtual void CreateSubControl();
    virtual	void TranslateUIMessage(UIGUID ControlID, DWORD dwMsg);

    void SetParam(const DWORD _emResponseType, const DWORD _dwInstanceID, const DWORD _dwResponseParam1, const DWORD _dwResponseParam2);
    void SetText(const char* _szContentText, const char* _szOkText, const char* _szCancelText);
    void SetTimer(float fTimer);

    void OnOk();
    void OnCancel();
    void OnTimeOver();

private:
    void sendMessageToServer(const DWORD _dwParam1, const DWORD _dwParam2 = 0, const DWORD _dwParam3 = 0, const DWORD _dwParam4 = 0);

protected:
    SimpleProgressTimer*    m_pTimer;	// 타이머 컨트롤;
    CBasicTextBox*		    m_pText;	// 텍스트 표시 컨트롤;
    CBasicTextButton*	    m_pOk;		// 입장 버튼;
    CBasicTextButton*	    m_pCancel;	// 취소 버튼;
    CUIControl*			    m_pClock;	// 시계 이미지;

    DWORD                   emResponseType;       // 메시지를 전송할 서버;
    DWORD                   dwInstanceID;     // 메시지를 전송할 인던의 instanceKeymapID;

    // 서버에 UI결과 메시지를 서버에 보낼 때 사용자의 버튼 선택에 관한 정보는 Param3번에 들어갑니다. ;
    // 만약 숫자입력등의 결과를 서버에 보내야할때는 Param4를 사용하도록 수정하면 됨;
    DWORD                   dwResponseParam1;       // 서버에 UI결과 메시지를 서버에 보낼 때 사용할 Param2;       ( Param1 사용 );
    DWORD                   dwResponseParam2;       // 서버에 UI결과 메시지를 서버에 보낼 때 사용할 Param2;       ( Param2 사용 );

    GLGaeaClient*           m_pGaeaClient;
};



class MySimpleTimerMessageBox : public ISimpleTimerMessageBox, public SimpleTimerMessageBox
{
public:
    MySimpleTimerMessageBox( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
        :SimpleTimerMessageBox( pGaeaClient, pInterface, pEngineDevice )
    {
    };
    virtual ~MySimpleTimerMessageBox()   {};

    virtual void CreateUIWindowAndRegisterOwnership();
    virtual void ShowSimpleTimerMessageBox(const NET_MSG_GENERIC* pMsg);
    virtual void Close();
};
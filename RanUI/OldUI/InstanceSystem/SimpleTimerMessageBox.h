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

// ������ Ÿ�̸� UI ( �ܵ����� �뵵���ٴ� �θ������쿡 �ڽ���Ʈ�ѷ� ���. Ÿ�̸� �ð��̺�Ʈ �޽����� �θ������쿡 ���� );
class SimpleProgressTimer : public CUIGroup
{
public:
    SimpleProgressTimer(EngineDeviceMan* pEngineDevice);
    virtual ~SimpleProgressTimer();


    enum
    {
        // Ÿ�̸� �ð� ������ �޽���
        PT_TIMEOVER = 0x01000000,
    };

    // Ÿ�̸� ���� : ���� x,y ��ǥ�� ������
    virtual void CreateSubControl();

    // Ÿ�̸� ���� (��ȣ��� �ʱ�ȭ��) ������ ��;
    void Start(float fTimer = 0);

    virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl);

protected:
    CBasicLineBoxSmart* m_pGauge;	//@ Ÿ�̸� ������;
    CBasicLineBoxSmart* m_pBlank;	//@ Ÿ�̸� ������ ���;
    CBasicTextBox*      m_pText;	//@ ���� �ð� ǥ�� �ؽ�Ʈ �ڽ�;
    float               m_fTimer;	//@ �� �ð�;
    float               m_fCur;		//@ ���� �ð�;

};



// ������ ���ѽð� �޽��� �ڽ�;
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
    SimpleProgressTimer*    m_pTimer;	// Ÿ�̸� ��Ʈ��;
    CBasicTextBox*		    m_pText;	// �ؽ�Ʈ ǥ�� ��Ʈ��;
    CBasicTextButton*	    m_pOk;		// ���� ��ư;
    CBasicTextButton*	    m_pCancel;	// ��� ��ư;
    CUIControl*			    m_pClock;	// �ð� �̹���;

    DWORD                   emResponseType;       // �޽����� ������ ����;
    DWORD                   dwInstanceID;     // �޽����� ������ �δ��� instanceKeymapID;

    // ������ UI��� �޽����� ������ ���� �� ������� ��ư ���ÿ� ���� ������ Param3���� ���ϴ�. ;
    // ���� �����Էµ��� ����� ������ �������Ҷ��� Param4�� ����ϵ��� �����ϸ� ��;
    DWORD                   dwResponseParam1;       // ������ UI��� �޽����� ������ ���� �� ����� Param2;       ( Param1 ��� );
    DWORD                   dwResponseParam2;       // ������ UI��� �޽����� ������ ���� �� ����� Param2;       ( Param2 ��� );

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
////bjju.CaptureTheField 필드점령 참여혹은 알림을 위해 제작된 UI
//// 추후에 다른 시스템 ( 클렙데스매치, 학원대항 등에서도 적용 될수 있음 )
//
//#pragma once
//
//#include "../../../EngineLib/GUInterface/UIGroup.h"
//#include "../../InnerInterface.h"
//
//#include <list>
//#include <string>
//
//class GLGaeaClient;
//class CInnerInterface;
//class CBasicLineBoxSmart;
//
//////////////////////////////////////////////////////////////////////
////	사용자 메시지 정의
//const DWORD UIMSG_MOUSEIN_COMPETITION_ALARM_BUTTON = UIMSG_USER1;
//const DWORD UIMSG_CLICK_COMPETITION_ALARM_BUTTON   = UIMSG_USER2;
//////////////////////////////////////////////////////////////////////
//
//class CCompetitionAlarm : public CUIGroup
//{
//private:
//    enum
//    {
//        COMPETITION_ALARM_BUTTON_CAPTURETHEFIELD = NO_ID + 1,
//
//        COMPETITION_ALARM_BUTTON_CAPTURETHEFIELD_TEMP2,
//        COMPETITION_ALARM_BUTTON_CAPTURETHEFIELD_TEMP3,
//        COMPETITION_ALARM_BUTTON_CAPTURETHEFIELD_TEMP4,
//    };
//
//private:
//    struct SBUTTON : public CUIGroup
//    {
//        enum EMSTATE { EMSTATE_DEFAULT, EMSTATE_SHOW, EMSTATE_HIDE, EMSTATE_MOVING, EMSTATE_END };
//        enum { ID_BUTTON = NO_ID + 1 };
//
//        CBasicLineBoxSmart*  pLINEBOX;
//        CBasicTextBox*       pTEXTBOX;
//        EMSTATE              emSTATE;
//
//        bool                 bALARM;
//        float                fALARMTIME;
//
//        float                fMOVINGDEST;
//        float                fMOVINGSPEEDTIME;
//
//        UIRECT               rcVISIBLESHOW;
//        UIRECT               rcVISIBLEHIDE;
//        float                fVISIBLETIME;
//
//        SBUTTON(EngineDeviceMan* pEngineDevice) 
//            : CUIGroup(pEngineDevice)
//            , pLINEBOX(NULL)
//            , pTEXTBOX(NULL)
//            , emSTATE(EMSTATE_DEFAULT)
//            , bALARM(false)
//            , fALARMTIME(0.0f)
//            , fMOVINGSPEEDTIME(0.0f)
//            , fVISIBLETIME(0.0f)
//        {
//        }
//
//        void    CreateSubControl ();
//        void    Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
//        void    UpdateAlarm ( float fElapsedTime );
//        void    TranslateUIMessage( UIGUID cID, DWORD dwMsg );
//        void    SetText ( const std::string& strCaption );
//        void    SetState ( const EMSTATE emState );
//        EMSTATE GetState () { return emSTATE; }
//
//        void    SetAlarm             ( const bool bEnable );
//        void    SetShowRect          ( const UIRECT& rcRECT ) { rcVISIBLESHOW = rcRECT; }
//        void    SetHideRect          ( const UIRECT& rcRECT ) { rcVISIBLEHIDE = rcRECT; }
//        void    SetMovingDestHeight  ( const float fDest )    { fMOVINGDEST = fDest; }
//    };
//
//    typedef std::list<SBUTTON*>  BUTTONLIST;
//    typedef BUTTONLIST::iterator BUTTONLIST_ITER;
//
//public:
//    CCompetitionAlarm( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
//    virtual	~CCompetitionAlarm();
//
//public:
//    void CreateSubControl();
//
//public:
//    virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
//    virtual	void TranslateUIMessage( UIGUID cID, DWORD dwMsg );
//
//public:
//    void ShowCaptureTheField() { ShowButton (COMPETITION_ALARM_BUTTON_CAPTURETHEFIELD); }
//    void HideCaptureTheField() { HideButton (COMPETITION_ALARM_BUTTON_CAPTURETHEFIELD); }
//                //Temp
//                                        void ShowTemp ();
//                                        void HideTemp ();
//                                    //Temp
//
//public:
//    void JoinCaptureTheField();
//
//private:
//    void ShowButton ( const UIGUID cID );
//    void HideButton ( const UIGUID cID );
//
//private:
//    SBUTTON* CreateButton ( const UIGUID cID );
//    SBUTTON* GetButton ( const UIGUID cID );
//
//private:
//    const float GetButtonPostionYByIndex ( const UINT nIndex );
//
//private:
//    BUTTONLIST m_VisibleButtonList;
//    UIRECT     m_rcBaseButton;
//    UIRECT     m_rcBaseGap;
//    UIRECT     m_rcBaseShow;
//    UIRECT     m_rcBaseHide;
//    float      m_fMousePosX;
//    float      m_fMousePosY;
//
//protected:
//    GLGaeaClient*    m_pGaeaClient;
//    CInnerInterface* m_pInterface;
//};
//
//class MyCompetitionAlarm : public ICompetitionAlarm, private CCompetitionAlarm
//{
//public:
//    MyCompetitionAlarm( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
//    virtual ~MyCompetitionAlarm()   {};
//
//public:
//    virtual void CreateUIWindowAndRegisterOwnership();
//    virtual void ShowCaptureTheField();
//    virtual void HideCaptureTheField();
//
//};

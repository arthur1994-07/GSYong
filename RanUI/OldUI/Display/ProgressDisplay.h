#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../RanLogic/GLogicData.h"

#include "../../InnerInterface.h"

//-----------------------------------------------------------------------------------------------//

#include <string>
#include <list>

//-----------------------------------------------------------------------------------------------//

class CInnerInterface;
class GLGaeaClient;
class CBasicTextBox;
class CBasicLineBoxSmart;
class CSwapImage;

class CProgressDisplay : public CUIGroup
{
public:
    CProgressDisplay(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
    virtual	~CProgressDisplay ();

public:
    void                        CreateSubControl ();

public:
    virtual HRESULT             RestoreDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice );
    virtual	void	            Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );

	virtual void				ctf_hide_display();

protected:
    virtual void                SetVisibleSingle( BOOL bVisible );

public:
    void                        Show ( const UI::EMPROGRESSDISPLAY_TYPE emType, const std::string& strText );
	void                        ShowPublic ( const std::string& strText );
    void                        Begin ();
    void                        Hide ();
    void                        Update ();
	
private:
    void                        ResizeControl ( const UI::EMPROGRESSDISPLAY_TYPE emType );
    void                        SetPositionAlign ( const std::string& strControl );

private:
    void                        Begin_Activity_Complete  ();
    void                        Begin_Activity_Progress  ();
    void                        Begin_Attendance_Complete  ();

    void                        Update_Activity_Complete ( float fElapsedTime );
    void                        Update_Activity_Progress ( float fElapsedTime );
    void                        Update_Attendance_Complete ( float fElapsedTime );

    void                        ResizeControl_SchoolRecord();

private:
    void                        Begin_CTF_Start();
    void                        Begin_CTF_Stop();
    void                        Begin_CTF_Ready();
    void                        Begin_CTF_CapturedBySM();
    void                        Begin_CTF_CapturedByHA();
    void                        Begin_CTF_CapturedByBH();

    void                        Update_CTF ( float fElapsedTime );

    void                        ResizeControl_CTF();
    void                        ResizeControl_CTF_Ready();

	void                        Begin_RN_CDM_UI_Start();
	void                        Begin_RN_CDM_UI_End();
	void                        update_CDM(float fElapsedTime);
	void						ResizeControl_CDM();

	void						Begin_Public_UI_Ready();
	void                        Update_Public_UI ( float fElapsedTime );
	void                        ResizeControl_Public_UI_Ready();

private:
    // Attendance, Activity
    struct SCHOOLRECORD
    {
        CBasicLineBoxSmart* pBackground;
        CUIControl*         pSchoolBack;
        CSwapImage*         pSchoolImage;
        CBasicTextBox*      pTextBox;
        CBasicTextBox*      pTextBox_Shadow;

        SCHOOLRECORD () 
            : pBackground(NULL)
            , pSchoolBack(NULL)
            , pSchoolImage(NULL)
            , pTextBox(NULL)
            , pTextBox_Shadow(NULL)
        {
        }

        void SetVisibleSingle( BOOL bVisible );

    } m_SchoolRecord;

    // CTF 
    struct CTF
    {
        CSwapImage*  pBack;
        CSwapImage*  pSchool;
        CSwapImage*  pText;

        CBasicTextBox*  pTextBox_Shadow;

        CTF () 
            : pBack(NULL)
            , pSchool(NULL)
            , pText(NULL)
            , pTextBox_Shadow(NULL)
        {
        }

        void SetVisibleSingle( BOOL bVisible );

    } m_CTF;

	struct PUBLIC_UI
	{
		CSwapImage*  pBack;
		CSwapImage*  pSchool;
		CSwapImage*  pText;

		CBasicTextBox*  pTextBox_Shadow;

		PUBLIC_UI () 
			: pBack(NULL)
			, pSchool(NULL)
			, pText(NULL)
			, pTextBox_Shadow(NULL)
		{
		}

		void SetVisibleSingle( BOOL bVisible );
	} m_PUBLIC_UI;

private:
    struct INFOCAPS
    {
        INFOCAPS(UI::EMPROGRESSDISPLAY_TYPE _emType, std::string _strText)
            : emType(_emType)
            , strText(_strText)
            , IsBegin(false)
        {
        }

        const bool operator == (const INFOCAPS& rhs) const
        {
            if ( emType == rhs.emType )
            {
                return true;
            }

            return false;
        }

        const bool operator < (const INFOCAPS& rhs) const
        {
            if ( static_cast<UINT>(emType) < static_cast<UINT>(rhs.emType) )
            {
                return true;
            }

            return false;
        }

        UI::EMPROGRESSDISPLAY_TYPE emType;
        std::string                strText;
        bool                       IsBegin;
    };

    std::list<INFOCAPS>         m_InfoQueue;
    INFOCAPS*                   m_pCurInfo;

private:
    std::string                 m_strPosition;
private:
    float                       m_fAniTime;

protected:
    CInnerInterface*            m_pInterface;
    GLGaeaClient*               m_pGaeaClient;
};

//----------------------------------------------------------------------------------------------------//

class MyProgressDisplay : public IProgressDisplay, private CProgressDisplay
{
public:
    MyProgressDisplay( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyProgressDisplay()    {};

public:
    virtual void       CreateUIWindowAndRegisterOwnership();
    virtual void       Show ( const UI::EMPROGRESSDISPLAY_TYPE emType, const std::string& strText );
	virtual void       ShowPublic (const std::string& strText );
    virtual void       Update ();

	// TODO : #RM 695
	virtual void		ctf_hide_display();

};

#pragma once

#include "../Util/UIWindowEx.h"
#include "../../InnerInterface.h"

#include "../Util/TapSelectorEx.h"

class GLGaeaClient;

class CReferCharWearTab;
class CReferCharSkillTab;
class CBasicComboBoxRollOver;
class CBasicTextBoxEx;

class RnCharacterAdditionalWindow;

class CReferCharWindow : public CUIWindowEx , private CTapSelectorEx<DEFAULT_TAPBUTTON>
{
public:
    typedef CTapSelectorEx::BUTTON STAPBUTTON;

    enum
    {
        CHARACTER_INFO_TAP = ET_CONTROL_NEXT + 1,
        ID_CHAR_WEAR_TAB, 
        ID_CHAR_SKILL_TAB, 
        ID_CHAR_WEAR_REGION,
        ID_CHAR_SKILL_REGION,

        ID_CHAR_REFRESH,
        ID_CHAR_COMBOBOX,
        ID_CHAR_COMBOBOX_ROLLOVER,
    };

    CReferCharWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    void                CreateSubControl();
    void                DeleteUIMeshData();
	std::string			GetSelectTextName();
	void				SetVisibleReferCharacterAdditionalWindow( bool bVisible );

private:
    virtual void        TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void		SetVisibleSingle( BOOL bVisible );
	virtual void		SetFocusControl();
	virtual void		ResetFocusControl();
	virtual void		Refresh();
    CUIGroup*           GetTapWindow(UIGUID controlID);
    void                EventSelectedTap(UIGUID controlID);

private:
	void				SetSelectedTargetName();
    void                SetComboboxItem();
    GLGaeaClient*       m_pGaeaClient;
    CInnerInterface*    m_pInterface;

	CReferCharWearTab*  m_pCharacterWearTap;
    CReferCharSkillTab* m_pCharacterSkillTap;
    CUIGroup*			m_pCurrentTap;

    CBasicComboBoxRollOver* m_pComboBoxRollOver;
    CBasicTextBoxEx*    m_pComboBoxText;

	RnCharacterAdditionalWindow* m_pCharacterAdditionalAbility;
	UIRECT	m_rcBasicPos;
	UIRECT	m_rcChangePos;	// 추가 능력창 표기시 로컬사이즈
};
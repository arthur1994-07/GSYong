#pragma	once

#include "../Util/UIWindowEx.h"
#include "../Util/TapSelectorEx.h"
#include "../../InnerInterface.h"
#include "../../../RanLogicClient/Input/GLInputManager_KeySetting.h"
#include <map>

class CInnerInterface;
class CBasicLineBoxSmart;
class CBasicTextBox;
class CD3DFontPar;
class CShotCutBox;

#define MAX_DWKEY 0xEE

#define QUICK_SLOT_NUM	6
#define QUICK_SKILL_NUM	10
#define MENU_SHOTCUT_NUM 17
#define	BASIC_MENU_NUM	12
#define MAX_SETTING_KEY	( QUICK_SLOT_NUM + QUICK_SKILL_NUM + MENU_SHOTCUT_NUM)

/* 
   MenuShotCut 추가시 KEY_SETTING_SHOTCUTEDIT 숫자는 마지막 숫자를 사용하고
   만약 위치가 바뀌어야 한다면 xml 파일에서 위치만 조정함 
   RANPARAM::KeySettingToRanparam[QUICK_MENU_NUM];
   RANPARAM::BasicMenuToRanparam[BASICMENU_NUM];
   필히 수정해야됨
*/


//---------------------------------------------------------------------//
// 
// struct SKEYSETTING_TAPBUTTON : public CUIGroup
// {
// 	CBasicLineBoxSmart* ACTIVE;
// 	CBasicLineBoxSmart* INACTIVE;
// 	CBasicTextBox*      TEXTBOX;
// 	bool                ENABLE;
// 
// 	SKEYSETTING_TAPBUTTON(EngineDeviceMan* pEngineDevice)
// 		: CUIGroup(pEngineDevice)
// 		, ACTIVE(NULL)
// 		, INACTIVE(NULL)
// 		, TEXTBOX(NULL)
// 		, ENABLE(FALSE)
// 	{
// 	}
// 
// 	void CreateSubControl ( const std::string& strLabel );
// 	void SetEnable        ( const bool bEnable );
// };

//---------------------------------------------------------------------//

class CKeySettingWindow : public CUIWindowEx, private CTapSelectorEx<DEFAULT_TAPBUTTON>
{
protected:
	typedef CTapSelectorEx<DEFAULT_TAPBUTTON>::BUTTON		STAPBUTTON;
	typedef CTapSelectorEx<STAPBUTTON>  TAPSELECTOR;
	typedef std::map<UINT,EMSHORTCUTS>  MAP_SHORTCUTS;

protected:
	enum
	{
		KEY_SETTING_QUICKEDIT0 = ET_CONTROL_NEXT + 1,
		KEY_SETTING_QUICKEDIT1,
		KEY_SETTING_QUICKEDIT2,
		KEY_SETTING_QUICKEDIT3,
		KEY_SETTING_QUICKEDIT4,
		KEY_SETTING_QUICKEDIT5,
		KEY_SETTING_SKILLEDIT0,
		KEY_SETTING_SKILLEDIT1,
		KEY_SETTING_SKILLEDIT2,
		KEY_SETTING_SKILLEDIT3,
		KEY_SETTING_SKILLEDIT4,
		KEY_SETTING_SKILLEDIT5,
		KEY_SETTING_SKILLEDIT6,
		KEY_SETTING_SKILLEDIT7,
		KEY_SETTING_SKILLEDIT8,
		KEY_SETTING_SKILLEDIT9,
		KEY_SETTING_SHOTCUTEDIT0,
		KEY_SETTING_SHOTCUTEDIT1,
		KEY_SETTING_SHOTCUTEDIT2,
		KEY_SETTING_SHOTCUTEDIT3,
		KEY_SETTING_SHOTCUTEDIT4,
		KEY_SETTING_SHOTCUTEDIT5,
		KEY_SETTING_SHOTCUTEDIT6,
		KEY_SETTING_SHOTCUTEDIT7,
		KEY_SETTING_SHOTCUTEDIT8,
		KEY_SETTING_SHOTCUTEDIT9,
		KEY_SETTING_SHOTCUTEDIT10,
		KEY_SETTING_SHOTCUTEDIT11,
		KEY_SETTING_SHOTCUTEDIT12,
		KEY_SETTING_SHOTCUTEDIT13,
		KEY_SETTING_SHOTCUTEDIT14,
		KEY_SETTING_SHOTCUTEDIT15,
		KEY_SETTING_SHOTCUTEDIT16,
		KEYSETTING_DEFAULTSETTING,
		KEYSETTING_APPLY,
		KEYSETTING_OK,
		KEYSETTING_TAB_TYPE_A,
		KEYSETTING_TAB_TYPE_B,
	};

public:
	enum
	{
		TAP_TYPE_A  = KEYSETTING_TAB_TYPE_A,
		TAP_TYPE_B  = KEYSETTING_TAB_TYPE_B,

		TAP_MAX     = 2,
		TAP_DEFAULT = TAP_TYPE_A,
	};

public:
	CKeySettingWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CKeySettingWindow();

public:
	void	CreateSubControl();
	void	EDIT_END(int num);
	void	EDIT_BEGIN(int num);

	CString GetdwKeyToString(int dwKey);
	void SetShotCutKey();
	void GetShotCutKey();
	void GetDefaultKey();
	void InitData();
	void Open( const EM_CONTROLTYPE emControlType );
	CString GetDuplicationShortcutString();
	const bool IsEmpty();

private:
    bool CheckPossibleKeyString( const std::string& strKeyString );

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle ( BOOL bVisible );

private:
// 	virtual void TapEnable  ( TAPSELECTOR::BUTTON* );
// 	virtual void TapDisable ( TAPSELECTOR::BUTTON* );

private:
	CD3DFontPar*	m_pFont9;
	CShotCutBox*	m_pEditBox[MAX_SETTING_KEY];
	CShotCutBox*	m_pBeginEdit;
	CString			m_BeforeShotCut;
	
	CString m_QuickSlot[QUICK_SLOT_NUM];
	CString m_SkillSlot[QUICK_SKILL_NUM];
	CString m_MenuShotcut[MENU_SHOTCUT_NUM];

private:
	STAPBUTTON*     m_pTapButton[TAP_MAX];
	DWORD			m_dwCurrentSettingType;
	EM_CONTROLTYPE	m_emCurrentSettingType;
	EM_CONTROLTYPE	m_emSaveSettingType;
	MAP_SHORTCUTS	m_mGUIDToShorcuts;

private:
    CBasicTextBox* m_pAtteckKey;


};

class MyKeySettingWindow : public IKeySettingWindow, private CKeySettingWindow
{
public:
    MyKeySettingWindow( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
    virtual ~MyKeySettingWindow()    {};

public:
    virtual void CreateUIWindowAndRegisterOwnership();

    virtual UI::String GetdwKeyToString( int dwKey );
    virtual void GetShotCutKey();
};


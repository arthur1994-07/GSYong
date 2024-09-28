


#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_RNBUTTON_CLICK = UIMSG_USER1;
const DWORD UIMSG_RNBUTTON_EVENT_PRESSED = UIMSG_USER2;

////////////////////////////////////////////////////////////////////
class CBasicLineBoxSmart;
class CBasicTextBox;



class RnButton: public CUIGroup
{
public:
	enum STATE{DEFAULT,MOUSEOVER,MOUSECLICK,INACTIVE,COUNT};
    enum
    {
        WND_DEFAULT_ID = NO_ID + 1,
        WND_MOUSEOVER_ID,
        WND_MOUSECLICK_ID,
        WND_INACTIVE_ID,
    };
public:
	struct CreateArg
	{
        CreateArg();

//		std::string  key;
		std::string  defaultTextureKey;
		std::string  mouseOverTextureKey;
		std::string  mouseClickTextureKey;
		std::string  inactiveTextureKey;

		
		std::string  imageKey;

//		std::string  textKey;
		std::string  text;

		CD3DFontPar* pFont;
        DWORD dwColor;
        int nTextAlign;
	};
public:
	RnButton(EngineDeviceMan* pEngineDevice);
	virtual void CreateSubControl(const CreateArg& arg);

	virtual void SetEnable(bool isEnable);
    
	bool IsState( STATE state );
	void SetText( const CString& strText, const D3DCOLOR& TextColor );
	void SetOneLineText( const CString& strText, const D3DCOLOR& TextColor );
	std::string GetText();

	void SetPressedEventTime(float time) { m_PressedEventTime = time; }
private:
	virtual void Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );
protected:
	void SetState(STATE state);

public:
	inline bool IsEnable() { return m_isEnable; }

private:

	float m_PressedEventTime;
	float m_PressedEventTimeAccumulated;
	bool m_isEnable;
	bool m_isPressed;

	CBasicLineBoxSmart* m_pButtonImage[COUNT];

	CBasicTextBox*		m_pText;
	CUIControl*         m_pAboveImage;
};
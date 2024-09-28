#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CBasicTextBox;
class CUIMultiEditBox;
class GLGaeaClient;

class CClubAnnounce : public CUIGroup
{
protected:
	enum
	{
		CLUB_ANNOUNCE_TEXTBOX = NO_ID + 1,
		CLUB_ANNOUNCE_EDITBOX_L01,
		CLUB_ANNOUNCE_EDITBOX_L02,
		CLUB_ANNOUNCE_EDITBOX_L03,
		CLUB_ANNOUNCE_EDITBOX_L04,
		CLUB_ANNOUNCE_EDITBOX_L05,
		CLUB_ANNOUNCE_EDITBOX_L06,
		CLUB_ANNOUNCE_EDITBOX_L07,
		CLUB_ANNOUNCE_EDITBOX_L08,
		CLUB_ANNOUNCE_EDITBOX_L09,
		CLUB_ANNOUNCE_EDITBOX_L10
	};

public:
	CClubAnnounce(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice);
	virtual	~CClubAnnounce();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void CreateSubControl();

public:
	virtual	void SetVisibleSingle ( BOOL bVisible );

public:
	void SetAnnouncement();
	void BeginEdit();
	void EndEdit( BOOL bOK );
	void ClearText();

private:
	CBasicTextBox *	m_pTextBox;
	CUIMultiEditBox * m_pMultiEditBox;
};
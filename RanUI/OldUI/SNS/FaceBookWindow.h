
// bjju.sns Ãß°¡

#pragma	once

#include "../Util/WebWindowBase.h"

class GLGaeaClient;
class CBasicTextBox;
class CBasicTextButton;
class CBasicButton;
class CBasicLineBox;

class CFaceBookWindow : public CWebWindowBase
{
protected:
	enum
	{
		FACEBOOK_WINDOW_NONE = ET_CONTROL_NEXT,
	};

public:
	enum PAGETYPE
	{
		PAGETYPE_NONE,
		PAGETYPE_AUTH,
		PAGETYPE_UNAUTH,
		PAGETYPE_FRIEND_ADD,
	};

public:
	CFaceBookWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~CFaceBookWindow ();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual	void SetVisibleSingle ( BOOL bVisible );

public:
	void		 SetPageType( const PAGETYPE emType );
	void		 SetAddFriendURL( const std::string& strURL ) { m_strAddFriendURL = strURL; }

public:
	BOOL		 BeginAuth();
	BOOL		 EndAuth();

	BOOL		 BeginUnAuth();
	BOOL		 EndUnAuth();

	BOOL		 BeginFriendAdd();
	BOOL		 EndFriendAdd();

private:
	std::string m_strAddFriendURL;
	PAGETYPE	m_emType;

private:
    GLGaeaClient* m_pGaeaClient;
};
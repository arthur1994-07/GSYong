
#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

#include "../../../EngineLib/G-Logic/GLDefine.h"

class CBasicLineBox;
class CBasicLineBoxSmart;
class CBasicTextBox; 
class CCheckBox;
class CItemImageEx;
class CSwapImage;
class GLGaeaClient;

struct SAPOSTHISTORY;

class CPostBoxHistoryItem : public CUIGroup
{

protected:
	enum
	{
		POSTBOX_PAGE_RECEIVE_ITEM_NONE = NO_ID,
	};

public:
	enum
	{
		STATE_CARD  = 0,
		STATE_ITEM  = 1,
		STATE_MONEY = 2,
		STATE_MAX   = 3,
	};

public:
	CPostBoxHistoryItem(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice);
	virtual	~CPostBoxHistoryItem();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	virtual void		 Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void		 TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	CSwapImage*			 CreateSwapImage			( const char* szControl, UIGUID ControlID = NO_ID );
	CBasicTextBox*		 CreateTextBox				( const char* szControl, CD3DFontPar* pFont, int nAlign, const UIGUID& ControlID = NO_ID );
	CBasicLineBox*		 CreateBackgroundControl	( const char* szControl );

public:
	void				 CreateSubControl();

public:
	void				 Reset ();

public:
	void				 SetHistory ( SAPOSTHISTORY* pHistory, const BOOL bForcing=FALSE );

private:
	CSwapImage*			 m_pImage_Type;
	CSwapImage*			 m_pImage_State[ STATE_MAX ];

	CBasicTextBox*		 m_pText_Date;
	CBasicTextBox*		 m_pText_Name;
	CBasicTextBox*		 m_pText_Info;

private:
	SAPOSTHISTORY*		 m_pHistory;

};

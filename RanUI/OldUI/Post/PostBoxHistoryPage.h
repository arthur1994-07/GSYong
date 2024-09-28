
#pragma	once

#include "PostBoxPage.h"

class  CPostBoxHistoryList;
class  CBasicTextButton;
class  CBasicTextBox; 
class  CSwapImage;

class CPostBoxHistoryPage : public CPostBoxPage
{

protected:
	enum
	{
		POSTBOX_PAGE_HISTORY_NONE = POSTBOX_PAGE_NEXT,

        //POSTBOX_PAGE_HISTORY_LIST,
        POSTBOX_PAGE_SEND_HISTORY_LIST,
        POSTBOX_PAGE_RECV_HISTORY_LIST,
		POSTBOX_PAGE_HISTORY_IMAGE_ARROW_LEFT,
		POSTBOX_PAGE_HISTORY_IMAGE_ARROW_RIGHT,
        POSTBOX_PAGE_HISTORY_BUTTON_RECV_VIEW,
        POSTBOX_PAGE_HISTORY_BUTTON_SEND_VIEW,
	};

public:
	CPostBoxHistoryPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CPostBoxHistoryPage ();

protected:
	virtual void	CreateSubControl ();

protected:
	virtual void	BeginSubControl ();
	virtual void	OpenSubControl  ();
	virtual void	EndSubControl   ();

public:
	virtual void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void			UpdateVariableText	  ();
	void			UpdateVariableControl ();

public:
	void			UpdateList ();

public:
    void            ResetRequestState();

protected:
    void            RequestCurrentState ();

protected:
    void            ChangeActivePostBoxList ( CPostBoxHistoryList* pNewList );

private:
    CPostBoxHistoryList* m_pListActive;
    //CPostBoxHistoryList* m_pList;
    CPostBoxHistoryList* m_pSendList;
    CPostBoxHistoryList* m_pRecvList;

	CBasicTextBox*		 m_pText_Static_Describe;
	CBasicTextBox*		 m_pText_Page;

	CSwapImage*			 m_pImage_Arrow_Left;
	CSwapImage*			 m_pImage_Arrow_Right;

    CBasicTextButton*	 m_pButton_History_Send_View;
    CBasicTextButton*    m_pButton_History_Recv_View;

    //BOOL                 m_bStateRequested;
    BOOL                 m_bSendStateRequested;
    BOOL                 m_bRecvStateRequested;
};


#pragma	once

#include "PostBoxPage.h"

class  CInnerInterface;
class  CBasicLineBoxSmart;
class  CBasicTextBox; 
class  CBasicTextButton; 
class  CItemImageEx;
class  CSwapImage;
struct SAPOSTCLIENT;

class CPostBoxReadPage : public CPostBoxPage
{

protected:
	enum
	{
		POSTBOX_PAGE_READ_NONE = POSTBOX_PAGE_NEXT,

		POSTBOX_PAGE_READ_ITEM,

		POSTBOX_PAGE_READ_BUTTON_TAKE,
		POSTBOX_PAGE_READ_BUTTON_RETURN,
		POSTBOX_PAGE_READ_BUTTON_DELETE,
		POSTBOX_PAGE_READ_BUTTON_REPLY,
		POSTBOX_PAGE_READ_BUTTON_CLOSE,

		POSTBOX_PAGE_READ_NEXT,
	};

public:
	static const int	nMONEYUNIT					= 3;

	static const DWORD	nDESIGN_CARD_NUM_WIDTH		= 4;
	static const DWORD	nDESIGN_CARD_SIZE_WIDTH		= 256;
	static const DWORD	nDESIGN_CARD_SIZE_HEIGHT	= 156; 

	static const DWORD	cTITLE_COLOR				= D3DCOLOR_ARGB(255,197,254,0); 

public:
	CPostBoxReadPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CPostBoxReadPage ();

protected:
	virtual void  CreateSubControl ();
	virtual void  OpenSubControl   ();
	virtual void  CloseSubControl  ();

public:
	virtual void  Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void  TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	void		  Reset ();
	void		  ResetDesignCard ();

public:
	virtual void  SetPost ( SAPOSTCLIENT* pPost ) { m_pPost = pPost; }
	SAPOSTCLIENT* GetPost ()					  { return m_pPost;  }

public:
	void		  ReqDelete   ( bool bConfirmAttach );
	void		  ReqReturn   ();
	void		  ReqTakeItem ();

protected:
	virtual void  UpdateVariableControl	();
	virtual void  UpdateVariableText	();

private:
	CItemImageEx*					 m_pItem;

	CSwapImage*						 m_pImage_Money;

	CBasicTextBox*					 m_pText_Static_Date;
	CBasicTextBox*					 m_pText_Static_From;
	CBasicTextBox*					 m_pText_Static_NoneItem;
	CBasicTextBox*					 m_pText_Static_ReceiveItem;
	CBasicTextBox*					 m_pText_Static_ReceiveMoney;
	CBasicTextBox*					 m_pText_Static_BillingMoney;
	CBasicTextBox*					 m_pText_Static_AttachItem;

	CBasicTextBox*					 m_pText_From;
	CBasicTextBox*					 m_pText_Date;
	CBasicTextBox*					 m_pText_Subject;
	CBasicTextBox*					 m_pText_Contents;
	CBasicTextBox*					 m_pText_Money;

	CBasicTextButton*				 m_pButton_Close;

	CBasicLineBoxSmart*				 m_pLineBox_Contents;
	CBasicLineBoxSmart*				 m_pLineBox_Item;
	CBasicLineBoxSmart*				 m_pLineBox_Item_Billing;
	CBasicLineBoxSmart*				 m_pLineBox_Money;

protected:
	CBasicTextBox*					 m_pText_Static_MyMoney;

	CBasicTextBox*					 m_pText_MyMoney;
	CBasicTextBox*					 m_pText_LeftTime;

	CBasicTextButton*				 m_pButton_Take;
	CBasicTextButton*				 m_pButton_Return;
	CBasicTextButton*				 m_pButton_Delete;
	CBasicTextButton*				 m_pButton_Reply;

	CBasicLineBoxSmart*				 m_pLineBox_MyMoney;

protected:
	SAPOSTCLIENT*					 m_pPost;
};

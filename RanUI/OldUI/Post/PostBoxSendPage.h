
#pragma	once

#include "PostBoxPage.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"

#include "../../../EngineLib/G-Logic/GLDefine.h"

class  CInnerInterface;
class  CBasicLineBox;
class  CBasicLineBoxSmart;
class  CBasicButton;
class  CBasicTextBox;
class  CBasicTextButton;
class  CBasicComboBoxRollOver;
class  CSmartComboBoxBasic;
class  CUIEditBox;
class  CUIMultiEditBox;
class  CUIMultiEditBoxSmart;
class  CItemImageEx;
class  CSwapImage;
struct SAPOST;
struct SAPOSTCLIENT;

class CPostBoxSendPage : public CPostBoxPage
{

protected:
	enum
	{
		POSTBOX_PAGE_SEND_NONE = POSTBOX_PAGE_NEXT,

		POSTBOX_PAGE_SEND_IMAGE_SENDMONEY,
		POSTBOX_PAGE_SEND_IMAGE_BILLING,

		POSTBOX_PAGE_SEND_LINEBOX_ITEM,

		POSTBOX_PAGE_SEND_STATIC_TEXT_SENDMONEY,
		POSTBOX_PAGE_SEND_STATIC_TEXT_BILLING,

		POSTBOX_PAGE_SEND_COMBOBOX_DESIGN_OPEN,
		POSTBOX_PAGE_SEND_COMBOBOX_ROLLOVER_DESIGN,

		POSTBOX_PAGE_SEND_EDITBOX_TO,
		POSTBOX_PAGE_SEND_EDITBOX_SUBJECT,
		POSTBOX_PAGE_SEND_EDITBOX_SENDMONEY,
		POSTBOX_PAGE_SEND_EDITBOX_BILLING,

		POSTBOX_PAGE_SEND_MULTIEDITBOX_CONTENTS,
		POSTBOX_PAGE_SEND_MULTIEDITBOX_CONTENTS_L01,
		POSTBOX_PAGE_SEND_MULTIEDITBOX_CONTENTS_L02,
		POSTBOX_PAGE_SEND_MULTIEDITBOX_CONTENTS_L03,
		POSTBOX_PAGE_SEND_MULTIEDITBOX_CONTENTS_L04,
		POSTBOX_PAGE_SEND_MULTIEDITBOX_CONTENTS_L05,
		POSTBOX_PAGE_SEND_MULTIEDITBOX_CONTENTS_L06,
		POSTBOX_PAGE_SEND_MULTIEDITBOX_CONTENTS_L07,
		POSTBOX_PAGE_SEND_MULTIEDITBOX_CONTENTS_L08,
		POSTBOX_PAGE_SEND_MULTIEDITBOX_CONTENTS_L09,
		POSTBOX_PAGE_SEND_MULTIEDITBOX_CONTENTS_L10,

		POSTBOX_PAGE_SEND_BUTTON_FIND,
		POSTBOX_PAGE_SEND_BUTTON_PREVIEW,
		POSTBOX_PAGE_SEND_BUTTON_SEND,
	};

protected:
	enum EMREQMONEYTYPE
	{
		EMREQMONEYTYPE_NONE		= 0,
		EMREQMONEYTYPE_SEND		= 1,
		EMREQMONEYTYPE_BILLING	= 2,
	};

public:
	static const D3DCOLOR	cCOLOR_TEXT_ENABLE		 = NS_UITEXTCOLOR::WHITE;
	static const D3DCOLOR	cCOLOR_TEXT_DISABLE		 = NS_UITEXTCOLOR::DARKGRAY;

	static const int		nLIMIT_STRING_TO;
	static const int		nLIMIT_STRING_SUBJECT;
	static const int		nLIMIT_STRING_CONTENTS;
	static const int		nLIMIT_STRING_CONTENTS_LINE		 = 23;
	static const int		nLIMIT_STRING_CONTENTS_LINE_LAST = 23;
	static const int		nLIMIT_STRING_SENDMONEY			 = 11;
	static const int		nLIMIT_STRING_BILLING			 = 10;

	static const int		nMONEYUNIT						 = 3;

public:
	CPostBoxSendPage(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CPostBoxSendPage ();

protected:
	virtual void		CreateSubControl();
	virtual	void		OpenSubControl  ();
	virtual	void		CloseSubControl ();
	virtual void		EndSubControl   ();

public:
	virtual void		Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void		TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	virtual void		TranslateEditBoxMsg			  ( CUIEditBox*				pEditBox,			DWORD dwMsg );
	virtual void		TranslateMultiEditBoxMsg	  ( CUIMultiEditBox*		pMultiEditBox,		DWORD dwMsg );
	virtual void		TranslateMultiEditBoxSmartMsg ( CUIMultiEditBoxSmart*	pMultiEditBoxSmart,	DWORD dwMsg );

protected:
	CBasicLineBoxSmart* CreateInputLineBox		   ( const char* szControl, UIGUID ControlID = NO_ID );
	CBasicLineBoxSmart* CreateInputLineBox_Disable ( const char* szControl, UIGUID ControlID = NO_ID );

public:
	void				Reset ();
	void				ResetDesignCombo ();
	void				ResetDesignCard	 ();

public:
	void				SetMoneyReqType ( const EMREQMONEYTYPE emType );
	void				SetEditBoxTo    ( const CString& strName );

public:
	const BOOL			ReqSendPost ();

public:
	void				MakeSendPost	( SAPOST&		OutPost		  );
	void				MakePreviewPost ( SAPOSTCLIENT& OutPostClient );

protected:
	void				UpdateVariableText		();
	void				UpdateVariableControl	();
	void				UpdateComboBox			();
	void				UpdateEditBoxTabShift	();

protected:
	CItemImageEx*					 m_pItem;

	CSwapImage*						 m_pImage_SendMoney;
	CSwapImage*						 m_pImage_Billing;

	CBasicTextBox*					 m_pText_Static_To;
	CBasicTextBox*					 m_pText_Static_Design;
	CBasicTextBox*					 m_pText_Static_Subject;
	CBasicTextBox*					 m_pText_Static_Contents;
	CBasicTextBox*					 m_pText_Static_SendMoney;
	CBasicTextBox*					 m_pText_Static_MyMoney_Caption;
	CBasicTextBox*					 m_pText_Static_Item;
	CBasicTextBox*					 m_pText_Static_Billing; // 대금청구

	CBasicTextBox*					 m_pText_Bytes;
	CBasicTextBox*					 m_pText_MyMoney_Money;
	CBasicTextBox*					 m_pText_Tax; // 이용금액

	CBasicLineBoxSmart*				 m_pLineBox_To;
	CBasicLineBoxSmart*				 m_pLineBox_Subject;
	CBasicLineBoxSmart*				 m_pLineBox_Contents;
	CBasicLineBoxSmart*				 m_pLineBox_SendMoney;
	CBasicLineBoxSmart*				 m_pLineBox_SendMoney_Disable;
	CBasicLineBoxSmart*				 m_pLineBox_Billing;
	CBasicLineBoxSmart*				 m_pLineBox_Billing_Disable;
	CBasicLineBoxSmart*				 m_pLineBox_Item;

	CSmartComboBoxBasic*			 m_pComboBox_Design;	
	CBasicComboBoxRollOver*			 m_pComboBox_RollOver_Design;

	CUIEditBox*						 m_pEditBox_To;
	CUIEditBox*						 m_pEditBox_Subject;
	CUIEditBox*						 m_pEditBox_SendMoney;
	CUIEditBox*						 m_pEditBox_Billing;

	CUIMultiEditBoxSmart*			 m_pMultiEditBox_Contents;

	CBasicButton*					 m_pButton_Find;
	CUIControl*						 m_pButton_Find_Down;
	CBasicTextButton*				 m_pButton_Preview;
	CBasicTextButton*				 m_pButton_Send;

protected:
	SNATIVEID						 m_sItemID;
	EMREQMONEYTYPE					 m_emMoneyReq;

	UIGUID							 m_RollOverID;
	BOOL							 m_bRollOverLBUP;
	DWORD							 m_dwDesign;

	CUIEditBox*						 m_pLastEditBox;
};

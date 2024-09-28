
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
class CInnerInterface;
struct SAPOSTCLIENT;

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_POSTBOX_RECEIVE_ITEM_READ = UIMSG_USER1;
////////////////////////////////////////////////////////////////////

class CPostBoxReceiveItem : public CUIGroup
{

protected:
	enum
	{
		POSTBOX_PAGE_RECEIVE_ITEM_NONE = NO_ID,

		POSTBOX_PAGE_RECEIVE_ITEM_BOX_DEFAULT,
		POSTBOX_PAGE_RECEIVE_ITEM_BOX_EMPTY,

		POSTBOX_PAGE_RECEIVE_ITEM_BOXLINE_SELECTED,
		POSTBOX_PAGE_RECEIVE_ITEM_BOXLINE_NEW,

		POSTBOX_PAGE_RECEIVE_ITEM_CHECKBOX,
	};

public:
	enum EMRECEIVEITEMBOXTYPE
	{
		EMRECEIVEITEMBOXTYPE_DEFAULT	= 0,
		EMRECEIVEITEMBOXTYPE_EMPTY		= 1,
		EMRECEIVEITEMBOXTYPE_SELECTED	= 2,
		EMRECEIVEITEMBOXTYPE_NEW		= 3,
	};

	enum
	{
		BOX_DEFAULT  = 0,
		BOX_EMPTY	 = 1,

		BOX_MAX		 = 2,
	};

	enum
	{
		BOXLINE_SELECTED  = 0,
		BOXLINE_NEW		  = 1,

		BOXLINE_MAX		  = 2,
	};

public:
	static const DWORD COLOR_ENABLE			 = D3DCOLOR_ARGB(255, 255, 255, 255);
	static const DWORD COLOR_DISABLE		 = D3DCOLOR_ARGB(255, 150, 150, 150);
	static const DWORD COLOR_SUBJECT_DEFAULT = D3DCOLOR_ARGB(255, 220, 220, 220);

	static const DWORD nBOX_SIZE_WIDTH	= 234;
	static const DWORD nBOX_SIZE_HEIGHT	= 47;

public:
	CPostBoxReceiveItem(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CPostBoxReceiveItem ();

protected:
    CInnerInterface* m_pInterface;
    GLGaeaClient* m_pGaeaClient;

public:
	virtual void		 Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void		 TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	CItemImageEx*		 CreateItemImage			( const char* szControl, UIGUID ControlID = NO_ID );
	CSwapImage*			 CreateSwapImage			( const char* szControl, UIGUID ControlID = NO_ID );
	CBasicTextBox*		 CreateTextBox				( const char* szControl, CD3DFontPar* pFont, int nAlign, const UIGUID& ControlID = NO_ID );
	CBasicLineBox*		 CreateBackgroundControl	( const char* szControl );

public:
	void				 CreateSubControl();

public:
	void				 Reset ();

public:
	void				 SetPost ( SAPOSTCLIENT* pPostData, const BOOL bForcing=FALSE );
	SAPOSTCLIENT*		 GetPost () { return m_PostData; }

	void				 SetSelected ( const BOOL bEnable );

public:
	const BOOL			 IsChecked  ();
	const BOOL			 IsSelected ();

protected:
	void				 UpdateBox			   ();
	void				 UpdateVariableControl ();

protected:
	void				 SetItemImage ( const SNATIVEID& sNativeID );

private:
	CBasicLineBoxSmart*	 m_pBox		[BOX_MAX];
	CBasicLineBoxSmart*	 m_pBoxLine	[BOXLINE_MAX];
	CSwapImage*			 m_pBoxFlag;
	CSwapImage*			 m_pBoxAlpha;

	CCheckBox*			 m_pCheck;

	CItemImageEx*		 m_pItem;
	CSwapImage*			 m_pItemException;

	CBasicTextBox*		 m_pText_Date;
	CBasicTextBox*		 m_pText_Subject;
	CBasicTextBox*		 m_pText_From;
	CBasicTextBox*		 m_pText_LeftTime;

	CSwapImage*			 m_pType;

private:
	EMRECEIVEITEMBOXTYPE m_emBoxType;

private:
	SAPOSTCLIENT*		 m_PostData;
	
};

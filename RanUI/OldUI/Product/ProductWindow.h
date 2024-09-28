#pragma once

#include "../Util/UIWindowEx.h"
#include "../../InnerInterface.h"

class CProductType;
class CProductList;
class CProductGoods;
class CBasicComboBoxRollOver;
class CSmartComboBoxBasic;
class CCheckBox;
class CProductButton;
class RnButton;
class CMessageBox;

class CProductWindow : public CUIWindowEx
{
protected:
    enum
    {
		//hsshin 오타수정
        ID_PRODUCT_TYPE = ET_CONTROL_NEXT + 1,
        ID_PRODUCT_LIST,
        ID_PRODUCT_GOODS,
        ID_PRODUCT_FILTERING_COMBOBOX_OPEN,         // 콤보박스
        ID_PRODUCT_FILTERING_COMBOBOX_ROLLOVER,     //
        ID_PRODUCT_FILTERING_CHECKBOX,              // 생산가능 물품만 보기
        ID_PRODUCT_PRODUCTION_BUTTON,               // 생산하기 버튼

		//hsshin 숙련도추가 관련 UI추가
		ID_PRODUCT_SHOW_SKILL_CHECKBOX,				//숙련도정보 보기 버튼
		ID_PRODUCT_HELP_BUTTON,						//도움말 버튼
		ID_PRODUCT_STUDY_MSGBOX,					//제작분야 학습 확인 메시지박스
    };

	enum
	{
		MESSAGEBOX_NOT_UST = 0,
		MESSAGEBOX_CONFIRM_PRODUCT,
	};

public:
    CProductWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

	void                    CreateSubControl();
    virtual	void            Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void            TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );
	virtual void			TranslateMessageBox ( unsigned int nIdentity, DWORD dwMsg );
	virtual	void		    SetVisibleSingle( BOOL bVisible );
	virtual void		    Refresh();

	virtual void			DialogMaterialConfirm();

private:
	

protected:
    void                    MatchingEum( int nIndex );
	
    void                    CreateUIWindowAndRegisterOwnership();
    void                    SetSeletedTabID( UIGUID ControlID );
    void                    SetItemMix( SProductRecipe* pProductRecipe );
    void                    SetProducting( bool bProducting );

	CInnerInterface*        m_pInterface;
    GLGaeaClient*           m_pGaeaClient;

	CProductGoods*          m_pGoods;
    CProductList*           m_pList;
    CProductType*           m_pType;
    CBasicTextBox*          m_pTypeListText;
    CBasicTextBox*          m_pGoodsName;
    CBasicComboBoxRollOver* m_pClassFirterComboBoxRollOver;
    CSmartComboBoxBasic*    m_pClassFirterComboBox; 
    CCheckBox*              m_pShowProductAbleCheckBox;
	bool				    m_bIsShowProductAbleCheckBoxOn;
    RnButton*               m_pProductionButton;
    BYTE                    m_eFindClass;
    bool				    m_bProducting;

	//hsshin 숙련도 시스템 추가
	CCheckBox*			    m_pShowSkillCheckBox;
	bool				    m_bIsShowSkillCheckBoxOn;
    CBasicTextBox*          m_pStudyPointTextBox;
	CBasicLineBoxSmart*	    m_pHelpBox;
	CBasicTextBox*		    m_pHelpText_A;
	CBasicTextBox*		    m_pHelpText_B;
	CBasicTextBox*		    m_pHelpText_C;
	CMessageBox*		    m_pStudyMsgBox;

    //최초 1회 제조창을 열면 데이터를 서버로 부터 받아와서 초기화 한다.
    bool                    m_bDataInit;
};

class MyProductWindow : public IProductWindow, private CProductWindow
{
public:
    MyProductWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );

public:
    virtual void            CreateUIWindowAndRegisterOwnership();
    virtual void            SetSeletedTabID( UIGUID ControlID );
    virtual void            SetItemMix( SProductRecipe* pProductRecipe );
    virtual void            SetProducting( bool bProducting );
	virtual void			DialogMaterialConfirm();
};
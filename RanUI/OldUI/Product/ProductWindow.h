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
		//hsshin ��Ÿ����
        ID_PRODUCT_TYPE = ET_CONTROL_NEXT + 1,
        ID_PRODUCT_LIST,
        ID_PRODUCT_GOODS,
        ID_PRODUCT_FILTERING_COMBOBOX_OPEN,         // �޺��ڽ�
        ID_PRODUCT_FILTERING_COMBOBOX_ROLLOVER,     //
        ID_PRODUCT_FILTERING_CHECKBOX,              // ���갡�� ��ǰ�� ����
        ID_PRODUCT_PRODUCTION_BUTTON,               // �����ϱ� ��ư

		//hsshin ���õ��߰� ���� UI�߰�
		ID_PRODUCT_SHOW_SKILL_CHECKBOX,				//���õ����� ���� ��ư
		ID_PRODUCT_HELP_BUTTON,						//���� ��ư
		ID_PRODUCT_STUDY_MSGBOX,					//���ۺо� �н� Ȯ�� �޽����ڽ�
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

	//hsshin ���õ� �ý��� �߰�
	CCheckBox*			    m_pShowSkillCheckBox;
	bool				    m_bIsShowSkillCheckBoxOn;
    CBasicTextBox*          m_pStudyPointTextBox;
	CBasicLineBoxSmart*	    m_pHelpBox;
	CBasicTextBox*		    m_pHelpText_A;
	CBasicTextBox*		    m_pHelpText_B;
	CBasicTextBox*		    m_pHelpText_C;
	CMessageBox*		    m_pStudyMsgBox;

    //���� 1ȸ ����â�� ���� �����͸� ������ ���� �޾ƿͼ� �ʱ�ȭ �Ѵ�.
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
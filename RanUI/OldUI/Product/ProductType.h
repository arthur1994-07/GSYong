#pragma once
#include "../Util/TapSelectorEx.h"
#include "../../InnerInterface.h"
#include "ProductTypeTapButton.h"

class GLGaeaClient;

class CProductType : public CBasicLineBoxSmart, public CTapSelectorEx<CProductTypeTapButton>
{
private:
    enum
    {
        ID_PRODUCT_TYPE_TAB_HEADGEAR = Product::ETypeHeadGear + 1,
        ID_PRODUCT_TYPE_TAB_UPPER,
        ID_PRODUCT_TYPE_TAB_LOWER,
        ID_PRODUCT_TYPE_TAB_HAND,
        ID_PRODUCT_TYPE_TAB_FOOT,
        ID_PRODUCT_TYPE_TAB_HANDHELD,
        ID_PRODUCT_TYPE_TAB_NECKEARRING,
        ID_PRODUCT_TYPE_TAB_WRISTWAIST,
        ID_PRODUCT_TYPE_TAB_FINGERACCESSORIES,
        ID_PRODUCT_TYPE_TAB_ETC,
        ID_PRODUCT_TYPE_TAB_RANDOMBOX,
        ID_PRODUCT_TYPE_TAB_COSTUME,
        ID_PRODUCT_TYPE_TAB_ALL,
    };
public:
    CProductType( GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice, CInnerInterface* pInterface );

	void                    CreateSubControl();
    virtual void            TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

	void                    EventSelectedTap( UIGUID controlID );

	void				    RefreshTapButton();
	void				    SetShowSkill( const bool bState );
protected:
    const BYTE              GetStudyState( WORD wProductType );                         //�ش� ���ۺо��� �н����¸� enum(E_STUDY)���� ��ȯ

	CInnerInterface*        m_pInterface;
    CUIGroup*			    m_pCurrentTap;
    GLGaeaClient*           m_pGaeaClient;

	//hsshin ���õ������� custom��ư ����
	CProductTypeTapButton*	m_pAllTap;
	CProductTypeTapButton*	m_pHeadGearTap;
	CProductTypeTapButton*	m_pUpperTap;
	CProductTypeTapButton*	m_pLowerTap;
	CProductTypeTapButton*	m_pHandTap;
	CProductTypeTapButton*	m_pFootTap;
	CProductTypeTapButton*	m_pHandHeldTap;
	CProductTypeTapButton*	m_pNeck_EarringTap;
	CProductTypeTapButton*	m_pWrist_WaistBandTap;
	CProductTypeTapButton*	m_pFinger_AccessoriesTap;
	CProductTypeTapButton*	m_pEtcTap;
	CProductTypeTapButton*	m_pRandomBoxTap;
	CProductTypeTapButton*	m_pCostumeTap;
	
	UIGUID					m_iPreviousSelectedTapID;		//���õǸ� �ȵǴ� �ǹ�ư Ŭ���� �Ǽ����� �ѹ��ϱ� ���� ����

};
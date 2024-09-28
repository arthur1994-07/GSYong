#pragma once

#include "../Util/TapSelectorEx.h"
#include "../../../RanLogic/Product/GLProduct.h"

class CBasicTextBox;

//hsshin ���ۺо� �н��޽���â�� �������� �޽��� �߰�
namespace ProductTypeMSG
{
    const DWORD UI_MSG_PRODUCT_STUDY = UIMSG_USER1;
}

//hsshin ���� �о߼��� : �ǹ�ư�� ���õ� ������ �н������ �߰� (2012.1.23)
class CProductTypeTapButton : public TAPBUTTON_RNBUTTON
{
public:
    enum
    {
        E_STUDY_NONE	= 0,		                                                //������ ���� (�н��� ���þ��� �κп��� ��� ex TypeTotal)
        E_STUDY_PUBLIC,			                                                    //���� ���ۺо� ������
        E_STUDY_NEED,				                                                //�н��ʿ� ������
        E_STUDY_STUDIED,			                                                //�н��Ϸ� ������
        E_STUDY_DISABLE,			                                                //�н� ��Ȱ��ȭ ������
        E_STUDY_UNABLE,				                                                //�н� �Ұ� ������ (ex ������ ���۱��� ������)
    };
    enum
    {
        E_CID_STUDY_BUTTON = RnButton::WND_INACTIVE_ID + 1,                         //��Ʈ�� ID�� �ߺ��� ���ϱ� ����
    };
protected:
	BYTE					m_eStudyState;					                        //�н���������
	int						m_iSkill;						                        //���õ� ����
	CUIControl*				m_pStudyIcon_Study_Public;		                        //�н� ���ʿ� ������
	CUIControl*				m_pStudyIcon_Study_Need;		                        //�н� �ʿ� ������
    CUIControl*				m_pStudyIcon_Study_Need_Cross;	                        //�н� �ʿ� ������(���� ���ڰ�)
	CUIControl*				m_pStudyIcon_Study_Studied;		                        //�н� �Ϸ� ������
	CUIControl*				m_pStudyIcon_Study_Disable;		                        //�н� ����Ʈ ���� ������	(������ icon�� ������ �߰� �� ���ɼ������� �־����)
	CUIControl*				m_pStudyIcon_Study_Unable;		                        //���ۺо� �Ұ��� ������

	CBasicLineBoxSmart*		m_pSkillTooltip_Title;			                        //���õ� ����
	CBasicTextBox*			m_pSkillTooltip;				                        //���õ� ����
public:
	CProductTypeTapButton( EngineDeviceMan* pEngineDevice );                
	virtual void            CreateSubControl(const CreateArg& arg);	                //Rn��ư ��ɿܿ� ���õ� ������ �н����������� ����
    virtual void            TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );    //�н����������� Ŭ���̺�Ʈ ĳġ
	void                    SetState( const BYTE eState );						    //�н����� ���ɻ��� ����
	void                    SetSkill( const int iSkill );						    //���õ� ����
	void                    SetShowSkillTooltip(const bool bState);				    //���õ� ǥ��,��ǥ�� ���º���
	const bool              isShowSkillTooltip();                                   //���õ� ǥ�� ���¸� ������ �Լ�
	const BYTE              GetState();
private:
	void                    AllHideIcon();
};
#pragma once

#include "../Util/TapSelectorEx.h"
#include "../../../RanLogic/Product/GLProduct.h"

class CBasicTextBox;

//hsshin 제작분야 학습메시지창르 띄우기위해 메시지 추가
namespace ProductTypeMSG
{
    const DWORD UI_MSG_PRODUCT_STUDY = UIMSG_USER1;
}

//hsshin 제작 분야선택 : 탭버튼에 숙련도 툴팁과 학습기능을 추가 (2012.1.23)
class CProductTypeTapButton : public TAPBUTTON_RNBUTTON
{
public:
    enum
    {
        E_STUDY_NONE	= 0,		                                                //아이콘 없음 (학습과 관련없는 부분에서 사용 ex TypeTotal)
        E_STUDY_PUBLIC,			                                                    //공개 제작분야 아이콘
        E_STUDY_NEED,				                                                //학습필요 아이콘
        E_STUDY_STUDIED,			                                                //학습완료 아이콘
        E_STUDY_DISABLE,			                                                //학습 비활성화 아이콘
        E_STUDY_UNABLE,				                                                //학습 불가 아이콘 (ex 국가별 제작금지 컨텐츠)
    };
    enum
    {
        E_CID_STUDY_BUTTON = RnButton::WND_INACTIVE_ID + 1,                         //콘트롤 ID의 중복을 피하기 위함
    };
protected:
	BYTE					m_eStudyState;					                        //학습가능정보
	int						m_iSkill;						                        //숙련도 정보
	CUIControl*				m_pStudyIcon_Study_Public;		                        //학습 불필요 아이콘
	CUIControl*				m_pStudyIcon_Study_Need;		                        //학습 필요 아이콘
    CUIControl*				m_pStudyIcon_Study_Need_Cross;	                        //학습 필요 아이콘(내부 십자가)
	CUIControl*				m_pStudyIcon_Study_Studied;		                        //학습 완료 아이콘
	CUIControl*				m_pStudyIcon_Study_Disable;		                        //학습 포인트 부족 아이콘	(지금은 icon이 없지만 추가 될 가능성때문에 넣어놓음)
	CUIControl*				m_pStudyIcon_Study_Unable;		                        //제작분야 불가능 아이콘

	CBasicLineBoxSmart*		m_pSkillTooltip_Title;			                        //숙련도 툴팁
	CBasicTextBox*			m_pSkillTooltip;				                        //숙련도 툴팁
public:
	CProductTypeTapButton( EngineDeviceMan* pEngineDevice );                
	virtual void            CreateSubControl(const CreateArg& arg);	                //Rn버튼 기능외에 숙련도 툴팁과 학습정보아이콘 생성
    virtual void            TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );    //학습정보아이콘 클릭이벤트 캐치
	void                    SetState( const BYTE eState );						    //학습정도 가능상태 변경
	void                    SetSkill( const int iSkill );						    //숙련도 변경
	void                    SetShowSkillTooltip(const bool bState);				    //숙련도 표시,미표시 상태변경
	const bool              isShowSkillTooltip();                                   //숙련도 표시 상태를 얻어오는 함수
	const BYTE              GetState();
private:
	void                    AllHideIcon();
};
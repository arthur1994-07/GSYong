#pragma once
#include <set>
#include "./GLProductRecipeMan.h"

namespace se
{
	class ByteStream;
}

//  [2/8/2013 hsshin]
// 제작 시스템 리뉴얼

namespace Product
{
    enum
    {
        EStateNothing,	                // 재료가 하나도 없다
        EStatePart,		                // 재료가 일부는 있다
        EStateEnough,	                // 재료 조건이 충족되었다.
        EStateNotLearn,	                // 조합서를 배우지 않았다

        EStateOverCount,                // 생산수량이 넘었다
        EStateNotMoney,	                // 돈이 부족하다
        EStateInven,	                // 인벤토리 공간 부족
        EStateRateFail,	                // 확률 실패

        
        EStateTypeFail,	                // 제작가능 분야가 아니다.
        EStateSkillFail,                // 숙련도가 부족하다.
        EStateLevelFail,                // 레벨이 부족하다.
        EStateLevelAndSkillFail,        // 레벨과 숙련도 둘다 부족하다
		EStateCostumeFail,				// 재료에 코스츔이 있다.

        EStateMotion,	                // 모든 조건이 충족한다, 모션을 취해라
		EStateOngoingMotion,			// 모션을 계속해서 이어가라;
		EStateNeedConfirm,				// 확인 필요하다;
        EStateCancel,	                // 조합 취소
        EStateSuccess,	                // 조합 아이템 생성
        EStateGetEXP,	                // 숙련도 획득 & 조합 아이템 생성
        EStateComplete,	                // 조합 진행 완료

        EStateERROR,	                // 사용안함 에러검출용 

        EStateTypeAdd,					// 조합 항목 배우기
        EStateTypeReset,				// 조합 항목 지우기
    };
    const static int MAX_SKILL = 999;   // 최대 숙련치 (정수 3자리를 넘어가지 않도록 한것)

	enum ProductStudyState
	{
		E_STUDY_NONE	= 0,		    //아이콘 없음 (학습과 관련없는 부분에서 사용 ex TypeTotal)
		E_STUDY_PUBLIC,					//공개 제작분야 아이콘
		E_STUDY_NEED,				    //학습필요 아이콘
		E_STUDY_STUDIED,				//학습완료 아이콘
		E_STUDY_DISABLE,			    //학습 비활성화 아이콘
		E_STUDY_UNABLE,				    //학습 불가 아이콘 (ex 국가별 제작금지 컨텐츠)
	};
}

struct GLCHARLOGIC;

typedef std::set< DWORD > ProductRecipeExpContainer;    // 경험치를 얻은 조합법 맵
typedef ProductRecipeExpContainer::iterator ProductRecipeExpContainer_Iter;

typedef std::set< DWORD > ProductRecipeLearnContainer;  // 배운 조합서 맵
typedef ProductRecipeLearnContainer::iterator ProductRecipeLearnContainer_Iter;	        

class GLProduct
{
public:
    GLProduct();
	virtual ~GLProduct();
protected:
	bool		m_pProductTypeStudyState[ Product::ETypeTotal ];		                                                // 제작분야 학습상태
	WORD		m_pProductTypeSkill[ Product::ETypeTotal ];			        	                                        // 제작분야 숙련도
	WORD		m_nProductTypeMaxStudyPoint;							                                                // 제작분야 최대 학습가능 개수
	WORD        m_nProductTypeStudyPoint;								                                                // 제작분야 현재 학습포인트
	
	ProductRecipeLearnContainer		m_ProductRecipeLearnSet;					                                        // 조합서 리스트
	ProductRecipeExpContainer		m_ProductRecipeExpSet;						                                        // 경험치를 얻은 조합법 리스트

    void        ClearStudyProducttype();                                                                                // 모든 제작분야를 안배운것으로 초기화
public:
    //hsshin
    // 제작 검사 함수
    bool        CheckProductLearn( const SProductRecipe* pProductRecipe );                                              // 조합법을 배웠거나 사용할수 있는가?
    bool        CheckProductSkill( const SProductRecipe* pProductRecipe );                                              // 필요 숙련도를 만족하는가?
    bool        CheckProductLevel( const SProductRecipe* pProductRecipe, const WORD wLevel );                           // 필요 레벨을 만족하는가?
    bool        CheckProductInven( const SProductRecipe* pProductRecipe, GLCHARLOGIC* pCharLogic );                     // 1개 제작할 필요 인벤토리공간에 대한 검사

    // 제작 가능한 개수 체크 함수 ( 제작 불가시 return 0; )
    WORD        CountProductMaterial( const SProductRecipe* pProductRecipe, GLCHARLOGIC* pCharLogic, BYTE& eState );    // 필요 재료를 기준으로 제작 가능한 개수 검사
    WORD        CountProductMoney( const SProductRecipe* pProductRecipe, const LONGLONG Money );                        // 필요 소지금을 기준으로 제작 가능한 개수 검사
    WORD        CountProductInven( const SProductRecipe* pProductRecipe, GLCHARLOGIC* pCharLogic );                     // 인벤토리 공간을 기준으로 제작 가능한 개수 검사

    // 익힌 조합서 관련
	bool        LearnProductRecipe( const DWORD dwProductRecipeKey );                                                   // 해당 조합서 익힘 (조합법 이미 있을시 return false)	
	bool        IsLearnedProductRecipe( const DWORD dwProductRecipeKey );                                               // 해당 조합서를 익혔는가?
    bool        DelLearnedProductRecipe( const DWORD dwProductRecipeKey );                                              // 해당 조합서를 익히지 않은 상태로 변경

    // 숙련도얻은 조합법 관련
	bool        ExpProductRecipe( const DWORD dwProductRecipeKey );                                                     // 해당 조합서로 경험치 얻음
	bool        IsExpProductRecipe( const DWORD dwProductRecipeKey );                                                   // 해당 조합서로 경험치를 얻은적이 있는가?
    bool        DelExpProductRecipe( const DWORD dwProductRecipeKey );                                                  // 해당 조합서를 경험치 얻은적이 없는 상태로 변경

    // 경험치얻은 조합법리스트를 통해 숙련도를 계산
    bool        CountProductSkill();										                                

    // 숙련도 관련
	WORD        GetProductSkill( const WORD wProductType ) const;							                            // 해당 제작 분야 숙련도 얻어오기
	bool        SetProductSkill( const WORD wProductType, const WORD wSkill );				                            // 해당 제작 분야 숙련도 설정

    // 제작분야 관련 ( 제작분야가 공개분야인지 아닌지, 금지된 제작분야인지 아닌지를 default.charclass 에서 정보를 얻어온다.)
	bool        StudyProductType( const WORD wProductType );								                            // 해당 제작 분야를 익힌다.
    bool        IsStudyProductType( const WORD wProductType );								                            // 해당 제작 분야를 익혔는지 알아본다.
    bool        SetStudyProductType( const WORD wProductType , const bool bSet );			                            // 해당 제작 분야를 학습상태를 변경한다.
    bool        IsUsableProductType( const WORD wProductType );                                                         // 제작 가능한 분야인가? ( 공개or학습 )
    bool        IsOpenProductType( const WORD wProductType );                                                           // 해당 제작분야가 공개분야인가?
    bool        IsUnableProductType( const WORD wProductType );                                                         // 해당 제작분야가 금지분야인가?

    // 제작분야 최대 학습포인트에 관한 정보는 defaulf charclass에서 정보를 얻어온다.
	WORD        GetProductTypeStudyPoint() const;										                                // 제작분야 학습포인트를 얻어온다.
	WORD        GetProductTypeMaxStudyPoint() const;									                                // 제작분야 최대 학습포인트를 얻어온다.
};

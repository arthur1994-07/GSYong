#pragma once
#include <set>
#include "./GLProductRecipeMan.h"

namespace se
{
	class ByteStream;
}

//  [2/8/2013 hsshin]
// ���� �ý��� ������

namespace Product
{
    enum
    {
        EStateNothing,	                // ��ᰡ �ϳ��� ����
        EStatePart,		                // ��ᰡ �Ϻδ� �ִ�
        EStateEnough,	                // ��� ������ �����Ǿ���.
        EStateNotLearn,	                // ���ռ��� ����� �ʾҴ�

        EStateOverCount,                // ��������� �Ѿ���
        EStateNotMoney,	                // ���� �����ϴ�
        EStateInven,	                // �κ��丮 ���� ����
        EStateRateFail,	                // Ȯ�� ����

        
        EStateTypeFail,	                // ���۰��� �о߰� �ƴϴ�.
        EStateSkillFail,                // ���õ��� �����ϴ�.
        EStateLevelFail,                // ������ �����ϴ�.
        EStateLevelAndSkillFail,        // ������ ���õ� �Ѵ� �����ϴ�
		EStateCostumeFail,				// ��ῡ �ڽ����� �ִ�.

        EStateMotion,	                // ��� ������ �����Ѵ�, ����� ���ض�
		EStateOngoingMotion,			// ����� ����ؼ� �̾��;
		EStateNeedConfirm,				// Ȯ�� �ʿ��ϴ�;
        EStateCancel,	                // ���� ���
        EStateSuccess,	                // ���� ������ ����
        EStateGetEXP,	                // ���õ� ȹ�� & ���� ������ ����
        EStateComplete,	                // ���� ���� �Ϸ�

        EStateERROR,	                // ������ ��������� 

        EStateTypeAdd,					// ���� �׸� ����
        EStateTypeReset,				// ���� �׸� �����
    };
    const static int MAX_SKILL = 999;   // �ִ� ����ġ (���� 3�ڸ��� �Ѿ�� �ʵ��� �Ѱ�)

	enum ProductStudyState
	{
		E_STUDY_NONE	= 0,		    //������ ���� (�н��� ���þ��� �κп��� ��� ex TypeTotal)
		E_STUDY_PUBLIC,					//���� ���ۺо� ������
		E_STUDY_NEED,				    //�н��ʿ� ������
		E_STUDY_STUDIED,				//�н��Ϸ� ������
		E_STUDY_DISABLE,			    //�н� ��Ȱ��ȭ ������
		E_STUDY_UNABLE,				    //�н� �Ұ� ������ (ex ������ ���۱��� ������)
	};
}

struct GLCHARLOGIC;

typedef std::set< DWORD > ProductRecipeExpContainer;    // ����ġ�� ���� ���չ� ��
typedef ProductRecipeExpContainer::iterator ProductRecipeExpContainer_Iter;

typedef std::set< DWORD > ProductRecipeLearnContainer;  // ��� ���ռ� ��
typedef ProductRecipeLearnContainer::iterator ProductRecipeLearnContainer_Iter;	        

class GLProduct
{
public:
    GLProduct();
	virtual ~GLProduct();
protected:
	bool		m_pProductTypeStudyState[ Product::ETypeTotal ];		                                                // ���ۺо� �н�����
	WORD		m_pProductTypeSkill[ Product::ETypeTotal ];			        	                                        // ���ۺо� ���õ�
	WORD		m_nProductTypeMaxStudyPoint;							                                                // ���ۺо� �ִ� �н����� ����
	WORD        m_nProductTypeStudyPoint;								                                                // ���ۺо� ���� �н�����Ʈ
	
	ProductRecipeLearnContainer		m_ProductRecipeLearnSet;					                                        // ���ռ� ����Ʈ
	ProductRecipeExpContainer		m_ProductRecipeExpSet;						                                        // ����ġ�� ���� ���չ� ����Ʈ

    void        ClearStudyProducttype();                                                                                // ��� ���ۺо߸� �ȹ������� �ʱ�ȭ
public:
    //hsshin
    // ���� �˻� �Լ�
    bool        CheckProductLearn( const SProductRecipe* pProductRecipe );                                              // ���չ��� ����ų� ����Ҽ� �ִ°�?
    bool        CheckProductSkill( const SProductRecipe* pProductRecipe );                                              // �ʿ� ���õ��� �����ϴ°�?
    bool        CheckProductLevel( const SProductRecipe* pProductRecipe, const WORD wLevel );                           // �ʿ� ������ �����ϴ°�?
    bool        CheckProductInven( const SProductRecipe* pProductRecipe, GLCHARLOGIC* pCharLogic );                     // 1�� ������ �ʿ� �κ��丮������ ���� �˻�

    // ���� ������ ���� üũ �Լ� ( ���� �Ұ��� return 0; )
    WORD        CountProductMaterial( const SProductRecipe* pProductRecipe, GLCHARLOGIC* pCharLogic, BYTE& eState );    // �ʿ� ��Ḧ �������� ���� ������ ���� �˻�
    WORD        CountProductMoney( const SProductRecipe* pProductRecipe, const LONGLONG Money );                        // �ʿ� �������� �������� ���� ������ ���� �˻�
    WORD        CountProductInven( const SProductRecipe* pProductRecipe, GLCHARLOGIC* pCharLogic );                     // �κ��丮 ������ �������� ���� ������ ���� �˻�

    // ���� ���ռ� ����
	bool        LearnProductRecipe( const DWORD dwProductRecipeKey );                                                   // �ش� ���ռ� ���� (���չ� �̹� ������ return false)	
	bool        IsLearnedProductRecipe( const DWORD dwProductRecipeKey );                                               // �ش� ���ռ��� �����°�?
    bool        DelLearnedProductRecipe( const DWORD dwProductRecipeKey );                                              // �ش� ���ռ��� ������ ���� ���·� ����

    // ���õ����� ���չ� ����
	bool        ExpProductRecipe( const DWORD dwProductRecipeKey );                                                     // �ش� ���ռ��� ����ġ ����
	bool        IsExpProductRecipe( const DWORD dwProductRecipeKey );                                                   // �ش� ���ռ��� ����ġ�� �������� �ִ°�?
    bool        DelExpProductRecipe( const DWORD dwProductRecipeKey );                                                  // �ش� ���ռ��� ����ġ �������� ���� ���·� ����

    // ����ġ���� ���չ�����Ʈ�� ���� ���õ��� ���
    bool        CountProductSkill();										                                

    // ���õ� ����
	WORD        GetProductSkill( const WORD wProductType ) const;							                            // �ش� ���� �о� ���õ� ������
	bool        SetProductSkill( const WORD wProductType, const WORD wSkill );				                            // �ش� ���� �о� ���õ� ����

    // ���ۺо� ���� ( ���ۺо߰� �����о����� �ƴ���, ������ ���ۺо����� �ƴ����� default.charclass ���� ������ ���´�.)
	bool        StudyProductType( const WORD wProductType );								                            // �ش� ���� �о߸� ������.
    bool        IsStudyProductType( const WORD wProductType );								                            // �ش� ���� �о߸� �������� �˾ƺ���.
    bool        SetStudyProductType( const WORD wProductType , const bool bSet );			                            // �ش� ���� �о߸� �н����¸� �����Ѵ�.
    bool        IsUsableProductType( const WORD wProductType );                                                         // ���� ������ �о��ΰ�? ( ����or�н� )
    bool        IsOpenProductType( const WORD wProductType );                                                           // �ش� ���ۺо߰� �����о��ΰ�?
    bool        IsUnableProductType( const WORD wProductType );                                                         // �ش� ���ۺо߰� �����о��ΰ�?

    // ���ۺо� �ִ� �н�����Ʈ�� ���� ������ defaulf charclass���� ������ ���´�.
	WORD        GetProductTypeStudyPoint() const;										                                // ���ۺо� �н�����Ʈ�� ���´�.
	WORD        GetProductTypeMaxStudyPoint() const;									                                // ���ۺо� �ִ� �н�����Ʈ�� ���´�.
};

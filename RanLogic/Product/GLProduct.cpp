#include "pch.h"
#include "./GLProduct.h"
#include "./GLProductRecipe.h"
#include "./GLProductRecipeMan.h"
#include "../GLogicEx.h"
#include "../GLogicData.h"
#include "../GLogicDataMan.h"

GLProduct::GLProduct()
    : m_nProductTypeMaxStudyPoint((WORD)GLCONST_CHAR::dwStudyPointDefault)
    , m_nProductTypeStudyPoint(0)
{
	memset( m_pProductTypeStudyState, 0, sizeof( m_pProductTypeStudyState ) );
	memset( m_pProductTypeSkill, 0, sizeof( m_pProductTypeSkill ) );
}

GLProduct::~GLProduct()
{
	
}

bool GLProduct::CheckProductLearn( const SProductRecipe* pProductRecipe ) 
{
    if ( pProductRecipe->bNeedLearn )
    {
        if ( !IsLearnedProductRecipe( pProductRecipe->dwKey ) )
        {
            return false;
        }
    }
    return true;
}

bool GLProduct::CheckProductLevel( const SProductRecipe* pProductRecipe, const WORD wLevel )
{
    const WORD ReqLevel = pProductRecipe->wLevel;
    if ( ReqLevel != Product::PRODUCT_NOT_USE_REQ && wLevel < ReqLevel ) //�ʿ� ������ �ȵ� ��
    {
        return false;
    }
    return true;
}

bool GLProduct::CheckProductSkill( const SProductRecipe* pProductRecipe )
{
    const WORD ReqWS	= pProductRecipe->wWs;
    const WORD ReqType	= pProductRecipe->eProductType;
    if ( ReqWS != Product::PRODUCT_NOT_USE_REQ && GetProductSkill( ReqType ) < ReqWS ) //�ʿ� ������ �ȵ� ��
    {
        return false;
    }
    return true;
}

bool GLProduct::CheckProductInven( const SProductRecipe* pProductRecipe, GLCHARLOGIC* pCharLogic )
{
    if ( !pProductRecipe )
    {
        return false;
    }
    const SITEM* pItem = GLogicData::GetInstance().GetItem( pProductRecipe->sResultItem.sNID );	//��� ������ ����
    if ( !pItem )
    {
        return false;
    }
    if ( pItem->ISPILE() )
    {
        return pCharLogic->m_cInventory.ValidPileInsrt( 1, pProductRecipe->sResultItem.sNID, pItem->sDrugOp.wPileNum, false );
    }
    else
    {
        return pCharLogic->m_cInventory.ValidCheckInsrt( 1, false );
    }
}

WORD GLProduct::CountProductMoney( const SProductRecipe* pProductRecipe, const LONGLONG Money )
{
    if ( !pProductRecipe )
    {
        return 0;
    }
    if ( pProductRecipe->dwPrice > 0 )		//���۰����� �ִٸ�
    {
        const WORD wCount = WORD( Money / pProductRecipe->dwPrice);	// ���� ���� ���� = ������ / ���۰���
        return wCount;
    }
    return 0;
}

WORD GLProduct::CountProductInven( const SProductRecipe* pProductRecipe, GLCHARLOGIC* pCharLogic )
{
    if ( !pProductRecipe )
    {
        return 0;
    }
    const SNATIVEID sNID = pProductRecipe->sResultItem.sNID;
    const SITEM* pItem = GLogicData::GetInstance().GetItem( sNID );	//��� ������ ����
    if ( !pItem )
    {
        return 0;
    }
    GLInventory* pInven = &pCharLogic->m_cInventory;
    if ( pItem->ISPILE() )
    {
        return (WORD)pInven->GetAmountEmptyPile( sNID, FALSE );
    }
    else
    {
        return (WORD)pInven->GetAmountEmpty( FALSE );
    }
}

WORD GLProduct::CountProductMaterial( const SProductRecipe* pProductRecipe, GLCHARLOGIC* pCharLogic, BYTE& eState)
{
    if ( !pProductRecipe )
    {
        eState = Product::EStateERROR;
        return false;
    }

    WORD wCount = USHRT_MAX;
    bool bPart = false;

    for( int nMaterialCnt =0; nMaterialCnt < Product::ITEM_PRODUCT_MAX_MATERIAL; ++nMaterialCnt )
    {
        const SProductItem& sData = pProductRecipe->sMeterialItem[ nMaterialCnt ];

        if ( sData.sNID == NATIVEID_NULL() || sData.nNum <= 0 )
            continue;

        //�κ� ������
        const DWORD nMaterial = pCharLogic->m_cInventory.CountTurnItem( sData.sNID ); //���� ����� ������ ���´�
        if ( nMaterial > 0 )
        {
            WORD nCountPossibleMaterial = static_cast< WORD >( nMaterial / sData.nNum );    // ���۰��� ���� = �������� / �ʿ����
            if ( nCountPossibleMaterial > 0 )
            {
                bPart = true;   //��� �ϳ��� ���� �����Ѵٴ� �÷���
                wCount = min( wCount , nCountPossibleMaterial );    //���� ���� ������ �ݿ�
            }
            else
            {
                wCount = 0;
            }
        }
        else	//�������� �Ѱ��� ���� ��
        {
            wCount = 0;
        }
    }

    if( wCount > 0 )    //�Ѱ��� ���� �����ϸ�
    {
        eState = Product::EStateEnough;
    }
    else
    {
        eState = bPart ? Product::EStatePart : Product::EStateNothing;	//��� �Ѱ����� �����߾��ٸ� �κ� ��Ḹ������ ����
    }
    return wCount;
}

bool GLProduct::LearnProductRecipe( const DWORD dwProductRecipeKey )
{
	if ( IsLearnedProductRecipe( dwProductRecipeKey ) )
	{
		return false;	//�̹� ���չ��� �ִٸ� ����
	}
	m_ProductRecipeLearnSet.insert( dwProductRecipeKey );
	return true;
}

bool GLProduct::IsLearnedProductRecipe( const DWORD dwProductRecipeKey )
{
	ProductRecipeLearnContainer_Iter it = m_ProductRecipeLearnSet.find( dwProductRecipeKey );
	if ( it == m_ProductRecipeLearnSet.end() )
	{
		return false;	//���չ��� ���ٸ� ����
	}
	return true;
}

bool GLProduct::DelLearnedProductRecipe( const DWORD dwProductRecipeKey )
{
    ProductRecipeLearnContainer_Iter it = m_ProductRecipeLearnSet.find( dwProductRecipeKey );
    if ( it == m_ProductRecipeLearnSet.end() )
    {
        return false;	//���չ��� ���ٸ� ����
    }
    m_ProductRecipeLearnSet.erase( it );
    return true;
}

//  [2/8/2013 hsshin] ���õ� �ý��� ����
bool GLProduct::CountProductSkill()
{
	memset( m_pProductTypeSkill, 0, sizeof( m_pProductTypeSkill) );	//0���� �ʱ�ȭ
	
	BOOST_FOREACH ( ProductRecipeExpContainer::value_type ProductRecipeExp, m_ProductRecipeExpSet )
	{
		SProductRecipe* pRecipe = GLProductRecipeMan::GetInstance().GetProductRecipe( ProductRecipeExp );
		const WORD wType = pRecipe->eProductType;
		if ( wType < Product::ETypeHeadGear ||  wType >= Product::ETypeTotal )	//Ÿ�԰��� �̻��ϸ� �߸��� �迭������ �����ϹǷ� ����ó��
		{
			return false;
		}
		if ( pRecipe->wWsp == Product::PRODUCT_NOT_USE_REQ )	//����ġ�� �����ϴ� ���չ��϶� ����ó��
		{
			return false;
		}
		m_pProductTypeSkill[wType] += pRecipe->wWsp;
	}
	return true;
}

bool GLProduct::ExpProductRecipe( const DWORD dwProductRecipeKey )					
{
    const SProductRecipe* sProductRecipe = GLProductRecipeMan::GetInstance().GetProductRecipe( dwProductRecipeKey );
    if (!sProductRecipe)
	{
		sc::writeLogError( stringformat( "Can't find %1% Key value. Check ItemMix.ims file.", dwProductRecipeKey ) );
        return false;
	}

    const WORD wProductType = sProductRecipe->eProductType;
    if ( wProductType < Product::ETypeHeadGear || wProductType >= Product::ETypeTotal)
    {
        return false;
    }
    ProductRecipeExpContainer_Iter it = m_ProductRecipeExpSet.find( dwProductRecipeKey );
    if ( it != m_ProductRecipeExpSet.end() )   //������ �̹� ����ġ�� �������� �ִٸ�
    {
        return false;
    }
    if ( sProductRecipe->wWsp != Product::PRODUCT_NOT_USE_REQ )		//����ġ�� �����ϴ� ���չ��϶� ����ó��;
    {
        m_ProductRecipeExpSet.insert( dwProductRecipeKey );
        m_pProductTypeSkill[wProductType] += sProductRecipe->wWsp;	//���õ� ȹ��;
    }
	return true;
}

bool GLProduct::IsExpProductRecipe( const DWORD dwProductRecipeKey )					//�ش� ���ռ��� ����ġ�� �������� �ִ°�?
{
	ProductRecipeExpContainer_Iter it = m_ProductRecipeExpSet.find( dwProductRecipeKey );
	if ( it == m_ProductRecipeExpSet.end() )
	{
		return false;	//���չ��� ���;ȿ� ���ٸ� �������� ���°�;
	}
	return true ;
}

bool GLProduct::DelExpProductRecipe( const DWORD dwProductRecipeKey )
{
	const SProductRecipe* sProductRecipe = GLProductRecipeMan::GetInstance().GetProductRecipe( dwProductRecipeKey );
    ProductRecipeExpContainer_Iter it = m_ProductRecipeExpSet.find( dwProductRecipeKey );

    if (!sProductRecipe)
	{
		sc::writeLogError( stringformat( "Can't find %1% Key value. Check ItemMix.ims file.", dwProductRecipeKey ) );
        return false;
	}

    const WORD wProductType = sProductRecipe->eProductType;
    if ( wProductType < Product::ETypeHeadGear || wProductType >= Product::ETypeTotal)
    {
        return false;
    }

	if ( it == m_ProductRecipeExpSet.end() )
	{
		return false;	//���չ��� ���;ȿ� �ִٸ� �������� ���°�;
	}
    
	m_ProductRecipeExpSet.erase( it );

	m_pProductTypeSkill[wProductType] -= sProductRecipe->wWsp;	//���õ� ����;

    return true;
}

WORD GLProduct::GetProductSkill( const WORD wProductType ) const						//�ش� ���� �о� ���õ� ������
{
	if ( wProductType < Product::ETypeHeadGear || wProductType >= Product::ETypeTotal)
	{
		return 0;
	}
	return m_pProductTypeSkill[wProductType];
}

bool GLProduct::SetProductSkill( const WORD wProductType, const WORD wSkill )
{
	if ( wProductType < Product::ETypeHeadGear || wProductType >= Product::ETypeTotal)
	{
		return false;
	}
	if ( wSkill < 0 || wSkill > Product::MAX_SKILL )
	{
		return false;
	}
	m_pProductTypeSkill[wProductType] = wSkill;
	return true;
}

bool GLProduct::StudyProductType( const WORD wProductType )							//�ش� ���� �о߸� ������.
{
	if ( wProductType < Product::ETypeHeadGear || wProductType >= Product::ETypeTotal)
	{
		return false;
	}
	if ( m_nProductTypeStudyPoint <= 0 )	//�н�����Ʈ�� ���ٸ� ���� false;
	{
		return false;
	}
    if ( IsUnableProductType( wProductType ) )    //���Ұ��о߶�� ���� false;
    {
        return false;
    }
    if ( IsOpenProductType( wProductType ) )    //�������ۺо߶�� ���� false;
    {
        return false;
    }

    if ( m_pProductTypeStudyState[wProductType] == false )
    {
        m_pProductTypeStudyState[wProductType] = true;
        m_nProductTypeStudyPoint--;
    }
    return true;
}

bool GLProduct::IsStudyProductType( const WORD wProductType )							//�ش� ���� �о߸� �������� �˾ƺ���.
{
	if ( wProductType < Product::ETypeHeadGear || wProductType >= Product::ETypeTotal)
	{
		return false;
	}
    return m_pProductTypeStudyState[wProductType];
}

bool GLProduct::SetStudyProductType( const WORD wProductType , const bool bSet)					//�ش� ���� �о��� �н����¸� �����Ѵ�.
{
	if ( wProductType < Product::ETypeHeadGear || wProductType >= Product::ETypeTotal)
	{
		return false;
	}
	m_pProductTypeStudyState[wProductType] = bSet;
	return true;
}

bool GLProduct::IsUsableProductType( const WORD wProductType )
{
    if ( IsUnableProductType( wProductType ) )
    {
        return false;
    }
    if ( IsOpenProductType( wProductType ) )
    {  
        return true;
    }
    return m_pProductTypeStudyState[wProductType];
}


WORD GLProduct::GetProductTypeStudyPoint() const
{
	return m_nProductTypeStudyPoint;
}

WORD GLProduct::GetProductTypeMaxStudyPoint() const
{
	return m_nProductTypeMaxStudyPoint;
}
void GLProduct::ClearStudyProducttype()
{
    memset( m_pProductTypeStudyState, 0, sizeof( m_pProductTypeStudyState ) );
}

bool GLProduct::IsOpenProductType( const WORD wProductType )
{
    if ( wProductType < Product::ETypeHeadGear || wProductType >= Product::ETypeTotal)
    {
        return false;
    }
    return GLCONST_CHAR::bPublicStudyProductType[wProductType];
}

bool GLProduct::IsUnableProductType( const WORD wProductType )
{
    if ( wProductType < Product::ETypeHeadGear || wProductType >= Product::ETypeTotal)
    {
        return false;
    }
    return GLCONST_CHAR::bUnableProductType[wProductType];
}
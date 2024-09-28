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
    if ( ReqLevel != Product::PRODUCT_NOT_USE_REQ && wLevel < ReqLevel ) //필요 레벨이 안될 때
    {
        return false;
    }
    return true;
}

bool GLProduct::CheckProductSkill( const SProductRecipe* pProductRecipe )
{
    const WORD ReqWS	= pProductRecipe->wWs;
    const WORD ReqType	= pProductRecipe->eProductType;
    if ( ReqWS != Product::PRODUCT_NOT_USE_REQ && GetProductSkill( ReqType ) < ReqWS ) //필요 레벨이 안될 때
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
    const SITEM* pItem = GLogicData::GetInstance().GetItem( pProductRecipe->sResultItem.sNID );	//재료 아이템 얻어옴
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
    if ( pProductRecipe->dwPrice > 0 )		//제작가격이 있다면
    {
        const WORD wCount = WORD( Money / pProductRecipe->dwPrice);	// 제작 가능 수량 = 가진돈 / 제작가격
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
    const SITEM* pItem = GLogicData::GetInstance().GetItem( sNID );	//재료 아이템 얻어옴
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

        //인벤 얻어야함
        const DWORD nMaterial = pCharLogic->m_cInventory.CountTurnItem( sData.sNID ); //가진 재료의 수량을 얻어온다
        if ( nMaterial > 0 )
        {
            WORD nCountPossibleMaterial = static_cast< WORD >( nMaterial / sData.nNum );    // 제작가능 수량 = 가진수량 / 필요수량
            if ( nCountPossibleMaterial > 0 )
            {
                bPart = true;   //적어도 하나의 재료는 만족한다는 플래그
                wCount = min( wCount , nCountPossibleMaterial );    //생산 가능 수량을 반영
            }
            else
            {
                wCount = 0;
            }
        }
        else	//재료수량이 한개도 없을 때
        {
            wCount = 0;
        }
    }

    if( wCount > 0 )    //한개라도 제작 가능하면
    {
        eState = Product::EStateEnough;
    }
    else
    {
        eState = bPart ? Product::EStatePart : Product::EStateNothing;	//재료 한가지라도 만족했었다면 부분 재료만족값을 리턴
    }
    return wCount;
}

bool GLProduct::LearnProductRecipe( const DWORD dwProductRecipeKey )
{
	if ( IsLearnedProductRecipe( dwProductRecipeKey ) )
	{
		return false;	//이미 조합법이 있다면 실패
	}
	m_ProductRecipeLearnSet.insert( dwProductRecipeKey );
	return true;
}

bool GLProduct::IsLearnedProductRecipe( const DWORD dwProductRecipeKey )
{
	ProductRecipeLearnContainer_Iter it = m_ProductRecipeLearnSet.find( dwProductRecipeKey );
	if ( it == m_ProductRecipeLearnSet.end() )
	{
		return false;	//조합법이 없다면 실패
	}
	return true;
}

bool GLProduct::DelLearnedProductRecipe( const DWORD dwProductRecipeKey )
{
    ProductRecipeLearnContainer_Iter it = m_ProductRecipeLearnSet.find( dwProductRecipeKey );
    if ( it == m_ProductRecipeLearnSet.end() )
    {
        return false;	//조합법이 없다면 실패
    }
    m_ProductRecipeLearnSet.erase( it );
    return true;
}

//  [2/8/2013 hsshin] 숙련도 시스템 로직
bool GLProduct::CountProductSkill()
{
	memset( m_pProductTypeSkill, 0, sizeof( m_pProductTypeSkill) );	//0으로 초기화
	
	BOOST_FOREACH ( ProductRecipeExpContainer::value_type ProductRecipeExp, m_ProductRecipeExpSet )
	{
		SProductRecipe* pRecipe = GLProductRecipeMan::GetInstance().GetProductRecipe( ProductRecipeExp );
		const WORD wType = pRecipe->eProductType;
		if ( wType < Product::ETypeHeadGear ||  wType >= Product::ETypeTotal )	//타입값이 이상하면 잘못된 배열접근이 위험하므로 예외처리
		{
			return false;
		}
		if ( pRecipe->wWsp == Product::PRODUCT_NOT_USE_REQ )	//경험치를 사용안하는 조합법일때 예외처리
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
    if ( it != m_ProductRecipeExpSet.end() )   //기존에 이미 경험치를 얻은적이 있다면
    {
        return false;
    }
    if ( sProductRecipe->wWsp != Product::PRODUCT_NOT_USE_REQ )		//경험치를 사용안하는 조합법일때 예외처리;
    {
        m_ProductRecipeExpSet.insert( dwProductRecipeKey );
        m_pProductTypeSkill[wProductType] += sProductRecipe->wWsp;	//숙련도 획득;
    }
	return true;
}

bool GLProduct::IsExpProductRecipe( const DWORD dwProductRecipeKey )					//해당 조합서로 경험치를 얻은적이 있는가?
{
	ProductRecipeExpContainer_Iter it = m_ProductRecipeExpSet.find( dwProductRecipeKey );
	if ( it == m_ProductRecipeExpSet.end() )
	{
		return false;	//조합법이 벡터안에 없다면 얻은적이 없는것;
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
		return false;	//조합법이 벡터안에 있다면 얻은적이 없는것;
	}
    
	m_ProductRecipeExpSet.erase( it );

	m_pProductTypeSkill[wProductType] -= sProductRecipe->wWsp;	//숙련도 삭제;

    return true;
}

WORD GLProduct::GetProductSkill( const WORD wProductType ) const						//해당 제작 분야 숙련도 얻어오기
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

bool GLProduct::StudyProductType( const WORD wProductType )							//해당 제작 분야를 익힌다.
{
	if ( wProductType < Product::ETypeHeadGear || wProductType >= Product::ETypeTotal)
	{
		return false;
	}
	if ( m_nProductTypeStudyPoint <= 0 )	//학습포인트가 없다면 리턴 false;
	{
		return false;
	}
    if ( IsUnableProductType( wProductType ) )    //사용불가분야라면 리턴 false;
    {
        return false;
    }
    if ( IsOpenProductType( wProductType ) )    //공개제작분야라면 리턴 false;
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

bool GLProduct::IsStudyProductType( const WORD wProductType )							//해당 제작 분야를 익혔는지 알아본다.
{
	if ( wProductType < Product::ETypeHeadGear || wProductType >= Product::ETypeTotal)
	{
		return false;
	}
    return m_pProductTypeStudyState[wProductType];
}

bool GLProduct::SetStudyProductType( const WORD wProductType , const bool bSet)					//해당 제작 분야의 학습상태를 변경한다.
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
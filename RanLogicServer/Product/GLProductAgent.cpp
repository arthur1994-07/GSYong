#include "pch.h"
#include "GLProductAgent.h"

#include "../../SigmaCore/Math/Random.h"

#include "../../RanLogic/Msg/ProductMsg.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Product/GLProductRecipe.h"
#include "../../RanLogic/Product/GLProductRecipeMan.h"

#include "../AgentServer/GLAgentServer.h"

#include "../Database/DBAction/DbActionGame.h"
#include "../Database/DBAction/DbActionGameProduct.h"

GLProductAgent::GLProductAgent()
    : m_bIsLoadedBook(0)
    , m_bIsLoadedType(0)
    , m_bIsLoadedIsGetExp(0)
	, m_bIsLoadingIsGetExp(false)

{
}

bool GLProductAgent::CheckProduct_AgentServer(const WORD wLevel, const SProductRecipe* pProductRecipe, BYTE& eState )
{
    if ( !pProductRecipe )
    {
        eState = Product::EStateERROR;
        return false;
    }

    if( IsUsableProductType( pProductRecipe->eProductType ) == FALSE )
    {
        eState = Product::EStateTypeFail;
        return false;
    }

    // 조합법을 모르는가?
    if( CheckProductLearn( pProductRecipe ) == FALSE )
    {
        eState = Product::EStateNotLearn;
        return false;
    }

    const bool bCheckLevel = CheckProductLevel( pProductRecipe, wLevel );
    const bool bCheckSkill = CheckProductSkill( pProductRecipe );
    if ( bCheckLevel == false ) //필요레벨 불만족시
    {
        if ( bCheckSkill == false ) //필요레벨&숙련도 불만족시
        {
            eState = Product::EStateLevelAndSkillFail;
            return false;
        }
        eState = Product::EStateLevelFail;
        return false;
    }
    else if ( bCheckSkill == false )     //필요숙련도 불만족시
    {
        eState = Product::EStateSkillFail;
        return false;
    }
    eState = Product::EStateEnough;
    return true;
}

int GLProductAgent::IsProduct( DWORD nKey, GLAgentServer* pServer, GLCharAG* pChar )
{
    // 목록이 로드되어 있지 않다면 작업을 할 수 없다.
    if( !IsLoadedBook( pServer, pChar->m_CharDbNum ) ||
        !IsLoadedType( pServer, pChar->m_CharDbNum ) ||
        !IsLoadedIsGetExp( pServer, pChar->m_CharDbNum ) )
        return Product::EStateERROR;

    SProductRecipe* sProduct = GLProductRecipeMan::GetInstance().GetProductRecipe( nKey );
    if( sProduct == NULL )
        return Product::EStateERROR;

    byte eState;
    if ( CheckProduct_AgentServer( pChar->GetChaLevel(), sProduct, eState )  == false )    //필요레벨과 필요숙련도를 검사
    {
        return eState;
    }

    return Product::EStateMotion;
}

bool GLProductAgent::IsUsableBook( DWORD nItemID )
{
    const SITEM* pItem = GLogicData::GetInstance().GetItem( nItemID );
    if( !pItem || pItem->sBasicOp.emItemType != ITEM_PRODUCT_BOOK )
        return false;

    DWORD nProductBook = pItem->sSkillBookOp.sSkill_ID.dwID;

    const SProductRecipe* pProductRecipe = GLProductRecipeMan::GetInstance().GetProductRecipe( nProductBook );

    if( pProductRecipe == NULL || IsUsableProductType( pProductRecipe->eProductType ) == false ) //조합법을 배울수 있는 분야인지 체크
        return false;

    if( IsLearnedProductRecipe( nProductBook ) )    //조합법을 배운적이 있는지 체크
        return false;

    return true;
}

void GLProductAgent::SetBookList( const std::vector<DWORD>& vecBookList, GLAgentServer* pServer, GLCharAG* pChar )
{
    if( pChar == NULL || pServer == NULL )
        return;

    m_ProductRecipeLearnSet.clear();

    //목록을 로드하여 세팅
    BOOST_FOREACH( const DWORD& dwBookID, vecBookList )
        m_ProductRecipeLearnSet.insert( dwBookID );

    //목록이 로드 완료되면 목록을 클라이언트로 전송한다.
    m_bIsLoadedBook = TRUE;
    MegSendBookList( pServer, pChar );
}

void GLProductAgent::SetTypeList( const std::vector<WORD>& vecType, GLAgentServer* pServer, GLCharAG* pChar )
{
    if( pChar == NULL || pServer == NULL )
        return;

    ClearStudyProducttype();

    m_nProductTypeMaxStudyPoint = (WORD)GLCONST_CHAR::dwStudyPointDefault;
    m_nProductTypeStudyPoint    = m_nProductTypeMaxStudyPoint - vecType.size();

    //목록을 로드하여 세팅
    BOOST_FOREACH( const WORD& nType, vecType )
        if( SetStudyProductType( nType, TRUE ) == FALSE )
            sc::writeLogError( sc::string::format( "SetStudyProductType Is Unknown Type:%d, Char:%d", nType, pChar->ChaDbNum() ) );

    //목록이 로드 완료되면 목록을 클라이언트로 전송한다.
    m_bIsLoadedType = TRUE;
    MsgSendTypeList( pServer, pChar );
}

void GLProductAgent::AddType( int nType, GLAgentServer* pServer, GLCharAG* pChar )
{
    if( pChar == NULL || pServer == NULL )
    {
        MsgSendAdd( nType, FALSE, pServer, pChar );
        return;
    }

    if( IsLoadedType( pServer, pChar->ChaDbNum() ) == false)
    {
        MsgSendAdd( nType, FALSE, pServer, pChar );
        return;
    }

  
    if( StudyProductType( nType ) == FALSE )
    {
        MsgSendAdd( nType, FALSE, pServer, pChar );
        return;
    }


    // 타입 추가
    pServer->AddGameAdoJob(
            db::DbActionPtr(
                new db::ProductAddType( pChar->ChaDbNum(), nType )));
    MsgSendAdd( nType, TRUE, pServer, pChar );
}

bool GLProductAgent::IsDelType( const std::vector<WORD>& m_vecProductType, GLAgentServer* pServer, GLCharAG* pChar )
{
    if( IsLoadedType( pServer, pChar->ChaDbNum() ) == false)
        return FALSE;

    BOOST_FOREACH( const WORD& nType, m_vecProductType )
        if( IsStudyProductType( nType ) == FALSE )
            return FALSE;

    return TRUE;
}

void GLProductAgent::SetIsGetExpList( const std::vector<WORD>& vecIsGetExp, GLAgentServer* pServer, GLCharAG* pChar, bool bFirst, bool bLast )
{
	DWORD dwGetExpID = 0;

    if( pChar == NULL || pServer == NULL )
        return;

	if( bFirst == true )
	{
		m_ProductRecipeExpSet.clear();
		m_bIsLoadingIsGetExp = true; // 현재 제작 정보 로딩중
	}

    //목록을 로드하여 세팅
    BOOST_FOREACH( const WORD& dwIsGetExp, vecIsGetExp )
	{
		dwGetExpID = (DWORD) dwIsGetExp;

        if( ExpProductRecipe( dwGetExpID ) == FALSE )
            sc::writeLogError( sc::string::format( "SetIsGetExpList Is Unknown Id:%d, Char:%d", dwGetExpID, pChar->ChaDbNum() ) );
	}

	if( bLast == true )
	{
		//목록이 로드 완료되면 목록을 클라이언트로 전송한다.
		m_bIsLoadedIsGetExp = TRUE;
		m_bIsLoadingIsGetExp = false;
		MsgSendIsGetExpList( pServer, pChar, true, bFirst, bLast );
	}    
}

void GLProductAgent::AddIsGetExp( DWORD dwID, GLAgentServer* pServer, DWORD dwChaDbNum )
{
    if( pServer == NULL )
        return;

    // 경험치 추가
    pServer->AddGameAdoJob(
        db::DbActionPtr(
            new db::ProductIsGetExpInsert( dwChaDbNum, dwID )));

    IsLoadedIsGetExp( pServer, dwChaDbNum );

}

void GLProductAgent::MegSendBookList( GLAgentServer* pServer, GLCharAG* pChar )
{
    if( pChar == NULL || pServer == NULL )
        return;

    if( IsLoadedBook( pServer, pChar->ChaDbNum() ) == false)
        return;

    GLMSG::NET_PRODUCT_BOOK_LIST_ACK SendData;
    {
        SendData.m_dwChaDbNum = pChar->ChaDbNum();

        BOOST_FOREACH( const ProductRecipeLearnContainer::value_type& Date, m_ProductRecipeLearnSet )
            SendData.m_vecBookList.push_back( Date );
    }

    msgpack::sbuffer PackBuffer;
    msgpack::pack( PackBuffer, SendData );
    pServer->SENDTOCLIENT(pChar->m_ClientSlot, NET_MSG_PRODUCTBOOK, PackBuffer);

}

void GLProductAgent::MsgSendTypeList( GLAgentServer* pServer, GLCharAG* pChar )
{
    if( pChar == NULL || pServer == NULL )
        return;

    if( IsLoadedType( pServer, pChar->ChaDbNum() ) == false)
        return;

    GLMSG::NET_PRODUCT_TYPE_LIST_ACK SendData;
    {
        SendData.m_dwChaDbNum = pChar->ChaDbNum();
        for( int i=0; i<Product::ETypeTotal ; i++ )
            if( m_pProductTypeStudyState[i] == true )
                SendData.m_vecType.push_back(i);

    }

    msgpack::sbuffer PackBuffer;
    msgpack::pack( PackBuffer, SendData );
    pServer->SENDTOCLIENT(pChar->m_ClientSlot, NET_MSG_PRODUCT_TYPE_LIST_ACK, PackBuffer);
}

void GLProductAgent::MsgSendIsGetExpList( GLAgentServer* pServer, GLCharAG* pChar, bool bDBMsg, bool bFirst, bool bLast )
{
    if( pChar == NULL || pServer == NULL )
        return;
	
    if( IsLoadedIsGetExp( pServer, pChar->ChaDbNum() ) == false)
	        return;

	/*if( bDBMsg == true )
	{
		GLMSG::NET_PRODUCT_ISGETEXP_LIST_ACK SendData;
		{
			SendData.m_dwChaDbNum = pChar->ChaDbNum();

			BOOST_FOREACH( const ProductRecipeExpContainer::value_type& RecipeInfoData, m_ProductRecipeExpSet )
			{
				SendData.m_vecIsGetExp.push_back( (WORD)RecipeInfoData );
			}

			SendData.m_bFirst = bFirst;
			SendData.m_bLast = bLast;
		}

		msgpack::sbuffer PackBuffer;
		msgpack::pack( PackBuffer, SendData );
		pServer->SENDTOCLIENT(pChar->m_ClientSlot, NET_MSG_PRODUCT_ISGETEXP_LIST_ACK, PackBuffer);
	}
	else
	{*/
		std::vector<WORD> vecProductID;
		bool bFirstIdx = true;
		int nCount = 0;

		vecProductID.clear();

		BOOST_FOREACH( const ProductRecipeExpContainer::value_type& RecipeInfoData, m_ProductRecipeExpSet )
		{
			vecProductID.push_back( (WORD)RecipeInfoData );
			nCount++;

			if( nCount % 100 == 0 )
			{
				GLMSG::NET_PRODUCT_ISGETEXP_LIST_ACK SendData;
				SendData.m_dwChaDbNum = pChar->ChaDbNum();
				SendData.m_vecIsGetExp = vecProductID;
				bFirstIdx == true ? SendData.m_bFirst = true : SendData.m_bFirst = false;
				SendData.m_bLast = false;
				msgpack::sbuffer PackBuffer;
				msgpack::pack( PackBuffer, SendData );
				pServer->SENDTOCLIENT(pChar->m_ClientSlot, NET_MSG_PRODUCT_ISGETEXP_LIST_ACK, PackBuffer);

				vecProductID.clear();
				bFirstIdx = false;
			}

		}	

		// 마지막 데이터
		GLMSG::NET_PRODUCT_ISGETEXP_LIST_ACK SendDataEnd;
		SendDataEnd.m_dwChaDbNum = pChar->ChaDbNum();
		SendDataEnd.m_vecIsGetExp = vecProductID;
		SendDataEnd.m_bFirst = false;
		SendDataEnd.m_bLast = true;
		msgpack::sbuffer PackBufferEnd;
		msgpack::pack( PackBufferEnd, SendDataEnd );
		pServer->SENDTOCLIENT(pChar->m_ClientSlot, NET_MSG_PRODUCT_ISGETEXP_LIST_ACK, PackBufferEnd);
	//}
}

void GLProductAgent::MsgSendAdd( WORD nType, bool bOK, GLAgentServer* pServer, GLCharAG* pChar )
{
    if( pChar == NULL || pServer == NULL )
        return;

    GLMSG::NET_PRODUCT_TYPE_ADD_ACK SendData;
    {
        SendData.m_bOK          = bOK;
        SendData.m_dwChaDbNum   = pChar->ChaDbNum();
        SendData.m_nProductType = nType;
    }

    msgpack::sbuffer PackBuffer;
    msgpack::pack( PackBuffer, SendData );
    pServer->SENDTOCLIENT(pChar->m_ClientSlot, NET_MSG_PRODUCT_TYPE_ADD_ACK, PackBuffer);
}

int GLProductAgent::ProductSuccess( const DWORD RecipeID,GLAgentServer* pServer, DWORD ChaDbNum )
{
    SProductRecipe* sProductRecipe = GLProductRecipeMan::GetInstance().GetProductRecipe( RecipeID );
    if( sProductRecipe == NULL || pServer == NULL )
        return Product::EStateSuccess;

	// 경험치 획득 여부가 확인된 후 확률을 구해야 한다;
	// 경험치를 얻은적이 있는가?
	if( IsExpProductRecipe(RecipeID) == true )
		return Product::EStateSuccess;

    // 확률을 구한다.
    float fRate = (float)sProductRecipe->wWspPer;
    if( !sc::Random::getInstance().IsSucceed( fRate ) )
	{
		// 제작 숙련도 획득에 실패하였으므로 실패 제작 로그를 남긴다;
		GenerateItemFail( RecipeID, pServer, ChaDbNum );

        return Product::EStateSuccess;
	}

    // 경험치를 얻고 DB에 쓴다.
    if( ExpProductRecipe( RecipeID ) == false )
        return Product::EStateSuccess;

    // 경험치를 얻는 조합법 일 경우에만 DB에 저장한다.;
    if ( sProductRecipe->wWsp != Product::PRODUCT_NOT_USE_REQ )
    {
        pServer->AddGameAdoJob(
            db::DbActionPtr(
            new db::ProductIsGetExpInsert( ChaDbNum, RecipeID )));
    }

    return Product::EStateGetEXP;
}

void GLProductAgent::LearnBookSuccess( DWORD dwBookID)
{
    //DB작업은 필드서버에서 한다.
	m_ProductRecipeLearnSet.insert( dwBookID );
}

void GLProductAgent::TypeDeleteSuccess( const std::vector<WORD>& vecProductType, GLAgentServer* pServer, DWORD ChaDbNum )
{
    BOOST_FOREACH( const WORD& wType, vecProductType )
    {
        if ( wType < Product::ETypeHeadGear || wType >= Product::ETypeTotal)
            continue;

        if( m_pProductTypeStudyState[wType] == false )
        {
            sc::writeLogError( sc::string::format( "There is a fatal error to use of the Cashitem. ChaDbNum:%d, ProductType:%d", ChaDbNum, wType ) );
            continue;
        }

        // 해당 타입을 삭제
        m_pProductTypeStudyState[wType] = false;
        m_nProductTypeStudyPoint++;
        pServer->AddGameAdoJob(
            db::DbActionPtr( 
            new db::ProductDelType( ChaDbNum, wType )));

        // 목록 만들기
        PtrVector_ProductRecipe vecRecipe;
        GLProductRecipeMan::GetInstance().Find( vecRecipe, (BYTE)wType );

        // 경험치를 얻은 목록
        std::vector<PRODUCT_ITEM> vecGetExp;

        // 배운 조합서
        std::vector<PRODUCT_ITEM> vecBookID;

        BOOST_FOREACH( const SProductRecipe* pRecipe, vecRecipe )
        {
            if( IsExpProductRecipe( pRecipe->dwKey ) == true )
            {
				PRODUCT_ITEM tempItem;
				tempItem.dwProductType = pRecipe->eProductType;
				tempItem.dwProductID = pRecipe->dwKey;
                vecGetExp.push_back( tempItem );
                DelExpProductRecipe( pRecipe->dwKey );
            }

            if( m_ProductRecipeLearnSet.find( pRecipe->dwKey ) != m_ProductRecipeLearnSet.end() )
            {
				PRODUCT_ITEM tempItem;
				tempItem.dwProductType = pRecipe->eProductType;
				tempItem.dwProductID = pRecipe->dwKey;
                vecBookID.push_back( tempItem );
                DelLearnedProductRecipe( pRecipe->dwKey );
            }
        }

        // DB에서 경험치를 얻은정보 삭제
        pServer->AddGameAdoJob(
            db::DbActionPtr( 
                new db::ProductIsGetExpDelete( ChaDbNum, vecGetExp )));

        pServer->AddGameAdoJob(
            db::DbActionPtr( 
                new db::ProductBookDelete( ChaDbNum, vecBookID )));

    }
}

// 숙련도 얻는것을 성공할 경우;
void GLProductAgent::GenerateItemSuccess( const DWORD RecipeID
										 , GLAgentServer* pServer
										 , DWORD ChaDbNum
										 , bool bExp )
{
	SProductRecipe* sProductRecipe = GLProductRecipeMan::GetInstance().GetProductRecipe( RecipeID );
	if( sProductRecipe == NULL || pServer == NULL )
		return;

	CHAR_PRODUCT_LOG productLog;
	productLog.dwChaNum = ChaDbNum;
	productLog.dwProductType = sProductRecipe->eProductType;
	productLog.dwProductID = RecipeID;
	productLog.strAction = "MIX";
	productLog.point = 0;
	if( bExp )
		productLog.point = sProductRecipe->wWsp;
	productLog.totalPoint = GetProductSkill( sProductRecipe->eProductType );
	productLog.strStat = "Success";

	// 아이템 생성 성공;
	pServer->AddGameAdoJob(
		db::DbActionPtr(
		new db::CharProductLog( productLog ) ) );
}

// 숙련도 얻는 것에 실패 할 경우;
void GLProductAgent::GenerateItemFail( const DWORD RecipeID
									  , GLAgentServer* pServer
									  , DWORD ChaDbNum )
{
	SProductRecipe* sProductRecipe = GLProductRecipeMan::GetInstance().GetProductRecipe( RecipeID );
	if( sProductRecipe == NULL || pServer == NULL )
		return;

	CHAR_PRODUCT_LOG productLog;
	productLog.dwChaNum = ChaDbNum;
	productLog.dwProductType = sProductRecipe->eProductType;
	productLog.dwProductID = RecipeID;
	productLog.strAction = "MIX";
	productLog.totalPoint = GetProductSkill( sProductRecipe->eProductType );
	productLog.point = 0;
	productLog.strStat = "Fail";

	// 아이템 생성 실패;
	pServer->AddGameAdoJob(
		db::DbActionPtr(
		new db::CharProductLog( productLog ) ) );
}

// 제작 재료 Exchange 로그;
void GLProductAgent::LogProductExchangeMaterial( PRODUCT_EXCHANGE_LOG productLogData
												, GLAgentServer* pServer
												, DWORD ChaDbNum )
{
	if( pServer )
	{
		PRODUCT_EXCHANGE_LOG logData = productLogData;
			
		pServer->AddGameAdoJob(
			db::DbActionPtr( 
			new db::ProductExchangeLog( logData ) ) );
	}
}

// 제작 결과물 Exchange 로그;
void GLProductAgent::LogProductExchange( const DWORD RecipeID
										, LONGLONG llMoney
										, GLAgentServer* pServer
										, DWORD ChaDbNum
										, bool bExp
										, __int64 iResultItemMakeNum )
{
	if( pServer )
	{
		SProductRecipe* sProductRecipe = GLProductRecipeMan::GetInstance().GetProductRecipe( RecipeID );
		if( sProductRecipe == NULL || pServer == NULL )
			return;

		const SITEM* pItem = GLogicData::GetInstance().GetItem( sProductRecipe->sResultItem.sNID );
		if( !pItem )
			return;

		SITEMCUSTOM sITEM_NEW( sProductRecipe->sResultItem.sNID );
		//sITEM_NEW.lnGenNum = iResultItemMakeNum;

		PRODUCT_EXCHANGE_LOG logData;
		logData.sITEM = sITEM_NEW;
		logData.nNum = sITEM_NEW.wTurnNum;
		logData.dwDbNum = ChaDbNum;
		logData.dwProductID = sProductRecipe->dwKey;
		logData.dwProductType = sProductRecipe->eProductType;
		logData.tracePrice = llMoney;
		logData.point = 0;
		if( bExp )
			logData.point = sProductRecipe->wWsp;
		logData.totalPoint = GetProductSkill( sProductRecipe->eProductType );

		pServer->AddGameAdoJob(
			db::DbActionPtr( 
			new db::ProductExchangeLog( logData ) ) );
	}
}

bool GLProductAgent::IsLoadedBook( GLAgentServer* pServer, DWORD dwChaDbNum )
{
    if( pServer == NULL )
        return false;

    // 현재 목록이 로드되지 않은 상태라면 목록을 로드한다.
    if( m_bIsLoadedBook == FALSE )
    {
        pServer->AddGameAdoJob(
            db::DbActionPtr(
            new db::ProductBookLoad( dwChaDbNum )));
        return false;
    }

    return true;
}

bool GLProductAgent::IsLoadedType( GLAgentServer* pServer, DWORD dwChaDbNum )
{
    if( pServer == NULL )
        return false;

    // 현재 목록이 로드되지 않은 상태라면 목록을 로드한다.
    if( m_bIsLoadedType == FALSE )
    {
        pServer->AddGameAdoJob(
            db::DbActionPtr(
                new db::ProductGetType( dwChaDbNum )));
        return false;
    }

    return true;
}

bool GLProductAgent::IsLoadedIsGetExp( GLAgentServer* pServer, DWORD dwChaDbNum )
{
    if( pServer == NULL )
        return false;

    // 현재 목록이 로드되지 않은 상태이고 목록을 로딩하는 중이 아니라면 목록을 로드한다.	
	if( m_bIsLoadedIsGetExp == TRUE && m_bIsLoadingIsGetExp == false )
	{
		return true;
	}
    else if( m_bIsLoadedIsGetExp == FALSE && m_bIsLoadingIsGetExp == false )
    {
        pServer->AddGameAdoJob(
            db::DbActionPtr(
                new db::ProductIsGetExpSelect( dwChaDbNum )));
        return false;
    }
	else if(  m_bIsLoadedIsGetExp == FALSE && m_bIsLoadingIsGetExp == true )
	{
		return true;
	}
	else
	{
		sc::writeLogError( std::string( "IsLoadedIsGetExp ERROR. m_bIsLoadedIsGetExp == TRUE and m_bIsLoadingIsGetExp == TRUE " ) );
		return false;
	}

    return false;
}

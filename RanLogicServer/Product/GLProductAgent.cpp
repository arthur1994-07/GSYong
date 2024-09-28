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

    // ���չ��� �𸣴°�?
    if( CheckProductLearn( pProductRecipe ) == FALSE )
    {
        eState = Product::EStateNotLearn;
        return false;
    }

    const bool bCheckLevel = CheckProductLevel( pProductRecipe, wLevel );
    const bool bCheckSkill = CheckProductSkill( pProductRecipe );
    if ( bCheckLevel == false ) //�ʿ䷹�� �Ҹ�����
    {
        if ( bCheckSkill == false ) //�ʿ䷹��&���õ� �Ҹ�����
        {
            eState = Product::EStateLevelAndSkillFail;
            return false;
        }
        eState = Product::EStateLevelFail;
        return false;
    }
    else if ( bCheckSkill == false )     //�ʿ���õ� �Ҹ�����
    {
        eState = Product::EStateSkillFail;
        return false;
    }
    eState = Product::EStateEnough;
    return true;
}

int GLProductAgent::IsProduct( DWORD nKey, GLAgentServer* pServer, GLCharAG* pChar )
{
    // ����� �ε�Ǿ� ���� �ʴٸ� �۾��� �� �� ����.
    if( !IsLoadedBook( pServer, pChar->m_CharDbNum ) ||
        !IsLoadedType( pServer, pChar->m_CharDbNum ) ||
        !IsLoadedIsGetExp( pServer, pChar->m_CharDbNum ) )
        return Product::EStateERROR;

    SProductRecipe* sProduct = GLProductRecipeMan::GetInstance().GetProductRecipe( nKey );
    if( sProduct == NULL )
        return Product::EStateERROR;

    byte eState;
    if ( CheckProduct_AgentServer( pChar->GetChaLevel(), sProduct, eState )  == false )    //�ʿ䷹���� �ʿ���õ��� �˻�
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

    if( pProductRecipe == NULL || IsUsableProductType( pProductRecipe->eProductType ) == false ) //���չ��� ���� �ִ� �о����� üũ
        return false;

    if( IsLearnedProductRecipe( nProductBook ) )    //���չ��� ������� �ִ��� üũ
        return false;

    return true;
}

void GLProductAgent::SetBookList( const std::vector<DWORD>& vecBookList, GLAgentServer* pServer, GLCharAG* pChar )
{
    if( pChar == NULL || pServer == NULL )
        return;

    m_ProductRecipeLearnSet.clear();

    //����� �ε��Ͽ� ����
    BOOST_FOREACH( const DWORD& dwBookID, vecBookList )
        m_ProductRecipeLearnSet.insert( dwBookID );

    //����� �ε� �Ϸ�Ǹ� ����� Ŭ���̾�Ʈ�� �����Ѵ�.
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

    //����� �ε��Ͽ� ����
    BOOST_FOREACH( const WORD& nType, vecType )
        if( SetStudyProductType( nType, TRUE ) == FALSE )
            sc::writeLogError( sc::string::format( "SetStudyProductType Is Unknown Type:%d, Char:%d", nType, pChar->ChaDbNum() ) );

    //����� �ε� �Ϸ�Ǹ� ����� Ŭ���̾�Ʈ�� �����Ѵ�.
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


    // Ÿ�� �߰�
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
		m_bIsLoadingIsGetExp = true; // ���� ���� ���� �ε���
	}

    //����� �ε��Ͽ� ����
    BOOST_FOREACH( const WORD& dwIsGetExp, vecIsGetExp )
	{
		dwGetExpID = (DWORD) dwIsGetExp;

        if( ExpProductRecipe( dwGetExpID ) == FALSE )
            sc::writeLogError( sc::string::format( "SetIsGetExpList Is Unknown Id:%d, Char:%d", dwGetExpID, pChar->ChaDbNum() ) );
	}

	if( bLast == true )
	{
		//����� �ε� �Ϸ�Ǹ� ����� Ŭ���̾�Ʈ�� �����Ѵ�.
		m_bIsLoadedIsGetExp = TRUE;
		m_bIsLoadingIsGetExp = false;
		MsgSendIsGetExpList( pServer, pChar, true, bFirst, bLast );
	}    
}

void GLProductAgent::AddIsGetExp( DWORD dwID, GLAgentServer* pServer, DWORD dwChaDbNum )
{
    if( pServer == NULL )
        return;

    // ����ġ �߰�
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

		// ������ ������
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

	// ����ġ ȹ�� ���ΰ� Ȯ�ε� �� Ȯ���� ���ؾ� �Ѵ�;
	// ����ġ�� �������� �ִ°�?
	if( IsExpProductRecipe(RecipeID) == true )
		return Product::EStateSuccess;

    // Ȯ���� ���Ѵ�.
    float fRate = (float)sProductRecipe->wWspPer;
    if( !sc::Random::getInstance().IsSucceed( fRate ) )
	{
		// ���� ���õ� ȹ�濡 �����Ͽ����Ƿ� ���� ���� �α׸� �����;
		GenerateItemFail( RecipeID, pServer, ChaDbNum );

        return Product::EStateSuccess;
	}

    // ����ġ�� ��� DB�� ����.
    if( ExpProductRecipe( RecipeID ) == false )
        return Product::EStateSuccess;

    // ����ġ�� ��� ���չ� �� ��쿡�� DB�� �����Ѵ�.;
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
    //DB�۾��� �ʵ弭������ �Ѵ�.
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

        // �ش� Ÿ���� ����
        m_pProductTypeStudyState[wType] = false;
        m_nProductTypeStudyPoint++;
        pServer->AddGameAdoJob(
            db::DbActionPtr( 
            new db::ProductDelType( ChaDbNum, wType )));

        // ��� �����
        PtrVector_ProductRecipe vecRecipe;
        GLProductRecipeMan::GetInstance().Find( vecRecipe, (BYTE)wType );

        // ����ġ�� ���� ���
        std::vector<PRODUCT_ITEM> vecGetExp;

        // ��� ���ռ�
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

        // DB���� ����ġ�� �������� ����
        pServer->AddGameAdoJob(
            db::DbActionPtr( 
                new db::ProductIsGetExpDelete( ChaDbNum, vecGetExp )));

        pServer->AddGameAdoJob(
            db::DbActionPtr( 
                new db::ProductBookDelete( ChaDbNum, vecBookID )));

    }
}

// ���õ� ��°��� ������ ���;
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

	// ������ ���� ����;
	pServer->AddGameAdoJob(
		db::DbActionPtr(
		new db::CharProductLog( productLog ) ) );
}

// ���õ� ��� �Ϳ� ���� �� ���;
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

	// ������ ���� ����;
	pServer->AddGameAdoJob(
		db::DbActionPtr(
		new db::CharProductLog( productLog ) ) );
}

// ���� ��� Exchange �α�;
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

// ���� ����� Exchange �α�;
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

    // ���� ����� �ε���� ���� ���¶�� ����� �ε��Ѵ�.
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

    // ���� ����� �ε���� ���� ���¶�� ����� �ε��Ѵ�.
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

    // ���� ����� �ε���� ���� �����̰� ����� �ε��ϴ� ���� �ƴ϶�� ����� �ε��Ѵ�.	
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

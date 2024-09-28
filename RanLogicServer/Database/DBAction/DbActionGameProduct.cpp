#include "pch.h"
#include "../../../SigmaCore/Log/LogMan.h"
#include "../../../RanLogic/Msg/ProductMsg.h"
#include "../RanLogicServer/Server/AgentServer.h"

#include "../DbManager.h"
#include "./DbActionGameProduct.h"


// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------



namespace db
{
    ProductGetType::ProductGetType( DWORD dwChaDbNum )
        : m_dwChaDbNum(dwChaDbNum)
    {}
    int ProductGetType::Execute( NetServer* pServer )
    {
        if (m_pSubDbManager->GetConnectionType() != db::ADO)
        {
            sc::writeLogError("ProductGetType::Execute. must be use ADO");
            return sc::db::DB_ERROR;
        }


        std::vector<WORD> vecProductType;
        int nReturn = m_pSubDbManager->GetProductType(m_dwChaDbNum, vecProductType);
        

        if(nReturn == sc::db::DB_ERROR)
            sc::writeLogError(sc::string::format("ProductGetType::Execute() Fail. ChaDbNum:%d", m_dwChaDbNum));

        GLMSG::NET_PRODUCT_TYPE_LIST_ACK SendData;
        {
            SendData.m_dwChaDbNum   = m_dwChaDbNum;
            SendData.m_vecType      = vecProductType;
        }
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);

        CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*> (pServer);
        if (pAgentServer)
            pAgentServer->InsertMsg(DBACTION_CLIENT_ID, NET_MSG_PRODUCT_TYPE_LIST_ACK, SendBuffer);
        else
            return sc::db::DB_ERROR;

        return nReturn;
    }

    ProductAddType::ProductAddType( DWORD dwChaNum, int nProdutType )
        : m_dwChaDbNum(dwChaNum)
        , m_nProdutType(nProdutType)
    {}
    int ProductAddType::Execute( NetServer* pServer )
    {
        if (m_pSubDbManager->GetConnectionType() != db::ADO)
        {
            sc::writeLogError("ProductAddType::Execute. must be use ADO");
            return sc::db::DB_ERROR;
        }

        
        int nReturn = m_pSubDbManager->AddProductType(m_dwChaDbNum, m_nProdutType);


        if(nReturn == sc::db::DB_ERROR)
            sc::writeLogError(sc::string::format("ProductAddType::Execute() Fail. ChaDbNum:%d", m_dwChaDbNum));

        return nReturn;
    }

    ProductDelType::ProductDelType( DWORD dwChaNum, int nProdutType )
        : m_dwChaDbNum(dwChaNum)
        , m_nProdutType(nProdutType)
    {}
    int ProductDelType::Execute( NetServer* pServer )
    {
        if (m_pSubDbManager->GetConnectionType() != db::ADO)
        {
            sc::writeLogError("ProductDelType::Execute. must be use ADO");
            return sc::db::DB_ERROR;
        }


        int nReturn = m_pSubDbManager->DelProductType(m_dwChaDbNum, m_nProdutType);


        if(nReturn == sc::db::DB_ERROR)
            sc::writeLogError(sc::string::format("ProductGetType::Execute() Fail. ChaDbNum:%d", m_dwChaDbNum));

        return nReturn;
    }

    ProductIsGetExpSelect::ProductIsGetExpSelect( DWORD dwChaNum)
        : m_dwChaDbNum(dwChaNum)
    {}
    int ProductIsGetExpSelect::Execute( NetServer* pServer )
    {
        if (m_pSubDbManager->GetConnectionType() != db::ADO)
        {
            sc::writeLogError("ProductIsGetExpSelect::Execute. must be use ADO");
            return sc::db::DB_ERROR;
        }

    
        std::vector<WORD> vecIsGetExp;
        int nReturn = m_pSubDbManager->GetProductIsGetExp(m_dwChaDbNum, vecIsGetExp);


        if(nReturn == sc::db::DB_ERROR)
            sc::writeLogError(sc::string::format("ProductIsGetExpSelect::Execute() Fail. ChaDbNum:%d", m_dwChaDbNum));

        CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*> (pServer);
        if (pAgentServer)
		{
			std::vector<WORD> vecProductID;
			bool bFirst = true;

			vecProductID.clear();

			// 전송 시작 메세지
			//GLMSG::NET_PRODUCT_ISGETEXP_LIST_ACK SendDataStart;
			//SendDataStart.m_dwChaDbNum = m_dwChaDbNum;
			//SendDataStart.m_vecIsGetExp = vecProductID;
			//SendDataStart.m_bFirst = true;
			//SendDataStart.m_bLast = false;
			//msgpack::sbuffer SendBufferStart;
			//msgpack::pack(SendBufferStart, SendDataStart);
			//pAgentServer->InsertMsg(DBACTION_CLIENT_ID, NET_MSG_PRODUCT_ISGETEXP_LIST_ACK, SendBufferStart);

			// 실제 제작정보 전송
			for( DWORD i=0; i<vecIsGetExp.size(); )
			{
				vecProductID.push_back( (WORD)vecIsGetExp.at(i) );
				i++;

				if( i % 100 == 0 )
				{
					GLMSG::NET_PRODUCT_ISGETEXP_LIST_ACK SendData;
					SendData.m_dwChaDbNum = m_dwChaDbNum;
					SendData.m_vecIsGetExp = vecProductID;
					bFirst == true ? SendData.m_bFirst = true : SendData.m_bFirst = false;
					SendData.m_bLast = false;
					msgpack::sbuffer SendBuffer;
					msgpack::pack(SendBuffer, SendData);
					pAgentServer->InsertMsg(DBACTION_CLIENT_ID, NET_MSG_PRODUCT_ISGETEXP_LIST_ACK, SendBuffer);

					vecProductID.clear();
					bFirst = false;
				}
			}

			// 전송 종료 메세지
			GLMSG::NET_PRODUCT_ISGETEXP_LIST_ACK SendDataEnd;
			SendDataEnd.m_dwChaDbNum = m_dwChaDbNum;
			SendDataEnd.m_vecIsGetExp = vecProductID;
			SendDataEnd.m_bFirst = false;
			SendDataEnd.m_bLast = true;
			msgpack::sbuffer SendBufferEnd;
			msgpack::pack(SendBufferEnd, SendDataEnd);
			pAgentServer->InsertMsg(DBACTION_CLIENT_ID, NET_MSG_PRODUCT_ISGETEXP_LIST_ACK, SendBufferEnd);			

		}
        else
            return sc::db::DB_ERROR;

        return nReturn;
    }

    ProductIsGetExpInsert::ProductIsGetExpInsert( DWORD dwChaNum, DWORD dwProductID )
        : m_dwChaDbNum(dwChaNum)
        , m_dwProductID(dwProductID)
    {}
    int ProductIsGetExpInsert::Execute( NetServer* pServer )
    {
        if (m_pSubDbManager->GetConnectionType() != db::ADO)
        {
            sc::writeLogError("ProductIsGetExpInsert::Execute. must be use ADO");
            return sc::db::DB_ERROR;
        }


        int nReturn = m_pSubDbManager->AddProductIsGetExp(m_dwChaDbNum, m_dwProductID);


        if(nReturn == sc::db::DB_ERROR)
            sc::writeLogError(sc::string::format("ProductIsGetExpInsert::Execute() Fail. ChaDbNum:%d", m_dwChaDbNum));

        return nReturn;
    }

    ProductIsGetExpDelete::ProductIsGetExpDelete( DWORD dwChaNum,
		const std::vector<PRODUCT_ITEM>& vecProductID )
        : m_dwChaDbNum(dwChaNum)
    {
        m_vecProductID = vecProductID;
    }
    int ProductIsGetExpDelete::Execute( NetServer* pServer )
    {
        if (m_pSubDbManager->GetConnectionType() != db::ADO)
        {
            sc::writeLogError("ProductIsGetExpDelete::Execute. must be use ADO");
            return sc::db::DB_ERROR;
        }

        BOOST_FOREACH( const PRODUCT_ITEM& productItem, m_vecProductID )
            if( m_pSubDbManager->DelProductIsGetExp(m_dwChaDbNum, productItem) == sc::db::DB_ERROR )
                sc::writeLogError(sc::string::format("ProductIsGetExpDelete::Execute() Fail. ChaDbNum:%d", m_dwChaDbNum));

        return sc::db::DB_OK;
    }


    ProductBookSave::ProductBookSave( DWORD dwChaDbNum, const std::vector<DWORD>& setLearnProductBook )
        : m_dwChaDbNum(dwChaDbNum)
    {   
        m_vecLearnProductBook = setLearnProductBook;
    }
    int ProductBookSave::Execute( NetServer* pServer )
    {
        if (m_pSubDbManager->GetConnectionType() != db::ADO)
        {
            sc::writeLogError("ProductBookSave::Execute. must be use ADO");
            return sc::db::DB_ERROR;
        }


        int nReturn = m_pSubDbManager->SaveProductBook(m_dwChaDbNum, m_vecLearnProductBook);


        if(nReturn == sc::db::DB_ERROR)
            sc::writeLogError(sc::string::format("ProductBookSave::Execute() Fail. ChaDbNum:%d", m_dwChaDbNum));

        return nReturn;
    }

    ProductBookLoad::ProductBookLoad( DWORD dwChaDbNum )
        : m_dwChaDbNum(dwChaDbNum)
    {}

    int ProductBookLoad::Execute( NetServer* pServer )
    {
        if (m_pSubDbManager->GetConnectionType() != db::ADO)
        {
            sc::writeLogError("ProductBookLoad::Execute. must be use ADO");
            return sc::db::DB_ERROR;
        }


        std::vector<DWORD> vecLearnProductBook;
        int nReturn = m_pSubDbManager->LoadProductBook(m_dwChaDbNum, vecLearnProductBook);


        if(nReturn == sc::db::DB_ERROR)
            sc::writeLogError(sc::string::format("ProductBookLoad::Execute() Fail. ChaDbNum:%d", m_dwChaDbNum));



        GLMSG::NET_PRODUCT_BOOK_LIST_ACK SendData;
        {
            SendData.m_dwChaDbNum = m_dwChaDbNum;
            SendData.m_vecBookList = vecLearnProductBook;
        }

        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);

        CAgentServer* pAgentServer = reinterpret_cast<CAgentServer*> (pServer);
        if (pAgentServer)
            pAgentServer->InsertMsg(DBACTION_CLIENT_ID, NET_MSG_PRODUCTBOOK, SendBuffer);
        else
            return sc::db::DB_ERROR;


        return nReturn;
    }




    ProductBookDelete::ProductBookDelete( DWORD dwChaDbNum, const std::vector<PRODUCT_ITEM>& vecBookID )
        : m_dwChaDbNum( dwChaDbNum )
    {
        m_vecBookID = vecBookID;
    }


    int ProductBookDelete::Execute( NetServer* pServer )
    {

        if (m_pSubDbManager->GetConnectionType() != db::ADO)
        {
            sc::writeLogError("ProductBookDelete::Execute. must be use ADO");
            return sc::db::DB_ERROR;
        }

        int nReturn = m_pSubDbManager->DeleteProductBook( m_dwChaDbNum, m_vecBookID );


        if(nReturn == sc::db::DB_ERROR)
            sc::writeLogError(sc::string::format("ProductBookDelete::Execute() Fail. ChaDbNum:%d", m_dwChaDbNum));

        return sc::db::DB_OK;
    }




	CharProductLog::CharProductLog( const CHAR_PRODUCT_LOG& productLog )
		: m_productLog( productLog )
	{
	}

	int CharProductLog::Execute( NetServer* pServer )
	{

		if( m_pSubDbManager->GetConnectionType() != db::ADO )
		{
			sc::writeLogError("CharProductLog::Execute. must be use ADO");
			return sc::db::DB_ERROR;
		}

		int nReturn = m_pSubDbManager->CharProductLog( m_productLog );

		if( nReturn == sc::db::DB_ERROR )
		{
			sc::writeLogError( "CharProductLog::Execute() Fail." );
		}

		return sc::db::DB_OK;
	}


	ProductExchangeLog::ProductExchangeLog( const PRODUCT_EXCHANGE_LOG& exchangeLog )
		: m_exchangeLog( exchangeLog )
	{
	}

	int ProductExchangeLog::Execute( NetServer* pServer )
	{

		if( m_pSubDbManager->GetConnectionType() != db::ADO )
		{
			sc::writeLogError("ProductExchangeLog::Execute. must be use ADO");
			return sc::db::DB_ERROR;
		}

		int nReturn = m_pSubDbManager->ProductExchangeLog( m_exchangeLog );

		if( nReturn == sc::db::DB_ERROR )
		{
			sc::writeLogError( "ProductExchangeLog::Execute() Fail." );
		}

		return sc::db::DB_OK;
	}

} // namespace db
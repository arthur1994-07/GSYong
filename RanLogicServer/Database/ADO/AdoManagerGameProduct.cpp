#include "pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "./AdoManager.h"

#include "../../SigmaCore/DebugInclude.h"

#include "../../../RanLogic/Product/GLProductRecipeMan.h"

namespace db
{
    int AdoManager::AddProductType( const DWORD dwChaNum, const WORD nProductType )
    {
        /* 
        [dbo].[ChaFriendGroupInsert]
            @ChaNum			int,
            @ProductType	int,
            @nReturn		int OUTPUT
        */
        sc::db::AdoExt Ado(m_GameDBConnString);

        APPEND_IPARAM_INT(Ado, "@ChaNum"        , dwChaNum);
        APPEND_IPARAM_INT(Ado, "@ProductType"   , nProductType);
        APPEND_OPARAM_INT(Ado, "@nReturn");

        if( ExecuteStoredProcedureIntReturn("dbo.ProductTypeInsert", Ado, "@nReturn")
			== sc::db::DB_ERROR )
			return sc::db::DB_ERROR;

		// Add Product Type Log;
		CHAR_PRODUCT_LOG productLog;
		productLog.dwChaNum = dwChaNum;
		productLog.strAction = "LEARN";
		productLog.dwProductType = nProductType;
		productLog.strStat = "Default";

		return CharProductLog( productLog );
    }

    int AdoManager::GetProductType(const DWORD dwChaNum, std::vector<WORD>& vecProductType OUT)
    {
        /* 
        [dbo].[ProductTypeSelect]
            @ChaNum int,
            @ChaProductTypePoint int OUTPUT
        */
        sc::db::AdoExt Ado(m_GameDBConnString);

        APPEND_IPARAM_INT(Ado, "@ChaNum", dwChaNum);

        if (!Ado.Execute4Cmd("dbo.ProductTypeSelect", adCmdStoredProc))	
            return sc::db::DB_ERROR;

        if (Ado.GetEOF())
            return sc::db::DB_OK;
        do
        {   
            int nProductType;
            Ado.GetCollect("ProductType", nProductType);

            vecProductType.push_back( nProductType );

        } while (Ado.Next());

        return sc::db::DB_OK;
    }


    int AdoManager::DelProductType( const DWORD dwChaNum, const WORD nProductType )
    {
        /* 
        [dbo].[ProductTypeDelete]
            @ChaNum			int,
            @ProductType	int,
            @nReturn		int OUTPUT
        */
        sc::db::AdoExt Ado(m_GameDBConnString);

        APPEND_IPARAM_INT(Ado, "@ChaNum"        , dwChaNum);
        APPEND_IPARAM_INT(Ado, "@ProductType"   , nProductType);
        APPEND_OPARAM_INT(Ado, "@nReturn");

        if( ExecuteStoredProcedureIntReturn("dbo.ProductTypeDelete", Ado, "@nReturn")
			== sc::db::DB_ERROR)
			return sc::db::DB_ERROR;

		// Add Product Type Log;
		CHAR_PRODUCT_LOG productLog;
		productLog.dwChaNum = dwChaNum;
		productLog.strAction = "RESET";
		productLog.dwProductType = nProductType;
		productLog.strStat = "Default";

		return CharProductLog( productLog );
    }


    int AdoManager::AddProductIsGetExp( const DWORD dwChaNum, const DWORD dwProductID )
    {
        /* 
        [dbo].[ProductIsGetExpInsert]
            @ChaNum		int,
            @ProductID	int,
            @nReturn	int OUTPUT
        */
        sc::db::AdoExt Ado(m_GameDBConnString);

        APPEND_IPARAM_INT(Ado, "@ChaNum"    , dwChaNum);
        APPEND_IPARAM_INT(Ado, "@ProductID" , dwProductID);
        APPEND_OPARAM_INT(Ado, "@nReturn");

        return ExecuteStoredProcedureIntReturn("dbo.ProductIsGetExpInsert", Ado, "@nReturn");
    }


    int AdoManager::GetProductIsGetExp( const DWORD dwChaNum, std::vector<WORD>& vecProductID OUT)
    {
        /* 
        [dbo].[ProductIsGetExpSelect]
            @ChaNum int
        */
        sc::db::AdoExt Ado(m_GameDBConnString);
        APPEND_IPARAM_INT(Ado, "@ChaNum", dwChaNum);

        if (!Ado.Execute4Cmd("dbo.ProductIsGetExpSelect", adCmdStoredProc))	
            return sc::db::DB_ERROR;

        if (Ado.GetEOF())
            return sc::db::DB_OK;

        do
        {   
            DWORD   dwProductType;
            Ado.GetCollect("ProductID", dwProductType);

            vecProductID.push_back( (WORD)dwProductType );
        } while (Ado.Next());

        return sc::db::DB_OK;
    }


    int AdoManager::DelProductIsGetExp( const DWORD dwChaNum, const PRODUCT_ITEM& productItem )
    {
        /* 
        [dbo].[ProductIsGetExpDelete]
            @ChaNum		int,
            @ProductID	int,
            @nReturn	int OUTPUT
        */
        sc::db::AdoExt Ado(m_GameDBConnString);

        APPEND_IPARAM_INT(Ado, "@ChaNum"    , dwChaNum);
        APPEND_IPARAM_INT(Ado, "@ProductID" , productItem.dwProductID);
        APPEND_OPARAM_INT(Ado, "@nReturn");

        if( ExecuteStoredProcedureIntReturn("dbo.ProductIsGetExpDelete", Ado, "@nReturn")
			== sc::db::DB_ERROR )
			return sc::db::DB_ERROR;

		// Add Product Type Log;
		CHAR_PRODUCT_LOG productLog;
		productLog.dwChaNum = dwChaNum;
		productLog.strAction = "RESET MIX";
		productLog.dwProductType = productItem.dwProductType;
		productLog.dwProductID = productItem.dwProductID;
		productLog.strStat = "Default";
		productLog.point = productItem.point;

		return CharProductLog( productLog );
    }

    // ±‚¡∏ DB JOB
    int AdoManager::SaveProductBook(DWORD nChaNum, const std::vector<DWORD>& setLearnProductBook)
    {	
	    BOOST_FOREACH( const DWORD& dwBookID, setLearnProductBook )
	    {
		    sc::db::AdoExt ado(m_GameDBConnString);

		    APPEND_IPARAM_INT(ado, "@ChaNum", nChaNum);
		    APPEND_IPARAM_INT(ado, "@BookID", dwBookID);

		    if (ExecuteStoredProcedure("dbo.sp_ProductBookSave", ado) != sc::db::DB_OK)
		    {
			    sc::writeLogError(std::string("AdoManager::SaveProductBook() - ExecuteStoredProcedure failure."));

			    return sc::db::DB_ERROR;
		    }

			SProductRecipe* pRecipe =
				GLProductRecipeMan::GetInstance().GetProductRecipe( dwBookID );
			if( pRecipe )
			{
				// Add Product Type Log;
				CHAR_PRODUCT_LOG productLog;
				productLog.dwChaNum = nChaNum;
				productLog.strAction = "IMS";
				productLog.dwProductType = pRecipe->eProductType;
				productLog.dwProductID = pRecipe->dwKey;
				productLog.strStat = "Default";
				productLog.point = 0;

				if( CharProductLog( productLog ) == sc::db::DB_ERROR )
					return sc::db::DB_ERROR;
			}
	    }

	    return sc::db::DB_OK;
    }

    int AdoManager::LoadProductBook(DWORD nChaNum, std::vector<DWORD>& setLearnProductBook OUT)
    {
	    sc::db::AdoExt ado(m_GameDBConnString);

	    APPEND_IPARAM_INT(ado, "@ChaNum",	nChaNum);

	    if (ExecuteStoredProcedure("dbo.sp_ProductBookLoad", ado) != sc::db::DB_OK)
	    {
		    sc::writeLogError(std::string("AdoManager::LoadProductBook() - ExecuteStoredProcedure failure."));

		    return sc::db::DB_ERROR;
	    }

	    BEGIN_GETCOLLECT(ado);

	    DWORD nProductBook = 0;
	    ado.GetCollect("BookID", nProductBook);

	    setLearnProductBook.push_back( nProductBook );
    	
	    END_GETCOLLECT(ado);


	    return sc::db::DB_OK;
    }

    int AdoManager::DeleteProductBook( DWORD nChaNum, const std::vector<PRODUCT_ITEM>& vecBook )
    {
        /* 
        [dbo].[ProductTypeDelete]
            @ChaNum			int,
            @BookID	        int,
            @nReturn		int OUTPUT
        */
        BOOST_FOREACH( const PRODUCT_ITEM& productItem, vecBook )
        {
            sc::db::AdoExt Ado(m_GameDBConnString);

            APPEND_IPARAM_INT(Ado, "@ChaNum"    , nChaNum);
            APPEND_IPARAM_INT(Ado, "@BookID"    , productItem.dwProductID);
            APPEND_OPARAM_INT(Ado, "@nReturn");

            if (ExecuteStoredProcedure("dbo.ProductBookDelete", Ado) != sc::db::DB_OK)
            {
                sc::writeLogError(std::string("AdoManager::DeleteProductBook() - ExecuteStoredProcedure failure."));
                return sc::db::DB_ERROR;
            }

			// Add Product Type Log;
			CHAR_PRODUCT_LOG productLog;
			productLog.dwChaNum = nChaNum;
			productLog.strAction = "RESET MP";
			productLog.dwProductType = productItem.dwProductType;
			productLog.dwProductID = productItem.dwProductID;
			productLog.strStat = "Default";
			productLog.point = 0;

			if( CharProductLog( productLog ) == sc::db::DB_ERROR )
				return sc::db::DB_ERROR;
        }
        return sc::db::DB_OK;
    }

	int AdoManager::CharProductLog( const CHAR_PRODUCT_LOG& productLog )
	{
		// Add Product Type Log;
		sc::db::AdoExt adoLog( m_LogDBConnString );

		APPEND_IPARAM_INT( adoLog, "@ChaNum", productLog.dwChaNum );
		APPEND_IPARAM_VARCHAR( adoLog, "@Action", productLog.strAction.c_str(), 10 );
		APPEND_IPARAM_INT( adoLog, "@Type", productLog.dwProductType );
		APPEND_IPARAM_INT( adoLog, "@ID", productLog.dwProductID );
		APPEND_IPARAM_VARCHAR( adoLog, "@Stat", productLog.strStat.c_str(), 10 );
		APPEND_IPARAM_INT( adoLog, "@Point", productLog.point );
		APPEND_IPARAM_INT( adoLog, "@TotalPoint", productLog.totalPoint );

		return ExecuteStoredProcedure(
			"dbo.sp_CharProductLog", adoLog );
	}

	int AdoManager::ProductExchangeLog( const PRODUCT_EXCHANGE_LOG& productLog )
	{
		/*
			@NIDMain		int, 
			@NIDSub			int, 
			@SvrNum			int, 
			@FldNum			int,
			@MakeType		int,
			@MakeNum		bigint,
			@ChaNum			int,
			@ItemAmount		int,
			@ActionFlag		int,
			@Attack			int,
			@Defense		int,
			@ProductID		int,
			@ProductType	int,
			@Point			int,
			@TotalPoint		int,
			@TradePrice		bigint
		*/

		// Add Product Exchange Log;
		sc::db::AdoExt adoLog( m_LogDBConnString );

		APPEND_IPARAM_INT( adoLog, "@NIDMain", productLog.sITEM.GetNativeID().wMainID );
		APPEND_IPARAM_INT( adoLog, "@NIDSub", productLog.sITEM.GetNativeID().wSubID );
		APPEND_IPARAM_INT( adoLog, "@SvrNum", static_cast<int>( 0/*productLog.sITEM.cChnID*/ ) );
		APPEND_IPARAM_INT( adoLog, "@FldNum", sc::db::AdjustTinyInt( 0/*productLog.sITEM.cFieldID*/ ) );
		APPEND_IPARAM_INT( adoLog, "@MakeType", static_cast<int>( productLog.sITEM.cGenType ) );
		APPEND_IPARAM_BIGINT( adoLog, "@MakeNum", 0/*productLog.sITEM.lnGenNum*/ );

		APPEND_IPARAM_INT( adoLog, "@ChaNum", productLog.dwDbNum );
		APPEND_IPARAM_INT( adoLog, "@ItemAmount", productLog.nNum );
		//APPEND_IPARAM_INT( adoLog, "@ActionFlag", static_cast<int>( productLog.routeType ) );
		APPEND_IPARAM_INT( adoLog, "@Attack", static_cast<int>( productLog.sITEM.GETGRADE_DAMAGE() ) );
		APPEND_IPARAM_INT( adoLog, "@Defense", static_cast<int>( productLog.sITEM.GETGRADE_DEFENSE() ) );

		APPEND_IPARAM_INT( adoLog, "@ProductID", productLog.dwProductID );
		APPEND_IPARAM_INT( adoLog, "@ProductType", productLog.dwProductType );
		APPEND_IPARAM_INT( adoLog, "@Point", productLog.point );
		APPEND_IPARAM_INT( adoLog, "@TotalPoint", productLog.totalPoint );
		APPEND_IPARAM_BIGINT( adoLog, "@TradePrice", productLog.tracePrice );

		return ExecuteStoredProcedure(
			"dbo.sp_ProductLogExchange", adoLog );
	}


} // namespace db
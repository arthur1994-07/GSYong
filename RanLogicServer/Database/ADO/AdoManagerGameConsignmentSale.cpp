#include "pch.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "./AdoManager.h"

#include "../../SigmaCore/DebugInclude.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Util/DateTime.h"

namespace db
{

int AdoManager::ConsignmentsSaleGet( sc::MGUID& Guid, private_market::ConsignmentSaleItem& SaleItem, bool bDateCheck/* = false */ )
{
/*
CREATE PROCEDURE dbo.ConsignmentSaleGet
@ItemGuid uniqueidentifier,
@DateCheck int

SELECT 
 [ChaNum]
,[ChaName]
,[ExpirationDate]
,[RegistItem]
,[TurnNum]
,[Price]
,[SellType]
,[TotalTurnNum]
,[ServerGroup]
*/
    SaleItem.SetGUID( Guid );

    std::string strGuid = sc::string::uuidToString( static_cast< sc::MGUID >( Guid ) );
    sc::string::upperString( strGuid );

    sc::db::AdoExt Ado( m_ShopDBConnString );
    APPEND_IPARAM_VARCHAR( Ado, "@ItemGuid", strGuid.c_str(), static_cast< long >( strGuid.length() ) );
    APPEND_IPARAM_INT    ( Ado, "@DateCheck", static_cast< int >( bDateCheck ) );

    if ( !Ado.Execute4Cmd( "dbo.ConsignmentSaleGet", adCmdStoredProc ) )	
        return sc::db::DB_ERROR;

    if ( Ado.GetEOF() )
	{
		sc::writeLogError( sc::string::format( "AdoManager::ConsignmentsSaleGet() - GUID ERRO (%1%).", strGuid ) );
        return sc::db::DB_ERROR;
	}

    _variant_t Date;				// ExpirationDate
    std::vector< BYTE > vBuffer;	// RegistItem

    Ado.GetCollect( "ChaNum", SaleItem.dwChaDbNum );
    Ado.GetCollect( "ChaName", SaleItem.strChaName );
    Ado.GetCollect( "ExpirationDate", Date );
    Ado.GetChunk( "RegistItem", vBuffer);        
    Ado.GetCollect( "TurnNum", SaleItem.wTurnNum );
    _variant_t Money;
    Ado.GetCollect( "Price", Money );

    _variant_t temp;
    temp.ChangeType( VT_UI8, &Money );

    SaleItem.llPrice = temp;

    Ado.GetCollect( "SellType", SaleItem.SellType );
    Ado.GetCollect( "TotalTurnNum", SaleItem.wTotalTurnNum );
	Ado.GetCollect( "ServerGroup", SaleItem.ServerGroup );

    // ExpirationDate
    if ( Date.GetVARIANT().vt == VT_DATE )
        SaleItem.ExpirationDate = sc::time::GetTime( Date );

    // RegistItem
    se::ByteStream ByteStream( vBuffer );
    SITEMCUSTOM ITEMCUSTOM( NATIVEID_NULL() );
    if ( !ByteStream.IsEmpty() )
        ITEMCUSTOM.AutoAssign( ByteStream );
    SaleItem.sItemcustom = ITEMCUSTOM;

    return sc::db::DB_OK;
}

int AdoManager::ConsignmentsSaleSelect( private_market::SALE_ITEMS_VECTOR& vecItem, const DWORD& dwChaNum, const int& ServerGroup )
{
/*
CREATE PROCEDURE dbo.ConsignmentSaleSelect
@ChaNum int, 
@ServerGroup int, 

SELECT 
[ItemGuid]
,[ChaNum]
,[ChaName]
,[ExpirationDate]
,[RegistItem]
,[TurnNum]
,[Price]
,[SellType]
,[TotalTurnNum]
,[ServerGroup]
*/

    sc::db::AdoExt Ado( m_ShopDBConnString );
    APPEND_IPARAM_INT( Ado, "@ChaNum", dwChaNum );
	APPEND_IPARAM_INT( Ado, "@ServerGroup", ServerGroup );

    if ( !Ado.Execute4Cmd( "dbo.ConsignmentSaleSelect", adCmdStoredProc ) )	
        return sc::db::DB_ERROR;

    if ( Ado.GetEOF() )
        return sc::db::DB_OK;

    do
    {   
        private_market::ConsignmentSaleItem SaleItem;

        std::string strGuid;			// ItemGuid
        _variant_t Date;				// ExpirationDate
        std::vector< BYTE > vBuffer;	// RegistItem

        Ado.GetCollect( "ItemGuid", strGuid );
        Ado.GetCollect( "ChaNum", SaleItem.dwChaDbNum );
        Ado.GetCollect( "ChaName", SaleItem.strChaName );
        Ado.GetCollect( "ExpirationDate", Date );
        Ado.GetChunk( "RegistItem", vBuffer);        
        Ado.GetCollect( "TurnNum", SaleItem.wTurnNum );
        
        _variant_t Money;
        Ado.GetCollect( "Price", Money );
        
        _variant_t temp;
        temp.ChangeType( VT_UI8, &Money );

        SaleItem.llPrice = temp;
        
        Ado.GetCollect( "SellType", SaleItem.SellType );
        Ado.GetCollect( "TotalTurnNum", SaleItem.wTotalTurnNum );

		Ado.GetCollect( "ServerGroup", SaleItem.ServerGroup );

        // ItemGuid
        SaleItem.SetStrGUID( strGuid );

        // ExpirationDate
        if ( Date.GetVARIANT().vt == VT_DATE )
            SaleItem.ExpirationDate = sc::time::GetTime( Date );

        // RegistItem
        se::ByteStream ByteStream( vBuffer );
        SITEMCUSTOM ITEMCUSTOM( NATIVEID_NULL() );
        if ( !ByteStream.IsEmpty() )
            ITEMCUSTOM.AutoAssign( ByteStream );
        SaleItem.sItemcustom = ITEMCUSTOM;

        vecItem.push_back( SaleItem );
    } while ( Ado.Next() );

    return sc::db::DB_OK;
}

int AdoManager::ConsignmentsUpdateTurnNum( sc::MGUID& Guid, int nTurnNum )
{
/*
CREATE PROCEDURE dbo.ConsignmentSaleUpdateTurnNum
    @ItemGuid	uniqueidentifier,
    @TurnNum	int,
    @nReturn int OUTPUT
AS	
*/

    std::string strGuid = sc::string::uuidToString( static_cast< sc::MGUID >( Guid ) );
    sc::string::upperString( strGuid );

    sc::db::AdoExt Ado( m_ShopDBConnString );
    APPEND_IPARAM_VARCHAR( Ado, "@ItemGuid" , strGuid.c_str(), static_cast< long >( strGuid.length() ) );
    APPEND_IPARAM_INT( Ado, "@TurnNum"  , nTurnNum );
    
    APPEND_OPARAM_BIGINT( Ado, "@nReturn" );

    return ExecuteStoredProcedure( "dbo.ConsignmentSaleUpdateTurnNum", Ado );
}

int AdoManager::ConsignmentsSaleInsert( private_market::ConsignmentSaleItem& SaleItem )
{
/*
@ItemGuid		uniqueidentifier,
@ChaNum			int,
@ExpirationDate	datetime,
@RegistItem		image,
@TurnNum		int,
@Price			money,
@SellType		int,
@TotalTurnNum	int,
@nReturn		int OUTPUT
*/
    sc::db::AdoExt Ado( m_ShopDBConnString );

    std::string strGuid = SaleItem.GetStrGUID();

    std::string StrDate = sc::time::DateTimeFormat( SaleItem.ExpirationDate, true, true );

    _variant_t vartBLOB;
    ULONG nSizeOfChunk = 0;
    se::ByteStream ByteStream;
    LPBYTE pBuffer = NULL;
    DWORD dwSize = 0;

    ByteStream.ClearBuffer();
    ByteStream << SITEMCUSTOM::VERSION;
    ByteStream << ( DWORD ) sizeof( SITEMCUSTOM );
    dwSize = ( DWORD ) 1;
    ByteStream << dwSize;
    ByteStream.WriteBuffer( ( LPBYTE ) &SaleItem.sItemcustom, ( DWORD ) sizeof( SITEMCUSTOM ) * dwSize);        
    if ( !Ado.MakeChunk( vartBLOB, ByteStream, nSizeOfChunk ) )
        sc::writeLogError( sc::string::format( "PostSend Makechunk Size %1%", nSizeOfChunk ) );

    APPEND_IPARAM_VARCHAR( Ado, "@ItemGuid", strGuid.c_str(), static_cast< long >( strGuid.length() ) );
    APPEND_IPARAM_INT( Ado, "@ChaNum", SaleItem.dwChaDbNum );
    APPEND_IPARAM_VARCHAR( Ado, "@ExpirationDate", StrDate.c_str(), static_cast< long >( StrDate.length() ) );
    APPEND_IPARAM_IMAGE( Ado, "@RegistItem", vartBLOB, nSizeOfChunk );
    APPEND_IPARAM_INT( Ado, "@TurnNum", SaleItem.wTurnNum );
    APPEND_IPARAM_BIGINT( Ado, "@Price", SaleItem.llPrice );
    APPEND_IPARAM_INT( Ado, "@SellType", SaleItem.SellType );
    APPEND_IPARAM_INT( Ado, "@TotalTurnNum", SaleItem.wTurnNum );
	APPEND_IPARAM_INT( Ado, "@ServerGroup", SaleItem.ServerGroup );
	APPEND_IPARAM_VARCHAR( Ado, "@ChaName", SaleItem.strChaName.c_str(), static_cast< long >( SaleItem.strChaName.length() ) ); // strlen
    APPEND_OPARAM_BIGINT ( Ado, "@nReturn" );

    return ExecuteStoredProcedure( "dbo.ConsignmentSaleInsert", Ado );
}

int AdoManager::ConsignmentsSaleDelete( sc::MGUID& Guid )
{
/*
    CREATE PROCEDURE dbo.ConsignmentSaleDelete
    @ItemGuid		uniqueidentifier,
    @nReturn		int OUTPUT

    DELETE FROM  [dbo].[ConsignmentSale]
    WHERE [dbo].[ConsignmentSale].[ItemGuid] = @ItemGuid
*/

    std::string strGuid = sc::string::uuidToString( Guid );
    sc::string::upperString( strGuid );

    sc::db::AdoExt Ado( m_ShopDBConnString );

    APPEND_IPARAM_VARCHAR( Ado, "@ItemGuid", strGuid.c_str(), static_cast< long >( strGuid.length() ) );
    APPEND_OPARAM_BIGINT ( Ado, "@nReturn" );

    return ExecuteStoredProcedure( "dbo.ConsignmentSaleDelete", Ado );
}

int AdoManager::ConfignmentSaleSlotExpansionDateSelect( SCHARDATA2* pChaData2 )
{
    sc::db::AdoExt Ado( m_GameDBConnString );
    
    APPEND_IPARAM_INT( Ado, "@ChaNum", pChaData2->CharDbNum() );

    if ( FALSE == Ado.ExecuteStoredProcedure( "dbo.ConsignmentSaleSlotExpansionDateSelect" ) )
        return sc::db::DB_ERROR;

    if ( Ado.GetEOF() )    
        return sc::db::DB_ERROR;

    _variant_t Date;            // ExpirationDate
    Ado.GetCollect( "ChaSaleSlotExpansionDate", Date );      

    __time64_t tExpansionEndDate = 0;

    // ExpirationDate
    if ( Date.GetVARIANT().vt == VT_DATE )
        tExpansionEndDate = sc::time::GetTime(Date);

    pChaData2->SetConsignmentSaleSlotExpansionDate( tExpansionEndDate );

    return sc::db::DB_OK;
}

int AdoManager::ConfignmentSaleSlotExpansionDateSet( DWORD dwChaDbNum, __time64_t tExpansionDate )
{
    std::string StrDate = sc::time::DateTimeFormat( tExpansionDate, true, true );

    sc::db::AdoExt Ado( m_GameDBConnString );
    APPEND_IPARAM_INT( Ado, "@ChaNum", dwChaDbNum );
    APPEND_IPARAM_VARCHAR( Ado, "@SlotExpansionDate", StrDate.c_str(), static_cast< long >( StrDate.length() ) );

    APPEND_OPARAM_BIGINT( Ado, "@nReturn" );

    return ExecuteStoredProcedure( "dbo.ConsignmentSaleSlotExpansionDateSet", Ado );
}


} // namespace db

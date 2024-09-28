#include "pch.h"
#include "WishList.h"

#include <boost/functional/hash.hpp>
#include <boost/foreach.hpp>

#include "../../SigmaCore/File/FileUtil.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/Util/SystemInfo.h"

#include "../../EngineLib/Common/SUBPATH.h"
#include "../../EngineLib/G-Logic/TypeDefine.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/GLUseFeatures.h"

#include "../GLGaeaClient.h"

#include "../Widget/GLWidgetScript.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace WishList
{
	SItemBase::SItemBase()
	{
	}

	SItemBase::SItemBase( const SITEMCUSTOM& sItem )
		: m_sItem( sItem )
	{
	}

	void SItemBase::SaveFile( sc::SerialFile& SFile )
	{
		SFile << DWORD( Version );
		SFile.BeginBlock();
		{
			SFile << m_sItem.GetNativeID().dwID;
			SFile << m_sItem.nidDISGUISE.dwID;
			SFile << m_sItem.tBORNTIME;
			SFile << m_sItem.tDISGUISE;

			SFile << m_sItem.wTurnNum;

			SFile << m_sItem.cDAMAGE;
			SFile << m_sItem.cDEFENSE;
			SFile << m_sItem.cRESIST_FIRE;
			SFile << m_sItem.cRESIST_ICE;
			SFile << m_sItem.cRESIST_ELEC;
			SFile << m_sItem.cRESIST_POISON;
			SFile << m_sItem.cRESIST_SPIRIT;

			SFile << static_cast< BYTE >( m_sItem.GETOptTYPE1() );
			SFile << static_cast< BYTE >( m_sItem.GETOptTYPE2() );
			SFile << static_cast< BYTE >( m_sItem.GETOptTYPE3() );
			SFile << static_cast< BYTE >( m_sItem.GETOptTYPE4() );

			SFile << m_sItem.randomOption.getValue(0);
			SFile << m_sItem.randomOption.getValue(1);
			SFile << m_sItem.randomOption.getValue(2);
			SFile << m_sItem.randomOption.getValue(3);

			SFile << m_sItem.dwMainColor;
			SFile << m_sItem.dwSubColor;
		}
		SFile.EndBlock();
	}

	void SItemBase::LoadFile( sc::SerialFile& SFile )
	{
		DWORD nVersion = 0;
		SFile >> nVersion;

		DWORD nSize = 0;
		SFile >> nSize;

		switch( nVersion )
		{
		case Version:
			{
                DWORD dwID;
				SFile >> dwID;        

                SITEMCUSTOM sItemCutsom( dwID );
                SFile >> sItemCutsom.nidDISGUISE.dwID;

                SFile >> sItemCutsom.tBORNTIME;
                SFile >> sItemCutsom.tDISGUISE;

                SFile >> sItemCutsom.wTurnNum;

                SFile >> sItemCutsom.cDAMAGE;
                SFile >> sItemCutsom.cDEFENSE;
                SFile >> sItemCutsom.cRESIST_FIRE;
                SFile >> sItemCutsom.cRESIST_ICE;
                SFile >> sItemCutsom.cRESIST_ELEC;
                SFile >> sItemCutsom.cRESIST_POISON;
                SFile >> sItemCutsom.cRESIST_SPIRIT;

                SFile >> sItemCutsom.randomOption.option[0].cOptType;
                SFile >> sItemCutsom.randomOption.option[1].cOptType;
                SFile >> sItemCutsom.randomOption.option[2].cOptType;
                SFile >> sItemCutsom.randomOption.option[3].cOptType;

                SFile >> sItemCutsom.randomOption.option[0].nOptValue;
                SFile >> sItemCutsom.randomOption.option[1].nOptValue;
                SFile >> sItemCutsom.randomOption.option[2].nOptValue;
                SFile >> sItemCutsom.randomOption.option[3].nOptValue;

                SFile >> sItemCutsom.dwMainColor;
                SFile >> sItemCutsom.dwSubColor;

                m_sItem = sItemCutsom;
			}
			break;

		default:
			{
				SFile.SetOffSet( nSize );
				sc::writeLogError( "WishList::SItemBase::LoadFile - Version" );
			}
			break;
		}
	}

	SItemNpcStore::SItemNpcStore()
	{
	}

	SItemNpcStore::SItemNpcStore( const SITEMCUSTOM& sItem, SNATIVEID& sCrowID, const SNATIVEID& sMapID, int nMapX, int nMapY, LONGLONG nPrice )
		: SItemBase( sItem )
		, m_sCrowID( sCrowID )
		, m_sMapID( sMapID )
		, m_nMapX( nMapX )
		, m_nMapY( nMapY )
		, m_nPrice( nPrice )
	{
	}

	void SItemNpcStore::SaveFile( sc::SerialFile& SFile )
	{
		SItemBase::SaveFile( SFile );

		SFile << DWORD( Version );
		SFile.BeginBlock();
		{
			SFile << m_sCrowID.dwID;
			SFile << m_sMapID.dwID;
			SFile << m_nMapX;
			SFile << m_nMapY;
			SFile << m_nPrice;
		}
		SFile.EndBlock();
	}

	void SItemNpcStore::LoadFile( sc::SerialFile& SFile )
	{
		SItemBase::LoadFile( SFile ); 

		DWORD nVersion = 0;
		SFile >> nVersion;

		DWORD nSize = 0;
		SFile >> nSize;

		switch( nVersion )
		{
		case Version:
			{
				SFile >> m_sCrowID.dwID;
				SFile >> m_sMapID.dwID;
				SFile >> m_nMapX;
				SFile >> m_nMapY;
				SFile >> m_nPrice;
			}
			break;

		default:
			{
				SFile.SetOffSet( nSize );
				sc::writeLogError( "WishList::SItemNpcStore::LoadFile - Version" );
			}
			break;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SItemNpcExStore::SItemNpcExStore()
	{
	}

	SItemNpcExStore::SItemNpcExStore( const SITEMCUSTOM& sItem, SNATIVEID& sCrowID, const SNATIVEID& sMapID, int nMapX, int nMapY, LONGLONG nPrice, int iPosX, int iPosY, int iPage  )
		: SItemBase( sItem ), m_sCrowID(sCrowID), m_sMapID(sMapID), m_nMapX(nMapX), m_nMapY(nMapY), m_nPrice(nPrice), m_iX(iPosX), m_iY(iPosY), m_iPage(iPage)
	{
		
	}

	void SItemNpcExStore::SaveFile( sc::SerialFile& SFile )
	{
		SItemBase::SaveFile( SFile );

		SFile << DWORD( Version );
		SFile.BeginBlock();
		{
			SFile << m_sCrowID.dwID;
			SFile << m_sMapID.dwID;
			SFile << m_nMapX;
			SFile << m_nMapY;
			SFile << m_nPrice;

			SFile << m_iX;
			SFile << m_iY;
			SFile << m_iPage;
		}
		SFile.EndBlock();
	}

	void SItemNpcExStore::LoadFile( sc::SerialFile& SFile )
	{
		SItemBase::LoadFile( SFile );

		DWORD nVersion = 0;
		SFile >> nVersion;

		DWORD nSize = 0;
		SFile >> nSize;

		switch( nVersion )
		{
		case Version:
			{
				SFile >> m_sCrowID.dwID;
				SFile >> m_sMapID.dwID;
				SFile >> m_nMapX;
				SFile >> m_nMapY;
				SFile >> m_nPrice;

				SFile >> m_iX;
				SFile >> m_iY;
				SFile >> m_iPage;
			}
			break;

		default:
			{
				SFile.SetOffSet( nSize );
				sc::writeLogError( "WishList::SItemNpcExStore::LoadFile - Version" );
			}
			break;
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	SItemPointStore::SItemPointStore()
	{
	}

	SItemPointStore::SItemPointStore( const SITEMCUSTOM& sItem, LONGLONG nPrice )
		: SItemBase( sItem )
		, m_nPrice( nPrice )
	{
	}

	void SItemPointStore::SaveFile( sc::SerialFile& SFile )
	{
		SItemBase::SaveFile( SFile );

		SFile << DWORD( Version );
		SFile.BeginBlock();
		{
			SFile << m_nPrice;
		}
		SFile.EndBlock();
	}

	void SItemPointStore::LoadFile( sc::SerialFile& SFile )
	{
		SItemBase::LoadFile( SFile );

		DWORD nVersion = 0;
		SFile >> nVersion;

		DWORD nSize = 0;
		SFile >> nSize;

		switch( nVersion )
		{
		case Version:
			{
				SFile >> m_nPrice;
			}
			break;

		default:
			{
				SFile.SetOffSet( nSize );
				sc::writeLogError( "WishList::SItemPointStore::LoadFile - Version" );
			}
			break;
		}
	}

	SItemPrivatePerson::SItemPrivatePerson()
	{
	}

	SItemPrivatePerson::SItemPrivatePerson( const SITEMCUSTOM& sItem, LONGLONG nPrice, std::string& strCharName )
		: SItemBase( sItem )
		, m_nPrice( nPrice )
		, m_strCharName( strCharName )
	{
	}

	void SItemPrivatePerson::SaveFile( sc::SerialFile& SFile )
	{
		SItemBase::SaveFile( SFile );

		SFile << DWORD( Version );
		SFile.BeginBlock();
		{
			SFile << m_nPrice;
			SFile << m_strCharName;
		}
		SFile.EndBlock();
	}

	void SItemPrivatePerson::LoadFile( sc::SerialFile& SFile )
	{
		SItemBase::LoadFile( SFile );

		DWORD nVersion = 0;
		SFile >> nVersion;

		DWORD nSize = 0;
		SFile >> nSize;

		switch( nVersion )
		{
		case Version:
			{
				SFile >> m_nPrice;
				SFile >> m_strCharName;
			}
			break;

		default:
			{
				SFile.SetOffSet( nSize );
				sc::writeLogError( "WishList::SItemPrivatePerson::LoadFile - Version" );
			}
			break;
		}
	}

	SItemChatLink::SItemChatLink()
	{
	}

	SItemChatLink::SItemChatLink( const SITEMCUSTOM& sItem, std::string& strCharName )
		: SItemBase( sItem )
		, m_strCharName( strCharName )
	{
	}

	void SItemChatLink::SaveFile( sc::SerialFile& SFile )
	{
		SItemBase::SaveFile( SFile );

		SFile << DWORD( Version );
		SFile.BeginBlock();
		{
			SFile << m_strCharName;
		}
		SFile.EndBlock();
	}

	void SItemChatLink::LoadFile( sc::SerialFile& SFile )
	{
		SItemBase::LoadFile( SFile );

		DWORD nVersion = 0;
		SFile >> nVersion;

		DWORD nSize = 0;
		SFile >> nSize;

		switch( nVersion )
		{
		case Version:
			{
				SFile >> m_strCharName;
			}
			break;

		default:
			{
				SFile.SetOffSet( nSize );
				sc::writeLogError( "WishList::SItemChatLink::LoadFile - Version" );
			}
			break;
		}
	}

	SItemActivate::SItemActivate()
	{
	}

	SItemActivate::SItemActivate( const SITEMCUSTOM& sItem, LONGLONG nPrice )
		: SItemBase( sItem )
		, m_nPrice( nPrice )
	{
	}

	void SItemActivate::SaveFile( sc::SerialFile& SFile )
	{
		SItemBase::SaveFile( SFile );

		SFile << DWORD( Version );
		SFile.BeginBlock();
		{
			SFile << m_nPrice;
		}
		SFile.EndBlock();
	}

	void SItemActivate::LoadFile( sc::SerialFile& SFile )
	{
		SItemBase::LoadFile( SFile );

		DWORD nVersion = 0;
		SFile >> nVersion;

		DWORD nSize = 0;
		SFile >> nSize;

		switch( nVersion )
		{
		case Version:
			{
				SFile >> m_nPrice;
			}
			break;

		default:
			{
				SFile.SetOffSet( nSize );
				sc::writeLogError( "WishList::SItemActivate::LoadFile - Version" );
			}
			break;
		}
	}

	SItemTrade::SItemTrade()
	{
	}

	SItemTrade::SItemTrade( const SITEMCUSTOM& sItem, std::string& strCharName )
		: SItemBase( sItem )
		, m_strCharName( strCharName )
	{
	}

	void SItemTrade::SaveFile( sc::SerialFile& SFile )
	{
		SItemBase::SaveFile( SFile );

		SFile << DWORD( Version );
		SFile.BeginBlock();
		{
			SFile << m_strCharName;
		}
		SFile.EndBlock();
	}

	void SItemTrade::LoadFile( sc::SerialFile& SFile )
	{
		SItemBase::LoadFile( SFile );

		DWORD nVersion = 0;
		SFile >> nVersion;

		DWORD nSize = 0;
		SFile >> nSize;

		switch( nVersion )
		{
		case Version:
			{
				SFile >> m_strCharName;
			}
			break;

		default:
			{
				SFile.SetOffSet( nSize );
				sc::writeLogError( "WishList::SItemTrade::LoadFile - Version" );
			}
			break;
		}
	}

	SItemReferChar::SItemReferChar()
	{
	}

	SItemReferChar::SItemReferChar( const SITEMCUSTOM& sItem, std::string& strCharName )
		: SItemBase( sItem )
		, m_strCharName( strCharName )
	{
	}

	void SItemReferChar::SaveFile( sc::SerialFile& SFile )
	{
		SItemBase::SaveFile( SFile );

		SFile << DWORD( Version );
		SFile.BeginBlock();
		{
			SFile << m_strCharName;
		}
		SFile.EndBlock();
	}

	void SItemReferChar::LoadFile( sc::SerialFile& SFile )
	{
		SItemBase::LoadFile( SFile );

		DWORD nVersion = 0;
		SFile >> nVersion;

		DWORD nSize = 0;
		SFile >> nSize;

		switch( nVersion )
		{
		case Version:
			{
				SFile >> m_strCharName;
			}
			break;

		default:
			{
				SFile.SetOffSet( nSize );
				sc::writeLogError( "WishList::SItemReferChar::LoadFile - Version" );
			}
			break;
		}
	}

	SItemInventory::SItemInventory()
	{
	}

	SItemInventory::SItemInventory( const SITEMCUSTOM& sItem )
		: SItemBase( sItem )
	{
	}

	void SItemInventory::SaveFile( sc::SerialFile& SFile )
	{
		SItemBase::SaveFile( SFile );

		SFile << DWORD( Version );
		SFile.BeginBlock();
		{
		}
		SFile.EndBlock();
	}

	void SItemInventory::LoadFile( sc::SerialFile& SFile )
	{
		SItemBase::LoadFile( SFile );

		DWORD nVersion = 0;
		SFile >> nVersion;

		DWORD nSize = 0;
		SFile >> nSize;

		switch( nVersion )
		{
		case Version:
			{
			}
			break;

		default:
			{
				SFile.SetOffSet( nSize );
				sc::writeLogError( "WishList::SItemInventory::LoadFile - Version" );
			}
			break;
		}
	}

	SItemBank::SItemBank()
	{
	}

	SItemBank::SItemBank( const SITEMCUSTOM& sItem )
		: SItemBase( sItem )
	{
	}

	void SItemBank::SaveFile( sc::SerialFile& SFile )
	{
		SItemBase::SaveFile( SFile );

		SFile << DWORD( Version );
		SFile.BeginBlock();
		{
		}
		SFile.EndBlock();
	}

	void SItemBank::LoadFile( sc::SerialFile& SFile )
	{
		SItemBase::LoadFile( SFile );

		DWORD nVersion = 0;
		SFile >> nVersion;

		DWORD nSize = 0;
		SFile >> nSize;

		switch( nVersion )
		{
		case Version:
			{
			}
			break;

		default:
			{
				SFile.SetOffSet( nSize );
				sc::writeLogError( "WishList::SItemBank::LoadFile - Version" );
			}
			break;
		}
	}

	SItemLocker::SItemLocker()
	{
	}

	SItemLocker::SItemLocker( const SITEMCUSTOM& sItem )
		: SItemBase( sItem )
	{
	}

	void SItemLocker::SaveFile( sc::SerialFile& SFile )
	{
		SItemBase::SaveFile( SFile );

		SFile << DWORD( Version );
		SFile.BeginBlock();
		{
		}
		SFile.EndBlock();
	}

	void SItemLocker::LoadFile( sc::SerialFile& SFile )
	{
		SItemBase::LoadFile( SFile );

		DWORD nVersion = 0;
		SFile >> nVersion;

		DWORD nSize = 0;
		SFile >> nSize;

		switch( nVersion )
		{
		case Version:
			{
			}
			break;

		default:
			{
				SFile.SetOffSet( nSize );
				sc::writeLogError( "WishList::SItemLocker::LoadFile - Version" );
			}
			break;
		}
	}

	CClient::CClient( GLGaeaClient* pGaeaClient )
		: m_pGaeaClient( pGaeaClient )
	{
	}

	CClient::~CClient()
	{
		Clear();
	}

	ItemSPtr CClient::NewItem( BYTE nType )
	{
		switch( nType )
		{
		case ETypeNpcStoreEx:
			return ItemSPtr( new SItemNpcExStore );
		case ETypeNpcStore:
			return ItemSPtr( new SItemNpcStore );

		case ETypePointStore:
			return ItemSPtr( new SItemPointStore );

		case ETypePrivatePerson:
			return ItemSPtr( new SItemPrivatePerson );

		case ETypeChatLink:
			return ItemSPtr( new SItemChatLink );

		case ETypeActivate:
			return ItemSPtr( new SItemActivate );

		case ETypeTrade:
			return ItemSPtr( new SItemTrade );

		case ETypeReferChar:
			return ItemSPtr( new SItemReferChar );

		case ETypeInventory:
			return ItemSPtr( new SItemInventory );

		case ETypeBank:
			return ItemSPtr( new SItemBank );

		case ETypeLocker:
			return ItemSPtr( new SItemLocker );
		}

		return ItemSPtr();
	}

	DWORD CClient::GetEmptySlot()
	{
		if( m_mapItem.size() >= ETotalSlot )
			return UINT_MAX;

		for( WORD y=0; y<ETotalLine; ++y )
		{
			for( WORD x=0; x<ESlotX; ++x )
			{
				DWORD nSlot = MAKELONG( x, y );

				ItemMapIter it = m_mapItem.find( nSlot );
				if( it == m_mapItem.end() )
					return nSlot;
			}
		}

		return UINT_MAX;
	}

	bool CClient::InsertItem( DWORD nSlot, ItemSPtr pItem )
	{
		if( !GLUseFeatures::GetInstance().IsUsingRenewInvenWindow() )
			return false;

		if( !pItem )
			return false;

		ItemMapIter it = m_mapItem.find( nSlot );
		if( it != m_mapItem.end() )
			return false;

		if( nSlot >= UINT_MAX )
		{
			m_pGaeaClient->PrintMsgText(
				NS_UITEXTCOLOR::RED,
				ID2GAMEINTEXT( "WISH_LIST_FULL" ) );
			return false;
		}

		m_mapItem.insert( ItemMapValue( nSlot, pItem ) );

		std::string strText = sc::string::format(
			ID2GAMEINTEXT( "WISH_LIST_INSERT" ),
			pItem->m_sItem.GETNAME() );
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::ENABLE, strText.c_str() );

		WORD wPosX = LOWORD( nSlot );
		WORD wPosY = HIWORD( nSlot );

		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
			NSWIDGET_SCRIPT::g_strFunc_Inventory_WishSlotUpdate,
			"-n, -n", wPosX, wPosY );
		return true;
	}

	void CClient::Clear()
	{
		m_mapItem.clear();
	}

	void CClient::RemoveItem( DWORD nSlot )
	{
		ItemMapIter it = m_mapItem.find( nSlot );
		if( it != m_mapItem.end() )
			m_mapItem.erase( it );

		WORD wPosX = LOWORD( nSlot );
		WORD wPosY = HIWORD( nSlot );

		GLWidgetScript::GetInstance().LuaCallFunc< void >( 
			NSWIDGET_SCRIPT::g_strFunc_Inventory_WishSlotUpdate,
			"-n, -n", wPosX, wPosY );
	}

	ItemSPtr CClient::GetItem( DWORD nSlot )
	{
		ItemMapIter it = m_mapItem.find( nSlot );
		if( it != m_mapItem.end() )
			return it->second;

		return ItemSPtr();
	}

	bool CClient::NewNpcStoreItem( const SITEMCUSTOM& sItem, SNATIVEID& sCrowID, const SNATIVEID& sMapID, int nMapX, int nMapY, LONGLONG nPrice, DWORD nSlot )
	{
        SITEMCUSTOM sWishItemCustom = sItem;
        sWishItemCustom.BoxUnwrapping();
		SItemNpcStore* pWishItem = new SItemNpcStore( sWishItemCustom, sCrowID, sMapID, nMapX, nMapY, nPrice );
		return InsertItem( ( nSlot >= UINT_MAX ) ? GetEmptySlot() : nSlot, ItemSPtr( pWishItem ) );
	}

	bool CClient::NewNpcExStoreItem( const SITEMCUSTOM& sItem, SNATIVEID& sCrowID, const SNATIVEID& sMapID, int nMapX, int nMapY, LONGLONG nPrice, int iPosX, int iPosY, int iPage, DWORD nSlot )
	{
		SITEMCUSTOM sWishItemCustom = sItem;
		sWishItemCustom.BoxUnwrapping();
		SItemNpcExStore* pWishItem = new SItemNpcExStore( sWishItemCustom, sCrowID, sMapID, nMapX, nMapY, nPrice, iPosX, iPosY, iPage );
		return InsertItem( ( nSlot >= UINT_MAX ) ? GetEmptySlot() : nSlot, ItemSPtr( pWishItem ) );
	}

	bool CClient::NewPointStoreItem( const SITEMCUSTOM& sItem, LONGLONG nPrice, DWORD nSlot )
	{
        SITEMCUSTOM sWishItemCustom = sItem;
        sWishItemCustom.BoxUnwrapping();
		SItemPointStore* pWishItem = new SItemPointStore( sWishItemCustom, nPrice );
		return InsertItem( ( nSlot >= UINT_MAX ) ? GetEmptySlot() : nSlot, ItemSPtr( pWishItem ) );
	}

	bool CClient::NewPrivatePersonItem( const SITEMCUSTOM& sItem, LONGLONG nPrice, std::string& strCharName, DWORD nSlot )
	{
        SITEMCUSTOM sWishItemCustom = sItem;
        sWishItemCustom.BoxUnwrapping();
		SItemPrivatePerson* pWishItem = new SItemPrivatePerson( sWishItemCustom, nPrice, strCharName );
		return InsertItem( ( nSlot >= UINT_MAX ) ? GetEmptySlot() : nSlot, ItemSPtr( pWishItem ) );
	}

	bool CClient::NewChatLinkItem( const SITEMCUSTOM& sItem, std::string& strCharName, DWORD nSlot )
	{
        SITEMCUSTOM sWishItemCustom = sItem;
        sWishItemCustom.BoxUnwrapping();
		SItemChatLink* pWishItem = new SItemChatLink( sWishItemCustom, strCharName );
		return InsertItem( ( nSlot >= UINT_MAX ) ? GetEmptySlot() : nSlot, ItemSPtr( pWishItem ) );
	}

	bool CClient::NewActivateItem( const SITEMCUSTOM& sItem, LONGLONG nPrice, DWORD nSlot )
	{
        SITEMCUSTOM sWishItemCustom = sItem;
        sWishItemCustom.BoxUnwrapping();
		SItemActivate* pWishItem = new SItemActivate( sWishItemCustom, nPrice );
		return InsertItem( ( nSlot >= UINT_MAX ) ? GetEmptySlot() : nSlot, ItemSPtr( pWishItem ) );
	}

	bool CClient::NewTradeItem( const SITEMCUSTOM& sItem, std::string& strCharName, DWORD nSlot )
	{
        SITEMCUSTOM sWishItemCustom = sItem;
        sWishItemCustom.BoxUnwrapping();
		SItemTrade* pWishItem = new SItemTrade( sWishItemCustom, strCharName );
		return InsertItem( ( nSlot >= UINT_MAX ) ? GetEmptySlot() : nSlot, ItemSPtr( pWishItem ) );
	}

	bool CClient::NewReferCharItem( const SITEMCUSTOM& sItem, std::string& strCharName, DWORD nSlot )
	{
        SITEMCUSTOM sWishItemCustom = sItem;
        sWishItemCustom.BoxUnwrapping();
		SItemReferChar* pWishItem = new SItemReferChar( sWishItemCustom, strCharName );
		return InsertItem( ( nSlot >= UINT_MAX ) ? GetEmptySlot() : nSlot, ItemSPtr( pWishItem ) );
	}

	bool CClient::NewInventoryItem( const SITEMCUSTOM& sItem, DWORD nSlot )
	{
        SITEMCUSTOM sWishItemCustom = sItem;
        sWishItemCustom.BoxUnwrapping();
		SItemInventory* pWishItem = new SItemInventory( sWishItemCustom );
		return InsertItem( ( nSlot >= UINT_MAX ) ? GetEmptySlot() : nSlot, ItemSPtr( pWishItem ) );
	}

	bool CClient::NewBankItem( const SITEMCUSTOM& sItem, DWORD nSlot )
	{
        SITEMCUSTOM sWishItemCustom = sItem;
        sWishItemCustom.BoxUnwrapping();
		SItemBank* pWishItem = new SItemBank( sWishItemCustom );
		return InsertItem( ( nSlot >= UINT_MAX ) ? GetEmptySlot() : nSlot, ItemSPtr( pWishItem ) );
	}

	bool CClient::NewLockerItem( const SITEMCUSTOM& sItem, DWORD nSlot )
	{
        SITEMCUSTOM sWishItemCustom = sItem;
        sWishItemCustom.BoxUnwrapping();
		SItemLocker* pWishItem = new SItemLocker( sWishItemCustom );
		return InsertItem( ( nSlot >= UINT_MAX ) ? GetEmptySlot() : nSlot, ItemSPtr( pWishItem ) );
	}

	CString KeyToHash( WORD nServer, DWORD nCharDbNum )
	{
		boost::hash< std::string > hashString;

		CString strHashKey;
		strHashKey.Format( "%d_%d", nServer, nCharDbNum );
		strHashKey.Format( "%d", hashString( strHashKey.GetString() ) );

		return strHashKey;
	}

	void CClient::SaveFile( WORD nServer, DWORD nCharDbNum )
	{
		if( !GLUseFeatures::GetInstance().IsUsingRenewInvenWindow() )
			return;

		CString strPath = sc::getAppPath().c_str();
		strPath.Append( m_pGaeaClient->GetSubPath()->Cache().c_str() );

		if( !sc::file::isExist( strPath.GetString() ) )
			sc::file::createDirectory( strPath.GetString() );

		CString strHashKey = KeyToHash( nServer, nCharDbNum );
		strPath.AppendFormat( "Wish%s.dat", strHashKey.GetString() );

		sc::SerialFile SFile;
		if( !SFile.OpenFile( FOT_WRITE, strPath.GetString() ) )
			return;

		SaveFile( SFile );

		SFile.CloseFile();
	}

	void CClient::LoadFile( WORD nServer, DWORD nCharDbNum )
	{
		if( !GLUseFeatures::GetInstance().IsUsingRenewInvenWindow() )
			return;

		CString strPath = sc::getAppPath().c_str();
		strPath.Append( m_pGaeaClient->GetSubPath()->Cache().c_str() );

		CString strHashKey = KeyToHash( nServer, nCharDbNum );
		strPath.AppendFormat( "Wish%s.dat", strHashKey.GetString() );

		sc::SerialFile SFile;
		if( !SFile.OpenFile( FOT_READ, strPath.GetString() ) )
			return;

		LoadFile( SFile, strPath.GetString() );

		SFile.CloseFile();
	}

	void CClient::SaveFile( sc::SerialFile& SFile )
	{
		SFile << DWORD( Version );
		SFile.BeginBlock();
		{
			SFile << DWORD( m_mapItem.size() );

			BOOST_FOREACH( ItemMapValue& it, m_mapItem )
			{
				DWORD nSlot = it.first;
				SFile << nSlot;

				ItemSPtr pItem = it.second;
				SFile << pItem->GetType();

				pItem->SaveFile( SFile );
			}
		}
		SFile.EndBlock();
	}

	void CClient::LoadFile( sc::SerialFile& SFile, const TCHAR* pFileName )
	{
		DWORD nVersion = 0;
		SFile >> nVersion;

		DWORD nSize = 0;
		SFile >> nSize;

		// 만약 다르다면 Save 를 새로 해줘서 정상파일을 만들도록 한다.
		// 이렇게 안하면 유저가 직접 이 파일을 지우지 않는 이상 맵에 접속이 불가능할 수도 있다.
		if ( !SFile.IsSameRemindLoadSize( nSize ) || (nSize==0) ) 
		{
			SFile.CloseFile();

			// [shhan][2015.01.14] 이전 Save에서 문제가 발생했을 수도 있으므로(거의 낮은확률,하지만 BugTrap은 왔음) 재 Save 를 해서 다음 접속시 문제가 없도록 한다.
			{
				sc::SerialFile SFileNEW;
				if( !SFileNEW.OpenFile( FOT_WRITE, pFileName) )
					return;

				SaveFile( SFileNEW );

				SFileNEW.CloseFile();

				sc::writeLogError( "WishList::CClient::LoadFile - Version" );
			}

			return;
		}

		switch( nVersion )
		{
		case Version:
			{
				DWORD nCount = 0;
				SFile >> nCount;

				for( DWORD i=0; i<nCount; ++i )
				{
					DWORD nSlot = 0;
					SFile >> nSlot;

					BYTE nType = 0;
					SFile >> nType;

					ItemSPtr pItem = NewItem( nType );
					if( pItem )
					{
						pItem->LoadFile( SFile );

						if( m_mapItem.find( nSlot ) == m_mapItem.end() )
							m_mapItem.insert( ItemMapValue( nSlot, pItem ) );
					}
					else
					{
						DWORD nVersionItem = 0;
						SFile >> nVersionItem;

						DWORD nSizeItem = 0;
						SFile >> nSizeItem;
						SFile.SetOffSet( nSizeItem );
						sc::writeLogError( "WishList::CClient::LoadFile - Type" );
					}
				}
			}
			break;

		default:
			{
				SFile.SetOffSet( nSize );
				sc::writeLogError( "WishList::CClient::LoadFile - Version" );
			}
			break;
		}
	}
}
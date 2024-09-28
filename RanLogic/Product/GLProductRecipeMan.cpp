#include "pch.h"

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/File/StringFile.h"
#include "../../SigmaCore/File/SerialFile.h"
#include "../../SigmaCore/File/TxtFile.h"

#include "../../EngineLib/Common/IniLoader.h"
#include "../../EngineLib/G-Logic/GLogic.h"

#include "./GLProductRecipeMan.h"
#include "../GLogicData.h"
#include "../GLogicDataMan.h"
#include "../Item/GLItemMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace
{
	const char* _FILEHEAD = "GLITEM_MIX";
	const int nKeysize = ( sizeof( DWORD ) + sizeof( BYTE ) ) * Product::ITEM_PRODUCT_MAX_MATERIAL * 2;
}

GLProductRecipeMan& GLProductRecipeMan::GetInstance()
{
	static GLProductRecipeMan Instance;
	return Instance;
}

GLProductRecipeMan::GLProductRecipeMan() 
: m_bServer( false )
{
}

GLProductRecipeMan::~GLProductRecipeMan()
{
	CleanUp();
}

void GLProductRecipeMan::CleanUp()
{
	m_bServer = false;

	m_mapKeyProductRecipe.clear();
	m_mapStrProductRecipe.clear();
}

void GLProductRecipeMan::InsertRecipe( SProductRecipe& sProductRecipe )
{
	SortMeterialItem( sProductRecipe );

	m_mapKeyProductRecipe.insert( std::make_pair( sProductRecipe.dwKey, sProductRecipe ) );

	if( !m_bServer )
	{
		TCHAR szKey[ nKeysize + 1 ] = { 0 };

		for( int i=0; i<Product::ITEM_PRODUCT_MAX_MATERIAL; ++i )
		{
			_stprintf_s(
				szKey,
				nKeysize + 1,
				"%s%0.8x%0.2x",
				szKey,
				sProductRecipe.sMeterialItem[ i ].sNID.dwID,
				sProductRecipe.sMeterialItem[ i ].nNum );
		}

		m_mapStrProductRecipe.insert( std::make_pair( std::string( szKey ), sProductRecipe ) );
	}
}

class ITEMMIX_DATA_CMP
{
public:
	bool operator() ( const SProductItem& lvalue, const SProductItem& rvalue )
	{
		if( lvalue.sNID.dwID < rvalue.sNID.dwID )
		{
			return true;
		}
		else if( lvalue.sNID.dwID == rvalue.sNID.dwID )
		{
			if( lvalue.nNum < rvalue.nNum )
				return true;
		}
		return false;
	}
};

void GLProductRecipeMan::SortMeterialItem( SProductRecipe& sItemMix )
{
	std::vector< SProductItem >	vecTemp;
	for( int i=0; i<Product::ITEM_PRODUCT_MAX_MATERIAL; ++i ) 
		vecTemp.push_back( sItemMix.sMeterialItem[ i ] );
	std::sort( vecTemp.begin(), vecTemp.end(), ITEMMIX_DATA_CMP() );

	std::vector< SProductItem >::iterator pos = vecTemp.begin();
	for( int i=0; i<Product::ITEM_PRODUCT_MAX_MATERIAL; ++i ) 
	{
		sItemMix.sMeterialItem[ i ] = (SProductItem)( *pos );
		++pos;
	}
}

class ITEMMIX_INVEN_CMP
{
public:
	bool operator() ( const SINVENITEM& lvalue, const SINVENITEM& rvalue )
	{
		if( lvalue.sItemCustom.GetNativeID().dwID < rvalue.sItemCustom.GetNativeID().dwID )
		{
			return true;
		}
		else if( lvalue.sItemCustom.GetNativeID().dwID == rvalue.sItemCustom.GetNativeID().dwID )
		{
			if( lvalue.sItemCustom.wTurnNum < rvalue.sItemCustom.wTurnNum )
				return true;
		}
		return false;
	}
};

void GLProductRecipeMan::SortInvenItem( SINVENITEM* pInvenItem )
{
	std::vector< SINVENITEM > vecTemp;
	for( int i=0; i<Product::ITEM_PRODUCT_MAX_MATERIAL; ++i ) 
		vecTemp.push_back( pInvenItem[ i ] );
	std::sort( vecTemp.begin(), vecTemp.end(), ITEMMIX_INVEN_CMP() );

	std::vector< SINVENITEM >::iterator pos = vecTemp.begin();
	for( int i=0; i<Product::ITEM_PRODUCT_MAX_MATERIAL; ++i ) 
	{
		pInvenItem[ i ] = (SINVENITEM)( *pos );
		++pos;
	}
}

SProductRecipe* GLProductRecipeMan::GetProductRecipe( DWORD dwKey )
{
	PRODUCT_RECIPE_KEYMAP_ITER pos = m_mapKeyProductRecipe.find( dwKey );
	if( pos != m_mapKeyProductRecipe.end() )
		return &pos->second;

	return NULL;
}

SProductRecipe* GLProductRecipeMan::GetProductRecipeClient( SProductRecipe& sItemMix )
{
	TCHAR szKey[ nKeysize + 1 ] = { 0 };

	for( int i=0; i<Product::ITEM_PRODUCT_MAX_MATERIAL; ++i )
	{
		_stprintf_s(
			szKey,
			nKeysize + 1,
			"%s%0.8x%0.2x",
			szKey,
			sItemMix.sMeterialItem[ i ].sNID.dwID,
			sItemMix.sMeterialItem[ i ].nNum );
	}

	ProductRecipe_STRMAP_ITER pos = m_mapStrProductRecipe.find( std::string( szKey ) );
	if( pos != m_mapStrProductRecipe.end() )
		return &pos->second;

	return NULL;
}

SProductRecipe* GLProductRecipeMan::GetProductRecipeEmulator( SProductRecipe& sItemMix )
{
	PRODUCT_RECIPE_KEYMAP_ITER pos = m_mapKeyProductRecipe.begin();
	PRODUCT_RECIPE_KEYMAP_ITER end = m_mapKeyProductRecipe.end();

	for( ; pos != end; ++pos )
	{
		SProductRecipe& sItemMixFind = pos->second;
		bool bComp = true;

		for( int i=0; i<Product::ITEM_PRODUCT_MAX_MATERIAL; ++i )
		{
			if( sItemMix.sMeterialItem[ i ].sNID != sItemMixFind.sMeterialItem[ i ].sNID ||
				sItemMix.sMeterialItem[ i ].nNum != sItemMixFind.sMeterialItem[ i ].nNum )
			{
				bComp = false;
				break;
			}
		}

		if( bComp )
			return &pos->second;
	}

	return NULL;
}

bool GLProductRecipeMan::IsValidClass( const SITEM* pItem, BYTE eFindClass )
{
	switch( eFindClass )
	{
	case GLCL_FIGHTER:
		if( pItem->sBasicOp.dwReqCharClass & GLCC_FIGHTER )
			return true;
		return false;

	case GLCL_ARMS:
		if( pItem->sBasicOp.dwReqCharClass & GLCC_ARMS )
			return true;
		return false;

	case GLCL_ARCHER:
		if( pItem->sBasicOp.dwReqCharClass & GLCC_ARCHER )
			return true;
		return false;

	case GLCL_SPIRIT:
		if( pItem->sBasicOp.dwReqCharClass & GLCC_SPIRIT )
			return true;
		return false;

	case GLCL_EXTREME:
		if( pItem->sBasicOp.dwReqCharClass & GLCC_EXTREME )
			return true;
		return false;

	case GLCL_SCIENTIST:
		if( pItem->sBasicOp.dwReqCharClass & GLCC_SCIENTIST )
			return true;
		return false;

	case GLCL_ASSASSIN:
		if( pItem->sBasicOp.dwReqCharClass & GLCC_ASSASSIN )
			return true;
		return false;

	case GLCL_TRICKER:
		if( pItem->sBasicOp.dwReqCharClass & GLCC_TRICKER )
			return true;
		return false;
	case GLCL_ACTOR:
		if( pItem->sBasicOp.dwReqCharClass & GLCC_ACTOR )
			return true;
		return false;
	}

	return true;
}

void GLProductRecipeMan::Find( PtrVector_ProductRecipe& vecData, BYTE eFindType, BYTE eFindClass )
{
    PRODUCT_RECIPE_KEYMAP_ITER pos = m_mapKeyProductRecipe.begin();
	PRODUCT_RECIPE_KEYMAP_ITER end = m_mapKeyProductRecipe.end();

	for( ; pos != end; ++pos )
	{
		SProductRecipe* pProductRecipe = &pos->second;
		if( !pProductRecipe )
			continue;

		const SITEM* pItem = GLItemMan::GetInstance().GetItem( pProductRecipe->sResultItem.sNID );

		if( !pItem )
			continue;

        if (pProductRecipe->eProductType != eFindType)
        {
            continue;
        }

		if( !IsValidClass( pItem, eFindClass ) )            
			continue;

		if( IsRandomBoxValidClass( pItem, eFindClass ) == FALSE )
			continue;

		vecData.push_back( pProductRecipe );
	}
}

HRESULT GLProductRecipeMan::LoadFile_OLD( const std::string& FileName, BOOL bServer, bool bToolMode )
{
    std::string strFilePathName = m_strPATH + FileName;
    return LoadFile_OLD( strFilePathName.c_str(), FileName, bServer, bToolMode );
}

HRESULT GLProductRecipeMan::LoadFile_OLD( const char* szFile, const std::string& FileName, BOOL bServer, bool bToolMode )
{
	CleanUp();

	m_bServer = bServer ? TRUE : FALSE;

	CIniLoader cFILE( ",", bToolMode );

	if (GLOGIC::UseLogicZipFile())
		cFILE.SetZipFile(GLogicData::GetInstance().GetGlogicZipFile());

	std::string strSep( ",[]\t" );
	cFILE.reg_sep( strSep );

	if( !cFILE.open( szFile, true, GLOGIC::UseLogicPackFile() ) )
	{
		sc::writeLogError(
			sc::string::format(
			"GLProductRecipeMan::LoadFile(), File Open %1%", FileName ) );
		return E_FAIL;
	}

	INT nItemMixKeySize( 0 );
	cFILE.getflag( "ITEM_MIX_INFO", "ItemMixKeySize", 0, 1, nItemMixKeySize );

	DWORD dwNUM = cFILE.GetKeySize( "ITEM_MIX_LIST", "ITEM_MIX" );
	for( DWORD i=0; i<dwNUM; ++i )
	{
		SProductRecipe sItemMix;

		cFILE.getflag( i, "ITEM_MIX_LIST", "ITEM_MIX", 0, nItemMixKeySize, sItemMix.dwKey );

		for( int j=0; j<Product::ITEM_PRODUCT_MAX_MATERIAL; ++j )
		{
			cFILE.getflag( i, "ITEM_MIX_LIST", "ITEM_MIX", (j*3)+1, nItemMixKeySize, sItemMix.sMeterialItem[j].sNID.wMainID );
			cFILE.getflag( i, "ITEM_MIX_LIST", "ITEM_MIX", (j*3)+2, nItemMixKeySize, sItemMix.sMeterialItem[j].sNID.wSubID );		
			cFILE.getflag( i, "ITEM_MIX_LIST", "ITEM_MIX", (j*3)+3, nItemMixKeySize, sItemMix.sMeterialItem[j].nNum );
		}

		cFILE.getflag( i, "ITEM_MIX_LIST", "ITEM_MIX", 16, nItemMixKeySize, sItemMix.sResultItem.sNID.wMainID );
		cFILE.getflag( i, "ITEM_MIX_LIST", "ITEM_MIX", 17, nItemMixKeySize, sItemMix.sResultItem.sNID.wSubID );
		cFILE.getflag( i, "ITEM_MIX_LIST", "ITEM_MIX", 18, nItemMixKeySize, sItemMix.sResultItem.nNum );

		cFILE.getflag( i, "ITEM_MIX_LIST", "ITEM_MIX", 19, nItemMixKeySize, sItemMix.dwRate );
		cFILE.getflag( i, "ITEM_MIX_LIST", "ITEM_MIX", 20, nItemMixKeySize, sItemMix.dwPrice );

		InsertRecipe( sItemMix );
	}

	return S_OK;
}

HRESULT GLProductRecipeMan::LoadFile( const std::string& FileName, BOOL bServer )
{
    std::string strFilePathName = m_strPATH + FileName;
    return LoadFile( strFilePathName.c_str(), FileName, bServer );
}

HRESULT GLProductRecipeMan::LoadFile( const char* szFile, const std::string& FileName, BOOL bServer )
{
	CleanUp();

	m_bServer = bServer ? TRUE : FALSE;

	std::auto_ptr< sc::BaseStream > pBStream(
		GLOGIC::openfile_basestream(
		GLOGIC::UseLogicZipFile(), 
		GLogicData::GetInstance().GetGlogicZipFile(),
		szFile, 
		FileName,
		false,
		GLOGIC::UseLogicPackFile() ) );

	if( !pBStream.get() )
		return E_FAIL;

	sc::BaseStream &SFile = *pBStream;

	DWORD dwFILEVER;
	char szFILETYPE[ _MAX_FNAME ];
	SFile.GetFileType( szFILETYPE, _MAX_FNAME, dwFILEVER );
	SFile.SetEncode( true );

	DWORD dwNum = 0;
	SFile >> dwNum;

	for( DWORD i=0; i<dwNum; i++ )
	{
		SProductRecipe sProductRecipe;

		if( !sProductRecipe.LoadFile( SFile ) )
			return E_FAIL;
        
        if ( sProductRecipe.eProductType == Product::PRODUCT_NOT_USE_REQ)   //제작숙련도시스템 이전 버전일때
        {
            const SITEM* pItem = GLItemMan::GetInstance().GetItem( sProductRecipe.sResultItem.sNID );
            WORD tmpType = FindSuitType( pItem );
            if ( tmpType == Product::ETypeTotal )
            {
                return E_FAIL;
            }
            sProductRecipe.eProductType = tmpType;
        }

		InsertRecipe( sProductRecipe );
	}

	return S_OK;
}

HRESULT GLProductRecipeMan::SaveFile( const char* szFile )
{
	sc::SerialFile SFile;
	SFile.SetFileType( _FILEHEAD, VERSION );

	if( !SFile.OpenFile( FOT_WRITE, szFile ) )
		return E_FAIL;

	SFile.SetEncode( true );

	DWORD dwNum = (DWORD)m_mapKeyProductRecipe.size();
	SFile << dwNum;

	PRODUCT_RECIPE_KEYMAP_ITER pos = m_mapKeyProductRecipe.begin();
	PRODUCT_RECIPE_KEYMAP_ITER end = m_mapKeyProductRecipe.end();

	for( ; pos != end; ++pos )
	{
		SProductRecipe* pProductRecipe = &pos->second;
		if( !pProductRecipe ) 
		{
			MessageBox( NULL, "GLProductRecipeMan::SaveFile, Error ITEM_MIX", "", MB_OK );
			return E_FAIL;
		}

		pProductRecipe->SaveFile( SFile );
	}

	return S_OK;
}

namespace SAVECSV
{
	typedef std::pair< DWORD, SProductRecipe* > PAIR_PRODUCT_RECIPE_KEYMAP;
	typedef std::vector< PAIR_PRODUCT_RECIPE_KEYMAP > VEC_SORT;
	typedef VEC_SORT::iterator VEC_SORT_ITER;
	static VEC_SORT SortedVec;

	bool VEC_SORT_FUNCTOR( const PAIR_PRODUCT_RECIPE_KEYMAP& lvalue, const PAIR_PRODUCT_RECIPE_KEYMAP& rvalue )
	{
		return lvalue.first < rvalue.first;
	}
};

bool GLProductRecipeMan::SaveCsv( std::fstream &SFile )
{
	using namespace SAVECSV;

	PRODUCT_RECIPE_KEYMAP_ITER iter = m_mapKeyProductRecipe.begin();
	PRODUCT_RECIPE_KEYMAP_ITER end = m_mapKeyProductRecipe.end();

	SortedVec.clear();
	for( ; iter != end; ++iter )
		SortedVec.push_back( PAIR_PRODUCT_RECIPE_KEYMAP( iter->first, &iter->second ) );
	std::sort( SortedVec.begin(), SortedVec.end(), VEC_SORT_FUNCTOR );

	SProductRecipe::SaveCsvHead( SFile );

	VEC_SORT_ITER veciter = SortedVec.begin();
	VEC_SORT_ITER veciter_end = SortedVec.end();

	for( ; veciter != veciter_end; ++veciter )
	{
		if( veciter->second )
			veciter->second->SaveCsv( SFile );
	}

	return true;
}

bool GLProductRecipeMan::LoadCsv( std::string& strFileName )
{
	sc::CStringFile StrFile( 10240 );
	if( !StrFile.Open( strFileName.c_str() ) )
		return false;

	m_mapKeyProductRecipe.clear();

	STRUTIL::ClearSeparator();
	STRUTIL::RegisterSeparator( "," );

	CString strLine;
	CStringArray StrArray;

	StrFile.GetNextLine( strLine );
	STRUTIL::StringSeparate( strLine, StrArray );
	int iHead = (int)StrArray.GetCount();

	while( StrFile.GetNextLine( strLine ) )
	{
		STRUTIL::StringSeparate( strLine, StrArray );

		int iCount = (int)StrArray.GetCount();
		if( iCount < iHead )
		{
			char szError[ 256 ];
			StringCchPrintf( szError, 256, "GLProductRecipeMan::LoadCsv include blank column, next item load failed" );
			MessageBox( NULL, szError, _T( "Fail" ), MB_OK );
			return false;
		}

		SProductRecipe sItemMix;
		sItemMix.LoadCsv( StrArray );

		InsertRecipe( sItemMix );
	}

	return true;
}

void GLProductRecipeMan::SaveSQLFile ( void )
{
	std::string strPath( sc::getAppPath() );
	strPath.append( "\\ProductRecipeList.sql" );    

	sc::TxtFile sSqlFile( strPath, true, sc::TxtFile::ANSI );

	sSqlFile.write( "DELETE ProductRecipeList" );
	sSqlFile.write( "GO" );

	PRODUCT_RECIPE_KEYMAP_ITER iter = m_mapKeyProductRecipe.begin();
	PRODUCT_RECIPE_KEYMAP_ITER iterEnd = m_mapKeyProductRecipe.end();

	for( ; iter != iterEnd; ++iter )
	{
		SProductRecipe* pProductRecipe = &iter->second;
		if ( NULL == pProductRecipe ) 
		{
			continue;
		}

		const DWORD dwProductID = pProductRecipe->dwKey;
		WORD wWsp = pProductRecipe->wWsp;
		if ( Product::PRODUCT_NOT_USE_REQ == wWsp )
			wWsp = 0;

		sSqlFile.write( sc::string::format(
			"INSERT INTO ProductRecipeList( ProductID, Wsp ) VALUES( %1%, %2% )",
			dwProductID,
			wWsp ) );
	}

	sSqlFile.write( "GO" );
}

WORD GLProductRecipeMan::FindSuitType( const SITEM* pItem )
{
    if( pItem == NULL)
        return false;

	SITEMCUSTOM sItemCustom( pItem );

    // 장착 아이템
    if( pItem->BasicType() == ITEM_SUIT )
    {
        switch( pItem->Suit() )
        {
        case SUIT_HEADGEAR:
            return Product::ETypeHeadGear;
        case SUIT_UPPER:
            return Product::ETypeUpper;
        case SUIT_LOWER:
            return Product::ETypeLower;
        case SUIT_HAND:
            return Product::ETypeHand;
        case SUIT_FOOT:
            return Product::ETypeFoot;
        case SUIT_HANDHELD:
            return Product::ETypeHandheld;
        case SUIT_NECK:
        case SUIT_EARRING:
            return Product::ETypeNeck_Earring;
        case SUIT_WRIST:
        case SUIT_WAISTBAND:
            return Product::ETypeWrist_WaistBand;
        case SUIT_FINGER:
        case SUIT_ACCESSORIES:
            return Product::ETypeFinger_Accessories;
        case SUIT_DECORATION:
            return Product::ETypeRandomBox; // 장식은 랜덤박스 분야로 취급한다.;
        }
    }
    else if( pItem->BasicType() == ITEM_RANDOMITEM )
    {
        return Product::ETypeRandomBox;
    }
    //else if( pItem->sBasicOp.IsDISGUISE() )
	else if( sItemCustom.IsDisguise() != NULL )
    {
        return Product::ETypeCostume;
    }
    else
    {
        return Product::ETypeEtc;
    }
    return Product::ETypeTotal;
}

bool GLProductRecipeMan::IsRandomBoxValidClass( const SITEM* pItem, BYTE eFindClass )
{
	if( ITEM_RANDOMITEM == pItem->BasicType() )
	{
		bool bIsSuit = false;
		for( unsigned int nIndex = 0 ; nIndex < pItem->sRandomBox.vecBOX.size() ; nIndex++ )
		{
			const SITEM* pRandomItem = GLItemMan::GetInstance().GetItem( pItem->sRandomBox.vecBOX[nIndex].nidITEM );
			if( pRandomItem->BasicType() == ITEM_SUIT )
			{
				if( IsValidClass( pRandomItem, eFindClass ) == false )
					return false;
			}

		}
		if( bIsSuit )
			return false;
	}

	return true;
}

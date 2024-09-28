#include "pch.h"

#include "../../../SigmaCore/Lua/MinLua.h"
#include "../../../SigmaCore/Util/SystemInfo.h"

#include "../../../EngineLib/Common/SUBPATH.h"
#include "../../../EngineLib/G-Logic/GLogic.h"

#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/GLogicData.h"

#include "./GLItemBasicStatMan.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


const bool GLItemBasicStatMan::LoadData ( void )
{
	// Initialize Lua;
	sc::lua_init();

	std::string strFullPath( GLOGIC::GetPath() );
	strFullPath.append( GLogicData::GetInstance().GetItemBasicStatFileName() );

	std::auto_ptr< sc::BaseStream > pStream(
		GLOGIC::openfile_basestream(
		GLOGIC::UseLogicZipFile(),
		GLogicData::GetInstance().GetGlogicZipFile(),
		strFullPath.c_str(), 
		GLogicData::GetInstance().GetItemBasicStatFileName().c_str(), 
		false,
		GLOGIC::UseLogicPackFile() ) );

	if ( NULL == pStream.get() )		
	{
		sc::writeLogError(
			sc::string::format(
			"GLItemBasicStatMan::LoadFile(), File Open %1%", strFullPath ) );

		return false;
	}

	const size_t nSize = static_cast< size_t >( pStream->GetFileSize() );
	void* pBuffer = ::malloc( nSize );
	pStream->ReadBuffer( pBuffer, 0 );
	CTypeSmartPtr pSmartPtr( pBuffer );

	if ( false == sc::lua_doBuffer( static_cast<char*>( pBuffer ), nSize, NULL ) )
		return false;

	{
		LuaPlus::LuaObject sBasicGroup =
			sc::lua_getGlobalFromName( "ItemBasicStat" );
		if ( sBasicGroup.IsNil() )
			return false;

		int i = 0;
		for ( LuaPlus::LuaTableIterator iter( sBasicGroup.GetByName( "Attack" ) );
			iter; iter.Next() )
		{
			if ( EMITEM_LEVEL_NSIZE <= static_cast< EMITEMLEVEL >( i ) )
				break;

			std::string strItemSet = sc::GetStringByObject( iter.GetValue() );

			m_sAttackItemSetTypeManList[ i ].LoadData( strItemSet );

			++i;
		}

		i = 0;
		for ( LuaPlus::LuaTableIterator iter( sBasicGroup.GetByName( "Defence" ) );
			iter; iter.Next() )
		{
			if ( EMITEM_LEVEL_NSIZE <= static_cast< EMITEMLEVEL >( i ) )
				break;

			std::string strItemSet = sc::GetStringByObject( iter.GetValue() );

			m_sDefenceItemSetTypeManList[ i ].LoadData( strItemSet );

			++i;
		}
	}

	// Destroy Lua;
	sc::lua_destroyState();

	return true;
}


const GLPADATA GLItemBasicStatMan::GetAttackDamage_MakeItem (
	const EMITEMLEVEL _emItemLevel,
	const int _nType, 
	const int _nIndex ) const
{
	static const GLPADATA sDefault;

	if ( _emItemLevel < EMITEM_LEVEL_NORMAL || EMITEM_LEVEL_NSIZE <= _emItemLevel )
		return sDefault;

	return m_sAttackItemSetTypeManList[ _emItemLevel ].GetAttackDamage_MakeItem( _nType, _nIndex );
}

const GLPADATA GLItemBasicStatMan::GetAttackDamage_DropItem (
	const EMITEMLEVEL _emItemLevel,
	const int _nType, 
	const int _nIndex ) const
{
	static const GLPADATA sDefault;

	if ( _emItemLevel < EMITEM_LEVEL_NORMAL || EMITEM_LEVEL_NSIZE <= _emItemLevel )
		return sDefault;

	return m_sAttackItemSetTypeManList[ _emItemLevel ].GetAttackDamage_DropItem( _nType, _nIndex );
}

const WORD GLItemBasicStatMan::GetDefence_MakeItem (
	const EMITEMLEVEL _emItemLevel,
	const int _nType, 
	const int _nIndex ) const
{
	static const WORD wDefault;

	if ( _emItemLevel < EMITEM_LEVEL_NORMAL || EMITEM_LEVEL_NSIZE <= _emItemLevel )
		return wDefault;

	return m_sDefenceItemSetTypeManList[ _emItemLevel ].GetDefence_MakeItem( _nType, _nIndex );
}

const WORD GLItemBasicStatMan::GetDefence_DropItem (
	const EMITEMLEVEL _emItemLevel,
	const int _nType, 
	const int _nIndex ) const
{
	static const WORD wDefault;

	if ( _emItemLevel < EMITEM_LEVEL_NORMAL || EMITEM_LEVEL_NSIZE <= _emItemLevel )
		return wDefault;

	return m_sDefenceItemSetTypeManList[ _emItemLevel ].GetDefence_MakeItem( _nType, _nIndex );
}

const int GLItemBasicStatMan::GetSizeAttackItemSetMan ( const EMITEMLEVEL _emItemLevel, const int _nType ) const
{
	if ( _emItemLevel < EMITEM_LEVEL_NORMAL || EMITEM_LEVEL_NSIZE <= _emItemLevel )
		return 0;

	return m_sAttackItemSetTypeManList[ _emItemLevel ].GetSize( _nType );
}

const int GLItemBasicStatMan::GetSizeDefenceItemSetMan ( const EMITEMLEVEL _emItemLevel, const int _nType ) const
{
	if ( _emItemLevel < EMITEM_LEVEL_NORMAL || EMITEM_LEVEL_NSIZE <= _emItemLevel )
		return 0;

	return m_sDefenceItemSetTypeManList[ _emItemLevel ].GetSize( _nType );
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
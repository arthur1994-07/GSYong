
#include "pch.h"

#include "../../../SigmaCore/Lua/MinLua.h"
#include "../../../SigmaCore/Util/SystemInfo.h"

#include "../../../EngineLib/Common/SUBPATH.h"
#include "../../../EngineLib/G-Logic/GLogic.h"

#include "../../../RanLogic/GLogicDataMan.h"

#include "./GLItemLevel.h"

bool GLItemLevelMan::LoadData ()
{
	// Initialize Lua;
	sc::lua_init();

	std::string strFullPath( GLOGIC::GetPath() );
	strFullPath.append( GLogicData::GetInstance().GetItemLevelFileName() );

	std::auto_ptr< sc::BaseStream > pStream(
		GLOGIC::openfile_basestream(
		GLOGIC::UseLogicZipFile(),
		GLogicData::GetInstance().GetGlogicZipFile(),
		strFullPath.c_str(), 
		GLogicData::GetInstance().GetItemLevelFileName().c_str(), 
		false,
		GLOGIC::UseLogicPackFile() ) );

	if ( NULL == pStream.get() )		
	{
		sc::writeLogError(
			sc::string::format(
			"GLItemLevelMan::LoadFile(), File Open %1%", strFullPath ) );

		return false;
	}

	const size_t nSize = static_cast< size_t >( pStream->GetFileSize() );
	void* pBuffer = ::malloc( nSize );
	pStream->ReadBuffer( pBuffer, 0 );
	CTypeSmartPtr pSmartPtr( pBuffer );

	if ( false == sc::lua_doBuffer( static_cast<char*>( pBuffer ), nSize, NULL ) )
		return false;


	/*// Script 파일의 주소;
	SUBPATH* pPath = GLogicData::GetInstance().GetPath();
	if ( NULL == pPath )
		return false;

	// Path;
	std::string strScriptPath( sc::getAppPath() );
	strScriptPath += pPath->GLogicPath();

	if ( false == sc::lua_doFile(
		strScriptPath + GLogicData::GetInstance().GetItemLevelFileName() ) )
		return false;*/

	{
		LuaPlus::LuaObject sBasicGroup =
			sc::lua_getGlobalFromName( "ItemLevel" );
		if ( sBasicGroup.IsNil() )
			return false;

		int nItemLevel = 0;
		for ( LuaPlus::LuaTableIterator iter( sBasicGroup );
			iter; iter.Next() )
		{
			if ( EMITEM_LEVEL_NSIZE <= static_cast< EMITEMLEVEL >( nItemLevel ) )
				break;

			LuaPlus::LuaObject sTempMember( iter.GetValue() );

			m_sArrInfoByItemLevel[ nItemLevel ].wMaxGrade = sc::GetIntegerByName( sTempMember, "wMaxGrade" );
			m_sArrInfoByItemLevel[ nItemLevel ].wDamageByGrade = sc::GetIntegerByName( sTempMember, "wDamageByGrade" );
			m_sArrInfoByItemLevel[ nItemLevel ].wDefenceByGrade = sc::GetIntegerByName( sTempMember, "wDefenceByGrade" );

			m_sArrInfoByItemLevel[ nItemLevel ].fDamageRateByGrade = sc::GetNumberByName( sTempMember, "fDamageRateByGrade" );
			m_sArrInfoByItemLevel[ nItemLevel ].fDefenceRateByGrade = sc::GetNumberByName( sTempMember, "fDefenceRateByGrade" );

			m_sArrInfoByItemLevel[ nItemLevel ].wMaxResist = sc::GetIntegerByName( sTempMember, "wMaxResist" );
			m_sArrInfoByItemLevel[ nItemLevel ].wFireResistByGrade = sc::GetIntegerByName( sTempMember, "wFireResistByGrade" );
			m_sArrInfoByItemLevel[ nItemLevel ].wIceResistByGrade = sc::GetIntegerByName( sTempMember, "wIceResistByGrade" );
			m_sArrInfoByItemLevel[ nItemLevel ].wElectricResistByGrade = sc::GetIntegerByName( sTempMember, "wElectricResistByGrade" );
			m_sArrInfoByItemLevel[ nItemLevel ].wPoisonResistByGrade = sc::GetIntegerByName( sTempMember, "wPoisonResistByGrade" );
			m_sArrInfoByItemLevel[ nItemLevel ].wSpiritResistByGrade = sc::GetIntegerByName( sTempMember, "wSpiritResistByGrade" );

			int i = 0;
			for ( LuaPlus::LuaTableIterator tempIter( sTempMember.GetByName( "wArrUseItemCountByHighGrade" ) );
				tempIter; tempIter.Next() )
			{
				if ( GRADE_LIMIT_MAX-GRADE_HIGH <= i )
					break;

				m_sArrInfoByItemLevel[ nItemLevel ].wArrUseItemCountByHighGrade[ i ] = sc::GetIntegerByObject( tempIter.GetValue() );

				 ++i;
			}

			i = 0;
			for ( LuaPlus::LuaTableIterator tempIter( sTempMember.GetByName( "fArrDamageRateByHighGrade" ) );
				tempIter; tempIter.Next() )
			{
				if ( GRADE_LIMIT_MAX-GRADE_HIGH <= i )
					break;

				m_sArrInfoByItemLevel[ nItemLevel ].fArrDamageRateByHighGrade[ i ] = sc::GetNumberByObject( tempIter.GetValue() );

				++i;
			}

			i = 0;
			for ( LuaPlus::LuaTableIterator tempIter( sTempMember.GetByName( "fArrDefenceRateByHighGrade" ) );
				tempIter; tempIter.Next() )
			{
				if ( GRADE_LIMIT_MAX-GRADE_HIGH <= i )
					break;

				m_sArrInfoByItemLevel[ nItemLevel ].fArrDefenceRateByHighGrade[ i ] = sc::GetNumberByObject( tempIter.GetValue() );

				++i;
			}

			i = 0;
			for ( LuaPlus::LuaTableIterator tempIter( sTempMember.GetByName( "fArrSuccessRateByGrade" ) );
				tempIter; tempIter.Next() )
			{
				if ( GRADE_LIMIT_MAX <= i )
					break;

				m_sArrInfoByItemLevel[ nItemLevel ].fArrSuccessRateByGrade[ i ] = sc::GetNumberByObject( tempIter.GetValue() );

				++i;
			}

			i = 0;
			for ( LuaPlus::LuaTableIterator tempIter( sTempMember.GetByName( "fArrResetRateByGrade" ) );
				tempIter; tempIter.Next() )
			{
				if ( GRADE_LIMIT_MAX <= i )
					break;

				m_sArrInfoByItemLevel[ nItemLevel ].fArrResetRateByGrade[ i ] = sc::GetNumberByObject( tempIter.GetValue() );

				++i;
			}

			i = 0;
			for ( LuaPlus::LuaTableIterator tempIter( sTempMember.GetByName( "fArrTerminateRateByGrade" ) );
				tempIter; tempIter.Next() )
			{
				if ( GRADE_LIMIT_MAX <= i )
					break;

				m_sArrInfoByItemLevel[ nItemLevel ].fArrTerminateRateByGrade[ i ] = sc::GetNumberByObject( tempIter.GetValue() );

				++i;
			}

			i = 0;
			for ( LuaPlus::LuaTableIterator tempIter( sTempMember.GetByName( "fArrItemOptionIncreaseRateByGrade" ) );
				tempIter; tempIter.Next() )
			{
				if ( GRADE_LIMIT_MAX <= i )
					break;

				m_sArrInfoByItemLevel[ nItemLevel ].fArrItemOptionIncreaseRateByGrade[ i ] = sc::GetNumberByObject( tempIter.GetValue() );

				++i;
			}

			i = 0;
			for ( LuaPlus::LuaTableIterator tempIter( sTempMember.GetByName( "fArrItemOptionAntiResetRateByGrade" ) );
				tempIter; tempIter.Next() )
			{
				if ( GRADE_LIMIT_MAX <= i )
					break;

				m_sArrInfoByItemLevel[ nItemLevel ].fArrItemOptionAntiResetRateByGrade[ i ] = sc::GetNumberByObject( tempIter.GetValue() );

				++i;
			}

			m_sArrInfoByItemLevel[ nItemLevel ].bTerminateItem = sc::GetBooleanByName( sTempMember, "bTerminateItem" );

			++nItemLevel;
		}
	}

	// Destroy Lua;
	sc::lua_destroyState();
}

const SINFO_BYITEMLEVEL& GLItemLevelMan::GetInfoByItemLevel ( const EMITEMLEVEL _emItemLevel ) const
{
	static SINFO_BYITEMLEVEL sDefault;

	if ( _emItemLevel < EMITEM_LEVEL_NORMAL || EMITEM_LEVEL_NSIZE <= _emItemLevel )
		return sDefault;

	return m_sArrInfoByItemLevel[ _emItemLevel ];
}
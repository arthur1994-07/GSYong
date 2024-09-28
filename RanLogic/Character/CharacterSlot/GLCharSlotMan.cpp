#include "pch.h"

#include "../../../SigmaCore/Lua/MinLua.h"
#include "../../../SigmaCore/Util/SystemInfo.h"

#include "../../../EngineLib/Common/SUBPATH.h"
#include "../../../EngineLib/G-Logic/GLogic.h"

#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/GLogicData.h"

#include "./GLCharSlotMan.h"


BOOL SCHAR_SLOT_DATA::SETEXPSKILLS_BYBUF ( se::ByteStream& sByteStream )
{
	if ( sByteStream.IsEmpty() )
		return TRUE;

	m_ExpSkills.clear();

	DWORD dwVersion = 0;
	DWORD dwSize = 0;
	DWORD dwNum = 0;;

	sByteStream >> dwVersion;
	sByteStream >> dwSize;
	sByteStream >> dwNum;

	GASSERT ( ( dwSize==sizeof( SCHARSKILL ) ) && ( SCHARSKILL::VERSION == dwVersion ) );

	SCHARSKILL sCharSkill;
	for ( DWORD i=0; i<dwNum; i++ )
	{
		sByteStream.ReadBuffer( (LPBYTE)&sCharSkill, sizeof( SCHARSKILL ) );
		m_ExpSkills.insert( std::make_pair( sCharSkill.sNativeID.dwID, sCharSkill ) );
	}

	return TRUE;
}

BOOL SCHAR_SLOT_DATA::SETSKILL_QUICKSLOT ( se::ByteStream& sByteStream )
{
	if ( sByteStream.IsEmpty() )
		return TRUE;

	for ( int i = 0; i<EMSKILLQUICK_SIZE; ++i )
		m_sSKILLQUICK[i] = NATIVEID_NULL();

	DWORD dwVersion = 0;
	DWORD dwSize = 0;
	DWORD dwNum = 0;;

	sByteStream >> dwVersion;

	switch ( dwVersion )
	{
	case 0x0100:
		{
			sByteStream >> dwSize;
			sByteStream >> dwNum;

			SNATIVEID sSKILL_ID;
			for ( DWORD i = 0; i < dwNum; ++i )
			{
				if ( i >= EMSKILLQUICK_SIZE_OLD )
					break;

				GASSERT( dwSize == sizeof( SNATIVEID ) );

				sByteStream.ReadBuffer( (LPBYTE)&sSKILL_ID, sizeof( SNATIVEID ) );

				m_sSKILLQUICK[ i ] = sSKILL_ID;
			}
		}
		break;

	case 0x0101:
		{
			sByteStream >> dwSize;
			sByteStream >> dwNum;

			SNATIVEID sSKILL_ID;
			for ( DWORD i = 0; i < dwNum; ++i )
			{
				if ( i >= EMSKILLQUICK_SIZE_OLD )
					break;

				GASSERT( dwSize == sizeof( SNATIVEID ) );

				sByteStream.ReadBuffer( (LPBYTE)&sSKILL_ID, sizeof( SNATIVEID ) );

				m_sSKILLQUICK[ i ] = sSKILL_ID;
			}

			sByteStream >> m_wSKILLQUICK_ACT;
		}
		break;

	case EMSKILLQUICK_VERSION:
		{
			sByteStream >> dwSize;
			sByteStream >> dwNum;

			SNATIVEID sSKILL_ID;
			for ( DWORD i = 0; i < dwNum; ++i )
			{
				if ( i >= EMSKILLQUICK_SIZE )
					break;

				GASSERT ( dwSize == sizeof( SNATIVEID ) );

				sByteStream.ReadBuffer( (LPBYTE)&sSKILL_ID, sizeof( SNATIVEID ) );

				m_sSKILLQUICK[ i ] = sSKILL_ID;
			}

			sByteStream >> m_wSKILLQUICK_ACT;
		}
		break;
	};

	return TRUE;
}


BOOL SCHAR_SLOT_DATA::GETEXPSKILLS_BYBUF ( se::ByteStream &ByteStream ) const
{
	ByteStream.ClearBuffer();

	ByteStream << SCHARSKILL::VERSION;
	ByteStream << SCHARSKILL::SIZE;

	DWORD dwSize = (DWORD)m_ExpSkills.size();
	ByteStream << dwSize;

	SKILL_MAP_CITER iter = m_ExpSkills.begin ();
	SKILL_MAP_CITER iter_end = m_ExpSkills.end ();

	for ( ; iter!=iter_end; ++iter )
	{
		const SCHARSKILL& sSkill = (*iter).second;
		ByteStream.WriteBuffer( &sSkill, (DWORD)sizeof( SCHARSKILL ) );
	}

	return TRUE;
}

BOOL SCHAR_SLOT_DATA::GETSKILL_QUICKSLOT ( se::ByteStream &ByteStream ) const
{
	ByteStream.ClearBuffer ();

	ByteStream << EMSKILLQUICK_VERSION;
	ByteStream << (DWORD)sizeof( SNATIVEID );

	DWORD dwSize = (DWORD) EMSKILLQUICK_SIZE;
	ByteStream << dwSize;

	ByteStream.WriteBuffer( (LPBYTE)m_sSKILLQUICK, (DWORD)sizeof( SNATIVEID )*dwSize );

	ByteStream << m_wSKILLQUICK_ACT;

	return TRUE;
}

DWORD SCHAR_SLOT_DATA::GetExpSkillsDataSize ( void ) const
{
	DWORD dwDataSize = 0;

	dwDataSize += sizeof( SCHARSKILL::VERSION );
	dwDataSize += sizeof( SCHARSKILL::SIZE );

	dwDataSize += sizeof( DWORD );
	dwDataSize += sizeof( SCHARSKILL ) * m_ExpSkills.size();

	return dwDataSize;
}

BOOL SCHAR_SLOT_DATA::GetExpSkillsData ( BYTE* pDest, const DWORD dwDestSize ) const
{
	se::SerializeWritableData data( pDest, dwDestSize );

	data << SCHARSKILL::VERSION;
	data << SCHARSKILL::SIZE;

	DWORD dwSize = (DWORD) m_ExpSkills.size();
	data << dwSize;

	SKILL_MAP_CITER iter = m_ExpSkills.begin ();
	SKILL_MAP_CITER iter_end = m_ExpSkills.end ();
	for ( ;iter != iter_end; ++iter )
	{
		data.Write( &(*iter).second, sizeof( SCHARSKILL ) );
	}

	return TRUE;
}

DWORD SCHAR_SLOT_DATA::GetSkillQuickSlotDataSize ( void ) const
{
	DWORD dwDataSize = 0;

	dwDataSize += sizeof( EMSKILLQUICK_VERSION );
	dwDataSize += sizeof( DWORD );		//sizeof(SNATIVEID);

	dwDataSize += sizeof( DWORD );		//EMSKILLQUICK_SIZE;
	dwDataSize += sizeof( SNATIVEID ) * EMSKILLQUICK_SIZE;

	dwDataSize += sizeof( m_wSKILLQUICK_ACT );

	return dwDataSize;
}

BOOL SCHAR_SLOT_DATA::GetSkillQuickSlotData( BYTE* pDest, const DWORD dwDestSize ) const
{
	se::SerializeWritableData data( pDest, dwDestSize );

	data << EMSKILLQUICK_VERSION
		<< (DWORD)sizeof(SNATIVEID)
		<< (DWORD)EMSKILLQUICK_SIZE;

	data.Write( (LPBYTE)m_sSKILLQUICK, sizeof( SNATIVEID ) * EMSKILLQUICK_SIZE );

	data << m_wSKILLQUICK_ACT;

	return TRUE;
}



const bool GLCharSlot::SaveCharSlot ( const WORD _wChaSlot
									, const SCHAR_SLOT_DATA& _sSlotData )
{
	if ( 0 <= _wChaSlot && _wChaSlot < EMCHAR_SLOT_DATA_SIZE )
	{
		m_arrSlotData[ _wChaSlot ] = _sSlotData;

		return true;
	}

	return false;
}

const SCHAR_SLOT_DATA& GLCharSlot::GetSlotData ( const WORD _wChaSlot ) const
{
	static SCHAR_SLOT_DATA sDefault;

	if ( 0 <= _wChaSlot && _wChaSlot < EMCHAR_SLOT_DATA_SIZE )
	{
		return m_arrSlotData[ _wChaSlot ];
	}

	return sDefault;
}

void GLCharSlot::SkillPointUp ( const WORD _wSkillPoint )
{
	for ( int i=0; i<EMCHAR_SLOT_DATA_SIZE; ++i )
	{
		if ( m_arrSlotData[ i ].IsActiveSlot() )
		{
			m_arrSlotData[ i ].m_dwSkillPoint += _wSkillPoint;
		}
	}
}

void GLCharSlot::StatPointUp ( const WORD _wStatPoint )
{
	for ( int i=0; i<EMCHAR_SLOT_DATA_SIZE; ++i )
	{
		if ( m_arrSlotData[ i ].IsActiveSlot() )
		{
			m_arrSlotData[ i ].m_wStatsPoint += _wStatPoint;
		}
	}
}



const bool GLCharSlotMan::LoadData ( void )
{
	// Initialize Lua;
	sc::lua_init();

	std::string strFullPath( GLOGIC::GetPath() );
	strFullPath.append( GLogicData::GetInstance().GetCharacterSlotFileName() );

	std::auto_ptr< sc::BaseStream > pStream(
		GLOGIC::openfile_basestream(
		GLOGIC::UseLogicZipFile(),
		GLogicData::GetInstance().GetGlogicZipFile(),
		strFullPath.c_str(), 
		GLogicData::GetInstance().GetCharacterSlotFileName().c_str(), 
		false,
		GLOGIC::UseLogicPackFile() ) );

	if ( NULL == pStream.get() )		
	{
		sc::writeLogError(
			sc::string::format(
			"GLCharSlotMan::LoadFile(), File Open %1%", strFullPath ) );

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
			sc::lua_getGlobalFromName( "CharacterSlot" );
		if ( sBasicGroup.IsNil() )
			return false;

		m_nOpenSlotCount = sc::GetIntegerByName( sBasicGroup, "nOpenSlotCount" );
		m_nDefaultActiveSlotCount = sc::GetIntegerByName( sBasicGroup, "nDefaultActiveSlotCount" );
		m_nCoolTime_ChangeSlot = sc::GetIntegerByName( sBasicGroup, "nCoolTime_ChangeSlot" );
	}

	// Destroy Lua;
	sc::lua_destroyState();
}

const bool GLCharSlotMan::SaveCharSlot ( const DWORD _dwChaDBNum
									   , const WORD _wChaSlot
									   , const SCHAR_SLOT_DATA& _sSlotData )
{
	MAP_CHARSLOT_ITER iter = m_mapCharSlot.find( _dwChaDBNum );
	if ( iter != m_mapCharSlot.end() )
	{
		GLCharSlot& sCharSlot = iter->second;
		return sCharSlot.SaveCharSlot( _wChaSlot, _sSlotData );
	}
	else
	{
		GLCharSlot sCharSlot( _dwChaDBNum );
		if ( false == sCharSlot.SaveCharSlot( _wChaSlot, _sSlotData ) )
			return false;

		m_mapCharSlot.insert( MAP_CHARSLOT_VALUE( _dwChaDBNum, sCharSlot ) );

		return true;
	}

	return false;
}

const SCHAR_SLOT_DATA& GLCharSlotMan::GetSlotData ( const DWORD _dwChaDBNum
												  , const WORD _wChaSlot ) const
{
	static SCHAR_SLOT_DATA sDefault;

	MAP_CHARSLOT_CITER iter = m_mapCharSlot.find( _dwChaDBNum );
	if ( iter != m_mapCharSlot.end() )
	{
		const GLCharSlot& sCharSlot = iter->second;
		return sCharSlot.GetSlotData( _wChaSlot );
	}

	return sDefault;
}

void GLCharSlotMan::SkillPointUp ( const DWORD _dwChaDBNum
								 , const WORD _wSkillPoint )
{
	MAP_CHARSLOT_ITER iter = m_mapCharSlot.find( _dwChaDBNum );
	if ( iter != m_mapCharSlot.end() )
	{
		GLCharSlot& sCharSlot = iter->second;
		sCharSlot.SkillPointUp( _wSkillPoint );
	}
}

void GLCharSlotMan::StatPointUp ( const DWORD _dwChaDBNum
								, const WORD _wStatPoint )
{
	MAP_CHARSLOT_ITER iter = m_mapCharSlot.find( _dwChaDBNum );
	if ( iter != m_mapCharSlot.end() )
	{
		GLCharSlot& sCharSlot = iter->second;
		sCharSlot.StatPointUp( _wStatPoint );
	}
}
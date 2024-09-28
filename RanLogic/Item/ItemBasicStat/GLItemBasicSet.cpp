#include "pch.h"

#include "../../../SigmaCore/Math/Random.h"
#include "../../../SigmaCore/Lua/MinLua.h"
#include "../../../SigmaCore/Util/SystemInfo.h"

#include "../../../EngineLib/Common/SUBPATH.h"
#include "../../../EngineLib/G-Logic/GLogic.h"

#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/GLogicData.h"

#include "./GLItemBasicSet.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


const bool CBasicItemRandomAttackSet::LoadData_MakeItem ( LuaPlus::LuaObject& _sMember )
{
	// �� Ȯ�� �� ������ �����Ѵ�;
	WORD wProbabilityNum = 0;
	for ( LuaPlus::LuaTableIterator iter( _sMember );
		iter; iter.Next() )
	{
		// Ȯ���� ��� ���� BASIC_RANDOM_STAT_PROBABILITY_MAXNUM ������ �����ϴ�;
		// �������� ������ �ʴ´�;
		if ( wProbabilityNum >= BASIC_RANDOM_STAT_PROBABILITY_MAXNUM-1 )
			break;

		LuaPlus::LuaTableIterator tempMemberIter( iter.GetValue() );

		// ������ Ȯ�� ���� 0 �� ��� Ȯ���� �����Ƿ� �������� �ʴ´�;
		const int nProbabolityNum_Data = sc::GetIntegerByObject( tempMemberIter.GetValue() );
		if ( nProbabolityNum_Data <= 0 )
			continue;

		SBASIC_ITEM_RANDOM_ATTACK sTempItemRandomAttack;

		// Ȯ�� ������ ���� �ּ�, �ִ� ������ �����Ѵ�;
		sTempItemRandomAttack.SetProbabilityMinNum( wProbabilityNum );
		sTempItemRandomAttack.SetProbabilityMaxNum( wProbabilityNum + nProbabolityNum_Data - 1 );
		wProbabilityNum = sTempItemRandomAttack.GetProbabilityMaxNum() + 1;
		tempMemberIter.Next();

		// �ּҴ������ ������ �����Ѵ�;
		sTempItemRandomAttack.SetMinDamageStart( sc::GetIntegerByObject( tempMemberIter.GetValue() ) );
		tempMemberIter.Next();
		sTempItemRandomAttack.SetMinDamageEnd( sc::GetIntegerByObject( tempMemberIter.GetValue() ) );
		tempMemberIter.Next();

		// �ִ������� ������ �����Ѵ�;
		sTempItemRandomAttack.SetMaxDamageStart( sc::GetIntegerByObject( tempMemberIter.GetValue() ) );
		tempMemberIter.Next();
		sTempItemRandomAttack.SetMaxDamageEnd( sc::GetIntegerByObject( tempMemberIter.GetValue() ) );
		tempMemberIter.Next();

		m_vecMakeList.push_back( sTempItemRandomAttack );
	}

	return true;
}

const bool CBasicItemRandomAttackSet::LoadData_DropItem ( LuaPlus::LuaObject& _sMember )
{
	// �� Ȯ�� �� ������ �����Ѵ�;
	WORD wProbabilityNum = 0;
	for ( LuaPlus::LuaTableIterator iter( _sMember );
		iter; iter.Next() )
	{
		// Ȯ���� ��� ���� BASIC_RANDOM_STAT_PROBABILITY_MAXNUM ������ �����ϴ�;
		// �������� ������ �ʴ´�;
		if ( wProbabilityNum >= BASIC_RANDOM_STAT_PROBABILITY_MAXNUM-1 )
			break;

		LuaPlus::LuaTableIterator tempMemberIter( iter.GetValue() );

		// ������ Ȯ�� ���� 0 �� ��� Ȯ���� �����Ƿ� �������� �ʴ´�;
		const int nProbabolityNum_Data = sc::GetIntegerByObject( tempMemberIter.GetValue() );
		if ( nProbabolityNum_Data <= 0 )
			continue;

		SBASIC_ITEM_RANDOM_ATTACK sTempItemRandomAttack;

		// Ȯ�� ������ ���� �ּ�, �ִ� ������ �����Ѵ�;
		sTempItemRandomAttack.SetProbabilityMinNum( wProbabilityNum );
		sTempItemRandomAttack.SetProbabilityMaxNum( wProbabilityNum + nProbabolityNum_Data - 1 );
		wProbabilityNum = sTempItemRandomAttack.GetProbabilityMaxNum() + 1;
		tempMemberIter.Next();

		// �ּҴ������ ������ �����Ѵ�;
		sTempItemRandomAttack.SetMinDamageStart( sc::GetIntegerByObject( tempMemberIter.GetValue() ) );
		tempMemberIter.Next();
		sTempItemRandomAttack.SetMinDamageEnd( sc::GetIntegerByObject( tempMemberIter.GetValue() ) );
		tempMemberIter.Next();

		// �ִ������� ������ �����Ѵ�;
		sTempItemRandomAttack.SetMaxDamageStart( sc::GetIntegerByObject( tempMemberIter.GetValue() ) );
		tempMemberIter.Next();
		sTempItemRandomAttack.SetMaxDamageEnd( sc::GetIntegerByObject( tempMemberIter.GetValue() ) );
		tempMemberIter.Next();

		m_vecDropList.push_back( sTempItemRandomAttack );
	}

	return true;
}


const GLPADATA CBasicItemRandomAttackSet::GetAttackDamage_MakeItem ( void ) const
{
	static const GLPADATA sDefault;

	// 0~99 ������ �� �� �������� �ϳ� �����Ѵ�;
	unsigned int wProbabilityNum = sc::Random::getInstance().GetPercent_1000();

	// MakeList �� �� ���� ���ԵǴ� ������ ���Ѵ�;
	int nSelectedIndex = -1;
	for ( int i=0; i<m_vecMakeList.size(); ++i )
	{
		const SBASIC_ITEM_RANDOM_ATTACK& sTempRandomAttack = m_vecMakeList.at( i );
		if ( wProbabilityNum < sTempRandomAttack.GetProbabilityMinNum() )
			continue;

		if ( sTempRandomAttack.GetProbabilityMaxNum() < wProbabilityNum )
			continue;

		nSelectedIndex = i;
		break;
	}

	if ( -1 == nSelectedIndex )
		return sDefault;

	// ���õ� RandomAttack �� �̿��Ͽ� �ּ�, �ִ� ������� ���Ѵ�;
	const SBASIC_ITEM_RANDOM_ATTACK& sTempRandomAttack = m_vecMakeList.at( nSelectedIndex );
	
	WORD wMinDamage = sc::Random::getInstance().RandomNumber(
		sTempRandomAttack.GetMinDamageStart(),
		sTempRandomAttack.GetMinDamageEnd() );
	WORD wMaxDamage = sc::Random::getInstance().RandomNumber(
		sTempRandomAttack.GetMaxDamageStart(),
		sTempRandomAttack.GetMaxDamageEnd() );

	return GLPADATA( wMinDamage, wMaxDamage );
}

const GLPADATA CBasicItemRandomAttackSet::GetAttackDamage_DropItem ( void ) const
{
	static const GLPADATA sDefault;

	// 0~99 ������ �� �� �������� �ϳ� �����Ѵ�;
	unsigned int wProbabilityNum = sc::Random::getInstance().GetPercent_1000();

	// MakeList �� �� ���� ���ԵǴ� ������ ���Ѵ�;
	int nSelectedIndex = -1;
	for ( int i=0; i<m_vecDropList.size(); ++i )
	{
		const SBASIC_ITEM_RANDOM_ATTACK& sTempRandomAttack = m_vecDropList.at( i );
		if ( wProbabilityNum < sTempRandomAttack.GetProbabilityMinNum() )
			continue;

		if ( sTempRandomAttack.GetProbabilityMaxNum() < wProbabilityNum )
			continue;

		nSelectedIndex = i;
		break;
	}

	if ( -1 == nSelectedIndex )
		return sDefault;

	// ���õ� RandomAttack �� �̿��Ͽ� �ּ�, �ִ� ������� ���Ѵ�;
	const SBASIC_ITEM_RANDOM_ATTACK& sTempRandomAttack = m_vecDropList.at( nSelectedIndex );

	WORD wMinDamage = sc::Random::getInstance().RandomNumber(
		sTempRandomAttack.GetMinDamageStart(),
		sTempRandomAttack.GetMinDamageEnd() );
	WORD wMaxDamage = sc::Random::getInstance().RandomNumber(
		sTempRandomAttack.GetMaxDamageStart(),
		sTempRandomAttack.GetMaxDamageEnd() );

	return GLPADATA( wMinDamage, wMaxDamage );
}



const bool CBasicItemRandomDefenceSet::LoadData_MakeItem ( LuaPlus::LuaObject& _sMember )
{
	// �� Ȯ�� �� ������ �����Ѵ�;
	WORD wProbabilityNum = 0;
	for ( LuaPlus::LuaTableIterator iter( _sMember );
		iter; iter.Next() )
	{
		// Ȯ���� ��� ���� BASIC_RANDOM_STAT_PROBABILITY_MAXNUM ������ �����ϴ�;
		// �������� ������ �ʴ´�;
		if ( wProbabilityNum >= BASIC_RANDOM_STAT_PROBABILITY_MAXNUM-1 )
			break;

		LuaPlus::LuaTableIterator tempMemberIter( iter.GetValue() );

		// ������ Ȯ�� ���� 0 �� ��� Ȯ���� �����Ƿ� �������� �ʴ´�;
		const int nProbabolityNum_Data = sc::GetIntegerByObject( tempMemberIter.GetValue() );
		if ( nProbabolityNum_Data <= 0 )
			continue;

		SBASIC_ITEM_RANDOM_DEFENCE sTempItemRandomDefence;

		// Ȯ�� ������ ���� �ּ�, �ִ� ������ �����Ѵ�;
		sTempItemRandomDefence.SetProbabilityMinNum( wProbabilityNum );
		sTempItemRandomDefence.SetProbabilityMaxNum( wProbabilityNum + nProbabolityNum_Data - 1 );
		wProbabilityNum = sTempItemRandomDefence.GetProbabilityMaxNum() + 1;
		tempMemberIter.Next();

		// ������ ������ �����Ѵ�;
		sTempItemRandomDefence.SetDefenseStart( sc::GetIntegerByObject( tempMemberIter.GetValue() ) );
		tempMemberIter.Next();
		sTempItemRandomDefence.SetDefenseEnd( sc::GetIntegerByObject( tempMemberIter.GetValue() ) );
		tempMemberIter.Next();

		m_vecMakeList.push_back( sTempItemRandomDefence );
	}

	return true;
}

const bool CBasicItemRandomDefenceSet::LoadData_DropItem ( LuaPlus::LuaObject& _sMember )
{
	// �� Ȯ�� �� ������ �����Ѵ�;
	WORD wProbabilityNum = 0;
	for ( LuaPlus::LuaTableIterator iter( _sMember );
		iter; iter.Next() )
	{
		// Ȯ���� ��� ���� BASIC_RANDOM_STAT_PROBABILITY_MAXNUM ������ �����ϴ�;
		// �������� ������ �ʴ´�;
		if ( wProbabilityNum >= BASIC_RANDOM_STAT_PROBABILITY_MAXNUM-1 )
			break;

		LuaPlus::LuaTableIterator tempMemberIter( iter.GetValue() );

		// ������ Ȯ�� ���� 0 �� ��� Ȯ���� �����Ƿ� �������� �ʴ´�;
		const int nProbabolityNum_Data = sc::GetIntegerByObject( tempMemberIter.GetValue() );
		if ( nProbabolityNum_Data <= 0 )
			continue;

		SBASIC_ITEM_RANDOM_DEFENCE sTempItemRandomDefence;

		// Ȯ�� ������ ���� �ּ�, �ִ� ������ �����Ѵ�;
		sTempItemRandomDefence.SetProbabilityMinNum( wProbabilityNum );
		sTempItemRandomDefence.SetProbabilityMaxNum( wProbabilityNum + nProbabolityNum_Data - 1 );
		wProbabilityNum = sTempItemRandomDefence.GetProbabilityMaxNum() + 1;
		tempMemberIter.Next();

		// ������ ������ �����Ѵ�;
		sTempItemRandomDefence.SetDefenseStart( sc::GetIntegerByObject( tempMemberIter.GetValue() ) );
		tempMemberIter.Next();
		sTempItemRandomDefence.SetDefenseEnd( sc::GetIntegerByObject( tempMemberIter.GetValue() ) );
		tempMemberIter.Next();

		m_vecDropList.push_back( sTempItemRandomDefence );
	}

	return true;
}


const WORD CBasicItemRandomDefenceSet::GetDefence_MakeItem ( void ) const
{
	static const WORD wDefault = 0;

	// 0~99 ������ �� �� �������� �ϳ� �����Ѵ�;
	unsigned int wProbabilityNum = sc::Random::getInstance().GetPercent_1000();

	// MakeList �� �� ���� ���ԵǴ� ������ ���Ѵ�;
	int nSelectedIndex = -1;
	for ( int i=0; i<m_vecMakeList.size(); ++i )
	{
		const SBASIC_ITEM_RANDOM_DEFENCE& sTempRandomDefence = m_vecMakeList.at( i );
		if ( wProbabilityNum < sTempRandomDefence.GetProbabilityMinNum() )
			continue;

		if ( sTempRandomDefence.GetProbabilityMaxNum() < wProbabilityNum )
			continue;

		nSelectedIndex = i;
		break;
	}

	if ( -1 == nSelectedIndex )
		return wDefault;

	// ���õ� RandomDefence �� �̿��Ͽ� ������ ���Ѵ�;
	const SBASIC_ITEM_RANDOM_DEFENCE& sTempRandomDefence = m_vecMakeList.at( nSelectedIndex );

	WORD wDefence = sc::Random::getInstance().RandomNumber(
		sTempRandomDefence.GetDefenseStart(),
		sTempRandomDefence.GetDefenseEnd() );

	return wDefence;
}

const WORD CBasicItemRandomDefenceSet::GetDefence_DropItem ( void ) const
{
	static const WORD wDefault = 0;

	// 0~99 ������ �� �� �������� �ϳ� �����Ѵ�;
	unsigned int wProbabilityNum = sc::Random::getInstance().GetPercent_1000();

	// MakeList �� �� ���� ���ԵǴ� ������ ���Ѵ�;
	int nSelectedIndex = -1;
	for ( int i=0; i<m_vecDropList.size(); ++i )
	{
		const SBASIC_ITEM_RANDOM_DEFENCE& sTempRandomDefence = m_vecDropList.at( i );
		if ( wProbabilityNum < sTempRandomDefence.GetProbabilityMinNum() )
			continue;

		if ( sTempRandomDefence.GetProbabilityMaxNum() < wProbabilityNum )
			continue;

		nSelectedIndex = i;
		break;
	}

	if ( -1 == nSelectedIndex )
		return wDefault;

	// ���õ� RandomDefence �� �̿��Ͽ� ������ ���Ѵ�;
	const SBASIC_ITEM_RANDOM_DEFENCE& sTempRandomDefence = m_vecDropList.at( nSelectedIndex );

	WORD wDefence = sc::Random::getInstance().RandomNumber(
		sTempRandomDefence.GetDefenseStart(),
		sTempRandomDefence.GetDefenseEnd() );

	return wDefence;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


const bool CBasicItemRandomAttackSetMan::LoadData ( const std::string& _strItemSet )
{
	std::string strFullPath( GLOGIC::GetPath() );
	strFullPath.append( _strItemSet );

	std::auto_ptr< sc::BaseStream > pStream(
		GLOGIC::openfile_basestream(
		GLOGIC::UseLogicZipFile(),
		GLogicData::GetInstance().GetGlogicZipFile(),
		strFullPath.c_str(), 
		_strItemSet.c_str(), 
		false,
		GLOGIC::UseLogicPackFile() ) );

	if ( NULL == pStream.get() )		
	{
		sc::writeLogError(
			sc::string::format(
			"CBasicItemRandomAttackSetMan::LoadFile(), File Open %1%", strFullPath ) );

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
			sc::lua_getGlobalFromName( "ItemBasic" );
		if ( sBasicGroup.IsNil() )
			return false;

		for ( LuaPlus::LuaTableIterator iter( sBasicGroup );
			iter; iter.Next() )
		{
			LuaPlus::LuaObject sTempMember( iter.GetValue() );

			CBasicItemRandomAttackSet sItemSet;

			// Make ���� ���ݷ� ���;
			sItemSet.LoadData_MakeItem( sTempMember.GetByName( "Make" ) );

			// Drop ���� ���ݷ� ���;
			sItemSet.LoadData_DropItem( sTempMember.GetByName( "Drop" ) );

			m_vecAttackSetList.push_back( sItemSet );
		}
	}

	return true;
}


const GLPADATA CBasicItemRandomAttackSetMan::GetAttackDamage_MakeItem ( const int _nIndex ) const
{
	static const GLPADATA sDefault;

	if ( _nIndex < 0 || m_vecAttackSetList.size() <= _nIndex )
		return sDefault;

	const CBasicItemRandomAttackSet& sAttackSet = m_vecAttackSetList.at( _nIndex );
	return sAttackSet.GetAttackDamage_MakeItem();
}

const GLPADATA CBasicItemRandomAttackSetMan::GetAttackDamage_DropItem ( const int _nIndex ) const
{
	static const GLPADATA sDefault;

	if ( _nIndex < 0 || m_vecAttackSetList.size() <= _nIndex )
		return sDefault;

	const CBasicItemRandomAttackSet& sAttackSet = m_vecAttackSetList.at( _nIndex );
	return sAttackSet.GetAttackDamage_DropItem();
}



const bool CBasicItemRandomDefenceSetMan::LoadData ( const std::string& _strItemSet )
{
	std::string strFullPath( GLOGIC::GetPath() );
	strFullPath.append( _strItemSet );

	std::auto_ptr< sc::BaseStream > pStream(
		GLOGIC::openfile_basestream(
		GLOGIC::UseLogicZipFile(),
		GLogicData::GetInstance().GetGlogicZipFile(),
		strFullPath.c_str(), 
		_strItemSet.c_str(), 
		false,
		GLOGIC::UseLogicPackFile() ) );

	if ( NULL == pStream.get() )		
	{
		sc::writeLogError(
			sc::string::format(
			"CBasicItemRandomDefenceSetMan::LoadFile(), File Open %1%", strFullPath ) );

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
			sc::lua_getGlobalFromName( "ItemBasic" );
		if ( sBasicGroup.IsNil() )
			return false;

		for ( LuaPlus::LuaTableIterator iter( sBasicGroup );
			iter; iter.Next() )
		{
			LuaPlus::LuaObject sTempMember( iter.GetValue() );

			CBasicItemRandomDefenceSet sItemSet;

			// Make ���� ���� ���;
			sItemSet.LoadData_MakeItem( sTempMember.GetByName( "Make" ) );

			// Drop ���� ���� ���;
			sItemSet.LoadData_DropItem( sTempMember.GetByName( "Drop" ) );

			m_vecDefenceSetList.push_back( sItemSet );
		}
	}

	return true;
}


const WORD CBasicItemRandomDefenceSetMan::GetDefence_MakeItem ( const int _nIndex ) const
{
	static const WORD wDefault = 0;

	if ( _nIndex < 0 || m_vecDefenceSetList.size() <= _nIndex )
		return wDefault;

	const CBasicItemRandomDefenceSet& sDefenceSet = m_vecDefenceSetList.at( _nIndex );
	return sDefenceSet.GetDefence_MakeItem();
}

const WORD CBasicItemRandomDefenceSetMan::GetDefence_DropItem ( const int _nIndex ) const
{
	static const WORD wDefault = 0;

	if ( _nIndex < 0 || m_vecDefenceSetList.size() <= _nIndex )
		return wDefault;

	const CBasicItemRandomDefenceSet& sDefenceSet = m_vecDefenceSetList.at( _nIndex );
	return sDefenceSet.GetDefence_DropItem();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


const bool CBasicItemAttackTypeSetMan::LoadData ( const std::string& _strItemType )
{
	std::string strFullPath( GLOGIC::GetPath() );
	strFullPath.append( _strItemType );

	std::auto_ptr< sc::BaseStream > pStream(
		GLOGIC::openfile_basestream(
		GLOGIC::UseLogicZipFile(),
		GLogicData::GetInstance().GetGlogicZipFile(),
		strFullPath.c_str(), 
		_strItemType.c_str(), 
		false,
		GLOGIC::UseLogicPackFile() ) );

	if ( NULL == pStream.get() )		
	{
		sc::writeLogError(
			sc::string::format(
			"CBasicItemAttackTypeSetMan::LoadFile(), File Open %1%", strFullPath ) );

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
			sc::lua_getGlobalFromName( "ItemType" );
		if ( sBasicGroup.IsNil() )
			return false;

		for ( LuaPlus::LuaTableIterator iter( sBasicGroup );
			iter; iter.Next() )
		{
			CBasicItemRandomAttackSetMan sItemType;

			std::string strItemSetMan = sc::GetStringByObject( iter.GetValue() );
			sItemType.LoadData( strItemSetMan );

			m_vecAttackTypeSetList.push_back( sItemType );
		}
	}

	return true;
}

const GLPADATA CBasicItemAttackTypeSetMan::GetAttackDamage_MakeItem ( const int _nType, const int _nIndex ) const
{
	static const GLPADATA sDefault;

	if ( _nType < 0 || m_vecAttackTypeSetList.size() <= _nType )
		return sDefault;

	const CBasicItemRandomAttackSetMan& sItemType = m_vecAttackTypeSetList.at( _nType );
	return sItemType.GetAttackDamage_MakeItem( _nIndex );
}

const GLPADATA CBasicItemAttackTypeSetMan::GetAttackDamage_DropItem ( const int _nType, const int _nIndex ) const
{
	static const GLPADATA sDefault;

	if ( _nType < 0 || m_vecAttackTypeSetList.size() <= _nType )
		return sDefault;

	const CBasicItemRandomAttackSetMan& sItemType = m_vecAttackTypeSetList.at( _nType );
	return sItemType.GetAttackDamage_DropItem( _nIndex );
}

const int CBasicItemAttackTypeSetMan::GetSize ( const int _nType ) const
{
	if ( _nType < 0 || m_vecAttackTypeSetList.size() <= _nType )
		return 0;

	const CBasicItemRandomAttackSetMan& sItemType = m_vecAttackTypeSetList.at( _nType );
	return sItemType.GetSize();
}

const bool CBasicItemDefenceTypeSetMan::LoadData ( const std::string& _strItemType )
{
	std::string strFullPath( GLOGIC::GetPath() );
	strFullPath.append( _strItemType );

	std::auto_ptr< sc::BaseStream > pStream(
		GLOGIC::openfile_basestream(
		GLOGIC::UseLogicZipFile(),
		GLogicData::GetInstance().GetGlogicZipFile(),
		strFullPath.c_str(), 
		_strItemType.c_str(), 
		false,
		GLOGIC::UseLogicPackFile() ) );

	if ( NULL == pStream.get() )		
	{
		sc::writeLogError(
			sc::string::format(
			"CBasicItemDefenceTypeSetMan::LoadFile(), File Open %1%", strFullPath ) );

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
			sc::lua_getGlobalFromName( "ItemType" );
		if ( sBasicGroup.IsNil() )
			return false;

		for ( LuaPlus::LuaTableIterator iter( sBasicGroup );
			iter; iter.Next() )
		{
			CBasicItemRandomDefenceSetMan sItemType;

			std::string strItemSetMan = sc::GetStringByObject( iter.GetValue() );
			sItemType.LoadData( strItemSetMan );

			m_vecDefenceTypeSetList.push_back( sItemType );
		}
	}

	return true;
}

const WORD CBasicItemDefenceTypeSetMan::GetDefence_MakeItem ( const int _nType, const int _nIndex ) const
{
	static const WORD wDefault = 0;

	if ( _nType < 0 || m_vecDefenceTypeSetList.size() <= _nType )
		return wDefault;

	const CBasicItemRandomDefenceSetMan& sItemType = m_vecDefenceTypeSetList.at( _nType );
	return sItemType.GetDefence_MakeItem( _nIndex );
}

const WORD CBasicItemDefenceTypeSetMan::GetDefence_DropItem ( const int _nType, const int _nIndex ) const
{
	static const WORD wDefault = 0;

	if ( _nType < 0 || m_vecDefenceTypeSetList.size() <= _nType )
		return wDefault;

	const CBasicItemRandomDefenceSetMan& sItemType = m_vecDefenceTypeSetList.at( _nType );
	return sItemType.GetDefence_DropItem( _nIndex );
}

const int CBasicItemDefenceTypeSetMan::GetSize ( const int _nType ) const
{
	if ( _nType < 0 || m_vecDefenceTypeSetList.size() <= _nType )
		return 0;

	const CBasicItemRandomDefenceSetMan& sItemType = m_vecDefenceTypeSetList.at( _nType );
	return sItemType.GetSize();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
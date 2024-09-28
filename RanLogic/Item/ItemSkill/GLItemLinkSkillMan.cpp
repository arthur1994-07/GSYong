#include "pch.h"

#include "../../../SigmaCore/Lua/MinLua.h"
#include "../../../SigmaCore/Util/SystemInfo.h"

#include "../../../EngineLib/Common/SUBPATH.h"
#include "../../../EngineLib/G-Logic/GLogic.h"

#include "../../../RanLogic/GLogicDataMan.h"
#include "../../../RanLogic/GLogicData.h"

#include "./GLItemLinkSkillMan.h"


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


const bool CItemLinkSkillSet::LoadData ( LuaPlus::LuaObject& _sMember )
{
	for ( LuaPlus::LuaTableIterator iter( _sMember );
		iter; iter.Next() )
	{
		LuaPlus::LuaTableIterator tempMemberIter( iter.GetValue() );

		SLINK_SKILL_INFO sLinkSkillInfo;

		sLinkSkillInfo.SetTarget( static_cast< EMITEM_SKILL_LINK >( sc::GetIntegerByObject( tempMemberIter.GetValue() ) ) );
		tempMemberIter.Next();
		sLinkSkillInfo.SetOccurRate( static_cast< float >( sc::GetNumberByObject( tempMemberIter.GetValue() ) ) );
		tempMemberIter.Next();

		SNATIVEID sSkillID;
		sSkillID.SetMid( static_cast< WORD >( sc::GetIntegerByObject( tempMemberIter.GetValue() ) ) );
		tempMemberIter.Next();
		sSkillID.SetSid( static_cast< WORD >( sc::GetIntegerByObject( tempMemberIter.GetValue() ) ) );
		tempMemberIter.Next();
		sLinkSkillInfo.SetSkillID( sSkillID );

		// 레벨을 세팅할 때 직관적으로 레벨 1을 1으로 세팅하므로 1을 빼야한다;
		// 실제로 레벨 1을 의미하는것은 0 이다;
		sLinkSkillInfo.SetMinLevel( sc::GetIntegerByObject( tempMemberIter.GetValue() ) - 1 );
		tempMemberIter.Next();
		sLinkSkillInfo.SetMaxLevel( sc::GetIntegerByObject( tempMemberIter.GetValue() ) - 1 );
		tempMemberIter.Next();

		m_vecItemLinkSkill.push_back( sLinkSkillInfo );
	}

	return true;
}


void CItemLinkSkillSet::GetLinkSkill_Random ( EMITEM_SKILL_LINK& _emRefSkillTarget
											, SNATIVEID& _sRefSkillID
											, int& _nRefSkillLevel
											, float& _fRefSkillOccurRate ) const
{
	// 모든 아이템 링크 스킬이 나올 확률은 같다;
	int nIndex = sc::Random::getInstance().RandomNumber(
		0,
		m_vecItemLinkSkill.size()-1 );

	// 랜덤으로 얻어진 아이템 링크 스킬을 이용하여 구한다;
	const SLINK_SKILL_INFO& sSkillInfo = m_vecItemLinkSkill.at( nIndex );

	_emRefSkillTarget = sSkillInfo.GetTarget();
	_sRefSkillID = sSkillInfo.GetSkillID();
	int nSkillLevel = sc::Random::getInstance().RandomNumber(
		sSkillInfo.GetMinLevel(),
		sSkillInfo.GetMaxLevel() );
	_nRefSkillLevel = nSkillLevel;
	_fRefSkillOccurRate = sSkillInfo.GetOccurRate();
}

const int CItemLinkSkillSet::GetSizeLinkSkill ( void ) const
{
	return m_vecItemLinkSkill.size();
}



const bool GLItemLinkSkillMan::LoadData ( void )
{
	// Initialize Lua;
	sc::lua_init();

	std::string strFullPath( GLOGIC::GetPath() );
	strFullPath.append( GLogicData::GetInstance().GetItemLinkSkillFileName() );

	std::auto_ptr< sc::BaseStream > pStream(
		GLOGIC::openfile_basestream(
		GLOGIC::UseLogicZipFile(),
		GLogicData::GetInstance().GetGlogicZipFile(),
		strFullPath.c_str(), 
		GLogicData::GetInstance().GetItemLinkSkillFileName().c_str(), 
		false,
		GLOGIC::UseLogicPackFile() ) );

	if ( NULL == pStream.get() )		
	{
		sc::writeLogError(
			sc::string::format(
			"GLItemLinkSkillMan::LoadFile(), File Open %1%", strFullPath ) );

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
			sc::lua_getGlobalFromName( "ItemLinkSkill" );
		if ( sBasicGroup.IsNil() )
			return false;

		for ( LuaPlus::LuaTableIterator iter( sBasicGroup );
			iter; iter.Next() )
		{
			LuaPlus::LuaObject sTempMember( iter.GetValue() );

			CItemLinkSkillSet sItemLinkSkillSet;
			sItemLinkSkillSet.LoadData( sTempMember );

			m_vecItemLinkSkillSet.push_back( sItemLinkSkillSet );
		}
	}

	// Destroy Lua;
	sc::lua_destroyState();

	return true;
}


void GLItemLinkSkillMan::GetLinkSkill ( const int _nIndex
									  , EMITEM_SKILL_LINK& _emRefSkillTarget
									  , SNATIVEID& _sRefSkillID
									  , int& _nRefSkillLevel
									  , float& _fRefSkillOccurRate ) const
{
	if ( _nIndex < 0 || m_vecItemLinkSkillSet.size() <= _nIndex )
		return;

	const CItemLinkSkillSet& sItemLinkSkillSet = m_vecItemLinkSkillSet.at( _nIndex );
	sItemLinkSkillSet.GetLinkSkill_Random( _emRefSkillTarget, _sRefSkillID, _nRefSkillLevel, _fRefSkillOccurRate );
}

const int GLItemLinkSkillMan::GetSizeLinkSkillSet ( void ) const
{
	return m_vecItemLinkSkillSet.size();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
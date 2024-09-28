#include "pch.h"

#include "../../SigmaCore/Lua/MinLua.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Util/MinMessageBox.h"

#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../SigmaCore/gassert.h"

#include "./GLSkillReset.h"
#include "../GLUseFeatures.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLSkillResetDefine::GLSkillResetDefine()
{

}

GLSkillResetDefine::~GLSkillResetDefine()
{

}

bool GLSkillResetDefine::Load( const std::string& strFileName, bool bServer )
{
	m_strFilePath = strFileName;

	try
	{   
		sc::lua_init();
		if ( !sc::lua_doFile( m_strFilePath ) )
		{
			std::string strErrMsg( sc::string::format( "%1% file missing.(error 1378)", m_strFilePath ) );
			sc::ErrorMessageBox( strErrMsg, false );
			return false;
		}

		loadCommon();
		loadUI();

		if ( bServer )
		{
			loadServer();
		}

		sc::writeLogInfo( sc::string::format( "GLSkillResetDefine count %1%", m_SkillGradeMap.size() ) );

		return true;
	}
	catch ( const LuaPlus::LuaException& e )
	{
		std::string strErrMsg( sc::string::format( "%1%, %2%", m_strFilePath, e.GetErrorMessage() ) );
		sc::ErrorMessageBox( strErrMsg, false );
		return false;
	} 
}

DWORD GLSkillResetDefine::GetSkillGrade( SNATIVEID& SkillID )
{
	std::map< SNATIVEID, DWORD >::iterator iter = m_SkillGradeMap.find( SkillID );
	if ( iter != m_SkillGradeMap.end() )
	{
		return iter->second;
	}
	else
	{
		return SNATIVEID::ID_NULL;
	}
}

LONGLONG GLSkillResetDefine::GetLearnedSkillCost( SKILL_MAP& LearnedSkillMap )
{
	LONGLONG Cost = 0;

	SKILL_MAP_ITER iter = LearnedSkillMap.begin();
	SKILL_MAP_ITER iterEnd = LearnedSkillMap.end();
	for ( ; iter != iterEnd ; ++iter )
	{
		const SCHARSKILL& LearnedSkill = ( *iter ).second;
		
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( LearnedSkill.sNativeID );
		if ( !pSkill )
		{
			continue;
		}

		if ( LearnedSkill.wLevel >= SKILL::MAX_LEVEL )
		{
			continue;
		}
		
		for ( WORD loop = 0; loop <= LearnedSkill.wLevel; loop++ )
		{
			Cost += static_cast< LONGLONG >( pSkill->m_sLEARN.sLVL_STEP[loop].dwReqMoney );
		}
	}

	return ::_abs64( Cost );
}

bool GLSkillResetDefine::loadCommon()
{
	try
	{        
		LuaPlus::LuaObject common = sc::lua_getGlobalFromName( "common" );
		for ( LuaPlus::LuaTableIterator Iter( common ); Iter; Iter.Next() )
		{
			std::string strKey( ( const char* ) Iter.GetKey().GetString() );

			if ( strKey == "Skill" )
			{
				m_SkillGradeMap.clear();

				for ( LuaPlus::LuaTableIterator Iter2( Iter.GetValue() ); Iter2; Iter2.Next() )
				{
					for ( LuaPlus::LuaTableIterator Iter3( Iter2.GetValue() ); Iter3; Iter3.Next() )
					{
						SNATIVEID sSkillID;
						DWORD SkillGrade = SNATIVEID::ID_NULL;
						sSkillID.wMainID = static_cast< WORD >( Iter3.GetValue().GetInteger() ); Iter3.Next();
						sSkillID.wSubID  = static_cast< WORD >( Iter3.GetValue().GetInteger() ); Iter3.Next();
						SkillGrade = static_cast< DWORD >( Iter3.GetValue().GetInteger() );

						std::pair< std::map< SNATIVEID, DWORD >::iterator, bool > InsertResult = m_SkillGradeMap.insert( std::map< SNATIVEID, DWORD >::value_type( sSkillID, SkillGrade ) );
					}
				}
			}
		}
		return true;
	}
	catch ( const LuaPlus::LuaException& e )
	{
		std::string strErrMsg( sc::string::format( "%1%, %2%", m_strFilePath, e.GetErrorMessage() ) );
		sc::ErrorMessageBox( strErrMsg, false );
		return false;
	}
}

bool GLSkillResetDefine::loadServer()
{
	return true;
}

bool GLSkillResetDefine::loadUI()
{
	return true;
}


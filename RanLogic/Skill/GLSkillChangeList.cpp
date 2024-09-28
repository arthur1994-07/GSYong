#include "pch.h"

#include "../../SigmaCore/Lua/MinLua.h"

#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../EngineLib/EngineTypeDefine.h"
#include "../../EngineLib/Common/ByteStream.h"
#include "../../EngineLib/G-Logic/GLDefine.h"

#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/Container/GLList.h"
#include "../../SigmaCore/Memory/SmartPtr.h"

#include "../GLogicDataMan.h"

#include "GLSkillChangeList.h"

CSkillChangeList& CSkillChangeList::GetInstance()
{
	static CSkillChangeList Instance;

	return Instance;
}

CSkillChangeList::CSkillChangeList()
{

}

CSkillChangeList::~CSkillChangeList()
{
	
}

BOOL CSkillChangeList::Load_Data(std::string strFileName )
{
	std::string strPath(GLOGIC::GetPath());

	std::string strSkillChange = strFileName;

	if(strPath.empty()) return FALSE;

	strPath.append(strSkillChange);

	char szName[_MAX_FNAME] = _T("");
	char szExt[_MAX_EXT] = _T("");
	_splitpath(strPath.c_str(), NULL, NULL, szName, szExt);//경로를 통해서 이름과 확장자를 알아냄
	StringCchCat(szName, _MAX_FNAME, szExt); //이름뒤에 확장자 붙여줌

	std::auto_ptr< sc::BaseStream > pStream(
		GLOGIC::openfile_basestream(
		GLOGIC::UseLogicZipFile(),
		GLogicData::GetInstance().GetGlogicZipFile(),
		strPath.c_str(), 
		szName, 
		false,
		GLOGIC::UseLogicPackFile() ) );

	//파일 경로를 따라가서 파일을 열음(zip파일인지 아닌지로 구분해서, 초기화)
	if(!pStream.get())
	{
		sc::writeLogError(sc::string::format("CSkillChangeList::Load_Data, File Open %1%", strPath));
		return FALSE;
	}

	size_t nSize = static_cast<size_t> (pStream->GetFileSize());
	void* pBuffer = malloc(nSize);
	pStream->ReadBuffer(pBuffer, 0);	
	CTypeSmartPtr cPtrData(pBuffer); //smartptr로 데이터 담음

	try
	{
		sc::lua_init();
		if(!sc::lua_doBuffer(static_cast<char*>(pBuffer), nSize, NULL))
		{
			MessageBox(NULL, sc::lua_getError().c_str(), strPath.c_str(), MB_OK);
			return FALSE;
		}		
		return PaserData();
	}
	catch(const LuaPlus::LuaException& e)
	{
		MessageBox(NULL, e.GetErrorMessage(), strPath.c_str(), MB_OK);
		return FALSE;
	}

	return FALSE;
}

BOOL CSkillChangeList::PaserData()
{
	bool bLoadSuccess = false;

	try
	{
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName("CHANGESKILL_TABLE");

		int iIndex = 0;

		std::wstring ws;

		for( LuaPlus::LuaTableIterator it(ob); it.IsValid(); it.Next(), iIndex++) 
		{
			LuaPlus::LuaObject _object = it.GetValue();

			LuaPlus::LuaObject luaObj = _object.GetByName( "MID_SID_LIST" );

			int iSubIndex = 0;
				
			for( LuaPlus::LuaTableIterator it_sub(luaObj); it_sub.IsValid(); it_sub.Next() ) 
			{
				LuaPlus::LuaTableIterator it_data( it_sub.GetValue() );
				
				if( !it_data.IsValid() ) continue;

				WORD wMainID = static_cast<WORD>(it_data.GetValue().GetInteger()); it_data.Next(); 
				WORD wSubID  = static_cast<WORD>(it_data.GetValue().GetInteger()); 
				
				m_Data[iIndex].push_back(SCHARSKILL(SNATIVEID(wMainID, wSubID), 0));
			}
		}

		bLoadSuccess = true;
	}
	catch( const LuaPlus::LuaException& e)
	{
		MessageBox(NULL, e.GetErrorMessage(), "CHANGESKILL_TABLE", MB_OK);
	}

	
	return bLoadSuccess;
}

const bool CSkillChangeList::IsOriginalSkill ( const SNATIVEID& _sSkillID ) const
{
	map_citer mapIterData = m_Data.begin();
	for ( ; mapIterData != m_Data.end(); ++mapIterData )
	{
		v_SKCList vTempSkillList = mapIterData->second;

		v_iter vIterTempSkillList = vTempSkillList.begin();
		while ( vIterTempSkillList != vTempSkillList.end() )
		{
			// 원본스킬이 맞다면 true 를 반환한다;
			if ( vIterTempSkillList->sNativeID == _sSkillID )
			{
				return true;
			}

			++vIterTempSkillList;
		}
	}

	// 원본스킬이 아니라면 false 를 반환한다;
	return false;
}

BOOL CSkillChangeList::GetChangeList( UINT iTableIdx, v_SKCList& vList ) const
{
	map_citer _mIter = m_Data.find( iTableIdx );

	if( _mIter == m_Data.end() ) return FALSE;

	vList = _mIter->second;

	return TRUE;
}

BOOL CSkillChangeList::IsChangeTable( UINT iTableIdx )
{
	map_citer _mIter = m_Data.find( iTableIdx );

	if( _mIter == m_Data.end() ) return FALSE;

	return TRUE;
}

GLSKILL* CSkillChangeList::GetChangeSkill( UINT iTableIdx, GLSKILL* pSkill, SCHARSKILL& pCharSkill )
{
	if( m_Data.size() <= iTableIdx ) return NULL;

	v_iter _vIter = m_Data[iTableIdx].begin();

	while( _vIter != m_Data[iTableIdx].end() )
	{
		if( _vIter->sNativeID == pSkill->m_sBASIC.sNATIVEID )
		{
			pCharSkill = (*_vIter);

			return GLSkillMan::GetInstance().GetData( pSkill->m_sEXT_DATA.idDisguiseSkill );
		}
		++_vIter;
	}

	return pSkill;
}

BOOL CSkillChangeList::GetChangeSkill( UINT iTableIdx, DWORD& dwIndex )
{
	if( m_Data.size() <= iTableIdx ) return FALSE;

	v_iter _vIter = m_Data[iTableIdx].begin();

	while( _vIter != m_Data[iTableIdx].end() )
	{
		if( _vIter->sNativeID.dwID == dwIndex )
		{
			GLSKILL* pSkill = GLSkillMan::GetInstance().GetData( _vIter->sNativeID );

			if( pSkill ) {
				dwIndex = pSkill->m_sEXT_DATA.idDisguiseSkill.dwID;
				return TRUE;
			}
		}

		++_vIter;
	}

	return FALSE;
}

GLSKILL* CSkillChangeList::GetOrgChangeSkill( UINT iTableIdx, const DWORD& dwChangeSkillID )
{
	if( m_Data.size() <= iTableIdx ) return FALSE;

	v_iter _vIter = m_Data[iTableIdx].begin();

	while( _vIter != m_Data[iTableIdx].end() )
	{
		if( _vIter->sNativeID.IsValidNativeID() )
		{
			GLSKILL* pSkill = GLSkillMan::GetInstance().GetData( _vIter->sNativeID );

			if( pSkill && pSkill->m_sEXT_DATA.idDisguiseSkill.dwID == dwChangeSkillID ) {
				
				return pSkill;
			}
		}

		++_vIter;
	}

	return NULL;
}
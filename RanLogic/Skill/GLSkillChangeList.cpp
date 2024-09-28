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
	_splitpath(strPath.c_str(), NULL, NULL, szName, szExt);//��θ� ���ؼ� �̸��� Ȯ���ڸ� �˾Ƴ�
	StringCchCat(szName, _MAX_FNAME, szExt); //�̸��ڿ� Ȯ���� �ٿ���

	std::auto_ptr< sc::BaseStream > pStream(
		GLOGIC::openfile_basestream(
		GLOGIC::UseLogicZipFile(),
		GLogicData::GetInstance().GetGlogicZipFile(),
		strPath.c_str(), 
		szName, 
		false,
		GLOGIC::UseLogicPackFile() ) );

	//���� ��θ� ���󰡼� ������ ����(zip�������� �ƴ����� �����ؼ�, �ʱ�ȭ)
	if(!pStream.get())
	{
		sc::writeLogError(sc::string::format("CSkillChangeList::Load_Data, File Open %1%", strPath));
		return FALSE;
	}

	size_t nSize = static_cast<size_t> (pStream->GetFileSize());
	void* pBuffer = malloc(nSize);
	pStream->ReadBuffer(pBuffer, 0);	
	CTypeSmartPtr cPtrData(pBuffer); //smartptr�� ������ ����

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
			// ������ų�� �´ٸ� true �� ��ȯ�Ѵ�;
			if ( vIterTempSkillList->sNativeID == _sSkillID )
			{
				return true;
			}

			++vIterTempSkillList;
		}
	}

	// ������ų�� �ƴ϶�� false �� ��ȯ�Ѵ�;
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
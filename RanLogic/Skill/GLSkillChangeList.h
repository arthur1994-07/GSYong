#pragma once

#include "../Character/GLCharData.h"

namespace sc 
{
	class BaseStream;
	class SerialFile;
}

namespace se
{
	class ByteStream;
}

#define GSCL_INST CSkillChangeList::GetInstance()

class CSkillChangeList
{
public :
	
	struct SSkillChange
	{
		SNATIVEID	sChangeSkillID;

		SSkillChange() : sChangeSkillID( NATIVEID_NULL() ) {}
		SSkillChange(WORD wMain, WORD wSub) : sChangeSkillID( SNATIVEID(wMain, wSub)){}
	};

	// wLevel 값은 바꿀수 있다.
	typedef std::vector<SCHARSKILL>   v_SKCList;
	typedef v_SKCList::iterator		  v_iter;
	typedef v_SKCList::value_type	  v_value;

	typedef std::map<DWORD, v_SKCList> map_Data;
	typedef map_Data::iterator		   map_iter;
	typedef map_Data::const_iterator   map_citer;
	typedef map_Data::value_type	   map_value;

	friend class  GLCharacter;
	friend struct GLCHARLOGIC;

public :	
	~CSkillChangeList();
	
	static CSkillChangeList& GetInstance();

private:
	CSkillChangeList();

protected:
	map_Data	m_Data;

public :
	BOOL Load_Data(std::string strFileName );
	BOOL PaserData();

	const bool IsOriginalSkill ( const SNATIVEID& _sSkillID ) const;

	BOOL GetChangeList( UINT iTableIdx, v_SKCList& vList ) const;
	BOOL IsChangeTable( UINT iTableIdx );
	GLSKILL* GetOrgChangeSkill( UINT iTableIdx, const DWORD& dwChangeSkillID );

	GLSKILL* GetChangeSkill( UINT iTableIdx, GLSKILL* pSkill, SCHARSKILL& pCharSkill );
	BOOL	 GetChangeSkill( UINT iTableIdx, DWORD& dwIndex );
	
};
#pragma once

#include <map>

#include "../../EngineLib/G-Logic/TypeDefine.h"

#include "../../SigmaCore/MsgPack/MinMsgPack.h"
#include "../../SigmaCore/Util/GLSingletone.h"

#include "../Character/GLCharData.h"

class GLSkillResetDefine : public CSingletone< GLSkillResetDefine >
{
protected:
	friend CSingletone;

public:
	GLSkillResetDefine();
	~GLSkillResetDefine();

public:
	bool Load( const std::string& strFileName, bool bServer );

public:
	DWORD GetSkillGrade( SNATIVEID& SkillID );
	LONGLONG GetLearnedSkillCost( SKILL_MAP& LearnedSkillMap );

protected:
	bool loadCommon();
	bool loadServer();
	bool loadUI();

protected:
	std::string m_strFilePath;
	std::map< SNATIVEID, DWORD > m_SkillGradeMap;

};

#pragma once

#include "../../RanLogic/LandEvent/GLLandEventNode.h"

//-------------------------------------------------------------------//

class CEVENT_NODE_CLIENT : public CEVENT_NODE
{

public :
	CEVENT_NODE_CLIENT()
	{
	}
	
	virtual ~CEVENT_NODE_CLIENT()
	{
	}

public :
    virtual EMEVENTNODETYPE GetType() const override = 0;

};

//-------------------------------------------------------------------//

class CEVENT_NODE_CLIENT_SUMMONGATE : public CEVENT_NODE_CLIENT
{

public :

	SNATIVEID   m_sSkillID;      // 해당 스킬 정보
	WORD        m_wLevel;        // 해당 스킬 레벨

	DWORD       m_dwGUIDOwner;   // 시전자의 GUID

	STARGETID   m_sEvent;        // 이벤트 정보
	STARGETID   m_sTarget;       // 타겟 정보

	std::string	m_strEffectName; // 해당 Effect의 이름 정보

public :
	CEVENT_NODE_CLIENT_SUMMONGATE () : 
	   m_wLevel(0), m_dwGUIDOwner(UINT_MAX), m_strEffectName("")
	   {
	   }

public :
	virtual void Restore();
	virtual void Clean();

public :
	virtual void FrameStart();
	virtual void FrameEvent();
    virtual void FrameEnd();

public :
    virtual EMEVENTNODETYPE GetType() const override { return EMEVENTNODETYPE_SUMMONGATE; }

};

//-------------------------------------------------------------------//
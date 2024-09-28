
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

	SNATIVEID   m_sSkillID;      // �ش� ��ų ����
	WORD        m_wLevel;        // �ش� ��ų ����

	DWORD       m_dwGUIDOwner;   // �������� GUID

	STARGETID   m_sEvent;        // �̺�Ʈ ����
	STARGETID   m_sTarget;       // Ÿ�� ����

	std::string	m_strEffectName; // �ش� Effect�� �̸� ����

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
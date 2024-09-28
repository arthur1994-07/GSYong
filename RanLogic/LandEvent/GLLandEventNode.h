
#pragma once

#include "../../EngineLib/G-Logic/GLDefine.h"

#include "../Actor/GLActorDefine.h"

//#include "../EngineLib/G-Logic/GLList.h"
//#include "../EngineLib/G-Logic/GLQuadTree.h"

//-------------------------------------------------------------------//

enum EMEVENTNODETYPE
{
    EMEVENTNODETYPE_NONE,
    EMEVENTNODETYPE_SUMMONGATE,
};

//-------------------------------------------------------------------//

class GLLandEvent;

//-------------------------------------------------------------------//

class CEVENT_NODE
{
public :
	FLOAT       m_fTimeEvent;    // �̺�Ʈ �ߵ� �ð�
	FLOAT       m_fTimeCurrent;  // �̺�Ʈ ���� �ð�
	FLOAT       m_fTimeEnd;      // �̺�Ʈ ���� �ð�

	BOOL        m_bInit;         // �ʱ�ȭ ����
	BOOL        m_bEvent;        // �̺�Ʈ �ߵ� ����

	DWORD       m_dwGlobID;
	Faction::GLActorID m_sOwnerID;
	SNATIVEID   m_sMapID;
	D3DXVECTOR3 m_vPosition;     // ��ġ����

public :
	CEVENT_NODE () :
        m_fTimeEvent(0.0f), 
        m_fTimeCurrent(0.0f), 
        m_fTimeEnd(0.0f), 
        m_bInit(FALSE), 
        m_bEvent(FALSE), 
        m_dwGlobID(UINT_MAX),
        m_vPosition(0,0,0)
	   {
	   }

	   virtual ~CEVENT_NODE()
	   {
		   Clean();
	   }

public :
	virtual void Restore() = 0;

public :
	virtual void Clean()        {}

public :
	virtual void FrameProcess() {}
	virtual void FrameStart()   {}
	virtual void FrameEvent()   {}
	virtual void FrameEnd()     {}

public :
	void         FrameMove( const FLOAT fElapsedTime );

public :
	const BOOL   IsEnd() { return !(!(m_fTimeCurrent >= m_fTimeEnd)); }

public :
    virtual EMEVENTNODETYPE GetType() const = 0;

};


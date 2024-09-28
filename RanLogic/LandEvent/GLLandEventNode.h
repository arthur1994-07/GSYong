
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
	FLOAT       m_fTimeEvent;    // 이벤트 발동 시간
	FLOAT       m_fTimeCurrent;  // 이벤트 지난 시간
	FLOAT       m_fTimeEnd;      // 이벤트 종료 시간

	BOOL        m_bInit;         // 초기화 여부
	BOOL        m_bEvent;        // 이벤트 발동 여부

	DWORD       m_dwGlobID;
	Faction::GLActorID m_sOwnerID;
	SNATIVEID   m_sMapID;
	D3DXVECTOR3 m_vPosition;     // 위치정보

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


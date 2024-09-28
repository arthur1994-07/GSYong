#ifndef _GL_AGLAND_MAN_H_
#define _GL_AGLAND_MAN_H_

#pragma once

#include <set>

#include "../Level/GLLevelFileServer.h"
#include "../../EngineLib/DxLand/DxLandGateMan.h"

class GLAgentServer;
namespace InstanceSystem
{
	class InstanceAgent;
}

class GLAGLandMan : public GLLevelFileServer
{
public:
    GLAGLandMan(GLAgentServer* pServer);
    virtual ~GLAGLandMan(void);

private:
    GLAgentServer* m_pServer;

public:
	typedef std::set<DWORD>       SETPC;
	typedef SETPC::iterator       SETPC_ITER;
    typedef SETPC::const_iterator SETPC_CITER;

protected:
	SETPC		m_setPC;
	DWORD		m_dwClubMapID;				//! 선도 및 CDM ID
	bool		m_bGuidBattleMap;			//! 선도 클럽 전투 존
	bool		m_bClubDeathMatchMap;		//! 클럽데스매치 맵

protected:
    BOOL LoadWldFile ( const char* szWldFile, bool bLandGateLoad );

public:
	bool DropPC(DWORD dwID);
	bool DropOutPC(DWORD dwID);
	void SENDTOALLCLIENT(NET_MSG_GENERIC* nmg);
    BOOL LoadFile(const char* szFile, MapAxisInfoMan* pAxisMan);
    void CleanUp();

    //! 선도 클럽 전투 존
    inline bool IsClubBattleMap() const { return m_bGuidBattleMap; }
    inline void SetClubBattleMap(bool Set) { m_bGuidBattleMap = Set; }

    //! 클럽데스매치 맵
    inline bool IsClubDeathMatchMap() const { return m_bClubDeathMatchMap; }
    inline void SetClubDeathMatchMap(bool Set) { m_bClubDeathMatchMap = Set; }

    inline size_t CharCount() const { return m_setPC.size(); }

    //! 선도 및 CDM ID
    inline void SetGuidance(DWORD MapID) { m_dwClubMapID = MapID; }
    inline DWORD GetGuidance() const { return m_dwClubMapID; }
    bool CheckCDMMembersLimited( DWORD ClubDbNum );
	
    const SETPC& GetChar() const { return m_setPC; }
    //SETPC& GetChar()             { return m_setPC; }
};

class GLAGLandManager
{
public:
    const bool loadLevelFile(const SNATIVEID& baseMapID, const char* szFile, MapAxisInfoMan* pAxisMan);		
    const DWORD getInstanceMapID(void) const;
	const DWORD getFieldChannel(void) const;
	const DWORD getFieldID(void) const;
    const SNATIVEID getBaseMapID(const unsigned int _indexStage = 0) const;
    BOOL GetUseLevelEtcFunc(const EMETCFUNC emEtcFunc);
    GLAGLandMan* const getLand(const DWORD dwSubMapID = 0) const;

    InstanceSystem::InstanceAgent* const getScriptInstance(void);
	void setScriptInstance(InstanceSystem::InstanceAgent* pScriptInstance);
    void setMapID(const SNATIVEID& baseMapID, const SNATIVEID& gaeaMapID, const bool bPeaceZone, const bool bPKZone);
	void setInstantMapData(const DWORD dwHostID, const GLPartyID& PartyID);

public:	
    GLAGLandManager(GLAgentServer* pAgentServer, const DWORD dwInstanceMapID);
    GLAGLandManager(const GLAGLandManager& _rhs, const DWORD dwInstanceMapID, const DWORD dwFieldChannel, const DWORD dwFieldID);
    GLAGLandManager(const GLAGLandManager& _rhs);
    ~GLAGLandManager(void);

private:
    GLAgentServer* m_pAgentServer;
	InstanceSystem::InstanceAgent* m_pScriptInstance; // 신규 인던 스크립트;
    GLAGLandMan* m_pLand[MAX_LAND_SID];
    DWORD m_dwInstanceMapID;
	DWORD m_dwFieldChannel;
	DWORD m_dwFieldID;
};

#endif // _GL_AGLAND_MAN_H_
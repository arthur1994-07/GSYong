#ifndef _GL_LEVEL_FILE_SERVER_H_
#define _GL_LEVEL_FILE_SERVER_H_

#pragma once

#include <string>
#include "../SigmaCore/String/StringUtils.h"
#include "../SigmaCore/File/StringFile.h"
#include "../SigmaCore/File/BaseStream.h"
#include "../SigmaCore/File/SerialFile.h"

#include "../EngineLib/DxTools/DxCustomTypes.h"
#include "../EngineLib/DxLand/DxLandDef.h"
#include "../EngineLib/DxLand/DxLandGateMan.h"
#include "../EngineLib/G-Logic/GLDefine.h"
#include "../EngineLib/G-Logic/GLQuadTree.h"

#include "../RanLogic/Character/GLCharDefine.h"
#include "../RanLogic/Land/GLMapAxisInfo.h"
#include "../RanLogic/GLLevelFile.h"
#include "../RanLogic/GLogicEx.h"

#include "../Crow/GLMobScheduleManServer.h"

#include "GLLevelFileServer.h"

class MapAxisInfoMan;

class GLLevelFileServer : public GLLevelFile
{
public:
    GLLevelFileServer();
    virtual ~GLLevelFileServer();

	void SetObjRotate90();

	/* 
		기존에는 몹 스케쥴에 레이어 개념이 없었는데
		던전이 들어가면서 하나의 맵을 재활용하면서 레이어 개념 추가

	 */
	void AddMobSchMan(UINT index);
	void SetMobSchMan(UINT index);
    UINT GetSizeMobSchMan();
	void DelMobSchMan(UINT index);
	void ResetMobSchMan(GLLandMan* pLandMan);
	void ClearMobSchMan();

	typedef std::tr1::shared_ptr<GLMobScheduleManServer> SpMobScheduleManServer;
	typedef std::vector<SpMobScheduleManServer>	MobSchManContainer;
	typedef MobSchManContainer::iterator		MobSchManContainerIterator;
	typedef MobSchManContainer::const_iterator	MobSchManContainerConstIterator;

	void operator=(const GLLevelFileServer &rhs);

protected:
	MobSchManContainer m_MobSchManContainer;

private:
	SpMobScheduleManServer m_MobSchMan;

public:
	SpMobScheduleManServer GetMobSchMan() { return m_MobSchMan; }
    SpMobScheduleManServer GetMobSchMan( UINT nLayer );
	UINT GetMobSchManCount() { return m_MobSchManContainer.size(); }

public:
	BOOL LOAD_000 ( sc::BaseStream &SFile );
	BOOL LOAD_101 ( sc::BaseStream &SFile );
	BOOL LOAD_102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bExceptEffect );
	BOOL LOAD_103 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bExceptEffect );
	BOOL LOAD_104 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bExceptEffect );
	BOOL LOAD_105 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bExceptEffect );	//	데이터 포맷 변경 진행
	BOOL LOAD_106 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bExceptEffect );
	BOOL LOAD_107 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bExceptEffect );
	BOOL LOAD_108 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bExceptEffect );
	BOOL LOAD_109 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bExceptEffect );	// Collision

public:
	BOOL SaveFile ( const char *szFile );

    // GLLevelFileClient 와 동시에 진행되야 합니다.
	// bExceptBaseEffect - bImport 가 true 일 때만 동작 됨. 
	//						bImport 가 false 일 때는 내부에서 강제로 FALSE 로 변경시킴.
    BOOL LoadFile(const std::string& FileName, LPDIRECT3DDEVICEQ pd3dDevice, MapAxisInfoMan* pAxisMan, const bool bImport = false, BOOL bExceptBaseEffect=FALSE);

public:
    BOOL SaveCsvFile ( std::fstream &SFile, bool bAllSave );
    BOOL LoadCsvFile ( const char *szFile );
    BOOL LoadAllCsvFile(LPDIRECT3DDEVICEQ pd3dDevice, MapAxisInfoMan* pAxisMan);

public:
	void ResetData();

protected:
	BOOL CheckMobSchMan(const MOBSCHEDULELIST* pMobScheduleList);

};

#endif // _GL_LEVEL_FILE_SERVER_H_

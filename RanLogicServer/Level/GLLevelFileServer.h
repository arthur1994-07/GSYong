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
		�������� �� �����쿡 ���̾� ������ �����µ�
		������ ���鼭 �ϳ��� ���� ��Ȱ���ϸ鼭 ���̾� ���� �߰�

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
	BOOL LOAD_105 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bExceptEffect );	//	������ ���� ���� ����
	BOOL LOAD_106 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bExceptEffect );
	BOOL LOAD_107 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bExceptEffect );
	BOOL LOAD_108 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bExceptEffect );
	BOOL LOAD_109 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice, BOOL bExceptEffect );	// Collision

public:
	BOOL SaveFile ( const char *szFile );

    // GLLevelFileClient �� ���ÿ� ����Ǿ� �մϴ�.
	// bExceptBaseEffect - bImport �� true �� ���� ���� ��. 
	//						bImport �� false �� ���� ���ο��� ������ FALSE �� �����Ŵ.
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

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
#include "../RanLogic/Crow/GLMobScheduleMan.h"
#include "../RanLogic/Land/GLMapAxisInfo.h"
#include "../RanLogic/GLLevelFile.h"
#include "../RanLogic/GLogicEx.h"

#include "GLLevelFileClient.h"

class GLGaeaClient;

class GLLevelFileClient : public GLLevelFile
{
protected:
    GLGaeaClient* m_pGaeaClient;
	typedef std::tr1::shared_ptr<GLMobScheduleMan> SpMobScheduleMan;
	typedef std::vector<SpMobScheduleMan>		MobSchManContainer;
	typedef MobSchManContainer::iterator	MobSchManContainerIterator;

	MobSchManContainer m_MobSchManContainer;
	SpMobScheduleMan m_MobSchMan;

public:
	const GLMobScheduleMan* GetMobSchMan () const { return m_MobSchMan.get(); }
	void SetMobSchMan(UINT index);

public:
	BOOL LOAD_000 ( sc::BaseStream &SFile );
	BOOL LOAD_101 ( sc::BaseStream &SFile );
	BOOL LOAD_102 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL LOAD_103 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL LOAD_104 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL LOAD_105 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );	//	데이터 포맷 변경 진행
	BOOL LOAD_106 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL LOAD_107 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL LOAD_108 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );
	BOOL LOAD_109 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice );

public:
    // GLLevelFileServer 와 동시에 진행되야 합니다.
    BOOL LoadFile(const std::string& FileName, LPDIRECT3DDEVICEQ pd3dDevice);


public:
	GLLevelFileClient(GLGaeaClient* pGaeaClient);
	virtual ~GLLevelFileClient();
};
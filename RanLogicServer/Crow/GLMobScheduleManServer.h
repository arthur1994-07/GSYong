#pragma once

#include <hash_map>
#include <hash_set>
#include <set>

#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/Container/GLList.h"

#include "../../EngineLib/DxTools/DxMethods.h"
#include "../../EngineLib/G-Logic/GLDefine.h"
#include "../../EngineLib/NaviMesh/NavigationMesh.h"

#include "../../RanLogic/Crow/GLMobScheduleMan.h"
#include "../../RanLogic/Crow/GLMobSchedule.h"

namespace sc {
    class SerialFile;
}

class GLLandMan;
class GLMobScheduleManServer : public GLMobScheduleMan
{
	//	ÈÖ¹ß¼º ÀÚ·á.
protected:
	GLLandMan*				m_pLandMan;

public:
	HRESULT FrameMove ( float fTime, float fElapsedTime, bool bFirstCreateForInstMap = FALSE );
	HRESULT InitMobScheduleState();

	const DWORD DropCrow ( GLMobSchedule* pMobSchedule, float fElapsedTime = 0.1f, bool bFirstCreateForInstMap = false );

public:
	void SetLandMan ( GLLandMan* pLandMan );
	GLLandMan* GetLandMan() { return m_pLandMan; }

	void SetLayerIndex ( const DWORD dwLayerIndex ) { m_dwLayerIndex = dwLayerIndex; }
	const DWORD GetLayerIndex () { return m_dwLayerIndex; }

	void CleanUp ();

public:
	virtual const GLMobScheduleManServer& operator =(const GLMobScheduleManServer& _rhs);
	GLMobScheduleManServer();
	virtual ~GLMobScheduleManServer();

public:
    DWORD GetAliveCount();

private:
    DWORD m_dwMobMaxCount;
    DWORD m_dwMobAliveCount;

	DWORD m_dwLayerIndex;
};

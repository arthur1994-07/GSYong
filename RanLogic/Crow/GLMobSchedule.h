#ifndef GLMOBSCHEDULE_H_
#define GLMOBSCHEDULE_H_

#pragma once

#include <boost/tr1/unordered_map.hpp>
#include <hash_set>
#include <set>

#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/Container/GLList.h"

#include "../../EngineLib/DxTools/DxMethods.h"
#include "../../EngineLib/G-Logic/GLDefine.h"
#include "../../EngineLib/NaviMesh/NavigationMesh.h"

namespace sc {
    class SerialFile;
}

enum EMMOBACTIONS
{
	EMACTION_MOVE		= 0,
	EMACTION_ACTSTAY	= 1,

	EMACTION_SIZE		= 2,
};

struct SMOBACTION
{
	const static DWORD	VERSION;

	EMMOBACTIONS	emAction;

	const bool operator ==(const SMOBACTION&) { return false; }
	union
	{
		struct { D3DXVECTOR3 vPos; };
		struct { float fLife; };
	};

	SMOBACTION () :
		emAction(EMACTION_MOVE),
		vPos(0,0,0)
	{
	}
};
typedef sc::CGLLIST<SMOBACTION>	MOBACTIONLIST;
typedef sc::SGLNODE<SMOBACTION>	MOBACTIONNODE;


class GLMobMan;

class GLMobSchedule : public DXAFFINEMATRIX
{
public:
	enum EMFLAGS
	{
		EM_EDIT			= 1,
		EM_MAXSZNAME	= 65,
		EM_MAXRENDPOS	= 12,
		
		EM_MINDISTFACTOR	= 10,
		EM_MAXDISTFACTOR	= 100,
		
		EM_RENDDISTSTART	= 30,
		EM_MAXRENDDISTRANGE	= 14,
	};

	enum EMVOLATFLAGS
	{
		EM_ONCE		= 0x0001,
		EM_EXMODE	= 0x0002,
		//0x0004;
		//0x0008;
	};

	enum EMMOBFLAGS
	{
		EMMF_DISPLAY_MAP_POINT	= 0x001,		// 미니맵에 표시할 것인가 옵션
	};

	static const D3DXVECTOR3	m_vHIGHER;
	static const float			m_fSPHERE_S1;
	static const float			m_fSPHERE_S2;
	static const float			m_fSPHERE_S3;

	typedef std::vector<D3DXVECTOR3>	VEC3ARRAY;
	typedef VEC3ARRAY::iterator			VEC3ARRAY_ITER;


	// Save/Load 필요 자료.
public:
	enum { VERSION = 0x0113, };
	
	static BOOL	m_bRENDACT;

	SNATIVEID			m_CrowID;
	char				m_szName[EM_MAXSZNAME];

	bool				m_bDayOfWeek[7];
	
	float				m_fReGenTimeMin;
	float				m_fReGenTimeMax;
	int					m_nRegenHour;
	int					m_nRegenMin;

	MOBACTIONLIST		m_sMobActList;

	BOOL				m_bEachregen;
	BOOL				m_bGroupRegen;
	BOOL				m_bLeaderMonster;
	std::string			m_strGroupName;
	
	BOOL				m_bRendGenPos;
	WORD				m_wRendGenPosNum;
	WORD				m_wRendGenPosDist;
	VEC3ARRAY			m_vectorRendGenPos;	

	DWORD				m_dwPC_REGEN_GATEID;
	std::string			m_strBUSLOCATION;

	DWORD				m_dwFlags;				// EMMOBFLAGS
    float               m_fScale;

	float				m_fActivityArea;		// 행동 반경(활동 영역)
	float				m_fInfluenceY;

public:
	void SetObjRotate90();
	void ModifierSchList( D3DXVECTOR3* pOffset, NavigationMesh* pNaviMesh );

	const bool GetUseRegenTimer ( );
	const bool GetUseRegenTimeStamp ( );
	const bool IsRegen ( );

	void SetGenTime();

	void SetRegenTimer ( float fRegenTimeMin, float fRegenTimeMax );
	void AddPosition ( const D3DXVECTOR3& sPopsition );
	void SetRotation ( float fRotate );
	void SetCrowID ( const SNATIVEID& sCrowID );
	void SetScale ( float fScale );

	BOOL IsDisplayMapPoint() const	{ return m_dwFlags&EMMF_DISPLAY_MAP_POINT; }	// 미니맵에 표시할 것인가 옵션

public:
	BOOL Load ( sc::BaseStream &SFile );
	BOOL Save ( sc::SerialFile &SFile );

	BOOL Load_0100( sc::BaseStream &SFile );
	BOOL Load_0101( sc::BaseStream &SFile );
	BOOL Load_0102( sc::BaseStream &SFile );
	BOOL Load_0103( sc::BaseStream &SFile );
	BOOL Load_0104( sc::BaseStream &SFile );
	BOOL Load_0105( sc::BaseStream &SFile );
	BOOL Load_0106( sc::BaseStream &SFile );
	BOOL Load_0107( sc::BaseStream &SFile );	// 데이터 포맷 변경 진행
	BOOL Load_0108( sc::BaseStream &SFile );	// EMMF_DISPLAY_MAP_POINT 추가
	BOOL Load_0109( sc::BaseStream &SFile );	// 개별 리젠 m_bEachregen 추가
    BOOL Load_0110( sc::BaseStream &SFile );
	BOOL Load_0111( sc::BaseStream &SFile );	// 랜덤 생성 시간 추가;
	BOOL Load_0112( sc::BaseStream &SFile );	// 행동 반경(활동 영역)
	BOOL Load_0113( sc::BaseStream &SFile );

public:
	void SetExMode() { m_dwVolatFlags |= EM_EXMODE; }
	BOOL GenerateRendGenPos ( NavigationMesh* pNaviMesh );
	BOOL IsCollision ( D3DXVECTOR3 vPoint1, D3DXVECTOR3 vPoint2 );

	HRESULT RenderGenPos ( LPDIRECT3DDEVICEQ pd3dDevice );
	HRESULT Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv );

protected:
	void CalculateBox ();

	// 휘발성 자료.
public:
	DWORD				m_dwVolatFlags;
	float				m_fTimer;

	BOOL				m_bALive;
	BOOL				m_bGroub;
	DWORD				m_dwGlobID; // Note : 몹소환 ID추가 by 경대

	DWORD				m_dwRendGenIndex;

	D3DXVECTOR3			m_vMax;
	D3DXVECTOR3			m_vMin;

	GLMobSchedule*		m_pNext;

	CTime				m_RegenTime;

public:
	GLMobSchedule(void);
	~GLMobSchedule(void);

public:
	void Reset ()
	{
		m_fTimer = 0.0f;
		m_bALive = FALSE;
		m_dwGlobID = UINT_MAX;		
		m_RegenTime	= 0;
	}

	void ResetTimer();

	void operator= ( const GLMobSchedule &MobSch );

	D3DXVECTOR3 GetRendGenPos ()
	{
		D3DXVECTOR3 vGenPos;
		if ( m_vectorRendGenPos.empty() )	vGenPos = m_pAffineParts->vTrans;
		else
		{
			vGenPos = m_vectorRendGenPos[m_dwRendGenIndex++];
			if ( m_dwRendGenIndex >= m_vectorRendGenPos.size() )	m_dwRendGenIndex = 0;
		}

		return vGenPos;
	}
};

typedef sc::CGLLIST<GLMobSchedule*>		MOBSCHEDULELIST;
typedef sc::SGLNODE<GLMobSchedule*>		MOBSCHEDULENODE;

typedef std::vector<GLMobSchedule*>	VECSCHEDULE;
typedef VECSCHEDULE::iterator		VECSCHEDULE_ITER;


struct GLMOBGROUB
{
	float			m_fTimer;
	DWORD			m_dwRendGenIndex;

	VECSCHEDULE		m_vecSchedule;

	GLMOBGROUB () :
		m_fTimer(0),
		m_dwRendGenIndex(UINT_MAX)
	{
	}

	GLMOBGROUB ( const GLMOBGROUB& value )
	{
		operator= ( value );
	}

	GLMOBGROUB& operator= ( const GLMOBGROUB& value );
	void ADD ( GLMobSchedule* pSch );
	void DEL ( GLMobSchedule* pSch );
};

typedef std::map<DWORD, GLMOBGROUB> MAPMOBGROUB;
typedef MAPMOBGROUB::iterator		MAPMOBGROUB_ITER;

namespace COMMENT
{
	extern std::string MOBACTIONS[EMACTION_SIZE];
};

#endif // GLMOBSCHEDULE_H_

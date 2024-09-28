#pragma once

#include <map>
#include "../../RanLogic/Skill/GLSkill.h"

class ServerActor;
class GLGaeaServer;
struct D3DXVECTOR3;
struct SKILLTARGETDATA;
struct GLSKILL;
struct STARGETID;
struct SNATIVEID;

typedef std::vector<STARDIST> TARDISTMAP;
typedef TARDISTMAP::iterator  TARDISTMAP_ITER;

typedef std::vector<STARGETID>						VEC_SK_TAR;
typedef VEC_SK_TAR::iterator							VEC_SK_ITER;
typedef std::pair<VEC_SK_ITER,VEC_SK_ITER>	VEC_SK_ITER_RANGE;

class SkillTargetSystem
{
public:
	SkillTargetSystem();
	virtual ~SkillTargetSystem();

private:
	BOOL SelectCircle( ServerActor* pActorCaster, 
						  ServerActor* pActorTarget,
						  const GLSKILL& sSKILL,
						  const WORD wLEVEL, 
						  const D3DXVECTOR3&	vPos,
						  STARID *sTARIDS, 
						  WORD& wTargetNum  );

	BOOL SelectLine( ServerActor* pActorCaster, 
		ServerActor* pActorTarget,
		const GLSKILL& sSKILL,
		const WORD wLEVEL, 
		const D3DXVECTOR3&	vPos,
		STARID *sTARIDS, 
		WORD& wTargetNum  );

	void SelectFromNearRange( IN TARDISTMAP& vecTAR_TEMP, 
		OUT WORD &nTarget, 
		IN const WORD wTARNUM, 
		IN const WORD wMaxTarget, 
		OUT STARID *sTARIDS  );

	void EraseVector( TARDISTMAP &vecTar );
	void AddTarget(  ServerActor* pActor, STARID* sTARIDS, WORD& wTARNUM );
	void AddTarget( const STARGETID& sTargetID, STARID* sTARIDS, WORD& wTARNUM  );
	VEC_SK_TAR* DetectTargetCircle( const GLSKILL& sSKILL, 
								   ServerActor* pActorCaster, 
								   ServerActor* pActorTarget, 
								   int nApplyRange,
								   D3DXVECTOR3 &vTarOrgDir,
								   D3DXVECTOR3 &vTarPosition );
	void SelectTarget();
	//DETECTMAP_RANGE Equal_Range( DETECTMAP *pDetectMap, EMCROW emCrow );

	TARDISTMAP	m_mapTarget;
	VEC_SK_TAR m_vecDetectID;

	BOOL IsAvailableTarget( ServerActor* pActorCaster, STARGETID &sTarget, EMIMPACT_SIDE emTargetSide );

public:
	BOOL SelectSkillTarget( ServerActor* pActorCaster, // 주체
		ServerActor* pActorTarget, // 대상체
		const SNATIVEID& sSkillID, 
		const WORD wSkillLevel, 
		const D3DXVECTOR3& vPos, 
		STARID* sTARIDS, 
		WORD& wTargetNum  );

};
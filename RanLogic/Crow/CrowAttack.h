#ifndef _CROW_ATTACK_H_
#define _CROW_ATTACK_H_

#pragma once

#include <string>
#include "../../EngineLib/DxMeshs/SkinMesh/SAnimationDefine.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinAniControl.h"
#include "../../RanLogic/Character/GLCharDefine.h"

//---------------------------------------------------------------------- OLD VER
struct SCROWATTACK_100
{
    enum { VERSION = 0x0100, };

    BOOL			bUsed;						//	사용가능.
    EMATT_RGTYPE	emAttRgType;				//	공격가능거리 유형.
    WORD			wRange;						//	공격가능 거리.
    WORD			wSphere;					//	공격영향 범위.

    //	Skill 의 영향 받는 목표.
    EMIMPACT_TARGET	emImpact_Tar;				//	타겟. ( 자신, 대상, 위치 ) &&
    EMIMPACT_REALM	emImpact_Realm;				//	영역. ( 타겟, 타겟과 그 주위, 그 주위 ) &&
    EMIMPACT_SIDE	emImpact_Side;				//	"편" ( 그 주위의 자기편, 그 주위의 상대편 )

    EMELEMENT		emElement;					//	공격 속성.
    GLPADATA		sDamage;					//	타격값.
    float			fDelay;						//	딜래이.
    float			fLife;						//	지속 시간.

    WORD			wUse_MP;					//	사용시 MP 소진량.
    WORD			wUse_SP;					//	사용시 SP 소진량.

    EMSTATE_BLOW	emBlow;						//	상태이상.
    float			fBlowRATE;					//	발생 확율.
    float			fBlowVALUE;					//	적용 값.

    EMIMPACT_ADDON	emImpact;					//	부가효과 종류.
    int				nImpactVAR;					//	변화 수치.

    char			szAniFile[ACF_SZNAME];		//	에니메이션 파일 이름.
    SANIATTACK		sAniAttack;					//	에니메이션 스텝.

    char			szSelfBodyEffect[ACF_SZNAME];	//	자기 자신의 몸에 붓는 이팩트.
    char			szTargBodyEffect[ACF_SZNAME];	//	목표 유닛의 몸에 붓는 이팩트.
    char			szTargetEffect[ACF_SZNAME];		//	목표 지향 이팩트.
};

//----------------------------------------------------------------------
struct SCROWATTACK_101
{
    BOOL			bUsed;						//	사용가능.
    EMATT_RGTYPE	emAttRgType;				//	공격가능거리 유형.
    WORD			wRange;						//	공격가능 거리.

    GLPADATA		sDamage;					//	타격값.
    float			fDelay;						//	딜래이.

    WORD			wUse_SP;					//	사용시 SP 소진량.

    char			szAniFile[ACF_SZNAME];		//	에니메이션 파일 이름.
    SANIATTACK		sAniAttack;					//	에니메이션 스텝.

    SNATIVEID		skill_id;					//	설정된 스킬 종류.

    char			szSelfBodyEffect[ACF_SZNAME];	//	자기 자신의 몸에 붓는 이팩트.
    char			szTargBodyEffect[ACF_SZNAME];	//	목표 유닛의 몸에 붓는 이팩트.
    char			szTargetEffect[ACF_SZNAME];		//	목표 지향 이팩트.
};

struct SCROWATTACK_102
{
    BOOL			bUsed;						//	사용가능.
    EMATT_RGTYPE	emAttRgType;				//	공격가능거리 유형.
    WORD			wRange;						//	공격가능 거리.

    GLPADATA		sDamage;					//	타격값.
    float			fDelay;						//	딜래이.

    WORD			wUse_SP;					//	사용시 SP 소진량.

    char			szAniFile[ACF_SZNAME];		//	에니메이션 파일 이름.
    SANIATTACK		sAniAttack;					//	에니메이션 스텝.

    SNATIVEID		skill_id;					//	설정된 스킬 종류.

    EMSTATE_BLOW	emBLOW_TYPE;				//	'상태이상' 적용 타입.
    float			fBLOW_RATE;					//	발생 확율.
    float			fBLOW_LIFE;					//	지속 시간.
    float			fBLOW_VAR1;					//	적용 값 1.
    float			fBLOW_VAR2;					//	적용 값 2.

    char			szSelfBodyEffect[ACF_SZNAME];	//	자기 자신의 몸에 붓는 이팩트.
    char			szTargBodyEffect[ACF_SZNAME];	//	목표 유닛의 몸에 붓는 이팩트.
    char			szTargetEffect[ACF_SZNAME];		//	목표 지향 이팩트.
};

enum EMCROWSKTAR
{
    CROWSKTAR_OUR	= 0,
    CROWSKTAR_ENEMY	= 1,
};

struct SCROWATTACK_103
{
    BOOL			bUsed;						//	사용가능.
    EMATT_RGTYPE	emAttRgType;				//	공격가능거리 유형.
    WORD			wRange;						//	공격가능 거리.

    GLPADATA		sDamage;					//	타격값.
    float			fDelay;						//	딜래이.

    WORD			wUse_SP;					//	사용시 SP 소진량.

    char			szAniFile[ACF_SZNAME];		//	에니메이션 파일 이름.
    SANIATTACK_100	sAniAttack;					//	에니메이션 스텝.

    SNATIVEID		skill_id;					//	스킬 종류.
    WORD			skill_lev;					//	스킬 래벨.
    EMCROWSKTAR		skill_tar;					//	스킬 타겟.

    EMSTATE_BLOW	emBLOW_TYPE;				//	'상태이상' 적용 타입.
    float			fBLOW_RATE;					//	발생 확율.
    float			fBLOW_LIFE;					//	지속 시간.
    float			fBLOW_VAR1;					//	적용 값 1.
    float			fBLOW_VAR2;					//	적용 값 2.

    char			szSelfBodyEffect[ACF_SZNAME];	//	자기 자신의 몸에 붓는 이팩트.
    char			szTargBodyEffect[ACF_SZNAME];	//	목표 유닛의 몸에 붓는 이팩트.
    char			szTargetEffect[ACF_SZNAME];		//	목표 지향 이팩트.
};

struct SCROWATTACK_104
{
    BOOL			bUsed;						//	사용가능.
    EMATT_RGTYPE	emAttRgType;				//	공격가능거리 유형.
    WORD			wRange;						//	공격가능 거리.

    GLPADATA		sDamage;					//	타격값.
    float			fDelay;						//	딜래이.

    WORD			wUse_SP;					//	사용시 SP 소진량.

    char			szAniFile[ACF_SZNAME];		//	에니메이션 파일 이름.
    SANIATTACK		sAniAttack;					//	에니메이션 스텝.

    SNATIVEID		skill_id;					//	스킬 종류.
    WORD			skill_lev;					//	스킬 래벨.
    EMCROWSKTAR		skill_tar;					//	스킬 타겟.

    EMSTATE_BLOW	emBLOW_TYPE;				//	'상태이상' 적용 타입.
    float			fBLOW_RATE;					//	발생 확율.
    float			fBLOW_LIFE;					//	지속 시간.
    float			fBLOW_VAR1;					//	적용 값 1.
    float			fBLOW_VAR2;					//	적용 값 2.

    char			szSelfBodyEffect[ACF_SZNAME];	//	자기 자신의 몸에 붓는 이팩트.
    char			szTargBodyEffect[ACF_SZNAME];	//	목표 유닛의 몸에 붓는 이팩트.
    char			szTargetEffect[ACF_SZNAME];		//	목표 지향 이팩트.
};

//----------------------------------------------------------------------
struct SCROWATTACK
{
    enum { VERSION = 0x0109, };					//	3개에서 10개로 수정

    BOOL			bUsed;						//	사용가능.
    EMATT_RGTYPE	emAttRgType;				//	공격가능거리 유형.
    WORD			wRange;						//	공격가능 거리.

    WORD			wUse_SP;					//	사용시 SP 소진량.

    GLPADATA		sDamage;					//	타격값.
    float			fDelay;						//	딜래이.

    SNATIVEID		skill_id;					//	스킬 종류.
    WORD			skill_lev;					//	스킬 래벨.
    EMCROWSKTAR		skill_tar;					//	스킬 타겟.

    EMSTATE_BLOW	emBLOW_TYPE;				//	'상태이상' 적용 타입.
    float			fBLOW_RATE;					//	발생 확율.
    float			fBLOW_LIFE;					//	지속 시간.
    float			fBLOW_VAR1;					//	적용 값 1.
    float			fBLOW_VAR2;					//	적용 값 2.

    SANIATTACK		sAniAttack;					//	에니메이션 스텝.
    std::string		strAniFile;					//	에니메이션 파일 이름.

    std::string		strSelfBodyEffect;			//	자기 자신의 몸에 붓는 이팩트.
    std::string		strTargBodyEffect;			//	목표 유닛의 몸에 붓는 이팩트.
    std::string		strTargetEffect;			//	목표 지향 이팩트.

	BOOL			m_bAnimationMove;			//	AnimationMove ON/OFF
	D3DXVECTOR3		m_vAnimationMovePos;		//	AnimationMove Offset

    SCROWATTACK();
    SCROWATTACK(const SCROWATTACK& value);

    SCROWATTACK& operator= ( const SCROWATTACK_100 &OldData );
    SCROWATTACK& operator= ( const SCROWATTACK_101 &OldData );
    SCROWATTACK& operator= ( const SCROWATTACK_102 &OldData );
    SCROWATTACK& operator= ( const SCROWATTACK_103 &OldData );
    SCROWATTACK& operator= ( const SCROWATTACK_104 &OldData );
    SCROWATTACK& operator= ( const SCROWATTACK &value );

    bool LOAD_0105 ( sc::BaseStream &SFile );	//	데이터 포맷 변경 진행
	bool LOAD_0106_0108 ( sc::BaseStream &SFile );

    bool LOAD ( sc::BaseStream &SFile );
    bool SAVE ( sc::SerialFile &SFile );

    static VOID SaveCsvHead ( std::fstream &SFile, int iIndex );
    VOID SaveCsv ( std::fstream &SFile );
    VOID LoadCsv(CStringArray& StrArray, int& iCsvCur);
};

#endif // _CROW_ATTACK_H_
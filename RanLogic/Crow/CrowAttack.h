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

    BOOL			bUsed;						//	��밡��.
    EMATT_RGTYPE	emAttRgType;				//	���ݰ��ɰŸ� ����.
    WORD			wRange;						//	���ݰ��� �Ÿ�.
    WORD			wSphere;					//	���ݿ��� ����.

    //	Skill �� ���� �޴� ��ǥ.
    EMIMPACT_TARGET	emImpact_Tar;				//	Ÿ��. ( �ڽ�, ���, ��ġ ) &&
    EMIMPACT_REALM	emImpact_Realm;				//	����. ( Ÿ��, Ÿ�ٰ� �� ����, �� ���� ) &&
    EMIMPACT_SIDE	emImpact_Side;				//	"��" ( �� ������ �ڱ���, �� ������ ����� )

    EMELEMENT		emElement;					//	���� �Ӽ�.
    GLPADATA		sDamage;					//	Ÿ�ݰ�.
    float			fDelay;						//	������.
    float			fLife;						//	���� �ð�.

    WORD			wUse_MP;					//	���� MP ������.
    WORD			wUse_SP;					//	���� SP ������.

    EMSTATE_BLOW	emBlow;						//	�����̻�.
    float			fBlowRATE;					//	�߻� Ȯ��.
    float			fBlowVALUE;					//	���� ��.

    EMIMPACT_ADDON	emImpact;					//	�ΰ�ȿ�� ����.
    int				nImpactVAR;					//	��ȭ ��ġ.

    char			szAniFile[ACF_SZNAME];		//	���ϸ��̼� ���� �̸�.
    SANIATTACK		sAniAttack;					//	���ϸ��̼� ����.

    char			szSelfBodyEffect[ACF_SZNAME];	//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
    char			szTargBodyEffect[ACF_SZNAME];	//	��ǥ ������ ���� �״� ����Ʈ.
    char			szTargetEffect[ACF_SZNAME];		//	��ǥ ���� ����Ʈ.
};

//----------------------------------------------------------------------
struct SCROWATTACK_101
{
    BOOL			bUsed;						//	��밡��.
    EMATT_RGTYPE	emAttRgType;				//	���ݰ��ɰŸ� ����.
    WORD			wRange;						//	���ݰ��� �Ÿ�.

    GLPADATA		sDamage;					//	Ÿ�ݰ�.
    float			fDelay;						//	������.

    WORD			wUse_SP;					//	���� SP ������.

    char			szAniFile[ACF_SZNAME];		//	���ϸ��̼� ���� �̸�.
    SANIATTACK		sAniAttack;					//	���ϸ��̼� ����.

    SNATIVEID		skill_id;					//	������ ��ų ����.

    char			szSelfBodyEffect[ACF_SZNAME];	//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
    char			szTargBodyEffect[ACF_SZNAME];	//	��ǥ ������ ���� �״� ����Ʈ.
    char			szTargetEffect[ACF_SZNAME];		//	��ǥ ���� ����Ʈ.
};

struct SCROWATTACK_102
{
    BOOL			bUsed;						//	��밡��.
    EMATT_RGTYPE	emAttRgType;				//	���ݰ��ɰŸ� ����.
    WORD			wRange;						//	���ݰ��� �Ÿ�.

    GLPADATA		sDamage;					//	Ÿ�ݰ�.
    float			fDelay;						//	������.

    WORD			wUse_SP;					//	���� SP ������.

    char			szAniFile[ACF_SZNAME];		//	���ϸ��̼� ���� �̸�.
    SANIATTACK		sAniAttack;					//	���ϸ��̼� ����.

    SNATIVEID		skill_id;					//	������ ��ų ����.

    EMSTATE_BLOW	emBLOW_TYPE;				//	'�����̻�' ���� Ÿ��.
    float			fBLOW_RATE;					//	�߻� Ȯ��.
    float			fBLOW_LIFE;					//	���� �ð�.
    float			fBLOW_VAR1;					//	���� �� 1.
    float			fBLOW_VAR2;					//	���� �� 2.

    char			szSelfBodyEffect[ACF_SZNAME];	//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
    char			szTargBodyEffect[ACF_SZNAME];	//	��ǥ ������ ���� �״� ����Ʈ.
    char			szTargetEffect[ACF_SZNAME];		//	��ǥ ���� ����Ʈ.
};

enum EMCROWSKTAR
{
    CROWSKTAR_OUR	= 0,
    CROWSKTAR_ENEMY	= 1,
};

struct SCROWATTACK_103
{
    BOOL			bUsed;						//	��밡��.
    EMATT_RGTYPE	emAttRgType;				//	���ݰ��ɰŸ� ����.
    WORD			wRange;						//	���ݰ��� �Ÿ�.

    GLPADATA		sDamage;					//	Ÿ�ݰ�.
    float			fDelay;						//	������.

    WORD			wUse_SP;					//	���� SP ������.

    char			szAniFile[ACF_SZNAME];		//	���ϸ��̼� ���� �̸�.
    SANIATTACK_100	sAniAttack;					//	���ϸ��̼� ����.

    SNATIVEID		skill_id;					//	��ų ����.
    WORD			skill_lev;					//	��ų ����.
    EMCROWSKTAR		skill_tar;					//	��ų Ÿ��.

    EMSTATE_BLOW	emBLOW_TYPE;				//	'�����̻�' ���� Ÿ��.
    float			fBLOW_RATE;					//	�߻� Ȯ��.
    float			fBLOW_LIFE;					//	���� �ð�.
    float			fBLOW_VAR1;					//	���� �� 1.
    float			fBLOW_VAR2;					//	���� �� 2.

    char			szSelfBodyEffect[ACF_SZNAME];	//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
    char			szTargBodyEffect[ACF_SZNAME];	//	��ǥ ������ ���� �״� ����Ʈ.
    char			szTargetEffect[ACF_SZNAME];		//	��ǥ ���� ����Ʈ.
};

struct SCROWATTACK_104
{
    BOOL			bUsed;						//	��밡��.
    EMATT_RGTYPE	emAttRgType;				//	���ݰ��ɰŸ� ����.
    WORD			wRange;						//	���ݰ��� �Ÿ�.

    GLPADATA		sDamage;					//	Ÿ�ݰ�.
    float			fDelay;						//	������.

    WORD			wUse_SP;					//	���� SP ������.

    char			szAniFile[ACF_SZNAME];		//	���ϸ��̼� ���� �̸�.
    SANIATTACK		sAniAttack;					//	���ϸ��̼� ����.

    SNATIVEID		skill_id;					//	��ų ����.
    WORD			skill_lev;					//	��ų ����.
    EMCROWSKTAR		skill_tar;					//	��ų Ÿ��.

    EMSTATE_BLOW	emBLOW_TYPE;				//	'�����̻�' ���� Ÿ��.
    float			fBLOW_RATE;					//	�߻� Ȯ��.
    float			fBLOW_LIFE;					//	���� �ð�.
    float			fBLOW_VAR1;					//	���� �� 1.
    float			fBLOW_VAR2;					//	���� �� 2.

    char			szSelfBodyEffect[ACF_SZNAME];	//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
    char			szTargBodyEffect[ACF_SZNAME];	//	��ǥ ������ ���� �״� ����Ʈ.
    char			szTargetEffect[ACF_SZNAME];		//	��ǥ ���� ����Ʈ.
};

//----------------------------------------------------------------------
struct SCROWATTACK
{
    enum { VERSION = 0x0109, };					//	3������ 10���� ����

    BOOL			bUsed;						//	��밡��.
    EMATT_RGTYPE	emAttRgType;				//	���ݰ��ɰŸ� ����.
    WORD			wRange;						//	���ݰ��� �Ÿ�.

    WORD			wUse_SP;					//	���� SP ������.

    GLPADATA		sDamage;					//	Ÿ�ݰ�.
    float			fDelay;						//	������.

    SNATIVEID		skill_id;					//	��ų ����.
    WORD			skill_lev;					//	��ų ����.
    EMCROWSKTAR		skill_tar;					//	��ų Ÿ��.

    EMSTATE_BLOW	emBLOW_TYPE;				//	'�����̻�' ���� Ÿ��.
    float			fBLOW_RATE;					//	�߻� Ȯ��.
    float			fBLOW_LIFE;					//	���� �ð�.
    float			fBLOW_VAR1;					//	���� �� 1.
    float			fBLOW_VAR2;					//	���� �� 2.

    SANIATTACK		sAniAttack;					//	���ϸ��̼� ����.
    std::string		strAniFile;					//	���ϸ��̼� ���� �̸�.

    std::string		strSelfBodyEffect;			//	�ڱ� �ڽ��� ���� �״� ����Ʈ.
    std::string		strTargBodyEffect;			//	��ǥ ������ ���� �״� ����Ʈ.
    std::string		strTargetEffect;			//	��ǥ ���� ����Ʈ.

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

    bool LOAD_0105 ( sc::BaseStream &SFile );	//	������ ���� ���� ����
	bool LOAD_0106_0108 ( sc::BaseStream &SFile );

    bool LOAD ( sc::BaseStream &SFile );
    bool SAVE ( sc::SerialFile &SFile );

    static VOID SaveCsvHead ( std::fstream &SFile, int iIndex );
    VOID SaveCsv ( std::fstream &SFile );
    VOID LoadCsv(CStringArray& StrArray, int& iCsvCur);
};

#endif // _CROW_ATTACK_H_
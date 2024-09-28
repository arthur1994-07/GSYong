#include "../pch.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "./CrowBasic.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

SCROWBASIC::SCROWBASIC() :
    m_emTribe(TRIBE_HUMAN),
    m_emCrow(CROW_MOB),
    m_emBright(BRIGHT_LIGHT),
    m_bNoAllPenalty(FALSE),
    m_dwNoPenaltyFlag(0),

    m_wLevel(1),

    m_dwHP(70),
    m_wMP(40),
    m_wSP(20),

    m_fIncHP(0.3f),
    m_fIncMP(0.3f),
    m_fIncSP(0.5f),

    m_wAvoidRate(2),
    m_wHitRate(4),

    m_wViewRange(80),
    m_wDefense(0),

    m_wBonusExp(0),
    m_wBonusCP(0),

    m_bOverlapAttack(FALSE),

    m_bNonDamaged(FALSE),

    m_emPetType(PETTYPE_NONE),
    m_emSummonType(SUMMON_TYPE_NORMAL),
    m_emNPCType(NPC_TYPE_NORMAL),
    m_dwGenTime(0),
    m_wGatherTimeLow(0),
    m_wGatherTimeHigh(0),
    m_fGatherRate(0.0f),
    m_wGatherAnimation(0),

    m_bHideNaming(false),
    m_bNonTargeting(false),

    m_fScale(1.f),
	m_bHideTooltip(false),
	m_wResist_Probability(100),
	m_fBarrierRadius(50.0f),
	m_fBarrierWidth(5.0f),
    m_fRefenceActionableRange(-1.0f)
{
    SecureZeroMemory ( m_szName, sizeof(m_szName) );
}

SCROWBASIC& SCROWBASIC::operator = ( const SCROWBASIC_100 &sCrowBasic )
{
	sNativeID = sCrowBasic.sNativeID;
	StringCbCopy ( m_szName, CHAR_SZNAME, sCrowBasic.m_szName );
	m_emTribe = sCrowBasic.m_emTribe;
	m_emCrow = sCrowBasic.m_emCrow;
	m_emBright = sCrowBasic.m_emBright;

	// 소환수 타입
	m_emSummonType = SUMMON_TYPE_NORMAL;

	// NPC 타입
	m_emNPCType = NPC_TYPE_NORMAL;

	m_wLevel = sCrowBasic.m_wLevel;

	m_dwHP = sCrowBasic.m_wHP;
	m_wMP = sCrowBasic.m_wMP;
	m_wSP = sCrowBasic.m_wSP;

	//	회복율.
	m_fIncHP = sCrowBasic.m_fIncHP;
	m_fIncMP = sCrowBasic.m_fIncMP;
	m_fIncSP = sCrowBasic.m_fIncSP;

	m_wAvoidRate = sCrowBasic.m_wAvoidRate;
	m_wHitRate = sCrowBasic.m_wHitRate;

	m_wViewRange = sCrowBasic.m_wViewRange;
	m_wDefense = sCrowBasic.m_wDefense;

	m_bOverlapAttack = FALSE;

	m_bNonDamaged = FALSE;

	m_sResist.Assign ( sCrowBasic.m_sResist );

	m_bNoAllPenalty   = FALSE;
	m_dwNoPenaltyFlag = 0;

	return *this;
}

SCROWBASIC& SCROWBASIC::operator = ( const SCROWBASIC_101 &sCrowBasic )
{
	sNativeID = sCrowBasic.sNativeID;
	StringCbCopy ( m_szName, CHAR_SZNAME, sCrowBasic.m_szName );
	m_emTribe = sCrowBasic.m_emTribe;
	m_emCrow = sCrowBasic.m_emCrow;
	m_emBright = sCrowBasic.m_emBright;

	// 소환수 타입
	m_emSummonType = SUMMON_TYPE_NORMAL;

	// NPC 타입
	m_emNPCType = NPC_TYPE_NORMAL;

	m_wLevel = sCrowBasic.m_wLevel;

	m_dwHP = sCrowBasic.m_wHP;
	m_wMP = sCrowBasic.m_wMP;
	m_wSP = sCrowBasic.m_wSP;

	//	회복율.
	m_fIncHP = sCrowBasic.m_fIncHP;
	m_fIncMP = sCrowBasic.m_fIncMP;
	m_fIncSP = sCrowBasic.m_fIncSP;

	m_wAvoidRate = sCrowBasic.m_wAvoidRate;
	m_wHitRate = sCrowBasic.m_wHitRate;

	m_wViewRange = sCrowBasic.m_wViewRange;
	m_wDefense = sCrowBasic.m_wDefense;

	m_sResist = sCrowBasic.m_sResist;

	m_bOverlapAttack = FALSE;

	m_bNonDamaged = FALSE;

	m_bNoAllPenalty   = FALSE;
	m_dwNoPenaltyFlag = 0;

	return *this;
}

SCROWBASIC& SCROWBASIC::operator = ( const SCROWBASIC_102 &sCrowBasic )
{
	sNativeID = sCrowBasic.sNativeID;
	StringCbCopy( m_szName, CHAR_SZNAME, sCrowBasic.m_szName );
	m_emTribe = sCrowBasic.m_emTribe;
	m_emCrow = sCrowBasic.m_emCrow;
	m_emBright = sCrowBasic.m_emBright;

	// 소환수 타입
	m_emSummonType = SUMMON_TYPE_NORMAL;

	// NPC 타입
	m_emNPCType = NPC_TYPE_NORMAL;

	m_wLevel = sCrowBasic.m_wLevel;

	m_dwHP = sCrowBasic.m_wHP;
	m_wMP = sCrowBasic.m_wMP;
	m_wSP = sCrowBasic.m_wSP;

	//	회복율.
	m_fIncHP = sCrowBasic.m_fIncHP;
	m_fIncMP = sCrowBasic.m_fIncMP;
	m_fIncSP = sCrowBasic.m_fIncSP;

	m_wAvoidRate = sCrowBasic.m_wAvoidRate;
	m_wHitRate = sCrowBasic.m_wHitRate;

	m_wViewRange = sCrowBasic.m_wViewRange;
	m_wDefense = sCrowBasic.m_wDefense;

	m_sResist = sCrowBasic.m_sResist;

	m_wBonusExp = sCrowBasic.m_wBonusExp;

	m_bOverlapAttack = FALSE;

	m_bNonDamaged = FALSE;

	m_bNoAllPenalty   = FALSE;
	m_dwNoPenaltyFlag = 0;

	return *this;
}

SCROWBASIC& SCROWBASIC::operator = ( const SCROWBASIC_103 &sCrowBasic )
{
	sNativeID = sCrowBasic.sNativeID;
	StringCbCopy( m_szName, CHAR_SZNAME, sCrowBasic.m_szName );
	m_emTribe = sCrowBasic.m_emTribe;
	m_emCrow = sCrowBasic.m_emCrow;
	m_emBright = sCrowBasic.m_emBright;

	// 소환수 타입
	m_emSummonType = SUMMON_TYPE_NORMAL;

	// NPC 타입
	m_emNPCType = NPC_TYPE_NORMAL;

	m_wLevel = sCrowBasic.m_wLevel;

	m_dwHP = sCrowBasic.m_dwHP;
	m_wMP = sCrowBasic.m_wMP;
	m_wSP = sCrowBasic.m_wSP;

	//	회복율.
	m_fIncHP = sCrowBasic.m_fIncHP;
	m_fIncMP = sCrowBasic.m_fIncMP;
	m_fIncSP = sCrowBasic.m_fIncSP;

	m_wAvoidRate = sCrowBasic.m_wAvoidRate;
	m_wHitRate = sCrowBasic.m_wHitRate;

	m_wViewRange = sCrowBasic.m_wViewRange;
	m_wDefense = sCrowBasic.m_wDefense;

	m_sResist = sCrowBasic.m_sResist;

	m_wBonusExp = sCrowBasic.m_wBonusExp;

	m_bOverlapAttack = FALSE;

	m_bNonDamaged = FALSE;

	m_bNoAllPenalty   = FALSE;
	m_dwNoPenaltyFlag = 0;

	return *this;
}

SCROWBASIC& SCROWBASIC::operator = ( const SCROWBASIC_104 &sCrowBasic )
{
	sNativeID = sCrowBasic.sNativeID;
	StringCbCopy( m_szName, CHAR_SZNAME, sCrowBasic.m_szName );
	m_emTribe = sCrowBasic.m_emTribe;
	m_emCrow = sCrowBasic.m_emCrow;
	m_emBright = sCrowBasic.m_emBright;

	// 팻타입
	m_emPetType = sCrowBasic.m_emPetType;

	// 소환수 타입
	m_emSummonType = SUMMON_TYPE_NORMAL;

	// NPC 타입
	m_emNPCType = NPC_TYPE_NORMAL;

	m_wLevel = sCrowBasic.m_wLevel;

	m_dwHP = sCrowBasic.m_dwHP;
	m_wMP = sCrowBasic.m_wMP;
	m_wSP = sCrowBasic.m_wSP;

	//	회복율.
	m_fIncHP = sCrowBasic.m_fIncHP;
	m_fIncMP = sCrowBasic.m_fIncMP;
	m_fIncSP = sCrowBasic.m_fIncSP;

	m_wAvoidRate = sCrowBasic.m_wAvoidRate;
	m_wHitRate = sCrowBasic.m_wHitRate;

	m_wViewRange = sCrowBasic.m_wViewRange;
	m_wDefense = sCrowBasic.m_wDefense;

	m_sResist = sCrowBasic.m_sResist;

	m_wBonusExp = sCrowBasic.m_wBonusExp;

	m_bOverlapAttack = FALSE;

	m_bNonDamaged = FALSE;

	m_bNoAllPenalty   = FALSE;
	m_dwNoPenaltyFlag = 0;

	return *this;
}

SCROWBASIC& SCROWBASIC::operator = ( const SCROWBASIC_105 &sCrowBasic )
{
	sNativeID = sCrowBasic.sNativeID;
	StringCbCopy( m_szName, CHAR_SZNAME, sCrowBasic.m_szName );
	m_emTribe = sCrowBasic.m_emTribe;
	m_emCrow = sCrowBasic.m_emCrow;
	m_emBright = sCrowBasic.m_emBright;

	// 팻타입
	m_emPetType = sCrowBasic.m_emPetType;

	// 소환수 타입
	m_emSummonType = SUMMON_TYPE_NORMAL;

	// NPC 타입
	m_emNPCType = NPC_TYPE_NORMAL;

	m_wLevel = sCrowBasic.m_wLevel;

	m_dwHP = sCrowBasic.m_dwHP;
	m_wMP = sCrowBasic.m_wMP;
	m_wSP = sCrowBasic.m_wSP;

	//	회복율.
	m_fIncHP = sCrowBasic.m_fIncHP;
	m_fIncMP = sCrowBasic.m_fIncMP;
	m_fIncSP = sCrowBasic.m_fIncSP;

	m_wAvoidRate = sCrowBasic.m_wAvoidRate;
	m_wHitRate = sCrowBasic.m_wHitRate;

	m_wViewRange = sCrowBasic.m_wViewRange;
	m_wDefense = sCrowBasic.m_wDefense;

	m_sResist = sCrowBasic.m_sResist;

	m_wBonusExp = sCrowBasic.m_wBonusExp;

	m_bOverlapAttack = FALSE;

	m_bNonDamaged = FALSE;

	m_bNoAllPenalty   = FALSE;
	m_dwNoPenaltyFlag = 0;

	return *this;
}

SCROWBASIC& SCROWBASIC::operator = ( const SCROWBASIC_106 &sCrowBasic )
{
	sNativeID = sCrowBasic.sNativeID;
	StringCbCopy( m_szName, CHAR_SZNAME, sCrowBasic.m_szName );
	m_emTribe = sCrowBasic.m_emTribe;
	m_emCrow = sCrowBasic.m_emCrow;
	m_emBright = sCrowBasic.m_emBright;

	// 팻타입
	m_emPetType = sCrowBasic.m_emPetType;

	// 소환수 타입
	m_emSummonType = SUMMON_TYPE_NORMAL;

	// NPC 타입
	m_emNPCType = NPC_TYPE_NORMAL;

	m_wLevel = sCrowBasic.m_wLevel;

	m_dwHP = sCrowBasic.m_dwHP;
	m_wMP = sCrowBasic.m_wMP;
	m_wSP = sCrowBasic.m_wSP;

	//	회복율.
	m_fIncHP = sCrowBasic.m_fIncHP;
	m_fIncMP = sCrowBasic.m_fIncMP;
	m_fIncSP = sCrowBasic.m_fIncSP;

	m_wAvoidRate = sCrowBasic.m_wAvoidRate;
	m_wHitRate = sCrowBasic.m_wHitRate;

	m_wViewRange = sCrowBasic.m_wViewRange;
	m_wDefense = sCrowBasic.m_wDefense;

	m_sResist = sCrowBasic.m_sResist;

	m_wBonusExp = sCrowBasic.m_wBonusExp;

	m_bOverlapAttack = sCrowBasic.m_bOverlapAttack;

	m_bNonDamaged = FALSE;

	m_bNoAllPenalty   = FALSE;
	m_dwNoPenaltyFlag = 0;

	return *this;
}

SCROWBASIC& SCROWBASIC::operator = ( const SCROWBASIC_108 &sCrowBasic )
{
	sNativeID = sCrowBasic.sNativeID;
	StringCbCopy( m_szName, CHAR_SZNAME, sCrowBasic.m_szName );
	m_emTribe = sCrowBasic.m_emTribe;
	m_emCrow = sCrowBasic.m_emCrow;
	m_emBright = sCrowBasic.m_emBright;

	// 팻타입
	m_emPetType = sCrowBasic.m_emPetType;

	// 소환수 타입
	m_emSummonType = SUMMON_TYPE_NORMAL;

	// NPC 타입
	m_emNPCType = NPC_TYPE_NORMAL;

	m_wLevel = sCrowBasic.m_wLevel;

	m_dwHP = sCrowBasic.m_dwHP;
	m_wMP = sCrowBasic.m_wMP;
	m_wSP = sCrowBasic.m_wSP;

	//	회복율.
	m_fIncHP = sCrowBasic.m_fIncHP;
	m_fIncMP = sCrowBasic.m_fIncMP;
	m_fIncSP = sCrowBasic.m_fIncSP;

	m_wAvoidRate = sCrowBasic.m_wAvoidRate;
	m_wHitRate = sCrowBasic.m_wHitRate;

	m_wViewRange = sCrowBasic.m_wViewRange;
	m_wDefense = sCrowBasic.m_wDefense;

	m_sResist = sCrowBasic.m_sResist;

	m_wBonusExp = sCrowBasic.m_wBonusExp;

	m_bOverlapAttack = sCrowBasic.m_bOverlapAttack;

	m_bNonDamaged = FALSE;

	m_bNoAllPenalty   = FALSE;
	m_dwNoPenaltyFlag = 0;

	return *this;
}

SCROWBASIC& SCROWBASIC::operator = ( const SCROWBASIC_109 &sCrowBasic )
{
	sNativeID = sCrowBasic.sNativeID;
	StringCbCopy( m_szName, CHAR_SZNAME, sCrowBasic.m_szName );
	m_emTribe = sCrowBasic.m_emTribe;
	m_emCrow = sCrowBasic.m_emCrow;
	m_emBright = sCrowBasic.m_emBright;

	// 팻타입
	m_emPetType = sCrowBasic.m_emPetType;

	// 소환수 타입
	m_emSummonType = SUMMON_TYPE_NORMAL;

	// NPC 타입
	m_emNPCType = NPC_TYPE_NORMAL;

	m_wLevel = sCrowBasic.m_wLevel;

	m_dwHP = sCrowBasic.m_dwHP;
	m_wMP = sCrowBasic.m_wMP;
	m_wSP = sCrowBasic.m_wSP;

	//	회복율.
	m_fIncHP = sCrowBasic.m_fIncHP;
	m_fIncMP = sCrowBasic.m_fIncMP;
	m_fIncSP = sCrowBasic.m_fIncSP;

	m_wAvoidRate = sCrowBasic.m_wAvoidRate;
	m_wHitRate = sCrowBasic.m_wHitRate;

	m_wViewRange = sCrowBasic.m_wViewRange;
	m_wDefense = sCrowBasic.m_wDefense;

	m_sResist = sCrowBasic.m_sResist;

	m_wBonusExp = sCrowBasic.m_wBonusExp;

	m_bOverlapAttack = sCrowBasic.m_bOverlapAttack;

	m_bNonDamaged = FALSE;

	m_dwGenTime = sCrowBasic.m_dwGenTime;

	m_bNoAllPenalty   = FALSE;
	m_dwNoPenaltyFlag = 0;

	return *this;

}

SCROWBASIC& SCROWBASIC::operator = ( const SCROWBASIC_110 &sCrowBasic )
{
	sNativeID = sCrowBasic.sNativeID;
	StringCbCopy( m_szName, CHAR_SZNAME, sCrowBasic.m_szName );
	m_emTribe = sCrowBasic.m_emTribe;
	m_emCrow = sCrowBasic.m_emCrow;
	m_emBright = sCrowBasic.m_emBright;

	// 팻타입
	m_emPetType = sCrowBasic.m_emPetType;

	// 소환수 타입
	m_emSummonType = SUMMON_TYPE_NORMAL;

	// NPC 타입
	m_emNPCType = NPC_TYPE_NORMAL;

	m_wLevel = sCrowBasic.m_wLevel;

	m_dwHP = sCrowBasic.m_dwHP;
	m_wMP = sCrowBasic.m_wMP;
	m_wSP = sCrowBasic.m_wSP;

	//	회복율.
	m_fIncHP = sCrowBasic.m_fIncHP;
	m_fIncMP = sCrowBasic.m_fIncMP;
	m_fIncSP = sCrowBasic.m_fIncSP;

	m_wAvoidRate = sCrowBasic.m_wAvoidRate;
	m_wHitRate = sCrowBasic.m_wHitRate;

	m_wViewRange = sCrowBasic.m_wViewRange;
	m_wDefense = sCrowBasic.m_wDefense;

	m_sResist = sCrowBasic.m_sResist;

	m_wBonusExp = sCrowBasic.m_wBonusExp;

	m_bOverlapAttack = sCrowBasic.m_bOverlapAttack;

	m_bNonDamaged = FALSE;

	m_dwGenTime = sCrowBasic.m_dwGenTime;

	m_wGatherTimeLow = sCrowBasic.m_wGatherTimeLow;			
	m_wGatherTimeHigh = sCrowBasic.m_wGatherTimeHigh;			
	m_fGatherRate = sCrowBasic.m_fGatherRate;			
	m_wGatherAnimation = sCrowBasic.m_wGatherAnimation;

	m_bNoAllPenalty   = FALSE;
	m_dwNoPenaltyFlag = 0;

	return *this;

}

SCROWBASIC& SCROWBASIC::operator = ( const SCROWBASIC_111 &sCrowBasic )
{
	sNativeID = sCrowBasic.sNativeID;
	StringCbCopy( m_szName, CHAR_SZNAME, sCrowBasic.m_szName );
	m_emTribe = sCrowBasic.m_emTribe;
	m_emCrow = sCrowBasic.m_emCrow;
	m_emBright = sCrowBasic.m_emBright;

	// 팻타입
	m_emPetType = sCrowBasic.m_emPetType;

	// 소환수 타입
	m_emSummonType = SUMMON_TYPE_NORMAL;

	// NPC 타입
	m_emNPCType = NPC_TYPE_NORMAL;

	m_wLevel = sCrowBasic.m_wLevel;

	m_dwHP = sCrowBasic.m_dwHP;
	m_wMP = sCrowBasic.m_wMP;
	m_wSP = sCrowBasic.m_wSP;

	//	회복율.
	m_fIncHP = sCrowBasic.m_fIncHP;
	m_fIncMP = sCrowBasic.m_fIncMP;
	m_fIncSP = sCrowBasic.m_fIncSP;

	m_wAvoidRate = sCrowBasic.m_wAvoidRate;
	m_wHitRate = sCrowBasic.m_wHitRate;

	m_wViewRange = sCrowBasic.m_wViewRange;
	m_wDefense = sCrowBasic.m_wDefense;

	m_sResist = sCrowBasic.m_sResist;

	m_wBonusExp = sCrowBasic.m_wBonusExp;

	m_bOverlapAttack = sCrowBasic.m_bOverlapAttack;

	m_bNonDamaged = FALSE;

	m_dwGenTime = sCrowBasic.m_dwGenTime;

	m_wGatherTimeLow = sCrowBasic.m_wGatherTimeLow;			
	m_wGatherTimeHigh = sCrowBasic.m_wGatherTimeHigh;			
	m_fGatherRate = sCrowBasic.m_fGatherRate;			
	m_wGatherAnimation = sCrowBasic.m_wGatherAnimation;

	m_bNoAllPenalty   = FALSE;
	m_dwNoPenaltyFlag = 0;

	return *this;

}

SCROWBASIC& SCROWBASIC::operator = ( const SCROWBASIC_112 &sCrowBasic )
{
	sNativeID = sCrowBasic.sNativeID;
	StringCbCopy( m_szName, CHAR_SZNAME, sCrowBasic.m_szName );
	m_emTribe = sCrowBasic.m_emTribe;
	m_emCrow = sCrowBasic.m_emCrow;
	m_emBright = sCrowBasic.m_emBright;

	// 팻타입
	m_emPetType = sCrowBasic.m_emPetType;

	// 소환수 타입
	m_emSummonType = SUMMON_TYPE_NORMAL;

	// NPC 타입
	m_emNPCType = NPC_TYPE_NORMAL;

	m_wLevel = sCrowBasic.m_wLevel;

	m_dwHP = sCrowBasic.m_dwHP;
	m_wMP = sCrowBasic.m_wMP;
	m_wSP = sCrowBasic.m_wSP;

	//	회복율.
	m_fIncHP = sCrowBasic.m_fIncHP;
	m_fIncMP = sCrowBasic.m_fIncMP;
	m_fIncSP = sCrowBasic.m_fIncSP;

	m_wAvoidRate = sCrowBasic.m_wAvoidRate;
	m_wHitRate = sCrowBasic.m_wHitRate;

	m_wViewRange = sCrowBasic.m_wViewRange;
	m_wDefense = sCrowBasic.m_wDefense;

	m_sResist = sCrowBasic.m_sResist;

	m_wBonusExp = sCrowBasic.m_wBonusExp;

	m_bOverlapAttack = sCrowBasic.m_bOverlapAttack;

	m_bNonDamaged = FALSE;

	m_dwGenTime = sCrowBasic.m_dwGenTime;

	m_wGatherTimeLow = sCrowBasic.m_wGatherTimeLow;			
	m_wGatherTimeHigh = sCrowBasic.m_wGatherTimeHigh;			
	m_fGatherRate = sCrowBasic.m_fGatherRate;			
	m_wGatherAnimation = sCrowBasic.m_wGatherAnimation;

	m_bNoAllPenalty   = sCrowBasic.m_bNoPenalty;
	m_dwNoPenaltyFlag = 0;

	return *this;

}

SCROWBASIC& SCROWBASIC::operator = ( const SCROWBASIC_113 &sCrowBasic )
{
	sNativeID = sCrowBasic.sNativeID;
	StringCbCopy( m_szName, CHAR_SZNAME, sCrowBasic.m_szName );
	m_emTribe = sCrowBasic.m_emTribe;
	m_emCrow = sCrowBasic.m_emCrow;
	m_emBright = sCrowBasic.m_emBright;

	// 팻타입
	m_emPetType = sCrowBasic.m_emPetType;

	// 소환수 타입
	m_emSummonType = SUMMON_TYPE_NORMAL;

	// NPC 타입
	m_emNPCType = NPC_TYPE_NORMAL;

	m_wLevel = sCrowBasic.m_wLevel;

	m_dwHP = sCrowBasic.m_dwHP;
	m_wMP = sCrowBasic.m_wMP;
	m_wSP = sCrowBasic.m_wSP;

	//	회복율.
	m_fIncHP = sCrowBasic.m_fIncHP;
	m_fIncMP = sCrowBasic.m_fIncMP;
	m_fIncSP = sCrowBasic.m_fIncSP;

	m_wAvoidRate = sCrowBasic.m_wAvoidRate;
	m_wHitRate = sCrowBasic.m_wHitRate;

	m_wViewRange = sCrowBasic.m_wViewRange;
	m_wDefense = sCrowBasic.m_wDefense;

	m_sResist = sCrowBasic.m_sResist;

	m_wBonusExp = sCrowBasic.m_wBonusExp;
	m_wBonusCP = sCrowBasic.m_wBonusCP;

	m_bOverlapAttack = sCrowBasic.m_bOverlapAttack;

	m_bNonDamaged = FALSE;

	m_dwGenTime = sCrowBasic.m_dwGenTime;

	m_wGatherTimeLow = sCrowBasic.m_wGatherTimeLow;			
	m_wGatherTimeHigh = sCrowBasic.m_wGatherTimeHigh;			
	m_fGatherRate = sCrowBasic.m_fGatherRate;			
	m_wGatherAnimation = sCrowBasic.m_wGatherAnimation;

	m_bNoAllPenalty   = sCrowBasic.m_bNoPenalty;
	m_dwNoPenaltyFlag = 0;

	return *this;
}

SCROWBASIC& SCROWBASIC::operator = ( const SCROWBASIC_114 &sCrowBasic )
{
	sNativeID = sCrowBasic.sNativeID;
	StringCbCopy( m_szName, CHAR_SZNAME, sCrowBasic.m_szName );
	m_emTribe = sCrowBasic.m_emTribe;
	m_emCrow = sCrowBasic.m_emCrow;
	m_emBright = sCrowBasic.m_emBright;

	// 팻타입
	m_emPetType = sCrowBasic.m_emPetType;

	// 소환수 타입
	m_emSummonType = SUMMON_TYPE_NORMAL;

	// NPC 타입
	m_emNPCType = NPC_TYPE_NORMAL;

	m_wLevel = sCrowBasic.m_wLevel;

	m_dwHP = sCrowBasic.m_dwHP;
	m_wMP = sCrowBasic.m_wMP;
	m_wSP = sCrowBasic.m_wSP;

	//	회복율.
	m_fIncHP = sCrowBasic.m_fIncHP;
	m_fIncMP = sCrowBasic.m_fIncMP;
	m_fIncSP = sCrowBasic.m_fIncSP;

	m_wAvoidRate = sCrowBasic.m_wAvoidRate;
	m_wHitRate = sCrowBasic.m_wHitRate;

	m_wViewRange = sCrowBasic.m_wViewRange;
	m_wDefense = sCrowBasic.m_wDefense;

	m_sResist = sCrowBasic.m_sResist;

	m_wBonusExp = sCrowBasic.m_wBonusExp;
	m_wBonusCP = sCrowBasic.m_wBonusCP;

	m_bOverlapAttack = sCrowBasic.m_bOverlapAttack;

	m_bNonDamaged = sCrowBasic.m_bNonDamaged;

	m_dwGenTime = sCrowBasic.m_dwGenTime;

	m_wGatherTimeLow = sCrowBasic.m_wGatherTimeLow;			
	m_wGatherTimeHigh = sCrowBasic.m_wGatherTimeHigh;			
	m_fGatherRate = sCrowBasic.m_fGatherRate;			
	m_wGatherAnimation = sCrowBasic.m_wGatherAnimation;

	m_bNoAllPenalty   = sCrowBasic.m_bNoPenalty;
	m_dwNoPenaltyFlag = 0;

	return *this;
}

SCROWBASIC& SCROWBASIC::operator = ( const SCROWBASIC_115 &sCrowBasic )
{
	sNativeID = sCrowBasic.sNativeID;
	StringCbCopy( m_szName, CHAR_SZNAME, sCrowBasic.m_szName );
	m_emTribe = sCrowBasic.m_emTribe;
	m_emCrow = sCrowBasic.m_emCrow;
	m_emBright = sCrowBasic.m_emBright;

	// 팻타입
	m_emPetType = sCrowBasic.m_emPetType;

	// 소환수 타입
	m_emSummonType = sCrowBasic.m_emSummonType;

	// NPC 타입
	m_emNPCType = NPC_TYPE_NORMAL;

	m_wLevel = sCrowBasic.m_wLevel;

	m_dwHP = sCrowBasic.m_dwHP;
	m_wMP = sCrowBasic.m_wMP;
	m_wSP = sCrowBasic.m_wSP;

	//	회복율.
	m_fIncHP = sCrowBasic.m_fIncHP;
	m_fIncMP = sCrowBasic.m_fIncMP;
	m_fIncSP = sCrowBasic.m_fIncSP;

	m_wAvoidRate = sCrowBasic.m_wAvoidRate;
	m_wHitRate = sCrowBasic.m_wHitRate;

	m_wViewRange = sCrowBasic.m_wViewRange;
	m_wDefense = sCrowBasic.m_wDefense;

	m_sResist = sCrowBasic.m_sResist;

	m_wBonusExp = sCrowBasic.m_wBonusExp;
	m_wBonusCP = sCrowBasic.m_wBonusCP;

	m_bOverlapAttack = sCrowBasic.m_bOverlapAttack;

	m_bNonDamaged = sCrowBasic.m_bNonDamaged;

	m_dwGenTime = sCrowBasic.m_dwGenTime;

	m_wGatherTimeLow = sCrowBasic.m_wGatherTimeLow;			
	m_wGatherTimeHigh = sCrowBasic.m_wGatherTimeHigh;			
	m_fGatherRate = sCrowBasic.m_fGatherRate;			
	m_wGatherAnimation = sCrowBasic.m_wGatherAnimation;

	m_bNoAllPenalty   = sCrowBasic.m_bNoPenalty;
	m_dwNoPenaltyFlag = 0;

	return *this;
}

SCROWBASIC& SCROWBASIC::operator = ( const SCROWBASIC_116 &sCrowBasic )
{
    sNativeID = sCrowBasic.sNativeID;
    StringCbCopy( m_szName, CHAR_SZNAME, sCrowBasic.m_szName );
    m_emTribe = sCrowBasic.m_emTribe;
    m_emCrow = sCrowBasic.m_emCrow;
    m_emBright = sCrowBasic.m_emBright;

    // 팻타입
    m_emPetType = sCrowBasic.m_emPetType;

    // 소환수 타입
    m_emSummonType = sCrowBasic.m_emSummonType;

    // NPC 타입
    m_emNPCType = NPC_TYPE_NORMAL;

    m_wLevel = sCrowBasic.m_wLevel;

    m_dwHP = sCrowBasic.m_dwHP;
    m_wMP = sCrowBasic.m_wMP;
    m_wSP = sCrowBasic.m_wSP;

    //	회복율.
    m_fIncHP = sCrowBasic.m_fIncHP;
    m_fIncMP = sCrowBasic.m_fIncMP;
    m_fIncSP = sCrowBasic.m_fIncSP;

    m_wAvoidRate = sCrowBasic.m_wAvoidRate;
    m_wHitRate = sCrowBasic.m_wHitRate;

    m_wViewRange = sCrowBasic.m_wViewRange;
    m_wDefense = sCrowBasic.m_wDefense;

    m_sResist = sCrowBasic.m_sResist;

    m_wBonusExp = sCrowBasic.m_wBonusExp;
    m_wBonusCP = sCrowBasic.m_wBonusCP;

    m_bOverlapAttack = sCrowBasic.m_bOverlapAttack;

    m_bNonDamaged = sCrowBasic.m_bNonDamaged;

    m_dwGenTime = sCrowBasic.m_dwGenTime;

    m_wGatherTimeLow = sCrowBasic.m_wGatherTimeLow;			
    m_wGatherTimeHigh = sCrowBasic.m_wGatherTimeHigh;			
    m_fGatherRate = sCrowBasic.m_fGatherRate;			
    m_wGatherAnimation = sCrowBasic.m_wGatherAnimation;

    m_bNoAllPenalty   = sCrowBasic.m_bNoAllPenalty;    
    m_dwNoPenaltyFlag = 0;

    m_emNPCType = sCrowBasic.m_emNPCType;

    return *this;
}

SCROWBASIC& SCROWBASIC::operator = ( const SCROWBASIC_117 &sCrowBasic )
{
    sNativeID = sCrowBasic.sNativeID;
    StringCbCopy( m_szName, CHAR_SZNAME, sCrowBasic.m_szName );
    m_emTribe = sCrowBasic.m_emTribe;
    m_emCrow = sCrowBasic.m_emCrow;
    m_emBright = sCrowBasic.m_emBright;

    // 팻타입
    m_emPetType = sCrowBasic.m_emPetType;

    // 소환수 타입
    m_emSummonType = sCrowBasic.m_emSummonType;

    // NPC 타입
    m_emNPCType = NPC_TYPE_NORMAL;

    m_wLevel = sCrowBasic.m_wLevel;

    m_dwHP = sCrowBasic.m_dwHP;
    m_wMP = sCrowBasic.m_wMP;
    m_wSP = sCrowBasic.m_wSP;

    //	회복율.
    m_fIncHP = sCrowBasic.m_fIncHP;
    m_fIncMP = sCrowBasic.m_fIncMP;
    m_fIncSP = sCrowBasic.m_fIncSP;

    m_wAvoidRate = sCrowBasic.m_wAvoidRate;
    m_wHitRate = sCrowBasic.m_wHitRate;

    m_wViewRange = sCrowBasic.m_wViewRange;
    m_wDefense = sCrowBasic.m_wDefense;

    m_sResist = sCrowBasic.m_sResist;

    m_wBonusExp = sCrowBasic.m_wBonusExp;
    m_wBonusCP = sCrowBasic.m_wBonusCP;

    m_bOverlapAttack = sCrowBasic.m_bOverlapAttack;

    m_bNonDamaged = sCrowBasic.m_bNonDamaged;

    m_dwGenTime = sCrowBasic.m_dwGenTime;

    m_wGatherTimeLow = sCrowBasic.m_wGatherTimeLow;			
    m_wGatherTimeHigh = sCrowBasic.m_wGatherTimeHigh;			
    m_fGatherRate = sCrowBasic.m_fGatherRate;			
    m_wGatherAnimation = sCrowBasic.m_wGatherAnimation;

    m_bNoAllPenalty   = sCrowBasic.m_bNoAllPenalty;    
    m_dwNoPenaltyFlag = 0;

    m_emNPCType = sCrowBasic.m_emNPCType;

    m_bHideNaming = sCrowBasic.m_bHideNaming;
    m_bNonTargeting = sCrowBasic.m_bNonTargeting;

    return *this;
}

SCROWBASIC& SCROWBASIC::operator = ( const SCROWBASIC_118 &sCrowBasic )
{
	sNativeID = sCrowBasic.sNativeID;
	StringCbCopy( m_szName, CHAR_SZNAME, sCrowBasic.m_szName );
	m_emTribe = sCrowBasic.m_emTribe;
	m_emCrow = sCrowBasic.m_emCrow;
	m_emBright = sCrowBasic.m_emBright;

	// 팻타입
	m_emPetType = sCrowBasic.m_emPetType;

	// 소환수 타입
	m_emSummonType = sCrowBasic.m_emSummonType;

	// NPC 타입
	m_emNPCType = NPC_TYPE_NORMAL;

	m_wLevel = sCrowBasic.m_wLevel;

	m_dwHP = sCrowBasic.m_dwHP;
	m_wMP = sCrowBasic.m_wMP;
	m_wSP = sCrowBasic.m_wSP;

	//	회복율.
	m_fIncHP = sCrowBasic.m_fIncHP;
	m_fIncMP = sCrowBasic.m_fIncMP;
	m_fIncSP = sCrowBasic.m_fIncSP;

	m_wAvoidRate = sCrowBasic.m_wAvoidRate;
	m_wHitRate = sCrowBasic.m_wHitRate;

	m_wViewRange = sCrowBasic.m_wViewRange;
	m_wDefense = sCrowBasic.m_wDefense;

	m_sResist = sCrowBasic.m_sResist;

	m_wBonusExp = sCrowBasic.m_wBonusExp;
	m_wBonusCP = sCrowBasic.m_wBonusCP;

	m_bOverlapAttack = sCrowBasic.m_bOverlapAttack;

	m_bNonDamaged = sCrowBasic.m_bNonDamaged;

	m_dwGenTime = sCrowBasic.m_dwGenTime;

	m_wGatherTimeLow = sCrowBasic.m_wGatherTimeLow;			
	m_wGatherTimeHigh = sCrowBasic.m_wGatherTimeHigh;			
	m_fGatherRate = sCrowBasic.m_fGatherRate;			
	m_wGatherAnimation = sCrowBasic.m_wGatherAnimation;

	m_bNoAllPenalty   = sCrowBasic.m_bNoAllPenalty;    
	m_dwNoPenaltyFlag = 0;

	m_emNPCType = sCrowBasic.m_emNPCType;

	m_bHideNaming = sCrowBasic.m_bHideNaming;
	m_bNonTargeting = sCrowBasic.m_bNonTargeting;

	m_fScale = sCrowBasic.m_fScale;

	return *this;
}

SCROWBASIC& SCROWBASIC::operator = ( const SCROWBASIC_119 &sCrowBasic )
{
	sNativeID = sCrowBasic.sNativeID;
	StringCbCopy( m_szName, CHAR_SZNAME, sCrowBasic.m_szName );
	m_emTribe = sCrowBasic.m_emTribe;
	m_emCrow = sCrowBasic.m_emCrow;
	m_emBright = sCrowBasic.m_emBright;

	// 팻타입
	m_emPetType = sCrowBasic.m_emPetType;

	// 소환수 타입
	m_emSummonType = sCrowBasic.m_emSummonType;

	// NPC 타입
	m_emNPCType = NPC_TYPE_NORMAL;

	m_wLevel = sCrowBasic.m_wLevel;

	m_dwHP = sCrowBasic.m_dwHP;
	m_wMP = sCrowBasic.m_wMP;
	m_wSP = sCrowBasic.m_wSP;

	//	회복율.
	m_fIncHP = sCrowBasic.m_fIncHP;
	m_fIncMP = sCrowBasic.m_fIncMP;
	m_fIncSP = sCrowBasic.m_fIncSP;

	m_wAvoidRate = sCrowBasic.m_wAvoidRate;
	m_wHitRate = sCrowBasic.m_wHitRate;

	m_wViewRange = sCrowBasic.m_wViewRange;
	m_wDefense = sCrowBasic.m_wDefense;

	m_sResist = sCrowBasic.m_sResist;

	m_wBonusExp = sCrowBasic.m_wBonusExp;
	m_wBonusCP = sCrowBasic.m_wBonusCP;

	m_bOverlapAttack = sCrowBasic.m_bOverlapAttack;

	m_bNonDamaged = sCrowBasic.m_bNonDamaged;

	m_dwGenTime = sCrowBasic.m_dwGenTime;

	m_wGatherTimeLow = sCrowBasic.m_wGatherTimeLow;			
	m_wGatherTimeHigh = sCrowBasic.m_wGatherTimeHigh;			
	m_fGatherRate = sCrowBasic.m_fGatherRate;			
	m_wGatherAnimation = sCrowBasic.m_wGatherAnimation;

	m_bNoAllPenalty   = sCrowBasic.m_bNoAllPenalty;    
	m_dwNoPenaltyFlag = 0;

	m_emNPCType = sCrowBasic.m_emNPCType;

	m_bHideNaming = sCrowBasic.m_bHideNaming;
	m_bNonTargeting = sCrowBasic.m_bNonTargeting;

	m_fScale = sCrowBasic.m_fScale;
	m_bHideTooltip = sCrowBasic.m_bHideTooltip;
	return *this;
}

SCROWBASIC& SCROWBASIC::operator = ( const SCROWBASIC_120 &sCrowBasic )
{
	sNativeID = sCrowBasic.sNativeID;
	StringCbCopy( m_szName, CHAR_SZNAME, sCrowBasic.m_szName );
	m_emTribe = sCrowBasic.m_emTribe;
	m_emCrow = sCrowBasic.m_emCrow;
	m_emBright = sCrowBasic.m_emBright;

	// 팻타입
	m_emPetType = sCrowBasic.m_emPetType;

	// 소환수 타입
	m_emSummonType = sCrowBasic.m_emSummonType;

	// NPC 타입
	m_emNPCType = NPC_TYPE_NORMAL;

	m_wLevel = sCrowBasic.m_wLevel;

	m_dwHP = sCrowBasic.m_dwHP;
	m_wMP = sCrowBasic.m_wMP;
	m_wSP = sCrowBasic.m_wSP;

	//	회복율.
	m_fIncHP = sCrowBasic.m_fIncHP;
	m_fIncMP = sCrowBasic.m_fIncMP;
	m_fIncSP = sCrowBasic.m_fIncSP;

	m_wAvoidRate = sCrowBasic.m_wAvoidRate;
	m_wHitRate = sCrowBasic.m_wHitRate;

	m_wViewRange = sCrowBasic.m_wViewRange;
	m_wDefense = sCrowBasic.m_wDefense;

	m_sResist = sCrowBasic.m_sResist;

	m_wBonusExp = sCrowBasic.m_wBonusExp;
	m_wBonusCP = sCrowBasic.m_wBonusCP;

	m_bOverlapAttack = sCrowBasic.m_bOverlapAttack;

	m_bNonDamaged = sCrowBasic.m_bNonDamaged;

	m_dwGenTime = sCrowBasic.m_dwGenTime;

	m_wGatherTimeLow = sCrowBasic.m_wGatherTimeLow;			
	m_wGatherTimeHigh = sCrowBasic.m_wGatherTimeHigh;			
	m_fGatherRate = sCrowBasic.m_fGatherRate;			
	m_wGatherAnimation = sCrowBasic.m_wGatherAnimation;

	m_bNoAllPenalty   = sCrowBasic.m_bNoAllPenalty;    
	m_dwNoPenaltyFlag = 0;

	m_emNPCType = sCrowBasic.m_emNPCType;

	m_bHideNaming = sCrowBasic.m_bHideNaming;
	m_bNonTargeting = sCrowBasic.m_bNonTargeting;

	m_fScale = sCrowBasic.m_fScale;
	m_bHideTooltip = sCrowBasic.m_bHideTooltip;

	m_wResist_Probability = sCrowBasic.m_wResist_Probability;

	return *this;
}

SCROWBASIC& SCROWBASIC::operator = ( const SCROWBASIC_121 &sCrowBasic )
{
    sNativeID = sCrowBasic.sNativeID;
    StringCbCopy( m_szName, CHAR_SZNAME, sCrowBasic.m_szName );
    m_emTribe = sCrowBasic.m_emTribe;
    m_emCrow = sCrowBasic.m_emCrow;
    m_emBright = sCrowBasic.m_emBright;

    // 팻타입
    m_emPetType = sCrowBasic.m_emPetType;

    // 소환수 타입
    m_emSummonType = sCrowBasic.m_emSummonType;

    // NPC 타입
    m_emNPCType = NPC_TYPE_NORMAL;

    m_wLevel = sCrowBasic.m_wLevel;

    m_dwHP = sCrowBasic.m_dwHP;
    m_wMP = sCrowBasic.m_wMP;
    m_wSP = sCrowBasic.m_wSP;

    //	회복율.
    m_fIncHP = sCrowBasic.m_fIncHP;
    m_fIncMP = sCrowBasic.m_fIncMP;
    m_fIncSP = sCrowBasic.m_fIncSP;

    m_wAvoidRate = sCrowBasic.m_wAvoidRate;
    m_wHitRate = sCrowBasic.m_wHitRate;

    m_wViewRange = sCrowBasic.m_wViewRange;
    m_wDefense = sCrowBasic.m_wDefense;

    m_sResist = sCrowBasic.m_sResist;

    m_wBonusExp = sCrowBasic.m_wBonusExp;
    m_wBonusCP = sCrowBasic.m_wBonusCP;

    m_bOverlapAttack = sCrowBasic.m_bOverlapAttack;

    m_bNonDamaged = sCrowBasic.m_bNonDamaged;

    m_dwGenTime = sCrowBasic.m_dwGenTime;

    m_wGatherTimeLow = sCrowBasic.m_wGatherTimeLow;			
    m_wGatherTimeHigh = sCrowBasic.m_wGatherTimeHigh;			
    m_fGatherRate = sCrowBasic.m_fGatherRate;			
    m_wGatherAnimation = sCrowBasic.m_wGatherAnimation;

    m_bNoAllPenalty   = sCrowBasic.m_bNoAllPenalty;    
    m_dwNoPenaltyFlag = 0;

    m_emNPCType = sCrowBasic.m_emNPCType;

    m_bHideNaming = sCrowBasic.m_bHideNaming;
    m_bNonTargeting = sCrowBasic.m_bNonTargeting;

    m_fScale = sCrowBasic.m_fScale;
    m_bHideTooltip = sCrowBasic.m_bHideTooltip;

    m_wResist_Probability = sCrowBasic.m_wResist_Probability;

    m_fBarrierRadius = sCrowBasic.m_fBarrierRadius;
    m_fBarrierWidth = sCrowBasic.m_fBarrierWidth;

    return *this;
}

VOID SCROWBASIC::SaveCsvHead ( std::fstream &SFile )
{
	SFile << "sNativeID wMainID" << ",";
	SFile << "sNativeID wSubID" << ",";

	SFile << "szName" << ",";

//	기획팀 요청으로 제거함
//	SFile << "szName" << ",";

	SFile << "emTribe" << ",";
	SFile << "emCrow" << ",";
	SFile << "emBright" << ",";

	SFile << "wLevel" << ",";

	SFile << "dwHP" << ",";
	SFile << "wMP" << ",";
	SFile << "wSP" << ",";

	SFile << "fIncHP" << ",";
	SFile << "fIncMP" << ",";
	SFile << "fIncSP" << ",";

	SFile << "wAvoidRate" << ",";
	SFile << "wHitRate" << ",";

	SFile << "wViewRange" << ",";
	SFile << "wDefense" << ",";

	SFile << "sResist nFire" << ",";
	SFile << "sResist nIce" << ",";
	SFile << "sResist nElectric" << ",";
	SFile << "sResist nPoison" << ",";
	SFile << "sResist nSpirit" << ",";

	SFile << "wBonusExp" << ",";
	SFile << "wBonusCP" << ",";

	SFile << "bOverlapAttack" << ",";

	SFile << "m_bNonDamaged" << ",";

	// PET
	SFile << "emPetType" << ",";

	// SUMMON
	SFile << "m_emSummonType" << ",";

    SFile << "emNPCType" << ",";

	SFile << "dwGenTime" << ",";
	
	SFile << "wGatherTimeLow" << ",";
	SFile << "wGatherTimeHigh" << ",";
	SFile << "fGatherRate" << ",";
	SFile << "wGatherAnimation" << ",";

	SFile << "bNoAllPenalty" << ",";
	SFile << "dwNoPenalty"   << ",";

    SFile << "m_bHideNaming" << ",";
    SFile << "m_bNonTargeting" << ",";

    SFile << "m_fScale" << ",";

	SFile << "bHideTooltip" << ",";
	SFile << "wResist_Probability" << ",";

	SFile << "fBarrierRadius" << ",";
	SFile << "fBarrierWidth" << ",";

    SFile << "fInteractionRange" << ",";
}

VOID SCROWBASIC::SaveCsv ( std::fstream &SFile )
{
	SFile << sNativeID.wMainID << ",";
	SFile << sNativeID.wSubID << ",";

	std::string	strName = m_szName;
	STRUTIL::OutputStrCsv( SFile, strName );

//	기획팀 요청으로 제거함
/*
	std::string str = strName;
	const char* szpName = str.c_str();
	if( szpName )
	{
		const char* szpLongName = GLStringTable::GetInstance().GetString( szpName, GLStringTable::CROW );
		if( szpLongName )
			str = szpLongName;
	}
	STRUTIL::OutputStrCsv( SFile, str );
*/

	SFile << m_emTribe << ",";
	SFile << m_emCrow << ",";
	SFile << m_emBright << ",";

	SFile << m_wLevel << ",";

	SFile << m_dwHP << ",";
	SFile << m_wMP << ",";
	SFile << m_wSP << ",";

	SFile << m_fIncHP << ",";
	SFile << m_fIncMP << ",";
	SFile << m_fIncSP << ",";

	SFile << m_wAvoidRate << ",";
	SFile << m_wHitRate << ",";

	SFile << m_wViewRange << ",";
	SFile << m_wDefense << ",";

	SFile << m_sResist.nFire << ",";
	SFile << m_sResist.nIce << ",";
	SFile << m_sResist.nElectric << ",";
	SFile << m_sResist.nPoison << ",";
	SFile << m_sResist.nSpirit << ",";

	SFile << m_wBonusExp << ",";
	SFile << m_wBonusCP << ",";

	SFile << m_bOverlapAttack << ",";

	SFile << m_bNonDamaged << ",";

	// PET
	SFile << m_emPetType << ",";

	// SUMMON
	SFile << m_emSummonType << ",";

	// NPC
	SFile << m_emNPCType << ",";

	SFile << m_dwGenTime << ",";

	SFile << m_wGatherTimeLow << ",";
	SFile << m_wGatherTimeHigh << ",";
	SFile << m_fGatherRate << ",";
	SFile << m_wGatherAnimation << ",";

	SFile << m_bNoAllPenalty   << ",";
	SFile << m_dwNoPenaltyFlag << ",";

    SFile << m_bHideNaming << ",";
    SFile << m_bNonTargeting << ",";

    SFile << m_fScale << ",";
	SFile << m_bHideTooltip << ",";
	SFile << m_wResist_Probability << ",";

	SFile << m_fBarrierRadius << ",";
	SFile << m_fBarrierWidth << ",";

    SFile << m_fRefenceActionableRange << ",";
}

VOID SCROWBASIC::LoadCsv(CStringArray& StrArray, int& iCsvCur)
{
	iCsvCur = 0;

	sNativeID.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
	sNativeID.wSubID = (WORD)atoi( StrArray[ iCsvCur++ ] );

	std::string	strName;
	STRUTIL::InputStrCsv( StrArray[ iCsvCur++ ], strName );
	const char* pName = strName.c_str();
	StringCchCopy( m_szName, CHAR_SZNAME, pName );

//	기획팀 요청으로 제거함
//	++iCsvCur;	// Crow Name

	m_emTribe = (EMTRIBE)atoi( StrArray[ iCsvCur++ ] );
	m_emCrow = (EMCROW)atoi( StrArray[ iCsvCur++ ] );
	m_emBright = (EMBRIGHT)atoi( StrArray[ iCsvCur++ ] );

	m_wLevel = (WORD)atoi( StrArray[ iCsvCur++ ] );

	m_dwHP = (DWORD)atol( StrArray[ iCsvCur++ ] );
	m_wMP = (WORD)atoi( StrArray[ iCsvCur++ ] );
	m_wSP = (WORD)atoi( StrArray[ iCsvCur++ ] );

	m_fIncHP = (float)atof( StrArray[ iCsvCur++ ] );
	m_fIncMP = (float)atof( StrArray[ iCsvCur++ ] );
	m_fIncSP = (float)atof( StrArray[ iCsvCur++ ] );

	m_wAvoidRate = (WORD)atoi( StrArray[ iCsvCur++ ] );
	m_wHitRate = (WORD)atoi( StrArray[ iCsvCur++ ] );

	m_wViewRange = (WORD)atoi( StrArray[ iCsvCur++ ] );
	m_wDefense = (WORD)atoi( StrArray[ iCsvCur++ ] );

	m_sResist.nFire = (short)atoi( StrArray[ iCsvCur++ ] );
	m_sResist.nIce = (short)atoi( StrArray[ iCsvCur++ ] );
	m_sResist.nElectric = (short)atoi( StrArray[ iCsvCur++ ] );
	m_sResist.nPoison = (short)atoi( StrArray[ iCsvCur++ ] );
	m_sResist.nSpirit = (short)atoi( StrArray[ iCsvCur++ ] );

	m_wBonusExp = (DWORD)atoi( StrArray[ iCsvCur++ ] );
	m_wBonusCP = (WORD)atoi( StrArray[ iCsvCur++ ] );

	m_bOverlapAttack = (0!=atoi( StrArray[ iCsvCur++ ] ));

	m_bNonDamaged = (0!=atoi( StrArray[ iCsvCur++ ] ));

	// PET
	m_emPetType = (int)atoi( StrArray[ iCsvCur++ ] );

	// SUMMON
	m_emSummonType = (SUMMON_TYPE)atoi( StrArray[ iCsvCur++ ] );

	// NPC
	m_emNPCType = (NPC_TYPE)atoi( StrArray[ iCsvCur++ ] );

	m_dwGenTime = (DWORD)atoi( StrArray[ iCsvCur++ ] );

	m_wGatherTimeLow = (WORD)atoi( StrArray[ iCsvCur++ ] );
	m_wGatherTimeHigh = (WORD)atoi( StrArray[ iCsvCur++ ] );
	m_fGatherRate = (float)atof( StrArray[ iCsvCur++ ] );
	m_wGatherAnimation = (WORD)atoi( StrArray[ iCsvCur++ ] );

	m_bNoAllPenalty   = (0!=atoi( StrArray[ iCsvCur++ ] ));
	m_dwNoPenaltyFlag = (DWORD)atoi( StrArray[ iCsvCur++ ] );

    m_bHideNaming = (0!=atoi( StrArray[ iCsvCur++ ] ));
    m_bNonTargeting = (0!=atoi( StrArray[ iCsvCur++ ] ));

    m_fScale = (float)atof( StrArray[ iCsvCur++ ] );

	m_bHideTooltip = (0!=atoi( StrArray[ iCsvCur++ ] ));
	m_wResist_Probability = (WORD)atoi( StrArray[ iCsvCur++ ] );

	m_fBarrierRadius = (float)atof( StrArray[ iCsvCur++ ] );
	m_fBarrierWidth = (float)atof( StrArray[ iCsvCur++ ] );

    m_fRefenceActionableRange = (float)atof( StrArray[ iCsvCur++ ] );
}

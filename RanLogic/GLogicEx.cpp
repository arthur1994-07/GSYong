#include "pch.h"
#include "../SigmaCore/Math/Random.h"
#include "../EngineLib/G-Logic/GLPeriod.h"
#include "./Crow/GLCrowData.h"
#include "./Crow/CrowAttack.h"
#include "./GLogicEx.h"
#include "./Skill/GLSkill.h"
#include "./Item/GLItemMan.h"
#include "./GLogicData.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace GLOGICEX
{
	//	Note : 주위 환경에 따른 빛 / 어둠  캐릭터의 어드/디스 판단.
	EM_BRIGHT_FB GLSPACEGAP ( const EMBRIGHT emACTOR, const EMBRIGHT emRECEP, const EMBRIGHT emSPACE )
	{
		if ( emACTOR==emRECEP )		return BFB_AVER;

		if ( emSPACE==BRIGHT_LIGHT )
		{
			if ( emACTOR==BRIGHT_LIGHT && emRECEP==BRIGHT_DARK )	return BFB_ADV;
			if ( emACTOR==BRIGHT_DARK && emRECEP==BRIGHT_LIGHT )	return BFB_DIS;
		}
		else
		{
			if ( emACTOR==BRIGHT_LIGHT && emRECEP==BRIGHT_DARK )	return BFB_DIS;
			if ( emACTOR==BRIGHT_DARK && emRECEP==BRIGHT_LIGHT )	return BFB_ADV;
		}

		return BFB_AVER;
	}

	//	Note : 공격 성공율 계산.
	int GLHITRATE ( const int nHit, const int nAvoid, const EM_BRIGHT_FB bFB )
	{
		//	Note : 성공율 상수.
		const int nBirght[3] = { -10, 0, 10 };
		const int MAX_HIT(GLCONST_CHAR::nHitRateMaximum);
		const int MIN_HIT(GLCONST_CHAR::nHitRateMinimum);
		const int BASIC(100);

		int hitRate(BASIC + nHit - nAvoid + nBirght[bFB]);

		if ( hitRate > MAX_HIT )
			hitRate = MAX_HIT;
		else if ( hitRate < MIN_HIT )
			hitRate = MIN_HIT;

		return hitRate;
	}

	int GLHITRATE_RENEW(const int nHit, const int nAvoid, const INT nAtLv, const INT nDfLv)
	{
		// (((1+((nAtRate/30))*0.01))-((nDfRate/20)*0.01)+((nAtLevel/1.7-nDfLevel/1.7)*0.01))*100;

		const float fHit((float)nHit), fAvoid((float)nAvoid);
		INT nResult = ((1 + (fHit/GLCONST_CHAR::fHitRatePercentUnit)*0.01f)
			-((nAvoid/GLCONST_CHAR::fAvoidRatePercentUnit)*0.01f)
			+((nAtLv/GLCONST_CHAR::fAttackSuccesRate_LvScale-nDfLv/GLCONST_CHAR::fAttackSuccesRate_LvScale)*0.01f))*100.0f;

		nResult = min(GLCONST_CHAR::nHitRateMaximum, nResult);
		nResult = max(GLCONST_CHAR::nHitRateMinimum, nResult);
		return nResult;
	}

	// Note : Strock TRUE/FALSE 산출.	예) 적이 공격 동작시 취소가 되어 다시 동작을 취해야 한다.
	BOOL CHECKSHOCK ( const int nACTLEV, const int nDEFLEV, const int nDamage, const bool bCritical )
	{
		enum { CLEANHIT_RATE = 1, CRITICALHIT_RATE = 5, MIN_DXLEVEL = 5, MIN_DAMAGE = 6 };

		int nDXLEV = nDEFLEV - nACTLEV;
		if ( (-MIN_DXLEVEL) > nDXLEV )			return FALSE;
		if ( (nACTLEV+MIN_DAMAGE) > nDamage )	return FALSE;

		if ( bCritical )	return (CRITICALHIT_RATE>(sc::Random::RANDOM_POS()*100));
		return (CLEANHIT_RATE>(sc::Random::RANDOM_POS()*100));
	}

	//	Note : 방어력 , 주위 환경 속성에 따라 계산.
	int GLDEFENSE ( const int nDEFENSE, const EMBRIGHT emACTOR, const EMBRIGHT emRecep, const EMBRIGHT emSPACE )
	{
		float fFactor[BFB_SIZE] = { 0.8f, 1.0f, 1.2f };
		EM_BRIGHT_FB brightFB = GLSPACEGAP ( emACTOR, emRecep, emSPACE );

		return int(nDEFENSE*fFactor[brightFB]);
	}

	//	Note : 공격 수행시에 습득하는 경험치 습득량.
	int GLATTACKEXP(int nAttackerLev, int nDefenserLev, DWORD dwDamage, DWORD dwMaxHP, DWORD dwBonusExp)
	{
		using namespace GLCONST_CHAR;
		double fRate = static_cast<float>(dwDamage) / static_cast<double>(dwMaxHP);
		if (fRate > 1.0f)
            fRate = 1.0f;

		float fExpRate = GLCONST_CHAR::GETEXP_RATE(nAttackerLev, nDefenserLev);
		int nExp = int(dwBonusExp * fExpRate * fRate);
		return ((nExp < 0) ? 0 : nExp);
	}

	//	Note : 상대를 죽였을 경우 경험치 습득량.
	int	GLKILLEXP ( int nAttackerLev, int nDefenserLev, DWORD dwBonusExp )
	{
		float fExpRate = GLCONST_CHAR::GETEXP_RATE ( nAttackerLev, nDefenserLev );
		int nExp = int ( dwBonusExp * fExpRate * GLCONST_CHAR::fKILL_EXP_RATE );

		return ( ( nExp < 0 ) ? 0 : nExp );
	}

	//	죽었을때 경험치 감소.
	float GLDIE_DECEXP ( WORD wACTLEV )
	{
		WORD wSTEP = ( wACTLEV / 10 );
		if ( wSTEP >= GLCONST_CHAR::DIE_DECEXP_NUM )	wSTEP = GLCONST_CHAR::DIE_DECEXP_NUM-1;

		return GLCONST_CHAR::fDIE_DECEXP[wSTEP];
	}

	//	경험치 복구 비율
	float GLDIE_RECOVERYEXP ( WORD wACTLEV )
	{
		WORD wSTEP = ( wACTLEV / 10 );
		if ( wSTEP >= GLCONST_CHAR::DIE_DECEXP_NUM )	wSTEP = GLCONST_CHAR::DIE_DECEXP_NUM-1;

		return GLCONST_CHAR::fDIE_RECOVERYEXP[wSTEP];
	}

	//  경험치1당 돈
	float GLDIE_EXPMONEY ( WORD wACTLEV )
	{
		WORD wSTEP = ( wACTLEV / 10 );
		if ( wSTEP >= GLCONST_CHAR::DIE_DECEXP_NUM )	wSTEP = GLCONST_CHAR::DIE_DECEXP_NUM-1;

		return GLCONST_CHAR::fEXP_RATE_MONEY[wSTEP];
	}

	//	Note : 래밸업 하기 위해서 필요한 경험치 총량.
	LONGLONG GLNEEDEXP ( WORD wLev )
	{
		if ( wLev >= MAX_LEVEL )	
			return 0;

		return GLCONST_CHAR::lnEXP_MAX_TABLE[wLev];

		//WORD wLev1 = wLev+1;
		//WORD wLev2 = wLev+2;
		//return static_cast<int> ( ( wLev1*wLev + pow(wLev2,2) * ( wLev1 + GLCONST_CHAR::fLVL_EXP_G ) ) * GLCONST_CHAR::fLVL_EXP_S );
	}

	LONGLONG GLNEEDEXP2 ( WORD wLev )
	{
		if ( wLev >= MAX_LEVEL )	
			return 0;

		return GLCONST_CHAR::lnEXP_MAX_TABLE_2nd[wLev];
	}

	BOOL GLCALCULATEEXP ( WORD wCurLev, LONGLONG llCurExp, LONGLONG llCalExp, WORD &wExceptedLev, LONGLONG &llRemainExp, LONGLONG &llRemainMaxExp )
	{
		BOOL bRet = FALSE;
		LONGLONG llNextNeedExp = GLCONST_CHAR::lnEXP_MAX_TABLE[wCurLev] - llCurExp;

		while( llNextNeedExp < llCalExp )
		{
			llCalExp -= llNextNeedExp;
			wExceptedLev = ++wCurLev;
			llNextNeedExp = GLCONST_CHAR::lnEXP_MAX_TABLE[wCurLev];
			bRet = TRUE;
		}

		llRemainExp = llCalExp;
		llRemainMaxExp =  GLCONST_CHAR::lnEXP_MAX_TABLE[wExceptedLev];;

		return bRet;
	}

	BOOL GLCALCULATEEXP2 ( WORD wCurLev, LONGLONG llCurExp, LONGLONG llCalExp, WORD &wExceptedLev, LONGLONG &llRemainExp, LONGLONG &llRemainMaxExp )
	{
		BOOL bRet = FALSE;
		LONGLONG llNextNeedExp = GLCONST_CHAR::lnEXP_MAX_TABLE_2nd[wCurLev] - llCurExp;

		while( llNextNeedExp < llCalExp )
		{
			llCalExp -= llNextNeedExp;
			wExceptedLev = ++wCurLev;
			llNextNeedExp = GLCONST_CHAR::lnEXP_MAX_TABLE_2nd[wCurLev];
			bRet = TRUE;
		}

		llRemainExp = llCalExp;
		llRemainMaxExp =  GLCONST_CHAR::lnEXP_MAX_TABLE_2nd[wExceptedLev];;

		return bRet;
	}

	LONGLONG GLNEEDACCUMULATEDEXP ( WORD wCurLev, WORD wNeedLev )
	{
		LONGLONG accumulatedExp = 0;

		if ( wNeedLev >= MAX_LEVEL || wCurLev >= wNeedLev )	
			return 0;

		for( int i = (wCurLev - 1 ); i <= (wNeedLev - 1); i++ )
		{
			accumulatedExp += GLCONST_CHAR::lnEXP_MAX_TABLE[i];
		}
		// 마지막에 1을 더해주지 않으면 100%로 되기때문에 랩업시켜주기 위해서 1을 추가
		return accumulatedExp + 1;
	}

	LONGLONG GLNEEDACCUMULATEDEXP2 ( WORD wCurLev, WORD wNeedLev )
	{
		LONGLONG accumulatedExp = 0;

		if ( wNeedLev >= MAX_LEVEL || wCurLev >= wNeedLev )	
			return 0;

		for( int i = (wCurLev - 1 ); i <= (wNeedLev - 1); i++ )
		{
			accumulatedExp += GLCONST_CHAR::lnEXP_MAX_TABLE_2nd[i];
		}
		// 마지막에 1을 더해주지 않으면 100%로 되기때문에 랩업시켜주기 위해서 1을 추가
		return accumulatedExp + 1;
	}


	BOOL CHECKSTATEBLOW ( float fACTRATE, WORD wACTLEVEL, WORD wLEVEL, WORD wRESIST )
	{
		int nDXLEVEL = static_cast<int> ( wLEVEL - wACTLEVEL );

		int nINDEX = nDXLEVEL + GLCONST_CHAR::EMSTATEBLOW_LEVEL_BASE;
		if ( nINDEX < 0 )											nINDEX = 0;
		if ( nINDEX >= GLCONST_CHAR::EMSTATEBLOW_LEVEL_SIZE )		nINDEX = GLCONST_CHAR::EMSTATEBLOW_LEVEL_SIZE-1;

		return (sc::Random::RANDOM_POS()*100.0f) < ( fACTRATE - fACTRATE * 0.01f * wRESIST * 0.6f + GLCONST_CHAR::nSTATEBLOW_LEVEL[nINDEX] );
	}

	float SKILLDELAY ( DWORD dwSKILL_GRADE, WORD wSKILL_LEV, WORD wCHAR_LEVEL, float fDelay )
	{
		return static_cast<float>(dwSKILL_GRADE*wSKILL_LEV)/static_cast<float>(wCHAR_LEVEL) + fDelay;
	}

	float WEATHER_ELEMENT_POW ( EMELEMENT emElement, DWORD dwWeather, BOOL bWeatherActive )
	{
		float fPOWER = 1.0f;
		if ( !bWeatherActive )		return fPOWER;

		switch ( emElement )
		{
		case EMELEMENT_FIRE:
		case EMELEMENT_STONE:
			if ( dwWeather&FGW_RAIN )	fPOWER = 0.8f;
			if ( dwWeather&FGW_LEAVES )	fPOWER = 1.2f;
			break;

		case EMELEMENT_ICE:
		case EMELEMENT_MAD:
			if ( dwWeather&FGW_LEAVES )	fPOWER = 0.8f;
			if ( dwWeather&FGW_SNOW )	fPOWER = 1.2f;
			break;

		case EMELEMENT_ELECTRIC:
		case EMELEMENT_STUN:
			if ( dwWeather&FGW_SNOW )	fPOWER = 0.8f;
			if ( dwWeather&FGW_RAIN )	fPOWER = 1.2f;
			break;

		case EMELEMENT_POISON:
		case EMELEMENT_CURSE:
			fPOWER = 1.0f;
			break;
		};

		return fPOWER;
	}

	float WEATHER_BLOW_POW ( EMSTATE_BLOW emBlow, DWORD dwWeather, BOOL bWeatherActive )
	{
		float fPOWER = 1.0f;
		if ( !bWeatherActive )		return fPOWER;

		EMELEMENT emElement = STATE_TO_ELEMENT ( emBlow );
		return WEATHER_ELEMENT_POW ( emElement, dwWeather, bWeatherActive );
	}
    
    //! 이동속도
    float GetMoveBaseVelocity(float StateMove, float SkillMove, float ItemMove, float OptionMove)
    {
        float fMove = StateMove + SkillMove + ItemMove + OptionMove;
        return fMove < 0.01f ? 0.0f : fMove;
    }

    //! 공격속도
    float GetAttackVelocity(float AttackVelo, float fAddAttackVelo)
    {
        float fAttack = AttackVelo + fAddAttackVelo;
        return fAttack < 0.01f ? 0.0f : fAttack;
    }

    float GetMoveVelocity(
        BOOL bRun,
        float RunVelo,
        float WalkVelo,
        bool bUseBoost,
        float MoveVelo,
        float ItemVelo,
        float MoveVeloLimitMin,  // 최소 이동속도;
        float MoveVeloLimitMax,  // 최대 이동속도;
        bool bCdmSafe /*= false*/,
        float CdmSpeed /*= 1.0f*/)
    {
        const float fDefaultVelo = bRun ? RunVelo : WalkVelo;

        float fMoveVelo = bUseBoost ? 1.5f : 1.0f;
        fMoveVelo *= (MoveVelo + ItemVelo);

        if (bCdmSafe)
            fMoveVelo = fMoveVelo * CdmSpeed;

        if (MoveVeloLimitMin > 0.0f)
            fMoveVelo = max(fMoveVelo, MoveVeloLimitMin);

        fMoveVelo = min(fMoveVelo, MoveVeloLimitMax);
            
        return fMoveVelo * fDefaultVelo;
    }

}; // namespace GLOGICEX

EMANI_SUBTYPE CHECK_ANISUB (const SITEM* pRHAND, const SITEM* pLHAND, const EMCHARINDEX _emClass )
{
	EMANI_SUBTYPE emANISUBTYPE = AN_SUB_NONE;

	//	Note : 착용 아이템에 따른 에니메이션 종류.
	GLITEM_ATT emRHAtt = ITEMATT_NOTHING;
	GLITEM_ATT emLHAtt = ITEMATT_NOTHING;

	if ( pRHAND )		emRHAtt = pRHAND->sSuitOp.emAttack;
	if ( pLHAND )		emLHAtt = pLHAND->sSuitOp.emAttack;

	switch ( emRHAtt )
	{
	case ITEMATT_NOTHING:
		emANISUBTYPE = AN_SUB_NONE;
		break;

	case ITEMATT_SWORD:
		if ( pRHAND->sSuitOp.IsBOTHHAND() )
		{
			if ( pRHAND->sSuitOp.IsBIG() )		emANISUBTYPE = AN_SUB_BIGHSWORD;
			else								emANISUBTYPE = AN_SUB_TWOHSWORD;
		}
		else
		{
			emANISUBTYPE = AN_SUB_ONEHSWORD;
			if ( emLHAtt==ITEMATT_SWORD )		emANISUBTYPE = AN_SUB_EITHERSWORD;
		}
		break;

	case ITEMATT_DAGGER:
		emANISUBTYPE = AN_SUB_DAGGER;
		break;

	case ITEMATT_SPEAR:
		emANISUBTYPE = AN_SUB_SPEAR;
		break;

	case ITEMATT_BOW:
		emANISUBTYPE = AN_SUB_BOW;
		break;

	case ITEMATT_THROW:
		emANISUBTYPE = AN_SUB_THROW;
		break;

	case ITEMATT_STICK:
		emANISUBTYPE = AN_SUB_STICK;
		break;

	case ITEMATT_SABER:
		if ( pRHAND->sSuitOp.IsBOTHHAND() )
		{
			if ( pRHAND->sSuitOp.IsBIG() )		emANISUBTYPE = AN_SUB_BIGSABER;
			else								emANISUBTYPE = AN_SUB_TWOSABER;
		}
		else
		{
			emANISUBTYPE = AN_SUB_ONESABER;
			if ( emLHAtt==ITEMATT_SWORD )		emANISUBTYPE = AN_SUB_EITHERSABER;
		}
		break;

	case ITEMATT_GWON:
		emANISUBTYPE = AN_SUB_GWON;
		break;

	case ITEMATT_PISTOL:
		emANISUBTYPE = AN_SUB_PISTOL;
		break;

	case ITEMATT_RAILGUN:
		emANISUBTYPE = AN_SUB_RIFLE;
		break;

	case ITEMATT_PORTALGUN:
		emANISUBTYPE = AN_SUB_RIFLE;
		break;

    case ITEMATT_SIDE:
        emANISUBTYPE = AN_SUB_SIDE;
        break;

    case ITEMATT_DUALSPEAR:
        emANISUBTYPE = AN_SUB_DUALSPEAR;
        break;

    case ITEMATT_THROWING_KNIFE:
        emANISUBTYPE = AN_SUB_THROWING_KNIFE;
        break;

	case ITEMATT_EXTREME_GLOVE:
		emANISUBTYPE = AN_SUB_EXTREME_GLOVE;
		break;

	case ITEMATT_TRICK_STICK:
		emANISUBTYPE = AN_SUB_TRICK_STICK;
		break;
	case ITEMATT_TRICK_BOX:
		emANISUBTYPE = AN_SUB_TRICK_BOX;
		break;
	case ITEMATT_TRICK_WHIP:
		emANISUBTYPE = AN_SUB_TRICK_WHIP;
		break;

	case ITEMATT_ACTOR_SHIELD:
		emANISUBTYPE = AN_SUB_ACTOR_SHIELD;
		break;
	case ITEMATT_ACTOR_HAMMER:
		emANISUBTYPE = AN_SUB_ACTOR_HAMMER;
		break;
	case ITEMATT_ACTOR_UMBRELLA:
		emANISUBTYPE = AN_SUB_ACTOR_UMBRELLA;
		break;

	case ITEMATT_WEAPON_STR:
		{
			switch ( _emClass )
			{
			case GLCI_FIGHTER_M:
			case GLCI_FIGHTER_W:
				{
					emANISUBTYPE = AN_SUB_GWON;
				}
				break;

			case GLCI_ARMS_M:
			case GLCI_ARMS_W:
				{
					if ( pRHAND->sSuitOp.IsBOTHHAND() )
					{
						if ( pRHAND->sSuitOp.IsBIG() )		emANISUBTYPE = AN_SUB_BIGSABER;
						else								emANISUBTYPE = AN_SUB_TWOSABER;
					}
					else
					{
						emANISUBTYPE = AN_SUB_ONESABER;
						if ( emLHAtt==ITEMATT_SWORD )		emANISUBTYPE = AN_SUB_EITHERSABER;
					}
				}
				break;

			case GLCI_ARCHER_W:
			case GLCI_ARCHER_M:
				{
					emANISUBTYPE = AN_SUB_BOW;
				}
				break;

			case GLCI_SPIRIT_W:
			case GLCI_SPIRIT_M:
				{
					emANISUBTYPE = AN_SUB_SPEAR;
				}
				break;

			case GLCI_EXTREME_M:
			case GLCI_EXTREME_W:
				{
					emANISUBTYPE = AN_SUB_EXTREME_GLOVE;
				}
				break;
			
			case GLCI_SCIENTIST_M:
			case GLCI_SCIENTIST_W:
				{
					emANISUBTYPE = AN_SUB_RIFLE;
				}
				break;

			case GLCI_ASSASSIN_M:
			case GLCI_ASSASSIN_W:
				{
					emANISUBTYPE = AN_SUB_SIDE;
				}
				break;

			case GLCI_TRICKER_M:
			case GLCI_TRICKER_W:
				{
					emANISUBTYPE = AN_SUB_TRICK_WHIP;
				}
				break;

			case GLCI_ETC_M:
			case GLCI_ETC_W:
				{
					emANISUBTYPE = AN_SUB_NONE;
				}
				break;

			case GLCI_ACTOR_M:
			case GLCI_ACTOR_W:
				{
					emANISUBTYPE = AN_SUB_ACTOR_HAMMER;
				}
				break;
			}
		}
		break;

	case ITEMATT_WEAPON_DEX:
		{
			switch ( _emClass )
			{
			case GLCI_FIGHTER_M:
			case GLCI_FIGHTER_W:
				{
					emANISUBTYPE = AN_SUB_GWON;
				}
				break;

			case GLCI_ARMS_M:
			case GLCI_ARMS_W:
				{
					if ( pRHAND->sSuitOp.IsBOTHHAND() )
					{
						if ( pRHAND->sSuitOp.IsBIG() )		emANISUBTYPE = AN_SUB_BIGHSWORD;
						else								emANISUBTYPE = AN_SUB_TWOHSWORD;
					}
					else
					{
						emANISUBTYPE = AN_SUB_ONEHSWORD;
						if ( emLHAtt==ITEMATT_SWORD )		emANISUBTYPE = AN_SUB_EITHERSWORD;
					}
				}
				break;

			case GLCI_ARCHER_W:
			case GLCI_ARCHER_M:
				{
					emANISUBTYPE = AN_SUB_BOW;
				}
				break;

			case GLCI_SPIRIT_W:
			case GLCI_SPIRIT_M:
				{
					emANISUBTYPE = AN_SUB_SPEAR;
				}
				break;

			case GLCI_EXTREME_M:
			case GLCI_EXTREME_W:
				{
					emANISUBTYPE = AN_SUB_EXTREME_GLOVE;
				}
				break;

			case GLCI_SCIENTIST_M:
			case GLCI_SCIENTIST_W:
				{
					emANISUBTYPE = AN_SUB_PISTOL;
				}
				break;

			case GLCI_ASSASSIN_M:
			case GLCI_ASSASSIN_W:
				{
					emANISUBTYPE = AN_SUB_DUALSPEAR;
				}
				break;

			case GLCI_TRICKER_M:
			case GLCI_TRICKER_W:
				{
					emANISUBTYPE = AN_SUB_TRICK_BOX;
				}
				break;

			case GLCI_ETC_M:
			case GLCI_ETC_W:
				{
					emANISUBTYPE = AN_SUB_NONE;
				}
				break;

			case GLCI_ACTOR_M:
			case GLCI_ACTOR_W:
				{
					emANISUBTYPE = AN_SUB_ACTOR_SHIELD;
				}
				break;
			}
		}
		break;

	case ITEMATT_WEAPON_SPI:
		{
			switch ( _emClass )
			{
			case GLCI_FIGHTER_M:
			case GLCI_FIGHTER_W:
				{
					emANISUBTYPE = AN_SUB_GWON;
				}
				break;

			case GLCI_ARMS_M:
			case GLCI_ARMS_W:
				{
					if ( pRHAND->sSuitOp.IsBOTHHAND() )
					{
						if ( pRHAND->sSuitOp.IsBIG() )		emANISUBTYPE = AN_SUB_BIGHSWORD;
						else								emANISUBTYPE = AN_SUB_TWOHSWORD;
					}
					else
					{
						emANISUBTYPE = AN_SUB_ONEHSWORD;
						if ( emLHAtt==ITEMATT_SWORD )		emANISUBTYPE = AN_SUB_EITHERSWORD;
					}
				}
				break;

			case GLCI_ARCHER_W:
			case GLCI_ARCHER_M:
				{
					emANISUBTYPE = AN_SUB_BOW;
				}
				break;

			case GLCI_SPIRIT_W:
			case GLCI_SPIRIT_M:
				{
					emANISUBTYPE = AN_SUB_SPEAR;
				}
				break;

			case GLCI_EXTREME_M:
			case GLCI_EXTREME_W:
				{
					emANISUBTYPE = AN_SUB_EXTREME_GLOVE;
				}
				break;

			case GLCI_SCIENTIST_M:
			case GLCI_SCIENTIST_W:
				{
					emANISUBTYPE = AN_SUB_RIFLE;
				}
				break;

			case GLCI_ASSASSIN_M:
			case GLCI_ASSASSIN_W:
				{
					 emANISUBTYPE = AN_SUB_THROWING_KNIFE;
				}
				break;

			case GLCI_TRICKER_M:
			case GLCI_TRICKER_W:
				{
					emANISUBTYPE = AN_SUB_TRICK_STICK;
				}
				break;

			case GLCI_ETC_M:
			case GLCI_ETC_W:
				{
					emANISUBTYPE = AN_SUB_NONE;
				}
				break;

			case GLCI_ACTOR_M:
			case GLCI_ACTOR_W:
				{
					emANISUBTYPE = AN_SUB_ACTOR_UMBRELLA;
				}
				break;
			}
		}
		break;

	default:
		emANISUBTYPE = AN_SUB_NONE;
		break;
	};

	return emANISUBTYPE;
}

EMANI_SUBTYPE CHECK_ATTACK_ANISUB (const SITEM* pRHAND, const SITEM* pLHAND, const EMCHARINDEX _emClass )
{
	EMANI_SUBTYPE emANISUBTYPE = CHECK_ANISUB ( pRHAND, pLHAND, _emClass );

	bool bBROOM(false);
	if ( pRHAND )	bBROOM = pRHAND->sSuitOp.IsBROOM();

	if ( bBROOM )
		emANISUBTYPE = AN_SUB_BROOM;

	return emANISUBTYPE;
}

bool GLCROWLOGIC::VALIDACTION()
{
    DWORD dwSize = (DWORD)m_pCrowData->m_vecCrowAttack.size();
    for (DWORD i=0; i<dwSize; ++i)
    {
        if (m_pCrowData->m_vecCrowAttack[i].bUsed)
            return true;
    }
    return true;
}

bool GLCROWLOGIC::VALIDSKILL()
{
    DWORD dwSize = (DWORD) m_pCrowData->m_vecCrowAttack.size();
    for (DWORD i=0; i<dwSize; ++i)
    {
        if (m_pCrowData->m_vecCrowAttack[i].bUsed && 
            m_pCrowData->m_vecCrowAttack[i].skill_id!=SNATIVEID(false))
            return true;
    }
    return true;
}

bool GLCROWLOGIC::VALIDATTACK()
{
    DWORD dwSize = (DWORD) m_pCrowData->m_vecCrowAttack.size();
    for (DWORD i=0; i<dwSize; ++i)
    {
        if (m_pCrowData->m_vecCrowAttack[i].bUsed && 
            m_pCrowData->m_vecCrowAttack[i].skill_id==SNATIVEID(false))
            return true;
    }
    return true;
}

EMCROW GLCROWLOGIC::GETCROW() const
{ 
    return m_pCrowData->m_emCrow;
}

WORD GLCROWLOGIC::GETLEVEL() const
{ 
    return m_pCrowData->m_wLevel;
}

DWORD GLCROWLOGIC::GETHP() const
{ 
    return m_dwNowHP;
}

DWORD GLCROWLOGIC::GETMAXHP() const
{ 
    return m_pCrowData->m_dwHP * m_fMultipleHP;
}

float GLCROWLOGIC::GETMAXHPRATE() const
{
    return m_sLINKHP.fRef;
}

void GLCROWLOGIC::SETMAXHPRATE( float fRate ) 
{
    m_sLINKHP.fRef = fRate;
}

int GLCROWLOGIC::GETMP() const
{ 
    return m_wNowMP;
}

int GLCROWLOGIC::GETMAXMP() const
{ 
    return m_pCrowData->m_wMP;
}

int GLCROWLOGIC::GETSP() const
{ 
    return m_wNowSP;
}

int GLCROWLOGIC::GETMAXSP() const
{ 
    return m_pCrowData->m_wSP;
}

int GLCROWLOGIC::GETHIT() const
{ 
    return m_nSUM_HIT;
}

int GLCROWLOGIC::GETAVOID() const
{ 
    return m_nSUM_AVOID;
}

float GLCROWLOGIC::GETHPRESTORATIONRATE() const
{
	return m_fHPRestorationRate;
}

float GLCROWLOGIC::GETMPRESTORATIONRATE() const
{
	return m_fMPRestorationRate;
}

float GLCROWLOGIC::GETSPRESTORATIONRATE() const
{
	return m_fSPRestorationRate;
}

void GLCROWLOGIC::SetHPRestorationRatePlus( float fRate )
{
	float fTemp = m_fHPRestorationRate + m_fHPRestorationRatePlus;
	if ( fTemp >= 1.f )
	{
		m_fHPRestorationRatePlus = 1.f - m_fHPRestorationRate;
	}
	else if ( fTemp <= 0.f )
	{
		m_fHPRestorationRatePlus = -1 * m_fHPRestorationRate;
	}

	m_fHPRestorationRatePlus = fRate;
}

void GLCROWLOGIC::SetMPRestorationRatePlus( float fRate )
{
	float fTemp = m_fMPRestorationRate + m_fMPRestorationRatePlus;
	if ( fTemp >= 1.f )
	{
		m_fMPRestorationRatePlus = 1.f - m_fMPRestorationRate;
	}
	else if ( fTemp <= 0.f )
	{
		m_fMPRestorationRatePlus = -1 * m_fMPRestorationRate;
	}

	m_fMPRestorationRatePlus = fRate;
}

void GLCROWLOGIC::SetSPRestorationRatePlus( float fRate )
{
	float fTemp = m_fSPRestorationRate + m_fSPRestorationRatePlus;
	if ( fTemp >= 1.f )
	{
		m_fSPRestorationRatePlus = 1.f - m_fSPRestorationRate;
	}
	else if ( fTemp <= 0.f )
	{
		m_fSPRestorationRatePlus = -1 * m_fSPRestorationRate;
	}

	m_fSPRestorationRatePlus = fRate;
}

EMBRIGHT GLCROWLOGIC::GETBRIGHT() const
{ 
    return m_pCrowData->m_emBright;
}

int GLCROWLOGIC::GETDEFENSE() const
{ 
    return m_nSUM_DEFENSE;
}

int GLCROWLOGIC::GETSUM_PIERCE() const
{ 
    return m_nSUM_PIERCE;
}

WORD GLCROWLOGIC::GETBODYRADIUS() const
{ 
    return m_pCrowData->m_sAction.m_wBodyRadius;
}

WORD GLCROWLOGIC::GETATTACKRANGE() const
{ 
    return m_pCrowData->m_vecCrowAttack[m_dwAType].wRange;
}

void GLCROWLOGIC::RESETSKEFF(size_t i)
{
    if (i < CROW_SKILLFACT_SIZE)
        m_sSKILLFACT[i].sNATIVEID = NATIVEID_NULL();
}

void GLCROWLOGIC::DISABLEBLOW(int i)
{ 
    m_sSTATEBLOWS[i].emBLOW = EMBLOW_NONE;
}

void GLCROWLOGIC::GetEffList( DWORD dwSlot, BYTE* pSSPECA, BYTE* pSIMA )
{
	if( pSSPECA == NULL || pSIMA == NULL ) return;

	std::vector<SKILL::SIMPACT_ADDON_LEVEL>::iterator _iSater = m_sSKILLFACT[dwSlot].vecADDON.begin();

	int iCnt = 0;

	while( _iSater != m_sSKILLFACT[dwSlot].vecADDON.end())
	{
		pSIMA[iCnt++] = static_cast<BYTE>(_iSater->emADDON);

		++_iSater;
	}

	iCnt = 0;

	BOOL bInsert = TRUE;
	BOOL bCheck  = FALSE;

	BYTE iValue  = 0;

	PGLSKILL pSkill = NULL;

	if( RF_IMMUNE( m_EffSkillVarSet ).IsOn() ) 
	{
		pSkill = GLSkillMan::GetInstance().GetData ( m_sSKILLFACT[dwSlot].sNATIVEID );		

		if( pSkill && pSkill->m_sAPPLY.sDATA_LVL[m_sSKILLFACT[dwSlot].wLEVEL].fBASIC_VAR < 0 )
		{
			bCheck = TRUE;
		}
	}

	std::vector<SKILL::SSPEC_ADDON_LEVEL>::iterator _iSSater = m_sSKILLFACT[dwSlot].vecSPEC.begin();

	while( _iSSater != m_sSKILLFACT[dwSlot].vecSPEC.end())
	{
		bInsert = TRUE;

		iValue  = static_cast<BYTE>(_iSSater->emSPEC);

		if ( RF_IMMUNE( m_EffSkillVarSet ).IsImmune( iValue ) )
			bInsert = FALSE;

		if( bInsert && _iSSater->emSPEC == EMSPECA_ENTRANCE_EXIT ) {

			if( RF_ENTRANCE_EXIT( m_EffSkillVarSet ).IsOn() || IsBoss() || IsPosHold() )
			{
				bInsert = FALSE;
			}			
		}

		if( bInsert ) {
			pSSPECA[iCnt++] = iValue;

			++_iSSater;
		}
		else {
			_iSSater = m_sSKILLFACT[dwSlot].vecSPEC.erase( _iSSater );
		}

	}
}

SNATIVEID GLCROWLOGIC::GETACTIVESKILL()
{ 
    return m_idACTIVESKILL;
}

bool GLCROWLOGIC::IsDefenseSkill()
{ 
    return m_sDefenseSkill.m_bActive;
}

void GLCROWLOGIC::SetDefenseSkill(bool bDefense)
{ 
    m_sDefenseSkill.m_bActive = bDefense;
}

void GLCROWLOGIC::SetMultipleHP( float fMultiple )
{
    m_fMultipleHP = fMultiple;
}

float GLCROWLOGIC::GetMultipleHP() const
{
    return m_fMultipleHP;
}

#include "pch.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/DxEffect/Char/DxEffCharData.h"
#include "../../EngineLib/DxTools/DxMethods.h"
#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/RANPARAM.h"
#include "../Strike/GLStrikeM.h"

#include "../ClientActor.h"
#include "../GLGaeaClient.h"

#include "./GLFactEffect.h"


// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace FACTEFF
{
	void NewBlowEffect ( const STARGETID &_sTarID, DxSkinChar* _pSkinChar, EMSTATE_BLOW emBLOW, const D3DXMATRIX &_matTrans, const D3DXVECTOR3 &_vDir )
	{
		if ( !RANPARAM::bBuff )
            return;

        if( emBLOW < EMBLOW_NONE || emBLOW >= EMBLOW_SIZE )
            return;

		D3DXMATRIX matTrans, matYRot;
		D3DXMatrixRotationY ( &matYRot, D3DX_PI/2.0f );
		matTrans = matYRot * _matTrans;

		const std::string strCUREFFECT = GLCONST_CHAR::strBLOW_EFFECTS[emBLOW];
		const std::string strCURBODYEFFECT = GLCONST_CHAR::strBLOW_BODY_EFFECTS[emBLOW];

		DxEffGroupPlayer::GetInstance().PassiveEffect ( strCUREFFECT.c_str(), matTrans, _sTarID );
		DxEffcharDataMan::GetInstance().PutPassiveEffect ( _pSkinChar, strCURBODYEFFECT.c_str(), &_vDir );
	}

	void DeleteBlowEffect ( const STARGETID &_sTarID, DxSkinChar* _pSkinChar, EMSTATE_BLOW emBLOW )
	{
		if( emBLOW < EMBLOW_NONE || emBLOW >= EMBLOW_SIZE )
            return;

		std::string strCUREFFECT = GLCONST_CHAR::strBLOW_EFFECTS[emBLOW];
		std::string strCURBODYEFFECT = GLCONST_CHAR::strBLOW_BODY_EFFECTS[emBLOW];

		DxEffGroupPlayer::GetInstance().DeletePassiveEffect ( strCUREFFECT.c_str(), _sTarID );
		DxEffcharDataMan::GetInstance().OutEffect ( _pSkinChar, strCURBODYEFFECT.c_str() );
	}

	void DeleteBlowSingleEffect ( const STARGETID &_sTarID, DxSkinChar* _pSkinChar, SSTATEBLOW *_sSTATEBLOWS )
	{
		int i=0;
		for ( i=0; i<EMBLOW_MULTI; ++i )
		{
			SSTATEBLOW &sSTATEBLOW = _sSTATEBLOWS[i];
			if ( sSTATEBLOW.emBLOW == EMBLOW_NONE )		continue;

			DeleteBlowEffect ( _sTarID, _pSkinChar, sSTATEBLOW.emBLOW );
		}
	}

	void NewSkillFactEffect ( const STARGETID &_sTarID, DxSkinChar* _pSkinChar, const SNATIVEID &nidSKILL, const D3DXMATRIX &_matTrans, const D3DXVECTOR3 &_vDir )
	{
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( nidSKILL );
		if ( !pSkill )				return;

        // 버프표시 옵션이 꺼져있으면서 버프표시 옵션에 영향을 받는 스킬일 경우 이펙트 생성 안함;
        const bool bForceVisible(pSkill->m_sBASIC.dwFlags & FLAG_SB_EFFECT_FORCE_VISIBLE);
        if ( ( !RANPARAM::bBuff ) && ( !bForceVisible ) )
            return;

		D3DXMATRIX matTrans, matYRot;
		D3DXMatrixRotationY ( &matYRot, D3DX_PI/2.0f );
		matTrans = matYRot * _matTrans;

		EMELEMENT emELMT(EMELEMENT_SPIRIT);

		SKILL::SEXT_DATA &sEXT_DATA = pSkill->m_sEXT_DATA;
		if ( sEXT_DATA.emTARGZONE01 == SKILL::EMTIME_PASSIVE )
		{
			DxEffGroupPlayer::GetInstance().PassiveEffect ( sEXT_DATA.GETTARGZONE01(emELMT), matTrans, _sTarID );
		}

		if ( sEXT_DATA.emTARGZONE02 == SKILL::EMTIME_PASSIVE )
		{
			DxEffGroupPlayer::GetInstance().PassiveEffect ( sEXT_DATA.GETTARGZONE02(emELMT), matTrans, _sTarID );
		}

		if ( sEXT_DATA.emTARGZONE03 == SKILL::EMTIME_PASSIVE )
		{
			DxEffGroupPlayer::GetInstance().PassiveEffect ( sEXT_DATA.GETTARGZONE03(emELMT), matTrans, _sTarID );
		}

		if ( sEXT_DATA.emTARGBODY01 == SKILL::EMTIME_PASSIVE )
		{
			DxEffcharDataMan::GetInstance().PutPassiveEffect ( _pSkinChar, sEXT_DATA.GETTARGBODY01(emELMT), &_vDir );
		}

		if ( sEXT_DATA.emTARGBODY02 == SKILL::EMTIME_PASSIVE )
		{
			DxEffcharDataMan::GetInstance().PutPassiveEffect ( _pSkinChar, sEXT_DATA.GETTARGBODY02(emELMT), &_vDir );
		}
	}

	void ___NewSkillEffect_TargetZone(
		GLGaeaClient* pGaeaClient
		, const STARGETID& sTIDFrom
		, const STARGETID& sTIDTo
		, const SKILL::EMEFFECTPOS emPOS
		, const char* const szZONE_EFF)
	{
		const D3DXVECTOR3 vDir(sTIDTo.vPos - sTIDFrom.vPos);
		const float fDir_Y(DXGetThetaYFromDirection ( vDir, D3DXVECTOR3(1.f, 0.f, 0.f) ));

		D3DXMATRIX mxTrans;
		D3DXMatrixRotationY ( &mxTrans, fDir_Y );

		STARGETID sTarget(sTIDTo);
		//	스트라이크 지점에서;
		BOOL bTRANS(FALSE);
		if ( emPOS == SKILL::EMPOS_STRIKE )
		{
			DxSkinChar* pSkin = pGaeaClient->GetSkinChar ( sTarget );

			//	타겟의 위치;
			D3DXVECTOR3 vTARPOS ( sTIDFrom.vPos.x, sTIDFrom.vPos.y+15, sTIDFrom.vPos.z );;

			//	타격 위치를 알아냄;
			IMPACT_ARROW::SSTRIKE sStrike;
			IMPACT_ARROW::CSELECTOR Selector ( vTARPOS, pSkin->m_PartArray );
			BOOL bOk = Selector.SELECT ( sStrike );
			if ( bOk )
			{
				bTRANS = TRUE;
				mxTrans._41 = sStrike.vPos.x;
				mxTrans._42 = sStrike.vPos.y;
				mxTrans._43 = sStrike.vPos.z;
				sTarget.vPos = vTARPOS;
			}
		}

		//	지면기준 목표점;
		if ( !bTRANS )
		{
			bTRANS = TRUE;
			mxTrans._41 = sTIDFrom.vPos.x;
			mxTrans._42 = sTIDFrom.vPos.y;
			mxTrans._43 = sTIDFrom.vPos.z;
		}

		//	Note : 목표 위치 이펙트 발생시킴.
		DxEffGroupPlayer::GetInstance().NewEffGroup ( szZONE_EFF, mxTrans, &sTarget, TRUE, TRUE );
	}
	void NewSkillEffect_AppliPoint(
		GLGaeaClient* pGaeaClient
		, const STARGETID& sTIDFrom
		, const STARGETID& sTIDTo
		, const SNATIVEID& sSKILLNID )
	{
		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSKILLNID );
		if ( !pSkill )
			return;

		if ( pSkill->m_sBASIC.emACTION_TYPE != SKILL::EMACTION_TYPE_BUFF &&
			pSkill->m_sBASIC.emACTION_TYPE != SKILL::EMACTION_TYPE_DEBUFF )
			return;

		// 버프표시 옵션이 꺼져있으면서 버프표시 옵션에 영향을 받는 스킬일 경우 이펙트 생성 안함;
		const bool bForceVisible(pSkill->m_sBASIC.dwFlags & FLAG_SB_EFFECT_FORCE_VISIBLE);
		if ( ( !RANPARAM::bBuff ) && ( !bForceVisible ) )
			return;

		if ( pSkill->m_sEXT_DATA.emTARGZONE01 == SKILL::EMTIME_EFFECT || 
			pSkill->m_sEXT_DATA.emTARGZONE01 == SKILL::EMTIME_IMPACT )
		{
			___NewSkillEffect_TargetZone( pGaeaClient, sTIDFrom, sTIDTo
				, pSkill->m_sEXT_DATA.emTARGZONE01_POS
				, pSkill->m_sEXT_DATA.GETTARGZONE01(EMELEMENT_SPIRIT) );
		}

		if ( pSkill->m_sEXT_DATA.emTARGZONE02 == SKILL::EMTIME_EFFECT ||
			pSkill->m_sEXT_DATA.emTARGZONE02 == SKILL::EMTIME_IMPACT )
		{
			___NewSkillEffect_TargetZone( pGaeaClient, sTIDFrom, sTIDTo
				, pSkill->m_sEXT_DATA.emTARGZONE02_POS
				, pSkill->m_sEXT_DATA.GETTARGZONE02(EMELEMENT_SPIRIT) );
		}

		if ( pSkill->m_sEXT_DATA.emTARGZONE03 == SKILL::EMTIME_EFFECT ||
			pSkill->m_sEXT_DATA.emTARGZONE03 == SKILL::EMTIME_IMPACT )
		{
			___NewSkillEffect_TargetZone( pGaeaClient, sTIDFrom, sTIDTo
				, pSkill->m_sEXT_DATA.emTARGZONE03_POS
				, pSkill->m_sEXT_DATA.GETTARGZONE03(EMELEMENT_SPIRIT) );
		}

		//	Note : 목표 몸 이펙트.
//		if( pSkill->m_sEXT_DATA.emTARGBODY01 == SKILL::EMTIME_IMPACT )
//			DxEffGroupPlayer::GetInstance().NewEffBody ( m_pSkillData->m_sEXT_DATA.GETTARGBODY01(emELMT), &sTarget, &vDir );
//
//		if( pSkill->m_sEXT_DATA.emTARGBODY02 == SKILL::EMTIME_IMPACT )
//			DxEffGroupPlayer::GetInstance().NewEffBody ( m_pSkillData->m_sEXT_DATA.GETTARGBODY02(emELMT), &sTarget, &vDir );
	}

	void DeleteSkillFactEffect( const STARGETID &_sTarID, DxSkinChar* _pSkinChar, const SNATIVEID &nidSKILL )
	{
		PGLSKILL	pSkill = GLSkillMan::GetInstance().GetData ( nidSKILL );
		if( !pSkill )
			return;

		EMELEMENT			emELMT( EMELEMENT_SPIRIT );
		SKILL::SEXT_DATA&	sEXT_DATA = pSkill->m_sEXT_DATA;
		if( sEXT_DATA.emTARGZONE01 == SKILL::EMTIME_PASSIVE )
			DxEffGroupPlayer::GetInstance().DeletePassiveEffect( sEXT_DATA.GETTARGZONE01(emELMT), _sTarID );

		if( sEXT_DATA.emTARGZONE02 == SKILL::EMTIME_PASSIVE )
			DxEffGroupPlayer::GetInstance().DeletePassiveEffect( sEXT_DATA.GETTARGZONE02(emELMT), _sTarID );

		if( sEXT_DATA.emTARGZONE03 == SKILL::EMTIME_PASSIVE )
			DxEffGroupPlayer::GetInstance().DeletePassiveEffect( sEXT_DATA.GETTARGZONE03(emELMT), _sTarID );

		if( sEXT_DATA.emTARGBODY01 == SKILL::EMTIME_PASSIVE )
			DxEffcharDataMan::GetInstance().OutEffect( _pSkinChar, sEXT_DATA.GETTARGBODY01(emELMT) );

		if( sEXT_DATA.emTARGBODY02 == SKILL::EMTIME_PASSIVE )
			DxEffcharDataMan::GetInstance().OutEffect( _pSkinChar, sEXT_DATA.GETTARGBODY02(emELMT) );
	} //DeleteSkillFactEffect

	HRESULT UpdateSkillEffect( GLGaeaClient*		pGaeaClient,
							   const STARGETID&		_sTarID,
							   DxSkinChar*			_pSkinChar,
							   SSKILLFACT*			_sSKILLFACT,
							   const UINT			_nSKILLFACTSIZE,
							   SSTATEBLOW*			_sSTATEBLOWS,
							   BOOL					IsPrintMsg )
	{		
		//	Note : 스킬이팩트가 사라지는 경우 이팩트를 지워줌.
		//
		ClientActor* _pActor = pGaeaClient->GetCopyActor(_sTarID);		

		for( UINT i = 0; i < _nSKILLFACTSIZE; ++i )
		{
			SSKILLFACT &sSKILLFACT = _sSKILLFACT[i];
			if (sSKILLFACT.sNATIVEID==SNATIVEID(false))
                continue;

            if ( sSKILLFACT.IsHaveExpiredTime() )
            {
                __time64_t tTime = pGaeaClient->GetCurrentTime().GetTime();

                if ( static_cast<UINT>(tTime) < sSKILLFACT.nEXPIREDTIME )
                    continue;
            }
            else if ( 0.0f < sSKILLFACT.fAGE )
            {
                continue;
            } //if..else

			if ( IsPrintMsg )
			{
				EMSKILLFACT_TYPE emType = EMSKILLFACTTYPE_BY_INDEX(i);

				if( emType == EMSKILLFACT_TYPE_LUNCHBOX )
				{
					PGLSKILL pSkill = GLSkillMan::GetInstance().GetData( sSKILLFACT.sNATIVEID );
					if ( !pSkill )
						break;
					
					pGaeaClient->PrintMsgText(
                        NS_UITEXTCOLOR::RED,
                        sc::string::format(ID2GAMEINTEXT("LUNCHBOX_EXTINCTION"), pSkill->GetName()));
				}  //if

				if( emType == EMSKILLFACT_TYPE_EXP_LUNCHBOX )
				{
					continue;

				}

			} //if

			_pActor->UPDATE_DATA_FINISH(i);
		} //for

		//	Note : 상태이상 이팩트가 사라지는 경우를 확인하고 이팩트를 지워줌.
		//
		for ( UINT i=0; i<EMBLOW_MULTI; ++i )
		{
			SSTATEBLOW &sSTATEBLOW = _sSTATEBLOWS[i];
			if ( sSTATEBLOW.emBLOW == EMBLOW_NONE )		continue;

			if ( sSTATEBLOW.fAGE <= 0.0f )
			{
				DeleteBlowEffect ( _sTarID, _pSkinChar, sSTATEBLOW.emBLOW );
				
				sSTATEBLOW.emBLOW = EMBLOW_NONE;
			}
		}

		return S_OK;
	}

	HRESULT ReNewEffect(const STARGETID &_sTarID, DxSkinChar* _pSkinChar, SSKILLFACT *_sSKILLFACT, const UINT _nSKILLFACTSIZE, SSTATEBLOW *_sSTATEBLOWS, const D3DXMATRIX &_matTrans, const D3DXVECTOR3 &_vDir)
	{
		//	Note : 스킬이팩트를 확인하고 없을때는 이팩트를 만들어줌.
		//
		for (UINT i=0; i<_nSKILLFACTSIZE; ++i)
		{
            const SSKILLFACT& sSKILLFACT = _sSKILLFACT[i];
            if (sSKILLFACT.sNATIVEID != SNATIVEID(false))
                NewSkillFactEffect(_sTarID, _pSkinChar, sSKILLFACT.sNATIVEID, _matTrans, _vDir);
		}

        // 버프표시 옵션이 꺼져있는경우 스킬의 경우를 제외한 나머지의 이펙트 생성 안함 [9/5/2014 hsshin];
        if ( !RANPARAM::bBuff )	return S_OK;

		//	Note : 상태이상 이팩트를 확인하고 없을때는 이팩트를 만들어줌.
		//
		for ( UINT i=0; i<EMBLOW_MULTI; ++i )
		{
			SSTATEBLOW &sSTATEBLOW = _sSTATEBLOWS[i];
			if ( sSTATEBLOW.emBLOW == EMBLOW_NONE )		continue;
		
			NewBlowEffect ( _sTarID, _pSkinChar, sSTATEBLOW.emBLOW, _matTrans, _vDir );
		}

		return S_OK;
	}

	HRESULT DeleteEffect ( const STARGETID &_sTarID, DxSkinChar* _pSkinChar, SSKILLFACT *_sSKILLFACT, const UINT _nSKILLFACTSIZE, SSTATEBLOW *_sSTATEBLOWS )
	{
		for ( UINT i=0; i<_nSKILLFACTSIZE; ++i )
		{
            SSKILLFACT &sSKILLFACT = _sSKILLFACT[i];
			if ( sSKILLFACT.sNATIVEID==SNATIVEID(false) )
                continue;

			PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sSKILLFACT.sNATIVEID.wMainID, sSKILLFACT.sNATIVEID.wSubID );
			if ( !pSkill )
                return FALSE;

			EMELEMENT emELMT(EMELEMENT_SPIRIT);

			SKILL::SEXT_DATA &sEXT_DATA = pSkill->m_sEXT_DATA;
			if ( sEXT_DATA.emTARGZONE01 == SKILL::EMTIME_PASSIVE )
			{
				DxEffGroupPlayer::GetInstance().DeletePassiveEffect ( sEXT_DATA.GETTARGZONE01(emELMT), _sTarID );
			}

			if ( sEXT_DATA.emTARGZONE02 == SKILL::EMTIME_PASSIVE )
			{
				DxEffGroupPlayer::GetInstance().DeletePassiveEffect ( sEXT_DATA.GETTARGZONE02(emELMT), _sTarID );
			}

			if ( sEXT_DATA.emTARGZONE03 == SKILL::EMTIME_PASSIVE )
			{
				DxEffGroupPlayer::GetInstance().DeletePassiveEffect ( sEXT_DATA.GETTARGZONE03(emELMT), _sTarID );
			}

			if ( sEXT_DATA.emTARGBODY01 == SKILL::EMTIME_PASSIVE )
			{
				DxEffcharDataMan::GetInstance().OutEffect ( _pSkinChar, sEXT_DATA.GETTARGBODY01(emELMT) );
			}

			if ( sEXT_DATA.emTARGBODY02 == SKILL::EMTIME_PASSIVE )
			{
				DxEffcharDataMan::GetInstance().OutEffect ( _pSkinChar, sEXT_DATA.GETTARGBODY02(emELMT) );
			}
		}

		for ( UINT i=0; i<EMBLOW_SIZE; ++i )
		{
			DeleteBlowEffect ( _sTarID, _pSkinChar, (EMSTATE_BLOW)i );
		}

		return TRUE;
	}

	HRESULT DeleteAllEffect ( const STARGETID &_sTarID, DxSkinChar* _pSkinChar, SSKILLFACT *_sSKILLFACT, const UINT _nSKILLFACTSIZE, SSTATEBLOW *_sSTATEBLOWS )
	{
		DeleteEffect ( _sTarID, _pSkinChar, _sSKILLFACT, _nSKILLFACTSIZE, _sSTATEBLOWS );

		DxEffGroupPlayer::GetInstance().DeletePassiveEffect ( _sTarID );

		return S_OK;
	}

};

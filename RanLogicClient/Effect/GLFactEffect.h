#ifndef GLFACTEFFECT_H_
#define GLFACTEFFECT_H_

#pragma once

#include "../../RanLogic/Character/GLCharData.h"
#include "../../RanLogic/Character/GLCharDefine.h"
#include "../../EngineLib/G-Logic/GLDefine.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinChar.h"

class GLGaeaClient;

namespace FACTEFF
{
	void NewBlowEffect(
        const STARGETID& _sTarID,
        DxSkinChar* _pSkinChar,
        EMSTATE_BLOW emBLOW,
        const D3DXMATRIX& _matTrans,
        const D3DXVECTOR3& _vDir);

	void DeleteBlowEffect(
        const STARGETID& _sTarID,
        DxSkinChar* _pSkinChar,
        EMSTATE_BLOW emBLOW);

	void DeleteBlowSingleEffect(
        const STARGETID& _sTarID,
        DxSkinChar* _pSkinChar,
        SSTATEBLOW* _sSTATEBLOWS);

	void NewSkillFactEffect(
		const STARGETID& _sTarID,
		DxSkinChar* _pSkinChar,
		const SNATIVEID& nidSKILL,
		const D3DXMATRIX& _matTrans,
		const D3DXVECTOR3& _vDir);

	// SKILL::EMTIME_EFFECT(적용시점)인 경우 스킬 효과를 받을때 이펙트를 출력한다;
	void NewSkillEffect_AppliPoint(
		GLGaeaClient* pGaeaClient
		, const STARGETID& sTIDFrom
		, const STARGETID& sTIDTo
		, const SNATIVEID& sSKILLNID );

	void DeleteSkillFactEffect(
        const STARGETID& _sTarID,
        DxSkinChar* _pSkinChar,
        const SNATIVEID& nidSKILL);

	HRESULT UpdateSkillEffect(
        GLGaeaClient* pGaeaClient,
        const STARGETID& _sTarID,
        DxSkinChar* _pSkinChar,
        SSKILLFACT* _sSKILLFACT,
        const UINT _nSKILLFACTSIZE,
        SSTATEBLOW* _sSTATEBLOWS,
        BOOL IsPrintMsg=FALSE);

	HRESULT ReNewEffect(
        const STARGETID& _sTarID,
        DxSkinChar* _pSkinChar,
        SSKILLFACT* _sSKILLFACT,
        const UINT _nSKILLFACTSIZE,
        SSTATEBLOW* _sSTATEBLOWS,
        const D3DXMATRIX& _matTrans,
        const D3DXVECTOR3& _vDir);

	HRESULT DeleteEffect(
        const STARGETID& _sTarID,
        DxSkinChar* _pSkinChar,
        SSKILLFACT* _sSKILLFACT,
        const UINT _nSKILLFACTSIZE,
        SSTATEBLOW* _sSTATEBLOWS);

	HRESULT DeleteAllEffect(
        const STARGETID& _sTarID,
        DxSkinChar* _pSkinChar,
        SSKILLFACT* _sSKILLFACT,
        const UINT _nSKILLFACTSIZE,
        SSTATEBLOW* _sSTATEBLOWS);
};

#endif // GLFACTEFFECT_H_
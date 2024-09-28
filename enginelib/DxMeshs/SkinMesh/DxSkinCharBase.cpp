#include "pch.h"

#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/Loading/NSLoadingDirect.h"
#include "../../../SigmaCore/Log/LogMan.h"

#include "../../../VisualMaterialLib/Util/NSNearPointLight.h"

#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../DxTools/IrradianceVolume/NSLightMapAmbientColor.h"
#include "../../DxTools/Light/DxLightMan.h"
#include "../../DxTools/DxInputDevice.h"
#include "../../DxTools/RENDERPARAM.h"
#include "../../DxTools/DxDynamicVB.h"
#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/EDITMESHS.h"
#include "../../DxTools/COLLISION.h"
#include "../../G-Logic/GLPeriod.h"
#include "../../DxLand/DxLandMan.h"
#include "./DxSkinAniControlData.h"
#include "./DxSkinMeshManager.h"
#include "./DxSkinAnimation.h"
#include "./SANIMCONINFO.h"
#include "./DxSkinDefine.h"
#include "./DxJiggleBone.h"

#include "DxSkinChar.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


//////////////////////////////////////////////////////////////////////////
DxSkinCharBase::DxSkinCharBase(void)
	: m_vDiffuseColor(1.f,1.f,1.f)
	, m_vPointLightColor(0.f,0.f,0.f)
{
}

DxSkinCharBase::~DxSkinCharBase(void)
{
}

void DxSkinCharBase::SetWorldLight( const D3DXVECTOR3 &vDiffuseColor, const D3DXVECTOR3 &vPointLight )
{
	m_vDiffuseColor = vDiffuseColor;
	m_vPointLightColor = vPointLight;
}



//////////////////////////////////////////////////////////////////////////
DxSkinCharEffAniBase::DxSkinCharEffAniBase(void)
{
}

DxSkinCharEffAniBase::~DxSkinCharEffAniBase(void)
{
}


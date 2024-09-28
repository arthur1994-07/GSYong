#include "../pch.h"
#include "../../SigmaCore/File/SerialFile.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/DxTools/EditMeshs.h"
#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../EngineLib/DxTools/DebugSet.h"

#include "./GLLandMark.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLLANDMARK::GLLANDMARK()
    : m_dwID(0)
	, m_nKeyWordIndex(0)
	, m_dwColor(NS_UITEXTCOLOR::ORANGE)
	, m_vMax(0,0,0)
    , m_vMin(0,0,0)
    //, m_pNext(NULL)
{
	UseAffineMatrix();
}

GLLANDMARK::~GLLANDMARK()
{
	//m_pNext = NULL;
}

const D3DXVECTOR3 GLLANDMARK::m_vHIGHER = D3DXVECTOR3(0,4,0);
const float GLLANDMARK::m_fSPHERE_S1 = 3.0f;

void GLLANDMARK::operator= ( const GLLANDMARK &sVALUE )
{
	DXAFFINEMATRIX::SetAffineValue ( sVALUE.m_pAffineParts );

	m_strMARK = sVALUE.m_strMARK;
	m_dwColor = sVALUE.m_dwColor;
	m_dwID = sVALUE.m_dwID;

	CALC_BOX ();
}

void GLLANDMARK::CALC_BOX ()
{
	m_vMax = m_vMin = m_pAffineParts->vTrans;
	
	m_vMax += D3DXVECTOR3 ( 10, 20, 10 );
	m_vMin += D3DXVECTOR3 ( -10, 0, -10 );
}

void GLLANDMARK::ConvertName()
{
	m_strConvertName = ID2GAMEINTEXT( m_strMARK.c_str(), m_nKeyWordIndex );
}

HRESULT GLLANDMARK::RenderEdit( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV )
{
	HRESULT hr = S_OK;
	if ( !COLLISION::IsCollisionVolume(*pCV,m_vMax,m_vMin) )	return S_FALSE;

	if ( pd3dDevice )
	{
		D3DXVECTOR3 vOldPos = DXAFFINEMATRIX::m_pAffineParts->vTrans + m_vHIGHER;
		EDITMESHS::RENDERSPHERE ( pd3dDevice, vOldPos, m_fSPHERE_S1 );

		D3DXVECTOR3 vScr0 = DxViewPort::GetInstance().ComputeVec3Project ( &vOldPos, NULL );
		if ( m_strConvertName.empty() )
		{
			CDebugSet::ToPos ( vScr0.x, vScr0.y, m_strMARK.c_str() );
		}
		else
		{
			CDebugSet::ToPos ( vScr0.x, vScr0.y, m_strConvertName.c_str() );
		}
	}

	return S_OK;
}
//
//HRESULT GLLANDMARK::Render ( LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME *pCV )
//{
//	HRESULT hr = S_OK;
//	if ( !COLLISION::IsCollisionVolume(*pCV,m_vMax,m_vMin) )	return S_FALSE;
//
//	if ( pd3dDevice )
//	{
//		D3DXVECTOR3 vOldPos = DXAFFINEMATRIX::m_pAffineParts->vTrans + m_vHIGHER;
//		EDITMESHS::RENDERSPHERE ( pd3dDevice, vOldPos, m_fSPHERE_S1 );
//
//		D3DXVECTOR3 vScr0 = DxViewPort::GetInstance().ComputeVec3Project ( &vOldPos, NULL );
//		if ( m_strConvertName.empty() )
//		{
//			CDebugSet::ToPos ( vScr0.x, vScr0.y, m_strMARK.c_str() );
//		}
//		else
//		{
//			CDebugSet::ToPos ( vScr0.x, vScr0.y, m_strConvertName.c_str() );
//		}
//	}
//
//	return S_OK;
//}

BOOL GLLANDMARK::IsCollision ( CLIPVOLUME &cv ) const
{
	return COLLISION::IsCollisionVolume ( cv, m_vMax, m_vMin );
}

BOOL GLLANDMARK::SAVE ( sc::SerialFile &SFile )
{
	SFile << (DWORD)VERSION;

	SFile.BeginBlock();
	{
		SFile << m_strMARK;
		SFile << m_nKeyWordIndex;
		SFile << m_dwColor;

		SFile.WriteBuffer ( m_pAffineParts, sizeof(DXAFFINEPARTS) );
	}
	SFile.EndBlock();

	// 제대로 셋팅 되었는지 확인차 호출한다.
	ConvertName();

	return TRUE;
}

BOOL GLLANDMARK::LOAD ( sc::BaseStream &SFile )
{
	DWORD dwVER(0);
	DWORD dwBufferSize(0);

	SFile >> dwVER;

	switch ( dwVER )
	{
	case 0x0001:
	case 0x0002:
		{
			SFile >> m_strMARK;

			SFile.ReadBuffer ( m_pAffineParts, sizeof(DXAFFINEPARTS) );
			SFile >> m_vMax;
			SFile >> m_vMin;

			DXAFFINEMATRIX::SetAffineValue ( m_pAffineParts );

            m_strConvertName = m_strMARK;
		}
		break;
	case 0x0003:
		{
			SFile >> dwBufferSize;

			SFile >> m_strMARK;
			SFile >> m_nKeyWordIndex;
			SFile >> m_dwColor;

			SFile.ReadBuffer ( m_pAffineParts, sizeof(DXAFFINEPARTS) );
			DXAFFINEMATRIX::SetAffineValue ( m_pAffineParts );

            ConvertName();
		}
		break;
	default :	
		break;
	}

	CALC_BOX();

	return TRUE;
}

#include "pch.h"

#include "../../../SigmaCore/File/SerialFile.h"
#include "../../../SigmaCore/File/BaseStream.h"

#include "./DxSkeletonMaintain.h"

#include "DxAttBoneLinkThreadNeedData.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------




DxAttBoneLinkThreadNeedData::DxAttBoneLinkThreadNeedData()
	: m_strLinkBoneName("")
	, m_emWeaponWhereBack(EMPEACE_WEAPON_RSLOT)
	, m_bAttackMode(TRUE)
	, m_vPos(0.0f, 0.0f, 0.0f)
	, m_vRotate(0.0f, 0.0f, 0.0f)
	, m_vScale(0.0f, 0.0f, 0.0f)
	, m_vNonAtkPos(0.0f, 0.0f, 0.0f)
	, m_vNonAtkRotate(0.0f, 0.0f, 0.0f)
	, m_vNonAtkScale(0.0f, 0.0f, 0.0f)
	, m_bInverseMatrix(FALSE)
{
	D3DXMatrixIdentity( &m_matInverseMatrix );
}

void DxAttBoneLinkThreadNeedData::ComputeMatrix( D3DXMATRIX& matOut, const DxSkeletonMaintain *pLinkSkeleton ) const
{
	D3DXMATRIX matBase, matScale, matRotate, matTrans, matInverseRotate;
	D3DXMatrixIdentity( &matBase );

	if( pLinkSkeleton )
	{
		if( m_bAttackMode )
		{
			const DxBoneTransMaintain* pBoneTran = pLinkSkeleton->FindBone( m_strLinkBoneName.c_str() );
			if( pBoneTran )
				matBase = pBoneTran->m_matCombined;

			D3DXMatrixScaling( &matScale, m_vScale.x, m_vScale.y, m_vScale.z );
			D3DXMatrixRotationYawPitchRoll( &matRotate, D3DXToRadian(m_vRotate.y), D3DXToRadian(m_vRotate.x), D3DXToRadian(m_vRotate.z) );
			D3DXMatrixTranslation( &matTrans, m_vPos.x, m_vPos.y, m_vPos.z );

			if ( m_bInverseMatrix )
			{
				matOut = m_matInverseMatrix * matScale * matRotate * matTrans * matBase;
			}
			else
			{
				matOut = matScale * matRotate * matTrans * matBase;
			}
		}
		else
		{
			const DxBoneTransMaintain* pBoneTran = pLinkSkeleton->FindBone( SKINTYPE_STING::m_szWHEREBACKTYPE[m_emWeaponWhereBack] );
			if( pBoneTran )
				matBase = pBoneTran->m_matCombined;

			D3DXMatrixScaling( &matScale, m_vNonAtkScale.x, m_vNonAtkScale.y, m_vNonAtkScale.z );
			D3DXMatrixRotationYawPitchRoll( &matRotate, D3DXToRadian(m_vNonAtkRotate.y), D3DXToRadian(m_vNonAtkRotate.x), D3DXToRadian(m_vNonAtkRotate.z) );
			D3DXMatrixTranslation( &matTrans, m_vNonAtkPos.x, m_vNonAtkPos.y, m_vNonAtkPos.z );

			if ( m_bInverseMatrix )
			{
				matOut = m_matInverseMatrix * matScale * matRotate * matTrans * matBase;
			}
			else
			{
				matOut = matScale * matRotate * matTrans * matBase;
			}
		}
	}
	else
	{
		D3DXMatrixIdentity( &matOut );
	}
}

void DxAttBoneLinkThreadNeedData::ClearAll ()
{
	m_vPos	  = m_vNonAtkPos    = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_vRotate = m_vNonAtkRotate = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_vScale  = m_vNonAtkScale  = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );

	m_bInverseMatrix = FALSE;
	D3DXMatrixIdentity( &m_matInverseMatrix );

	m_strLinkBoneName.clear();
}

void DxAttBoneLinkThreadNeedData::Import ( DxAttBoneLinkThreadNeedData* pSrc )
{
	*this = *pSrc;
}

void DxAttBoneLinkThreadNeedData::Save ( sc::SerialFile &SFile )
{
	SFile << static_cast<DWORD>( VERSION );

	SFile.BeginBlock();
	{
		SFile << m_strLinkBoneName;

		DWORD dwType = (DWORD)m_emWeaponWhereBack;
		SFile << dwType;

		SFile.WriteBuffer ( m_vPos, sizeof(D3DXVECTOR3) );
		SFile.WriteBuffer ( m_vRotate, sizeof(D3DXVECTOR3) );
		SFile.WriteBuffer ( m_vScale, sizeof(D3DXVECTOR3) );

		SFile.WriteBuffer ( m_vNonAtkPos, sizeof(D3DXVECTOR3) );
		SFile.WriteBuffer ( m_vNonAtkRotate, sizeof(D3DXVECTOR3) );
		SFile.WriteBuffer ( m_vNonAtkScale, sizeof(D3DXVECTOR3) );

		SFile << m_bInverseMatrix;
		SFile.WriteBuffer ( m_matInverseMatrix, sizeof(D3DXMATRIX) );
		
	}
	SFile.EndBlock();
}

void DxAttBoneLinkThreadNeedData::Load ( sc::BaseStream &SFile )
{
	DWORD dwVer, dwBufferSize;

	SFile >> dwVer;
	SFile >> dwBufferSize;

	SFile >> m_strLinkBoneName;

	DWORD dwTemp;
	SFile >> dwTemp;
	m_emWeaponWhereBack = (EMPEACEZONEWEAPON) dwTemp;
	if ( m_emWeaponWhereBack == EMPEACE_WEAPON_NONE_MINUS )		// 다음버젼이 생기면 이 작업은 필요 없다.
	{
		m_emWeaponWhereBack = EMPEACE_WEAPON_NONE;
	}

	SFile.ReadBuffer ( m_vPos, sizeof(D3DXVECTOR3) );
	SFile.ReadBuffer ( m_vRotate, sizeof(D3DXVECTOR3) );
	SFile.ReadBuffer ( m_vScale, sizeof(D3DXVECTOR3) );

	if ( m_vScale.x < 0.f )
		m_vScale.x = -m_vScale.x;
	if ( m_vScale.y < 0.f )
		m_vScale.y = -m_vScale.y;
	if ( m_vScale.z < 0.f )
		m_vScale.z = -m_vScale.z;

	SFile.ReadBuffer ( m_vNonAtkPos, sizeof(D3DXVECTOR3) );
	SFile.ReadBuffer ( m_vNonAtkRotate, sizeof(D3DXVECTOR3) );
	SFile.ReadBuffer ( m_vNonAtkScale, sizeof(D3DXVECTOR3) );

	if ( m_vNonAtkScale.x < 0.f )
		m_vNonAtkScale.x = -m_vNonAtkScale.x;
	if ( m_vNonAtkScale.y < 0.f )
		m_vNonAtkScale.y = -m_vNonAtkScale.y;
	if ( m_vNonAtkScale.z < 0.f )
		m_vNonAtkScale.z = -m_vNonAtkScale.z;

	SFile >> m_bInverseMatrix;
	SFile.ReadBuffer ( m_matInverseMatrix, sizeof(D3DXMATRIX) );
}
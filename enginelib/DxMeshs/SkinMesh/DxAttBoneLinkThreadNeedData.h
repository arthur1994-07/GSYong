#pragma once

#include "./DxPieceDefine.h"

class DxSkeletonMaintain;

namespace sc
{
	class SerialFile;
	class BaseStream;
};

struct DxAttBoneLinkThreadNeedData
{
	enum
	{
		VERSION = 0x100,
	};

	std::string			m_strLinkBoneName;
	EMPEACEZONEWEAPON	m_emWeaponWhereBack;
	BOOL				m_bAttackMode;
	D3DXVECTOR3			m_vPos;
	D3DXVECTOR3			m_vRotate;
	D3DXVECTOR3			m_vScale;
	D3DXVECTOR3			m_vNonAtkPos;
	D3DXVECTOR3			m_vNonAtkRotate;
	D3DXVECTOR3			m_vNonAtkScale;
	BOOL				m_bInverseMatrix;
	D3DXMATRIX			m_matInverseMatrix;

	void ComputeMatrix( D3DXMATRIX& matOut, const DxSkeletonMaintain *pLinkSkeleton ) const;

	void ClearAll ();
	void Import ( DxAttBoneLinkThreadNeedData* pSrc );

	void Save ( sc::SerialFile &SFile );
	void Load ( sc::BaseStream &SFile );

	DxAttBoneLinkThreadNeedData();
};

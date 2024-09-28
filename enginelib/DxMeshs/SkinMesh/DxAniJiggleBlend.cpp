#include "pch.h"

#include "../../../SigmaCore/File/SerialFile.h"

#include "DxAniJiggleBlend.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DxAniJiggleBlend::DxAniJiggleBlend()
{
}

DxAniJiggleBlend::~DxAniJiggleBlend()
{
	m_mapAttachBoneKeyBlend.clear();
}

void DxAniJiggleBlend::Insert( int nIndex, DWORD dwKey, float fBlend )
{
	MAP_KEY_BLEND* pmapKeyBlend = GetMapKeyBlend_NoneConst( nIndex );
	if ( !pmapKeyBlend )
	{
		std::tr1::shared_ptr<MAP_KEY_BLEND> spNew = std::tr1::shared_ptr<MAP_KEY_BLEND>(new MAP_KEY_BLEND);
		m_mapAttachBoneKeyBlend.insert( std::make_pair(nIndex,spNew) );

		pmapKeyBlend = spNew.get();
	}

	pmapKeyBlend->insert( std::make_pair(dwKey,fBlend) );
}

void DxAniJiggleBlend::Delete( int nIndexA, DWORD dwIndex )
{
	MAP_KEY_BLEND* pmapKeyBlend = GetMapKeyBlend_NoneConst( nIndexA );
	if ( !pmapKeyBlend )
		return;

	DWORD dwCount(0);
	MAP_KEY_BLEND_ITER iter = pmapKeyBlend->begin();
	for ( ; iter!=pmapKeyBlend->end(); ++iter, ++dwCount )
	{
		if ( dwCount == dwIndex )
		{
			pmapKeyBlend->erase( iter );

			// 정보가 없으면 삭제한다.
			if ( pmapKeyBlend->empty() )
			{
				MAP_ATTACHBONE_KEY_BLEND_ITER iterA = m_mapAttachBoneKeyBlend.find( nIndexA );
				if ( iterA != m_mapAttachBoneKeyBlend.end() )
				{
					m_mapAttachBoneKeyBlend.erase( iterA );
				}
			}

			return;
		}
	}
}

// 0 KeyFrame <-> 1 JiggleBone
void DxAniJiggleBlend::GetKeyJiggleWeight_PureThread( float afKeyJiggleWeight[], DWORD dwKeyJiggleWeightNUM, DWORD dwKey ) const
{
	MAP_ATTACHBONE_KEY_BLEND_CITER citer = m_mapAttachBoneKeyBlend.begin();
	for ( ; citer!=m_mapAttachBoneKeyBlend.end(); ++citer )
	{
		const MAP_KEY_BLEND* pmapKeyBlend = (*citer).second.get();
		if ( !pmapKeyBlend )
			continue;

		MAP_KEY_BLEND_CITER citer2 = pmapKeyBlend->find( dwKey );
		if ( citer2 != pmapKeyBlend->end() )
		{
			if ( (*citer).first == -1 )
			{
				afKeyJiggleWeight[dwKeyJiggleWeightNUM-1] = (*citer2).second;
			}
			else
			{
				if ( dwKeyJiggleWeightNUM > static_cast<DWORD>( (*citer).first ) )
				{
					afKeyJiggleWeight[(*citer).first] = (*citer2).second;
				}
			}
		}
	}
}

const DxAniJiggleBlend::MAP_KEY_BLEND* DxAniJiggleBlend::GetMapKeyBlend( int nIndex )
{
	MAP_ATTACHBONE_KEY_BLEND_CITER citer = m_mapAttachBoneKeyBlend.find( nIndex );
	if ( citer == m_mapAttachBoneKeyBlend.end() )
	{
		return NULL;
	}
	else
	{
		return (*citer).second.get();
	}
}

DxAniJiggleBlend::MAP_KEY_BLEND* DxAniJiggleBlend::GetMapKeyBlend_NoneConst( int nIndex )
{
	MAP_ATTACHBONE_KEY_BLEND_ITER iter = m_mapAttachBoneKeyBlend.find( nIndex );
	if ( iter == m_mapAttachBoneKeyBlend.end() )
	{
		return NULL;
	}
	else
	{
		return (*iter).second.get();
	}
}

BOOL DxAniJiggleBlend::Save( sc::SerialFile& SFile )
{
	SFile << VERSION;

	SFile.BeginBlock();
	{
		DWORD dwSize = m_mapAttachBoneKeyBlend.size();
		SFile << dwSize;

		MAP_ATTACHBONE_KEY_BLEND_CITER iterA = m_mapAttachBoneKeyBlend.begin();
		for ( ; iterA!=m_mapAttachBoneKeyBlend.end(); ++iterA )
		{
			SFile << (*iterA).first;

			DxAniJiggleBlend::MAP_KEY_BLEND* pmapKeyBlend = (*iterA).second.get();
			DWORD dwSize = pmapKeyBlend->size();
			SFile << dwSize;

			MAP_KEY_BLEND_CITER iterB = pmapKeyBlend->begin();
			for ( ; iterB!=pmapKeyBlend->end(); ++iterB )
			{
				SFile << (*iterB).first;
				SFile << (*iterB).second;
			}
		}
	}
	SFile.EndBlock();

	return TRUE;
}

BOOL DxAniJiggleBlend::Load( sc::BaseStream& SFile )
{
	m_mapAttachBoneKeyBlend.clear();

	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	DWORD dwSize;
	SFile >> dwSize;
	for ( DWORD i=0; i<dwSize; ++i )
	{
		DWORD dwPart;
		SFile >> dwPart;

		std::tr1::shared_ptr<MAP_KEY_BLEND> spNew = std::tr1::shared_ptr<MAP_KEY_BLEND>(new MAP_KEY_BLEND);
		m_mapAttachBoneKeyBlend.insert( std::make_pair(dwPart,spNew) );

		DWORD dwSizeB;
		SFile >> dwSizeB;
		for ( DWORD b=0; b<dwSizeB; ++b )
		{
			DWORD dwKey;
			float fBlend;
			SFile >> dwKey;
			SFile >> fBlend;

			spNew->insert( std::make_pair(dwKey,fBlend) );
		}
	}

	return TRUE;
}

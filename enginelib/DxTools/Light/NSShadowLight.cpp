#include "pch.h"

#include "../../../SigmaCore/File/SerialFile.h"

#include "../Collision.h"
#include "../DxViewPort.h"
#include "./DxShadowLight.h"
#include "./DxLightMan.h"

#include "NSShadowLight.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
// ShadowLightData
ShadowLightData::ShadowLightData()
	: m_bDirectionLight(TRUE)
{

}

void ShadowLightData::CleanUp()
{
	m_vecShadowSpotLight.clear();
	m_setEraseLightID.clear();
}

//----------------------------------------------------------------------------------------------------------------------
// Edit
void ShadowLightData::Insert( std::tr1::shared_ptr<DxShadowSpotLight> spShadowSpotLight )
{
	spShadowSpotLight->m_dwID = m_vecShadowSpotLight.size() + 1;
	m_vecShadowSpotLight.push_back( spShadowSpotLight );
}

void ShadowLightData::ResetID()
{
	DWORD dwCount = 1;
	std::vector<std::tr1::shared_ptr<DxShadowSpotLight>>::iterator iter = m_vecShadowSpotLight.begin();
	for ( ; iter!=m_vecShadowSpotLight.end(); ++iter, ++dwCount )
	{
		(*iter)->m_dwID = dwCount;
	}
}

void ShadowLightData::Delete( int nIndex )
{
	std::vector<std::tr1::shared_ptr<DxShadowSpotLight>>::iterator iter = m_vecShadowSpotLight.begin();
	int nCount(0);
	for ( ; iter!=m_vecShadowSpotLight.end(); ++iter, ++nCount )
	{
		if ( nIndex == nCount )
		{
			m_vecShadowSpotLight.erase( iter );
			ResetID();
			return;
		}
	}
}

BOOL ShadowLightData::LightItemUp( int nIndex )
{
	if ( nIndex == 0 )
		return FALSE;

	// 2개 미만이면 하지 않는다.
	if ( m_vecShadowSpotLight.size() < 2 )
		return FALSE;

	std::tr1::shared_ptr<DxShadowSpotLight> spShadowSpotLightA = m_vecShadowSpotLight[nIndex-1];
	std::tr1::shared_ptr<DxShadowSpotLight> spShadowSpotLightB = m_vecShadowSpotLight[nIndex];

	m_vecShadowSpotLight[nIndex-1] = spShadowSpotLightB;
	m_vecShadowSpotLight[nIndex] = spShadowSpotLightA;

	spShadowSpotLightB->m_dwID = nIndex-1;
	spShadowSpotLightA->m_dwID = nIndex;

	return TRUE;
}

BOOL ShadowLightData::LightItemDown( int nIndex )
{
	// 2개 미만이면 하지 않는다.
	if ( m_vecShadowSpotLight.size() < 2 )
		return FALSE;

	if ( nIndex >= static_cast<int>(m_vecShadowSpotLight.size())-1 )
		return FALSE;

	std::tr1::shared_ptr<DxShadowSpotLight> spShadowSpotLightA = m_vecShadowSpotLight[nIndex+1];
	std::tr1::shared_ptr<DxShadowSpotLight> spShadowSpotLightB = m_vecShadowSpotLight[nIndex];

	m_vecShadowSpotLight[nIndex+1] = spShadowSpotLightB;
	m_vecShadowSpotLight[nIndex] = spShadowSpotLightA;

	spShadowSpotLightB->m_dwID = nIndex+1;
	spShadowSpotLightA->m_dwID = nIndex;

	return TRUE;
}

void ShadowLightData::Clone_MouseShift( LPD3DXMATRIX& pMatrix )
{
	std::vector<std::tr1::shared_ptr<DxShadowSpotLight>>::iterator iter = m_vecShadowSpotLight.begin();
	for( ; iter!=m_vecShadowSpotLight.end(); ++iter )
	{
		std::tr1::shared_ptr<DxShadowSpotLight> spLight = (*iter);
		DWORD_PTR dwBuffer = (DWORD_PTR) &spLight->m_matWorld;
		if( dwBuffer == (DWORD_PTR) pMatrix )
		{
			std::tr1::shared_ptr<DxShadowSpotLight> spNew = std::tr1::shared_ptr<DxShadowSpotLight>(new DxShadowSpotLight);
			*(spNew.get()) = *(spLight.get());
			spNew->m_strName =  _T("UNNAMED");
			Insert( spNew );

			// Note : DxObjectMRS 커서를 바꿔준다. < 중요 >
			pMatrix = &spNew->m_matWorld;

			// 작업하고 있다는 표시도 취소해 놓는다.
			spLight->m_bEditCheck = FALSE;

			// 복제시 바로 확인 할 수 있도록 해줌.
			NSShadowLight::SetActive( this );

			return;
		}
	}
}

void ShadowLightData::Import( const ShadowLightData* pData, const SET_DWORD& setEraseLightID )
{
	m_bDirectionLight = pData->m_bDirectionLight;
	m_vecShadowSpotLight = pData->m_vecShadowSpotLight;
	m_setEraseLightID = setEraseLightID;
}

void ShadowLightData::Save ( sc::SerialFile &SFile )
{
	SFile << VERSION;
	SFile.BeginBlock();
	{
		SFile << m_bDirectionLight;

		DWORD dwSize = m_vecShadowSpotLight.size();
		SFile << dwSize;
		for ( DWORD i=0; i<dwSize; ++i )
		{
			m_vecShadowSpotLight[i]->Save ( SFile );
		}

		dwSize = m_setEraseLightID.size();
		SFile << dwSize;
		SET_DWORD_CITER citer = m_setEraseLightID.begin();
		for ( ; citer!=m_setEraseLightID.end(); ++citer )
		{
			SFile << (*citer);
		}
	}
	SFile.EndBlock();
}

void ShadowLightData::Load ( sc::SerialFile &SFile )
{
	CleanUp();

	DWORD dwVer, dwBufferSize;
	SFile >> dwVer;
	SFile >> dwBufferSize;

	if ( dwVer==0x102 )
	{
		SFile >> m_bDirectionLight;

		DWORD dwSize(0);
		SFile >> dwSize;
		for ( DWORD i=0; i<dwSize; ++i )
		{
			std::tr1::shared_ptr<DxShadowSpotLight> spSpotLight = std::tr1::shared_ptr<DxShadowSpotLight>(new DxShadowSpotLight);
			spSpotLight->Load( SFile, i+1 );

			m_vecShadowSpotLight.push_back( spSpotLight );
		}

		SFile >> dwSize;
		for ( DWORD i=0; i<dwSize; ++i )
		{
			DWORD dwData;
			SFile >> dwData;
			m_setEraseLightID.insert( dwData );
		}
	}
	else if ( dwVer==0x101 )
	{
		SFile >> m_bDirectionLight;

		DWORD dwSize(0);
		SFile >> dwSize;
		for ( DWORD i=0; i<dwSize; ++i )
		{
			std::tr1::shared_ptr<DxShadowSpotLight> spSpotLight = std::tr1::shared_ptr<DxShadowSpotLight>(new DxShadowSpotLight);
			spSpotLight->Load( SFile, i+1 );

			m_vecShadowSpotLight.push_back( spSpotLight );
		}
	}
	else if ( dwVer==0x100 )
	{
		SFile >> m_bDirectionLight;

		BOOL bTemp;
		SFile >> bTemp;

		DWORD dwSize(0);
		SFile >> dwSize;
		for ( DWORD i=0; i<dwSize; ++i )
		{
			std::tr1::shared_ptr<DxShadowSpotLight> spSpotLight = std::tr1::shared_ptr<DxShadowSpotLight>(new DxShadowSpotLight);
			spSpotLight->Load( SFile, i+1 );

			m_vecShadowSpotLight.push_back( spSpotLight );
		}
	}
}

namespace NSShadowLight
{
	float						g_fAlphaDivSpeed(1.f);		// Light가 변할시 1.f/g_fAlphaDivSpeed 초 만큼 걸린다.
	BOOL						g_bRangeRender(FALSE);		// 
	BOOL						g_bCascadeShadow(TRUE);
	std::map<float,DWORD>		g_mapDistanceLightID;		// 거리로 가까운 ID를 얻는다.
	std::set<DWORD>				g_setGatherRenderSpot;		// 지형에서 쓰인 SpotID 정보를 모으는 곳.

	// PSF 모드에서는 전체 다 가지고 있는다.
	// WLD 모드에서는 RealTime 으로 사용되어지는 것만 가지고 있도록 한다.
	std::vector<std::tr1::shared_ptr<DxShadowSpotLight>>	g_vecShadowSpotLight;		// 전체 데이터							Save, Load
	std::vector<std::tr1::shared_ptr<DxShadowSpotLight>>	g_vecShadowSpotLightUSED;	// 사용되어지는 라이트. (2개이하)
	DxShadowLightNode*		g_pTreeHead(NULL);
	OBJAABBNode*			g_pMeshTree(NULL);

	//----------------------------------------------------------------------------------------------------------------------
	// Edit
	const std::vector<std::tr1::shared_ptr<DxShadowSpotLight>>& GetVecShadowSpotLight()
	{
		return g_vecShadowSpotLight;
	}

	//----------------------------------------------------------------------------------------------------------------------
	// Game
	void CleanUp()
	{
		g_mapDistanceLightID.clear();
		g_vecShadowSpotLight.clear();
		g_vecShadowSpotLightUSED.clear();

		SAFE_DELETE ( g_pMeshTree );
		SAFE_DELETE ( g_pTreeHead );
	}

	void MakeAABBTree( const SET_DWORD& setEraseLightID )
	{
		// setEraseLightID 이외것만 셋팅한다.
		std::vector<std::tr1::shared_ptr<DxShadowSpotLight>>	vecShadowSpotLight;
		std::vector<std::tr1::shared_ptr<DxShadowSpotLight>>::iterator iter = g_vecShadowSpotLight.begin();
		for ( DWORD i=0; i<g_vecShadowSpotLight.size(); ++i )
		{
			SET_DWORD::const_iterator iter = setEraseLightID.find( g_vecShadowSpotLight[i]->m_dwID );
			if ( iter == setEraseLightID.end() )
			{
				vecShadowSpotLight.push_back( g_vecShadowSpotLight[i] );
			}
		}

		// List 를 만든다.
		SAFE_DELETE ( g_pTreeHead );
		for ( DWORD i=0; i<vecShadowSpotLight.size(); ++i )
		{
			DxShadowLightNode* pNew = new DxShadowLightNode;
			pNew->SetData( vecShadowSpotLight[i].get(), &g_pTreeHead );
		}

		COLLISION::MakeAABBTree( g_pMeshTree, g_pTreeHead );
	}

	void SetActive( const ShadowLightData* pData )
	{
		g_mapDistanceLightID.clear();
		g_vecShadowSpotLightUSED.clear();

		g_bCascadeShadow = pData->m_bDirectionLight;
		g_vecShadowSpotLight = pData->m_vecShadowSpotLight;

		MakeAABBTree( pData->m_setEraseLightID );
	}

	void MakeAABBTree()
	{
		// List 를 만든다.
		SAFE_DELETE ( g_pTreeHead );
		for ( DWORD i=0; i<g_vecShadowSpotLight.size(); ++i )
		{
			DxShadowLightNode* pNew = new DxShadowLightNode;
			pNew->SetData( g_vecShadowSpotLight[i].get(), &g_pTreeHead );
		}

		COLLISION::MakeAABBTree( g_pMeshTree, g_pTreeHead );
	}

	void Insert_mapDistanceLightID( float fDistance, DWORD dwID )
	{
		if ( dwID <= 0 )
			return;

		DWORD dwArrayID = dwID-1;
		if ( dwArrayID >= g_vecShadowSpotLight.size() )
			return;

		// ArrayID 를 second 로 넣도록 한다.
		g_mapDistanceLightID.insert ( std::make_pair( fDistance, dwArrayID ) );
	}
	
	void FrameMoveTool( float fElapsedTime )
	{
		if ( !g_pMeshTree )
			return;

		// 가까운것을 구한다.
		g_mapDistanceLightID.clear();

		// 여기에서 Insert_DistanceLightID 이 호출된다.
		COLLISION::FrameMove( fElapsedTime, g_pMeshTree );

		g_vecShadowSpotLightUSED.clear();
		std::map<float,DWORD>::iterator iter = g_mapDistanceLightID.begin();
		std::map<float,DWORD>::iterator iter_end = g_mapDistanceLightID.end();
		for ( ; iter!=iter_end; ++iter )
		{			
			g_vecShadowSpotLight[(*iter).second]->FrameMove( fElapsedTime );
			g_vecShadowSpotLightUSED.push_back( g_vecShadowSpotLight[(*iter).second] );
		}

		PreRender( DxViewPort::GetInstance().GetMatView() );
	}

	void NSShadowLight::PreRender( const D3DXMATRIX& matView )
	{
		for ( DWORD i=0; i<g_vecShadowSpotLight.size(); ++i )
		{
			g_vecShadowSpotLight[i]->PreRender( matView );
		}
	}

	// 지형 렌더링이 다 끝난 후에 호출해야한다.
	// 그렇지 않으면, g_vecShadowSpotLightUSED 가 변경될때마다 Spot 의 튐현상이 생긴다.`
	void FrameMove( float fElapsedTime )
	{
		if ( !g_pMeshTree )
			return;

		// 가까운것을 구한다.
		g_mapDistanceLightID.clear();

		// 여기에서 Insert_DistanceLightID 이 호출된다.
		COLLISION::FrameMove( fElapsedTime, g_pMeshTree );

		// g_vecShadowSpotLightUSED 의 시간을 빼준다.
		for ( DWORD i=0; i<g_vecShadowSpotLightUSED.size(); ++i )
		{
			g_vecShadowSpotLightUSED[i]->m_fAlpha -= fElapsedTime*g_fAlphaDivSpeed;
		}

		// 
		{
			DWORD m_dwRenderCount(0);
			std::map<float,DWORD>::iterator iter = g_mapDistanceLightID.begin();
			std::map<float,DWORD>::iterator iter_end = g_mapDistanceLightID.end();
			for ( ; iter!=iter_end && m_dwRenderCount<2; ++iter, ++m_dwRenderCount )
			{			
				if ( g_vecShadowSpotLightUSED.empty() )			// 아무것도 없을 때는 무조건 넣는다.
				{
					g_vecShadowSpotLight[(*iter).second]->m_fAlpha = -fElapsedTime*g_fAlphaDivSpeed;		// 초기화라보면된다.
					g_vecShadowSpotLightUSED.push_back( g_vecShadowSpotLight[(*iter).second] );
				}
				else if ( g_vecShadowSpotLightUSED.size()==1 )	// 1개만 있다.
				{
					if ( g_vecShadowSpotLightUSED[0]->m_dwID != g_vecShadowSpotLight[(*iter).second]->m_dwID )
					{
						// 같지 않을 경우만 넣는다.
						g_vecShadowSpotLight[(*iter).second]->m_fAlpha = -fElapsedTime*g_fAlphaDivSpeed;	// 초기화라보면된다.
						g_vecShadowSpotLightUSED.push_back( g_vecShadowSpotLight[(*iter).second] );
					}
				}

				// g_vecShadowSpotLightUSED 에 등록된 애들만, 시간을 올려주도록 한다.
				for ( DWORD i=0; i<g_vecShadowSpotLightUSED.size(); ++i )
				{
					if ( g_vecShadowSpotLightUSED[i]->m_dwID == g_vecShadowSpotLight[(*iter).second]->m_dwID )
					{
						// 시간을 건들여준다. (2개만 시간을 올리도록 한다.)
						g_vecShadowSpotLight[(*iter).second]->m_fAlpha += fElapsedTime*g_fAlphaDivSpeed*2.f;
						if ( g_vecShadowSpotLight[(*iter).second]->m_fAlpha > 1.f )
							g_vecShadowSpotLight[(*iter).second]->m_fAlpha = 1.f;
					}
				}
			}
		}

		// g_vecShadowSpotLightUSED 의 시간이 마이너스라면 g_vecShadowSpotLightUSED에서 삭제한다.
		{
			std::vector<std::tr1::shared_ptr<DxShadowSpotLight>>::iterator iter = g_vecShadowSpotLightUSED.begin();
			for ( ; iter<g_vecShadowSpotLightUSED.end(); )
			{
				if ( (*iter)->m_fAlpha < 0.f )
				{
					iter = g_vecShadowSpotLightUSED.erase( iter );
				}
				else
				{
					(*iter)->FrameMove( fElapsedTime );
					++iter;
				}
			}
		}

		// Test 버젼.
		for ( DWORD i=0; i<g_vecShadowSpotLight.size(); ++i )
		{
			if ( g_vecShadowSpotLight[i]->m_fAlpha < 0.f )
				g_vecShadowSpotLight[i]->m_fAlpha = 0.f;
		}
	}

	DWORD GetLightNumber()
	{
		return g_vecShadowSpotLightUSED.size();
	}

	const DxShadowSpotLight* GetShadowSpotLight( int nIndex )
	{
		if ( nIndex >= static_cast<int>(g_vecShadowSpotLightUSED.size()) )
			return NULL;

		return g_vecShadowSpotLightUSED[nIndex].get();
	}

	DWORD GetLightNumberALL()
	{
		return g_vecShadowSpotLight.size();
	}

	const DxShadowSpotLight* GetShadowSpotLightALL( DWORD dwID )
	{
		if ( dwID <= 0 )
			return NULL;

		if ( dwID == WORD(-1) )
			return NULL;
		
		DWORD dwArrayID = dwID-1;
		if ( dwArrayID >= g_vecShadowSpotLight.size() )
			return NULL;

		return g_vecShadowSpotLight[dwArrayID].get();
	}

	// 이 셋팅은 직사광의 cast 물체에 영향을 받지 않는 셋팅임.
	BOOL IsCascadeShadow()
	{
		return g_bCascadeShadow;
	}

	// 지형에 속한 Spot 들의 정보를 모아놓은 것 삭제함.
	// 실제 Spot 데이터를 지우는 것이 아님.
	void ClearRenderSpotID()
	{
		g_setGatherRenderSpot.clear();
	}
};



//////////////////////////////////////////////////////////////////////////
//							NSShadowLightUtil
namespace NSShadowLightUtil
{
	DWORD GetPassFX_PS_3_0_TYPE_USED( PS_3_0_TYPE emPS_3_0_Type, WORDWORD wwLightID )
	{
		DWORD dwNumber = NSShadowLight::GetLightNumber();

		switch ( emPS_3_0_Type )
		{
		case PS_3_0_TYPE_ZERO:
		case PS_3_0_TYPE_CSM:
			return 0;

		case PS_3_0_TYPE_CSM_PL:
			{
				BOOL bUsedPL(FALSE);
				for ( DWORD z=0; z<dwNumber; ++z )
				{
					const DxShadowSpotLight* pSpotLight = NSShadowLight::GetShadowSpotLight(z);
					if ( pSpotLight )
					{
						if ( wwLightID.wB == pSpotLight->m_dwID )
						{
							bUsedPL = TRUE;
							break;
						}
					}
				}

				if ( bUsedPL )
				{
					return 0;
				}
				else
				{
					return 2;
				}
			}
			break;

		case PS_3_0_TYPE_PL1:
			{
				BOOL bUsedPL(FALSE);
				for ( DWORD z=0; z<dwNumber; ++z )
				{
					const DxShadowSpotLight* pSpotLight = NSShadowLight::GetShadowSpotLight(z);
					if ( pSpotLight )
					{
						if ( wwLightID.wA == pSpotLight->m_dwID )
						{
							bUsedPL = TRUE;
							break;
						}
					}
				}

				if ( bUsedPL )
				{
					return 0;
				}
				else
				{
					return 1;
				}
			}
			break;

		case PS_3_0_TYPE_PL2:
			{
				BOOL bUsedPL(FALSE);
				for ( DWORD z=0; z<dwNumber; ++z )
				{
					const DxShadowSpotLight* pSpotLight = NSShadowLight::GetShadowSpotLight(z);
					if ( pSpotLight )
					{
						if ( wwLightID.wB == pSpotLight->m_dwID )
						{
							bUsedPL = TRUE;
							break;
						}
					}
				}

				if ( bUsedPL )
				{
					return 0;
				}
				else
				{
					return 2;
				}
			}
			break;

		case PS_3_0_TYPE_PL_PL:
			{
				BOOL bUsedPL(FALSE);
				BOOL bUsedPL2(FALSE);
				for ( DWORD z=0; z<dwNumber; ++z )
				{
					const DxShadowSpotLight* pSpotLight = NSShadowLight::GetShadowSpotLight(z);
					if ( pSpotLight )
					{
						if ( wwLightID.wA == pSpotLight->m_dwID )
						{
							bUsedPL = TRUE;
						}
						else if ( wwLightID.wB == pSpotLight->m_dwID )
						{
							bUsedPL2 = TRUE;
						}
					}
				}

				if ( bUsedPL && bUsedPL2 )
				{
					return 0;
				}
				else if ( bUsedPL )
				{
					return 2;
				}
				else if ( bUsedPL2 )
				{
					return 1;
				}
				else
				{
					return 3;
				}
			}
			break;
		}

		return 0;
	}

	int GetPassFX_PS_3_0_TYPE_USED_MPShading( PS_3_0_TYPE emPS_3_0_Type, WORDWORD wwLightID )
	{
		DWORD dwNumber = NSShadowLight::GetLightNumber();

		switch ( emPS_3_0_Type )
		{
		case PS_3_0_TYPE_ZERO:
		case PS_3_0_TYPE_CSM:
			return -1;

		case PS_3_0_TYPE_CSM_PL:
			{
				BOOL bUsedPL(FALSE);
				for ( DWORD z=0; z<dwNumber; ++z )
				{
					const DxShadowSpotLight* pSpotLight = NSShadowLight::GetShadowSpotLight(z);
					if ( pSpotLight )
					{
						if ( wwLightID.wB == pSpotLight->m_dwID )
						{
							bUsedPL = TRUE;
							break;
						}
					}
				}

				if ( bUsedPL )
				{
					return 1;
				}
				else
				{
					return -1;
				}
			}
			break;

		case PS_3_0_TYPE_PL1:
			{
				BOOL bUsedPL(FALSE);
				for ( DWORD z=0; z<dwNumber; ++z )
				{
					const DxShadowSpotLight* pSpotLight = NSShadowLight::GetShadowSpotLight(z);
					if ( pSpotLight )
					{
						if ( wwLightID.wA == pSpotLight->m_dwID )
						{
							bUsedPL = TRUE;
							break;
						}
					}
				}

				if ( bUsedPL )
				{
					return 0;
				}
				else
				{
					return -1;
				}
			}
			break;

		case PS_3_0_TYPE_PL2:
			{
				BOOL bUsedPL(FALSE);
				for ( DWORD z=0; z<dwNumber; ++z )
				{
					const DxShadowSpotLight* pSpotLight = NSShadowLight::GetShadowSpotLight(z);
					if ( pSpotLight )
					{
						if ( wwLightID.wB == pSpotLight->m_dwID )
						{
							bUsedPL = TRUE;
							break;
						}
					}
				}

				if ( bUsedPL )
				{
					return 1;
				}
				else
				{
					return -1;
				}
			}
			break;

		case PS_3_0_TYPE_PL_PL:
			{
				BOOL bUsedPL(FALSE);
				BOOL bUsedPL2(FALSE);
				for ( DWORD z=0; z<dwNumber; ++z )
				{
					const DxShadowSpotLight* pSpotLight = NSShadowLight::GetShadowSpotLight(z);
					if ( pSpotLight )
					{
						if ( wwLightID.wA == pSpotLight->m_dwID )
						{
							bUsedPL = TRUE;
						}
						else if ( wwLightID.wB == pSpotLight->m_dwID )
						{
							bUsedPL2 = TRUE;
						}
					}
				}

				if ( bUsedPL && bUsedPL2 )
				{
					return 2;
				}
				else if ( bUsedPL )
				{
					return 0;
				}
				else if ( bUsedPL2 )
				{
					return 1;
				}
				else
				{
					return -1;
				}
			}
			break;
		}

		return -1;
	}
}
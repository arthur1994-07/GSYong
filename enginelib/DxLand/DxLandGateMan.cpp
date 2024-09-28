#include "pch.h"

#include "../DxTools/DxMethods.h"
#include "../DxTools/EDITMESHS.h"
#include "../NaviMesh/NavigationMesh.h"
#include "../../SigmaCore/File/SerialFile.h"
#include "../../SigmaCore/File/BaseStream.h"
#include "../Common/StlFunctions.h"

#include <algorithm>
#include "DxLandGateMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

BOOL DxLandGateMan::m_bREND = TRUE;

void DxLandGate::SetGate ( DWORD dwFlags, DWORD GateID, const D3DXVECTOR2 &vDiv )
{
	m_dwGeteFlags = dwFlags;
	m_GateID = GateID;	
	m_vDiv = vDiv;
}

void DxLandGate::SetPos ( const D3DXVECTOR3 &vPos )
{
	D3DXVECTOR3 vSize = m_vMax - m_vMin;

	m_vMax = vPos + vSize/2;
	m_vMin = vPos - vSize/2;
}

void DxLandGate::SetBox ( const D3DXVECTOR3 &vMax, const D3DXVECTOR3 &vMin )
{
	GASSERT(vMax.x>=vMin.x);
	GASSERT(vMax.y>=vMin.y);
	GASSERT(vMax.z>=vMin.z);

	m_vMax = vMax;
	m_vMin = vMin;
}

const DWORD DxLandGate::GetNGateToMapID(void) const
{
	return m_nToGate;
}
const DWORD DxLandGate::AddToGateMapID(const SNATIVEID mapID, const DWORD dwGateID, const DWORD dwNameIndex, const char* const szGateName)
{
	if ( m_nToGate >= GATETO_NSIZE )
		return -1;

	for ( DWORD _i(m_nToGate); _i--; )
	{
		if ( m_ToMapID[_i] == mapID && m_ToGateID[_i] == dwGateID )
			return -1;
	}

	const DWORD nIndex(m_nToGate);
	StringCchCopy(m_szGateNameKey[nIndex], MAX_SZNAME, szGateName);
	m_nGateNameIndex[nIndex] = dwNameIndex;
	m_ToMapID[nIndex] = mapID;
	m_ToGateID[nIndex] = dwGateID;
	++m_nToGate;
	return nIndex;
}
const bool DxLandGate::SetToGateMapID(const SNATIVEID mapID, const DWORD dwGateID, const DWORD dwNameIndex, const char* const szGateName, const DWORD dwIndex)
{
	if( dwIndex >= m_nToGate && !(dwIndex==0&&m_nToGate==0) )
    {
		return false;
    }

	for ( DWORD _i(m_nToGate); _i--; )
	{
		if ( m_ToMapID[_i] == mapID && m_ToGateID[_i] == dwGateID )
		{
			StringCchCopy(m_szGateNameKey[_i], MAX_SZNAME, szGateName);
			m_nGateNameIndex[_i] = dwNameIndex;
			m_ToMapID[_i] = mapID;
			m_ToGateID[_i] = dwGateID;
			return true;
		}
	}

	StringCchCopy(m_szGateNameKey[dwIndex], MAX_SZNAME, szGateName);
	m_nGateNameIndex[dwIndex] = dwNameIndex;
	m_ToMapID[dwIndex] = mapID;
	m_ToGateID[dwIndex] = dwGateID;
	return true;
}
const bool DxLandGate::ResetToGateMapID(const DWORD dwIndex)
{
	if ( dwIndex >= m_nToGate )
		return false;
	
	memset( m_szGateNameKey[dwIndex], 0, sizeof(char)*MAX_SZNAME );
	m_nGateNameIndex[dwIndex] = -1;
	m_ToMapID[dwIndex] = SNATIVEID(false);
	m_ToGateID[dwIndex] = 0;
	for ( DWORD _i(dwIndex); _i < (m_nToGate - 1); ++_i )
	{
		StringCchCopy(m_szGateNameKey[_i], MAX_SZNAME, m_szGateNameKey[_i+1]);
		m_nGateNameIndex[_i] = m_nGateNameIndex[_i+1];
		m_ToMapID[_i] = m_ToMapID[_i+1];
		m_ToGateID[_i] = m_ToGateID[_i+1];
		
		// deinit;
		memset( m_szGateNameKey[_i+1], 0, sizeof(char)*MAX_SZNAME );
		m_nGateNameIndex[_i+1] = -1;
		m_ToMapID[_i+1] = SNATIVEID(false);
		m_ToGateID[_i+1] = 0;
	}

	--m_nToGate;
	return true;
}
void DxLandGate::ResetToGateMapID(void)
{
	m_nToGate = 0;
	for ( DWORD _i(GATETO_NSIZE); _i--; )
	{
		memset( m_szGateNameKey[_i], 0, sizeof(char)*MAX_SZNAME );
		m_nGateNameIndex[_i] = -1;
		m_ToMapID[_i] = SNATIVEID(false);
		m_ToGateID[_i] = 0;
	}
}

D3DXVECTOR3 DxLandGate::GetGenPos ( DxLandGate::EMGENPOS_SEL emGenSel )
{
	if ( m_dwStartPNum==0 )		return D3DXVECTOR3(0,0,0);

	DWORD dwGenPos = 0;
	switch ( emGenSel )
	{
	case GEN_FIRST:
		{
			dwGenPos = 0;
		}
		break;

	case GEN_RENDUM:
		{
			if ( ++m_dwLastUsedPos < m_dwStartPNum )
			{
				dwGenPos = m_dwLastUsedPos;
			}
			else
			{
				dwGenPos = m_dwLastUsedPos = 0;
			}
		}
		break;
	};

	return m_pvStartPos[dwGenPos];
}

void DxLandGate::SetObjRotate90()
{
	D3DXMATRIX matRotate;
	D3DXMatrixRotationY( &matRotate, D3DX_PI*0.5f );

	D3DXMatrixAABBTransform( m_vMax, m_vMin, matRotate );

	for ( DWORD i=0; i<m_dwStartPNum; i++ )
	{
		D3DXVec3TransformCoord( &m_pvStartPos[i], &m_pvStartPos[i], &matRotate );
	}
}

HRESULT DxLandGate::Load ( sc::BaseStream	&SFile )
{
	DWORD dwVer, dwSize;
	SFile >> dwVer;
	SFile >> dwSize;

	switch ( dwVer )
	{
	case 0x0100:
		{
			PROPERTY_100 sOldData;

			SFile.ReadBuffer ( &sOldData, sizeof(PROPERTY_100) );

			m_Prop.Assign ( sOldData );

			if ( m_Prop.m_dwStartPNum != 0 )
			{
				m_pvStartPos = new D3DXVECTOR3[m_Prop.m_dwStartPNum];
				SFile.ReadBuffer ( m_pvStartPos, sizeof(D3DXVECTOR3)*m_Prop.m_dwStartPNum );
			}
		}
		break;
	case 0x0101:
		{
			PROPERTY_101 sOldData;

			SFile.ReadBuffer ( &sOldData, sizeof(PROPERTY_101) );

			m_Prop.Assign ( sOldData );

			if ( m_Prop.m_dwStartPNum != 0 )
			{
				m_pvStartPos = new D3DXVECTOR3[m_Prop.m_dwStartPNum];
				SFile.ReadBuffer ( m_pvStartPos, sizeof(D3DXVECTOR3)*m_Prop.m_dwStartPNum );
			}
		}
		break;
	case 0x0102:
		{
			PROPERTY_102 sOldData;

			SFile.ReadBuffer ( &sOldData, sizeof(PROPERTY_102) );

			m_Prop.Assign ( sOldData );

			if ( m_Prop.m_dwStartPNum != 0 )
			{
				m_pvStartPos = new D3DXVECTOR3[m_Prop.m_dwStartPNum];
				SFile.ReadBuffer ( m_pvStartPos, sizeof(D3DXVECTOR3)*m_Prop.m_dwStartPNum );
			}
		}
		break;
	case VERSION:
		SFile.ReadBuffer ( &m_Prop, sizeof(PROPERTY) );

		if ( m_Prop.m_dwStartPNum != 0 )
		{
			m_pvStartPos = new D3DXVECTOR3[m_Prop.m_dwStartPNum];
			SFile.ReadBuffer ( m_pvStartPos, sizeof(D3DXVECTOR3)*m_Prop.m_dwStartPNum );
		}
		break;
	default:
		SFile.SetOffSet ( SFile.GetfTell()+dwSize );
		break;
	}

	return S_OK;
}

HRESULT DxLandGate::Save ( sc::SerialFile	&SFile )
{
	DWORD dwSize = sizeof(PROPERTY) + sizeof(DWORD) + sizeof(D3DXVECTOR3)*m_dwStartPNum;

	SFile << VERSION;
	SFile << dwSize;

	SFile.WriteBuffer ( &m_Prop, sizeof(PROPERTY) );
		
	if ( m_Prop.m_dwStartPNum != 0 )
	{
		SFile.WriteBuffer ( m_pvStartPos, sizeof(D3DXVECTOR3)*m_Prop.m_dwStartPNum );
	}

	return S_OK;
}

void DxLandGate::operator= (const DxLandGate &LandGate )
{
	m_Prop = LandGate.m_Prop;

	SAFE_DELETE_ARRAY(m_pvStartPos);
	m_pvStartPos = new D3DXVECTOR3[m_Prop.m_dwStartPNum];
	memcpy ( m_pvStartPos, LandGate.m_pvStartPos, sizeof(D3DXVECTOR3)*m_Prop.m_dwStartPNum );
}

BOOL DxLandGate::MakeStartPos ( NavigationMesh* pNaviMesh )
{
	GASSERT(pNaviMesh);
	SAFE_DELETE_ARRAY(m_pvStartPos);

	D3DXVECTOR3 vTestUp;
	D3DXVECTOR3 vTestDown;

	D3DXVECTOR3 vCollision;
	DWORD dwCollisionID;
	BOOL bCollision;
	D3DXVECTOR3 vCenter = (m_vMax+m_vMin)/2;

	m_dwStartPNum = 0;
	std::vector<D3DXVECTOR3> PosArray;
	for ( float fX=(m_vMin.x+m_vDiv.x); fX<(m_vMax.x-m_vDiv.x); fX+=m_vDiv.x )
	for ( float fZ=(m_vMin.z+m_vDiv.y); fZ<(m_vMax.z-m_vDiv.y); fZ+=m_vDiv.y )
	{
		vTestUp = D3DXVECTOR3(fX,m_vMax.y,fZ);
		vTestDown = D3DXVECTOR3(fX,m_vMin.y-10.f,fZ);

		pNaviMesh->IsCollision( vTestUp, vTestDown, vCollision, dwCollisionID, bCollision );
		if ( bCollision )
		{
			PosArray.push_back ( vCollision );
			m_dwStartPNum++;
		}
	}
	if ( m_dwStartPNum == 0 )
	{
		vTestUp = D3DXVECTOR3(vCenter.x, m_vMax.y, vCenter.z );
		vTestDown = D3DXVECTOR3(vCenter.x, m_vMin.y-10.f, vCenter.z );

		pNaviMesh->IsCollision ( vTestUp, vTestDown, vCollision, dwCollisionID, bCollision );
		if ( bCollision )
		{
			m_pvStartPos = new D3DXVECTOR3[1];
			m_pvStartPos[0] = vCollision;
			m_dwStartPNum = 1;
		}
		else	return FALSE;
	}
	else
	{
		//	Note : PosArray 에 있는 위치를 정렬후 배열에 복사 작업.
		//
		m_pvStartPos = new D3DXVECTOR3[m_dwStartPNum];
		if ( m_dwStartPNum > 1 )
		{
			std_afunc::CCompare Comp(vCenter);
			std::sort ( PosArray.begin(), PosArray.end(), Comp );

			for ( DWORD j=0; j<m_dwStartPNum; j++ )
				m_pvStartPos[j] = *(PosArray.begin()+j);
		}
	}

	return TRUE;
}

HRESULT DxLandGate::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	EDITMESHS::RENDERAABB ( pd3dDevice, m_vMax, m_vMin );

	for ( DWORD i=0; i<m_dwStartPNum; i++ )
	{
		EDITMESHS::RENDERSPHERE ( pd3dDevice, m_pvStartPos[i], 2.0f );
	}

	return S_OK;
}

DxLandGateMan::DxLandGateMan(void) :
	m_dwNumLandGate(0),
	m_pLandGateList(NULL),
	m_pLandGateTree(NULL)
{
}

DxLandGateMan::~DxLandGateMan(void)
{
	CleanUp ();
}

void DxLandGateMan::AddLandGate ( PDXLANDGATE pLandGate )
{
	m_dwNumLandGate++;
	pLandGate->m_pNext = m_pLandGateList;
	m_pLandGateList = pLandGate;
}

void DxLandGateMan::DelLandGate ( char* szName )
{
	GASSERT(szName);
	PDXLANDGATE pCur, pBack;

	if ( !strcmp(m_pLandGateList->GetName(),szName) )
	{
		pCur = m_pLandGateList;
		m_pLandGateList = m_pLandGateList->m_pNext;

		pCur->m_pNext = NULL;
		delete pCur;

		m_dwNumLandGate--;
		return;
	}

	pCur = m_pLandGateList;
	while ( pCur )
	{
		if ( !strcmp(pCur->GetName(),szName) )
		{
			pBack->m_pNext = pCur->m_pNext;

			pCur->m_pNext = NULL;
			delete pCur;

			m_dwNumLandGate--;
			return;
		}

		pBack = pCur;
		pCur = pCur->m_pNext;
	}
}

PDXLANDGATE DxLandGateMan::FindLandGate ( const char* szName )
{
	GASSERT(szName);

	PDXLANDGATE pCur = m_pLandGateList;
	while ( pCur )
	{
		if ( !strcmp(szName,pCur->GetName()) )	return pCur;
		pCur = pCur->m_pNext;
	}

	return NULL;
}

PDXLANDGATE DxLandGateMan::FindLandGate ( DWORD dwGateID )
{
	PDXLANDGATE pCur = m_pLandGateList;
	while ( pCur )
	{
		if ( dwGateID == pCur->GetGateID() )	return pCur;
		pCur = pCur->m_pNext;
	}

	return NULL;
}

void DxLandGateMan::BuildTree ()
{
	COLLISION::MakeAABBTree ( m_pLandGateTree, m_pLandGateList );
}

void DxLandGateMan::SetObjRotate90()
{
	PDXLANDGATE pCur = m_pLandGateList;
	while( pCur )
	{
		pCur->SetObjRotate90();
		pCur = pCur->m_pNext;
	}
	COLLISION::MakeAABBTree( m_pLandGateTree, m_pLandGateList );
}

PDXLANDGATE DxLandGateMan::DetectGate ( const D3DXVECTOR3 &vPos )
{
	if ( !m_pLandGateTree )		return NULL;

	PDXLANDGATE pLandGate = (PDXLANDGATE) COLLISION::IsCollisonPointToOBJAABBTree ( m_pLandGateTree, vPos );

	if ( pLandGate )	return pLandGate;
	return NULL;
}

void DxLandGateMan::CleanUp ()
{
	m_dwNumLandGate = 0;
	SAFE_DELETE(m_pLandGateList);
	SAFE_DELETE(m_pLandGateTree);
}

HRESULT DxLandGateMan::Render ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	PDXLANDGATE pCur = m_pLandGateList;
	while ( pCur )
	{
		pCur->Render ( pd3dDevice );
		pCur = pCur->m_pNext;
	}

	return S_OK;
}

VOID DxLandGateMan::SaveCsvHead( std::fstream &SFile )
{
	SFile << "Gate Name" << ",";	
	SFile << "Gate ID" << ",";
	SFile << "Type" << ",";

	SFile << "Gate Name Key0"<< ",";
	SFile << "Gate Name Index0" <<",";
	SFile << "ToMapMID0" << ",";
	SFile << "ToMapSID0" << ",";

	SFile << "Gate Name Key1"<< ",";
	SFile << "Gate Name Index1" <<",";
	SFile << "ToMapMID1" << ",";
	SFile << "ToMapSID1" << ",";

	SFile << "Gate Name Key2"<< ",";
	SFile << "Gate Name Index2" <<",";
	SFile << "ToMapMID2" << ",";
	SFile << "ToMapSID2" << ",";

	SFile << "Gate Name Key3"<< ",";
	SFile << "Gate Name Index3" <<",";
	SFile << "ToMapMID3" << ",";
	SFile << "ToMapSID3" << ",";

	SFile << "Gate Name Key4"<< ",";
	SFile << "Gate Name Index4" <<",";
	SFile << "ToMapMID4" << ",";
	SFile << "ToMapSID4" << ",";

	SFile << "Gate Name Key5"<< ",";
	SFile << "Gate Name Index5" <<",";
	SFile << "ToMapMID5" << ",";
	SFile << "ToMapSID5" << ",";

	SFile << "Gate Name Key6"<< ",";
	SFile << "Gate Name Index6" <<",";
	SFile << "ToMapMID6" << ",";
	SFile << "ToMapSID6" << ",";

	SFile << "Gate Name Key7"<< ",";
	SFile << "Gate Name Index7" <<",";
	SFile << "ToMapMID7" << ",";
	SFile << "ToMapSID7" << ",";
	SFile << "ToGate" << ",";
	SFile << "SizeA" << ",";
	SFile << "SizeB" << ",";
}

VOID DxLandGateMan::SaveCsv( std::fstream &SFile )
{
	PDXLANDGATE pCur = m_pLandGateList;
	while ( pCur )
	{
		SFile << pCur->GetName() << ",";		

		SFile << pCur->GetGateID() << ",";
		SFile << pCur->GetFlags() << ",";
		
		for ( DWORD _i(0); _i < DxLandGate::GATETO_NSIZE; ++_i )
		{
			SFile << pCur->GetGateNameKey(_i) << ",";
			SFile << pCur->GetGateNameIndex(_i) << ",";
			SFile << pCur->GetToMapID(_i).wMainID << ",";
			SFile << pCur->GetToMapID(_i).wSubID << ",";
			SFile << pCur->GetToGateID(_i) << ",";
		}		

		SFile << pCur->GetDiv().x << ",";
		SFile << pCur->GetDiv().y << ",";
		SFile << std::endl;

		pCur = pCur->m_pNext;
	};
}

VOID DxLandGateMan::LoadCsv( CStringArray &StrArray )
{
	PDXLANDGATE pCur = new DxLandGate;

	int			iCsvCur = 0;
	DWORD	    dwFlags;
	DWORD	    dwGateID;
	SNATIVEID   sToMapID;
	DWORD	    dwToGateID;
	D3DXVECTOR2 vDiv;

	pCur->SetName( StrArray[ iCsvCur++ ] );	

	dwGateID = (DWORD)atoi( StrArray[ iCsvCur++ ] );
	dwFlags  = (DWORD)atoi( StrArray[ iCsvCur++ ] );

	for ( DWORD _i(0); _i < DxLandGate::GATETO_NSIZE; ++_i )
	{
		CString szToGateNameKey(StrArray[ iCsvCur++ ]);
		DWORD dwToGateNameIndex((DWORD)atoi( StrArray[ iCsvCur++ ]));

		sToMapID.wMainID = (WORD)atoi( StrArray[ iCsvCur++ ] );
		sToMapID.wSubID  = (WORD)atoi( StrArray[ iCsvCur++ ] );
		dwToGateID = (DWORD)atoi( StrArray[ iCsvCur++ ] );		
		pCur->AddToGateMapID(sToMapID, dwToGateID, dwToGateNameIndex, szToGateNameKey.GetString());
	}

	vDiv.x = (float)atof( StrArray[ iCsvCur++ ] );
	vDiv.y = (float)atof( StrArray[ iCsvCur++ ] );

	pCur->SetGate( dwFlags, dwGateID, vDiv );


	AddLandGate ( pCur );

}


HRESULT DxLandGateMan::Load ( sc::BaseStream &SFile )
{
	BOOL bExist = FALSE;
	CleanUp ();

	SFile >> bExist;	

	while ( bExist )
	{
		PDXLANDGATE pCur = new DxLandGate;
		pCur->Load ( SFile );
	
		AddLandGate ( pCur );

		SFile >> bExist;
	}

	BuildTree ();

	return S_OK;
}

HRESULT DxLandGateMan::Save ( sc::SerialFile	&SFile )
{
	PDXLANDGATE pCur = m_pLandGateList;
	while ( pCur )
	{
		SFile << TRUE;
		pCur->Save ( SFile );

		pCur = pCur->m_pNext;
	};
	
	SFile << FALSE;

	return S_OK;
}

DxLandGateMan& DxLandGateMan::operator= (const DxLandGateMan &LandGateMan )
{
	CleanUp ();

	PDXLANDGATE pCur = LandGateMan.m_pLandGateList;
	while ( pCur )
	{
		DxLandGate *pNew = new DxLandGate;
		*pNew = *pCur;
		AddLandGate ( pNew );

		pCur = pCur->m_pNext;
	}

	BuildTree();

	return *this;
}


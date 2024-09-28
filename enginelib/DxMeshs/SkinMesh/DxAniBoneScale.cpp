#include "pch.h"
#include "./DxAniBoneScale.h"

#include "../../Common/StlFunctions.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------
//						D	x		A	n	i		S	c	a	l	e		M	A	I	N
//------------------------------------------------------------------------------------------------------------
void DxAniScaleMAIN::UpdateBoneScale( DxBoneTrans* pBoneTrans, float fWeight, BOOL bFirst, float fTime, float fSTime, float fETime, float UnitTime, UPDATETYPE emType )
{
	float fStartTime = 0.f;
	float fEndTime = 0.f;
	float dwStartSCALE = 1.f;
	float dwEndSCALE = 1.f;
	float dwEndSCALE_BACK = 1.f;
	float fScale = 1.f;
	MAP_ANISCALEDATA_ITER iter = m_mapAniScaleData.begin();
	for( ; iter!=m_mapAniScaleData.end(); ++iter )
	{
		fEndTime = (*iter).first*UnitTime - fSTime;

		if( emType == UPDATE_X ) dwEndSCALE = (*iter).second.x;
		if( emType == UPDATE_Y ) dwEndSCALE = (*iter).second.y;
		if( emType == UPDATE_Z ) dwEndSCALE = (*iter).second.z;

		if( (*iter).first == 0 )
		{
			dwEndSCALE_BACK = dwEndSCALE;
		}

		if( fTime >= fStartTime && fTime < fEndTime )
		{
			float fDiffScale = (dwEndSCALE-dwStartSCALE) / (fEndTime-fStartTime);
			fScale = dwStartSCALE;
			fScale += (fTime-fStartTime)*fDiffScale;

			goto _RETURN;
		}
		else if( fTime == fEndTime )
		{
			fScale = dwEndSCALE;

			goto _RETURN;
		}

		fStartTime = fEndTime;
		dwStartSCALE = dwEndSCALE;
	}

	fEndTime = fETime;
	dwEndSCALE = dwEndSCALE_BACK;

	if( fTime >= fStartTime && fTime < fEndTime )
	{
		float fDiffScale = (dwEndSCALE-dwStartSCALE) / (fEndTime-fStartTime);
		fScale = dwStartSCALE;
		fScale += (fTime-fStartTime)*fDiffScale;

		goto _RETURN;
	}
	else if( fTime == fEndTime )
	{
		fScale = dwEndSCALE;

		goto _RETURN;
	}

_RETURN:
	if( emType == UPDATE_X ) 
	{
		pBoneTrans->vBoneScale.x *= fScale*fWeight;
		/*
		if( bFirst )	pBoneTrans->m_vScale.x = fScale*fWeight;
		else			pBoneTrans->m_vScale.x += fScale*fWeight;
		*/
	}
	if( emType == UPDATE_Y )
	{
		pBoneTrans->vBoneScale.y *= fScale*fWeight;
		/*
		if( bFirst )	pBoneTrans->m_vScale.y = fScale*fWeight;
		else			pBoneTrans->m_vScale.y += fScale*fWeight;
		*/
	}
	if( emType == UPDATE_Z ) 
	{
		pBoneTrans->vBoneScale.z *= fScale*fWeight;
		/*
		if( bFirst )	pBoneTrans->m_vScale.z = fScale*fWeight;
		else			pBoneTrans->m_vScale.z += fScale*fWeight;
		*/
	}
}

void DxAniScaleMAIN::InsertData(DWORD dwKey, const D3DXVECTOR3& vScale)
{
	// Note : 여기까지 왔다면 값이 없는 것이다. 새로 만들어야 한다.
	m_mapAniScaleData.insert(MAP_ANISCALEDATA_VALUE(dwKey, vScale));
}

void DxAniScaleMAIN::ModifyData(int nIndex, const D3DXVECTOR3& vScale)
{
	int nCount(0);
	MAP_ANISCALEDATA_ITER iter = m_mapAniScaleData.begin();
	for ( ; iter!=m_mapAniScaleData.end(); ++iter, ++nCount)
	{
		if (nIndex == nCount)
		{
			(*iter).second = vScale;
			return;
		}
	}
}

void DxAniScaleMAIN::DeleteData(int nIndex)
{
	int nCount(0);
	MAP_ANISCALEDATA_ITER iter = m_mapAniScaleData.begin();
	for( ; iter!=m_mapAniScaleData.end(); ++iter, ++nCount )
	{
		if( nIndex==nCount )
		{
			m_mapAniScaleData.erase( iter );
			return;
		}
	}
}

void DxAniScaleMAIN::Save( sc::SerialFile& SFile )
{
	SFile << (DWORD)m_mapAniScaleData.size();
	MAP_ANISCALEDATA_ITER iter = m_mapAniScaleData.begin();
	for( ; iter!=m_mapAniScaleData.end(); ++iter )
	{
		SFile << (*iter).first;
		SFile << (*iter).second;
	}
}

void DxAniScaleMAIN::Load( sc::BaseStream& SFile )
{
	m_mapAniScaleData.clear();

	DWORD dwFirst(0);
	D3DXVECTOR3 vSecond(0.f,0.f,0.f);
	DWORD dwSize(0);
	SFile >> dwSize;
	for( DWORD i=0; i<dwSize; ++i )
	{
		SFile >> dwFirst;
		SFile >> vSecond;

		m_mapAniScaleData.insert( std::make_pair( dwFirst, vSecond ) );
	}
}

DxAniScaleMAIN& DxAniScaleMAIN::operator = ( DxAniScaleMAIN &value )
{
	m_mapAniScaleData.clear();

	MAP_ANISCALEDATA_ITER iter = value.m_mapAniScaleData.begin();
	for( ; iter!=value.m_mapAniScaleData.end(); ++iter )
	{
		m_mapAniScaleData.insert( std::make_pair( (*iter).first, (*iter).second ) );
	}

	return *this;
}


//------------------------------------------------------------------------------------------------------------
//								D	x		A	n	i		S	c	a	l	e
//------------------------------------------------------------------------------------------------------------
DxAniScale::DxAniScale()
{
}

DxAniScale::~DxAniScale()
{
	CleanUp();
}

void DxAniScale::CleanUp()
{
	std::for_each( m_mapAniScaleMain.begin(), m_mapAniScaleMain.end(), std_afunc::DeleteMapObject() );
	m_mapAniScaleMain.clear();
}

void DxAniScale::UpdateBoneScale( DxSkeleton* pSkeleton, float fWeight, BOOL bFirst, float fTime, float fSTime, float fETime, float UnitTime )
{
    DxAniScaleMAIN* pAniScale = NULL;
    DxBoneTrans* pBoneTrans = NULL;
	MAP_ANISCALEMAIN_ITER iter = m_mapAniScaleMain.begin();
	for ( ; iter!=m_mapAniScaleMain.end(); ++iter)
	{
		pBoneTrans = pSkeleton->FindBone_t((*iter).first);
		if (pBoneTrans)
        {
            pAniScale = iter->second;
		    pAniScale->UpdateBoneScale( pBoneTrans, fWeight, bFirst, fTime, fSTime, fETime, UnitTime, UPDATE_X );
		    pAniScale->UpdateBoneScale( pBoneTrans, fWeight, bFirst, fTime, fSTime, fETime, UnitTime, UPDATE_Y );
		    pAniScale->UpdateBoneScale( pBoneTrans, fWeight, bFirst, fTime, fSTime, fETime, UnitTime, UPDATE_Z );
        }
	}
}

void DxAniScale::InsertAniScale(const std::string& AniName)
{
	MAP_ANISCALEMAIN_ITER iter = m_mapAniScaleMain.find(AniName);
	if (iter == m_mapAniScaleMain.end())
    {
	    DxAniScaleMAIN* pNew = new DxAniScaleMAIN;
	    m_mapAniScaleMain.insert(MAP_ANISCALEMAIN_VALUE(AniName, pNew));
    }
}

void DxAniScale::DeleteAniScale(int nIndex)
{
	int nCount(0);
	MAP_ANISCALEMAIN_ITER iter = m_mapAniScaleMain.begin();
	for ( ; iter!=m_mapAniScaleMain.end(); ++iter, ++nCount)
	{
		if (nIndex==nCount)
		{
			m_mapAniScaleMain.erase(iter);
			return;
		}
	}
}

DxAniScaleMAIN* DxAniScale::GetAniScaleMAIN(int nIndex)
{
	int nCount(0);
	MAP_ANISCALEMAIN_ITER iter = m_mapAniScaleMain.begin();
	for ( ; iter!=m_mapAniScaleMain.end(); ++iter, ++nCount)
	{
		if (nIndex == nCount)
			return iter->second;
	}
	return NULL;
}

void DxAniScale::Save( sc::SerialFile& SFile )
{
	SFile << (DWORD)m_mapAniScaleMain.size();
	MAP_ANISCALEMAIN_ITER iter = m_mapAniScaleMain.begin();
	for( ; iter!=m_mapAniScaleMain.end(); ++iter )
	{
		SFile << (*iter).first;
		(*iter).second->Save( SFile );
	}
}

void DxAniScale::Load( sc::BaseStream& SFile )
{
	CleanUp();

	TSTRING strAniName;
	DWORD dwSize(0);
	SFile >> dwSize;
	for( DWORD i=0; i<dwSize; ++i )
	{
		SFile >> strAniName;
		DxAniScaleMAIN* pNew = new DxAniScaleMAIN;
		pNew->Load( SFile );

		m_mapAniScaleMain.insert(MAP_ANISCALEMAIN_VALUE(strAniName, pNew));
	}
}

DxAniScale& DxAniScale::operator = ( DxAniScale &value )
{
	CleanUp();

	MAP_ANISCALEMAIN_ITER iter = value.m_mapAniScaleMain.begin();
	for( ; iter!=value.m_mapAniScaleMain.end(); ++iter )
	{
		DxAniScaleMAIN* pNew = new DxAniScaleMAIN;
		*pNew = *(*iter).second;

		m_mapAniScaleMain.insert(MAP_ANISCALEMAIN_VALUE((*iter).first, pNew));
	}
	return *this;
}



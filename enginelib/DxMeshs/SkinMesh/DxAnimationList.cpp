#include "pch.h"

#include "../../../../RanThirdParty/DirectX/Include/rmxfguid.h"


#include "../../../SigmaCore/Util/Util.h"

#include "../../DxTools/RENDERPARAM.h"
#include "../../DxTools/DxMethods.h"
#include "../../Common/stlfunctions.h"
#include "../../Common/ByteStream.h"
#include "../../DxCommon/dxutil.h"
#include "../../DxCommon/xrmxftmpl.h"
#include "../../Archive/Archive.h"

#include "../DxAniKeys.h"

#include "./DxSkeletonManager.h"
#include "./DxSkinAnimationManager.h"
#include "./SAnimation.h"

#include "DxAnimationList.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

const TCHAR DxAnimationList::szFILETYPE[] = _T("AnimContainer");

DxAnimationList::DxAnimationList()
	: m_dwRefCount(0)
	, m_vStartPos_Bip01(0.f,0.f,0.f)
{
}

DxAnimationList::~DxAnimationList()
{
	CleanUp();
}

void DxAnimationList::CleanUp()
{
	std::for_each(
		m_listAniROOT.begin(),
		m_listAniROOT.end(),
		std_afunc::DeleteObject() );
	m_listAniROOT.clear();
	
	std::for_each(
		m_listAnimation.begin(),
		m_listAnimation.end(),
		std_afunc::DeleteObject() );
	m_listAnimation.clear();

	std::for_each(
		m_listAniUPBODY.begin(),
		m_listAniUPBODY.end(),
		std_afunc::DeleteObject() );
	m_listAniUPBODY.clear();
}

//! xFile에서 에니메이션 데이터를 로딩.
HRESULT DxAnimationList::LoadAnimList_XFile( LPDIRECT3DDEVICEQ pd3dDevice,
										SANIMCONINFO &sAnimInfo,										
										DxSkeleton* pSkeleton )
{
	HRESULT hr = S_OK;

	CleanUp();

	LPDIRECTXFILE pxofapi = NULL;
	LPDIRECTXFILEENUMOBJECT pxofenum = NULL;
	LPDIRECTXFILEDATA pxofobjCur = NULL;
    
	// xFile 개체를 만듬.
	hr = DirectXFileCreate(&pxofapi);
	if (FAILED(hr))	goto e_Exit;
    
    // xFile 템플리트 등록.
	hr = pxofapi->RegisterTemplates(
			(LPVOID)D3DRM_XTEMPLATES,
			D3DRM_XTEMPLATE_BYTES );
	if (FAILED(hr))
		goto e_Exit;
    

	TCHAR szPath[MAX_PATH] = _T("");
	::StringCchCopy( szPath, MAX_PATH, DxSkinAnimationManager::GetInstance().GetPath().c_str() );
	::StringCchCat( szPath, MAX_PATH, sAnimInfo.m_strXFile.c_str() );

    char szAnsiName[MAX_PATH];
#ifdef UNICODE
	WideCharToMultiByte( CP_ACP, 0, szPath, -1, szAnsiName, MAX_PATH, NULL, NULL );
    szAnsiName[MAX_PATH-1] = 0;
#else
    ::StringCchCopy( szAnsiName, MAX_PATH, szPath );
#endif

    // 실제 xFile을 열음.
	hr = pxofapi->CreateEnumObject(
			szAnsiName,
			DXFILELOAD_FROMFILE,
			&pxofenum );
	if (FAILED(hr))	
	{
		goto e_Exit;
	}

	// 최상위 오브젝트를 읽어옴.
	while (SUCCEEDED(pxofenum->GetNextDataObject(&pxofobjCur)))
	{
		const GUID *type;

		hr = pxofobjCur->GetType(&type);
		if (FAILED(hr))	
		{
			goto e_Exit;
		}

		// 에니메이션 오브젝트.
		if ( *type == TID_D3DRMAnimationSet )
		{
			LoadAnimationSet( pxofobjCur, sAnimInfo, pSkeleton );
		}
		else if ( *type == TID_D3DRMAnimation )
		{
			LoadAnimation( pxofobjCur, sAnimInfo, pSkeleton );
		}

		GXRELEASE(pxofobjCur);
        
		if (FAILED(hr))
		{
			goto e_Exit;
		}
	}


	if ( sAnimInfo.m_dwFlag & ACF_NEWINFO )
	{
		sAnimInfo.m_dwFlag &= ~ACF_NEWINFO;

		// Note 
		SaveToFile( sAnimInfo.m_strXFile.c_str() );
	}

e_Exit:
    GXRELEASE(pxofobjCur);
    GXRELEASE(pxofenum);
    GXRELEASE(pxofapi);
    
    return hr;
}

//! 에니메이션을 읽어옴.
HRESULT DxAnimationList::LoadAnimation( IDirectXFileData* pxofobjCur, 
									SANIMCONINFO& sAniContInfo, 
									DxSkeleton* pSkeleton )
{
	HRESULT hr = S_OK;
	SRotateKeyXFile *pFileRotateKey;
	SScaleKeyXFile *pFileScaleKey;
	SPositionKeyXFile *pFilePosKey;
	SMatrixKeyXFile *pFileMatrixKey;
	SAnimation *pAnimationCur;
	LPDIRECTXFILEDATA pxofobjChild = NULL;
	LPDIRECTXFILEOBJECT pxofChild = NULL;
	LPDIRECTXFILEDATAREFERENCE pxofobjChildRef = NULL;
	const GUID *type;
	DWORD dwSize;
	PBYTE pData;
	DWORD dwKeyType;
	DWORD cKeys;
	DWORD iKey;
	DWORD cchName;
	char *szBoneName;
    
	// 새로운 Animation 만듬.
	pAnimationCur = new SAnimation();
	if (pAnimationCur == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto e_Exit;
	}
    
	// 순차적으로 탐색하면서 에니메이션을 읽어온다.
	// QueryInterface()를 사용하여 오브젝트의 타입을 확인함.
	while ( SUCCEEDED(pxofobjCur->GetNextObject(&pxofChild)) )
	{
        // Object가 "FileDataReference"일때.
		hr = pxofChild->QueryInterface(
				IID_IDirectXFileDataReference,
				(LPVOID *)&pxofobjChildRef );
		if (SUCCEEDED(hr))
		{
			hr = pxofobjChildRef->Resolve( &pxofobjChild );
			if (SUCCEEDED(hr))
			{
				hr = pxofobjChild->GetType(&type);
				if (FAILED(hr))
					goto e_Exit;
                
				// 에니에이션에 연결된 프레임을 지정.
				if ( TID_D3DRMFrame == *type )
				{
					if ( pAnimationCur->m_pBoneToAnimate != NULL )
					{
						hr = E_INVALIDARG;
						goto e_Exit;
					}
                    
					// 연결된 프레임의 이름을 가져옴.
					hr = pxofobjChild->GetName( NULL, &cchName );
					if (FAILED(hr))
						goto e_Exit;
                    
					if (cchName == 0)
					{
						hr = E_INVALIDARG;
						goto e_Exit;
					}
                    
					szBoneName = (char*)_alloca(cchName);
					if (szBoneName == NULL)
					{
						hr = E_OUTOFMEMORY;
						goto e_Exit;
					}
                    
					hr = pxofobjChild->GetName( szBoneName, &cchName );
					if (FAILED(hr))
						goto e_Exit;
                    
                    //	NEED : 연결된 "BONE" 을 지정.
					//
					pAnimationCur->m_pBoneToAnimate = pSkeleton->FindBone_t ( szBoneName );
					if ( !pAnimationCur->m_pBoneToAnimate )
					{
						//hr = E_INVALIDARG;  // 본이 없을 경우는 현재 애니메이션은 연결 안해줘도 된다.
						goto e_Exit;
					}

					// 연결된 "BONE" 이름 저장.
					pAnimationCur->m_strBone = szBoneName;
				}
                
				GXRELEASE(pxofobjChild);

			}	//	Resolve()
            
			GXRELEASE(pxofobjChildRef);

		}	// IID_IDirectXFileDataReference ?
		else
		{
            // Object가 "FileData"일 경우.
			hr = pxofChild->QueryInterface(
					IID_IDirectXFileData,
					(LPVOID *) &pxofobjChild);
			if (SUCCEEDED(hr))
			{
				hr = pxofobjChild->GetType(&type);
				if (FAILED(hr))
				{
					goto e_Exit;
				}
                
				if ( TID_D3DRMAnimationKey == *type )
				{
					hr = pxofobjChild->GetData( NULL, &dwSize, (PVOID*)&pData );
					if (FAILED(hr))
					{
						goto e_Exit;
					}
                    
					dwKeyType = ((DWORD*)pData)[0];
					cKeys = ((DWORD*)pData)[1];

					DWORD dwEndTime = 0;
					DWORD dwUnitKey = UINT_MAX;
					
					// 쿼터니언 회전 키.
					if ( dwKeyType == 0 )
					{
						if ( pAnimationCur->m_pRotateKeys != NULL )
						{
							hr = E_INVALIDARG;
							goto e_Exit;
						}
                        
						// 회전키 생성.
						pAnimationCur->m_pRotateKeys = new SRotateKey[cKeys];
						if ( pAnimationCur->m_pRotateKeys == NULL )
						{
							hr = E_OUTOFMEMORY;
							goto e_Exit;
						}
                        
						//	회전키의 개수.
                        pAnimationCur->m_cRotateKeys = cKeys;
                        
						// 쿼터니언 값.
                        pFileRotateKey =  (SRotateKeyXFile*)(pData + (sizeof(DWORD) * 2));
                        for (iKey = 0;iKey < cKeys; iKey++)
                        {
							//	현재 키의 시간.
                            pAnimationCur->m_pRotateKeys[iKey].dwTime = pFileRotateKey->dwTime;

                            pAnimationCur->m_pRotateKeys[iKey].quatRotate.x = pFileRotateKey->x;
                            pAnimationCur->m_pRotateKeys[iKey].quatRotate.y = pFileRotateKey->y;
                            pAnimationCur->m_pRotateKeys[iKey].quatRotate.z = pFileRotateKey->z;
                            pAnimationCur->m_pRotateKeys[iKey].quatRotate.w = pFileRotateKey->w;
                            
							if ( iKey!=0 )
							{
								DWORD dxTime = pAnimationCur->m_pRotateKeys[iKey].dwTime - pAnimationCur->m_pRotateKeys[iKey-1].dwTime;
								if (  dxTime < dwUnitKey )
									dwUnitKey = dxTime;
							}

                            pFileRotateKey += 1;
                        }

						dwEndTime = pAnimationCur->m_pRotateKeys[cKeys-1].dwTime;
                    }
					//	Note : 스케일 키.
					//
                    else if (dwKeyType == 1)
                    {
                        if (pAnimationCur->m_pScaleKeys != NULL)
                        {
                            hr = E_INVALIDARG;
                            goto e_Exit;
                        }
                
						//	Note : 스케일키 생성.
						//
                        pAnimationCur->m_pScaleKeys = new SScaleKey[cKeys];
                        if (pAnimationCur->m_pScaleKeys == NULL)
                        {
                            hr = E_OUTOFMEMORY;
                            goto e_Exit;
                        }
                        
						//	스케일키 개수.
                        pAnimationCur->m_cScaleKeys = cKeys;
                        
						//	Note : 스케일키 값.
						//
                        pFileScaleKey =  (SScaleKeyXFile*)(pData + (sizeof(DWORD) * 2));
                        for (iKey = 0;iKey < cKeys; iKey++)
                        {
							//	현재 키의 시간.
                            pAnimationCur->m_pScaleKeys[iKey].dwTime = pFileScaleKey->dwTime;
                            
							pAnimationCur->m_pScaleKeys[iKey].vScale = pFileScaleKey->vScale;

							if ( iKey!=0 )
							{
								DWORD dxTime = pAnimationCur->m_pScaleKeys[iKey].dwTime - pAnimationCur->m_pScaleKeys[iKey-1].dwTime;
								if (  dxTime < dwUnitKey )
									dwUnitKey = dxTime;
							}
                            
                            pFileScaleKey += 1;
                        }

						dwEndTime = pAnimationCur->m_pScaleKeys[cKeys-1].dwTime;
                    }
					//	Note : 이동 키.
					//
                    else if (dwKeyType == 2)
                    {
                        if (pAnimationCur->m_pPositionKeys != NULL)
                        {
                            hr = E_INVALIDARG;
                            goto e_Exit;
                        }
                        
						//	Note : 이동 키 생성.
						//
                        pAnimationCur->m_pPositionKeys = new SPositionKey[cKeys];
                        if (pAnimationCur->m_pPositionKeys == NULL)
                        {
                            hr = E_OUTOFMEMORY;
                            goto e_Exit;
                        }
                        
						//	이동키 개수.
                        pAnimationCur->m_cPositionKeys = cKeys;
                        
						//	Note : 이동키 값.
						//
                        pFilePosKey =  (SPositionKeyXFile*)(pData + (sizeof(DWORD) * 2));
                        for (iKey = 0;iKey < cKeys; iKey++)
                        {
							//	현재 키의 시간.
                            pAnimationCur->m_pPositionKeys[iKey].dwTime = pFilePosKey->dwTime;
                            
							pAnimationCur->m_pPositionKeys[iKey].vPos = pFilePosKey->vPos;

							if ( iKey!=0 )
							{
								DWORD dxTime = pAnimationCur->m_pPositionKeys[iKey].dwTime - pAnimationCur->m_pPositionKeys[iKey-1].dwTime;
								if (  dxTime < dwUnitKey )
									dwUnitKey = dxTime;
							}
                            
                            pFilePosKey += 1;
                        }

						dwEndTime = pAnimationCur->m_pPositionKeys[cKeys-1].dwTime;
                    }
					//	Note : 트렌스폼 키.
					//
                    else if (dwKeyType == 4)
                    {
						if ( pAnimationCur->m_pMatrixKeys!=NULL )
                        {
                            hr = E_INVALIDARG;
                            goto e_Exit;
                        }
                        
						//	Note : 트렌스폼 키 생성.
						//
						pAnimationCur->m_pMatrixKeys = new SMatrixKey[cKeys];
						if (pAnimationCur->m_pMatrixKeys == NULL)
						{
							hr = E_OUTOFMEMORY;
							goto e_Exit;
						}

						//	트렌스폼 키 개수.
                        pAnimationCur->m_cMatrixKeys = cKeys;
                        
						//	Note : 트렌스폼 키 값.
						//
                        pFileMatrixKey =  (SMatrixKeyXFile*)(pData + (sizeof(DWORD) * 2));
                        for (iKey = 0;iKey < cKeys; iKey++)
                        {
							//	현재 프레임의 시간.
                            pAnimationCur->m_pMatrixKeys[iKey].dwTime = pFileMatrixKey->dwTime;
                            
							pAnimationCur->m_pMatrixKeys[iKey].mat = pFileMatrixKey->mat;
                            
							if (iKey != 0)
							{
								DWORD dwTime = pAnimationCur->m_pMatrixKeys[iKey].dwTime - pAnimationCur->m_pMatrixKeys[iKey-1].dwTime;
								if (dwTime < dwUnitKey)
									dwUnitKey = dwTime;
							}

                            pFileMatrixKey += 1;
                        }

						// Optimize 
						pAnimationCur->m_fDivFullTime = 1.f/pAnimationCur->m_pMatrixKeys[cKeys-1].dwTime;
						pAnimationCur->m_fMatrixKeys = static_cast<float> (pAnimationCur->m_cMatrixKeys);

						dwEndTime = pAnimationCur->m_pMatrixKeys[cKeys-1].dwTime;

						//////////////////////////////////////////////////////////////////////////
						// Position 값을 Bone 정보로 셋팅 할 필요가 없다.
						// 애니메이션의 Position 값을 따라가야 해서 NULL 이 들어가야 한다.
						//
						//// Convert Quaternion
						//if (pAnimationCur->m_pBoneToAnimate)
						//{
					 //       pAnimationCur->ConvertMatToQuat( pAnimationCur->m_pBoneToAnimate );
						//}
						//else
						//{
						//	pAnimationCur->ConvertMatToQuat( NULL );
						//}
						//
						pAnimationCur->ConvertMatToQuat( NULL );
                    }
                    else
                    {
                        hr = E_INVALIDARG;
                        goto e_Exit;
                    }
					
					sAniContInfo.m_dwETimeOrig = dwEndTime;

					if ( (sAniContInfo.m_UNITTIME>dwUnitKey) )
						sAniContInfo.m_UNITTIME = dwUnitKey;

					if ( sAniContInfo.m_dwETime>sAniContInfo.m_dwETimeOrig )
					{
						sAniContInfo.m_dwETime = sAniContInfo.m_dwETimeOrig;
						if (sAniContInfo.m_dwSTime > sAniContInfo.m_dwETime)
                            sAniContInfo.m_dwSTime = sAniContInfo.m_dwETime;
					}
                }
                
                GXRELEASE(pxofobjChild);
            }
        }
        
        GXRELEASE(pxofChild);
    }

	//	Note : 에니메이션 리스트에 등록.
	switch( pAnimationCur->m_pBoneToAnimate->m_emBody )
	{
	case EMBODY_ROOT:
		m_listAniROOT.push_back( pAnimationCur );
		break;
	case EMBODY_DOWNBODY:
		m_listAnimation.push_back( pAnimationCur );
		break;
	case EMBODY_UPBODY:
		m_listAniUPBODY.push_back( pAnimationCur );
		break;
	};

	GXRELEASE(pxofobjChild);
    GXRELEASE(pxofChild);
    GXRELEASE(pxofobjChildRef);
	return hr;
    
e_Exit:
	SAFE_DELETE( pAnimationCur );
    GXRELEASE(pxofobjChild);
    GXRELEASE(pxofChild);
    GXRELEASE(pxofobjChildRef);
    return hr;
}

//! AnimationSet 를 읽어와 구성함.
HRESULT DxAnimationList::LoadAnimationSet( IDirectXFileData* pxofobjCur, 
										SANIMCONINFO& sAniContInfo, 
										DxSkeleton* pSkeleton )
{
    const GUID *type;
    HRESULT hr = S_OK;
    LPDIRECTXFILEDATA pxofobjChild = NULL;
    LPDIRECTXFILEOBJECT pxofChild = NULL;
    
   
	// 순차적으로 탐색하면서 에니메이션을 읽어온다.
	// QueryInterface()를 사용하여 오브젝트의 타입을 확인함.
    while (SUCCEEDED(pxofobjCur->GetNextObject(&pxofChild)))
    {
        // Query the child for it's FileData
        hr = pxofChild->QueryInterface ( IID_IDirectXFileData, (LPVOID *)&pxofobjChild );
        
		if (SUCCEEDED(hr))
        {
            hr = pxofobjChild->GetType(&type);
            if (FAILED(hr))
                goto e_Exit;
            
            if( TID_D3DRMAnimation == *type )
            {
                hr = LoadAnimation( pxofobjChild, sAniContInfo, pSkeleton );
                if (FAILED(hr))
                    goto e_Exit;
            }
            
            GXRELEASE(pxofobjChild);
        }
        
        GXRELEASE(pxofChild);
    }
    
e_Exit:
    GXRELEASE(pxofobjChild);
    GXRELEASE(pxofChild);
    return hr;
}

void DxAnimationList::UpdateTime_PureThread( float fCurTime,
							float fWeight,
							BOOL bFirst,
							EMBODY emBody )
{
	SANILIST_ITER iter;
    SANILIST_ITER iterEnd;
	switch (emBody)
	{
	case EMBODY_DEFAULT:
		iter    = m_listAniROOT.begin();
        iterEnd = m_listAniROOT.end();
		for ( ; iter!=iterEnd; ++iter)
		{
			(*iter)->SetTime_PureThread( fCurTime, fWeight, bFirst);
		}

		iter    = m_listAniUPBODY.begin();
        iterEnd = m_listAniUPBODY.end();
		for ( ; iter!=iterEnd; ++iter)
		{
			(*iter)->SetTime_PureThread( fCurTime, fWeight, bFirst);
		}

		iter    = m_listAnimation.begin();
        iterEnd = m_listAnimation.end();
		for ( ; iter!=iterEnd; ++iter)
		{
			(*iter)->SetTime_PureThread( fCurTime, fWeight, bFirst);
		}

		break;

	case EMBODY_ROOT:
		iter    = m_listAniROOT.begin();
        iterEnd = m_listAniROOT.end();
		for ( ; iter!=iterEnd; ++iter)
		{
			(*iter)->SetTime_PureThread( fCurTime, fWeight, bFirst);
		}
		break;

	case EMBODY_UPBODY:
		iter    = m_listAniUPBODY.begin();
        iterEnd = m_listAniUPBODY.end();
		for ( ; iter!=iterEnd; ++iter)
		{
			(*iter)->SetTime_PureThread(  fCurTime, fWeight, bFirst );
		}
		break;

	case EMBODY_DOWNBODY:
		iter    = m_listAnimation.begin();
        iterEnd = m_listAnimation.end();
		for ( ; iter!=iterEnd; ++iter)
		{
			(*iter)->SetTime_PureThread( fCurTime, fWeight, bFirst);
		}
		break;

	default:
		break;
	};
}

void DxAnimationList::UpdateMix_PureThread( float fCurMixTime )
{
	SANILIST_ITER iter = m_listAniROOT.begin();
	SANILIST_ITER iter_end = m_listAniROOT.end();
	for( ; iter!=iter_end; ++iter )
	{
		(*iter)->SetMix_PureThread( fCurMixTime );
	}

	iter = m_listAniUPBODY.begin();
	iter_end = m_listAniUPBODY.end();
	for( ; iter!=iter_end; ++iter )
	{
		(*iter)->SetMix_PureThread( fCurMixTime );
	}

	iter = m_listAnimation.begin();
	iter_end = m_listAnimation.end();
	for( ; iter!=iter_end; ++iter )
	{
		(*iter)->SetMix_PureThread( fCurMixTime );
	}
	//std::for_each(
	//	m_listAniROOT.begin(),
	//	m_listAniROOT.end(),
	//	std::bind2nd(std::mem_fun(&SAnimation::SetMix),
	//	fCurMixTime) );

	//std::for_each(
	//	m_listAniUPBODY.begin(),
	//	m_listAniUPBODY.end(),
	//	std::bind2nd(std::mem_fun(&SAnimation::SetMix),
	//	fCurMixTime) );

	//std::for_each(
	//	m_listAnimation.begin(),
	//	m_listAnimation.end(),
	//	std::bind2nd(std::mem_fun(&SAnimation::SetMix),
	//	fCurMixTime) );
}

void DxAnimationList::UpdateBoneBip01_PureThread( float fCurTime, float fWeight, BOOL bFirst )
{
	SANILIST_ITER iter = m_listAniROOT.begin();
	for( ; iter!=m_listAniROOT.end(); ++iter )
	{
		(*iter)->SetTime_PureThread( fCurTime, fWeight, bFirst );
	}
}

HRESULT DxAnimationList::SaveToFile( const TCHAR* pNameXFile )
{
	TSTRING strBinFile = sc::util::ChangeExtName( pNameXFile, _T("bin") );

	TSTRING strPath;
	strPath += DxSkinAnimationManager::GetInstance().GetPath();
	strPath += strBinFile.c_str();

	sc::SerialFile	SFile;
	SFile.SetFileType( szFILETYPE, VERSION );

	if ( !SFile.OpenFile( FOT_WRITE, strPath.c_str() ) )		return E_FAIL;
	
	SANILIST_ITER iter;
	SANILIST_ITER iter_end;

	{
		SFile << (DWORD)m_listAniROOT.size();

		iter = m_listAniROOT.begin();
		iter_end = m_listAniROOT.end();
		for ( ; iter!=iter_end; ++iter )
		{
			(*iter)->SaveToFile(SFile);
		}
	}

	{
		SFile << (DWORD)m_listAnimation.size();
		
		iter = m_listAnimation.begin();
		iter_end = m_listAnimation.end();
		for ( ; iter!=iter_end; ++iter )
		{
			(*iter)->SaveToFile(SFile);
		}
	}

	{
		SFile << (DWORD)m_listAniUPBODY.size();
		
		iter = m_listAniUPBODY.begin();
		iter_end = m_listAniUPBODY.end();
		for ( ; iter!=iter_end; ++iter )
		{
			(*iter)->SaveToFile(SFile);
		}
	}

	return S_OK;
}

HRESULT DxAnimationList::LoadAnimList_BIN(void* pBuf,UINT bufSize, const DxSkeleton* pSkeleton )
{
	se::ByteStream stream( LPBYTE(pBuf),bufSize );
	stream.ReadFileType();
	return LoadAnimList_BIN(stream,pSkeleton);
}

HRESULT DxAnimationList::LoadAnimList_BIN( const TSTRING &strNameBIN, const DxSkeleton* pSkeleton, boost::function<void (const TCHAR*)> pArchive )
{
	TSTRING strFullPath = DxSkinAnimationManager::GetInstance().GetPath();
	strFullPath += strNameBIN;

    SPSerialFile spFile;

    if (Archive::CreateSerialFile(pArchive, spFile) == false)
    {
        return E_FAIL;
    }

    sc::SerialFile& SFile = *spFile;

	if ( !SFile.OpenFile( FOT_READ, strFullPath.c_str() ) )
		return E_FAIL;

	return LoadAnimList_BIN(SFile,pSkeleton);
}

HRESULT DxAnimationList::LoadAnimList_BIN( sc::BaseStream& stream, const DxSkeleton* pSkeleton )
{
	CleanUp();

	TCHAR szFileType[FILETYPESIZE] = _T("");
	DWORD dwVersion;
	stream.GetFileType( szFileType, FILETYPESIZE, dwVersion );

	HRESULT hr(S_OK);

	if( dwVersion==VERSION )
	{
		DWORD dwNumAni(0);
		stream >> dwNumAni;
		for ( DWORD i=0; i<dwNumAni; ++i )
		{
			SAnimation *pAnimation = new SAnimation;
			hr = pAnimation->LoadFromFile( stream, pSkeleton );
			if( hr == S_OK )
			{
				m_listAniROOT.push_back ( pAnimation );

				// Bip01일 경우 초기 위치값 셋팅.
				if( pAnimation->m_strBone == _T("Bip01") )
				{
					m_vStartPos_Bip01 = pAnimation->m_pQuatPosKeys->m_vPos;
				}
			}
			else
			{
				SAFE_DELETE( pAnimation );
			}
		}

		stream >> dwNumAni;
		for ( DWORD i=0; i<dwNumAni; ++i )
		{
			SAnimation *pAnimation = new SAnimation;
			hr = pAnimation->LoadFromFile( stream, pSkeleton );
			if( hr == S_OK )
			{
				m_listAnimation.push_back ( pAnimation );
			}
			else
			{
				SAFE_DELETE( pAnimation );
			}
		}

		stream >> dwNumAni;
		for ( DWORD i=0; i<dwNumAni; ++i )
		{
			SAnimation *pAnimation = new SAnimation;
			hr = pAnimation->LoadFromFile( stream, pSkeleton );
			if( hr == S_OK )
			{
				m_listAniUPBODY.push_back ( pAnimation );
			}
			else
			{
				SAFE_DELETE( pAnimation );
			}
		}
	}
	else if( dwVersion==0x0103)
	{
		DWORD dwNumAni(0);
		stream >> dwNumAni;
		for ( DWORD i=0; i<dwNumAni; ++i )
		{
			SAnimation *pAnimation = new SAnimation;
			pAnimation->LoadFromFile( stream, pSkeleton );

			m_listAniUPBODY.push_back ( pAnimation );
		}

		DWORD dwGarbage;
		stream >> dwGarbage;

		stream >> dwNumAni;
		for ( DWORD i=0; i<dwNumAni; ++i )
		{
			SAnimation *pAnimation = new SAnimation;
			pAnimation->LoadFromFile( stream, pSkeleton );

			m_listAnimation.push_back ( pAnimation );
		}
	}
	else if( dwVersion==0x0102)
	{
		DWORD dwNumAni(0);
		stream >> dwNumAni;
		for ( DWORD i=0; i<dwNumAni; ++i )
		{
			SAnimation *pAnimation = new SAnimation;
			pAnimation->LoadFromFile( stream, pSkeleton );
	
			m_listAniUPBODY.push_back ( pAnimation );
		}
	
		stream >> dwNumAni;
		for ( DWORD i=0; i<dwNumAni; ++i )
		{
			SAnimation *pAnimation = new SAnimation;
			pAnimation->LoadFromFile( stream, pSkeleton );
	
			m_listAnimation.push_back ( pAnimation );
		}
	}
	else if( dwVersion==0x0101 )
	{
		DWORD dwNumAni(0);
		stream >> dwNumAni;
		for ( DWORD i=0; i<dwNumAni; ++i )
		{
			SAnimation *pAnimation = new SAnimation;
			hr = pAnimation->LoadFromFile( stream, pSkeleton );
			if( hr == S_OK )
			{
				m_listAnimation.push_back ( pAnimation );
			}
			else
			{
				SAFE_DELETE( pAnimation );
			}
		}

		stream >> dwNumAni;
		for ( DWORD i=0; i<dwNumAni; ++i )
		{
			SAnimation *pAnimation = new SAnimation;
			hr = pAnimation->LoadFromFile( stream, pSkeleton );
			if( hr == S_OK )
			{
				m_listAniUPBODY.push_back ( pAnimation );
			}
			else
			{
				SAFE_DELETE( pAnimation );
			}
		}
	}
	else if( dwVersion==0x0100 )
	{
		DWORD dwNumAni(0);
		stream >> dwNumAni;
		for ( DWORD i=0; i<dwNumAni; ++i )
		{
			SAnimation *pAnimation = new SAnimation;
			hr = pAnimation->LoadFromFile( stream, pSkeleton );
			if( hr == S_OK )
			{
				m_listAnimation.push_back ( pAnimation );
			}
			else
			{
				SAFE_DELETE( pAnimation );
			}
		}
	}
	else
	{
		// [ Thread Work shhan ]
		// cache를 할 필요가 없어서 강제로 셋팅 해준다.
		stream.CloseFile();

		//	지원하지 않는 버전의 파일입니다.
		return E_FAIL;
	}

	// [ Thread Work shhan ]
	// cache를 할 필요가 없어서 강제로 셋팅 해준다.
	stream.CloseFile();

	return S_OK;
}

void DxAnimationList::ReAttachBoneForEdit(DxSkeleton* pSkeleton)
{
    BOOST_FOREACH(SAnimation* pAni, m_listAniROOT)
    {
        pAni->ReAttachBoneForEdit(pSkeleton);
    }

    BOOST_FOREACH(SAnimation* pAni, m_listAnimation)
    {
        pAni->ReAttachBoneForEdit(pSkeleton);
    }

    BOOST_FOREACH(SAnimation* pAni, m_listAniUPBODY)
    {
        pAni->ReAttachBoneForEdit(pSkeleton);
    }
}

//
//HRESULT SAnimList::LoadAnimFromFile(
//	const TSTRING &strFilePath,
//	DxSkeleton* pSkeleton,
//	SANILIST &listANIROOT,
//	SANILIST &listANI,
//	SANILIST &listANIUPBODY )
//{
//	CORE_COMMON::CSerialFile SFile;
//	if( strFilePath.empty() ) 
//	{
//		return E_FAIL;
//	}
//	if ( !SFile.OpenFile( CORE_COMMON::FOT_READ, strFilePath.c_str() ) )
//	{
//		return E_FAIL;
//	}
//
//	TCHAR szFileType[FILETYPESIZE] = _T("");
//	DWORD dwVersion;
//	SFile.GetFileType( szFileType, FILETYPESIZE, dwVersion );
//	if ( !((VERSION==dwVersion) || (0x0100==dwVersion))  )
//	{
//		//	지원하지 않는 버전의 파일입니다.
//		return E_FAIL;
//	}
//
//	HRESULT hr(S_OK);
//
//	if( dwVersion==VERSION )
//	{
//		DWORD dwNumAni(0);
//		SFile >> dwNumAni;
//		for ( DWORD i=0; i<dwNumAni; ++i )
//		{
//			SAnimation *pAnimation = new SAnimation;
//			hr = pAnimation->LoadFromFile(SFile, pSkeleton );
//			if( hr == S_OK )
//			{
//				listANIROOT.push_back ( pAnimation );
//			}
//			else
//			{
//				SAFE_DELETE( pAnimation );
//			}
//		}
//
//		SFile >> dwNumAni;
//		for ( DWORD i=0; i<dwNumAni; ++i )
//		{
//			SAnimation *pAnimation = new SAnimation;
//			hr = pAnimation->LoadFromFile(SFile, pSkeleton );
//			if( hr == S_OK )
//			{
//				listANI.push_back ( pAnimation );
//			}
//			else
//			{
//				SAFE_DELETE( pAnimation );
//			}
//		}
//
//		SFile >> dwNumAni;
//		for ( DWORD i=0; i<dwNumAni; ++i )
//		{
//			SAnimation *pAnimation = new SAnimation;
//			hr = pAnimation->LoadFromFile(SFile, pSkeleton );
//			if( hr == S_OK )
//			{
//				listANIUPBODY.push_back ( pAnimation );
//			}
//			else
//			{
//				SAFE_DELETE( pAnimation );
//			}
//		}
//	}
//	else if( dwVersion==0x0101 )
//	{
//		DWORD dwNumAni(0);
//		SFile >> dwNumAni;
//		for ( DWORD i=0; i<dwNumAni; ++i )
//		{
//			SAnimation *pAnimation = new SAnimation;
//			hr = pAnimation->LoadFromFile(SFile, pSkeleton );
//			if( hr == S_OK )
//			{
//				listANI.push_back ( pAnimation );
//			}
//			else
//			{
//				SAFE_DELETE( pAnimation );
//			}
//		}
//
//		SFile >> dwNumAni;
//		for ( DWORD i=0; i<dwNumAni; ++i )
//		{
//			SAnimation *pAnimation = new SAnimation;
//			hr = pAnimation->LoadFromFile(SFile, pSkeleton );
//			if( hr == S_OK )
//			{
//				listANIUPBODY.push_back ( pAnimation );
//			}
//			else
//			{
//				SAFE_DELETE( pAnimation );
//			}
//		}
//	}
//	else if( dwVersion==0x0100 )
//	{
//		DWORD dwNumAni(0);
//		SFile >> dwNumAni;
//		for ( DWORD i=0; i<dwNumAni; ++i )
//		{
//			SAnimation *pAnimation = new SAnimation;
//			hr = pAnimation->LoadFromFile(SFile, pSkeleton );
//			if( hr == S_OK )
//			{
//				listANI.push_back ( pAnimation );
//			}
//			else
//			{
//				SAFE_DELETE( pAnimation );
//			}
//		}
//	}
//	else
//	{
//		//	지원하지 않는 버전의 파일입니다.
//		return E_FAIL;
//	}
//
//	return S_OK;
//}

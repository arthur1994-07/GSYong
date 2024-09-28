 // DxFrameMesh.cpp: implementation of the DxFrameMesh class.
//
//	Note [02.07.11] : "프레임+에니메이션" 인 파일만을 로딩할 수 있다.
//					여러게 그룹의 에니메이션을 등록 할 수 없다.
#include "pch.h"

#include <algorithm>

#include "../../../VisualMaterialLib/Define/DxVMDefine.h"

#include "../../../SigmaCore/String/StringUtils.h"
#include "../../../SigmaCore/Log/LogMan.h"

#include "../../DxCommon9/DXUT_UTIL.h"
#include "../../DxCommon/xrmxftmpl.h"
#include "../../Common/StlFunctions.h"
#include "../../DxMeshs/FrameMesh/DxAnimationManager.h"
#include "../../DxTools/Collision.h"
#include "../../DxTools/TextureManager.h"
#include "../../DxTools/PrivateResource.h"
#include "../../DxTools/Material/NSMaterialSetting.h"
#include "../../DxTools/Material/MaterialMesh.h"
#include "../../DxTools/Light/NSShadowLight.h"
#include "../../DxEffect/Frame/DxEffectFrame.h"
#include "../../DxEffect/TexEff/DxTextureEffMan.h"
#include "../../DxLand/LightMap/NSLightMapTools.h"
#include "../../DxLand/DxLandMan.h"

#include "./DxReplaceContainer.h"
#include "./NSXFormatLoader.h"

#include "DxFrameMesh.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

BOOL DxFrameMesh::m_bNaviDraw = TRUE;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DxFrame::~DxFrame ()
{
	SAFE_DELETE_ARRAY(szName);
	SAFE_DELETE(pmsMeshs);
	SAFE_DELETE(pframeFirstChild);
	SAFE_DELETE(pframeSibling);
}

DxFrameMesh::DxFrameMesh( BOOL bMeshOptimize ) 
	: m_bMeshOptimize( bMeshOptimize ) ,
	m_dwFVF(NULL),
	m_pDxFrame(NULL),
	pAnimHead(NULL),
	pAnimManHead(NULL),
	m_pszFileName(NULL),
	fCurTime(0.0f),
	m_UNITTIME(FLT_MAX),
	m_dwVertices(0),
	m_dwTriangles(0),
	vTreeMax(0,0,0),
	vTreeMin(0,0,0),
	m_pDxAlphaMapFrameHead(NULL),
	m_fTime(0.0f),
	m_fElapsedTime(0.0f),
	m_pPieceObjHead(NULL),
	m_pPieceObjTree(NULL),
	m_pEffectHead(NULL),
	m_pCollDetectFrame(NULL),
	m_pTextureEff(NULL)

	, m_scpMaterialSetting(new CMaterialSetting)
{
}

DxFrameMesh::~DxFrameMesh()
{
	CleanUp ();
	SAFE_DELETE_ARRAY ( m_pszFileName );
}

// Key Animation 이 하나만 있는 경우는 애니메이션 없앰.
void DxFrameMesh::OneKeyAnimationLinkOff()
{
	std::vector<DxAnimation*> vecAnimation;
	DxAnimation* pCur = pAnimHead;
	while ( pCur )
	{
		vecAnimation.push_back( pCur );
		pCur = pCur->pAnimNext;
	}

	std::vector<DxAnimation*>::iterator iter = vecAnimation.begin();
	for ( ; iter!=vecAnimation.end(); )
	{
		if ( (*iter)->IsAnimationMaxKey() <= 1 )
		{
			if ( (*iter)->pframeToAnimate )
			{
				(*iter)->pframeToAnimate->pframeFromAnimate = NULL;
			}
			(*iter)->pAnimNext = NULL;
			SAFE_DELETE( (*iter) );

			iter = vecAnimation.erase( iter );
		}
		else
		{
			++iter;
		}
	}

	DxAnimation* pPrev(NULL);
	std::vector<DxAnimation*>::reverse_iterator riter = vecAnimation.rbegin();
	for ( ; riter!=vecAnimation.rend(); ++riter )
	{
		(*riter)->pAnimNext = pPrev;
		pPrev = (*riter);
	}
	pAnimHead = pPrev;
}

HRESULT DxFrameMesh::LoadMeshHierarchy ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT ( pd3dDevice != NULL );
	GASSERT ( m_pszFileName != NULL );

    HRESULT hr = S_OK;

	SAFE_DELETE(m_pEffectHead);

	SAFE_DELETE(pAnimHead);
	SAFE_DELETE(m_pPieceObjTree);
	SAFE_DELETE(m_pPieceObjHead);

	// Note : Texture Eff
	SAFE_DELETE( m_pTextureEff );
	m_pTextureEff = new DxTextureEffMan;

	m_pDxFrame = NULL;
	m_pDxAlphaMapFrameHead = NULL;
	m_pCollDetectFrame = NULL;

	//	Note : 로딩 계체.
	//
    LPD3DXFILE pxofapi = NULL;
    LPD3DXFILEENUMOBJECT pxofenum = NULL;
    LPD3DXFILEDATA pxofobjCur = NULL;
	SIZE_T cChildren = (0);

	TSTRING strFullName;
    CString strEncryptFile;
    CPrivateResource cResource( CPrivateResource::ETypeSkin );

	//	Note : 데이터 초기화.
	//
	vTreeMax = D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	vTreeMin = D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX);

	m_dwVertices = 0;
	m_dwTriangles = 0;

	//	Note : 루트 만듬.
	//
	m_pDxFrame = new DxFrame();
    
    if ( m_pDxFrame == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

	int nStrLen = static_cast<int>( strlen(__DXFRAMEROOT)+1 );
	m_pDxFrame->szName = new char[nStrLen];
	StringCchCopy( m_pDxFrame->szName, nStrLen, __DXFRAMEROOT );
	
	//	Note : xFile 열기.
	//

	//	xFile 개체를 만듬.
    hr = D3DXFileCreate ( &pxofapi );
    if (FAILED(hr))
        goto e_Exit;
    
    //	xFile 템플리트 등록.
    hr = pxofapi->RegisterTemplates ( (LPVOID)D3DRM_XTEMPLATES,
        D3DRM_XTEMPLATE_BYTES );
    if ( FAILED(hr) )
        goto e_Exit;

	STRUTIL::ChangeExt( m_pszFileName, strEncryptFile, ".mxf" );

	strFullName = m_strPath;
	strFullName += strEncryptFile;

	if ( cResource.LoadFileDec( strFullName.c_str() ) )
	{
		D3DXF_FILELOADMEMORY sData;
		sData.lpMemory = (LPCVOID)cResource.GetData();
		sData.dSize = cResource.GetSize();

		hr = pxofapi->CreateEnumObject((LPVOID)&sData,
			D3DXF_FILELOAD_FROMMEMORY,
			&pxofenum);

		if (FAILED(hr))
			goto e_Exit;
	}
	else
	{
		strFullName = m_strPath;
		strFullName += m_pszFileName;

		//	실제 xFile을 열음.
		hr = pxofapi->CreateEnumObject ( (LPVOID)strFullName.c_str(), D3DXF_FILELOAD_FROMFILE, &pxofenum );
		if ( FAILED(hr) )
			goto e_Exit;
	}
    
	//	Note : 최상위 오브젝트를 읽어옴.
	//
	pxofenum->GetChildren ( &cChildren );
    for ( UINT iChild=0; iChild<cChildren; iChild++ )
    {
		hr = pxofenum->GetChild ( iChild, &pxofobjCur );
		if ( FAILED(hr) )	goto e_Exit;

        hr = LoadFrames ( pxofobjCur, m_dwFVF, pd3dDevice, m_pDxFrame );
        GXRELEASE(pxofobjCur);
        if (FAILED(hr))		goto e_Exit;
    }

	//while (SUCCEEDED(pxofenum->GetNextDataObject(&pxofobjCur)))
	//{
	//	hr = LoadFrames ( pxofobjCur, m_dwFVF, pd3dDevice, m_pDxFrame );
	//	GXRELEASE(pxofobjCur);
	//	if (FAILED(hr))		goto e_Exit;




	//	const GUID *type;

	//	hr = pxofobjCur->GetType(&type);
	//	if (FAILED(hr))	{
	//		goto e_Exit;
	//	}

	//	//	Note : 본만 가져온다.
	//	//
	//	if ( *type == TID_D3DRMFrame )
	//	{
	//		// 여기 부분은 더 이상 분할할 부분이 없다.
	//		//---------------------------------------------------//
	//		NSLoadingDirect::FrameMoveRender( FALSE, FALSE );
	//		//---------------------------------------------------//

	//		hr = LoadBoneTrans( m_skeletonPart.pBoneRoot, &m_skeletonPart.BoneMap, pxofobjCur, EMBODY_ROOT );
	//		if (FAILED(hr))	{
	//			goto e_Exit;
	//		}
	//	}

	//	GXRELEASE(pxofobjCur);
	//}

	////	Note : 순차적으로 탐색하면서 에니메이션을 읽어온다.
	////		QueryInterface()를 사용하여 오브젝트의 타입을 확인함.
	////
	//while ( SUCCEEDED(pxofobjCur->GetNextObject(&pxofChild)) )
	//{
	//	hr = pxofChild->QueryInterface(IID_IDirectXFileData,
	//		(LPVOID *)&pxofobjChild);

	//	if (SUCCEEDED(hr))
	//	{
	//		hr = LoadBoneTrans ( pBoneTransCur, pBoneMap, pxofobjChild, pBoneTransCur->m_emBody );
	//		if (FAILED(hr))		goto e_Exit;

	//		GXRELEASE(pxofobjChild);
	//	}

	//	GXRELEASE(pxofChild);
	//}


	//	Note : Frame 각각의 바운딩 볼륨 계산.
	//
	{
		D3DXMATRIX mCur;
		D3DXMatrixIdentity ( &mCur );
		m_pDxFrame->CalculateBoundingSphere( &mCur, 0.033f * UNITANIKEY_PERSEC );//m_UNITTIME );

		vTreeMax = m_pDxFrame->vTreeMax;
		vTreeMin = m_pDxFrame->vTreeMin;
	}

	// Animation 을 굳이 안해도 되는 녀석들은 지움.
	OneKeyAnimationLinkOff();

	//	Note : 에니메이션 설정을 위해 - 분류별로 목록화.
	//
	MakeAnimationList ();

e_Exit:
    GXRELEASE(pxofobjCur);
    GXRELEASE(pxofenum);
    GXRELEASE(pxofapi);
    
    if (FAILED(hr))
    {
        delete m_pDxFrame;
		m_pDxFrame = NULL;
    }
    
    return hr;
}

HRESULT DxFrameMesh::LoadFrames ( LPD3DXFILEDATA pxofobjCur, DWORD fvf,
							   LPDIRECT3DDEVICEQ pD3DDevice, DxFrame *pframeParent )
{
    HRESULT hr = S_OK;

    LPD3DXFILEDATA pxofobjChild = NULL;

    GUID type;
    DWORD cbSize;
    D3DXMATRIX *pmatNew;
    DxFrame *pframeCur;
    DWORD cchName;
    
    //	Note : 오브젝트의 타입.
	//
    hr = pxofobjCur->GetType ( &type );
    if (FAILED(hr))
        goto e_Exit;
    
    //	Note : 메쉬.
	//
    if ( type == TID_D3DRMMesh )
    {
		if ( pframeParent->szName == NULL || !strlen(pframeParent->szName) )
		{
			DxFrame* pframe = FindParentFrame ( pframeParent );

			if ( pframe && (pframe->szName!=NULL) )
			{
				SAFE_DELETE_ARRAY(pframeParent->szName);

				char szMesh[] = "[Mesh]";
				int nStrLen = static_cast<int>( strlen(pframe->szName)+strlen(szMesh)+1 );
				pframeParent->szName = new char[nStrLen];
				memset( pframeParent->szName, 0, sizeof(char)*nStrLen );

				StringCchCopy( pframeParent->szName, nStrLen, pframe->szName );
				StringCchCat( pframeParent->szName, nStrLen, szMesh );
			}
		}

        hr = LoadMesh ( pxofobjCur, fvf, pD3DDevice, pframeParent );
        if (FAILED(hr))
		{
			std::string strMsg = std::string(pframeParent->szName) + " -- 메시를 읽지 못하였습니다.";
			MessageBox ( NULL, strMsg.c_str(), "ERROR", MB_OK );
            goto e_Exit;
		}
    }
    //	Note : 트렌스폼.
	//
    else if ( type == TID_D3DRMFrameTransformMatrix )
    {
		hr = pxofobjCur->Lock( &cbSize, (LPCVOID*)&pmatNew );
        if( FAILED(hr) )
		{
            CDebugSet::ToLogFile(std::string("DxFrameMesh::LoadFrames() -- Lock() -- Failed"));
			goto e_Exit;
		}

		// update the parents matrix with the new one
		pframeParent->matRot = *pmatNew;
		pframeParent->matRotOrig = *pmatNew;

		if( pframeParent->matRot._11 == 1.f && pframeParent->matRot._12 == 0.f && pframeParent->matRot._13 == 0.f && pframeParent->matRot._14 == 0.f && 
			pframeParent->matRot._21 == 0.f && pframeParent->matRot._22 == 1.f && pframeParent->matRot._23 == 0.f && pframeParent->matRot._24 == 0.f && 
			pframeParent->matRot._31 == 0.f && pframeParent->matRot._32 == 0.f && pframeParent->matRot._33 == 1.f && pframeParent->matRot._34 == 0.f && 
			pframeParent->matRot._41 == 0.f && pframeParent->matRot._42 == 0.f && pframeParent->matRot._43 == 0.f && pframeParent->matRot._44 == 1.f )
		{
			pframeParent->bParentMatrixUSE = TRUE;
		}
		else
		{
			pframeParent->bParentMatrixUSE = FALSE;
		}
		pxofobjCur->Unlock();
			
    }
	//	Note : 에니메이션셋.
	//
    else if ( type == TID_D3DRMAnimationSet )
    {
        LoadAnimationSet ( pxofobjCur, fvf, pD3DDevice );
    }
	//	Note : 에니메이션.
	//
    else if ( type == TID_D3DRMAnimation )
    {
        LoadAnimation ( pxofobjCur, fvf, pD3DDevice );
    }
	//	Note : 하위 프레임.
	//
    else if ( type == TID_D3DRMFrame )
    {
		char *szframeName = NULL;

		//	Note : 프레임의 이름 가져옴.
		//
        hr = pxofobjCur->GetName ( NULL, &cchName );
        if (FAILED(hr))
            goto e_Exit;
        
        if ( cchName > 0 )
        {
            szframeName = new char[cchName];
            if ( szframeName == NULL )
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }
            
            hr = pxofobjCur->GetName ( szframeName, &cchName );
            if (FAILED(hr))
                goto e_Exit;
        }

		//if ( pframeParent->szName && !strcmp(pframeParent->szName,__DXFRAMEROOT) )
		//{
		//	pframeCur = pframeParent;
		//	
		//	SAFE_DELETE_ARRAY(pframeCur->szName);
		//	pframeCur->szName = szframeName;
		//}
		//else
		{
   			//	Note : 새 프레임 만듬.
			//
			pframeCur = new DxFrame();
			if ( pframeCur == NULL )
			{
				hr = E_OUTOFMEMORY;
				goto e_Exit;
			}

			pframeCur->szName = szframeName;

	        pframeParent->AddFrame ( pframeCur );
		}
 
		//	Note : 순차적으로 탐색하면서 에니메이션을 읽어온다.
		//		QueryInterface()를 사용하여 오브젝트의 타입을 확인함.
		//
		SIZE_T cChildren(0);
		pxofobjCur->GetChildren ( &cChildren );
		for ( UINT iChild=0; iChild<cChildren; iChild++ )
		{
			hr = pxofobjCur->GetChild ( iChild, &pxofobjChild );
			if ( FAILED(hr) )	goto e_Exit;

            hr = LoadFrames ( pxofobjChild, fvf, pD3DDevice, pframeCur );
            if (FAILED(hr))		goto e_Exit;
                
            GXRELEASE(pxofobjChild);
        }
        
    }
    
e_Exit:
    GXRELEASE(pxofobjChild);
    return hr;
}

HRESULT DxFrameMesh::LoadMesh ( LPD3DXFILEDATA pxofobjCur, DWORD fvf,
							LPDIRECT3DDEVICEQ pD3DDevice, DxFrame *pframeParent )
{
	HRESULT hr = S_OK;

	DxMeshes *pmcMesh = NULL;
	LPD3DXBUFFER pbufMaterials = NULL;
	LPD3DXBUFFER pbufEffectInstances = NULL;
	LPD3DXBUFFER pbufAdjacency = NULL;
    
	DWORD cchName;
	UINT iMaterial;
    
	//	Note : 메쉬 콘데이너 생성.
	//	
	pmcMesh = new DxMeshes();
	if ( pmcMesh == NULL )
	{
		hr = E_OUTOFMEMORY;
		goto e_Exit;
	}
    
	//	Note : 이름 가져옴.
	//
	hr = pxofobjCur->GetName ( NULL, &cchName );
	if (FAILED(hr))
		goto e_Exit;
    
	if ( cchName > 0 )
	{
		pmcMesh->m_szName = new char[cchName];
		if ( pmcMesh->m_szName == NULL )
		{
			hr = E_OUTOFMEMORY;
			goto e_Exit;
		}

		hr = pxofobjCur->GetName ( pmcMesh->m_szName, &cchName );
		if (FAILED(hr))
			goto e_Exit;
	}

	// Mesh 정보
	{
		SAFE_DELETE_ARRAY( pmcMesh->m_szName );
		size_t nSize;
		StringCchLength( pframeParent->szName, MAX_PATH, &nSize );
		nSize += 1;
		pmcMesh->m_szName = new char[nSize];
		StringCchCopy( pmcMesh->m_szName, nSize, pframeParent->szName );
	}

	//	Note : DXFILEDATA(pxofobjCur) 오브젝트에서 메쉬를 읽어옴.
	//
	if ( FAILED( hr = D3DXLoadMeshFromXof ( pxofobjCur, D3DXMESH_SYSTEMMEM, pD3DDevice,
									&pbufAdjacency, &pbufMaterials, &pbufEffectInstances,
									&pmcMesh->m_cMaterials, &pmcMesh->m_pSysMemMesh ) ) )
	{
		goto e_Exit;
	}

	// FVF 
	//pmcMesh->MakeLocalMeshs( pD3DDevice, fvf );

	//	Note : 메쉬 최적화.
	//
	if ( m_bMeshOptimize )
	{		
		LPD3DXMESH pOptimizedMesh;
		if ( FAILED( hr = pmcMesh->m_pSysMemMesh->Optimize(
			D3DXMESHOPT_COMPACT|D3DXMESHOPT_ATTRSORT,
			(DWORD*)pbufAdjacency->GetBufferPointer(), NULL, NULL, NULL, &pOptimizedMesh ) ) )
		{
			SAFE_RELEASE( pbufAdjacency );
			SAFE_RELEASE( pbufMaterials );
			SAFE_RELEASE( pbufEffectInstances );

			goto e_Exit;
		}
		SAFE_RELEASE(pmcMesh->m_pSysMemMesh);
		pmcMesh->m_pSysMemMesh = pOptimizedMesh;
	}

	m_dwVertices += pmcMesh->m_pSysMemMesh->GetNumVertices();
	m_dwTriangles += pmcMesh->m_pSysMemMesh->GetNumFaces();

	// Note : [shhan][2013.02.07] FVF 값이 없는 경우가 있어서 강제 셋팅을 해주자.
	{
		LPD3DXMESH pOptimizedMesh;
		pmcMesh->m_pSysMemMesh->CloneMeshFVF( pmcMesh->m_pSysMemMesh->GetOptions(), VERTEX_WORLD::FVF, pD3DDevice, &pOptimizedMesh );

		// Normal 은 무조건 있다는 것으로 가정을 하자.
		// 현재 무조건 pmcMesh->m_pSysMemMesh->GetFVF() & D3DFVF_NORMAL 는 FALSE 가 나와버린다.
		//if ( !(pmcMesh->m_pSysMemMesh->GetFVF() & D3DFVF_NORMAL) )
		//{
		//	// Normal 이 없을 경우만 만들도록 하자.
		//	//D3DXComputeNormals( pOptimizedMesh, NULL );
		//}

		DWORD dwFVF = pmcMesh->m_pSysMemMesh->GetFVF();

		// 새로운 Exporter 에서는 decl 로 보내기 때문에 이 작업이 필요하다.
		BOOL bDecl_Diffuse_ON(FALSE);
		{
			D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
			LPD3DVERTEXELEMENT9 pDeclCur;
			pmcMesh->m_pSysMemMesh->GetDeclaration( pDecl );

			// the vertex shader is expecting to interpret the UBYTE4 as a D3DCOLOR, so update the type 
			//   NOTE: this cannot be done with CloneMesh, that would convert the UBYTE4 data to float and then to D3DCOLOR
			//          this is more of a "cast" operation
			pDeclCur = pDecl;
			while (pDeclCur->Stream != 0xff)
			{
				if ((pDeclCur->Usage == D3DDECLUSAGE_COLOR) && (pDeclCur->UsageIndex == 0))
					bDecl_Diffuse_ON = TRUE;
				pDeclCur++;
			}
		}

		// Diffuse 가 없다면 새로 만든 Mesh의 Diffuse 값을 흰색으로 넣어주도록 한다.
		if ( !(pmcMesh->m_pSysMemMesh->GetFVF() & D3DFVF_DIFFUSE) && !bDecl_Diffuse_ON )
		{
			VERTEX_WORLD* pVertices;
			pOptimizedMesh->LockVertexBuffer ( 0L, (VOID**)&pVertices );
			for ( DWORD i=0; i<pOptimizedMesh->GetNumVertices(); ++i )
			{
				pVertices[i].dwColor = 0xffffffff;	
			}
			pOptimizedMesh->UnlockVertexBuffer();

			pmcMesh->m_bVertexColor = FALSE;
		}
		else
		{
			pmcMesh->m_bVertexColor = TRUE;
		}

		SAFE_RELEASE(pmcMesh->m_pSysMemMesh);
		pmcMesh->m_pSysMemMesh = pOptimizedMesh;
	}

	// Material 별로 Mesh 를 만들어 놓는다.
	pmcMesh->m_spMaterialMeshArray->Insert( pD3DDevice, pmcMesh->m_pSysMemMesh );
	

	//	Note : 메터리얼값이 없다면 기본 메터리얼 지정.
	//
    if ( (pbufMaterials == NULL) || (pmcMesh->m_cMaterials == 0) )
    {
        pmcMesh->m_rgMaterials = new D3DMATERIALQ[1];
		pmcMesh->m_exMaterials = new D3DEXMATERIAL[1];
        pmcMesh->m_pTextureRes = new TextureResource[1];
		pmcMesh->m_pMaskTexRes = new TextureResource[1];
		pmcMesh->m_strTextureFiles = new CString[1];
		pmcMesh->m_pTexEff = new TEXEFF_PROPERTY[1];

        if ( !pmcMesh->m_rgMaterials || !pmcMesh->m_pTextureRes
			|| !pmcMesh->m_strTextureFiles || !pmcMesh->m_exMaterials || !pmcMesh->m_pTexEff )
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }
       
        memset(pmcMesh->m_rgMaterials, 0, sizeof(D3DMATERIALQ));
        pmcMesh->m_rgMaterials[0].Diffuse.r = 0.5f;
        pmcMesh->m_rgMaterials[0].Diffuse.g = 0.5f;
        pmcMesh->m_rgMaterials[0].Diffuse.b = 0.5f;

		pmcMesh->m_strTextureFiles[0] = "";
    }
	//	Note : 메터리얼 값이 있을 경우.
	//
    else
    {
        pmcMesh->m_rgMaterials = new D3DMATERIALQ[pmcMesh->m_cMaterials];
		pmcMesh->m_exMaterials = new D3DEXMATERIAL[pmcMesh->m_cMaterials];
        pmcMesh->m_pTextureRes = new TextureResource[pmcMesh->m_cMaterials];
		pmcMesh->m_pMaskTexRes = new TextureResource[pmcMesh->m_cMaterials];
		pmcMesh->m_strTextureFiles = new CString[pmcMesh->m_cMaterials];
		pmcMesh->m_pTexEff = new TEXEFF_PROPERTY[pmcMesh->m_cMaterials];

        if ( !pmcMesh->m_rgMaterials || !pmcMesh->m_pTextureRes
			|| !pmcMesh->m_strTextureFiles || !pmcMesh->m_exMaterials || !pmcMesh->m_pTexEff )
        {
            hr = E_OUTOFMEMORY;
            goto e_Exit;
        }
        
        LPD3DXMATERIAL pMaterials = (LPD3DXMATERIAL)pbufMaterials->GetBufferPointer();

        for ( iMaterial = 0; iMaterial < pmcMesh->m_cMaterials; iMaterial++ )
        {
            //	Note : 메터리얼 값 지정.
			//
			pmcMesh->m_rgMaterials[iMaterial] = pMaterials[iMaterial].MatD3D;

			// Material 수치 강제 셋팅.
			pmcMesh->m_rgMaterials[iMaterial].Diffuse.r = 1.f;
			pmcMesh->m_rgMaterials[iMaterial].Diffuse.g = 1.f;
			pmcMesh->m_rgMaterials[iMaterial].Diffuse.b = 1.f;
			pmcMesh->m_rgMaterials[iMaterial].Diffuse.a = 1.f;

			//	Note : 읽은 값에서는 Ambient 값은 없으므로 이를 Diffuse 값으로 초기화 해준다.
			//
			pmcMesh->m_rgMaterials[iMaterial].Ambient = pmcMesh->m_rgMaterials[iMaterial].Diffuse;
			pmcMesh->m_rgMaterials[iMaterial].Specular.r = pmcMesh->m_rgMaterials[iMaterial].Specular.g = pmcMesh->m_rgMaterials[iMaterial].Specular.b = 0.0f;


			//	Note : 텍스쳐 이름 설정.
			//
			if ( pMaterials[iMaterial].pTextureFilename )
			{
				TSTRING strName = pMaterials[iMaterial].pTextureFilename;
				std::transform ( strName.begin(), strName.end(), strName.begin(), tolower );
				pmcMesh->m_strTextureFiles[iMaterial] = strName.c_str();
			}

			//	Note : 텍스쳐 로드.
			//
            if ( pMaterials[iMaterial].pTextureFilename != NULL )
            {
                pmcMesh->m_pTextureRes[iMaterial] = TextureManager::GetInstance().LoadTexture(
                    pMaterials[iMaterial].pTextureFilename,
                    false,
                    TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                    0,
                    D3DFMT_UNKNOWN,
                    TEXTURE_RESIZE_1024,
                    TEXTURE_LAND,
					false,
					FALSE );

                if( pmcMesh->m_pTextureRes[iMaterial].GetTexture() )
                {
				    TEXTURE_ALPHA emTexType = pmcMesh->m_pTextureRes[iMaterial].GetAlphaType();
				    if( emTexType==EMTT_NORMAL )			pmcMesh->m_exMaterials[iMaterial].MtrlType = MATERIAL_NORMAL;
				    else if( emTexType==EMTT_ALPHA_HARD )	pmcMesh->m_exMaterials[iMaterial].MtrlType = MATERIAL_ALPHA_HARD;
				    else if( emTexType==EMTT_ALPHA_SOFT )	pmcMesh->m_exMaterials[iMaterial].MtrlType = MATERIAL_ALPHA_SOFT;
				    else if( emTexType==EMTT_ALPHA_SOFT01 )	pmcMesh->m_exMaterials[iMaterial].MtrlType = MATERIAL_ALPHA_SOFT;
				    else if( emTexType==EMTT_ALPHA_SOFT02 )	pmcMesh->m_exMaterials[iMaterial].MtrlType = MATERIAL_ALPHA_SOFT;
                }

				// Note : TextureEff 사용을 위해서 파일이름을 얻어온다.
				//m_pTextureEff->InsertTexList( pmcMesh->m_strTextureFiles[iMaterial] );
            }
        }
    }

	//////////////////////////////////////////////////////////////////////////
	// Effect Instances
	// 현재 max Exporter 에서 다중 Material 일 경우는 무조건 m_strTextureFiles 이름이 들어온다. (0)
	// 단일 Material 이면서, 기본 Shading을 썼을 경우 m_strTextureFiles 이름이 들어온다. (0)
	// 단일 Material 이면서, ShaderFX 를 썼을 경우 m_strTextureFiles 에 이름이 안 들어온다. (X)
	//	LPD3DXEFFECTINSTANCE 쪽에 들어간다.
	// 그래서 그부분을 보완하기 위한 작업이다.
	// 
	LPD3DXEFFECTINSTANCE pEffects = (LPD3DXEFFECTINSTANCE)pbufEffectInstances->GetBufferPointer();
	if ( pEffects )
	{
		if ( pmcMesh->m_strTextureFiles[0].IsEmpty() && pEffects->pEffectFilename )	// pEffectFilename 에는 값이 있어야지..
		{
			for ( DWORD i=0; i<pEffects->NumDefaults; ++i )
			{
				if ( pEffects->pDefaults[i].Type == D3DXEDT_STRING )
				{
					TCHAR* pName = new TCHAR[pEffects->pDefaults[i].NumBytes];
					StringCchCopy ( pName, pEffects->pDefaults[i].NumBytes, reinterpret_cast<TCHAR*>(pEffects->pDefaults[i].pValue) );

					TSTRING strName = pName;
					std::transform ( strName.begin(), strName.end(), strName.begin(), tolower );
					pmcMesh->m_strTextureFiles[0] = strName.c_str();

					SAFE_DELETE_ARRAY ( pName );

					//	Note : 텍스쳐 로드.
					if ( !strName.empty() )
					{
						pmcMesh->m_pTextureRes[0] = TextureManager::GetInstance().LoadTexture(
							strName.c_str(),
							false,
							TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
							0,
							D3DFMT_UNKNOWN,
							TEXTURE_RESIZE_1024,
							TEXTURE_LAND,
							false,
							FALSE );

						if( pmcMesh->m_pTextureRes[0].GetTexture() )
						{
							TEXTURE_ALPHA emTexType = pmcMesh->m_pTextureRes[0].GetAlphaType();
							if( emTexType==EMTT_NORMAL )			pmcMesh->m_exMaterials[0].MtrlType = MATERIAL_NORMAL;
							else if( emTexType==EMTT_ALPHA_HARD )	pmcMesh->m_exMaterials[0].MtrlType = MATERIAL_ALPHA_HARD;
							else if( emTexType==EMTT_ALPHA_SOFT )	pmcMesh->m_exMaterials[0].MtrlType = MATERIAL_ALPHA_SOFT;
							else if( emTexType==EMTT_ALPHA_SOFT01 )	pmcMesh->m_exMaterials[0].MtrlType = MATERIAL_ALPHA_SOFT;
							else if( emTexType==EMTT_ALPHA_SOFT02 )	pmcMesh->m_exMaterials[0].MtrlType = MATERIAL_ALPHA_SOFT;
						}

						// Note : TextureEff 사용을 위해서 파일이름을 얻어온다.
						//m_pTextureEff->InsertTexList( pmcMesh->m_strTextureFiles[iMaterial] );
					}

					// 한번만 하고 끝내자.
					break;
				}
			}
		}
	}

	// Material Name을 넣는다.
	NSXFormatLoader::ParseChildObjects_MaterialName( pxofobjCur, pmcMesh->m_vecMaterialName );

	// Default Material Setting 을 위해 Material 이름들을 입력해놓는다.
	m_scpMaterialSetting->Insert( pmcMesh->m_vecMaterialName );

    //	Note : 새로운 메쉬 콘테이너를 삽입.
    pframeParent->AddMesh ( pmcMesh );
    pmcMesh = NULL;
    
	//	Note : 알파멥 유무에 따라 분류.
	//
	ModiflyAlphaMapFrame ( pframeParent );

e_Exit:
    delete pmcMesh;
    
    GXRELEASE(pbufAdjacency);
    GXRELEASE(pbufMaterials);
	GXRELEASE(pbufEffectInstances);

    return hr;
}

HRESULT DxFrameMesh::LoadAnimationSet ( LPD3DXFILEDATA pxofobjCur, DWORD fvf,
							   LPDIRECT3DDEVICEQ pD3DDevice )
{
    GUID type;
    HRESULT hr = S_OK;
    
	SIZE_T cChildren = 0;
	LPD3DXFILEDATA pxofobjChild = NULL;
    
	//	Note : 순차적으로 탐색하면서 에니메이션을 읽어온다.
	//
	pxofobjCur->GetChildren ( &cChildren );
	for ( UINT iChild=0; iChild<cChildren; iChild++ )
	{
		hr = pxofobjCur->GetChild ( iChild, &pxofobjChild );
		if ( FAILED(hr) )	goto e_Exit;

        hr = pxofobjChild->GetType ( &type );
        if (FAILED(hr))
            goto e_Exit;
        
        if ( TID_D3DRMAnimation == type )
        {
            hr = LoadAnimation ( pxofobjChild, fvf, pD3DDevice );
            if (FAILED(hr))
                goto e_Exit;
        }
        
        GXRELEASE(pxofobjChild);
    }
    
e_Exit:
    GXRELEASE(pxofobjChild);

    return hr;
}

HRESULT DxFrameMesh::LoadAnimation ( LPD3DXFILEDATA pxofobjCur, DWORD fvf,
							   LPDIRECT3DDEVICEQ pD3DDevice )
{
    HRESULT hr = S_OK;

    SRotateKeyXFile *pFileRotateKey;
    SScaleKeyXFile *pFileScaleKey;
    SPositionKeyXFile *pFilePosKey;
    SMatrixKeyXFile *pFileMatrixKey;
    
	DxAnimation *pAnimCur;
    
	SIZE_T cChildren = 0;
	LPD3DXFILEDATA pxofobjChild = NULL;
    GUID type;
    
	DWORD dwSize;
    PBYTE pData;
    DWORD dwKeyType;
    DWORD cKeys;
    DWORD iKey;
    DWORD cchName;
        
	//	Note : 새로운 프레임 만듬. - Animation.
	//
    pAnimCur = new DxAnimation();
    if (pAnimCur == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto e_Exit;
    }

	//	Note : 에니메이션 프레임 리스트에 등록.
	//
    AddAnimationFrame(pAnimCur);
    
	//	Note : 순차적으로 탐색하면서 에니메이션을 읽어온다.
	//
	pxofobjCur->GetChildren ( &cChildren );
	for ( UINT iChild=0; iChild<cChildren; iChild++ )
	{
		hr = pxofobjCur->GetChild ( iChild, &pxofobjChild );
		if ( FAILED(hr) )	goto e_Exit;

        hr = pxofobjChild->GetType(&type);
        if (FAILED(hr))		goto e_Exit;
                
		//	Note : 에니에이션에 연결된 프레임을 지정.
		//
        if( TID_D3DRMFrame == type )
        {
            if (pAnimCur->pframeToAnimate != NULL)
            {
                hr = E_INVALIDARG;
                goto e_Exit;
            }
            
			//	Note : 연결된 프레임의 이름을 가져옴.
			//
            hr = pxofobjChild->GetName(NULL, &cchName);
            if (FAILED(hr))
                goto e_Exit;
            
            if (cchName == 0)
            {
                hr = E_INVALIDARG;
                goto e_Exit;
                
            }
            
            // stack overflow 위험 있음
            char* szFrameName = (char*) _malloca(cchName);
            if (szFrameName == NULL)
            {
                hr = E_OUTOFMEMORY;
                goto e_Exit;
            }
            
            hr = pxofobjChild->GetName(szFrameName, &cchName);
            if (FAILED(hr))
                goto e_Exit;
            
			//	Note : 연결된 프레임을 지정.
			//
            pAnimCur->pframeToAnimate = FindFrame(szFrameName);
            if (pAnimCur->pframeToAnimate == NULL)
            {
                hr = E_INVALIDARG;
                goto e_Exit;
            }

			if( szFrameName )
			{
				int nStrLen = static_cast<int>( strlen(szFrameName)+1 );
				pAnimCur->szName = new char[nStrLen];
				StringCchCopy( pAnimCur->szName, nStrLen, szFrameName );
				pAnimCur->pframeToAnimate->pframeFromAnimate = pAnimCur;
				pAnimCur->pframeToAnimate->bParentMatrixUSE = FALSE;		// 계산에 들어가야 함.
			}
             _freea(szFrameName);
        }
		//	Note : 에니메이션키일 경우.
		//
		else if ( TID_D3DRMAnimationKey == type )
        {
			hr = pxofobjChild->Lock ( &dwSize, (LPCVOID*)&pData );
            if( FAILED(hr) )
			{
                CDebugSet::ToLogFile(std::string("DxFrameMesh::LoadAnimation() -- Lock -- Failed"));
                goto e_Exit;
			}
            
            dwKeyType = ((DWORD*)pData)[0];
            cKeys = ((DWORD*)pData)[1];
            
			DWORD dwUnitTime = UINT_MAX;
			
			//	Note : 쿼터니언 회전 키.
			//
            if ( dwKeyType == 0 )
            {
                if (pAnimCur->m_pRotateKeys != NULL)
                {
					pxofobjChild->Unlock();
                    hr = E_INVALIDARG;
                    goto e_Exit;
                }
                
				//	Note : 회전키 생성.
				//
                pAnimCur->m_pRotateKeys = new SRotateKey[cKeys];
                if (pAnimCur->m_pRotateKeys == NULL)
                {
					pxofobjChild->Unlock();
                    hr = E_OUTOFMEMORY;
                    goto e_Exit;
                }
                
				//	회전키의 갯수.
                pAnimCur->m_cRotateKeys = cKeys;
                
				//	NOTE : 쿼터니언 값.
				//
                pFileRotateKey =  (SRotateKeyXFile*)(pData + (sizeof(DWORD) * 2));
                for (iKey = 0;iKey < cKeys; iKey++)
                {
					//	현제 키의 시간.
                    pAnimCur->m_pRotateKeys[iKey].dwTime = pFileRotateKey->dwTime;

                    pAnimCur->m_pRotateKeys[iKey].quatRotate.x = pFileRotateKey->x;
                    pAnimCur->m_pRotateKeys[iKey].quatRotate.y = pFileRotateKey->y;
                    pAnimCur->m_pRotateKeys[iKey].quatRotate.z = pFileRotateKey->z;
                    pAnimCur->m_pRotateKeys[iKey].quatRotate.w = pFileRotateKey->w;
            
					if ( iKey!=0 )
					{
						DWORD dxTime = pAnimCur->m_pRotateKeys[iKey].dwTime - pAnimCur->m_pRotateKeys[iKey-1].dwTime;
						if (  dxTime < dwUnitTime )
							dwUnitTime = dxTime;
					}

                    pFileRotateKey += 1;
                }

				pxofobjChild->Unlock();
			}
			//	Note : 스케일 키.
			//
            else if (dwKeyType == 1)
            {
                if (pAnimCur->m_pScaleKeys != NULL)
                {
                    hr = E_INVALIDARG;
                    goto e_Exit;
                }
        
				//	Note : 스케일키 생성.
				//
                pAnimCur->m_pScaleKeys = new SScaleKey[cKeys];
                if (pAnimCur->m_pScaleKeys == NULL)
                {
                    hr = E_OUTOFMEMORY;
                    goto e_Exit;
                }
                
				//	스케일키 갯수.
                pAnimCur->m_cScaleKeys = cKeys;
                
				//	Note : 스케일키 값.
				//
                pFileScaleKey =  (SScaleKeyXFile*)(pData + (sizeof(DWORD) * 2));
                for (iKey = 0;iKey < cKeys; iKey++)
                {
					//	현제 키의 시간.
                    pAnimCur->m_pScaleKeys[iKey].dwTime = pFileScaleKey->dwTime;
                    
					pAnimCur->m_pScaleKeys[iKey].vScale = pFileScaleKey->vScale;

					if ( iKey!=0 )
					{
						DWORD dxTime = pAnimCur->m_pScaleKeys[iKey].dwTime - pAnimCur->m_pScaleKeys[iKey-1].dwTime;
						if (  dxTime < dwUnitTime )
							dwUnitTime = dxTime;
					}
                    
                    pFileScaleKey += 1;
                }
            }
			//	Note : 이동 키.
			//
            else if (dwKeyType == 2)
            {
                if (pAnimCur->m_pPositionKeys != NULL)
                {
                    hr = E_INVALIDARG;
                    goto e_Exit;
                }
                
				//	Note : 이동 키 생성.
				//
                pAnimCur->m_pPositionKeys = new SPositionKey[cKeys];
                if (pAnimCur->m_pPositionKeys == NULL)
                {
                    hr = E_OUTOFMEMORY;
                    goto e_Exit;
                }
                
				//	이동키 갯수.
                pAnimCur->m_cPositionKeys = cKeys;
                
				//	Note : 이동키 값.
				//
                pFilePosKey =  (SPositionKeyXFile*)(pData + (sizeof(DWORD) * 2));
                for (iKey = 0;iKey < cKeys; iKey++)
                {
					//	현제 키의 시간.
                    pAnimCur->m_pPositionKeys[iKey].dwTime = pFilePosKey->dwTime;
                    
					pAnimCur->m_pPositionKeys[iKey].vPos = pFilePosKey->vPos;
                    
					if ( iKey!=0 )
					{
						DWORD dxTime = pAnimCur->m_pPositionKeys[iKey].dwTime - pAnimCur->m_pPositionKeys[iKey-1].dwTime;
						if (  dxTime < dwUnitTime )
							dwUnitTime = dxTime;
					}

                    pFilePosKey += 1;
                }
            }
			//	Note : 트렌스폼 키.
			//
            else if (dwKeyType == 4)
            {
                if (pAnimCur->m_pMatrixKeys != NULL)
                {
                    hr = E_INVALIDARG;
                    goto e_Exit;
                }
                
				//	Note : 트렌스폼 키 생성.
				//
                pAnimCur->m_pMatrixKeys = new SMatrixKey[cKeys];
                if (pAnimCur->m_pMatrixKeys == NULL)
                {
                    hr = E_OUTOFMEMORY;
                    goto e_Exit;
                }
                
				//	트렌스폼 키 갯수.
                pAnimCur->m_cMatrixKeys = cKeys;
                
				//	Note : 트렌스폼 키 값.
				//
                pFileMatrixKey =  (SMatrixKeyXFile*)(pData + (sizeof(DWORD) * 2));
                for (iKey = 0;iKey < cKeys; iKey++)
                {
					//	현제 프레임의 시간.
					pAnimCur->m_pMatrixKeys[iKey].dwTime = pFileMatrixKey->dwTime;
                    
					pAnimCur->m_pMatrixKeys[iKey].mat = pFileMatrixKey->mat;

					if ( iKey!=0 )
					{
						DWORD dwTime = pAnimCur->m_pMatrixKeys[iKey].dwTime - pAnimCur->m_pMatrixKeys[iKey-1].dwTime;
						if (  dwTime < dwUnitTime )	dwUnitTime = dwTime;
					}

                    pFileMatrixKey += 1;
                }
            }
            else
            {
                hr = E_INVALIDARG;
                goto e_Exit;
            }
			
			if ( (m_UNITTIME>(float)dwUnitTime) )	m_UNITTIME = (float)dwUnitTime;
        }
		GXRELEASE(pxofobjChild);
    }

	// EndTime을 구한다.
	pAnimCur->CalculateEndTime();
    
e_Exit:
    GXRELEASE(pxofobjChild);
    return hr;
}

HRESULT DxFrameMesh::CreateTangentMesh( LPDIRECT3DDEVICEQ pd3dDevice )
{
    return CreateTangentMesh( pd3dDevice, GetFrameRoot() );
}

HRESULT DxFrameMesh::CompressTangentMesh( LPDIRECT3DDEVICEQ pd3dDevice )
{
    return CompressTangentMesh( pd3dDevice, GetFrameRoot() );
}

HRESULT DxFrameMesh::RenderVisualMaterial( LPDIRECT3DDEVICEQ pd3dDevice, 
                                        ID3DXEffect* pFX,
                                        LPDIRECT3DVERTEXDECLARATION9 pDCRT,
                                        const D3DXMATRIX& matView, 
                                        const D3DXMATRIX& matProj )
{
    return RenderVisualMaterial( pd3dDevice, GetFrameRoot(), pFX, pDCRT, matView, matProj );
}

HRESULT DxFrameMesh::CreateTangentMesh( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur )
{
    if ( !pframeCur )
        return S_OK;

    HRESULT hr(S_OK);
    DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : 메쉬 FVF 설정.
	//
	pmsMeshs = pframeCur->pmsMeshs;
	if( pmsMeshs != NULL )
	{
		hr = pmsMeshs->CreateTangentMesh( pd3dDevice );
		if (FAILED(hr))
			return hr;
	}

	//	Note : 자식 프레임 순환.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = CreateTangentMesh ( pd3dDevice, pframeChild );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

    return S_OK;
}

HRESULT DxFrameMesh::CompressTangentMesh( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur )
{
    if ( !pframeCur )
        return S_OK;

    HRESULT hr(S_OK);
    DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : 메쉬 FVF 설정.
	//
	pmsMeshs = pframeCur->pmsMeshs;
	if( pmsMeshs != NULL )
	{
		hr = pmsMeshs->CompressTangentMesh( pd3dDevice );
		if (FAILED(hr))
			return hr;
	}

	//	Note : 자식 프레임 순환.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = CompressTangentMesh ( pd3dDevice, pframeChild );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

    return S_OK;
}

HRESULT DxFrameMesh::RenderVisualMaterial( LPDIRECT3DDEVICEQ pd3dDevice, 
                                       DxFrame *pframeCur, 
                                       ID3DXEffect* pFX, 
                                       LPDIRECT3DVERTEXDECLARATION9 pDCRT,
                                       const D3DXMATRIX& matView, 
                                       const D3DXMATRIX& matProj )
{
    HRESULT hr(S_OK);
    DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

    //	Note : 원본 Render
	pmsMeshs = pframeCur->pmsMeshs;
	if( pmsMeshs )
	{
		if( pmsMeshs->m_pLocalMesh )
		{
            D3DXMATRIX matWV, matWVP;
		    D3DXMatrixMultiply( &matWV, &pframeCur->matCombined, &matView );
		    D3DXMatrixMultiply( &matWVP, &matWV, &matProj );

		    pFX->SetMatrixTranspose( "g_matWorldView",	&matWV );
		    pFX->SetMatrixTranspose( "g_matWVP",		&matWVP );

			pmsMeshs->RenderVisualMaterial( pd3dDevice, pDCRT );
		}
	}

	//	Note : 자식 프레임 그리기.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = RenderVisualMaterial ( pd3dDevice, pframeChild, pFX, pDCRT, matView, matProj );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

    return hr;
}

BOOL			DxMeshes::g_bAlphaUP(FALSE);
float			DxMeshes::g_fAlpha(1.f);
MESH_METERIAL	DxMeshes::g_emMeshMeterial(EM_MESH_MATERIAL);

DxMeshes::DxMeshes()
    : m_szName(NULL)
	, m_pSysMemMesh(NULL)
	, m_pLocalMesh(NULL)
	, m_pDxOctreeMesh(NULL)
	, m_dwVert(0L)
	, m_dwFace(0L)
	, m_dwAttrib(0L)
	, m_vMax(0.f,0.f,0.f)
	, m_vMin(0.f,0.f,0.f)
	, m_pVertSrc(NULL)
	, m_pEffVB(NULL)
	, m_pEffIB(NULL)
	, m_pAttrib(NULL)
	, m_cMaterials(0)
	, m_rgMaterials(NULL)
	, m_exMaterials(NULL)
	, m_pTexEff(NULL)
	, m_pTextureRes(NULL)
	, m_pMaskTexRes(NULL)
	, m_strTextureFiles(NULL)
	, m_bEnableLightMap(FALSE)	// 기본적으로 사용하지 않는다고 셋팅하자.
	, m_dwLightMapIndex(1)
    , m_fLightMapDetail(1.f)
	, m_wwLightMapUserUV_TextureSize(64,64)
	, m_bVertexColor(FALSE)
	, m_pMeshNext(NULL)
{
	m_spMaterialMeshArray = std::tr1::shared_ptr<MaterialMeshArray>(new MaterialMeshArray);
}

DxMeshes::~DxMeshes()
{
	if(m_pSysMemMesh)	GXRELEASE(m_pSysMemMesh);
	if(m_pLocalMesh)	GXRELEASE(m_pLocalMesh);

	m_dwVert = 0L;
	m_dwFace = 0L;
	m_dwAttrib = 0L;

	SAFE_DELETE_ARRAY( m_pVertSrc );
	SAFE_RELEASE( m_pEffVB );
	SAFE_RELEASE( m_pEffIB );
	SAFE_DELETE_ARRAY( m_pAttrib );

	SAFE_DELETE_ARRAY(m_rgMaterials);
	SAFE_DELETE_ARRAY(m_exMaterials);

	SAFE_DELETE_ARRAY(m_szName);
	SAFE_DELETE(m_pMeshNext);

	SAFE_DELETE(m_pDxOctreeMesh);

	SAFE_DELETE_ARRAY(m_strTextureFiles);
	SAFE_DELETE_ARRAY(m_pTextureRes);
	SAFE_DELETE_ARRAY(m_pMaskTexRes);
	SAFE_DELETE_ARRAY(m_pTexEff);

	m_spMaterialMeshArray.reset();
}

// Note : World Edit 할 때만 쓰이는 코드.
void DxMeshes::FrameMove( float fElapsedTime )
{
	if( g_bAlphaUP )
	{
		g_fAlpha += fElapsedTime;
		if( g_fAlpha > 1.f )
		{
			g_fAlpha = 1.f;
			g_bAlphaUP = FALSE;
		}
	}
	else
	{
		g_fAlpha -= fElapsedTime;
		if( g_fAlpha < 0.4f )
		{
			g_fAlpha = 0.4f;
			g_bAlphaUP = TRUE;
		}
	}
}

BOOL DxMeshes::MakeEffectMeshs( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFVF )
{
	if( !m_pSysMemMesh )	return FALSE;
	if( !m_pLocalMesh )		return FALSE;

	HRESULT hr;
	{
		m_dwVert = m_pLocalMesh->GetNumVertices();

		SAFE_DELETE_ARRAY( m_pVertSrc );
		m_pVertSrc = new VERTEX[ m_dwVert ];

		SAFE_RELEASE( m_pEffVB );
		hr = pd3dDevice->CreateVertexBuffer ( m_dwVert*sizeof(VERTEX), D3DUSAGE_WRITEONLY,
										VERTEX::FVF, D3DPOOL_SYSTEMMEM, &m_pEffVB, NULL );
		if( FAILED(hr) )
		{
            CDebugSet::ToLogFile(std::string("DxMeshes::MakeEffectMeshs() -- CreateVertexBuffer() -- Failed"));
			return FALSE;
		}

		VERTEX *pSrc, *Dest;
		hr = m_pLocalMesh->LockVertexBuffer( D3DLOCK_READONLY, (VOID**)&pSrc );
		if ( FAILED(hr) )
		{
			CDebugSet::ToLogFile(std::string("DxMeshes::MakeEffectMeshs() -- LockVertexBuffer -- Failed"));
			return FALSE;
		}

		hr = m_pEffVB->Lock( 0, 0, (VOID**)&Dest, 0L );
		if ( FAILED(hr) )
		{
			CDebugSet::ToLogFile(std::string("DxMeshes::MakeEffectMeshs() -- Lock -- Failed"));
			m_pLocalMesh->UnlockVertexBuffer();
			return FALSE;
		}

		memcpy( m_pVertSrc, pSrc, m_dwVert*sizeof(VERTEX) );
		memcpy( Dest, pSrc, m_dwVert*sizeof(VERTEX) );
		m_pEffVB->Unlock();
		m_pLocalMesh->UnlockVertexBuffer();
	}

	{
		m_dwFace = m_pLocalMesh->GetNumFaces();

		SAFE_RELEASE( m_pEffIB );
		hr = pd3dDevice->CreateIndexBuffer( m_dwFace*3*sizeof(WORD), 0L,
										D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pEffIB, NULL );
		if ( FAILED(hr) )
		{
			CDebugSet::ToLogFile(std::string("DxMeshes::MakeEffectMeshs() -- CreateIndexBuffer() -- Failed"));
			return FALSE;
		}

		WORD *pSrc, *Dest;
		hr = m_pLocalMesh->LockIndexBuffer( D3DLOCK_READONLY, (VOID**)&pSrc );
		if ( FAILED(hr) )
		{
			CDebugSet::ToLogFile(std::string("DxMeshes::MakeEffectMeshs() -- LockIndexBuffer() -- Failed"));
			return FALSE;
		}

		hr = m_pEffIB->Lock( 0, 0, (VOID**)&Dest, 0L );
		if ( FAILED(hr) )
		{
			CDebugSet::ToLogFile(std::string("DxMeshes::MakeEffectMeshs() -- LockIndexBuffer() -- Failed"));
			m_pLocalMesh->UnlockIndexBuffer();
			return FALSE;
		}

		memcpy( Dest, pSrc, m_dwFace*3*sizeof(WORD) );
		m_pEffIB->Unlock();
		m_pLocalMesh->UnlockIndexBuffer();
	}

	{
		m_pLocalMesh->GetAttributeTable( NULL, &m_dwAttrib );

		SAFE_DELETE_ARRAY( m_pAttrib );
		m_pAttrib = new D3DXATTRIBUTERANGE[ m_dwAttrib ];
		m_pLocalMesh->GetAttributeTable( m_pAttrib, &m_dwAttrib );
	}

	return TRUE;
}

HRESULT DxMeshes::MakeLocalMeshs ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwNewFVF )
{
	if ( NULL == m_pSysMemMesh )	return E_FAIL;

	//	Note : 만약 지정된 FVF가 있다면 이걸 이용해서 변환을 한다.
	//
	DWORD dwSysMeshFVF = m_pSysMemMesh->GetFVF();
	DWORD dwFVF = dwSysMeshFVF;
	if ( dwNewFVF != NULL )
	{
		dwFVF = dwNewFVF;
	}

	DWORD dwOptions = D3DXMESH_MANAGED;
	if ( m_pSysMemMesh->GetOptions () & D3DXMESH_32BIT )
		dwOptions |= D3DXMESH_32BIT;

	SAFE_RELEASE(m_pLocalMesh);
	if ( FAILED( m_pSysMemMesh->CloneMeshFVF( dwOptions, dwFVF,
										pd3dDevice, &m_pLocalMesh ) ) )
		return E_FAIL;


	if( m_pLocalMesh )
	{
		// Normal 값을 갖도록한다.
		if ( dwFVF & D3DFVF_NORMAL )
		{
			// 원본 Normal값이 없다면 생성.
			if ( !( dwSysMeshFVF & D3DFVF_NORMAL ) )
			{
				D3DXComputeNormals( m_pLocalMesh, NULL );
			}

			// Normalize 작업
			{
				DWORD dwVertices = m_pLocalMesh->GetNumVertices();

				DWORD dwFirstFVF = dwFVF & ( D3DFVF_POSITION_MASK );
				DWORD dwFirstFVFSize = D3DXGetFVFVertexSize( dwFirstFVF );
				DWORD dwFullFVFSize = D3DXGetFVFVertexSize( dwFVF );

				D3DXVECTOR3* pNormal;
				BYTE* pVertices;
				m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pVertices );
				for ( DWORD i=0; i<dwVertices; ++i )
				{
					pNormal = (D3DXVECTOR3*)(pVertices + dwFullFVFSize * i + dwFirstFVFSize);
					D3DXVec3Normalize( pNormal, pNormal );
				}
				m_pLocalMesh->UnlockVertexBuffer();
			}
		}

		// 원본에 Color 값이 없다면 Default 수치 0xffffffff를 넣는다.
		if ( !( dwSysMeshFVF & D3DFVF_DIFFUSE ) && ( dwFVF & D3DFVF_DIFFUSE ) )
		{
			DWORD dwVertices = m_pLocalMesh->GetNumVertices();

			DWORD dwFirstFVF = dwFVF & ( D3DFVF_POSITION_MASK|D3DFVF_NORMAL|D3DFVF_PSIZE );
			DWORD dwFirstFVFSize = D3DXGetFVFVertexSize( dwFirstFVF );
			DWORD dwFullFVFSize = D3DXGetFVFVertexSize( dwFVF );

			DWORD* pColor;
			BYTE* pVertices;
			m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pVertices );
			for ( DWORD i=0; i<dwVertices; ++i )
			{
				pColor = (DWORD*)(pVertices + dwFullFVFSize * i + dwFirstFVFSize);
				*pColor = 0xffffffff;
			}
			m_pLocalMesh->UnlockVertexBuffer();
		}

		CalculateMaxMin();
	}

	return S_OK;
}

HRESULT DxMeshes::SetFVF ( LPDIRECT3DDEVICEQ pd3dDevice, DWORD dwFVF )
{
	if ( !m_pLocalMesh )					return S_OK;
	if ( dwFVF == m_pSysMemMesh->GetFVF() )	return S_OK;

	LPD3DXMESH pTempLocalMesh  = NULL;

	DWORD dwOptions = D3DXMESH_MANAGED;
	if ( m_pLocalMesh->GetOptions () & D3DXMESH_32BIT )
		dwOptions |= D3DXMESH_32BIT;

	if( FAILED( m_pLocalMesh->CloneMeshFVF( dwOptions, dwFVF, pd3dDevice,
					&pTempLocalMesh ) ) )
	{
		return E_FAIL;
	}

	SAFE_RELEASE ( m_pLocalMesh );
	if ( pTempLocalMesh )  m_pLocalMesh  = pTempLocalMesh;

	if( m_pLocalMesh )
	{
		if ( m_pLocalMesh->GetFVF() == (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1) )
		{
			struct VERTEX { D3DXVECTOR3 vPos, Nor; D3DXVECTOR2 vTex; };

			DWORD dwVertices = m_pLocalMesh->GetNumVertices();

			VERTEX* pVertices;
			m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pVertices );
			for ( DWORD i=0; i<dwVertices; ++i )
			{
				D3DXVec3Normalize ( &pVertices[i].Nor, &pVertices[i].Nor );
			}
			m_pLocalMesh->UnlockVertexBuffer();
		}
		else if ( m_pLocalMesh->GetFVF() & D3DFVF_NORMAL )
		{
			D3DXComputeNormals( m_pLocalMesh, NULL );
		}

		CalculateMaxMin();
	}

    return S_OK;
}

void DxMeshes::CalculateMaxMin()
{
	if ( !m_pLocalMesh )
		return;

	m_vMax = D3DXVECTOR3( -FLT_MAX, -FLT_MAX, -FLT_MAX );
	m_vMin = D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX );

	DWORD dwVertices = m_pLocalMesh->GetNumVertices();

	DWORD dwFVFSize = D3DXGetFVFVertexSize ( m_pLocalMesh->GetFVF() );

	D3DXVECTOR3* pPosition;
	BYTE* pVertices;
	m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pVertices );
	for ( DWORD i=0; i<dwVertices; ++i )
	{
		pPosition = (D3DXVECTOR3*)( pVertices + dwFVFSize*i );

		if ( m_vMax.x < pPosition->x )
			m_vMax.x = pPosition->x;
		if ( m_vMax.y < pPosition->y )
			m_vMax.y = pPosition->y;
		if ( m_vMax.z < pPosition->z )
			m_vMax.z = pPosition->z;

		if ( m_vMin.x > pPosition->x )
			m_vMin.x = pPosition->x;
		if ( m_vMin.y > pPosition->y )
			m_vMin.y = pPosition->y;
		if ( m_vMin.z > pPosition->z )
			m_vMin.z = pPosition->z;
	}
	m_pLocalMesh->UnlockVertexBuffer();
}

// TangentMesh를 만든다.
HRESULT DxMeshes::CreateTangentMesh( LPDIRECT3DDEVICEQ pd3dDevice )
{
    LPD3DXMESH pMeshTangent;
    HRESULT hr = vm::CreateTagentMesh( pd3dDevice, m_pLocalMesh, &pMeshTangent );
	if( FAILED(hr) )
	{
		return hr;
	}
	if( !pMeshTangent )		
        return hr;

	LPD3DXMESH pTemp = m_pLocalMesh;
	m_pLocalMesh = pMeshTangent;
	SAFE_RELEASE( pTemp );

    return hr;
}

// 최적화된 TangetMesh를 만든다.
HRESULT DxMeshes::CompressTangentMesh( LPDIRECT3DDEVICEQ pd3dDevice )
{  
    HRESULT hr(S_OK);

    ID3DXMesh* pTempMesh = NULL;
    hr = m_pLocalMesh->CloneMeshFVF( m_pLocalMesh->GetOptions(), D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX2|D3DFVF_TEXCOORDSIZE1(1), pd3dDevice, &pTempMesh );	
    if ( !pTempMesh )
        return hr;

    float fTemp(0.f);
	vm::VERTEXNORTANGENTTEX2* pbVertSrc;
    vm::VERTEXNORTANGENTTEX2_COMPRESS* pbVertDestShader;
	m_pLocalMesh->LockVertexBuffer( 0, (VOID**)&pbVertSrc );
    pTempMesh->LockVertexBuffer( 0, (VOID**)&pbVertDestShader );
	for( DWORD i=0; i<m_pLocalMesh->GetNumVertices(); ++i )
	{
		pbVertDestShader[i].vPos = pbVertSrc[i].vPos;

		D3DXVec3Normalize( &pbVertSrc[i].vNor, &pbVertSrc[i].vNor );	// 노멀라이즈를 해준다.
		pbVertDestShader[i].vNor[0] = static_cast<short>( pbVertSrc[i].vNor.x * 32767.f );
		pbVertDestShader[i].vNor[1] = static_cast<short>( pbVertSrc[i].vNor.y * 32767.f );
		pbVertDestShader[i].vNor[2] = static_cast<short>( pbVertSrc[i].vNor.z * 32767.f );

		D3DXVec3Normalize( &pbVertSrc[i].vTan, &pbVertSrc[i].vTan );	// 노멀라이즈를 해준다.
		pbVertDestShader[i].vTan[0] = static_cast<short>( pbVertSrc[i].vTan.x * 32767.f );
		pbVertDestShader[i].vTan[1] = static_cast<short>( pbVertSrc[i].vTan.y * 32767.f );
		pbVertDestShader[i].vTan[2] = static_cast<short>( pbVertSrc[i].vTan.z * 32767.f );

        pbVertDestShader[i].vTex0 = pbVertSrc[i].vTex[0];
        pbVertDestShader[i].vTex1[0] = static_cast<short>( pbVertSrc[i].vTex[0].x * 32767.f );
        pbVertDestShader[i].vTex1[1] = static_cast<short>( pbVertSrc[i].vTex[1].x * 32767.f ); // beyond array bound
	}
    pTempMesh->UnlockVertexBuffer();
	m_pLocalMesh->UnlockVertexBuffer();

    SAFE_RELEASE( m_pLocalMesh );
    m_pLocalMesh = pTempMesh;

    return hr;
}

HRESULT DxMeshes::RenderVisualMaterial( LPDIRECT3DDEVICEQ pd3dDevice, LPDIRECT3DVERTEXDECLARATION9 pDCRT )
{
    if ( !m_pLocalMesh )
        return E_FAIL;

//	for ( DWORD i=0; i<cMaterials; i++ )
	{
        DrawMeshFX( pd3dDevice, m_pLocalMesh, pDCRT );
	}

    return S_OK;
}

void DxMeshes::DrawMeshFX( LPDIRECT3DDEVICEQ pd3dDevice, LPD3DXMESH pMesh, LPDIRECT3DVERTEXDECLARATION9 pDCRT )
{
    pd3dDevice->SetFVF( 0 );
    pd3dDevice->SetVertexDeclaration( pDCRT );

    LPDIRECT3DVERTEXBUFFER9 pVB(NULL);
    LPDIRECT3DINDEXBUFFER9 pIB(NULL);

    pMesh->GetVertexBuffer( &pVB );
    pMesh->GetIndexBuffer( &pIB );
    pd3dDevice->SetStreamSource( 0, pVB, 0, sizeof(vm::VERTEXNORTANGENTTEX2_COMPRESS) );
    pd3dDevice->SetIndices( pIB );
    pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, pMesh->GetNumVertices(), 0, pMesh->GetNumFaces() );

    SAFE_RELEASE( pVB );
    SAFE_RELEASE( pIB );
}

HRESULT DxFrameMesh::SetFVF ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur, DWORD *pdwFVF )
{
	HRESULT hr = S_OK;

	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : 메쉬 FVF 설정.
	//
	pmsMeshs = pframeCur->pmsMeshs;
	while ( pmsMeshs != NULL )
	{
		if ( pdwFVF )
		{
			hr = pmsMeshs->SetFVF ( pd3dDevice, *pdwFVF );
			if (FAILED(hr))	return hr;
		}
		else
		{
			hr = pmsMeshs->SetFVF ( pd3dDevice, m_dwFVF );
			if (FAILED(hr))	return hr;
		}

		pmsMeshs = pmsMeshs->m_pMeshNext;
	}

	//	Note : 자식 프레임 순환.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = SetFVF ( pd3dDevice, pframeChild );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	return S_OK;
}

void DxFrameMesh::SetFile ( const char* szPath, const char *szFileName )
{
	SAFE_DELETE_ARRAY ( m_pszFileName );

	if( !szFileName )	return;
	
	int nStrLen = static_cast<int>( strlen(szFileName)+1 );
	m_pszFileName = new char[nStrLen];
	StringCchCopy( m_pszFileName, nStrLen, szFileName );

	m_strPath = szPath;
}

HRESULT DxFrameMesh::OneTimeSceneInit()
{
	return S_OK;
}

HRESULT DxFrameMesh::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr;
	hr = LoadMeshHierarchy ( pd3dDevice );
	if ( FAILED(hr) ) return hr;

	//	Note : 만약 이미 등록된 효과가 존재할 경우에 이를 다시 연결시켜 준다.
	//
	DxEffectBase *pEffectCur = m_pEffectHead;
	while ( pEffectCur != NULL )
	{
		pEffectCur->InitDeviceObjects ( pd3dDevice );

		//	Note : 프레임 트리가 새로이 생성되었으므로 효과도 새로이 연결해준다.
		//
		DxFrame *pFrame = FindFrame ( pEffectCur->GetAdaptFrameName() );
		pEffectCur->AdaptToDxFrame ( pFrame, pd3dDevice );

		pEffectCur = pEffectCur->pEffectNext;
	}

	//	Note : 로컬 메쉬 생성.
	//
	hr = MakeLocalMeshs ( pd3dDevice, m_pDxFrame );
	if ( FAILED(hr) ) return hr;

	return S_OK;
}

HRESULT DxFrameMesh::MakeLocalMeshs ( LPDIRECT3DDEVICEQ pd3dDevice, DxFrame *pframeCur )
{
	HRESULT hr = S_OK;

	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : 메쉬 FVF 설정.
	//
	pmsMeshs = pframeCur->pmsMeshs;
	while ( pmsMeshs != NULL )
	{
		hr = pmsMeshs->MakeLocalMeshs ( pd3dDevice, m_dwFVF );
		if (FAILED(hr))
			return hr;

		pmsMeshs = pmsMeshs->m_pMeshNext;
	}

	//	Note : 자식 프레임 순환.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = MakeLocalMeshs ( pd3dDevice, pframeChild );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	return S_OK;
}

HRESULT DxFrameMesh::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	if ( !m_pDxFrame ) return S_OK;

	//	Note : 효과들.
	//
	DxEffectBase *pEffectCur = m_pEffectHead;
	while ( pEffectCur != NULL )
	{
		pEffectCur->RestoreDeviceObjects ( pd3dDevice );
		pEffectCur = pEffectCur->pEffectNext;
	}

	if( m_pTextureEff )
        m_pTextureEff->OnResetDevice( pd3dDevice );

	return S_OK;
}

HRESULT DxFrameMesh::DeleteLocalMeshs ( DxFrame *pframeCur )
{
	HRESULT hr = S_OK;

	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : 메쉬
	//
	pmsMeshs = pframeCur->pmsMeshs;
	while ( pmsMeshs != NULL )
	{
		GXRELEASE ( pmsMeshs->m_pLocalMesh );

		pmsMeshs = pmsMeshs->m_pMeshNext;
	}

	//	Note : 자식 프레임 순환.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		hr = DeleteLocalMeshs ( pframeChild );
		if (FAILED(hr))
			return hr;

		pframeChild = pframeChild->pframeSibling;
	}

	return S_OK;
}

HRESULT DxFrameMesh::InvalidateDeviceObjects( LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 효과 정리.
	//
	DxEffectBase *pEffectCur = m_pEffectHead;
	while ( pEffectCur != NULL )
	{
		pEffectCur->InvalidateDeviceObjects ();
		pEffectCur = pEffectCur->pEffectNext;
	}

	if( m_pTextureEff )
        m_pTextureEff->OnLostDevice();

	return S_OK;
}

HRESULT DxFrameMesh::DeleteDeviceObjects()
{
	SAFE_DELETE ( m_pDxFrame );

	m_pCollDetectFrame = NULL;

	//	Note : 효과 정리.
	//
	DxEffectBase *pEffectCur = m_pEffectHead;
	while ( pEffectCur != NULL )
	{
		pEffectCur->DeleteDeviceObjects ();
		pEffectCur = pEffectCur->pEffectNext;
	}

	if( m_pTextureEff )
        m_pTextureEff->OnDestroyDevice();

	m_scpMaterialSetting->CleanUp();

	return S_OK;
}

HRESULT DxFrameMesh::FinalCleanup()
{
	SAFE_DELETE(m_pEffectHead);

	DxAnimationMan *pAnimManCur = pAnimManHead;
	while ( pAnimManCur )
	{
		pAnimManCur->SetDxFrameRoot(NULL);
		pAnimManCur->SetAnimationHead(NULL);

		pAnimManCur = pAnimManCur->m_pNext;
	}

	SAFE_DELETE(pAnimManHead);


	SAFE_DELETE(pAnimHead);
	SAFE_DELETE(m_pPieceObjTree);
	SAFE_DELETE(m_pPieceObjHead);
	SAFE_DELETE( m_pTextureEff );

	return S_OK;
}

HRESULT DxFrame::CalculateBoundingBoxKey ( D3DXMATRIX *pmatCur,
								  D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin, float fTime )
{
	HRESULT hr = S_OK;
	PBYTE pbPoints = NULL;
	PBYTE pbCur;
	D3DXVECTOR3 *pvCur;
	D3DXVECTOR3 vDist;
	UINT iPoint;
	UINT cVertices;
	DxMeshes *pmsCur;
	DxFrame *pframeCur;
    
	D3DXMATRIXA16 matLocal;

	//	Note : 현제 시간의 에니메이션 트렌스폼을 적용.
	//
	if ( pframeFromAnimate )
		pframeFromAnimate->SetTime ( fTime );

	D3DXMatrixMultiply ( &matLocal, &matRot, pmatCur );

	pmsCur = pmsMeshs;
	while ( pmsCur != NULL )
	{
		DWORD fvfsize;
		if ( pmsCur->m_pDxOctreeMesh )
		{
			fvfsize = D3DXGetFVFVertexSize ( pmsCur->m_pDxOctreeMesh->m_dwFVF );
			cVertices = pmsCur->m_pDxOctreeMesh->m_dwNumVertices;

			hr = pmsCur->m_pDxOctreeMesh->LockVertexBuffer( (VOID**)&pbPoints );
			if ( FAILED(hr) )	goto e_Exit;
		}
		else
		{
			fvfsize = D3DXGetFVFVertexSize ( pmsCur->m_pSysMemMesh->GetFVF() );
			cVertices = pmsCur->m_pSysMemMesh->GetNumVertices();

			hr = pmsCur->m_pSysMemMesh->LockVertexBuffer ( 0, (VOID**)&pbPoints );
			if ( FAILED(hr) )	goto e_Exit;
		}

		for ( iPoint=0, pbCur = pbPoints; iPoint < cVertices; iPoint++, pbCur += fvfsize )
		{
			pvCur = (D3DXVECTOR3*)pbCur;

			D3DXVec3TransformCoord ( &vDist, pvCur, &matLocal );

			if ( vDist.x > vMax.x )	vMax.x = vDist.x;
			if ( vDist.y > vMax.y )	vMax.y = vDist.y;
			if ( vDist.z > vMax.z )	vMax.z = vDist.z;

			if ( vDist.x < vMin.x )	vMin.x = vDist.x;
			if ( vDist.y < vMin.y )	vMin.y = vDist.y;
			if ( vDist.z < vMin.z )	vMin.z = vDist.z;
		}

		if ( pmsCur->m_pDxOctreeMesh )	pmsCur->m_pDxOctreeMesh->UnlockVertexBuffer();
		else							pmsCur->m_pSysMemMesh->UnlockVertexBuffer();
		pbPoints = NULL;

		pmsCur = pmsCur->m_pMeshNext;
	}

	pframeCur = pframeFirstChild;
	while (pframeCur!=NULL)
	{
		hr = pframeCur->CalculateBoundingBoxKey ( &matLocal, vMax, vMin, fTime );
		if ( FAILED(hr) )
			goto e_Exit;
			
		pframeCur = pframeCur->pframeSibling;
	}

e_Exit:
	if ( pbPoints != NULL )
	{
		pmsCur->m_pSysMemMesh->UnlockVertexBuffer();
	}

	return hr;
}

HRESULT DxFrame::CalculateBoundingBox ( D3DXMATRIX *pmatCur,
								  D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin, float fUnitTime, BOOL bAnimate )
{
	HRESULT hr = S_OK;
	PBYTE pbPoints = NULL;
	PBYTE pbCur;
	D3DXVECTOR3 *pvCur;
	D3DXVECTOR3 vDist;
	UINT iPoint;
	UINT cVertices;
	DxMeshes *pmsCur;
	DxFrame *pframeCur;
    
	D3DXMATRIXA16 matLocal;

	if ( !pframeFromAnimate || !bAnimate )
	{
		D3DXMatrixMultiply ( &matLocal, &matRot, pmatCur );

		pmsCur = pmsMeshs;
		while ( pmsCur != NULL )
		{
			DWORD fvfsize;
			if ( pmsCur->m_pDxOctreeMesh )
			{
				fvfsize = D3DXGetFVFVertexSize ( pmsCur->m_pDxOctreeMesh->m_dwFVF );
				cVertices = pmsCur->m_pDxOctreeMesh->m_dwNumVertices;

				hr = pmsCur->m_pDxOctreeMesh->LockVertexBuffer( (VOID**)&pbPoints );
				if ( FAILED(hr) )	goto e_Exit;
			}
			else
			{
				fvfsize = D3DXGetFVFVertexSize ( pmsCur->m_pSysMemMesh->GetFVF() );
				cVertices = pmsCur->m_pSysMemMesh->GetNumVertices();

				hr = pmsCur->m_pSysMemMesh->LockVertexBuffer ( 0, (VOID**)&pbPoints );
				if ( FAILED(hr) )	goto e_Exit;
			}


			for ( iPoint=0, pbCur = pbPoints; iPoint < cVertices; iPoint++, pbCur += fvfsize )
			{
				pvCur = (D3DXVECTOR3*)pbCur;

				D3DXVec3TransformCoord ( &vDist, pvCur, &matLocal );

				if ( vDist.x > vMax.x )	vMax.x = vDist.x;
				if ( vDist.y > vMax.y )	vMax.y = vDist.y;
				if ( vDist.z > vMax.z )	vMax.z = vDist.z;

				if ( vDist.x < vMin.x )	vMin.x = vDist.x;
				if ( vDist.y < vMin.y )	vMin.y = vDist.y;
				if ( vDist.z < vMin.z )	vMin.z = vDist.z;
			}

			if ( pmsCur->m_pDxOctreeMesh )	pmsCur->m_pDxOctreeMesh->UnlockVertexBuffer();
			else							pmsCur->m_pSysMemMesh->UnlockVertexBuffer();
			pbPoints = NULL;

			pmsCur = pmsCur->m_pMeshNext;
		}

		pframeCur = pframeFirstChild;
		while (pframeCur!=NULL)
		{
			hr = pframeCur->CalculateBoundingBox ( &matLocal, vMax, vMin, fUnitTime, bAnimate );
			if ( FAILED(hr) )
				goto e_Exit;
			
			pframeCur = pframeCur->pframeSibling;
		}
	}
	else
	{
		D3DXVECTOR3 vMaxNew(-FLT_MAX,-FLT_MAX,-FLT_MAX);
		D3DXVECTOR3 vMinNew(FLT_MAX,FLT_MAX,FLT_MAX);
		float fMaxTime = pframeFromAnimate->GetAnimTimeLine ();

		for ( float fTime = 0.0f; fTime <= fMaxTime; fTime += fUnitTime )
		{
			CalculateBoundingBoxKey ( pmatCur, vMaxNew, vMinNew, fTime );
		
			if ( vMax.x < vMaxNew.x )	vMax.x = vMaxNew.x;
			if ( vMax.y < vMaxNew.y )	vMax.y = vMaxNew.y;
			if ( vMax.z < vMaxNew.z )	vMax.z = vMaxNew.z;

			if ( vMin.x > vMinNew.x )	vMin.x = vMinNew.x;
			if ( vMin.y > vMinNew.y )	vMin.y = vMinNew.y;
			if ( vMin.z > vMinNew.z )	vMin.z = vMinNew.z;
		}
	}

e_Exit:
	if ( pbPoints != NULL )
	{
		pmsCur->m_pSysMemMesh->UnlockVertexBuffer();
	}

	return hr;
}

HRESULT DxFrame::CalculateBoundingSphere ( D3DXMATRIX *pmatCur, float fUnitTime )
{
	HRESULT hr = S_OK;
    
	DxFrame *pframeCur;

	D3DXVECTOR3 vCenter(0,0,0);
	D3DXVECTOR3 vMax(-FLT_MAX,-FLT_MAX,-FLT_MAX);
	D3DXVECTOR3 vMin(FLT_MAX,FLT_MAX,FLT_MAX);
	UINT cVertices = 0;
	float fRadiusSq = 0;
    
	D3DXMATRIXA16 matCur;
    
	//	Note : 줌심점에서 최대 거리에 있는 백터를 찾아 반경 값을 가져온다.
	//
	matCur = *pmatCur;
	hr = CalculateBoundingBox( &matCur, vMax, vMin, fUnitTime, TRUE );
	if (FAILED(hr))
		goto e_Exit;
    
	//	Note : 계산 결과 저장.
	//
	vTreeMax = vMax;
	vTreeMin = vMin;

	if ( vMax==D3DXVECTOR3(-FLT_MAX,-FLT_MAX,-FLT_MAX) && vMin==D3DXVECTOR3(FLT_MAX,FLT_MAX,FLT_MAX) )
	{
		vTreeMax = D3DXVECTOR3(0,0,0);
		vTreeMin = D3DXVECTOR3(0,0,0);
	}
	else
	{
		vTreeMax = D3DXVECTOR3(vTreeMax.x+0.01f,vTreeMax.y+0.01f,vTreeMax.z+0.01f);
		vTreeMin = D3DXVECTOR3(vTreeMin.x+0.01f,vTreeMin.y+0.01f,vTreeMin.z+0.01f);
	}
	
	//	Note : 자식 노드들 계산.
	//
	pframeCur = pframeFirstChild;
	while (pframeCur!=NULL)
	{
		D3DXMATRIXA16 matLocal;
		D3DXMatrixMultiply ( &matLocal, &matRot, pmatCur );

		hr = pframeCur->CalculateBoundingSphere ( &matLocal, fUnitTime );
		if ( FAILED(hr) )
			goto e_Exit;
		
		pframeCur = pframeCur->pframeSibling;
	}

e_Exit:
	return hr;
}

void DxFrameMesh::SetFVF ( DWORD dwFVF, BOOL bNow, LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_dwFVF = dwFVF;

	if ( bNow )
		SetFVF ( pd3dDevice, m_pDxFrame );
}

void DxFrameMesh::ReloadTexEffVMFX( LPDIRECT3DDEVICEQ pd3dDevice )
{
    DxTextureEffMan* pTextureEff = GetTextureEffMan();
    if ( pTextureEff )
    {
        pTextureEff->ReloadVMFX( pd3dDevice );
    }
}

// Field의 TriangleLinkLight 를 생성하도록 한다.
void DxFrameMesh::CreateTriangleLinkLight( LPDIRECT3DDEVICEQ pd3dDevice, 
											 DxStaticMeshCollBase* pStaticMesh, 
											 DxPieceManager* pPieceManager,
											 SET_DWORD& setEraseLightID,
											 BOOL bDirectionLight )
{
	if ( !m_pDxFrame )
		return;

	// 각 삼각형에 영향주는 모든 Light를 구하도록 한다.
	std::vector<DATA_FOR_TRIANGLE_LINK_LIGHT> vecDataForTriangleLinkLight;	// 전 삼각형의 정점과 그정점의 면ID를 기록하는 곳.
	std::vector<VEC_DWORD> vecTriangle_vecLightID;							// 전 삼각형과 연결된 Light 기록하는 곳.
	CreateTriangleLinkLight
	( 
		pd3dDevice, 
		pStaticMesh, 
		pPieceManager,
		setEraseLightID,
		vecDataForTriangleLinkLight,
		vecTriangle_vecLightID,
		m_pDxFrame,
		bDirectionLight
	);

	//	Note : 
	ExportProgress::SetStateString ( "[std::sort] - DxFrameMesh::CreateTriangleLinkLight" );

	// [NOTE] 연결된 삼각형에도 정보를 주기위한 작업 보류
	//// x 축으로 sorting 한다.
	//std::sort( vecDataForTriangleLinkLight.begin(), vecDataForTriangleLinkLight.end(), DATA_FOR_TRIANGLE_LINK_LIGHT_SORT() );

	//	Note : 
	ExportProgress::SetStateString ( "[PermeateTriangleLinkLight] - DxFrameMesh::CreateTriangleLinkLight" );

	// [NOTE] 연결된 삼각형에도 정보를 주기위한 작업 보류
	//// [CascadeShadow] 작업보류중
	//// Triangle Link Light - 연결된 삼각형 Light 전파하기
	//{
	//	PermeateTriangleLinkLight( vecDataForTriangleLinkLight, vecTriangle_vecLightID, m_pDxFrame );
	//}

	// 필요없는 데이터 정리.
	vecDataForTriangleLinkLight.clear();
	vecTriangle_vecLightID.clear();

	//	Note : 
	ExportProgress::SetStateString ( "DxFrameMesh::CreateTriangleLinkLight - EraseLightID" );

	//// LightID 를 순차적으로 회전해서 m_vecTriangle_wwLightID 에 값을 넣도록 한다.
	//DWORD dwLightNumber = NSShadowLight::GetLightNumber();

	//// dwLightNumber+1 을 한 이유는 Direction Light가 0 으로 쓰이고 있기 때문이다.
	//// 0 - Direction Light
	//// 1~(dwLightNumber+1) - SpotLight
	//for ( DWORD i=0; i<dwLightNumber+1; ++i )
	//{
	//	// 현재 라이트와 연관된 삼각형들의 첫번째 슬롯에 자리가 남는지 확인.
	//	if ( IsTriangleLightID_InsertSlot( i, m_pDxFrame, 0 ) )
	//	{
	//		// 현재 라이트와 연관된 삼각형들의 첫번째 슬롯에 현재 LightID 를 셋팅.
	//		InsertSlotTriangleLightID( i, m_pDxFrame, 0 );
	//		continue;
	//	}

	//	// 현재 라이트와 연관된 삼각형들의 두번째 슬롯에 자리가 남는지 확인.
	//	if ( IsTriangleLightID_InsertSlot( i, m_pDxFrame, 1 ) )
	//	{
	//		// 현재 라이트와 연관된 삼각형들의 두번째 슬롯에 현재 LightID 를 셋팅.
	//		InsertSlotTriangleLightID( i, m_pDxFrame, 1 );
	//		continue;
	//	}
	//}

	//// Triangle Link Light - Light가 3개 이상인 것 찾기 및 Light 갯수 2개로 제한
	//BOOL bLightOver3(TRUE);
	//while ( bLightOver3 )
	//{
	//	DWORD dwLightID(0);
	//	bLightOver3 = FALSE;

	//	if ( FindTriangleLimitLightNumOver3( dwLightID, m_pDxFrame ) )
	//	{
	//		// Erease 되는 Light 등록
	//		setEraseLightID.insert( dwLightID );

	//		// 특정 라이트를 줄여준다.
	//		EraseLightID( dwLightID, m_pDxFrame );
	//		bLightOver3 = TRUE;
	//	}
	//}
}

// Field의 TriangleLinkLight 를 생성하도록 한다.
void DxFrameMesh::CreateTriangleLinkLight( LPDIRECT3DDEVICEQ pd3dDevice, 
											 DxStaticMeshCollBase* pStaticMesh, 
											 DxPieceManager* pPieceManager,
											 SET_DWORD& setEraseLightID,
											 std::vector<DATA_FOR_TRIANGLE_LINK_LIGHT>& vecDataForTriangleLinkLight,
											 std::vector<VEC_DWORD>& vecTriangle_vecLightID,
											 DxFrame *pframeCur,
											 BOOL bDirectionLight )
{
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : 
	//
	pmsMeshs = pframeCur->pmsMeshs;
	if( pmsMeshs != NULL )
	{
		pmsMeshs->CreateTriangleLinkLight
		(
			pd3dDevice, 
			pStaticMesh, 
			pPieceManager,
			setEraseLightID,
			vecDataForTriangleLinkLight,
			vecTriangle_vecLightID,
			pframeCur->matCombined,
			bDirectionLight
		);
	}

	//	Note : 자식 프레임 순환.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		CreateTriangleLinkLight
		(
			pd3dDevice, 
			pStaticMesh, 
			pPieceManager,
			setEraseLightID,
			vecDataForTriangleLinkLight,
			vecTriangle_vecLightID,
			pframeChild,
			bDirectionLight
		);

		pframeChild = pframeChild->pframeSibling;
	}
}

// Field의 TriangleLinkLight 를 생성하도록 한다.
void DxMeshes::CreateTriangleLinkLight( LPDIRECT3DDEVICEQ pd3dDevice, 
									   DxStaticMeshCollBase* pStaticMesh, 
									   DxPieceManager* pPieceManager,
									   SET_DWORD& setEraseLightID,
									   std::vector<DATA_FOR_TRIANGLE_LINK_LIGHT>& vecDataForTriangleLinkLight,
									   std::vector<VEC_DWORD>& vecTriangle_vecLightID,
									   const D3DXMATRIX& matWorld,
									   BOOL bDirectionLight )
{
	if ( !m_pLocalMesh )
		return;

	DWORD dwVertices = m_pLocalMesh->GetNumVertices();
	DWORD dwFaces = m_pLocalMesh->GetNumFaces();

	DWORD dwFVFSize = D3DXGetFVFVertexSize ( m_pLocalMesh->GetFVF() );

	VERTEXNOR sData;
	std::vector<VERTEXNOR> vecVerticesS;
	vecVerticesS.reserve( dwFaces*3 );

	D3DXVECTOR3* pPosition;
	D3DXVECTOR3* pNormal;
	BYTE* pVertices;
	WORD* pIndices;
	m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pVertices );
	m_pLocalMesh->LockIndexBuffer ( 0L, (VOID**)&pIndices );
	for ( DWORD i=0; i<dwFaces; ++i )
	{
		for ( DWORD z=0; z<3; ++z )
		{
			DWORD dwVertexIndex = pIndices[i*3+z];

			pPosition = (D3DXVECTOR3*)( pVertices + dwFVFSize*dwVertexIndex );
			pNormal = (D3DXVECTOR3*)( pVertices + dwFVFSize*dwVertexIndex + sizeof(D3DXVECTOR3) );

			sData.vPos = *pPosition;
			sData.vNor = *pNormal;

			D3DXVec3TransformCoord( &sData.vPos, &sData.vPos, &matWorld );
			D3DXVec3TransformNormal( &sData.vNor, &sData.vNor, &matWorld );

			vecVerticesS.push_back( sData );
		}
	}
	m_pLocalMesh->UnlockIndexBuffer();
	m_pLocalMesh->UnlockVertexBuffer();

	DWORD dwFace = vecVerticesS.size()/3;

	// clear
	m_vecTriangle_vecLightID.clear();
	m_vecTriangle_wwLightID.clear();
	m_vecTriangle_vecLightID.reserve( dwFace );
	m_vecTriangle_wwLightID.reserve( dwFace );

	int nCount = 0;

	// 각 삼각형 마다 영향받는 Light를 얻도록 하자.
	std::vector<VERTEXNOR> vecVerticesD;
	for ( DWORD i=0; i<dwFace; ++i )
	{
		vecVerticesD.clear();
		vecVerticesD.push_back( vecVerticesS[i*3+0] );
		vecVerticesD.push_back( vecVerticesS[i*3+1] );
		vecVerticesD.push_back( vecVerticesS[i*3+2] );

		VEC_DWORD vecLightID;
		NSLightMapTools::OneTriangleIncludeLight
		( 
			vecLightID,
			pd3dDevice, 
			pStaticMesh, 
			pPieceManager, 
			vecVerticesD,
			bDirectionLight
		);

		// 사용할 수 없는 Light 는 지우도록 한다.
		VEC_DWORD_ITER iter = vecLightID.begin();
		for ( ; iter!=vecLightID.end(); )
		{
			if ( setEraseLightID.find( (*iter) ) != setEraseLightID.end() )
			{
				iter = vecLightID.erase( iter );
			}
			else
			{
				++iter;
			}
		}

		m_vecTriangle_vecLightID.push_back( vecLightID );
		m_vecTriangle_wwLightID.push_back( WORDWORD(-1,-1) );


		// [NOTE] 연결된 삼각형에도 정보를 주기위한 작업 보류
		//// TriangleLinkLight 를 위한 Triangle 정보를 셋팅한다.
		//{
		//	vecTriangle_vecLightID.push_back( vecLightID );

		//	DATA_FOR_TRIANGLE_LINK_LIGHT sData;

		//	sData.m_sVertexNor = vecVerticesS[i*3+0];
		//	sData.m_dwFaceID = i;
		//	vecDataForTriangleLinkLight.push_back( sData );

		//	sData.m_sVertexNor = vecVerticesS[i*3+1];
		//	sData.m_dwFaceID = i;
		//	vecDataForTriangleLinkLight.push_back( sData );

		//	sData.m_sVertexNor = vecVerticesS[i*3+2];
		//	sData.m_dwFaceID = i;
		//	vecDataForTriangleLinkLight.push_back( sData );
		//}

		++nCount;
		if( nCount >= 1000 )
		{
			nCount = 0;
			TSTRINGSTREAM strStringStream;
			strStringStream << "CreateTriangleLinkLight - ";
			strStringStream << i+1;
			strStringStream << "/";
			strStringStream << dwFace;
			ExportProgress::SetStateString( strStringStream.str().c_str() );
		}
	}
}

// Triangle Link Light - 연결된 삼각형 Light 전파하기
void DxFrameMesh::PermeateTriangleLinkLight( std::vector<DATA_FOR_TRIANGLE_LINK_LIGHT>& vecDataForTriangleLinkLight,
											std::vector<VEC_DWORD>& vecTriangle_vecLightID,
											DxFrame *pframeCur )
{
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : 
	//
	pmsMeshs = pframeCur->pmsMeshs;
	if( pmsMeshs != NULL )
	{
		pmsMeshs->PermeateTriangleLinkLight( vecDataForTriangleLinkLight, vecTriangle_vecLightID, pframeCur->matCombined );
	}

	//	Note : 자식 프레임 순환.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		PermeateTriangleLinkLight ( vecDataForTriangleLinkLight, vecTriangle_vecLightID, pframeChild );
		pframeChild = pframeChild->pframeSibling;
	}
}

// Triangle Link Light - 연결된 삼각형 Light 전파하기
void DxMeshes::PermeateTriangleLinkLight( std::vector<DATA_FOR_TRIANGLE_LINK_LIGHT>& vecDataForTriangleLinkLight,
										 std::vector<VEC_DWORD>& vecTriangle_vecLightID,
										 const D3DXMATRIX& matWorld )
{
	if ( !m_pLocalMesh )
		return;

	DWORD dwVertices = m_pLocalMesh->GetNumVertices();
	DWORD dwFaces = m_pLocalMesh->GetNumFaces();

	DWORD dwFVFSize = D3DXGetFVFVertexSize ( m_pLocalMesh->GetFVF() );

	D3DXVECTOR3 vPosistion;
	D3DXVECTOR3* pPosition;
	BYTE* pVertices;
	WORD* pIndices;
	m_pLocalMesh->LockVertexBuffer ( 0L, (VOID**)&pVertices );
	m_pLocalMesh->LockIndexBuffer ( 0L, (VOID**)&pIndices );
	for ( DWORD i=0; i<dwFaces; ++i )
	{
		for ( DWORD z=0; z<3; ++z )
		{
			DWORD dwVertexIndex = pIndices[i*3+z];

			pPosition = (D3DXVECTOR3*)( pVertices + dwFVFSize*dwVertexIndex );
			D3DXVec3TransformCoord( &vPosistion, pPosition, &matWorld );

			std::vector<DATA_FOR_TRIANGLE_LINK_LIGHT>::iterator iter = vecDataForTriangleLinkLight.begin();
			while ( TRUE )
			{
				iter = std::find_if( iter, vecDataForTriangleLinkLight.end(), DATA_FOR_TRIANGLE_LINK_LIGHT_FIND(vPosistion.x) );
				if ( iter == vecDataForTriangleLinkLight.end() )
					break;

				if ( COLLISION::IsSamePosition ( vPosistion, (*iter).m_sVertexNor.vPos ) )
				{
					// 추가하도록 한다.
					const VEC_DWORD& vecLightID_S = vecTriangle_vecLightID[ (*iter).m_dwFaceID ];
					VEC_DWORD& vecLightID_D = m_vecTriangle_vecLightID[ i ];
					for ( DWORD y=0; y<vecLightID_S.size(); ++y )
					{
						BOOL bExist(FALSE);
						for ( DWORD w=0; w<vecLightID_D.size(); ++w )
						{
							if ( vecLightID_S[y] == vecLightID_D[w] )
							{
								bExist = TRUE;
								break;	// 존재한다면 건너띈다.
							}
						}

						// 새로 추가한다.
						if ( !bExist )
						{
							vecLightID_D.push_back( vecLightID_S[y] );
						}
					}
				}

				++iter;
			}
		}
	}
	m_pLocalMesh->UnlockIndexBuffer();
	m_pLocalMesh->UnlockVertexBuffer();
}

//  현재 라이트와 연관된 삼각형들의 특정 슬롯에 자리가 남는지 확인.
BOOL DxFrameMesh::IsTriangleLightID_InsertSlot( DWORD dwLightID, DxFrame *pframeCur, DWORD dwSlot )
{
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : 
	//
	pmsMeshs = pframeCur->pmsMeshs;
	if( pmsMeshs != NULL )
	{
		if ( !pmsMeshs->IsTriangleLightID_InsertSlot( dwLightID, dwSlot ) )
			return FALSE;
	}

	//	Note : 자식 프레임 순환.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		if ( !IsTriangleLightID_InsertSlot( dwLightID, pframeChild, dwSlot ) )
			return FALSE;

		pframeChild = pframeChild->pframeSibling;
	}

	return TRUE;
}

//  현재 라이트와 연관된 삼각형들의 특정 슬롯에 자리가 남는지 확인.
BOOL DxMeshes::IsTriangleLightID_InsertSlot( DWORD dwLightID, DWORD dwSlot )
{
	for ( DWORD i=0; i<m_vecTriangle_vecLightID.size(); ++i )
	{
		const VEC_DWORD& vecLightID = m_vecTriangle_vecLightID[i];
		const WORDWORD& wwLightID = m_vecTriangle_wwLightID[i];
		VEC_DWORD::const_iterator citer = std::find( vecLightID.begin(), vecLightID.end(), dwLightID );
		if ( citer!=vecLightID.end() )
		{
			if ( dwSlot == 0 )		// 1번슬롯
			{
				if ( wwLightID.wA != WORD(-1) )
				{
					return FALSE;	// 값이 있다면 FALSE
				}
			}
			else					// 2번슬롯
			{
				if ( wwLightID.wB != WORD(-1) )
				{
					return FALSE;	// 값을 있다면 FALSE
				}
			}
		}
	}

	return TRUE;	// 현재 공간에 넣을 수 있다.
}

// 현재 라이트와 연관된 삼각형들의 특정 슬롯에 현재 LightID 를 셋팅.
void DxFrameMesh::InsertSlotTriangleLightID( DWORD dwLightID, DxFrame *pframeCur, DWORD dwSlot )
{
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : 
	//
	pmsMeshs = pframeCur->pmsMeshs;
	if( pmsMeshs != NULL )
	{
		pmsMeshs->InsertSlotTriangleLightID( dwLightID, dwSlot );
	}

	//	Note : 자식 프레임 순환.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		InsertSlotTriangleLightID( dwLightID, pframeChild, dwSlot );
		pframeChild = pframeChild->pframeSibling;
	}
}

// 현재 라이트와 연관된 삼각형들의 특정 슬롯에 현재 LightID 를 셋팅.
void DxMeshes::InsertSlotTriangleLightID( DWORD dwLightID, DWORD dwSlot )
{
	for ( DWORD i=0; i<m_vecTriangle_vecLightID.size(); ++i )
	{
		const VEC_DWORD& vecLightID = m_vecTriangle_vecLightID[i];
		WORDWORD& wwLightID = m_vecTriangle_wwLightID[i];
		VEC_DWORD::const_iterator citer = std::find( vecLightID.begin(), vecLightID.end(), dwLightID );
		if ( citer!=vecLightID.end() )
		{
			if ( dwSlot == 0 )		// 1번슬롯
			{
				wwLightID.wA = static_cast<WORD>( dwLightID );
			}
			else					// 2번슬롯
			{
				wwLightID.wB = static_cast<WORD>( dwLightID );
			}
		}
	}
}

// Triangle Link Light - Light가 3개 이상인 것 찾기 및 Light 갯수 2개로 제한
void DxFrameMesh::EraseLightID4LightOver3( SET_DWORD& setEraseLightID )
{
	// Triangle Link Light - Light가 3개 이상인 것 찾기 및 Light 갯수 2개로 제한
	BOOL bLightOver3(TRUE);
	while ( bLightOver3 )
	{
		DWORD dwLightID(0);
		bLightOver3 = FALSE;

		if ( FindTriangleLimitLightNumOver3( dwLightID, m_pDxFrame ) )
		{
			// Erease 되는 Light 등록
			setEraseLightID.insert( dwLightID );

			// 특정 라이트를 줄여준다.
			EraseLightID( dwLightID, m_pDxFrame );
			bLightOver3 = TRUE;
		}
	}
}

// setEraseLightID 을 바탕으로 FrameMesh, 쪽 LightID를 삭제 하도록 한다.
void DxFrameMesh::EraseLightID4TriangleLinkLight( const SET_DWORD& setEraseLightID )
{
	// 지워야될 LightID 들
	SET_DWORD_CITER citer = setEraseLightID.begin();
	for ( ; citer!=setEraseLightID.end(); ++citer )
	{
		// 실제 Erease 시킨다.
		EraseLightID( (*citer), m_pDxFrame );
	}
}

// m_vecTriangle_vecLightID 과 m_vecTriangle_wwLightID 를 비교하여 m_vecTriangle_wwLightID 에 없는 것은 삭제할 수 있게 dwLightID 를 넘긴다.
BOOL DxFrameMesh::FindTriangleLimitLightNumOver3( DWORD& dwLightID, DxFrame *pframeCur ) const
{
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : 
	//
	pmsMeshs = pframeCur->pmsMeshs;
	if( pmsMeshs != NULL )
	{
		if ( pmsMeshs->FindTriangleLimitLightNumOver3( dwLightID ) )
			return TRUE;
	}

	//	Note : 자식 프레임 순환.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		if ( FindTriangleLimitLightNumOver3 ( dwLightID, pframeChild ) )
			return TRUE;

		pframeChild = pframeChild->pframeSibling;
	}

	return FALSE;
}

// m_vecTriangle_vecLightID 과 m_vecTriangle_wwLightID 를 비교하여 m_vecTriangle_wwLightID 에 없는 것은 삭제할 수 있게 dwLightID 를 넘긴다.
BOOL DxMeshes::FindTriangleLimitLightNumOver3( DWORD& dwLightID ) const
{
	for ( DWORD i=0; i<m_vecTriangle_vecLightID.size(); ++i )
	{
		const VEC_DWORD& vecLightID = m_vecTriangle_vecLightID[i];
		const WORDWORD& wwLightID = m_vecTriangle_wwLightID[i];
		for ( DWORD z=0; z<vecLightID.size(); ++z )
		{
			if ( vecLightID[z] == wwLightID.wA )
			{
				continue;
			}
			else if ( vecLightID[z] == wwLightID.wB )
			{
				continue;
			}

			dwLightID = vecLightID[z];
			return TRUE;
		}
	}

	return FALSE;
}

// 특정 Light를 줄여주도록 한다.
void DxFrameMesh::EraseLightID( DWORD dwLightID, DxFrame *pframeCur )
{
	DxMeshes *pmsMeshs;
	DxFrame *pframeChild;

	//	Note : 
	//
	pmsMeshs = pframeCur->pmsMeshs;
	if( pmsMeshs != NULL )
	{
		pmsMeshs->EraseLightID( dwLightID );
	}

	//	Note : 자식 프레임 순환.
	//
	pframeChild = pframeCur->pframeFirstChild;
	while ( pframeChild != NULL )
	{
		EraseLightID ( dwLightID, pframeChild );
		pframeChild = pframeChild->pframeSibling;
	}
}

// 특정 Light를 줄여주도록 한다.
void DxMeshes::EraseLightID( DWORD dwLightID )
{
	for ( DWORD i=0; i<m_vecTriangle_vecLightID.size(); ++i )
	{
		VEC_DWORD& vecLightID = m_vecTriangle_vecLightID[i];
		VEC_DWORD_ITER iter = vecLightID.begin();
		for ( ; iter!=vecLightID.end(); ++iter )
		{
			if ( dwLightID == (*iter) )
			{
				vecLightID.erase( iter );
				break;
			}
		}
	}
}
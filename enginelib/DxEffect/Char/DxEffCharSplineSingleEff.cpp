#include "pch.h"

#include "../../DxEffect/Frame/DxEffectMan.h"

#include "../../DxEffect/Single/DxEffSingle.h"
#include "../Single/DxEffSinglePropGMan.h"
#include "../../../SigmaCore/File/SerialFile.h"

#include "../../DxTools/DxWeatherMan.h"
#include "../../G-Logic/GLPeriod.h"
#include "../../DxMeshs/SkinMesh/DxSkinPieceContainer.h"
#include "../../DxMeshs/SkinMesh/DxSkeletonMaintain.h"
#include "../../DxMeshs/SkinMesh/DxCharPart.h"

#include "../../DxTools/EDITMESHS.h"

#include "./DxEffCharSplineSingleEff.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern std::string	g_strPICK_BONE;
extern BOOL         g_bOBJECT100;
extern BOOL         g_bCHAR_EDIT_RUN;
extern BOOL         g_bCHAR_CHF_RENDER;
extern BOOL         g_bEFFECT_SPHERE_VISIBLE;

DWORD		DxEffCharSplineSingleEff::TYPEID			= EMEFFCHAR_SPLINE_SINGLE_EFF;
const DWORD	DxEffCharSplineSingleEff::VERSION			= 0x0100;
char		DxEffCharSplineSingleEff::NAME[MAX_PATH]	= "4.Spline_Single_Effect";

DxEffCharSplineSingleEff::DxEffCharSplineSingleEff(void) 
    : DxEffChar()
	, m_pd3dDevice(NULL)

    , m_rBoneTranMaintain(NULL)
	, m_nBoneIndex(-1)
    , m_pSkinMesh(NULL)

    , m_fSplineFullLength(1.f)
    , m_fSplineStartRate(1.f)
    , m_fSplineEndRate(1.f)
{
}

DxEffCharSplineSingleEff::~DxEffCharSplineSingleEff(void)
{
	CleanUp ();
}

DxEffChar* DxEffCharSplineSingleEff::CloneInstance ( LPDIRECT3DDEVICEQ pd3dDevice, DxCharPart* pCharPart, DxSkinPiece* pSkinPiece, DWORD dwMaterialNum )
{
	HRESULT hr;
	DxEffCharSplineSingleEff *pEffCharSingle = new DxEffCharSplineSingleEff;
	pEffCharSingle->SetFileName ( GetFileName() );
	pEffCharSingle->SetLinkObj ( pCharPart, pSkinPiece );
	pEffCharSingle->SetProperty ( &m_Property );

    pEffCharSingle->m_strSingleEffect   = m_strSingleEffect;
    pEffCharSingle->m_strBoneName       = m_strBoneName;
    pEffCharSingle->m_vecSplinePoint    = m_vecSplinePoint;
    pEffCharSingle->MakeRNS();

	hr = pEffCharSingle->Create ( pd3dDevice );
	if ( FAILED(hr) )
	{
		SAFE_DELETE(pEffCharSingle);
		return NULL;
	}

	return pEffCharSingle;
}

HRESULT DxEffCharSplineSingleEff::OneTimeSceneInit ()
{
    for ( DWORD i=0; i<m_vecSingleEffData.size(); ++i )
    {
        if ( m_vecSingleEffData[i].m_spSingleEff.get() )
            m_vecSingleEffData[i].m_spSingleEff->OneTimeSceneInit ();
    }

	return S_OK;
}

// DxEffSinglePropGMan를 ThreadLoading 하도록 바꾸었기 때문에 DxSingleGroup가 로딩안되었으면 로딩을 시도한다.
void DxEffCharSplineSingleEff::CreateDxEffSingleGroup()
{
	if ( m_vecSingleEffData.empty() )
		return;

	if ( m_sPropGroupRes.Update() )	
	{
		for ( DWORD i=0; i<m_dwEffNumber; ++i )
		{
			SPDXEFFSINGLEGROUP spSingleGroup = m_sPropGroupRes.get()->NEWEFFGROUP();
			if ( spSingleGroup.get() == NULL )
			{
				// Effect 만들어 질 수 없다고 알리자.
				std::string strErr = sc::string::format( "[%s] 의 DxEffCharSplineSingleEff : [%s] 가 만들어지지 않습니다.", m_strFileName.c_str(), m_strSingleEffect.c_str() );
				CDebugSet::ToLogFile( strErr );
				return;
			}

			SINGLEEFFDATA sData;
			sData.m_spSingleEff = spSingleGroup;
			sData.m_fPlayTime = static_cast<float>(i)/m_dwEffNumber;
			sData.m_fPlayTime *= (m_fSplineEndRate-m_fSplineStartRate);
			sData.m_fPlayTime += m_fSplineStartRate;
			m_vecSingleEffData.push_back( sData );

			//	Note : Eff Group Create Dev.
			spSingleGroup->Create ( m_pd3dDevice );
			spSingleGroup->m_fOut_Scale = m_fScale;				// 스케일
		}
	}
}

// DxEffSingleGroup 을 정리한다.
void DxEffCharSplineSingleEff::DeleteDxEffSingleGroup()
{
	for ( DWORD i=0; i<m_vecSingleEffData.size(); ++i )
	{
		if ( m_vecSingleEffData[i].m_spSingleEff.get() )
		{
			m_vecSingleEffData[i].m_spSingleEff->CleanUp();
			m_vecSingleEffData[i].m_spSingleEff.reset();
		}
	}
	m_vecSingleEffData.clear();
}

HRESULT DxEffCharSplineSingleEff::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;

	m_pd3dDevice = pd3dDevice;

	if ( m_pCharPart )
	{
		m_pSkinMesh = m_pCharPart->m_rSkinMesh;
	}
	else if ( m_pSkinPiece )
	{
		m_pSkinMesh = m_pSkinPiece->m_skinMeshRes.get();
	}

	// DxEffSingleGroup 을 정리한다.
	DeleteDxEffSingleGroup();

	if ( !m_strSingleEffect.empty() )
	{
		m_sPropGroupRes = DxEffSinglePropGMan::GetInstance().LoadEffectGProp ( m_strSingleEffect.c_str(), TRUE, FALSE );

		// DxEffSinglePropGMan를 ThreadLoading 하도록 바꾸었기 때문에 DxSingleGroup가 로딩안되었으면 로딩을 시도한다.
		CreateDxEffSingleGroup();
	}
	
	//if ( !m_pPropGroup )
 //   {
 //       if ( !m_strSingleEffect.empty() )
 //       {
 //           // Effect 가 없다고 알리자.
 //           std::string strErr = sc::string::format( "[%s] 의 DxEffCharSplineSingleEff : [%s] 가 존재하지 않습니다.", m_strFileName.c_str(), m_strSingleEffect.c_str() );
 //           CDebugSet::ToLogFile( strErr );
 //       }
 //       return S_OK;
 //   }

 //   m_vecSingleEffData.clear();
	//if ( m_pPropGroup )
	//{
 //       for ( DWORD i=0; i<m_dwEffNumber; ++i )
 //       {
 //           SPDXEFFSINGLEGROUP spSingleGroup = m_pPropGroup->NEWEFFGROUP();
 //           if ( spSingleGroup.get() == NULL )
 //           {
 //               // Effect 만들어 질 수 없다고 알리자.
 //               std::string strErr = sc::string::format( "[%s] 의 DxEffCharSplineSingleEff : [%s] 가 만들어지지 않습니다.", m_strFileName.c_str(), m_strSingleEffect.c_str() );
 //               CDebugSet::ToLogFile( strErr );
 //               return S_OK;
 //           }

 //           SINGLEEFFDATA sData;
 //           sData.m_spSingleEff = spSingleGroup;
 //           sData.m_fPlayTime = static_cast<float>(i)/m_dwEffNumber;
 //           sData.m_fPlayTime *= (m_fSplineEndRate-m_fSplineStartRate);
 //           sData.m_fPlayTime += m_fSplineStartRate;
 //           m_vecSingleEffData.push_back( sData );

 //           //	Note : Eff Group Create Dev.
	//	    spSingleGroup->Create ( pd3dDevice );
 //           spSingleGroup->m_fOut_Scale = m_fScale;				// 스케일

 //           spSingleGroup->InitDeviceObjects ( pd3dDevice );
 //       }
	//}

	return S_OK;
}

HRESULT DxEffCharSplineSingleEff::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;

    for ( DWORD i=0; i<m_vecSingleEffData.size(); ++i )
    {
        if ( m_vecSingleEffData[i].m_spSingleEff.get() )
            m_vecSingleEffData[i].m_spSingleEff->RestoreDeviceObjects ( pd3dDevice );
    }

	return S_OK;
}

HRESULT DxEffCharSplineSingleEff::InvalidateDeviceObjects ()
{
    for ( DWORD i=0; i<m_vecSingleEffData.size(); ++i )
    {
        if ( m_vecSingleEffData[i].m_spSingleEff.get() )
            m_vecSingleEffData[i].m_spSingleEff->InvalidateDeviceObjects ();
    }

	return S_OK;
}

HRESULT DxEffCharSplineSingleEff::DeleteDeviceObjects ()
{
    for ( DWORD i=0; i<m_vecSingleEffData.size(); ++i )
    {
        if ( m_vecSingleEffData[i].m_spSingleEff.get() )
            m_vecSingleEffData[i].m_spSingleEff->DeleteDeviceObjects ();
    }

	return S_OK;
}

HRESULT DxEffCharSplineSingleEff::FinalCleanup ()
{
    for ( DWORD i=0; i<m_vecSingleEffData.size(); ++i )
    {
        if ( m_vecSingleEffData[i].m_spSingleEff.get() )
        {
            m_vecSingleEffData[i].m_spSingleEff->FinalCleanup();
            m_vecSingleEffData[i].m_spSingleEff.reset();
        }
    }

	return S_OK;
}

HRESULT DxEffCharSplineSingleEff::FrameMove ( float fTime, float fElapsedTime )
{
	HRESULT hr = S_OK;

	m_fTime = fTime;

    float fElapsedTimeSpeed = fElapsedTime * m_fSpeed;

    // 시간이 너무 튀면 좀 줄여준다.
    if ( fElapsedTimeSpeed > 1.f )
        fElapsedTimeSpeed = 1.f;

	// DxEffSinglePropGMan를 ThreadLoading 하도록 바꾸었기 때문에 DxSingleGroup가 로딩안되었으면 로딩을 시도한다.
	CreateDxEffSingleGroup();

    for ( DWORD i=0; i<m_vecSingleEffData.size(); ++i )
    {
        m_vecSingleEffData[i].m_fPlayTime += fElapsedTimeSpeed;

        if ( m_vecSingleEffData[i].m_fPlayTime > m_fSplineEndRate )
        {
            m_vecSingleEffData[i].m_fPlayTime += m_fSplineStartRate - m_fSplineEndRate;
        }

        if ( m_vecSingleEffData[i].m_spSingleEff.get() )
	    {
		    m_vecSingleEffData[i].m_spSingleEff->FrameMove ( fTime, fElapsedTime );
	    }
    }

	return S_OK;
}

void	DxEffCharSplineSingleEff::OnlyRenderEff( LPDIRECT3DDEVICEQ pd3dDevice )
{
	D3DXMATRIX matTrans;
	D3DXVECTOR3 vPos(0.f,0.f,0.f);
	for ( DWORD i=0; i<m_vecSingleEffData.size(); ++i )
	{
		vPos = m_sSplinePoint.GetPosition( m_vecSingleEffData[i].m_fPlayTime );
		D3DXMatrixTranslation( &matTrans, vPos.x, vPos.y, vPos.z );

		if ( m_vecSingleEffData[i].m_spSingleEff.get() )
		{
			D3DXMatrixMultiply( &m_vecSingleEffData[i].m_spSingleEff->m_matWorld, &matTrans, &m_matRotateCombine );
			m_vecSingleEffData[i].m_spSingleEff->Render( pd3dDevice, D3DXVECTOR3(1.f,1.f,1.f), 1.f );
		}
	}
}

void	DxEffCharSplineSingleEff::RenderEff( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* pMatrix, const float fScale )
{
	PrecalculateBeforeRenderEff( pd3dDevice, pMatrix );
	OnlyRenderEff( pd3dDevice );
}

void DxEffCharSplineSingleEff::RenderSplineLine( LPDIRECT3DDEVICEQ pd3dDevice )
{
    D3DXMATRIX	matRotateCombine;
    D3DXMatrixIdentity( &matRotateCombine );

	if ( m_pCharPart )
	{
		if( !m_pCharPart->m_rMeshContainer->pSkinInfo )
        {
            // 무기일 경우
			matRotateCombine = m_pCharPart->GetStaticLinkBone();
        }
		else if ( m_rBoneTranMaintain )
        {
            // 손, 목
			matRotateCombine = m_rBoneTranMaintain->m_matCombined;
        }
	}
    else if ( m_rBoneTranMaintain )
    {
        // 손, 목
		matRotateCombine = m_rBoneTranMaintain->m_matCombined;
    }

    D3DXVECTOR3 vPos;
    D3DXVECTOR3 vFromPt;
	D3DXVECTOR3 vUp(0.f,1.f,0.f);
	for( DWORD i=0; i<m_vecSplinePoint.size(); ++i )
	{
		// 바라보는 위치
        D3DXVec3TransformCoord( &vPos, &m_vecSplinePoint[i], &matRotateCombine );
		EDITMESHS::RENDERSPHERE( pd3dDevice, vPos, 0.2f );
	}

	if( m_vecSplinePoint.size() >= 3 )
	{
		DWORD dwSize = static_cast<DWORD>( m_vecSplinePoint.size() );
		dwSize *= 10;

		// Spline - LookatPt
		D3DXVECTOR3 vPrev;
		D3DXVECTOR3 vCur;
		for( DWORD i=0; i<=dwSize; ++i )
		{
			if( i == 0 )
			{
				vPrev = m_sSplinePoint.GetPosition( 0.f );
                D3DXVec3TransformCoord( &vPrev, &vPrev, &matRotateCombine );
			}
			else
			{
                float fRate = static_cast<float>(i)/dwSize;
                vCur = m_sSplinePoint.GetPosition( fRate );
                D3DXVec3TransformCoord( &vCur, &vCur, &matRotateCombine );

                if ( fRate >= m_fSplineStartRate && fRate <= m_fSplineEndRate )
                {
				    EDITMESHS::RENDERLINE( pd3dDevice, vPrev, vCur, 0xffff0000 );
                }
				vPrev = vCur;
			}
		}

		EDITMESHS::RENDERLINE( pd3dDevice, vPrev, vCur );
	}
}

void DxEffCharSplineSingleEff::MakeRNS()
{
	// 초기화
	m_sSplinePoint.Init();

	// 3개 이상되야 동작되도록
	if( m_vecSplinePoint.size() <= 2 )
		return;

    // 맨뒤에것을 넣는다.
    m_sSplinePoint.AddNode( m_vecSplinePoint[m_vecSplinePoint.size()-1], 0.f );
    m_fSplineFullLength = 0.f;

	// AddNode 작업
    float fLength(1.f);
    D3DXVECTOR3 vPosPrev = m_vecSplinePoint[m_vecSplinePoint.size()-1];
	for( DWORD i=0; i<m_vecSplinePoint.size(); ++i )
	{
        fLength = GetSpline2PointLength( m_vecSplinePoint[i], vPosPrev );
        m_sSplinePoint.AddNode( m_vecSplinePoint[i], fLength );
        m_fSplineFullLength += fLength;
        //m_sSplinePoint.AddNode( m_vecSplinePoint[i], m_vecSplinePoint[i].m_fTime*fLength );

        if ( i==0 )
        {
            m_fSplineStartRate = fLength;
        }
	}

    // 맨 처음것을 넣는다.
    fLength = GetSpline2PointLength( m_vecSplinePoint[0], vPosPrev );
    m_sSplinePoint.AddNode( m_vecSplinePoint[0], fLength );
    m_fSplineFullLength += fLength;
    m_fSplineEndRate = m_fSplineFullLength;

    // 두번째 것을 넣는다.
    fLength = GetSpline2PointLength( m_vecSplinePoint[1], vPosPrev );
    m_sSplinePoint.AddNode( m_vecSplinePoint[1], fLength );
    m_fSplineFullLength += fLength;

	m_sSplinePoint.BuildSpline();

    m_fSplineStartRate /= m_fSplineFullLength;
    m_fSplineEndRate /= m_fSplineFullLength;
}

float DxEffCharSplineSingleEff::GetSpline2PointLength( const D3DXVECTOR3& vPos, D3DXVECTOR3& vPrevPos )
{
    D3DXVECTOR3 vDir = vPos - vPrevPos;
    vPrevPos = vPos;
	float fLength = D3DXVec3Length( &vDir );
	if( fLength < 1.f )
		fLength = 1.f;

    return fLength;
}

// Edit 에서 사용됨.
void DxEffCharSplineSingleEff::InsertSplinePoint( int nIndex )
{
    // 마지막에 추가함.
    if ( nIndex == -1 )
    {
        D3DXVECTOR3 vPos(0.f,0.f,0.f);
        m_vecSplinePoint.push_back( vPos );
    }

    VEC_VECTOR3 vecTemp;
    VEC_VECTOR3_ITER iter = m_vecSplinePoint.begin();
    for ( int i=0; iter!=m_vecSplinePoint.end(); ++iter, ++i )
    {
        vecTemp.push_back( m_vecSplinePoint[i] );

        // 중간에 추가한다.
        if ( i == nIndex )
        {
            vecTemp.push_back( m_vecSplinePoint[i] );
        }
    }

    m_vecSplinePoint = vecTemp;

    // Make Round Nonuniform Spline
    MakeRNS();
}

BOOL DxEffCharSplineSingleEff::DeleteSplinePoint( DWORD dwIndex )
{
    VEC_VECTOR3_ITER iter = m_vecSplinePoint.begin();
    for ( DWORD i=0; iter!=m_vecSplinePoint.end(); ++iter, ++i )
    {
        if ( i == dwIndex )
        {
            m_vecSplinePoint.erase( iter );

            // Make Round Nonuniform Spline
            MakeRNS();

            return TRUE;
        }
    }

    return FALSE;
}

DWORD DxEffCharSplineSingleEff::UpSplinePoint( DWORD dwIndex )
{
    if( dwIndex == 0 )	
        return dwIndex;

    D3DXVECTOR3 vPos;
	VEC_VECTOR3 vecTemp1;
    VEC_VECTOR3 vecTemp2;

    // 앞부분 백업
    {
        DWORD dwCount(0);
	    VEC_VECTOR3_ITER iter = m_vecSplinePoint.begin();
	    for( ; iter!=m_vecSplinePoint.end(); ++iter, ++dwCount )
	    {
		    if( dwCount==dwIndex-1 )
		    {
			    break;
		    }

            vecTemp1.push_back( (*iter) );
	    }
    }

    // 뒷부분 백업
    {
        DWORD dwCount(0);
        VEC_VECTOR3_ITER iter = m_vecSplinePoint.begin();
	    for( ; iter!=m_vecSplinePoint.end(); ++iter, ++dwCount )
	    {
		    if( dwCount > dwIndex )
		    {
			    vecTemp2.push_back( (*iter) );
		    }
	    }
    }

    // 중간부분 백업
    D3DXVECTOR3 vFirst = m_vecSplinePoint[ dwIndex ];
    D3DXVECTOR3 vSecond = m_vecSplinePoint[ dwIndex-1 ];

    // 모든데이터 정리 후 다시 셋팅함.
    {
        m_vecSplinePoint.clear();

        for ( DWORD i=0; i<vecTemp1.size(); ++i )
        {
            m_vecSplinePoint.push_back( vecTemp1[i] );
        }

        m_vecSplinePoint.push_back( vFirst );
        m_vecSplinePoint.push_back( vSecond );

        for ( DWORD i=0; i<vecTemp2.size(); ++i )
        {
            m_vecSplinePoint.push_back( vecTemp2[i] );
        }
    }

    // Make Round Nonuniform Spline
    MakeRNS();

    return dwIndex-1;
}

DWORD DxEffCharSplineSingleEff::DownSplinePoint( DWORD dwIndex )
{
    if( (dwIndex+1) >= m_vecSplinePoint.size() )	
        return dwIndex;

	D3DXVECTOR3 vPos;
	VEC_VECTOR3 vecTemp1;
    VEC_VECTOR3 vecTemp2;

    // 앞부분 백업
    {
        DWORD dwCount(0);
	    VEC_VECTOR3_ITER iter = m_vecSplinePoint.begin();
	    for( ; iter!=m_vecSplinePoint.end(); ++iter, ++dwCount )
	    {
		    if( dwCount==dwIndex )
		    {
			    break;
		    }

            vecTemp1.push_back( (*iter) );
	    }
    }

    // 뒷부분 백업
    {
        DWORD dwCount(0);
        VEC_VECTOR3_ITER iter = m_vecSplinePoint.begin();
	    for( ; iter!=m_vecSplinePoint.end(); ++iter, ++dwCount )
	    {
		    if( dwCount > (dwIndex+1) )
		    {
			    vecTemp2.push_back( (*iter) );
		    }
	    }
    }

    // 중간부분 백업
    D3DXVECTOR3 vFirst = m_vecSplinePoint[ dwIndex+1 ];
    D3DXVECTOR3 vSecond = m_vecSplinePoint[ dwIndex ];

    // 모든데이터 정리 후 다시 셋팅함.
    {
        m_vecSplinePoint.clear();

        for ( DWORD i=0; i<vecTemp1.size(); ++i )
        {
            m_vecSplinePoint.push_back( vecTemp1[i] );
        }

        m_vecSplinePoint.push_back( vFirst );
        m_vecSplinePoint.push_back( vSecond );

        for ( DWORD i=0; i<vecTemp2.size(); ++i )
        {
            m_vecSplinePoint.push_back( vecTemp2[i] );
        }
    }

    // Make Round Nonuniform Spline
    MakeRNS();

    return dwIndex+1;
}

HRESULT DxEffCharSplineSingleEff::LoadFile_0100 ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
	//	Note : 이팩트의 Property 를 저장.
	//
	SFile.ReadBuffer( &m_Property, sizeof(m_Property) );

    SFile >> m_strSingleEffect;
    SFile >> m_strBoneName;

    DWORD dwSize(0L);
    SFile >> dwSize;

    D3DXVECTOR3 vPos;
    m_vecSplinePoint.clear();
    for ( DWORD i=0; i<dwSize; ++i )
    {
        SFile >> vPos;
        m_vecSplinePoint.push_back( vPos );
    }

	return S_OK;
}

HRESULT	DxEffCharSplineSingleEff::LoadFile ( sc::BaseStream &SFile, LPDIRECT3DDEVICEQ pd3dDevice )
{
    CleanUp();

	HRESULT hr;
	DWORD dwVer, dwSize;

	SFile >> dwVer;
	SFile >> dwSize;

	switch ( dwVer )
	{
	case VERSION:	
        LoadFile_0100( SFile, pd3dDevice );	
        break;

	default:
		{
			//	Note : 버전이 틀릴 경우에는 파일에 쓰여진 DATA영역을 건더 띄는 작업을 진행.
			//
			SFile.SetOffSet ( SFile.GetfTell()+dwSize );

			return E_FAIL;
		}
		break;
	};

    // Make Round Nonuniform Spline
    MakeRNS();

	//	Note : Device 자원을 생성한다.
	//
	hr = Create ( pd3dDevice );
	if ( FAILED(hr) )	
        return hr;

	return S_OK;
}

HRESULT	DxEffCharSplineSingleEff::SaveFile ( sc::BaseStream &SFile )
{
	SFile << static_cast<DWORD>( TYPEID );
	SFile << static_cast<DWORD>( VERSION );

    SFile.BeginBlock();
    {
	    //	Note : 이팩트의 Property 를 저장.
	    SFile.WriteBuffer ( &m_Property, sizeof(m_Property) );

        SFile << m_strSingleEffect;
        SFile << m_strBoneName;

        SFile << static_cast<DWORD>( m_vecSplinePoint.size() );
        for ( DWORD i=0; i<m_vecSplinePoint.size(); ++i )
        {
            SFile << m_vecSplinePoint[i];
        }
    }
    SFile.EndBlock();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// Material System 에 현재 효과들을 붙이기 위한 작업.
void DxEffCharSplineSingleEff::PrecalculateBeforeRenderEff( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX* pMatrix )
{
	HRESULT hr = S_OK;
	D3DXVECTOR3 vNormalUp;
	D3DXVECTOR3 vNormalDown;

	if ( g_bCHAR_EDIT_RUN && !g_bCHAR_CHF_RENDER )
	{
		if ( m_rSkeleton && !g_bOBJECT100 )	
		{
			m_rSkeleton->CheckSphere( pd3dDevice, g_strPICK_BONE.c_str() );
		}

		// 캐릭터 툴은 매번 체크하게 된다.
		m_rBoneTranMaintain = m_rSkeleton->FindBone( m_strBoneName );
		if ( m_rBoneTranMaintain )
		{
			m_nBoneIndex = m_rBoneTranMaintain->GetIndex();
		}

		if ( !g_bOBJECT100 )	
		{
			RenderSplineLine( pd3dDevice );
		}
	}
	else
	{
		// 게임상에서는 한번 정해지면 바뀌지 않는다.
		if ( m_nBoneIndex == -1 )
		{
			m_rBoneTranMaintain = m_rSkeleton->FindBone( m_strBoneName );
			if ( m_rBoneTranMaintain )
			{
				m_nBoneIndex = m_rBoneTranMaintain->GetIndex();
			}
		}
		else
		{
			m_rBoneTranMaintain = m_rSkeleton->FindBone_Index( m_nBoneIndex );
		}
	}

	{
		D3DXMatrixIdentity( &m_matRotateCombine );

		if ( m_pCharPart )
		{
			if( !m_pCharPart->m_rMeshContainer->pSkinInfo )
			{
				// 무기일 경우
				m_matRotateCombine = m_pCharPart->GetStaticLinkBone();
			}
			else if ( m_rBoneTranMaintain )
			{
				// 손, 목
				m_matRotateCombine = m_rBoneTranMaintain->m_matCombined;
			}
		}
		else if ( m_rBoneTranMaintain )
		{
			// 손, 목
			m_matRotateCombine = m_rBoneTranMaintain->m_matCombined;
		}
		else if ( pMatrix )
		{
			m_matRotateCombine = *pMatrix;
		}
	}
}

void DxEffCharSplineSingleEff::RenderEffForMaterialSystem( LPDIRECT3DDEVICEQ pd3dDevice, 
														  const D3DXMATRIX* pMatrix, 
														  const float fScale )
{
	OnlyRenderEff( pd3dDevice );
}
#include "pch.h"

#include "../../DxTools/DxViewPort.h"
#include "../../DxTools/TextureManager.h"
#include "../../DxEffect/ShaderConstant.h"
#include "../../DxTools/DxDynamicVB.h"
#include "../../DxTools/EditMeshs.h"
#include "../../DxEffect/Frame/DxEffectMan.h"
#include "../../DxEffect/Single/DxEffSingleMan.h"
#include "../../../SigmaCore/File/SerialFile.h"

#include "./DxEffectParticleSys.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//	Note	:	����׿�,
extern BOOL		g_bEFFtoTEX;
BOOL			g_bDebugParticleRange = FALSE;


HRESULT DxEffectParticleSys::Render ( LPDIRECT3DDEVICEQ pd3dDevice, D3DXMATRIX &matComb, float fAlpha )
{
	HRESULT hr = S_OK;

	SingleForThread* pSingleForThread = m_spSingleForThread.get();
	ParticlesForThread* pParticleForThread = MIN_BOOST::polymorphic_downcast<ParticlesForThread*>( pSingleForThread );

	// Note : ���� �ɼǿ� ���� ������ ��� ����� ���־�� �Ѵ�.
	if( pParticleForThread->GetEmitterRandomPlay() )
	{
		D3DXMatrixMultiply( &matComb, &pParticleForThread->GetEmitterMatrixRandomR(), &matComb );
	}


	m_spSingleForThread->SetRenderCullingTRUE();
	pParticleForThread->SetCombine( matComb );
	pParticleForThread->SetAlpha( fAlpha );


	// ���� Tex�� �������� �� ��.
	if( (m_spEmitterSetting->GetFlag()&USEGROUNDTEX) && (!m_textureResColl.Update()) )	// �� �ѹ��� üũ�Ѵ�.
	{
		boost::shared_ptr<DxLandMan> spLandMan = DxEffSingleMan::GetInstance().GetLandMan();
		if ( spLandMan )
		{
			BOOL		bColl;
			D3DXVECTOR3	vColl;
			LPDXFRAME	pDxFrame = NULL;
			const char* szName = NULL;

			D3DXVECTOR3 vStart(matComb._41,matComb._42,matComb._43);
			D3DXVECTOR3 vEnd=vStart;
			vStart.y += 0.01f;
			vEnd.y -= 1000.f;
			spLandMan->IsCollisionEX ( vStart, vEnd, vColl, bColl, pDxFrame, szName );
			if ( bColl && szName )
			{
				std::string m_szCollTex = szName;

                m_textureResColl = TextureManager::GetInstance().LoadTexture(
                    szName,
                    false,
                    TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
                    0,
                    D3DFMT_UNKNOWN,
                    TEXTURE_RESIZE_NONE,
                    TEXTURE_EFFECT,
                    true,
					FALSE );
			}
		}
	}

	D3DXMATRIX	matView, matProj, matWVP;
	D3DXMATRIX	matEmitter, matWorld, matRotate, matRotateL, matScale, matTrans, matMesh, matParent, matChild, matIdentity;
	D3DXVECTOR3	lookAt;
	D3DXVECTOR3	vDir;

	// const �� �̰����� ��ƼŬ ������ ���� �� �� ���� �Ѵ�.
	const PARTICLE* pParticleNode(NULL);

	//D3DXVECTOR3	vPos_NowPrev;

	//	Note : ���� ���� �ڽ� ��Ÿ���� �� !!
	//
	if ( g_bDebugParticleRange && !g_bEFFtoTEX )
	{
		if ( m_spEmitterSetting->GetFlag() & USERANGE )
		{
			D3DXVECTOR3	vMax, vMin;
			vMax = D3DXVECTOR3 ( m_spEmitterSetting->GetRange().x*0.5f, m_spEmitterSetting->GetRange().y*0.5f, m_spEmitterSetting->GetRange().z*0.5f );
			vMin = D3DXVECTOR3 ( -m_spEmitterSetting->GetRange().x*0.5f, -m_spEmitterSetting->GetRange().y*0.5f, -m_spEmitterSetting->GetRange().z*0.5f );

			D3DXMatrixIdentity ( &matWorld );
			matWorld._41 = m_matLocal._41;
			matWorld._42 = m_matLocal._42;
			matWorld._43 = m_matLocal._43;
			D3DXVec3TransformCoord ( &vMax, &vMax, &matWorld );
			D3DXVec3TransformCoord ( &vMin, &vMin, &matWorld );

			EDITMESHS::RENDERAABB ( pd3dDevice, vMax, vMin );
		}
	}

	lookAt = D3DXVECTOR3(0.f,0.f,0.f);

	//	Note : ��Ʈ���� ����
	//SetMatrix ( matWorld, matComb );

	if ( !(m_dwRunFlag&EFF_PLY_PLAY) )		goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_END )			goto _RETURN;
	if ( m_dwRunFlag&EFF_PLY_RENDPASS )		goto _RETURN;

	if( m_spEmitterSetting->GetFlag() & USENEWEFF )
	{
		pParticleNode = pParticleForThread->GetParticleHead();
		while ( pParticleNode )
		{
			if( pParticleNode->m_spSingleGroup )
			{
				if( pParticleNode->IsSingleGroupPlay() )
				{
					pParticleNode->m_spSingleGroup->FrameMove( 0.f, m_fElapsedTime );
					pParticleNode->m_spSingleGroup->m_matWorld._41 = pParticleNode->m_vLocation.x;
					pParticleNode->m_spSingleGroup->m_matWorld._42 = pParticleNode->m_vLocation.y;
					pParticleNode->m_spSingleGroup->m_matWorld._43 = pParticleNode->m_vLocation.z;
					pParticleNode->m_spSingleGroup->Render( pd3dDevice, D3DXVECTOR3(1.f,1.f,1.f), 1.f );
				}
			}
			pParticleNode = pParticleNode->m_pNext;
		}
	}

	////	Note : ��ü�� ���� ���� ���� ��ƼŬ�� �ణ�� �̵� ������
	////
	//if ( (m_matPrevComb._41==0.f) && (m_matPrevComb._42==0.f) && (m_matPrevComb._43==0.f) )
	//{
	//	vPos_NowPrev = D3DXVECTOR3 ( 0.f, 0.f, 0.f );
	//}
	//else
	//{
	//	vPos_NowPrev = D3DXVECTOR3 ( matComb._41-m_matPrevComb._41, matComb._42-m_matPrevComb._42, matComb._43-m_matPrevComb._43 );
	//}

	//	Note : ���ο� ��ƼŬ�� ��� ������ ����.
	//
//	NewParticleCreate( pd3dDevice, matComb, vPos_NowPrev );

	//if ( m_uParticlesAlive==0 )
	if ( !pParticleForThread->IsParticlesAlive() )
        goto _RETURN;

    // V628 It's possible that the line was commented out improperly, thus altering the program's operation logics.
	if ( m_spEmitterSetting->GetFlag() & USEMESH ) 
    {
	    if ( !m_SimMeshRes.Update() )
		    goto _RETURN;
    }

//	// Note : OPTMSingleParticle�� ����ϱ� ���� �ʱ� �۾�.
//	OPTMSingleParticle::DATA* pData = OPTMSingleParticle::GetInstance().GetData();

	m_pSavedRenderStats->Capture();
	m_pDrawRenderStats->Apply();

	DWORD dwFogColor(0L);
	pd3dDevice->GetRenderState ( D3DRS_FOGCOLOR, &dwFogColor );
	
	//	Note : ������ ���� �϶�
	//
	if ( m_spEmitterSetting->GetFlag()&USESEQUENCE )
	{
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,	D3DTADDRESS_WRAP );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV,	D3DTADDRESS_WRAP );
	}
	else
	{
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_BORDERCOLOR, 0x00000000 );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,	D3DTADDRESS_BORDER );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV,	D3DTADDRESS_BORDER );
	}

	//	Note : ���� ��� ����
	//
	switch ( m_spParticleSetting->GetBlend() )
	{
	case 0 :	// Src+Dest (ADD)
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_BLENDOP,				D3DBLENDOP_ADD );
		break;
	case 1:		// Src-Dest (SUBTRACT)
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_BLENDOP,				D3DBLENDOP_SUBTRACT );
		break;
	case 2:		// Dest-Src (REVSUBTRACT)
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_BLENDOP,				D3DBLENDOP_REVSUBTRACT );
		break;
	case 3:		// Color MIN
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_BLENDOP,				D3DBLENDOP_MIN );
		break;
	case 4:		// Color MAX
		pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,			0x00000000 );
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_ONE );
		pd3dDevice->SetRenderState ( D3DRS_BLENDOP,				D3DBLENDOP_MAX );
		break;
	case 5:		// Src+Dest (ALPHA)
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		FALSE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
		pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );
		pd3dDevice->SetRenderState ( D3DRS_BLENDOP,				D3DBLENDOP_ADD );
		break;
	case 6:		// Src (MY)
		pd3dDevice->SetRenderState ( D3DRS_ZWRITEENABLE,		TRUE );
		pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	FALSE );
		break;
	}

	//	Note : ���� ���� ����
	//
	switch ( m_spParticleSetting->GetPower() )
	{
	case 0 :
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE );
		break;
	case 1:
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE2X );
		break;
	case 2:
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP,	D3DTOP_MODULATE4X );
		break;
	}

	// Mesh (Normal(Use/No))�� Particle �� ��� Ʋ���͵� ����.
	if ( m_spEmitterSetting->GetFlag() & USEMESH )
	{
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSU,	D3DTADDRESS_WRAP );
		pd3dDevice->SetSamplerState ( 0, D3DSAMP_ADDRESSV,	D3DTADDRESS_WRAP );

		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );

		if( m_spEmitterSetting->GetFlag()&USENORMAL2 )
		{
			pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			TRUE );
		}
		else
		{
			pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
			pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2,	D3DTA_TFACTOR );
			pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2,	D3DTA_TFACTOR );
		}
	}
	else
	{
		pd3dDevice->SetRenderState ( D3DRS_LIGHTING,			FALSE );
		pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE );
		
		if ( m_spEmitterSetting->GetFlag() & USEDIRECTION )
		{
			pd3dDevice->SetRenderState ( D3DRS_CULLMODE,	D3DCULL_NONE );	// ����. 
		}
	}

	matWorld = pParticleForThread->GetMatWorld();

	// [shhan][2015.10.29] ĳ���Ͱ� ������ �̵��� ��� Item �� �ٿ��� Effect �� ����� �и��Ǵ� ������ �־ �����۾���. 
	//						rm #2050 - ������ ���� �̵��� �� �帣�� ����Ʈ ��ġ ���� (w_as_blood_tiger_weapon_01.abl)
	//
	// ���� ����	(�����ǥ)	//	�����(ĳ��), ��(����)	, �̵��� �ص� ���� �׻���, ������ ȸ���� ������ ����
	if ( (m_spEmitterSetting->GetFlag() & USEPARENTMOVE) &&
		!( m_spEmitterSetting->GetFlag() & USEGOTOCENTER ) )			
	{
		matWorld._41 = matComb._41;
		matWorld._42 = matComb._42;
		matWorld._43 = matComb._43;
		matWorld._44 = matComb._44;
	}

	pd3dDevice->SetTransform ( D3DTS_WORLD, &matWorld );

	//m_matWorld = matWorld;

	//	Note : �޽� �Ѹ� ���ΰ�, �ƴϸ� ��ƼŬ�� �Ѹ� ���ΰ� ?
	//
	if( m_spEmitterSetting->GetFlag() & USEMESH )
	{
		pParticleNode = pParticleForThread->GetParticleHead();
		while ( pParticleNode )
		{
			float fSize = pParticleNode->m_fSize*0.2f;
			
			if ( m_spEmitterSetting->GetFlag()&USEDIRECTION )	// ������ �ִ� ��ü�� (����)�� ������ ��Ʈ������ �������.
			{

				//////////////////////////////////////////////////////////////////////////
				// [shhan][2014.04.10]
				// �̰� �� �̻��ϴ�.
				// �� ������ �־�����~? ������ �� �ȵǾ �ٲپ���.
				// surachun_fire.egp �� ȭ�� Mesh ���� �̻��ϰ� �ȴ�.
				//matTrans = DxBillboardLookAtDir ( &pParticleNode->m_vVelocity, &pParticleNode->m_vPrevLocation );
	
				DxBillboardLookAtDir ( matTrans, &pParticleNode->m_vLocation, &pParticleNode->m_vPrevLocation );
			}
			else
			{
				matTrans = pParticleNode->m_matRotate;
			}
			D3DXMatrixMultiply ( &matMesh, &matTrans, &matWorld );	// ȸ��

			D3DXMatrixTranslation ( &matTrans, pParticleNode->m_vLocation.x, pParticleNode->m_vLocation.y, pParticleNode->m_vLocation.z );
			D3DXMatrixMultiply ( &matMesh, &matMesh, &matTrans );	// �̵�	
			D3DXMatrixIdentity ( &matTrans );

			if ( fSize < 0.00001f )
			{
				pParticleNode = pParticleNode->m_pNext;
				continue;
			}

			matTrans._44 /= fSize;
			D3DXMatrixMultiply ( &matMesh, &matTrans, &matMesh );	// ������
			pd3dDevice->SetTransform ( D3DTS_WORLD, &matMesh );
			//m_matWorld = matMesh;


			D3DMATERIALQ	sMaterial;
			sMaterial.Diffuse.r = 1.f;
			sMaterial.Diffuse.g = 1.f;
			sMaterial.Diffuse.b = 1.f;
			sMaterial.Diffuse.a = 1.f;

			sMaterial.Ambient.r = 1.f;
			sMaterial.Ambient.g = 1.f;
			sMaterial.Ambient.b = 1.f;
			sMaterial.Ambient.a = 1.f;

			sMaterial.Specular.r = 1.f;
			sMaterial.Specular.g = 1.f;
			sMaterial.Specular.b = 1.f;
			sMaterial.Specular.a = 1.f;

			sMaterial.Emissive.r = 0.f;
			sMaterial.Emissive.g = 0.f;
			sMaterial.Emissive.b = 0.f;
			sMaterial.Emissive.a = 0.f;

			sMaterial.Power = 1.f;

			if( m_spEmitterSetting->GetFlag()&USENORMAL2 )
			{
				sMaterial.Diffuse.r = pParticleNode->m_cColor.x;
				sMaterial.Diffuse.g = pParticleNode->m_cColor.y;
				sMaterial.Diffuse.b = pParticleNode->m_cColor.z;
				sMaterial.Diffuse.a = pParticleNode->m_cColor.w;

				sMaterial.Ambient.r = pParticleNode->m_cColor.x;
				sMaterial.Ambient.g = pParticleNode->m_cColor.y;
				sMaterial.Ambient.b = pParticleNode->m_cColor.z;
				sMaterial.Ambient.a = pParticleNode->m_cColor.w;
			}
			else
			{
				D3DCOLOR	dwColor = ((DWORD)(pParticleNode->m_cColor.w*255.f*fAlpha)<<24) +
										((DWORD)(pParticleNode->m_cColor.x*255.f)<<16) +
										((DWORD)(pParticleNode->m_cColor.y*255.f)<<8) +
										(DWORD)(pParticleNode->m_cColor.z*255.f);
				pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR,	dwColor );
			}

			if ( m_spEmitterSetting->GetFlag() & USESEQUENCE )
			{
				D3DXMATRIX matTexture;
				D3DXMatrixIdentity( &matTexture );
				matTexture._11 /= m_spParticleSetting->GetCol();
				matTexture._22 /= m_spParticleSetting->GetRow();
				matTexture._31 = static_cast<float>(pParticleNode->m_nNowSprite)/m_spParticleSetting->GetCol();
				matTexture._32 = static_cast<float>(pParticleNode->m_nNowSprite/m_spParticleSetting->GetCol())/m_spParticleSetting->GetRow();
				pd3dDevice->SetTransform( D3DTS_TEXTURE0, &matTexture );

				pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3 );
			}

			if( m_textureResColl.GetTexture() )
			{
                m_SimMeshRes.get()->Render( pd3dDevice, m_textureResColl.GetTexture(), &sMaterial );
			}
			else if ( m_spEmitterSetting->GetFlag()&USETEXTURE )
			{
				if ( m_textureRes.GetTexture() )
				{
					m_SimMeshRes.get()->Render( pd3dDevice, m_textureRes.GetTexture(), &sMaterial );
				}
			}
			else
			{
                m_SimMeshRes.get()->Render( pd3dDevice, NULL, &sMaterial );
			}

			if ( m_spEmitterSetting->GetFlag() & USESEQUENCE )
			{
				pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
			}

			pParticleNode = pParticleNode->m_pNext;
		}
	}
	else
	{
		if( !m_textureRes.GetTexture() )
		{
			m_pSavedRenderStats->Apply();
			goto _RETURN;
		}

		pd3dDevice->SetTexture ( 0, m_textureRes.GetTexture() );

		int nDrawPoints = pParticleForThread->GetNumDrawPoints();
		if ( nDrawPoints > 0 )
		{
			DWORD dwVertexSizeFULL = nDrawPoints*sizeof(VERTEXCOLORTEX)*4;
            if( DxDynamicVB::g_sVB_I_24BYTE.IsValidSize( dwVertexSizeFULL ) )
            {
                UINT uiStartVertex = DxDynamicVB::g_sVB_I_24BYTE.CopyToVB(
		            pParticleForThread->GetUsedVertices(),
		            dwVertexSizeFULL,
		            nDrawPoints * 4 );

			    pd3dDevice->SetFVF ( VERTEXCOLORTEX::FVF );
			    pd3dDevice->SetStreamSource ( 0, DxDynamicVB::g_sVB_I_24BYTE.pVB, 0, sizeof(VERTEXCOLORTEX) );
			    pd3dDevice->SetIndices ( DxDynamicVB::g_pIBuffer );

				pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, uiStartVertex, 0, nDrawPoints*4, 0, nDrawPoints*2 );
            }
		}
	}

	pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR, dwFogColor );

	m_pSavedRenderStats->Apply();

_RETURN:

	//	Note : ������ ��ġ�� ��� �Ѵ�.
	//m_matPrevComb = matComb;

	//	Note : ����, �ڽ� ���.
	//
	D3DXMatrixTranslation ( &matTrans, pParticleForThread->GetEmitterGLocation().x, pParticleForThread->GetEmitterGLocation().y, pParticleForThread->GetEmitterGLocation().z );
	D3DXMatrixMultiply ( &matWorld, &matTrans, &pParticleForThread->GetMatWorld() );
	if ( m_pChild )		m_pChild->Render ( pd3dDevice, matWorld, fAlpha );
	if ( m_pSibling )	m_pSibling->Render ( pd3dDevice, matComb, fAlpha );

	D3DXMatrixIdentity ( &matIdentity );
	pd3dDevice->SetTransform ( D3DTS_WORLD, &matIdentity );

	return hr;
}
#include "pch.h"

#include "../../DxLand/DxPiece/DxPieceManager.h"
#include "../../DxEffect/Mesh/DxStaticMeshCollBase.h"
#include "../../DxTools/Light/NSShadowLight.h"
#include "../../DxTools/Light/DxShadowLight.h"
#include "../../DxTools/Light/DxLightMan.h"
#include "../../DxTools/DxVertexFVF.h"
#include "../../DxTools/Collision.h"

#include "NSLightMapTools.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace NSLightMapTools
{
	BOOL g_bRadiosityMode(FALSE);

	// 윗단계에서 IntersectTriangle_GetUV 값이 FALSE 인 녀석들만 검사한다.
	BOOL GetNearUV( D3DXVECTOR2 vTex[], const D3DXVECTOR2& vUV, float& u, float& v, DWORD& dwMainIndex, float& fLength )
	{
		D3DXVECTOR3 vUV_Orig( vUV.x, vUV.y, 1.f);
		D3DXVECTOR3 vdir( 0.f, 0.f, -10.f);

		D3DXVECTOR3 vUV_Array_NEW[3][3];
		vUV_Array_NEW[0][0].x = vTex[0].x;
		vUV_Array_NEW[0][0].y = vTex[0].y;
		vUV_Array_NEW[0][0].z = 0.f;
		vUV_Array_NEW[0][1].x = vTex[1].x;
		vUV_Array_NEW[0][1].y = vTex[1].y;
		vUV_Array_NEW[0][1].z = 0.f;
		vUV_Array_NEW[0][2].x = vTex[2].x;
		vUV_Array_NEW[0][2].y = vTex[2].y;
		vUV_Array_NEW[0][2].z = 0.f;

		vUV_Array_NEW[1][0].x = vTex[1].x;
		vUV_Array_NEW[1][0].y = vTex[1].y;
		vUV_Array_NEW[1][0].z = 0.f;
		vUV_Array_NEW[1][1].x = vTex[2].x;
		vUV_Array_NEW[1][1].y = vTex[2].y;
		vUV_Array_NEW[1][1].z = 0.f;
		vUV_Array_NEW[1][2].x = vTex[0].x;
		vUV_Array_NEW[1][2].y = vTex[0].y;
		vUV_Array_NEW[1][2].z = 0.f;

		vUV_Array_NEW[2][0].x = vTex[2].x;
		vUV_Array_NEW[2][0].y = vTex[2].y;
		vUV_Array_NEW[2][0].z = 0.f;
		vUV_Array_NEW[2][1].x = vTex[0].x;
		vUV_Array_NEW[2][1].y = vTex[0].y;
		vUV_Array_NEW[2][1].z = 0.f;
		vUV_Array_NEW[2][2].x = vTex[1].x;
		vUV_Array_NEW[2][2].y = vTex[1].y;
		vUV_Array_NEW[2][2].z = 0.f;

		D3DXVECTOR3 vDir[2];
		D3DXVECTOR3 vOut3;
		D3DXVECTOR2 vLengthDir;

		// 주변 비슷한 컬러를 가져올 수 있도록 변경됨.
		for ( DWORD i=0; i<3; ++i )
		{
			COLLISION::IntersectTriangle_GetUV( vUV_Orig,
				vdir,
				vUV_Array_NEW[i][0],
				vUV_Array_NEW[i][1],
				vUV_Array_NEW[i][2],
				&u,
				&v );

			if ( (u >= 0.0f) && (v >= 0.0f) )
			{
				dwMainIndex = i;
				float fNewU = u / (u+v);
				float fNewV = v / (u+v);

				// 이동 거리의 절반.
				u = fNewU + (fNewU-u) * 0.5f;
				v = fNewV + (fNewV-v) * 0.5f;

				vDir[0] = vUV_Array_NEW[i][1] - vUV_Array_NEW[i][0];
				vDir[1] = vUV_Array_NEW[i][2] - vUV_Array_NEW[i][0];

				vOut3 = vUV_Array_NEW[i][0];
				vOut3 += vDir[0]*u;
				vOut3 += vDir[1]*v;

				vLengthDir.x = vUV.x - vOut3.x;
				vLengthDir.y = vUV.y - vOut3.y;
				fLength = D3DXVec2Length( &vLengthDir );

				return TRUE;
			}
			else if ( (u <= 0.0f) && (v <= 0.0f) )
			{
				dwMainIndex = i;
				//float fNewU = 0.f;
				//float fNewV = 0.f;

				// 이동 거리의 절반.
				u = -u * 0.5f;
				v = -v * 0.5f;

				vOut3 = vUV_Array_NEW[i][0];

				vLengthDir.x = vUV.x - vOut3.x;
				vLengthDir.y = vUV.y - vOut3.y;
				fLength = D3DXVec2Length( &vLengthDir );

				return TRUE;
			}
		}			

		return FALSE;
	}

    BOOL IsInTriangle( D3DXVECTOR2 vTex[], const D3DXMATRIX& matRotate, const D3DXVECTOR2& vUV, float& fDistance, float& u, float& v )
    {
		D3DXVECTOR3 vUV_Orig( vUV.x, vUV.y, 1.f);
		D3DXVECTOR3 vdir( 0.f, 0.f, -10.f);

		D3DXVECTOR3 vUV_Array_NEW[3];
		for ( DWORD i=0; i<3; ++i )
		{
			vUV_Array_NEW[i].x = vTex[i].x;
			vUV_Array_NEW[i].y = vTex[i].y;
			vUV_Array_NEW[i].z = 0.f;
		}

		// 주변 비슷한 컬러를 가져올 수 있도록 변경됨.
		COLLISION::IntersectTriangle_GetUV( vUV_Orig,
			vdir,
			vUV_Array_NEW[0],
			vUV_Array_NEW[1],
			vUV_Array_NEW[2],
			&u,
			&v );

		if ( (u >= 0.0f) && (v >= 0.0f) && ((u + v) <= 1.f) )
		{
			return TRUE;	
		}

		return FALSE;
    }

	BOOL FindNearTriangle( const std::vector<D3DXVECTOR2>& vecUV, 
							const D3DXVECTOR2& vUV, 
							VEC_DWORD& vecFaceNUM, 
							DWORD& dwFaceOut, 
							float& u, 
							float& v, 
							DWORD& dwMainIndex )
	{
		if ( vecFaceNUM.empty() )
			return FALSE;

		// 딱맞는것이 없다면 첫 삼각형으로 하기 위함.
		{
			u = 0.f;
			v = 0.f;
			dwMainIndex = 0;
			dwFaceOut = vecFaceNUM[0];
		}

		DWORD dwFace = static_cast<DWORD> (vecUV.size());
		dwFace /= 3;

		u = 0.f;
		v = 0.f;
		dwMainIndex = 0;
		int nFaceIndex(0);
		float fShortDistance(FLT_MAX);
		float fDistance(0.f);
		D3DXVECTOR2 vTex[3];
		D3DXMATRIX matRotate;
		float fTempU, fTempV;
		D3DXMatrixRotationZ( &matRotate, D3DX_PI*0.5f );
		for ( DWORD i=0; i<vecFaceNUM.size(); ++i )
		{
			vTex[0] = vecUV[vecFaceNUM[i]*3+0];
			vTex[1] = vecUV[vecFaceNUM[i]*3+1];
			vTex[2] = vecUV[vecFaceNUM[i]*3+2];

			// 문제 있음.
			if ( vTex[0] == vTex[1] || vTex[0] == vTex[2] || vTex[1] == vTex[2] )
			{
				continue;
			}

			// 3면이 안쪽에 있다면 그 점은 현재 삼각형에 있는 것이다.
			if ( IsInTriangle( &vTex[0], matRotate, vUV, fDistance, fTempU, fTempV ) )
			{
				u = fTempU;
				v = fTempV;
				dwMainIndex = 0;
				dwFaceOut = vecFaceNUM[i];
				return TRUE;
			}
			else
			{
				// 가장 근접한 것을 구해서 그 삼각형안에 넣도록 한다.
				DWORD dwTempIndex;
				if ( GetNearUV( &vTex[0], vUV, fTempU, fTempV, dwTempIndex, fDistance ) )
				{
					if ( fDistance < fShortDistance )
					{
						u = fTempU;
						v = fTempV;
						dwMainIndex = dwTempIndex;
						dwFaceOut = vecFaceNUM[i];
						fShortDistance = fDistance;
					}
				}
			}
		}

		return TRUE;
	}

    void CalcPositionNormal( D3DXVECTOR3& vPos_Result, 
                            D3DXVECTOR3& vNor_Result, 
                            const D3DXVECTOR2 vUV_Array[], 
                            const D3DXVECTOR3 vPos_Array[], 
                            const D3DXVECTOR3 vNor_Array[],
                            const D3DXVECTOR2& vUV, 
                            DWORD dwTexelSizeX, 
                            DWORD dwTexelSizeY,
							int nTexelDetail )
    {
		D3DXVECTOR3 vUV_Orig( vUV.x, vUV.y, 1.f);
		D3DXVECTOR3 vdir( 0.f, 0.f, -10.f);

		D3DXVECTOR3 vUV_Array_NEW[3];
		for ( DWORD i=0; i<3; ++i )
		{
			vUV_Array_NEW[i].x = vUV_Array[i].x;
			vUV_Array_NEW[i].y = vUV_Array[i].y;
			vUV_Array_NEW[i].z = 0.f;
		}

		// 주변 비슷한 컬러를 가져올 수 있도록 변경됨.
		float u, v;
		COLLISION::IntersectTriangle_GetUV( vUV_Orig,
			vdir,
			vUV_Array_NEW[0],
			vUV_Array_NEW[1],
			vUV_Array_NEW[2],
			&u,
			&v );

		D3DXVECTOR3 vPos_Dir[2];
		vPos_Dir[0] = vPos_Array[1] - vPos_Array[0];
		vPos_Dir[1] = vPos_Array[2] - vPos_Array[0];
		vPos_Result = vPos_Array[0] + vPos_Dir[0]*( u ) + vPos_Dir[1]*( v );

		D3DXVECTOR3 vNor_Dir[2];
		vNor_Dir[0] = vNor_Array[1] - vNor_Array[0];
		vNor_Dir[1] = vNor_Array[2] - vNor_Array[0];
		vNor_Result = vNor_Array[0] + vNor_Dir[0]*( u ) + vNor_Dir[1]*( v );

		D3DXVec3Normalize( &vNor_Result, &vNor_Result );
    }


	float GetDotCollObject( LPDIRECT3DDEVICEQ pd3dDevice, 
		const D3DXVECTOR3& vPos, 
		const D3DXVECTOR3& vNor, 
		const D3DLIGHTQ& sLight,
		float fAttEndLineValueForSpot )
	{
		float fDot(0.f);
		if( sLight.Type == D3DLIGHT_DIRECTIONAL )
		{
			// Note : Direction Light 빛 세기.~!!
			D3DXVECTOR3 vDir = sLight.Direction;
			D3DXVec3Normalize( &vDir, &vDir );
			vDir = -vDir;

			return D3DXVec3Dot( &vNor, &vDir );
		}
		else if( sLight.Type == D3DLIGHT_POINT )
		{
			// Note : 일단 거리를 체크한다.
			D3DXVECTOR3 vDir = vPos-sLight.Position;
			float fLength = D3DXVec3Length( &vDir );

			// Note : 포인트 Light에 셋팅된 거리보다 실제 거리가 크면 계산하지 않는다.
			//if( fLength > fRange120 )	//sLight.Range )
			if( fLength > sLight.Range )
			{
				return -1.f;
			}

			// Note : Direction Light 빛 세기.~!!
			D3DXVec3Normalize( &vDir, &vDir );
			vDir = -vDir;

			fDot = D3DXVec3Dot( &vNor, &vDir );
			if( fDot < 0.f )
				return -1.f;

			// Point Light 쪽 감쇠
			{
				// Note : 일단 거리를 체크한다.
				D3DXVECTOR3 vDir = vPos-sLight.Position;
				float fLength = D3DXVec3Length( &vDir );

				// 라이트맵용 감쇠를 적용한다.
				if ( fLength > sLight.Range )
					return -1.f;

				// 선형 감쇠, 2차 감쇠, 지수 감쇠 적용
				FLOAT fAttenuationSub = sLight.Attenuation0;
				fAttenuationSub += sLight.Attenuation1 * fLength;
				fAttenuationSub += sLight.Attenuation2 * fLength * fLength;

				// 최종 감쇠값
				FLOAT fAttenuation = 1.f / fAttenuationSub;

				fDot *= ((fAttenuation-1.f>0.f)?(fAttenuation-1.f):0.f);
			}
		}
		else if( sLight.Type == D3DLIGHT_SPOT )
		{
			// Note : 일단 거리를 체크한다.
			D3DXVECTOR3 vDir = vPos-sLight.Position;
			float fLength = D3DXVec3Length( &vDir );

			// Note : 포인트 Light에 셋팅된 거리보다 실제 거리가 크면 계산하지 않는다.
			//if( fLength > fRange120 )	//sLight.Range )
			if( fLength > sLight.Range )
			{
				return -1.f;
			}

			// 외각 범위에도 미치치 못한다.
			D3DXVECTOR3 vLightDirection( sLight.Direction.x, sLight.Direction.y, sLight.Direction.z );
			D3DXVec3Normalize( &vDir, &vDir );
			float fRho = D3DXVec3Dot( &vDir, &vLightDirection );
			if ( fRho <= cosf(D3DX_PI*sLight.Phi/180.f*0.5f) )
			{
				return -1.f;
			}

			// Note : Direction Light 빛 세기.~!!
			D3DXVec3Normalize( &vDir, &vDir );
			vDir = -vDir;

			fDot = D3DXVec3Dot( &vNor, &vDir );
			if( fDot < 0.f )
				return -1.f;

			// Cone 강도를 적용한다.
			float fA_UP = fRho-cosf(D3DX_PI*sLight.Phi/180.f*0.5f);
			float fB_DOWN = cosf(D3DX_PI*sLight.Theta/180.f*0.5f)-cosf(D3DX_PI*sLight.Phi/180.f*0.5f);
			float fConePower = powf( fA_UP/fB_DOWN, sLight.Falloff );
			if ( fConePower > 1.f )
				fConePower = 1.f;
			fDot *= fConePower;

			// Point Light 쪽 감쇠
			{
				// Note : 일단 거리를 체크한다.
				D3DXVECTOR3 vDir = vPos-sLight.Position;
				float fLength = D3DXVec3Length( &vDir );

				// 라이트맵용 감쇠를 적용한다.
				if ( fLength > sLight.Range )
					return -1.f;

				// 선형 감쇠, 2차 감쇠, 지수 감쇠 적용
				FLOAT fAttenuationSub = sLight.Attenuation0;
				fAttenuationSub += sLight.Attenuation1 * fLength;
				fAttenuationSub += sLight.Attenuation2 * fLength * fLength;

				// 최종 감쇠값
				FLOAT fAttenuation = 1.f / fAttenuationSub;

				fDot *= ((fAttenuation-fAttEndLineValueForSpot>0.f)?(fAttenuation-fAttEndLineValueForSpot):0.f);
			}

			return fDot;
		}

		return fDot;
	}

    float GetDotCollObject( LPDIRECT3DDEVICEQ pd3dDevice, 
							DxStaticMeshCollBase* pStaticMesh, 
                            DxPieceManager* pPieceManager,
                            const D3DXVECTOR3& vPos, 
                            const D3DXVECTOR3& vNor, 
                            const D3DLIGHTQ& sLight,
                            BOOL& bPicking,
							BOOL bDontMakeShadowLM,
							float fAttEndLineValueForSpot )
    {
		float fDot = GetDotCollObject( pd3dDevice, vPos, vNor, sLight, fAttEndLineValueForSpot );
		if( fDot < 0.f )
			return -1.f;

        if( sLight.Type == D3DLIGHT_DIRECTIONAL )
		{
			D3DXVECTOR3 vDir = sLight.Direction;
			D3DXVec3Normalize( &vDir, &vDir );
			vDir = -vDir;

			// Note : 픽킹을 하여서 빛을 가리는 것을 찾는다.
			D3DXVECTOR3 vStart = vPos + vNor*0.001f;
			D3DXVECTOR3 vEnd = vStart + vDir*100000.f;
			if( pStaticMesh && pStaticMesh->IsCollisionLinePixel( pd3dDevice, vStart, vEnd, EMCC_CULL_NONE_EXCEPT_WATER, 0 ) )
			{
                bPicking = TRUE;
				return -1.f;
			}
            if ( pPieceManager && pPieceManager->IsCollisionLinePixel( pd3dDevice, vStart, vEnd, EMCC_CULL_NONE_EXCEPT_WATER, 0 ) )
            {
                bPicking = TRUE;
                return -1.f;
            }
		}
		else if( sLight.Type == D3DLIGHT_POINT || sLight.Type == D3DLIGHT_SPOT )
		{
			// Note : 픽킹을 하여서 빛을 가리는 것을 찾는다.
			D3DXVECTOR3 vStart = sLight.Position;	//vPos + vNor*0.1f;
			D3DXVECTOR3 vEnd = vPos + vNor*0.001f;	//vStart + vDir*sLight.Range;
			if( pStaticMesh && pStaticMesh->IsCollisionLinePixel( pd3dDevice, vStart, vEnd, EMCC_CULL_NONE_EXCEPT_WATER, 0 ) )
			{
                bPicking = TRUE;
                return -1.f;
			}
            if ( pPieceManager && pPieceManager->IsCollisionLinePixel( pd3dDevice, vStart, vEnd, EMCC_CULL_NONE_EXCEPT_WATER, 0 ) )
            {
                bPicking = TRUE;
                return -1.f;
            }
		}
        return fDot;
    }

    D3DXVECTOR3 GetColor( const D3DXVECTOR3& vPos,
                            const D3DXVECTOR3& vNor, 
                            const D3DLIGHTQ& sLight,
                            float fDot,
                            float fDiffusePower,
							BOOL bAmbientADD )
    {
        if( sLight.Type == D3DLIGHT_DIRECTIONAL )
		{
			// 컬러 리셋
			D3DXVECTOR3 vDirectLightDiffuseNew = D3DXVECTOR3( sLight.Diffuse.r, sLight.Diffuse.g, sLight.Diffuse.b );
			D3DXVECTOR3 vDirectLightAmbientNew = D3DXVECTOR3( sLight.Ambient.r, sLight.Ambient.g, sLight.Ambient.b );

			//// Note : 컬러 생성 작업.
			D3DXVECTOR3 vColor(0.f,0.f,0.f);
			if ( bAmbientADD )
			{
				vColor = vDirectLightDiffuseNew*fDot*fDiffusePower + vDirectLightAmbientNew;
			}
			else
			{
				// Radiosity 때 Ambient는 Zero 이다.
				vColor = vDirectLightDiffuseNew*fDot*fDiffusePower;
			}

			return vColor;
		}
		else if( sLight.Type == D3DLIGHT_POINT || sLight.Type == D3DLIGHT_SPOT )
		{
			// 색상 결정
			D3DXVECTOR3 vPointDiffuse( sLight.Diffuse.r, sLight.Diffuse.g, sLight.Diffuse.b );
			D3DXVECTOR3 vPointRGB = vPointDiffuse * fDot;

			return vPointRGB;
		}

		return D3DXVECTOR3(0.f,0.f,0.f);
    }

	void GetColorForObject( LPDIRECT3DDEVICEQ pd3dDevice, 
							DxStaticMeshCollBase* pStaticMesh, 
							DxPieceManager* pPieceManager,
							const D3DXVECTOR3& vPos, 
							const D3DXVECTOR3& vNor, 
							BOOL bDontMakeShadowLM,
							D3DXVECTOR3& vColorALL,
							D3DXVECTOR3& vColorIndirect,
							float& fSpot1Color,
							float& fSpot2Color,
							D3DXVECTOR3& vColorNoDynamicLight,
							D3DXVECTOR3& vColorDirectionDirect,
							const WORDWORD& wwLight )
	{
		vColorNoDynamicLight = D3DXVECTOR3 ( 0.f, 0.f, 0.f );

		// Light
		const DXLIGHT* pLightDAY = DxLightMan::GetInstance()->GetDirectNoonLight();

		// DirectionLight
		{
			// vColorALL 계산
			{
				BOOL bPicking(FALSE);
				float fDot = NSLightMapTools::GetDotCollObject( pd3dDevice, pStaticMesh, pPieceManager, vPos, vNor, pLightDAY->m_Light, bPicking, bDontMakeShadowLM, 1.f );
				if ( fDot > 0.f && NSShadowLight::IsCascadeShadow() )
				{
					vColorALL = NSLightMapTools::GetColor( vPos, vNor, pLightDAY->m_Light, fDot, DxLightMan::GetInstance()->GetLightMapDirectionDiffusePower(), !g_bRadiosityMode );
					vColorDirectionDirect = NSLightMapTools::GetColor( vPos, vNor, pLightDAY->m_Light, fDot, DxLightMan::GetInstance()->GetLightMapDirectionDiffusePower(), FALSE );
				}
				else if ( g_bRadiosityMode )
				{
					// Radiosity 때 음영은 Zero 이다.
					vColorALL = D3DXVECTOR3( 0.f, 0.f, 0.f );
					vColorDirectionDirect = D3DXVECTOR3( 0.f, 0.f, 0.f );
				}
				else
				{
					vColorALL = D3DXVECTOR3( pLightDAY->m_Light.Ambient.r, pLightDAY->m_Light.Ambient.g, pLightDAY->m_Light.Ambient.b );
					vColorDirectionDirect = D3DXVECTOR3( 0.f, 0.f, 0.f );
				}

				// vColorIndirect 계산
				vColorIndirect = D3DXVECTOR3( pLightDAY->m_Light.Ambient.r, pLightDAY->m_Light.Ambient.g, pLightDAY->m_Light.Ambient.b );

				// NoDynamicLight 계산 ( RAD에서만 사용 )
				vColorNoDynamicLight = D3DXVECTOR3 ( 0.f, 0.f, 0.f );
			}
		}

		// PointLight
		{
			D3DXVECTOR3 vPointColor;
			LIST_DXLIGHT& listPointLight = DxLightMan::GetInstance()->GetLIST_DXLIGHT();
			LIST_DXLIGHT_ITER iter = listPointLight.begin();
			for( ; iter!=listPointLight.end(); ++iter )
			{
				BOOL bPicking(FALSE);
				float fDot = NSLightMapTools::GetDotCollObject( pd3dDevice, pStaticMesh, pPieceManager, vPos, vNor, (*iter)->m_Light, bPicking, bDontMakeShadowLM, 1.f );
				if ( fDot > 0.f )
				{
					// 최대값을 하나당 셋팅 수치로 제한한다.
					if ( fDot > (*iter)->m_fOverLighting )
						fDot = (*iter)->m_fOverLighting;

					// Spot 과 마찬가지로 PointLight도 최대값을 하나당 3 이하로 제한한다.
					if ( fDot > 3.f )
						fDot = 3.f;

					vPointColor = NSLightMapTools::GetColor( vPos, vNor, (*iter)->m_Light, fDot, 1.f, !g_bRadiosityMode );

					// vColorALL 계산
					vColorALL += vPointColor;
					
					// vColorIndirect 계산
					vColorIndirect += vPointColor;

					// NoDynamicLight 계산 ( RAD에서만 사용 )
					vColorNoDynamicLight += vPointColor;
				}
			}
		}

		// SpotLight
		{
			D3DXVECTOR3 vPointColor;
			const std::vector<std::tr1::shared_ptr<DxShadowSpotLight>>& vecShadowSpotLight = NSShadowLight::GetVecShadowSpotLight();
			std::vector<std::tr1::shared_ptr<DxShadowSpotLight>>::const_iterator citer = vecShadowSpotLight.begin();
			for( ; citer!=vecShadowSpotLight.end(); ++citer )
			{
				BOOL bPicking(FALSE);
				float fDot = NSLightMapTools::GetDotCollObject( pd3dDevice, pStaticMesh, pPieceManager, vPos, vNor, (*citer)->m_sLight, bPicking, bDontMakeShadowLM, (*citer)->m_fAttEndLineValue );
				if ( fDot > 0.f )
				{
					// Spot 은 최대값을 하나당 셋팅 수치로 제한한다.
					if ( fDot > (*citer)->m_fOverLighting )
						fDot = (*citer)->m_fOverLighting;

					// Spot 은 최대값을 하나당 3 이하로 제한한다.
					if ( fDot > 3.f )
						fDot = 3.f;

					vPointColor = NSLightMapTools::GetColor( vPos, vNor, (*citer)->m_sLight, fDot, 1.f, !g_bRadiosityMode );

					vColorALL += vPointColor;

					// 연결안된것만 Indirect 에 기록하기 위해 검사를 한다.
					BOOL bLinkShadow(FALSE);
					{
						if ( wwLight.wA == (*citer)->m_dwID || wwLight.wB == (*citer)->m_dwID )
						{
							bLinkShadow = TRUE;
						}
					}

					// vColorIndirect 계산
					if ( !bLinkShadow )
					{
						vColorIndirect += vPointColor;

						// NoDynamicLight 계산 ( RAD에서만 사용 )
						vColorNoDynamicLight += vPointColor;
					}

					// Spot 강도 셋팅함.
					if ( wwLight.wA == (*citer)->m_dwID )
					{
						fSpot1Color = fDot;
					}
					else if ( wwLight.wB == (*citer)->m_dwID )
					{
						fSpot2Color = fDot;
					}
				}
			}
		}
	}

	void GetColor( LPDIRECT3DDEVICEQ pd3dDevice, 
					DxStaticMeshCollBase* pStaticMesh, 
					DxPieceManager* pPieceManager,
					const D3DXVECTOR3& vPos, 
					const D3DXVECTOR3& vNor, 
					BOOL bDontMakeShadowLM,
					D3DXVECTOR3& vColorALL,
					D3DXVECTOR4& vColorDirect_Shadow,
					D3DXVECTOR3& vColorIndirect,
					D3DXVECTOR3& vColorNoDynamicLight,
					const WORDWORD& wwLight,
					WORDWORD* pwwInTriangleSpot )
	{
		// 일단 초기화
		vColorDirect_Shadow = D3DXVECTOR4 ( 1.f, 0.f, 1.f, 0.f );
		vColorNoDynamicLight = D3DXVECTOR3 ( 0.f, 0.f, 0.f );		// ( RAD에서만 사용 )

		// Light
		const DXLIGHT* pLightDAY = DxLightMan::GetInstance()->GetDirectNoonLight();

		// DirectionLight
		{
			// vColorALL 계산
			{
				BOOL bPicking(FALSE);
				float fDot = NSLightMapTools::GetDotCollObject( pd3dDevice, pStaticMesh, pPieceManager, vPos, vNor, pLightDAY->m_Light, bPicking, bDontMakeShadowLM, 1.f );
				if ( fDot > 0.f && NSShadowLight::IsCascadeShadow() )
				{
					vColorALL = NSLightMapTools::GetColor( vPos, vNor, pLightDAY->m_Light, fDot, DxLightMan::GetInstance()->GetLightMapDirectionDiffusePower(), !g_bRadiosityMode );
				}
				else if ( g_bRadiosityMode )
				{
					// Radiosity 때 음영은 Zero 이다.
					vColorALL = D3DXVECTOR3( 0.f, 0.f, 0.f );
				}
				else
				{
					vColorALL = D3DXVECTOR3( pLightDAY->m_Light.Ambient.r, pLightDAY->m_Light.Ambient.g, pLightDAY->m_Light.Ambient.b );
				}

				// vColorDirect & vColorShadow 계산.
				{
					if ( wwLight.wA == 0 || wwLight.wA == static_cast<WORD>(-1) )
					{
						// vColorDirect 계산.
						fDot = NSLightMapTools::GetDotCollObject( pd3dDevice, vPos, vNor, pLightDAY->m_Light, 1.f );
						if ( fDot > 0.f )
						{
							vColorDirect_Shadow.w = fDot;
						}

						if ( NSShadowLight::IsCascadeShadow() )
						{
							// vColorShadow 계산
							if ( bPicking )
							{
								vColorDirect_Shadow.z = 0.f;
							}
							else
							{
								vColorDirect_Shadow.z = 1.f;
							}	
						}
						else
						{
							vColorDirect_Shadow.z = 0.f;
						}
					}
					//else if ( wwLight.wA == static_cast<WORD>(-1) )		// 작업의 이유는 상 옵에서 wwLight.wA 가 -1 인 부분과 0 인 부분에서의 절단면이 생긴다.
					//{
					//	// 무조건 그림자인 부분의 셋팅.
					//	// vColorShadow 계산
					//	if ( bPicking )
					//	{
					//		vColorDirect_Shadow.z = 0.f;
					//	}
					//	else
					//	{
					//		vColorDirect_Shadow.z = 1.f;
					//	}
					//}
				}

				// vColorIndirect 계산
				vColorIndirect = D3DXVECTOR3( pLightDAY->m_Light.Ambient.r, pLightDAY->m_Light.Ambient.g, pLightDAY->m_Light.Ambient.b );

				// NoDynamicLight 계산 ( RAD에서만 사용 )
				vColorNoDynamicLight = D3DXVECTOR3 ( 0.f, 0.f, 0.f );
			}
		}

		// PointLight
		{
			D3DXVECTOR3 vPointColor;
			LIST_DXLIGHT& listPointLight = DxLightMan::GetInstance()->GetLIST_DXLIGHT();
			LIST_DXLIGHT_ITER iter = listPointLight.begin();
			for( ; iter!=listPointLight.end(); ++iter )
			{
				BOOL bPicking(FALSE);
				float fDot = NSLightMapTools::GetDotCollObject( pd3dDevice, pStaticMesh, pPieceManager, vPos, vNor, (*iter)->m_Light, bPicking, bDontMakeShadowLM, 1.f );
				if ( fDot > 0.f )
				{
					// 최대값을 하나당 셋팅 수치로 제한한다.
					if ( fDot > (*iter)->m_fOverLighting )
						fDot = (*iter)->m_fOverLighting;

					// Spot 과 마찬가지로 PointLight도 최대값을 하나당 3 이하로 제한한다.
					if ( fDot > 3.f )
						fDot = 3.f;

					vPointColor = NSLightMapTools::GetColor( vPos, vNor, (*iter)->m_Light, fDot, 1.f, !g_bRadiosityMode );

					// vColorALL 계산
					vColorALL += vPointColor;

					// vColorIndirect 계산
					vColorIndirect += vPointColor;

					// NoDynamicLight 계산 ( RAD에서만 사용 )
					vColorNoDynamicLight += vPointColor;
				}
			}
		}

		// SpotLight
		{
			D3DXVECTOR3 vPointColor;
			const std::vector<std::tr1::shared_ptr<DxShadowSpotLight>>& vecShadowSpotLight = NSShadowLight::GetVecShadowSpotLight();
			std::vector<std::tr1::shared_ptr<DxShadowSpotLight>>::const_iterator citer = vecShadowSpotLight.begin();
			for( ; citer!=vecShadowSpotLight.end(); ++citer )
			{
				BOOL bPicking(FALSE);
				float fDot = NSLightMapTools::GetDotCollObject( pd3dDevice, pStaticMesh, pPieceManager, vPos, vNor, (*citer)->m_sLight, bPicking, bDontMakeShadowLM, (*citer)->m_fAttEndLineValue );
				if ( fDot > 0.f )
				{
					// Spot 은 최대값을 하나당 셋팅 수치로 제한한다.
					if ( fDot > (*citer)->m_fOverLighting )
						fDot = (*citer)->m_fOverLighting;

					// Spot 은 최대값을 하나당 3 이하로 제한한다.
					if ( fDot > 3.f )
						fDot = 3.f;

					vPointColor = NSLightMapTools::GetColor( vPos, vNor, (*citer)->m_sLight, fDot, 1.f, !g_bRadiosityMode );

					// vColorALL 계산
					vColorALL += vPointColor;

					// 연결안된것만 Indirect 에 기록하기 위해 검사를 한다.
					BOOL bLinkShadow(FALSE);
					{
						if ( wwLight.wA == (*citer)->m_dwID || wwLight.wB == (*citer)->m_dwID )
						{
							bLinkShadow = TRUE;
						}
					}

					// vColorIndirect 계산
					if ( !bLinkShadow )
					{
						vColorIndirect += vPointColor;

						// NoDynamicLight 계산 ( RAD에서만 사용 )
						vColorNoDynamicLight += vPointColor;
					}
				}

				// vColorDirect 계산.	// vColorShadow 계산
				{
					if ( wwLight.wA == (*citer)->m_dwID )
					{
						fDot = NSLightMapTools::GetDotCollObject( pd3dDevice, vPos, vNor, (*citer)->m_sLight, (*citer)->m_fAttEndLineValue );
						if ( fDot > 0.f )
						{
							// Spot 은 최대값을 하나당 셋팅 수치로 제한한다.
							if ( fDot > (*citer)->m_fOverLighting )
								fDot = (*citer)->m_fOverLighting;

							// Spot 은 최대값을 하나당 3 이하로 제한한다.
							if ( fDot > 3.f )
								fDot = 3.f;

							vColorDirect_Shadow.w = fDot;
						}

						// vColorShadow 계산
						if ( bPicking )
						{
							vColorDirect_Shadow.z = 0.f;
						}
						else
						{
							vColorDirect_Shadow.z = 1.f;
						}

						// 현재삼각형에 Spot_1 이 연결된다.
						if ( pwwInTriangleSpot )
						{
							pwwInTriangleSpot->wA = 1;
						}
					}
					else if ( wwLight.wB == (*citer)->m_dwID )
					{
						fDot = NSLightMapTools::GetDotCollObject( pd3dDevice, vPos, vNor, (*citer)->m_sLight, (*citer)->m_fAttEndLineValue );
						if ( fDot > 0.f )
						{
							// Spot 은 최대값을 하나당 셋팅 수치로 제한한다.
							if ( fDot > (*citer)->m_fOverLighting )
								fDot = (*citer)->m_fOverLighting;

							// Spot 은 최대값을 하나당 3 이하로 제한한다.
							if ( fDot > 3.f )
								fDot = 3.f;

							vColorDirect_Shadow.y = fDot;
						}

						// vColorShadow 계산
						if ( bPicking )
						{
							vColorDirect_Shadow.x = 0.f;
						}
						else
						{
							vColorDirect_Shadow.x = 1.f;
						}

						// 현재삼각형에 Spot_2 이 연결된다.
						if ( pwwInTriangleSpot )
						{
							pwwInTriangleSpot->wB = 1;
						}
					}
				}
			}
		}

		if ( vColorALL.x > 3.f )
			vColorALL.x = 3.f;
		if ( vColorALL.y > 3.f )
			vColorALL.y = 3.f;
		if ( vColorALL.z > 3.f )
			vColorALL.z = 3.f;

		if ( vColorIndirect.x > 3.f )
			vColorIndirect.x = 3.f;
		if ( vColorIndirect.y > 3.f )
			vColorIndirect.y = 3.f;
		if ( vColorIndirect.z > 3.f )
			vColorIndirect.z = 3.f;
	}

	void LinkTriangleLight( LPDIRECT3DDEVICEQ pd3dDevice, 
							DxStaticMeshCollBase* pStaticMesh, 
							DxPieceManager* pPieceManager,
							const D3DXVECTOR3& vPos, 
							const D3DXVECTOR3& vNor, 
							std::vector<DWORD>& vecLightForTriangle,
							BOOL bDirectionLight )
	{
		// Light
		const DXLIGHT* pLightDAY = DxLightMan::GetInstance()->GetDirectNoonLight();

		
		BOOL bPicking(FALSE);
		if ( bDirectionLight )		// Direction Light 그림자를 만든다고 할 경우만 작동되도록 한다.
		{
			float fDot = NSLightMapTools::GetDotCollObject( pd3dDevice, pStaticMesh, pPieceManager, vPos, vNor, pLightDAY->m_Light, bPicking, FALSE, 1.f );
			if ( fDot > 0.f )
			{
				BOOL bExist(FALSE);
				for ( DWORD i=0; i<vecLightForTriangle.size(); ++i )
				{
					if ( vecLightForTriangle[i] == 0 )
					{
						bExist = TRUE;
						break;
					}
				}

				if ( !bExist )
				{
					vecLightForTriangle.push_back( 0 );
				}
			}
		}

		const std::vector<std::tr1::shared_ptr<DxShadowSpotLight>>& vecShadowSpotLight = NSShadowLight::GetVecShadowSpotLight();
		std::vector<std::tr1::shared_ptr<DxShadowSpotLight>>::const_iterator citer = vecShadowSpotLight.begin();
		for( ; citer!=vecShadowSpotLight.end(); ++citer )
		{
			BOOL bPicking(FALSE);
			float fDot = NSLightMapTools::GetDotCollObject( pd3dDevice, pStaticMesh, pPieceManager, vPos, vNor, (*citer)->m_sLight, bPicking, FALSE, (*citer)->m_fAttEndLineValue );
			if ( fDot > 0.f )
			{
				BOOL bExist(FALSE);
				for ( DWORD i=0; i<vecLightForTriangle.size(); ++i )
				{
					if ( vecLightForTriangle[i] == (*citer)->m_dwID )
					{
						bExist = TRUE;
						break;
					}
				}

				if ( !bExist )
				{
					vecLightForTriangle.push_back( (*citer)->m_dwID );
				}
			}
		}
	}

	void GetPointForTriangleLightA( const std::vector<VERTEXNOR>& vecVerticesS, std::vector<VERTEXNOR>& vecPoint, BOOL bCenterAdd, int nDeepCount )
	{
		++nDeepCount;

		D3DXVECTOR3 vDir1 = vecVerticesS[1].vPos - vecVerticesS[0].vPos;
		D3DXVECTOR3 vDir2 = vecVerticesS[2].vPos - vecVerticesS[0].vPos;
		D3DXVECTOR3 vDir3 = vecVerticesS[2].vPos - vecVerticesS[1].vPos;
		float fLength1 = D3DXVec3Length( &vDir1 );
		float fLength2 = D3DXVec3Length( &vDir2 );
		float fLength3 = D3DXVec3Length( &vDir3 );
		if ( (fLength1 > 10.f || fLength2 > 10.f || fLength3 > 10.f) && nDeepCount < 6 )	// 7 일경우 6241 정점나옴. 6 일경우 1585 정점나옴.
		{
			// 4개로 분할하자.
			VERTEXNOR sTemp;

			// 0 번 기준
			std::vector<VERTEXNOR> vecVertices1;
			vecVertices1.push_back( vecVerticesS[0] );
			vecVertices1.push_back( (VERTEXNOR(vecVerticesS[0])+vecVerticesS[1])*0.5f );
			vecVertices1.push_back( (VERTEXNOR(vecVerticesS[0])+vecVerticesS[2])*0.5f );
			GetPointForTriangleLightA( vecVertices1, vecPoint, bCenterAdd, nDeepCount );

			// 1 번 기준
			std::vector<VERTEXNOR> vecVertices2;
			vecVertices2.push_back( vecVerticesS[1] );
			vecVertices2.push_back( (VERTEXNOR(vecVerticesS[1])+vecVerticesS[0])*0.5f );
			vecVertices2.push_back( (VERTEXNOR(vecVerticesS[1])+vecVerticesS[2])*0.5f );
			GetPointForTriangleLightA( vecVertices2, vecPoint, bCenterAdd, nDeepCount );

			// 2 번 기준
			std::vector<VERTEXNOR> vecVertices3;
			vecVertices3.push_back( vecVerticesS[2] );
			vecVertices3.push_back( (VERTEXNOR(vecVerticesS[2])+vecVerticesS[0])*0.5f );
			vecVertices3.push_back( (VERTEXNOR(vecVerticesS[2])+vecVerticesS[1])*0.5f );
			GetPointForTriangleLightA( vecVertices3, vecPoint, bCenterAdd, nDeepCount );

			// 센터
			std::vector<VERTEXNOR> vecVertices4;
			vecVertices4.push_back( (VERTEXNOR(vecVerticesS[1])+vecVerticesS[0])*0.5f );
			vecVertices4.push_back( (VERTEXNOR(vecVerticesS[2])+vecVerticesS[0])*0.5f );
			vecVertices4.push_back( (VERTEXNOR(vecVerticesS[2])+vecVerticesS[1])*0.5f );
			GetPointForTriangleLightA( vecVertices4, vecPoint, !bCenterAdd, nDeepCount );

			// vecPoint 에 정보를 넣자.
			if ( bCenterAdd )
			{
				for ( DWORD i=0; i<vecVertices4.size(); ++i )
				{
					vecPoint.push_back( vecVertices4[i] );
				}
			}
		}
		else
		{
			// vecPoint 에 정보를 넣자.
			vecPoint.push_back( (VERTEXNOR(vecVerticesS[2])+vecVerticesS[1]+vecVerticesS[0])/3 );
		}
	}

	void GetPointForTriangleLight( const std::vector<VERTEXNOR>& vecVerticesS, std::vector<VERTEXNOR>& vecPoint )
	{
		// vecPoint 에 정보를 넣자.
		vecPoint.push_back( vecVerticesS[0] );
		vecPoint.push_back( vecVerticesS[1] );
		vecPoint.push_back( vecVerticesS[2] );

		// 내부순환
		int nDeepCount(0);
		GetPointForTriangleLightA( vecVerticesS, vecPoint, TRUE, nDeepCount );
	}

	// 하나의 삼각형에 영향받는 라이트 갯수
	void OneTriangleIncludeLight( std::vector<DWORD>& vecLightID_OUT,
									LPDIRECT3DDEVICEQ pd3dDevice, 
									DxStaticMeshCollBase* pStaticMesh, 
									DxPieceManager* pPieceManager, 
									const std::vector<VERTEXNOR>& vecVerticesS,
									BOOL bDirectionLight )	// 삼각형 하나가 필요함. 그럼으로 정점은 3개
	{
		// 정점 3개가 아니면 동작하지 않는다.
		if ( vecVerticesS.size() != 3 )
			return;

		// 작업할 것이 없다.
		const std::vector<std::tr1::shared_ptr<DxShadowSpotLight>>& vecShadowSpotLight = NSShadowLight::GetVecShadowSpotLight();
		if ( !bDirectionLight && vecShadowSpotLight.empty() )
			return;

		std::vector<VERTEXNOR> vecPoint;
		vecPoint.reserve( 2000 );		// reserve 기준은 [7 일경우 6241 정점나옴. 6 일경우 1585 정점나옴] 임.
		GetPointForTriangleLight( vecVerticesS, vecPoint );

		// 계산을 하도록 한다.
		{
			// Light
			const DXLIGHT* pLightDAY = DxLightMan::GetInstance()->GetDirectNoonLight();

			if ( bDirectionLight )		// Direction Light 그림자를 만든다고 할 경우만 작동되도록 한다.
			{
				// vecLightID_OUT 에 없을 경우만 검사하도록 한다.
				std::vector<DWORD>::const_iterator iterFind = std::find( vecLightID_OUT.begin(), vecLightID_OUT.end(), 0 );
				if ( iterFind == vecLightID_OUT.end() )
				{
					// 삼각형과 빛과의 계산을 하도록 한다.
					BOOL bPicking(FALSE);
					for ( DWORD z=0; z<vecPoint.size(); ++z )
					{
						float fDot = NSLightMapTools::GetDotCollObject( pd3dDevice, pStaticMesh, pPieceManager, vecPoint[z].vPos, vecPoint[z].vNor, pLightDAY->m_Light, bPicking, FALSE, 1.f );
						if ( fDot > 0.f )
						{
							vecLightID_OUT.push_back( 0 );
							std::sort( vecLightID_OUT.begin(), vecLightID_OUT.end() );
							break;
						}
					}
				}
			}

			const std::vector<std::tr1::shared_ptr<DxShadowSpotLight>>& vecShadowSpotLight = NSShadowLight::GetVecShadowSpotLight();
			std::vector<std::tr1::shared_ptr<DxShadowSpotLight>>::const_iterator citer = vecShadowSpotLight.begin();
			for( ; citer!=vecShadowSpotLight.end(); ++citer )
			{
				// 일단 최단 거리로 닿을 수 있는 곳인지 아닌지 확인한다.
				D3DXPLANE sPlane;
				D3DXPlaneFromPoints( &sPlane, &vecVerticesS[0].vPos, &vecVerticesS[1].vPos, &vecVerticesS[2].vPos );
				D3DXVECTOR3 vPos( (*citer)->m_sLight.Position );
				float fLength = D3DXPlaneDotCoord( &sPlane, &vPos );
				fLength = fabsf( fLength );
				if ( fLength > (*citer)->m_sLight.Range )
				{
					continue;
				}

				// vecLightID_OUT 에 없을 경우만 검사하도록 한다.
				std::vector<DWORD>::const_iterator iterFind = std::find( vecLightID_OUT.begin(), vecLightID_OUT.end(), (*citer)->m_dwID );
				if ( iterFind == vecLightID_OUT.end() )
				{
					// 삼각형과 빛과의 계산을 하도록 한다.
					BOOL bPicking(FALSE);
					for ( DWORD z=0; z<vecPoint.size(); ++z )
					{
						float fDot = NSLightMapTools::GetDotCollObject( pd3dDevice, pStaticMesh, pPieceManager, vecPoint[z].vPos, vecPoint[z].vNor, (*citer)->m_sLight, bPicking, FALSE, (*citer)->m_fAttEndLineValue );
						if ( fDot > 0.f )
						{
							vecLightID_OUT.push_back( (*citer)->m_dwID );
							std::sort( vecLightID_OUT.begin(), vecLightID_OUT.end() );
							break;
						}
					}
				}
			}
		}
	}

	// 하나의 삼각형에 영향받는 라이트 갯수
	BOOL OneTriangleIncludeLightPiece( std::vector<DWORD>& vecLightID_OUT,
										LPDIRECT3DDEVICEQ pd3dDevice, 
										DxStaticMeshCollBase* pStaticMesh, 
										DxPieceManager* pPieceManager, 
										const std::vector<VERTEXNOR>& vecVerticesS,
										const D3DLIGHTQ& sLight,
										float fAttEndLineValue )	// 삼각형 하나가 필요함. 그럼으로 정점은 3개
	{
		// 정점 3개가 아니면 동작하지 않는다.
		if ( vecVerticesS.size() != 3 )
			return FALSE;

		std::vector<VERTEXNOR> vecPoint;
		vecPoint.reserve( 2000 );		// reserve 기준은 [7 일경우 6241 정점나옴. 6 일경우 1585 정점나옴] 임.
		GetPointForTriangleLight( vecVerticesS, vecPoint );

		// 계산을 하도록 한다.
		{
			// 삼각형과 빛과의 계산을 하도록 한다.
			BOOL bPicking(FALSE);
			for ( DWORD z=0; z<vecPoint.size(); ++z )
			{
				float fDot = NSLightMapTools::GetDotCollObject( pd3dDevice, pStaticMesh, pPieceManager, vecPoint[z].vPos, vecPoint[z].vNor, sLight, bPicking, FALSE, fAttEndLineValue );
				if ( fDot > 0.f )
				{
					return TRUE;
				}
			}
		}
		return FALSE;
	}

	DWORD ConvertColor( const D3DXVECTOR4& vSrc, int nMultiX, int nMultiY, int nMultiZ, int nMultiA )
	{
		DWORD dwRGB[4];

		dwRGB[0] = static_cast<DWORD>(vSrc.x*nMultiX);
		dwRGB[1] = static_cast<DWORD>(vSrc.y*nMultiY);
		dwRGB[2] = static_cast<DWORD>(vSrc.z*nMultiZ);
		dwRGB[3] = static_cast<DWORD>(vSrc.w*nMultiA);

		for( DWORD i=0; i<4; ++i )
		{
			if( dwRGB[i] > 255 )	dwRGB[i] = 255;
		}

		DWORD dwColor = (dwRGB[3]<<24) + (dwRGB[0]<<16) + (dwRGB[1]<<8) + dwRGB[2];
		return dwColor;
	}

    DWORD ConvertColor( const D3DXVECTOR3& vSrc, int nMulti )
	{
		DWORD dwRGB[3];

		dwRGB[0] = static_cast<DWORD>(vSrc.x*nMulti);
		dwRGB[1] = static_cast<DWORD>(vSrc.y*nMulti);
		dwRGB[2] = static_cast<DWORD>(vSrc.z*nMulti);

		for( DWORD i=0; i<3; ++i )
		{
			if( dwRGB[i] > 255 )	dwRGB[i] = 255;
		}

		DWORD dwColor = 0xff000000 + (dwRGB[0]<<16) + (dwRGB[1]<<8) + dwRGB[2];
		return dwColor;
	}

    DWORD ConvertColor( const D3DXVECTOR3& vSrc, int nMulti, DWORD dwMultiplyColor )
	{
		int nRGBA[4];
        float fRGBA[4];

		fRGBA[0] = vSrc.x*nMulti;
		fRGBA[1] = vSrc.y*nMulti;
		fRGBA[2] = vSrc.z*nMulti;
        fRGBA[3] = 255;

        // 일정 수치 이하는 나오지 않도록 작업함.
        // ( 0 ~ 1 ) -> ( 0 ~ 1.2 ) -> ( -0.2 ~ 1 )
        fRGBA[0] *= (static_cast<float>((dwMultiplyColor>>16)&0xff)/255);
        fRGBA[1] *= (static_cast<float>((dwMultiplyColor>>8)&0xff)/255);
        fRGBA[2] *= (static_cast<float>(dwMultiplyColor&0xff)/255);
        fRGBA[3] *= (static_cast<float>(dwMultiplyColor>>24)/255);


		for( DWORD i=0; i<4; ++i )
		{
            nRGBA[i] = static_cast<int>( fRGBA[i] );

			if( nRGBA[i] > 255 )	
            {
                nRGBA[i] = 255;
            }
            else if( nRGBA[i] < 0 )	
            {
                nRGBA[i] = 0;
            }
		}

		DWORD dwColor = (nRGBA[3]<<24) + (nRGBA[0]<<16) + (nRGBA[1]<<8) + nRGBA[2];
		return dwColor;
	}

	void GaussBlur4( DWORD* pData, int nWidth, int nHeight, int nSW, int nSH, int nEW, int nEH )
	{
		float afGauss4[5][5] =
		{ 
			{1.f,	4.f,	6.f,	4.f,	1.f},
			{4.f,	16.f,	24.f,	16.f,	4.f},
			{6.f,	24.f,	32.f,	24.f,	6.f},
			{4.f,	16.f,	24.f,	16.f,	4.f},
			{1.f,	4.f,	6.f,	4.f,	1.f} 
		};

		D3DXVECTOR4* pColor = new D3DXVECTOR4[nWidth*nHeight];

		// 값 백업.
		for( int y=0; y<nHeight; ++y )
		{
			for( int x=0; x<nWidth; ++x )
			{
				DWORD dwColor = pData[ x + y*nWidth ];

				pColor[ x + y*nWidth ].w = static_cast<float>((dwColor & 0xff000000) >> 24) / 255.f;
				pColor[ x + y*nWidth ].x = static_cast<float>((dwColor & 0xff0000) >> 16) / 255.f;
				pColor[ x + y*nWidth ].y = static_cast<float>((dwColor & 0xff00) >> 8) / 255.f;
				pColor[ x + y*nWidth ].z = static_cast<float>((dwColor & 0xff)) / 255.f;
			}
		}


		// 수치 셋팅.
		D3DXVECTOR4 vColor(0.f, 0.f, 0.f, 0.f);
		float fGaussValue(0.f);
		float fCountValue(0.f);
		for( int y=nSH; y<=nEH; ++y )
		{
			for( int x=nSW; x<=nEW; ++x )
			{
				vColor = D3DXVECTOR4(0.f,0.f,0.f,0.f);
				fCountValue = 0.f;
				
				for( int yy=(y-2); yy<(y+3); ++yy )
				{
					if( yy < 0 )		continue;
					if( yy >= nHeight )	continue;

					for( int xx=(x-2); xx<(x+3); ++xx )
					{
						if( xx < 0 )		continue;
						if( xx >= nWidth )	continue;

						fGaussValue = afGauss4[yy-(y-2)][xx-(x-2)];
						fCountValue += fGaussValue;
						vColor += pColor[ xx + yy*nWidth ]*fGaussValue;
					}
				}

				vColor /= fCountValue;

				if( vColor.x < 0.f )	vColor.x = 0.f;
				if( vColor.y < 0.f )	vColor.y = 0.f;
				if( vColor.z < 0.f )	vColor.z = 0.f;
				if( vColor.w < 0.f )	vColor.w = 0.f;
				if( vColor.x > 1.f )	vColor.x = 1.f;
				if( vColor.y > 1.f )	vColor.y = 1.f;
				if( vColor.z > 1.f )	vColor.z = 1.f;
				if( vColor.w > 1.f )	vColor.w = 1.f;

				pData[ x + y*nWidth ] = ((static_cast<DWORD>(vColor.w * 255.f)) << 24)  +
										((static_cast<DWORD>(vColor.x * 255.f)) << 16) +
										((static_cast<DWORD>(vColor.y * 255.f)) << 8) +
										(static_cast<DWORD>(vColor.z * 255.f));
			}
		}

		SAFE_DELETE_ARRAY( pColor );
	}

	void SetRadiosityMode( BOOL bRadiosity )
	{
		g_bRadiosityMode = bRadiosity;
	}

	BOOL IsRadiosityMode()
	{
		return g_bRadiosityMode;
	}

}
#include "pch.h"

#include "../../../../SigmaCore/Math/Random.h"

#include "../../../DxTools/DxViewPort.h"
#include "../DxEffectParticleSysDef.h"
#include "../DxEffectParticleSys.h"
#include "../DxEffSingleMan.h"

#include "ParticlesForThread.h"


//	Note	:	����׿�,
extern BOOL			g_bEFFtoTEX;

const float	MIN_SPEED			= 0.0f;
const float	MIN_LIFETIME		= 0.1f;
const float	MIN_SIZE			= 0.5f;
const float	MIN_GRAVITY			= -5.0f;
const float	MIN_ALPHA			= 0.0f;

const float	MAX_SPEED			= 250.0f;
const float	MAX_LIFETIME		= 10.0f;
const float	MAX_SIZE			= 100.0f;
const float	MAX_GRAVITY			= 5.0f;
const float	MAX_ALPHA			= 1.0f;
const float	_SCALEFACT_PAR		= 400.0f;

// useful macro to guarantee that values are within a given range
#define Clamp(x, min, max)  ( x = (x<min ? min : x<max ? x : max) )


PARTICLE::PARTICLE()
	: m_fAge(0.0f)
	, m_fSize(1.0f)
	, m_bPlay(FALSE)
	, m_pNext(NULL)
{
	m_cColor.x = 1.0f;
	m_cColor.y = 0.0f;
	m_cColor.z = 0.0f;
	m_cColor.w = 0.5f;

	D3DXMatrixIdentity( &m_matRotate );
	D3DXMatrixIdentity( &m_matWorld );
}

PARTICLE::~PARTICLE()
{
	if( m_spSingleGroup.get() )
	{
		m_spSingleGroup->InvalidateDeviceObjects ();
		m_spSingleGroup->DeleteDeviceObjects ();
		m_spSingleGroup->FinalCleanup();
		m_spSingleGroup.reset();
	}

	SAFE_DELETE(m_pNext);
}

// ��ƼŬ �ϳ��� ���ο� egp �� ��� �ִ� ����.
void PARTICLE::CreateSingleGroup_MainThread( LPDIRECT3DDEVICEQ pd3dDevice, DxEffSinglePropGroupResource& spPropGroup, const EmitterBaseSetting& sEmitterSetting )
{
	// ������ �����Ҷ� ���� �Ѹ���.
	if( !(sEmitterSetting.GetFlag()&(USENEWEFF_END|USENEWEFF_END_STOP)) )	
		m_bPlay = TRUE;

	if( m_spSingleGroup.get() )
	{
		m_spSingleGroup->CleanUp();
		m_spSingleGroup.reset();
	}

	m_spSingleGroup = spPropGroup.get()->NEWEFFGROUP();
	if ( m_spSingleGroup.get() == NULL )
		return;

	//	Note : Eff Group Create Dev.
	m_spSingleGroup->Create( pd3dDevice );

	m_matWorld._41 = m_vLocation.x;
	m_matWorld._42 = m_vLocation.y;
	m_matWorld._43 = m_vLocation.z;
	m_spSingleGroup->m_matWorld = m_matWorld;
}

BOOL PARTICLE::FrameMove ( float fElapsedTime, 
						  const D3DXVECTOR3 *pAttractLoc, 
						  const D3DXVECTOR3 *pGravity, 
						  BOOL bFromTo, 
						  const D3DXVECTOR3& vTARGET_POS,
						  const D3DXVECTOR3& vSTART_POS,
						  DWORD dwFlag )
{
	// ����� Thread �̱� ������ m_spSingleGroup->FrameMove �� ���� �ȵȴ�.
	//if( m_spSingleGroup.get() )
	//{
	//	if( m_bPlay )
	//	{
	//		m_spSingleGroup->FrameMove( 0.f, fElapsedTime );
	//	}

	//	// ����� Thread �̱� ������ �� �۾��� �Ǹ�, Main Thread ���� � ���� �������� �𸥴�.
	//	//if ( m_spSingleGroup->m_dwAliveCount == 0 )
	//	//{
	//	//	m_spSingleGroup->InvalidateDeviceObjects ();
	//	//	m_spSingleGroup->DeleteDeviceObjects ();
	//	//	m_spSingleGroup->FinalCleanup();
	//	//	m_spSingleGroup.reset();
	//	//}
	//}

	if ( m_fAge + fElapsedTime >= m_fLifetime ) 
	{
		if( (m_spSingleGroup.get() == NULL) ||								// ��ƼŬ egp �� �ƴ� ��� Ȥ��
			(m_spSingleGroup && (m_spSingleGroup->m_dwAliveCount == 0)) )	// ��ƼŬ egp �鵵 ��� ������ �� ���.
		{
			m_fAge = -1.0f;
			return FALSE;
		}

		// Note : USENEWEFF_END �� Ȱ��ȭ �Ǿ� �ִ� ��츦 ����ؼ� �־��ش�.
		m_bPlay = TRUE;

		if( dwFlag&USENEWEFF_END_STOP )	// �θ� �����̴°��� ������ �ʴ´ٸ� return �Ѵ�.
		{
			return TRUE;
		}
	}

	m_fAge += fElapsedTime;

	m_vPrevLocation = m_vLocation;

	if ( m_fAge == fElapsedTime )	m_vLocation += (m_vVelocity*fElapsedTime);
	else							m_vLocation += (m_vVelocity*fElapsedTime) + (m_vForceFull*(1.f-(m_fForceVar*m_fAge)));
	m_vVelocity += ( *pGravity * m_fGravity * fElapsedTime );

	if ( pAttractLoc && m_fAge > 0.03f )
	{
		D3DXVECTOR3 AttractDir = *pAttractLoc - m_vLocation;
		D3DXVec3Normalize ( &AttractDir, &AttractDir );
		m_vVelocity += AttractDir * 25.0f * fElapsedTime;
	}

	//	Note : ����� �浹��	- 
	//
	if ( m_bCollision )
	{
		boost::shared_ptr<DxLandMan> spLandMan = DxEffSingleMan::GetInstance().GetLandMan();
		if ( spLandMan )	// �̻�														// ó������ ��ġ�̸� Picking �����
		{
			BOOL		bCollision;
			D3DXVECTOR3	vCollision;
			D3DXVECTOR3	vPoint1( m_vLocation.x,	m_vLocation.y+5.f,		m_vLocation.z );		// �ѷ����� �߽� ��ġ ���ϱ�
			D3DXVECTOR3	vPoint2( m_vLocation.x,	m_vLocation.y-100.f,	m_vLocation.z );		// �ѷ����� �߽� ��ġ ���ϱ�
			LPDXFRAME	pDxFrame = NULL;

			if ( m_vVelocity.y < 0.f )
			{
				spLandMan->IsCollision( vPoint1, vPoint2, vCollision, bCollision, pDxFrame, EMCC_CULL_CCW );

				if ( (m_vLocation.y<=vCollision.y) &&  (m_vPrevLocation.y>=vCollision.y))
				{
					m_vVelocity.y = -m_vVelocity.y*0.7f;
					m_vLocation.y += m_vVelocity.y * fElapsedTime;
				}
			}
		}
	}

	if ( bFromTo )		// ��𿡼� ������ ����� ��ǥ�� ���� ��� ����Ѵ�.
	{
		D3DXVECTOR3	vDelta = vTARGET_POS - vSTART_POS;
		m_vLocation	= vSTART_POS + (vDelta*(m_fAge/m_fLifetime));
	}

	//	Note : ���� ���� 
	//
	if ( m_fAge < m_fAlphaTime1 )									m_fAlpha += m_fAlphaDelta1 * fElapsedTime;
	else if ( m_fAge >= m_fAlphaTime1 && m_fAge < m_fAlphaTime2)	m_fAlpha += m_fAlphaDelta2 * fElapsedTime;
	else															m_fAlpha += m_fAlphaDelta3 * fElapsedTime;

	if ( m_fAlpha < 0.f )		m_fAlpha = 0.f;
	if ( m_fAlpha > 1.f )		m_fAlpha = 1.f;

	//	Note : �÷� ���� 
	//
	m_cColor.x += m_cColorDelta.x * fElapsedTime;
	m_cColor.y += m_cColorDelta.y * fElapsedTime;
	m_cColor.z += m_cColorDelta.z * fElapsedTime;
	m_cColor.w = m_fAlpha;

	//	Note : ������, �߷� ��ȭ
	//	
	m_fSize += m_fSizeDelta * fElapsedTime;
	if ( m_fSize <= 0.f )	m_fSize = 0.01f;		// 0.f �� ��� ���� ���� ���ɼ� ������.

	m_fGravity += m_fGravityDelta * fElapsedTime;

	//	Note : �ؽ��� ȸ�� ����
	//
	m_fTexRotateAngelSum += m_fTexRotateAngel * fElapsedTime;
	m_vTexScale += m_vTexScaleDelta * fElapsedTime;

	return TRUE;
}





//////////////////////////////////////////////////////////////////////////
//							ParticleEmitter
//////////////////////////////////////////////////////////////////////////
ParticleEmitter::ParticleEmitter( const D3DXVECTOR3&	vGVelocity,
								 const D3DXVECTOR3&		vGGravity,
								 float					fLength,
								 const D3DXVECTOR3&		vPlayTime,
								 const D3DXVECTOR3&		vSleepTime )
	 : m_vGLocation(0.f,0.f,0.f)
	 , m_vGPrevLocation(0.f,0.f,0.f)
	 , m_bRandomPlay(FALSE)
	 , m_fRandomPlayTime(0.f)
	 , m_nCountR(0)
	 , m_fRotateLAngelSum(0.f)
	 , m_fElapsedTimeSum(0.f)
	 , m_fSequenceTime(0.f)

	 , m_vGVelocity(vGVelocity)
	 , m_vGGravity(vGGravity)
	 , m_fLength(fLength)
	 , m_vPlayTime(vPlayTime)
	 , m_vSleepTime(vSleepTime)
{
	D3DXMatrixIdentity( &m_matRandomR );
}

ParticleEmitter::~ParticleEmitter()
{

}

void ParticleEmitter::NewParticleCreate( PARTICLE* pParticle, 
										const DxEffSingleGroup* pThisGroup, 
										const EmitterBaseSetting& sEmitterSetting, 
										const D3DXMATRIX& matComb,
										const D3DXMATRIX& matLocal,
										D3DXVECTOR3 vPos_NowPrev, 
										float fElapsedTime )
{
	D3DXMATRIX matTrans, matRotate, matParent;

	D3DXVECTOR3 tempVelocity = ( m_vGLocation - m_vGPrevLocation ) / fElapsedTime;

	//	Note : �ʱ� ���� ��ġ ����. 
	pParticle->m_vLocation = m_vGPrevLocation + tempVelocity * sc::Random::RANDOM_NUM() * fElapsedTime;

	//	Note : ���� ���� ���� 
	D3DXVECTOR3 tempLocation (0.f, 0.f, 0.f );

	if ( sEmitterSetting.GetFlag() & USERANGE )
	{
		if ( sEmitterSetting.IsFlagEX( USE_EX_RANGE_SCREEN ) )
		{
			/*
			//	Note :	LookAt Matrix
			//	vRight.x	vUp.x	vDir.x
			//	vRight.y	vUp.y	vDir.y
			//	vRight.z	vUp.z	vDir.z
			//
			D3DXVECTOR3 vUp, vDir, vRight;
			vRight.x = m_matView._11;
			vRight.y = m_matView._21;
			vRight.z = m_matView._31;

			vUp.x = m_matView._12;
			vUp.y = m_matView._22;
			vUp.z = m_matView._32;

			vDir.x = m_matView._13;
			vDir.y = m_matView._23;
			vDir.z = m_matView._33;
			*/
			const D3DXMATRIX& matView = DxViewPort::GetInstance().GetMatView();

			D3DXVECTOR3 vUp, vRight;
			vRight.x = matView._11;
			vRight.y = matView._21;
			vRight.z = matView._31;

			vUp.x = matView._12;
			vUp.y = matView._22;
			vUp.z = matView._32;

			tempLocation += vRight * (sc::Random::RANDOM_NUM()*sEmitterSetting.GetRange().x);
			tempLocation += vUp * (sc::Random::RANDOM_NUM()*sEmitterSetting.GetRange().y);
		}
		else
		{
			tempLocation.x = (sEmitterSetting.GetRange().x * (rand()%1000)/1000.f) - (sEmitterSetting.GetRange().x*0.5f);
			tempLocation.y = (sEmitterSetting.GetRange().y * (rand()%1000)/1000.f) - (sEmitterSetting.GetRange().y*0.5f);
			tempLocation.z = (sEmitterSetting.GetRange().z * (rand()%1000)/1000.f) - (sEmitterSetting.GetRange().z*0.5f);

			if ( sEmitterSetting.GetFlag() & USESPHERE )
			{
				if ( (tempLocation.x*tempLocation.x) + (tempLocation.z*tempLocation.z) > (sEmitterSetting.GetRange().x*sEmitterSetting.GetRange().x/4.f) )
				{
					tempLocation = tempLocation/2.f;
				}

				//	Note : ���� �����ȿ� �ִ� ��ƼŬ �� .. ���������� �ִ� �͵���  �������� ������ �� �ִ�.
				//
				if ( sEmitterSetting.GetRangeRate() > 0.f )
				{
					float	fTempLong;

					fTempLong = sEmitterSetting.GetRange().x/2.f;
					if ( fTempLong < tempLocation.y/2.f ) fTempLong = tempLocation.y;
					if ( fTempLong < tempLocation.z/2.f ) fTempLong = tempLocation.z;	//�ְ� �� ���� ã�´�.

					// 1. ���� ������ �ִ��� �˻�		// �ٲ���
					float fTemp1 = sqrtf( (tempLocation.x*tempLocation.x) + (tempLocation.y*tempLocation.y) );
					if ( sqrtf( (fTemp1*fTemp1) + (tempLocation.z*tempLocation.z) ) < fTempLong*sEmitterSetting.GetRangeRate() )
					{

						// 2. ������ ã��
						float		fRate, fTemp;
						D3DXVECTOR3 tempStart;
						tempStart = (sEmitterSetting.GetRange())*sEmitterSetting.GetRangeRate()/2.f;

						fRate = fabsf(tempStart.x/tempLocation.x);

						fTemp = fabsf(tempStart.y/tempLocation.y);
						if ( fRate > fTemp ) fRate = fTemp;

						fTemp = fabsf(tempStart.z/tempLocation.z);
						if ( fRate > fTemp ) fRate = fTemp;	// �ּ� ���� ã��

						tempStart = tempLocation * fRate;	// ������ ���ϱ�

						// 3. �����ν� ���ο� �� ã��
						tempLocation = tempStart + tempLocation*(1-sEmitterSetting.GetRangeRate());
					}
				}
			}
			else
			{
				//	Note : ���� �����ȿ� �ִ� ��ƼŬ �� .. ���������� �ִ� �͵���  �������� ������ �� �ִ�.
				//
				if ( sEmitterSetting.GetRangeRate() > 0.f )
				{
					// 1. ���� ������ �ִ��� �˻�	// �̰��� ���簢��, ����ü�� �ش�
					if ( sEmitterSetting.GetRange().x*sEmitterSetting.GetRangeRate()/2.f >= tempLocation.x && -sEmitterSetting.GetRange().x*sEmitterSetting.GetRangeRate()/2.f <= tempLocation.x &&
						sEmitterSetting.GetRange().y*sEmitterSetting.GetRangeRate()/2.f >= tempLocation.y && -sEmitterSetting.GetRange().y*sEmitterSetting.GetRangeRate()/2.f <= tempLocation.y &&
						sEmitterSetting.GetRange().z*sEmitterSetting.GetRangeRate()/2.f >= tempLocation.z && -sEmitterSetting.GetRange().z*sEmitterSetting.GetRangeRate()/2.f <= tempLocation.z )
					{

						// 2. ������ ã��
						float		fRate, fTemp;
						D3DXVECTOR3 tempStart;
						tempStart = (sEmitterSetting.GetRange())*sEmitterSetting.GetRangeRate()/2.f;

						fRate = fabsf(tempStart.x/tempLocation.x);

						fTemp = fabsf(tempStart.y/tempLocation.y);
						if ( fRate > fTemp ) fRate = fTemp;

						fTemp = fabsf(tempStart.z/tempLocation.z);
						if ( fRate > fTemp ) fRate = fTemp;	// �ּ� ���� ã��

						tempStart = tempLocation * fRate;	// ������ ���ϱ�

						// 3. �����ν� ���ο� �� ã��
						tempLocation = tempStart + tempLocation*(1-sEmitterSetting.GetRangeRate());
					}
				}
			}
		}
		pParticle->m_vLocation += tempLocation;
	}

	//	Note : ȸ�� ����
	if ( sEmitterSetting.GetFlag() & USEROTATEL )
	{
		D3DXVECTOR3		vCenter ( 0.f, 0.f, 0.f );
		D3DXVECTOR3		vPoint ( 1.f, 0.f, 0.f );		
		D3DXVECTOR3		vTempPoint1 ( -0.01f, 10000.f, 0.f );
		D3DXVECTOR3		vTempPoint2 ( -0.01f, -10000.f, 0.f );
		D3DXPLANE		vPlane;										

		if ( tempVelocity.y <= 0.01f && tempVelocity.y >= -0.01f )		vPoint = D3DXVECTOR3 ( 0.f, 1.f, 0.f );
		else
		{
			D3DXPlaneFromPointNormal ( &vPlane, &vCenter, &tempVelocity );
			vPoint = DxSplit ( &vPlane, &vTempPoint1, &vTempPoint2 );
			D3DXVec3Normalize ( &vPoint, &vPoint );
		}

		//	Note : ȸ�� ������ - FrameMove
		//
		if ( pThisGroup->m_fGAge >= pThisGroup->m_fGBeginTime && pThisGroup->m_fGAge < sEmitterSetting.GetRotateLTime1() )
			m_fLength +=  sEmitterSetting.GetRotateLDelta1()*m_fElapsedTimeSum;
		else if ( pThisGroup->m_fGAge >= sEmitterSetting.GetRotateLTime1() && pThisGroup->m_fGAge < sEmitterSetting.GetRotateLTime2() )	
			m_fLength +=  sEmitterSetting.GetRotateLDelta2()*m_fElapsedTimeSum;
		else														
			m_fLength +=  sEmitterSetting.GetRotateLDelta3()*m_fElapsedTimeSum;


		D3DXMATRIX matRotateL;
		vPoint *= m_fLength;
		m_fRotateLAngelSum += sEmitterSetting.GetRotateLAngel() * m_fElapsedTimeSum * 360.f;
		if ( tempVelocity.y > 0.f )	D3DXMatrixRotationAxis ( &matRotateL ,&tempVelocity , m_fRotateLAngelSum*D3DX_PI/180.f );	// ȸ�� ��Ʈ���� ����
		else						D3DXMatrixRotationAxis ( &matRotateL ,&tempVelocity , m_fRotateLAngelSum*D3DX_PI/180.f + D3DX_PI );	// ȸ�� ��Ʈ���� ����

		D3DXVec3TransformCoord ( &vCenter, &vPoint ,&matRotateL );

		pParticle->m_vLocation += vCenter;
		m_fElapsedTimeSum = 0.f;
	}

	//	Note : �޽� ��ƼŬ�� ���⼺�� ������ ������ �����ϰ� ȸ������ ���´�.
	if ( (sEmitterSetting.GetFlag()&USEMESH) && !(sEmitterSetting.GetFlag()&USEDIRECTION) )
	{
		//D3DXMatrixIdentity ( &matTrans );
		//D3DXMatrixRotationX ( &matRotate, (float)rand() );
		//D3DXMatrixMultiply ( &matTrans, &matTrans, &matRotate );
		//D3DXMatrixRotationY ( &matRotate, (float)rand() );
		//D3DXMatrixMultiply ( &matTrans, &matTrans, &matRotate );
		//D3DXMatrixRotationZ ( &matRotate, (float)rand() );

		D3DXMatrixIdentity ( &matTrans );
		D3DXMatrixRotationY ( &matRotate, (float)rand() );

		D3DXMatrixMultiply ( &pParticle->m_matRotate, &matTrans, &matRotate );
	}

	//	Note : �̵� �� ���� �� ���� !
	//
	if ( sEmitterSetting.GetFlag()&USEPARENTMOVE )			// ���� ����	(�����ǥ)	//	�����(ĳ��)
	{
		matParent = matLocal;
		//D3DXVec3TransformCoord ( &pParticle->m_vLocation, &pParticle->m_vLocation, m_pmatLocal );
	}
	else	// ȸ���� ����	(������ǥ)	// ��(����)
	{
		//	matParent = matComb;
		//	matParent._41 = pThisGroup->m_matWorld._41;
		//	matParent._42 = pThisGroup->m_matWorld._42;
		//	matParent._43 = pThisGroup->m_matWorld._43;
		D3DXMatrixMultiply ( &matParent, &matLocal, &matComb );//&matParent );

		//D3DXVec3TransformCoord ( &pParticle->m_vLocation, &pParticle->m_vLocation, &matParent );
	}

	//	Note : ȸ�� �� ���� �� ���� !
	//
	if ( sEmitterSetting.GetFlag() & USEPARENTROTATE_FOR_PARTICLE )
	{
		matRotate = matComb;
		matRotate._41 = 0.f;
		matRotate._42 = 0.f;
		matRotate._43 = 0.f;
		matRotate._44 = 1.f;
		//D3DXMatrixMultiply ( &matParent, &matParent, &matRotate );
		D3DXMatrixMultiply_RM3( matParent, matRotate );
	}

	//	Note : �̵��� ȸ�� ���� ������
	//
	D3DXVec3TransformCoord ( &pParticle->m_vLocation, &pParticle->m_vLocation, &matParent );

	pParticle->m_vPrevLocation = m_vGLocation;
}

void ParticleEmitter::FrameMove( DWORD dwRunFlag, float fElapsedTime, const DxEffectParticleSys* pParticleSysSource, PARTICLE* pParticleHead )
{
	const EmitterBaseSetting& sEmitterSetting = pParticleSysSource->GetEmitterSetting();
	const ParticlesBaseSetting& sParticleSetting = pParticleSysSource->GetParticleSetting();

	m_fSequenceTime += fElapsedTime;

	////	Note : ȸ�� ����
	////
	//if ( (m_dwFlag&USERANGEHIGH) )			
	//	m_fRotateAngelSum += m_fRotateAngel * fElapsedTime;

	if ( !( dwRunFlag & EFF_PLY_DONE ) )
	{
		//	Note : ��ġ �̵�.
		//
		m_vGPrevLocation = m_vGLocation;
		m_vGLocation += m_vGVelocity * fElapsedTime;


		//	Note : ȸ������ �ʿ�, �ð��� ����
		m_fElapsedTimeSum += fElapsedTime;

		//	Note : �ӵ� ��ȭ. [ GRAVITY ������ �ִ°� ����. ]  - [�����ʿ�]
		D3DXVECTOR3 vDeltaGVel = m_vGGravity*fElapsedTime;

		m_vGVelocity += vDeltaGVel;

		//	Note : �߷� ��ȭ.
		m_vGGravity += sEmitterSetting.GetGGravityDelta() * fElapsedTime;
	}

	// Note : ���� Play ����
	if ( sEmitterSetting.GetFlag() & USERANDOMPLAY )
	{
		if( m_bRandomPlay )
		{
			if( m_fRandomPlayTime > m_vPlayTime.z )
			{
				m_fRandomPlayTime = 0.f;
				m_bRandomPlay = FALSE;

				m_vSleepTime.z = m_vSleepTime.x + m_vSleepTime.y * sc::Random::RANDOM_POS();
			}
		}
		else
		{
			if( m_fRandomPlayTime > m_vSleepTime.z )	
			{
				m_fRandomPlayTime = 0.f;
				m_bRandomPlay = TRUE;

				m_vPlayTime.z = m_vPlayTime.x + m_vPlayTime.y * sc::Random::RANDOM_POS();

				// Note : ȿ���� �� Play �� ��� ������ �ٲ��ִ� ����
				if ( sEmitterSetting.GetFlag() & USERANDOMDIRECT )
				{
					D3DXMATRIX matRotate;
					if( m_nCountR==0 )
					{
						D3DXMatrixRotationX( &matRotate, (float)rand() );
						D3DXMatrixMultiply( &m_matRandomR, &m_matRandomR, &matRotate );
					}
					else if( m_nCountR==1 )
					{
						D3DXMatrixRotationY( &matRotate, (float)rand() );
						D3DXMatrixMultiply( &m_matRandomR, &m_matRandomR, &matRotate );
					}
					else if( m_nCountR==2 )
					{
						D3DXMatrixRotationZ( &matRotate, (float)rand() );
						D3DXMatrixMultiply( &m_matRandomR, &m_matRandomR, &matRotate );
					}

					++m_nCountR;

					if( m_nCountR==3 )	m_nCountR = 0;
				}
			}
		}

		m_fRandomPlayTime += fElapsedTime;
	}

	//	Note : ������ ����
	//
	if ( sEmitterSetting.GetFlag() & USESEQUENCE )
	{
		PARTICLE* pParticleNode(NULL);
		if ( sEmitterSetting.GetFlag() & USESEQUENCELOOP )
		{
			if ( m_fSequenceTime > sParticleSetting.GetAniTime() )
			{
				m_fSequenceTime = 0.f;

				pParticleNode = pParticleHead;
				while ( pParticleNode )
				{
					if ( pParticleNode->m_bSequenceRoop )	pParticleNode->m_nNowSprite++;
					else									pParticleNode->m_nNowSprite--;

					pParticleNode = pParticleNode->m_pNext;
				}
			}

			pParticleNode = pParticleHead;
			while ( pParticleNode )
			{
				if ( pParticleNode->m_nNowSprite >= sParticleSetting.GetAllSprite() )
				{
					pParticleNode->m_bSequenceRoop	= !pParticleNode->m_bSequenceRoop;
					pParticleNode->m_nNowSprite		-= 2;
				}
				else if ( pParticleNode->m_nNowSprite < 0 )
				{
					pParticleNode->m_bSequenceRoop	= !pParticleNode->m_bSequenceRoop;
					pParticleNode->m_nNowSprite		+= 2;
				}

				pParticleNode = pParticleNode->m_pNext;
			}
		}
		else
		{
			if ( m_fSequenceTime > sParticleSetting.GetAniTime() )
			{
				m_fSequenceTime = 0.f;

				pParticleNode = pParticleHead;
				while ( pParticleNode )
				{
					pParticleNode->m_nNowSprite++;

					if ( pParticleNode->m_nNowSprite >= sParticleSetting.GetAllSprite() )	
						pParticleNode->m_nNowSprite = 0;

					pParticleNode = pParticleNode->m_pNext;
				}
			}
		}
	}
}






//////////////////////////////////////////////////////////////////////////
//							ParticlesForThread
//////////////////////////////////////////////////////////////////////////
ParticlesForThread::ParticlesForThread( boost::shared_ptr<ParticleEmitter>& spParicleEmitter,
									   boost::shared_ptr<DxEffSingleGroup>& spThisGroup,
									   const DxEffectParticleSys* pEffectParticleSys )
   : SingleForThread( spThisGroup )
   , m_spParicleEmitter(spParicleEmitter)
   //, m_wpThisGroup(spThisGroup)
   , m_rParticleSysSource(pEffectParticleSys)
   , m_pParticleHead(NULL)
   , m_uParticlesAlive(0)
   , m_uParticlesCreated(0)
   , m_fEmissionResidue(0.f)
   , m_fAlpha(1.f)
{
	D3DXMatrixIdentity( &m_matPrevComb );
	D3DXMatrixIdentity( &m_matRotate );
	D3DXMatrixIdentity( &m_matCombine );

	m_dwDrawPoints[0] = 0;
	m_dwDrawPoints[1] = 0;
	D3DXMatrixIdentity( &m_matWorld[0] );
	D3DXMatrixIdentity( &m_matWorld[1] );

	//m_rParticleSysSource = MIN_BOOST::polymorphic_downcast<const DxEffectParticleSys*>( pSingleSource );
}

ParticlesForThread::~ParticlesForThread()
{
	SAFE_DELETE ( m_pParticleHead );
}

void ParticlesForThread::PutSleepParticles ( PARTICLE* pParticles )
{
	if ( !pParticles )	
		return;

	PARTICLE* pCur = pParticles;
	while ( pCur )
	{
		if( pCur->m_spSingleGroup.get() )
		{
			pCur->m_spSingleGroup->InvalidateDeviceObjects ();
			pCur->m_spSingleGroup->DeleteDeviceObjects ();
			pCur->m_spSingleGroup->FinalCleanup();
			pCur->m_spSingleGroup.reset();
		}

		pCur = pCur->m_pNext;
	}

	//	Note : ���� ���� ��ƼŬ ����Ʈ�� �� ��带 ã�´�.
	//
	{
		PARTICLE* pCur = pParticles;
		while ( pCur )
		{
			PARTICLE* pRelease = pCur;
			pCur = pCur->m_pNext;

			pRelease->m_pNext = NULL;
			SAFE_DELETE(pRelease);
		}
	}
}

void ParticlesForThread::PutSleepParticle ( PARTICLE* pParticle )
{
	if ( !pParticle )	
		return;

	if( pParticle->m_spSingleGroup.get() )
	{
		pParticle->m_spSingleGroup->InvalidateDeviceObjects ();
		pParticle->m_spSingleGroup->DeleteDeviceObjects ();
		pParticle->m_spSingleGroup->FinalCleanup();
		pParticle->m_spSingleGroup.reset();
	}

	//	Note : ���� ���� ��ƼŬ ����Ʈ�� �� ��带 ã�´�.
	{
		pParticle->m_pNext = NULL;
		SAFE_DELETE(pParticle);
	}
}

void ParticlesForThread::CheckAABBBox( D3DXVECTOR3& vMax, D3DXVECTOR3& vMin ) const
{
	if ( !m_pParticleHead )
		return;

	float fSize;
	D3DXVECTOR3 vBase;
	D3DXVECTOR3 vPos;
	PARTICLE* pParticleNode = m_pParticleHead;
	while ( pParticleNode )
	{
		//if ( pParticleNode->m_fAge + fElapsedTime >= pParticleNode->m_fLifetime ) 
		if ( pParticleNode->m_fAge >= pParticleNode->m_fLifetime ) 
		{
			pParticleNode = pParticleNode->m_pNext;
			continue;
		}

		fSize = pParticleNode->m_fSize*0.5f;
		D3DXVec3TransformCoord( &vBase, &pParticleNode->m_vLocation, &m_matWorld[m_nUSED] );

		vPos = vBase + D3DXVECTOR3( fSize, fSize, fSize);
		if( vMax.x < vPos.x )	vMax.x = vPos.x;
		if( vMax.y < vPos.y )	vMax.y = vPos.y;
		if( vMax.z < vPos.z )	vMax.z = vPos.z;

		vPos = vBase - D3DXVECTOR3( fSize, fSize, fSize);
		if( vMin.x > vPos.x )	vMin.x = vPos.x;
		if( vMin.y > vPos.y )	vMin.y = vPos.y;
		if( vMin.z > vPos.z )	vMin.z = vPos.z;

		pParticleNode = pParticleNode->m_pNext;
	}
}

void ParticlesForThread::FrameMove( float fElapsedTime )
{
	if ( m_nUSED == -1 )
		return;

	const boost::shared_ptr<DxEffSingleGroup> spThisGroup = m_wpThisGroup.lock();
	if ( !spThisGroup )
		return;

	//boost::shared_ptr<DxEffSingleGroup> spThisGroup = m_wpThisGroup.lock();
	//if ( !spThisGroup )
	//	return;

	const EmitterBaseSetting& sEmitterSetting = m_rParticleSysSource->GetEmitterSetting();
	const ParticlesBaseSetting& sParticleSetting = m_rParticleSysSource->GetParticleSetting();

	//	Note : �߷�.. �߷�. �θ��� ȸ���� ������ ������ �޵��� �����.
	D3DXVECTOR3	vLocal (0.f,9.8f,0.f);
	D3DXVec3TransformCoord ( &vLocal, &vLocal, &m_matRotate );

	//	Note : ��ƼŬ ������Ʈ.
	m_uParticlesAlive = 0;

	D3DXVECTOR3 vAttractLocBase( 0.f, 0.f, 0.f );
	const D3DXVECTOR3 *pAttractLoc(NULL);
	if ( sEmitterSetting.GetFlag()&USEATTRACTION )
	{
		if ( sEmitterSetting.GetFlag()&USEPARENTMOVE )		pAttractLoc = &vAttractLocBase;
		else												pAttractLoc = &(spThisGroup->m_vGNowPos);
	}

	BOOL		bFromTo(FALSE);
	D3DXVECTOR3 vSTART_POS(0.f,0.f,0.f);
	D3DXVECTOR3 vTARGET_POS(0.f,0.f,0.f);
	if ( sEmitterSetting.GetFlag()&USEFROMTO )
	{
		bFromTo		= TRUE;
		vSTART_POS	= spThisGroup->GetNowPos();
		vTARGET_POS	= spThisGroup->GetTargetPos();
	}

	m_spParicleEmitter->FrameMove( m_rParticleSysSource->m_dwRunFlag, fElapsedTime, m_rParticleSysSource, m_pParticleHead );

	m_uParticlesAlive = 0;

	PARTICLE* pParticleNode = m_pParticleHead;
	PARTICLE* pParticlePrev = NULL;
	while ( pParticleNode )
	{		
		if ( pParticleNode->FrameMove ( fElapsedTime, pAttractLoc, &vLocal, bFromTo, vTARGET_POS, vSTART_POS, sEmitterSetting.GetFlag() ) )
		{
			m_uParticlesAlive++;

			pParticlePrev = pParticleNode;
			pParticleNode = pParticleNode->m_pNext;
		}
		else
		{
			// ���� �Ǿ��� �� ���.
			m_vecDeleteParticles.push_back( pParticleNode );

			//	Note : ����Ʈ���� �и�.
			//
			PARTICLE* pParticleThis = pParticleNode;
			if ( pParticleThis == m_pParticleHead )
			{
				m_pParticleHead = pParticleNode->m_pNext;
				pParticlePrev = NULL;

				pParticleNode = m_pParticleHead;
			}
			else
			{
				pParticlePrev->m_pNext = pParticleNode->m_pNext;

				pParticleNode = pParticlePrev->m_pNext;
			}
		}
	}

	// ���� ������� ��ƼŬ ������ ����Ѵ�.
	{
		// fps �� �ʴٰ� 1 Frame �� �ʹ� ���� ��ƼŬ�� �Ѹ��� �ʵ��� ����. ���� (30fps)
		if ( fElapsedTime > 0.0333f )
			fElapsedTime = 0.0333f;

		float fParticlesNeeded = m_rParticleSysSource->GetParticlesPerSec() * fElapsedTime + m_fEmissionResidue;
		m_uParticlesCreated = (DWORD) fParticlesNeeded;

		if ( m_uParticlesCreated >= m_rParticleSysSource->GetParticlesPerSec() )
		{
			m_uParticlesCreated = m_rParticleSysSource->GetParticlesPerSec();
		}

		m_fEmissionResidue = fParticlesNeeded - m_uParticlesCreated;

		if ( m_rParticleSysSource->m_dwRunFlag & EFF_PLY_DONE )
		{
			m_uParticlesCreated = 0;
		}

	}
}

void ParticlesForThread::NewParticleCreate( const D3DXMATRIX &matComb, const D3DXVECTOR3& vPos_NowPrev, float fElapsedTime )
{
	const boost::shared_ptr<DxEffSingleGroup> spThisGroup = m_wpThisGroup.lock();
	if ( !spThisGroup )
		return;

	const EmitterBaseSetting& sEmitterSetting = m_rParticleSysSource->GetEmitterSetting();

	// Note : Play�� �ƴҶ��� ������ ���Ѵ�.
	if ( sEmitterSetting.GetFlag() & USERANDOMPLAY )
	{
		if( !m_spParicleEmitter->GetRandomPlay() )
		{
			m_uParticlesCreated = 0;
			return;
		}
	}

	if ( !spThisGroup->m_bOut_Particle )	
	{
		m_uParticlesCreated = 0;
	}

	if( !g_bEFFtoTEX )
	{
		if ( m_uParticlesCreated >= 1 )		
			m_uParticlesCreated = 1;	// 1 ���� ũ�� ���� .. �������� ����
	}

	D3DXMATRIX matTrans, matRotate, matParent;

	int		nBlur = 5;
	if ( sEmitterSetting.GetFlag() & USEBEZIER_P )
	{
		m_uParticlesCreated *= nBlur;	// �ܻ� ��.
	}

	//	Note : DONE ���¿��� ������ �ȴ�.
	//
	PARTICLE* pParticle = NULL;
	while  ( m_uParticlesCreated > 0 && !(m_rParticleSysSource->m_dwRunFlag&EFF_PLY_DONE) )
	{
		//	Note : ���ο� ��ƼŬ �Ҵ�.
		pParticle = new PARTICLE;

		////	Note : ��ƼŬ Ȱ�� ����Ʈ�� ���.
		//pParticle->m_pNext = m_pParticleHead;
		//m_pParticleHead = pParticle;

		m_rParticleSysSource->GetParticleSetting().NewParticleCreate( pParticle, m_rParticleSysSource );

		//	Note : ��ƼŬ ������ �浹�� �Ѵ�.
		if ( sEmitterSetting.GetFlag() & USECOLLISION )	pParticle->m_bCollision = TRUE;
		else											pParticle->m_bCollision = FALSE;

		m_spParicleEmitter->NewParticleCreate( pParticle, spThisGroup.get(), sEmitterSetting, matComb, m_rParticleSysSource->GetLocalMatrix(), vPos_NowPrev, fElapsedTime );

		float fRate = 1.f;

		//	Note : ������ ���ؼ� �������� ���..
		//
		if ( sEmitterSetting.GetFlag() & USEBEZIER_P )	
			fRate = 1.f - (float)(m_uParticlesCreated % nBlur)/(float)(nBlur);

		pParticle->m_vLocation -= spThisGroup->m_vGDelta * fElapsedTime * fRate;

		m_rParticleSysSource->GetParticleSetting().NewParticleCreate2
		( 
			pParticle, 
			spThisGroup.get(), 
			m_rParticleSysSource, 
			vPos_NowPrev, 
			fElapsedTime, 
			sEmitterSetting.GetFlag(),
			m_matRotate 
		);

		m_uParticlesCreated--;

		m_vecCreateParticles.push_back( pParticle );
	}

	m_spParicleEmitter->m_vGPrevLocation = m_spParicleEmitter->m_vGLocation;
}

// pParticle->CreateSingleGroup( pd3dDevice, m_PropGroupRes ); �۾��� �ʿ��ϴ�.

void ParticlesForThread::SetMatrix( D3DXMATRIX &matWorld, const D3DXMATRIX &matComb )
{
	const boost::shared_ptr<DxEffSingleGroup> spThisGroup = m_wpThisGroup.lock();
	if ( !spThisGroup )
		return;

	const EmitterBaseSetting& sEmitterSetting = m_rParticleSysSource->GetEmitterSetting();

	//// ��ũ��~!!
	//if ( sEmitterSetting.IsFlagEX( USE_EX_RANGE_SCREEN ) )
	//{
	//	const D3DXVECTOR3& vFromPt = DxViewPort::GetInstance().GetFromPt();
	//	const D3DXVECTOR3& vLookDir = DxViewPort::GetInstance().GetLookDir();
	//	//matWorld = DxViewPort::GetInstance().GetMatView();

	//	D3DXMatrixIdentity( &matWorld );
	//	matWorld._41 = vFromPt.x + vLookDir.x * 6.f;
	//	matWorld._42 = vFromPt.y + vLookDir.y * 6.f;
	//	matWorld._43 = vFromPt.z + vLookDir.z * 6.f;

	//	return;
	//}

	float		fTime;
	D3DXVECTOR3	vDelta(0.f,0.f,0.f);
	D3DXMATRIX matTrans, matEmitter;	//, matRotate, matParent;

	//	Note : �θ� ȸ�� ����� �Ͱ� �ȵ� ��Ʈ���� �����
	//			�׷��� ������ġ�� DTS_WORLD�� �̵��� ������ �־�� �Ѵ�.	�ѷ��� �� ��ġ ��Ʈ����!!!
	//
	if ( sEmitterSetting.GetFlag() & USEPARENTROTATEWORLD )	// ���� ������ ����..
	{
		matEmitter = matComb;
		matEmitter._41 = spThisGroup->m_matWorld._41;
		matEmitter._42 = spThisGroup->m_matWorld._42;
		matEmitter._43 = spThisGroup->m_matWorld._43;
	}
	else
	{
		D3DXMatrixIdentity ( &matEmitter );
		matEmitter._41 = spThisGroup->m_matWorld._41;
		matEmitter._42 = spThisGroup->m_matWorld._42;
		matEmitter._43 = spThisGroup->m_matWorld._43;
	}

	//	Note : �ڱ� �ڽ��� ��Ʈ������ �־��ش�.
	//
	//matWorld = m_matLocal;
	D3DXMatrixIdentity( &matWorld );
	matWorld._41 = 0.f;
	matWorld._42 = 0.f;
	matWorld._43 = 0.f;

	//	Note : ���� ������ �־��ִ� ȸ��(!!!ȸ��!!!)�� ���� �ϱ� ���Ͽ� �� ����� �Ѵ�.
	//
	m_matRotate = matEmitter;
	m_matRotate._41 = 0.f;
	m_matRotate._42 = 0.f;
	m_matRotate._43 = 0.f;


	// ���� ����	(�����ǥ)	//	�����(ĳ��), ��(����)	, �̵��� �ص� ���� �׻���, ������ ȸ���� ������ ����
	if ( (sEmitterSetting.GetFlag()&USEPARENTMOVE) )			
	{
		matWorld._41 = matComb._41;
		matWorld._42 = matComb._42;
		matWorld._43 = matComb._43;
		matWorld._44 = matComb._44;
	}
	// ���� �� ��Ŵ	(������ǥ)	//							, �̵��� �ϸ� ��鸰��.
	else
	{
		matWorld._44 = matComb._44;
	}

	//	Note : 	�ڽ��� ��ġ���� �������� �������� ����
	//
	if ( sEmitterSetting.GetFlag() & USEGOTOCENTER )
	{
		matTrans = m_rParticleSysSource->GetLocalMatrix();

		fTime = m_rParticleSysSource->m_fGBeginTime+m_rParticleSysSource->m_fGLifeTime-m_rParticleSysSource->m_fGAge;

		vDelta.x = m_rParticleSysSource->GetLocalMatrix()._41/m_rParticleSysSource->m_fGLifeTime;
		vDelta.y = m_rParticleSysSource->GetLocalMatrix()._42/m_rParticleSysSource->m_fGLifeTime;
		vDelta.z = m_rParticleSysSource->GetLocalMatrix()._43/m_rParticleSysSource->m_fGLifeTime;

		matTrans._41 = vDelta.x*fTime;
		matTrans._42 = vDelta.y*fTime;
		matTrans._43 = vDelta.z*fTime;

		D3DXMatrixMultiply ( &matWorld, &matWorld, &matTrans );
	}
}

void ParticlesForThread::SetVertices( float fElapsedTime, const D3DXMATRIX& matCameraView )
{
	const boost::shared_ptr<DxEffSingleGroup> spThisGroup = m_wpThisGroup.lock();
	if ( !spThisGroup )
		return;

	const EmitterBaseSetting& sEmitterSetting = m_rParticleSysSource->GetEmitterSetting();

	D3DXMATRIX matComb(m_matCombine);
	float fAlpha(m_fAlpha);

	VERTEXCOLORTEX* pVertices = m_sVertices[m_nBackUp];

	// Note : ���� �ɼǿ� ���� ������ ��� ����� ���־�� �Ѵ�.
	if( m_spParicleEmitter->m_bRandomPlay )
	{
		D3DXMatrixMultiply( &matComb, &m_spParicleEmitter->m_matRandomR, &matComb );
	}

	D3DXMATRIX	matView, matProj, matWVP;
	D3DXMATRIX	matEmitter, matWorld, matRotate, matRotateL, matScale, matTrans, matMesh, matParent, matChild, matIdentity;
	PARTICLE*	pParticleNode;
	D3DXVECTOR3	lookAt;
	D3DXVECTOR3	vDir;
	D3DXVECTOR3	vPos_NowPrev;

	lookAt = D3DXVECTOR3(0.f,0.f,0.f);

	//	Note : ��Ʈ���� ����
	SetMatrix ( matWorld, matComb );

	if ( !(m_rParticleSysSource->m_dwRunFlag&EFF_PLY_PLAY) )		
		return;

	if ( m_rParticleSysSource->m_dwRunFlag&EFF_PLY_END )			
		return;

	if ( m_rParticleSysSource->m_dwRunFlag&EFF_PLY_RENDPASS )		
		return;

	//	Note : ��ü�� ���� ���� ���� ��ƼŬ�� �ణ�� �̵� ������
	//
	if ( (m_matPrevComb._41==0.f) && (m_matPrevComb._42==0.f) && (m_matPrevComb._43==0.f) )
	{
		vPos_NowPrev = D3DXVECTOR3 ( 0.f, 0.f, 0.f );
	}
	else
	{
		vPos_NowPrev = D3DXVECTOR3 ( matComb._41-m_matPrevComb._41, matComb._42-m_matPrevComb._42, matComb._43-m_matPrevComb._43 );
	}

	//	Note : ���ο� ��ƼŬ�� ��� ������ ����.
	//
	NewParticleCreate( matComb, vPos_NowPrev, fElapsedTime );

	if ( m_uParticlesAlive==0 )
		return;

	m_matWorld[m_nBackUp] = matWorld;



	//	Note : �޽� �Ѹ� ���ΰ�, �ƴϸ� ��ƼŬ�� �Ѹ� ���ΰ� ?
	//
	if( sEmitterSetting.GetFlag() & USEMESH )
	{
	}
	else
	{

		int nDrawPoints = 0;

		//	Note : �ؽ��� ���̱⸦ ����Ѵ�.
		//
		if ( sEmitterSetting.GetFlag() & USETEXSCALE )
		{
			D3DXMATRIX matLocal;
			D3DXVECTOR3	vBillPos, vDirc;

			//	Note : ��� ��ǥ�� �� �Ǵ� ���� ��ǥ �� �� ..!!
			//
			if ( (sEmitterSetting.GetFlag()&USEPARENTMOVE) )	
			{
				vBillPos.x = m_rParticleSysSource->GetLocalMatrix()._41;
				vBillPos.y = m_rParticleSysSource->GetLocalMatrix()._42;
				vBillPos.z = m_rParticleSysSource->GetLocalMatrix()._43;
				D3DXVec3TransformCoord ( &vBillPos, &vBillPos, &matWorld );
			}
			else
			{
				vBillPos.x = spThisGroup->m_matWorld._41;
				vBillPos.y = spThisGroup->m_matWorld._42;
				vBillPos.z = spThisGroup->m_matWorld._43;
			}

			if ( sEmitterSetting.GetFlag()&USEBILLBOARDUP )
				DxBillboardLookAtHeight ( matLocal, &vBillPos, matCameraView );		// 
			else if ( sEmitterSetting.GetFlag()&USEGROUND )
				D3DXMatrixBillboardGround( matLocal );
			else
				DxBillboardLookAt ( matLocal, &vBillPos, matCameraView );			//
			matLocal._41 = 0.f;
			matLocal._42 = 0.f;
			matLocal._43 = 0.f;

			VERTEXCOLORTEX	sVertex[4];
			DWORD dwVertexSize( sizeof(VERTEXCOLORTEX)*4 );
			DWORD dwColor(0);
			int nCount00(0);
			int nCount10(0);
			int nCount01(0);
			int nCount11(0);

			float fSize(0.f);
			float fScaleX(0.f);
			float fScaleY(0.f);

			D3DXVECTOR3 vAddPos(0.f,0.f,0.f);

			pParticleNode = m_pParticleHead;
			while ( pParticleNode )
			{
				if( MAX_PARTICLES==nDrawPoints )
					break;

				if ( sEmitterSetting.GetFlag()&USEBILLBOARDALL )
				{
					if ( (sEmitterSetting.GetFlag()&USEPARENTMOVE) )	
					{
						vBillPos = pParticleNode->m_vLocation;
						D3DXVec3TransformCoord ( &vBillPos, &vBillPos, &matWorld );
					}
					else
					{
						vBillPos = pParticleNode->m_vLocation;
					}
					if ( sEmitterSetting.GetFlag()&USEBILLBOARDUP )
						DxBillboardLookAtHeight ( matLocal, &vBillPos, matCameraView );		// 
					else if ( sEmitterSetting.GetFlag()&USEGROUND )
						D3DXMatrixBillboardGround( matLocal );
					else
						DxBillboardLookAt ( matLocal, &vBillPos, matCameraView );			//
					matLocal._41 = 0.f;
					matLocal._42 = 0.f;
					matLocal._43 = 0.f;
				}
				if ( sEmitterSetting.GetFlag()&USEDIRECTION )	// ������ �ִ� ��ü�� (����)�� ������ ��Ʈ������ �������.
				{
					if ( (sEmitterSetting.GetFlag()&USEPARENTMOVE) )	
					{
						D3DXVec3TransformCoord ( &vDirc, &pParticleNode->m_vLocation, &matWorld );
						vDir.x = spThisGroup->m_matWorld._41;
						vDir.y = spThisGroup->m_matWorld._42;
						vDir.z = spThisGroup->m_matWorld._43;
						DxBillboardLookAtDir ( matLocal, &vDirc, &vDir );
					}
					else
					{
						DxBillboardLookAtDir ( matLocal, &pParticleNode->m_vLocation, &pParticleNode->m_vPrevLocation );
					}
				}

				fSize = pParticleNode->m_fSize*0.25f;
				fScaleX = fSize*pParticleNode->m_vTexScale.x;
				fScaleY = fSize*pParticleNode->m_vTexScale.y;

				vAddPos = pParticleNode->m_vLocation;
				dwColor = D3DCOLOR_COLORVALUE ( pParticleNode->m_cColor.x, pParticleNode->m_cColor.y, pParticleNode->m_cColor.z, pParticleNode->m_cColor.w*fAlpha );
				nCount00 = (nDrawPoints*4)+0;


				if ( sEmitterSetting.GetCenterPoint() == 0)	// ��
				{
					D3DXVECTOR3 vLocal = D3DXVECTOR3( -fScaleX, 0.f, 0.f );
					D3DXVec3TransformCoord ( &sVertex[0].vPos, &vLocal, &matLocal );
					vLocal = D3DXVECTOR3( fScaleX, 0.f, 0.f );
					D3DXVec3TransformCoord ( &sVertex[1].vPos, &vLocal, &matLocal );
					vLocal = D3DXVECTOR3( -fScaleX, -fScaleY*2.f, 0.f );
					D3DXVec3TransformCoord ( &sVertex[2].vPos, &vLocal, &matLocal );
					vLocal = D3DXVECTOR3( fScaleX, -fScaleY*2.f, 0.f );
					D3DXVec3TransformCoord ( &sVertex[3].vPos, &vLocal, &matLocal );
				}
				else if ( sEmitterSetting.GetCenterPoint() == 1)	// �߽�
				{
					D3DXVECTOR3 vLocal = D3DXVECTOR3( -fScaleX, fScaleY, 0.f );
					D3DXVec3TransformCoord ( &sVertex[0].vPos, &vLocal, &matLocal );
					vLocal = D3DXVECTOR3( fScaleX, fScaleY, 0.f );
					D3DXVec3TransformCoord ( &sVertex[1].vPos, &vLocal, &matLocal );
					vLocal = D3DXVECTOR3( -fScaleX, -fScaleY, 0.f );
					D3DXVec3TransformCoord ( &sVertex[2].vPos, &vLocal, &matLocal );
					vLocal = D3DXVECTOR3( fScaleX, -fScaleY, 0.f );
					D3DXVec3TransformCoord ( &sVertex[3].vPos, &vLocal, &matLocal );
				}
				else if ( sEmitterSetting.GetCenterPoint() == 2)	// �Ʒ�
				{
					D3DXVECTOR3 vLocal = D3DXVECTOR3( -fScaleX, fScaleY*2.f, 0.f );
					D3DXVec3TransformCoord ( &sVertex[0].vPos, &vLocal, &matLocal );
					vLocal = D3DXVECTOR3( fScaleX, fScaleY*2.f, 0.f );
					D3DXVec3TransformCoord ( &sVertex[1].vPos, &vLocal, &matLocal );
					vLocal = D3DXVECTOR3( -fScaleX, 0.f, 0.f );
					D3DXVec3TransformCoord ( &sVertex[2].vPos, &vLocal, &matLocal );
					vLocal = D3DXVECTOR3( fScaleX, 0.f, 0.f );
					D3DXVec3TransformCoord ( &sVertex[3].vPos, &vLocal, &matLocal );
				}

				if ( sEmitterSetting.GetFlag() & USESEQUENCE )
				{
					int nCol = m_rParticleSysSource->GetParticleSetting().GetCol();
					int nRow = m_rParticleSysSource->GetParticleSetting().GetRow();

					sVertex[0].vPos += vAddPos;
					sVertex[0].dwColor = dwColor;
					sVertex[0].vTex.x = pParticleNode->m_nNowSprite / (float)nCol;
					sVertex[0].vTex.y = (pParticleNode->m_nNowSprite / nCol) / (float)nRow;

					sVertex[1].vPos += vAddPos;
					sVertex[1].dwColor = dwColor;
					sVertex[1].vTex.x = (pParticleNode->m_nNowSprite+1) / (float)nCol;
					sVertex[1].vTex.y = (pParticleNode->m_nNowSprite / nCol) / (float)nRow;

					sVertex[2].vPos += vAddPos;
					sVertex[2].dwColor = dwColor;
					sVertex[2].vTex.x = pParticleNode->m_nNowSprite / (float)nCol;
					sVertex[2].vTex.y = (pParticleNode->m_nNowSprite / nCol) / (float)nRow + 1.f/(float)nRow;

					sVertex[3].vPos += vAddPos;
					sVertex[3].dwColor = dwColor;
					sVertex[3].vTex.x = (pParticleNode->m_nNowSprite+1) / (float)nCol;
					sVertex[3].vTex.y = (pParticleNode->m_nNowSprite / nCol) / (float)nRow + 1.f/(float)nRow;

					memcpy( &pVertices[ nCount00 ], sVertex, dwVertexSize );
				}
				else
				{
					sVertex[0].vPos += vAddPos;
					sVertex[0].dwColor = dwColor;
					sVertex[0].vTex.x = 0.f;
					sVertex[0].vTex.y = 0.f;

					sVertex[1].vPos += vAddPos;
					sVertex[1].dwColor = dwColor;
					sVertex[1].vTex.x = 1.f;
					sVertex[1].vTex.y = 0.f;

					sVertex[2].vPos += vAddPos;
					sVertex[2].dwColor = dwColor;
					sVertex[2].vTex.x = 0.f;
					sVertex[2].vTex.y = 1.f;

					sVertex[3].vPos += vAddPos;
					sVertex[3].dwColor = dwColor;
					sVertex[3].vTex.x = 1.f;
					sVertex[3].vTex.y = 1.f;

					memcpy( &pVertices[ nCount00 ], sVertex, dwVertexSize );
				}

				++nDrawPoints;

				pParticleNode = pParticleNode->m_pNext;
			}
		}
		//	Note : �ؽ��� ȸ�� ��� �Ѵ� ? Yes
		//
		else if ( sEmitterSetting.GetFlag() & USETEXROTATE )
		{
			D3DXVECTOR3	vBillPos;

			//	Note : ��� ��ǥ�� �� �Ǵ� ���� ��ǥ �� �� ..!!
			//
			if ( (sEmitterSetting.GetFlag()&USEPARENTMOVE) )	
			{
				vBillPos.x = m_rParticleSysSource->GetLocalMatrix()._41;
				vBillPos.y = m_rParticleSysSource->GetLocalMatrix()._42;
				vBillPos.z = m_rParticleSysSource->GetLocalMatrix()._43;
				D3DXVec3TransformCoord ( &vBillPos, &vBillPos, &matWorld );
			}
			else
			{
				vBillPos.x = spThisGroup->m_matWorld._41;
				vBillPos.y = spThisGroup->m_matWorld._42;
				vBillPos.z = spThisGroup->m_matWorld._43;
			}

			D3DXMATRIX	matLocal;

			if ( sEmitterSetting.GetFlag()&USEBILLBOARDUP )
				DxBillboardLookAtHeight ( matLocal, &vBillPos, matCameraView );		// ��찡 2������. 
			else if ( sEmitterSetting.GetFlag()&USEGROUND )
				D3DXMatrixBillboardGround( matLocal );
			else
				DxBillboardLookAt ( matLocal, &vBillPos, matCameraView );			//
			matLocal._41 = 0.f;
			matLocal._42 = 0.f;
			matLocal._43 = 0.f;

			VERTEXCOLORTEX	sVertex[4];
			DWORD dwVertexSize( sizeof(VERTEXCOLORTEX)*4 );
			DWORD dwColor(0);
			int nCount00(0);
			int nCount10(0);
			int nCount01(0);
			int nCount11(0);
			float		fSize(0.f);
			float		fTexRotateAngelSum(0.f);
			D3DXVECTOR3 vLocal(0.f,0.f,0.f);
			D3DXVECTOR3 vAddPos(0.f,0.f,0.f);
			D3DXMATRIX	matScale, matFinal;

			D3DXVECTOR3 vPos00( -1.f, 1.f, 0.f );
			D3DXVECTOR3 vPos10( 1.f, 1.f, 0.f );
			D3DXVECTOR3 vPos01( -1.f, -1.f, 0.f );
			D3DXVECTOR3 vPos11( 1.f, -1.f, 0.f );

			pParticleNode = m_pParticleHead;
			while ( pParticleNode )
			{
				if( MAX_PARTICLES==nDrawPoints )
					break;

				if ( sEmitterSetting.GetFlag()&USEBILLBOARDALL )
				{
					if ( (sEmitterSetting.GetFlag()&USEPARENTMOVE) )	
					{
						vBillPos = pParticleNode->m_vLocation;
						D3DXVec3TransformCoord ( &vBillPos, &vBillPos, &matWorld );
					}
					else
					{
						vBillPos = pParticleNode->m_vLocation;
					}
					if ( sEmitterSetting.GetFlag()&USEBILLBOARDUP )
						DxBillboardLookAtHeight ( matLocal, &vBillPos, matCameraView );		// ��찡 2������. 
					else if ( sEmitterSetting.GetFlag()&USEGROUND )
						D3DXMatrixBillboardGround( matLocal );
					else
						DxBillboardLookAt ( matLocal, &vBillPos, matCameraView );			//
					matLocal._41 = 0.f;
					matLocal._42 = 0.f;
					matLocal._43 = 0.f;
				}

				vAddPos = pParticleNode->m_vLocation;
				dwColor = D3DCOLOR_COLORVALUE ( pParticleNode->m_cColor.x, pParticleNode->m_cColor.y, pParticleNode->m_cColor.z, pParticleNode->m_cColor.w*fAlpha );
				nCount00 = (nDrawPoints*4)+0;

				fSize = pParticleNode->m_fSize*0.5f;
				fTexRotateAngelSum = pParticleNode->m_fTexRotateAngelSum;

				D3DXMatrixRotationZ( &matRotate, fTexRotateAngelSum );
				D3DXMatrixScaling( &matScale, fSize, fSize, fSize );
				D3DXMatrixTranslation( &matTrans, vAddPos.x, vAddPos.y, vAddPos.z );
				D3DXMatrixMultiply( &matFinal, &matScale, &matRotate );
				D3DXMatrixMultiply( &matFinal, &matFinal, &matLocal );
				D3DXMatrixMultiply( &matFinal, &matFinal, &matTrans );

				if ( sEmitterSetting.GetFlag() & USESEQUENCE )
				{
					int nCol = m_rParticleSysSource->GetParticleSetting().GetCol();
					int nRow = m_rParticleSysSource->GetParticleSetting().GetRow();

					D3DXVec3TransformCoord ( &sVertex[0].vPos, &vPos00, &matFinal );
					sVertex[0].dwColor = dwColor;
					sVertex[0].vTex.x = pParticleNode->m_nNowSprite / (float)nCol;
					sVertex[0].vTex.y = (pParticleNode->m_nNowSprite / nCol) / (float)nRow;

					D3DXVec3TransformCoord ( &sVertex[1].vPos, &vPos10, &matFinal );
					sVertex[1].dwColor = dwColor;
					sVertex[1].vTex.x = (pParticleNode->m_nNowSprite+1) / (float)nCol;
					sVertex[1].vTex.y = (pParticleNode->m_nNowSprite / nCol) / (float)nRow;

					D3DXVec3TransformCoord ( &sVertex[2].vPos, &vPos01, &matFinal );
					sVertex[2].dwColor = dwColor;
					sVertex[2].vTex.x = pParticleNode->m_nNowSprite / (float)nCol;
					sVertex[2].vTex.y = (pParticleNode->m_nNowSprite / nCol) / (float)nRow + 1.f/(float)nRow;

					D3DXVec3TransformCoord ( &sVertex[3].vPos, &vPos11, &matFinal );
					sVertex[3].dwColor = dwColor;
					sVertex[3].vTex.x = (pParticleNode->m_nNowSprite+1) / (float)nCol;
					sVertex[3].vTex.y = (pParticleNode->m_nNowSprite / nCol) / (float)nRow + 1.f/(float)nRow;

					memcpy( &pVertices[ nCount00 ], sVertex, dwVertexSize );
				}
				else
				{
					D3DXVec3TransformCoord ( &sVertex[0].vPos, &vPos00, &matFinal );
					sVertex[0].dwColor = dwColor;
					sVertex[0].vTex.x = 0.f;
					sVertex[0].vTex.y = 0.f;

					D3DXVec3TransformCoord ( &sVertex[1].vPos, &vPos10, &matFinal );
					sVertex[1].dwColor = dwColor;
					sVertex[1].vTex.x = 1.f;
					sVertex[1].vTex.y = 0.f;

					D3DXVec3TransformCoord ( &sVertex[2].vPos, &vPos01, &matFinal );
					sVertex[2].dwColor = dwColor;
					sVertex[2].vTex.x = 0.f;
					sVertex[2].vTex.y = 1.f;

					D3DXVec3TransformCoord ( &sVertex[3].vPos, &vPos11, &matFinal );
					sVertex[3].dwColor = dwColor;
					sVertex[3].vTex.x = 1.f;
					sVertex[3].vTex.y = 1.f;

					memcpy( &pVertices[ nCount00 ], sVertex, dwVertexSize );
				}

				++nDrawPoints;

				pParticleNode = pParticleNode->m_pNext;
			}
		}
		else
		{
			D3DXVECTOR3	vBillPos;

			//	Note : ��� ��ǥ�� �� �Ǵ� ���� ��ǥ �� �� ..!!
			//
			if ( (sEmitterSetting.GetFlag()&USEPARENTMOVE) )	
			{
				vBillPos.x = m_rParticleSysSource->GetLocalMatrix()._41;
				vBillPos.y = m_rParticleSysSource->GetLocalMatrix()._42;
				vBillPos.z = m_rParticleSysSource->GetLocalMatrix()._43;
				D3DXVec3TransformCoord ( &vBillPos, &vBillPos, &matWorld );
			}
			else
			{
				vBillPos.x = spThisGroup->m_matWorld._41;
				vBillPos.y = spThisGroup->m_matWorld._42;
				vBillPos.z = spThisGroup->m_matWorld._43;
			}

			D3DXMATRIX	matLocal;

			if ( sEmitterSetting.GetFlag()&USEBILLBOARDUP )
				DxBillboardLookAtHeight ( matLocal, &vBillPos, matCameraView );		//
			else if ( sEmitterSetting.GetFlag()&USEGROUND )
				D3DXMatrixBillboardGround( matLocal );
			else
				DxBillboardLookAt ( matLocal, &vBillPos, matCameraView );			//
			matLocal._41 = 0.f;
			matLocal._42 = 0.f;
			matLocal._43 = 0.f;

			VERTEXCOLORTEX	sVertex[4];
			DWORD dwVertexSize( sizeof(VERTEXCOLORTEX)*4 );
			DWORD dwColor(0);
			int nCount00(0);
			int nCount10(0);
			int nCount01(0);
			int nCount11(0);
			float fSize(0.f);
			D3DXVECTOR3 vLocal(0.f,0.f,0.f);
			D3DXVECTOR3 vAddPos(0.f,0.f,0.f);

			D3DXMATRIX	matScale, matFinal;

			D3DXVECTOR3 vPos00( -1.f, 1.f, 0.f );
			D3DXVECTOR3 vPos10( 1.f, 1.f, 0.f );
			D3DXVECTOR3 vPos01( -1.f, -1.f, 0.f );
			D3DXVECTOR3 vPos11( 1.f, -1.f, 0.f );

			pParticleNode = m_pParticleHead;
			while ( pParticleNode )
			{
				if( MAX_PARTICLES==nDrawPoints )
					break;

				if ( sEmitterSetting.GetFlag()&USEBILLBOARDALL )
				{
					if ( (sEmitterSetting.GetFlag()&USEPARENTMOVE) )	
					{
						vBillPos = pParticleNode->m_vLocation;
						D3DXVec3TransformCoord ( &vBillPos, &vBillPos, &matWorld );
					}
					else
					{
						vBillPos = pParticleNode->m_vLocation;
					}
					if ( sEmitterSetting.GetFlag()&USEBILLBOARDUP )
						DxBillboardLookAtHeight ( matLocal, &vBillPos, matCameraView );		//  
					else if ( sEmitterSetting.GetFlag()&USEGROUND )
						D3DXMatrixBillboardGround( matLocal );
					else
						DxBillboardLookAt ( matLocal, &vBillPos, matCameraView );			//
					matLocal._41 = 0.f;
					matLocal._42 = 0.f;
					matLocal._43 = 0.f;
				}

				fSize = pParticleNode->m_fSize*0.25f;

				vAddPos = pParticleNode->m_vLocation;
				dwColor = D3DCOLOR_COLORVALUE ( pParticleNode->m_cColor.x, pParticleNode->m_cColor.y, pParticleNode->m_cColor.z, pParticleNode->m_cColor.w*fAlpha );
				nCount00 = nDrawPoints * 4;

				D3DXMatrixScaling( &matScale, fSize, fSize, fSize );
				D3DXMatrixTranslation( &matTrans, vAddPos.x, vAddPos.y, vAddPos.z );
				D3DXMatrixMultiply( &matFinal, &matScale, &matLocal );
				D3DXMatrixMultiply( &matFinal, &matFinal, &matTrans );

				if ( sEmitterSetting.GetFlag() & USESEQUENCE )
				{
					int nCol = m_rParticleSysSource->GetParticleSetting().GetCol();
					int nRow = m_rParticleSysSource->GetParticleSetting().GetRow();

					D3DXVec3TransformCoord ( &sVertex[0].vPos, &vPos00, &matFinal );
					sVertex[0].dwColor = dwColor;
					sVertex[0].vTex.x = pParticleNode->m_nNowSprite / (float)nCol;
					sVertex[0].vTex.y = (pParticleNode->m_nNowSprite / nCol) / (float)nRow;

					D3DXVec3TransformCoord ( &sVertex[1].vPos, &vPos10, &matFinal );
					sVertex[1].dwColor = dwColor;
					sVertex[1].vTex.x = (pParticleNode->m_nNowSprite+1) / (float)nCol;
					sVertex[1].vTex.y = (pParticleNode->m_nNowSprite / nCol) / (float)nRow;

					D3DXVec3TransformCoord ( &sVertex[2].vPos, &vPos01, &matFinal );
					sVertex[2].dwColor = dwColor;
					sVertex[2].vTex.x = pParticleNode->m_nNowSprite / (float)nCol;
					sVertex[2].vTex.y = (pParticleNode->m_nNowSprite / nCol) / (float)nRow + 1.f/(float)nRow;

					D3DXVec3TransformCoord ( &sVertex[3].vPos, &vPos11, &matFinal );
					sVertex[3].dwColor = dwColor;
					sVertex[3].vTex.x = (pParticleNode->m_nNowSprite+1) / (float)nCol;
					sVertex[3].vTex.y = (pParticleNode->m_nNowSprite / nCol) / (float)nRow + 1.f/(float)nRow;

					memcpy( &pVertices[ nCount00 ], sVertex, dwVertexSize );
				}
				else
				{
					D3DXVec3TransformCoord ( &sVertex[0].vPos, &vPos00, &matFinal );
					sVertex[0].dwColor = dwColor;
					sVertex[0].vTex.x = 0.f;
					sVertex[0].vTex.y = 0.f;

					D3DXVec3TransformCoord ( &sVertex[1].vPos, &vPos10, &matFinal );
					sVertex[1].dwColor = dwColor;
					sVertex[1].vTex.x = 1.f;
					sVertex[1].vTex.y = 0.f;

					D3DXVec3TransformCoord ( &sVertex[2].vPos, &vPos01, &matFinal );
					sVertex[2].dwColor = dwColor;
					sVertex[2].vTex.x = 0.f;
					sVertex[2].vTex.y = 1.f;

					D3DXVec3TransformCoord ( &sVertex[3].vPos, &vPos11, &matFinal );
					sVertex[3].dwColor = dwColor;
					sVertex[3].vTex.x = 1.f;
					sVertex[3].vTex.y = 1.f;

					memcpy( &pVertices[ nCount00 ], sVertex, dwVertexSize );
				}

				++nDrawPoints;

				pParticleNode = pParticleNode->m_pNext;
			}
		}

		// draw �Ǵ� ������ ����Ѵ�.
		m_dwDrawPoints[m_nBackUp] = nDrawPoints;
	}

	//	Note : ������ ��ġ�� ��� �Ѵ�.
	m_matPrevComb = matComb;
}

void ParticlesForThread::Swap( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// �⺻���� �� ����.
	if ( !SingleForThread::IsSwap() )
		return;

	const EmitterBaseSetting& sEmitterSetting = m_rParticleSysSource->GetEmitterSetting();

	// ���� �� �� ���� �۾�
	for ( DWORD i=0; i<m_vecCreateParticles.size(); ++i )
	{
		m_vecCreateParticles[i]->m_pNext = m_pParticleHead;
		m_pParticleHead = m_vecCreateParticles[i];
	}

	// Note : EffGroup�� �ִٸ� �����Ѵ�.
	if ( sEmitterSetting.GetFlag() & USENEWEFF )
	{
		DxEffectParticleSys* pParticleSysSource_NoneConst = const_cast<DxEffectParticleSys*>(m_rParticleSysSource);
		if( pParticleSysSource_NoneConst->m_PropGroupRes.Update() )
		{
			for ( DWORD i=0; i<m_vecCreateParticles.size(); ++i )
			{
				m_vecCreateParticles[i]->CreateSingleGroup_MainThread( pd3dDevice, pParticleSysSource_NoneConst->m_PropGroupRes, sEmitterSetting );
			}
		}
	}

	// ���� �� �� ���� �۾�.
	PARTICLE* pParticleNode = NULL;
	PARTICLE* pParticlePrev = NULL;
	for ( DWORD i=0; i<m_vecDeleteParticles.size(); ++i )
	{
		pParticleNode = m_vecDeleteParticles[i];

		//	Note : �Ҹ��ϴ� ��ƼŬ Pool�� ���.
		PutSleepParticle ( pParticleNode );
	}

	m_vecCreateParticles.clear();
	m_vecDeleteParticles.clear();
}

DWORD ParticlesForThread::GetNumDrawPoints() const
{
	return m_dwDrawPoints[m_nUSED];
}

const VERTEXCOLORTEX* ParticlesForThread::GetUsedVertices() const
{
	return &m_sVertices[m_nUSED][0];
}

const D3DXMATRIX& ParticlesForThread::GetMatWorld() const
{
	return m_matWorld[m_nUSED];
}

BOOL ParticlesForThread::IsParticlesAlive() const
{
	return m_pParticleHead ? TRUE : FALSE;
}

const PARTICLE*	ParticlesForThread::GetParticleHead() const
{
	return m_pParticleHead;
}
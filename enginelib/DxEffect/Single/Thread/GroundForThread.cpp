#include "pch.h"

#include "../DxEffSingleMan.h"

#include "GroundForThread.h"





//////////////////////////////////////////////////////////////////////////
//							GroundForThread
//////////////////////////////////////////////////////////////////////////
GroundForThread::GroundForThread( boost::shared_ptr<DxEffSingleGroup>& spThisGroup, const DxEffectGround* pGroundSource )
	: SingleForThread( spThisGroup )
	, m_rGroundSource(pGroundSource)
	, m_bCollisionOK(FALSE)
	, m_vCheckPoint(0.f,0.f,0.f)
	, m_fHeight(0.f)
{
}

GroundForThread::~GroundForThread()
{
}


//////////////////////////////////////////////////////////////////////////
// Effect Thread
void GroundForThread::FrameMove( float fElapsedTime )
{
	if ( m_bCollisionOK )
	{
		if ( m_vCheckPointPrev == m_vCheckPoint )
			return;
	}

	boost::shared_ptr<DxLandMan> spLandMan = DxEffSingleMan::GetInstance().GetLandMan();
	if ( spLandMan )
	{
		BOOL		bCollision(FALSE);
		D3DXVECTOR3	vCollision(m_vCheckPoint);
		D3DXVECTOR3	vPoint1(m_vCheckPoint);
		D3DXVECTOR3	vPoint2(m_vCheckPoint);
		vPoint1.y += 5.f;
		vPoint2.y -= 100.f;
		LPDXFRAME	pDxFrame = NULL;

		spLandMan->IsCollision ( vPoint1, vPoint2, vCollision, bCollision, pDxFrame, EMCC_CULL_CCW );

		if ( bCollision )
		{
			m_fHeight = vCollision.y + 0.3f;

			m_vCheckPointPrev = m_vCheckPoint;
			m_bCollisionOK = TRUE;
		}
	}
}
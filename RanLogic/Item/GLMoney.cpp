#include "../pch.h"
#include "../../EngineLib/DxTools/Collision.h"
#include "../../EngineLib/DxTools/DxCustomTypes.h"
#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../EngineLib/DxMeshs/SkinMesh/NSSkinAniThreadDef.h"
#include "../../EngineLib/DxMeshs/SkinMesh/NSSkinAniThread.h"
#include "../../EngineLib/DxMeshs/DxSimpleMeshMan.h"
#include "../../EngineLib/GlobalParam.h"

#include "../Item/GLItemMan.h"
#include "../GLogicData.h"
#include "./GLMoney.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

bool CMoneyDrop::IsTakeItem(const GLPartyID& PartyID, const DWORD dwGaeaID)
{
	bool bhold(false);

	switch ( emGroup )
	{
	case EMGROUP_ONE:
		if ( dwHoldGID == GAEAID_NULL )
            bhold = true;
		else if ( dwHoldGID == dwGaeaID )
            bhold = true;
		break;

	case EMGROUP_PARTY:
		if ( (PartyID == dwHoldGID) || (dwHoldGID == PARTY_NULL) )
            bhold = true;
		break;

	case EMGROUP_GUILD:
		break;
	};

	return bhold;
}

bool CMoneyDrop::IsDropOut()
{
	return ( GLCONST_CHAR::wMAXITEM_AGE < fAge );
}

void CMoneyDrop::Update ( float fElapsedTime )
{
	fAge += fElapsedTime;

	//	Note : 임시 소유자 유효성 검사.
	//
	if ( dwHoldGID!=GAEAID_NULL )
	{
		if ( GLCONST_CHAR::wMAXITEM_HOLD < fAge )
		{
			switch ( emGroup )
			{
			case EMGROUP_ONE:
				dwHoldGID = GAEAID_NULL;
				break;

			case EMGROUP_PARTY:
				dwHoldGID = PARTY_NULL;
				break;

			case EMGROUP_GUILD:
				break;
			};
		}
	}	
}

//////////////////////////////////////////////////////////////////////////
//						CMoneyClientDrop
CMoneyClientDrop::CMoneyClientDrop(void)
	: fAge(0.0f)
	, fNextMsgDelay(0.0f)
	, pMoneyMesh(NULL)
	, bCanGet(FALSE)
	, m_spNameCollision(new ActorNameCollision(EMCC_CULL_NONE))
{
}

bool CMoneyClientDrop::IsCollision ( const D3DXVECTOR3 &vFromPt, const D3DXVECTOR3 &vTargetPt ) const
{
	DxSimMesh* pMoneyMesh = NULL;
	if (lnAmount > 200)
        pMoneyMesh = GLItemMan::GetInstance().GetMoneyMesh ( 2 );
	else if (lnAmount > 60)
        pMoneyMesh = GLItemMan::GetInstance().GetMoneyMesh ( 1 );
	else
        pMoneyMesh = GLItemMan::GetInstance().GetMoneyMesh ( 0 );
	if (!pMoneyMesh)
        return false;

	D3DXVECTOR3 vMax, vMin;
	pMoneyMesh->GetBoundBox ( vMax, vMin );
	COLLISION::MINDETECTAABB ( vMax, vMin, 1.1f, 4.0f );

	COLLISION::TransformAABB ( vMax, vMin, matWld );

	BOOL bCol = COLLISION::IsCollisionLineToAABB ( vFromPt, vTargetPt, vMax, vMin );
	return FALSE != bCol;
}

bool CMoneyClientDrop::IsCollision ( const CLIPVOLUME &cv ) const
{
	DxSimMesh* pMoneyMesh = NULL;
	if (lnAmount > 200)
        pMoneyMesh = GLItemMan::GetInstance().GetMoneyMesh ( 2 );
	else if (lnAmount > 60)
        pMoneyMesh = GLItemMan::GetInstance().GetMoneyMesh ( 1 );
	else
        pMoneyMesh = GLItemMan::GetInstance().GetMoneyMesh ( 0 );
	if (!pMoneyMesh)
        return false;

	D3DXVECTOR3 vMax, vMin;
	pMoneyMesh->GetBoundBox ( vMax, vMin );
	COLLISION::MINDETECTAABB ( vMax, vMin, 1.1f, 4.0f );

	COLLISION::TransformAABB ( vMax, vMin, matWld );

	BOOL bCol = COLLISION::IsCollisionVolume ( cv, vMax, vMin );
	return FALSE != bCol;
}

void CMoneyClientDrop::FrameMove( const boost::shared_ptr<DxLandMan>& spLandMan, float fElapsedTime )
{
	if (fNextMsgDelay > 0.0f)
		fNextMsgDelay -= fElapsedTime;

	//////////////////////////////////////////////////////////////////////////
	//			Name Collision 을 Thread 상에서 확인하도록 유도한다.
	// Thread 상으로 넘긴다.
	m_spNameCollision->InitData( spLandMan );
	NSSkinAniThread::AddNameCollision( m_spNameCollision, DxViewPort::GetInstance().GetFromPt(), vPos + D3DXVECTOR3(0.0f, 8.0f, 0.0f) );
}

HRESULT CMoneyClientDrop::RenderItem ( LPDIRECT3DDEVICEQ pd3dDevice, D3DMATERIALQ* pMaterials )
{
    if( !GLOBAL_PARAM::IsValue( GP_CHAR_MOB_ITEM ) )
		return S_OK;

	if ( pMoneyMesh )
	{
		pMoneyMesh->RenderItem ( pd3dDevice, matWld, pMaterials );
	}

	return S_OK;
}

BOOL CMoneyClientDrop::IsVisibleDisplayName() const
{
	if ( m_spNameCollision->GetCollsion() )
		return FALSE;

	return TRUE;
}

#include "pch.h"

#include "../GLGaeaClient.h"
#include "./GLFlyCameraBaseMode.h"
#include "./GLFlyCameraControl.h"

GLFlyCameraBaseMode::GLFlyCameraBaseMode( GLGaeaClient* pGaeaClient, GLFlyCameraControl* pParentCtrl, DWORD dwModeType )
: m_pGaeaClient ( pGaeaClient )
, m_pParentCtrl ( pParentCtrl )
, m_dwModeType ( dwModeType )
, m_vReservMove ( 0.0f, 0.0f, 0.0f )
, m_vReservRotate ( 0.0f, 0.0f, 0.0f )
{
}

GLFlyCameraBaseMode::~GLFlyCameraBaseMode(void)
{
}

D3DXVECTOR3 GLFlyCameraBaseMode::GetReserveMove(void) 
{
	D3DXVECTOR3 vGet = m_vReservMove;
	m_vReservMove = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	return vGet; 
}
D3DXVECTOR3 GLFlyCameraBaseMode::GetReserveRotation(void) 
{
	D3DXVECTOR3 vGet = m_vReservRotate;
	m_vReservRotate = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	return vGet; 
}
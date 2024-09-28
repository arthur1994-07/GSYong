
#pragma once

#include "../../enginelib/G-Logic/GLDefine.h"
#include "./GLFlyCameraBaseMode.h"

class GLFlyCameraFollowMode : public GLFlyCameraBaseMode
{
	// Fixed -> Follow ÀüÈ¯½Ã;
private:
	//float	m_fInterpT;
	float	m_fAccumT;

	// Follow Control;
private:
	STARGETID		m_sTargetID;
	
	D3DXVECTOR3		m_vTargetPosition;
	D3DXVECTOR3		m_vPrevPosition;

public:
	GLFlyCameraFollowMode( GLGaeaClient* pGaeaClient, GLFlyCameraControl* pParentCtrl );
	virtual ~GLFlyCameraFollowMode();

public:
	void Init(void) override;
	void MsgProcedures ( DWORD dwType, DWORD dwMsg ) override;
	void Update( float fTime, float fElapsedTime ) override;

	const STARGETID& GetTarget(void)	{ return m_sTargetID; }

private:
	void FollowModeMove(const D3DXVECTOR3& vMove);
	BOOL SetSelectTarget( const DWORD dwGaeaID );
};
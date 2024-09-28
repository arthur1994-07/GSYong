#pragma once

#include "../../enginelib/DxEffect/Single/DxEffSingleMan.h"

class GLGaeaClient;

class GLCTFlagEffectMan
{
public :
	GLCTFlagEffectMan( GLGaeaClient* pClient );
	~GLCTFlagEffectMan();

protected :
	LPDIRECT3DDEVICEQ m_pDevice;
	GLGaeaClient* m_pClient;

	SPDXEFFSINGLEGROUP m_spRedFlagArrowEffect;
	SPDXEFFSINGLEGROUP m_spBlueFlagArrowEffect;

	float m_fTime;
	STARGETID m_vRedFlagGuideArrowTarget;
	STARGETID m_vBlueFlagGuideArrowTarget;

	bool m_bBlinkRedFlag;
	bool m_bBlinkBlueFlag;
	float m_fBlinkTimer;
	bool m_bBlinkState;

	bool m_bVisibleRedFlag;
	bool m_bVisibleBlueFlag;

	bool m_bUse;

public :
	void OnInit( LPDIRECT3DDEVICEQ pDevice );
	void OnFrameMove( float fElapsedTime );
	void OnFrameRedner();
	void OnReset ( void );

public :
	void SetFlagGuideArrow( const int nFlagType, const STARGETID& sTargetID, const bool bBlink );
};

#ifndef DXPARAMSET_H_
#define DXPARAMSET_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../EngineLib/DxCommon/D3DApp.h"

namespace DXPARAMSET
{
	struct STESTSET
	{
		bool			m_bDIS_CONFT;		//	'�ڵ�' ��� �ź�.
		bool			m_bDIS_TRADE;		//	'�ڵ�' �ŷ� �ź�.
		bool			m_bDIS_PARTY;		//	'�ڵ�' ��Ƽ �ź�.
		bool			m_bSHOW_SIMPLEHP;	//	������ ��ǥ��
		bool			m_bSIMPLE_SUM_NAME; //  ������ ��ȯ�� �̸�ǥ��
        bool            m_bAUTO_CROW_HP;    //  '�ڵ�' �ڽ��� Ÿ������ ���� HP�� ǥ��
//		bool			m_bDIS_FRIEND;		//	'�ڵ�' ģ�� ��û �ź�.

		DWORD			m_dwScrWidth;
		DWORD			m_dwScrHeight;
		EMSCREEN_FORMAT	m_emScrFormat;

		UINT			m_uScrRefreshHz;
		bool			m_bScrWindowed;

		DWORD			m_dwVideoLevel;
		DWORD			m_dwShadowChar;
		bool			m_bBuff;
		bool			m_bRealReflect;
		DWORD			m_dwSkinDetail;
		bool			m_bGlow;
		bool			m_bPost;
		bool			m_bRefract;
		bool			m_bFrameLimit;
		bool			m_bTargetEffect;
		bool			m_bMineEffect;
		bool			m_bClickEffect;
		bool			m_bUseNormalMap;
        bool            m_bVolumeLight;
		bool			m_bPossibleNormalMap;

		DWORD			m_dwFogRange;
		DWORD			m_dwTexFilter;
		DWORD			m_dwTexQuality;
		DWORD			m_dwPhysXCloth;

		STESTSET () :
			m_bDIS_CONFT(false),
			m_bDIS_TRADE(false),
			m_bDIS_PARTY(false),
			m_bSHOW_SIMPLEHP(false),

			m_dwScrWidth(800),
			m_dwScrHeight(600),
			m_emScrFormat(EMSCREEN_F16),
			m_uScrRefreshHz(60),
			m_bScrWindowed(false),

			m_dwShadowChar(4),
			m_bBuff(true),
			m_bRealReflect(true),
			m_dwSkinDetail(2),
			m_bGlow(true),
			m_bPost(true),
			m_bRefract(true),
			m_bFrameLimit(true),
			m_bTargetEffect(true),
			m_bMineEffect(true),
			m_bClickEffect(true),
			m_bUseNormalMap(true),
			m_dwFogRange(0)
			, m_dwTexFilter(0)
			, m_dwTexQuality(2)
			, m_dwPhysXCloth(2)
		{
		};

		void SetShadowChar( DWORD dwShadowChar );
		void SetSkinDetail( DWORD dwSkinDetail );
		void SetVideoLevel( DWORD dwVideoLevel );
		void VideoOK(CD3DApplication *pD3dApp, const SUBPATH* pPath, EMSERVICE_PROVIDER ServiceProvider);
		void GameOK(const SUBPATH* pPath, EMSERVICE_PROVIDER ServiceProvider);
		void AudioOK(const SUBPATH* pPath, EMSERVICE_PROVIDER ServiceProvider);
		void CANCEL( CD3DApplication *pD3dApp );

		void VideoOKFromWidget();
		void GameOKFromWidget();
		void AudioOKFromWidget();

	private:
		void APPLY ( CD3DApplication *pD3dApp );
	};

	extern STESTSET& GetInstance();

	void	INIT ( );
	void	LOADDATA ();
};

#endif // DXPARAMSET_H_
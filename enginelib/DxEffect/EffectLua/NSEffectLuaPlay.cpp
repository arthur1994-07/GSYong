#include "pch.h"

#include <list>

#include "../../DxTools/DxViewPort.h"
#include "../EffProj/DxEffProjBase.h"
#include "../EffProj/DxEffProjTexture.h"
#include "../EffProj/DxEffProjTextureShader.h"
#include "../Single/DxEffGroupPlayer.h"
#include "../DxPostEffect.h"
#include "./NSEffectLua.h"

#include "NSEffectLuaPlay.h"

class DxEffProjTexture;

//////////////////////////////////////////////////////////////////////////
//	NSEffectLua::g_mapEffects ���� �����Ͽ� ����Ѵ�.
//	���� ���� reset �ȴٸ�, ���� �������ϴ� �����鵵 ��� reset ���ֵ��� �Ѵ�.
//////////////////////////////////////////////////////////////////////////
namespace NSEffectPlay
{
	// EffProj ���� �������� ������ �ִ�.
	// FrameMove�� Rendering �� �̰� �������� �Ѵ�.
	std::list<boost::shared_ptr<DxEffProjBase>>		g_listEffProjBase;



	//////////////////////////////////////////////////////////////////////////
	//								DATA
	struct DATA
	{
		const NSEffectLua::STEP_DATA*		m_rStepData;	// �������̴�. ���� ���ǰ� �ʿ��ϴ�.
		int									m_nIndex;

		STARGETID							m_sTargetID;		// egp ����. TargetID
		BOOL								m_bEgpNeverDie;		// egp ����.

		float								m_fTime;
		BOOL								m_bNextStepON;			// ���� step �� �����°�~?
		boost::shared_ptr<DxEffProjBase>	m_spEffProjBase;		// shared_ptr �� �ؼ� m_listEffProj �� �����ϰ� ���ش�. EnableEFF �� �ϱ� ���� ������ �ִ´�.

		// FrameMove
		void FrameMove( float ElapsedTime )
		{
			m_fTime += ElapsedTime;
		}

		// ���� step �� �۵��Ǿ� �Ѵٸ�, ���� step index �� �Ѱ��ش�.
		int IsCreateNexStep()
		{
			// ���� Step �� ON �Ǿ��ٸ� FALSE �� �༭ ���� STEP �� ������ �ʰ� �Ѵ�.
			if ( m_bNextStepON )
				return -1;

			if ( m_fTime >= m_rStepData->m_fNextStepTime )
			{
				m_bNextStepON = TRUE;
				return m_nIndex+1;
			}
			return -1;
		}

		// ���� �ұ�~?
		BOOL IsEnd() const
		{
			if ( m_rStepData->m_emEndCondition == NSEffectLua::END_CONDITION_AUTO )
			{
				if ( m_fTime >= m_rStepData->m_fNextStepTime )
				{
					return TRUE;
				}
			}
			return FALSE;
		}

		void Start( LPDIRECT3DDEVICEQ pd3dDevice, const D3DXMATRIX &matStart, const STARGETID &sTargetID )
		{
			if ( !m_rStepData->m_strEgpFile.empty() )
			{
				m_sTargetID = sTargetID;
				SPDXEFFSINGLEGROUP spSingleGroup;

				if ( m_rStepData->m_bEgpInFrontOfEye )
				{
					// ����(ī�޶��)���� �������� ���۵ǵ��� �Ѵ�.
					const D3DXVECTOR3& vFromPt = DxViewPort::GetInstance().GetFromPt();
					const D3DXVECTOR3& vLookDir = DxViewPort::GetInstance().GetLookDir();

					D3DXMATRIX matTransNew;
					D3DXMatrixIdentity( &matTransNew );
					matTransNew._41 = vFromPt.x + vLookDir.x * 6.f;
					matTransNew._42 = vFromPt.y + vLookDir.y * 6.f;
					matTransNew._43 = vFromPt.z + vLookDir.z * 6.f;

					spSingleGroup = DxEffGroupPlayer::GetInstance().NewEffGroup( m_rStepData->m_strEgpFile.c_str(), matTransNew, &sTargetID, FALSE, TRUE );
				}
				else
				{
					spSingleGroup = DxEffGroupPlayer::GetInstance().NewEffGroup( m_rStepData->m_strEgpFile.c_str(), matStart, &sTargetID, FALSE, TRUE );
				}

				// ���� ���� ����.
				if ( spSingleGroup.get() && spSingleGroup->IsFlag( EFF_CFG_NEVERDIE ) )
				{
					m_bEgpNeverDie = TRUE;
				}
			}

			if ( m_rStepData->m_emScreenEffectType == NSEffectLua::SCREENEFFECT_BASE_TEXTURE )
			{
				DxEffProjBase* pEffProjBase(NULL);
				if ( m_rStepData->m_strScreenEffectShader.empty() )
				{
					pEffProjBase = new DxEffProjTexture
					( 
						m_rStepData->m_strScreenEffectTexture.c_str(),
						m_rStepData->m_fScreenEffectCreatingTime,
						m_rStepData->m_fScreenEffectDestroyingTime,
						TRUE
					);
				}
				else
				{
					pEffProjBase = new DxEffProjTextureShader
					( 
						m_rStepData->m_strScreenEffectTexture.c_str(),
						m_rStepData->m_strScreenEffectShader.c_str(),
						m_rStepData->m_fScreenEffectCreatingTime,
						m_rStepData->m_fScreenEffectDestroyingTime,
						TRUE
					);
				}
				m_spEffProjBase = boost::shared_ptr<DxEffProjBase>( pEffProjBase );
				m_spEffProjBase->OnCreateDevice( pd3dDevice );
				m_spEffProjBase->OnResetDevice( pd3dDevice );
				
				// �����Ǵ� ���� �ִ´�.
				g_listEffProjBase.push_back( m_spEffProjBase );
				DxPostEffect::GetInstance().Check_SetDeactivePostSurface();
			}
		}

		void Delete()
		{
			if ( m_rStepData->m_emEndCondition == NSEffectLua::END_CONDITION_AUTO )
			{
				m_fTime = m_rStepData->m_fNextStepTime;
			}
			else if ( m_rStepData->m_emEndCondition == NSEffectLua::END_CONDITION_NONE )
			{
				if ( m_spEffProjBase.get() )
				{
					m_spEffProjBase->EnableEFF( FALSE );
				}
			}
		}

		DATA( LPDIRECT3DDEVICEQ pd3dDevice, const NSEffectLua::STEP_DATA* pStepData, int nIndex, const D3DXMATRIX &matTrans, const STARGETID &sTargetID )
			: m_rStepData(pStepData)
			, m_nIndex(nIndex)
			, m_fTime(0.f)
			, m_bNextStepON(FALSE)
			, m_bEgpNeverDie(FALSE)
		{
			Start( pd3dDevice, matTrans, sTargetID );
		}

		~DATA()
		{
			if ( m_bEgpNeverDie )
			{
				DxEffGroupPlayer::GetInstance().DeletePassiveEffect( m_rStepData->m_strEgpFile.c_str(), m_sTargetID );
			}
			else
			{
				// ���� DeletePassiveEffect �� �۾��� �ȴٸ�, ��ƼŬ �������� �۾� �� ������ �ʾҴµ� ����� ���� �ִ�.
				// �׷��� else ������ �ƹ� �۾��� ���� �ʴ´�.
			}
		}
	};



	//////////////////////////////////////////////////////////////////////////
	//							DATA_MAIN


	struct DATA_MAIN
	{
		const NSEffectLua::VEC_STEP_DATA*	m_rvecStep;	// �������̴�. ���� ���ǰ� �ʿ��ϴ�.
		std::list<boost::shared_ptr<DATA>>	m_listData;
		D3DXMATRIX							m_matTrans;
		STARGETID							m_sTargetID;

		void FrameMove( LPDIRECT3DDEVICEQ pd3dDevice, float ElapsedTime )
		{
			if ( m_listData.empty() )
				return;

			std::list<boost::shared_ptr<DATA>>::iterator iter = m_listData.begin();
			for ( ; iter != m_listData.end(); )
			{
				(*iter)->FrameMove( ElapsedTime );

				// ���ο� Step �� ��������� �Ѵٸ� �����ǰ� �Ѵ�.
				int nNextStep = (*iter)->IsCreateNexStep();
				if ( nNextStep != -1 )
				{
					if ( nNextStep < static_cast<int>( m_rvecStep->size() ) )
					{
						boost::shared_ptr<DATA> spData( new DATA( pd3dDevice, (*m_rvecStep)[nNextStep].get(), nNextStep, m_matTrans, m_sTargetID ) );
						m_listData.push_back( spData );
					}
				}

				// ���� �ȴٸ�..
				if ( (*iter)->IsEnd() )
				{
					iter = m_listData.erase( iter );
				}
				else
				{
					++iter;
				}
			}
		}

		void Delete()
		{
			std::list<boost::shared_ptr<DATA>>::iterator iter = m_listData.begin();
			for ( ; iter != m_listData.end(); ++iter )
			{
				(*iter)->Delete();
			}
		}

		DATA_MAIN( LPDIRECT3DDEVICEQ pd3dDevice, const NSEffectLua::VEC_STEP_DATA* pvecStep, const D3DXMATRIX &matTrans, const STARGETID &sTargetID )
			: m_rvecStep(pvecStep)
			, m_matTrans(matTrans)
			, m_sTargetID(sTargetID)
		{
			boost::shared_ptr<DATA> spData( new DATA( pd3dDevice, (*pvecStep)[0].get(), 0, matTrans, sTargetID ) );
			m_listData.push_back( spData );
		}

		~DATA_MAIN()
		{
			m_listData.clear();
		}
	};



	//////////////////////////////////////////////////////////////////////////
	//							g_mapPlayData
	std::map<TSTRING,DATA_MAIN>	g_mapPlayData;





	//////////////////////////////////////////////////////////////////////////
	// RanLogicClient ���� ����ϸ� ��.
	void StartPlay( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const D3DXMATRIX &matTrans, const STARGETID &sTargetID )
	{
		const NSEffectLua::VEC_STEP_DATA* pvecStep = NSEffectLua::Find( szName );
		if ( !pvecStep )
			return;

		// ���� ������ ��ϵ��� �ʴ´�.
		if ( (*pvecStep).empty() )
			return;

		// ������ �ִٸ� �� ������� �ʴ´�.
		std::map<TSTRING,DATA_MAIN>::const_iterator citer = g_mapPlayData.find( szName );
		if ( citer != g_mapPlayData.end() )
		{
			return;
		}

		// ����Ѵ�.
		DATA_MAIN sDataMain( pd3dDevice, pvecStep, matTrans, sTargetID );
		g_mapPlayData.insert( std::make_pair(szName,sDataMain) );
	}

	void Delete( const char* szName )
	{
		std::map<TSTRING,DATA_MAIN>::iterator iter = g_mapPlayData.find( szName );
		if ( iter != g_mapPlayData.end() )
		{
			(*iter).second.Delete();
			g_mapPlayData.erase( iter );
		}
	}
	// RanLogicClient ���� ����ϸ� ��.
	//////////////////////////////////////////////////////////////////////////





	//////////////////////////////////////////////////////////////////////////
	// Engine ���� ����ϸ� ��.
	void DeleteALL()
	{
		// �������� ��� �����Ƿ� �ٷλ���.
		{
			std::map<TSTRING,DATA_MAIN>::iterator iter = g_mapPlayData.begin();
			for ( ; iter!=g_mapPlayData.end(); ++iter )
			{
				(*iter).second.Delete();
			}
			g_mapPlayData.clear();
		}

		{
			std::list<boost::shared_ptr<DxEffProjBase>>::iterator iter = g_listEffProjBase.begin();
			for ( ; iter!=g_listEffProjBase.end(); ++iter )
			{
				(*iter)->OnLostDevice();
				(*iter)->OnDestroyDevice();
			}
			g_listEffProjBase.clear();
			DxPostEffect::GetInstance().Check_SetDeactivePostSurface();

			// fx ���� ����
			DxEffProjTextureShader::DeleteCustomFX_STATIC();
		}
	}

	void OnResetDevice( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		DxEffProjTexture::OnResetDevice_STATIC( pd3dDevice );
		DxEffProjTextureShader::OnResetDevice_STATIC( pd3dDevice );

		std::list<boost::shared_ptr<DxEffProjBase>>::iterator iter = g_listEffProjBase.begin();
		for ( ; iter!=g_listEffProjBase.end(); ++iter )
		{
			(*iter)->OnResetDevice( pd3dDevice );
		}
	}

	void OnLostDevice()
	{
		std::list<boost::shared_ptr<DxEffProjBase>>::iterator iter = g_listEffProjBase.begin();
		for ( ; iter!=g_listEffProjBase.end(); ++iter )
		{
			(*iter)->OnLostDevice();
		}

		DxEffProjTexture::OnLostDevice_STATIC();
		DxEffProjTextureShader::OnLostDevice_STATIC();
	}

	void OnDestroyDevice()
	{
		std::list<boost::shared_ptr<DxEffProjBase>>::iterator iter = g_listEffProjBase.begin();
		for ( ; iter!=g_listEffProjBase.end(); ++iter )
		{
			(*iter)->OnDestroyDevice();
		}
		g_listEffProjBase.clear();
		g_mapPlayData.clear();
		DxPostEffect::GetInstance().Check_SetDeactivePostSurface();
	}

	BOOL IsTextureShader()
	{
		std::list<boost::shared_ptr<DxEffProjBase>>::iterator iter = g_listEffProjBase.begin();
		for ( ; iter!=g_listEffProjBase.end(); ++iter )
		{
			if ( (*iter)->IsTextureShader() )
				return TRUE;
		}
		return FALSE;
	}

	void FrameMove( LPDIRECT3DDEVICEQ pd3dDevice, float fElapsedTime )
	{
		// DATA_MAIN ��
		{
			std::map<TSTRING,DATA_MAIN>::iterator iter = g_mapPlayData.begin();
			for ( ; iter!=g_mapPlayData.end(); ++iter )
			{
				// FrameMove
				(*iter).second.FrameMove( pd3dDevice, fElapsedTime );
			}
		}

		// DxEffProjBase
		{
			// FrameMove
			std::list<boost::shared_ptr<DxEffProjBase>>::iterator iter = g_listEffProjBase.begin();
			for ( ; iter!=g_listEffProjBase.end(); ++iter )
			{
				(*iter)->FrameMove( fElapsedTime );
			}

			// erase
			iter = g_listEffProjBase.begin();
			for ( ; iter!=g_listEffProjBase.end(); )
			{
				if ( (*iter)->IsPlayOFF() )
				{
					iter = g_listEffProjBase.erase( iter );
					DxPostEffect::GetInstance().Check_SetDeactivePostSurface();
				}
				else
				{
					++iter;
				}
			}
		}
	}

	// ������ ������ �� ���� �� ȿ���� ȭ�� ���� ȿ���� FX �� �۾��Ͽ� BackBuffer �� ������ �ǰ� �Ѵ�.
	void RenderSourceBackBuffer( LPDIRECT3DDEVICEQ pd3dDevice, 
									LPDIRECT3DSURFACEQ pSurface1,
									LPDIRECT3DTEXTUREQ pTexture1, 
									LPDIRECT3DSURFACEQ pSurface2, 
									LPDIRECT3DTEXTUREQ pTexture2, 
									LPDIRECT3DSURFACEQ& pCurrentSurfaceOUT )	// pCurrentSurface �� ���� �� ���� �޾Ƽ� ������ �Ѽս�Ű�� �ʵ��� �Ѵ�.
																				// ó�� �Է¹޴� ���� pSurface1 �� ������ ���� ����ϵ��� �Ѵ�.
	{
		LPDIRECT3DSURFACEQ pSurface(NULL); 
		LPDIRECT3DTEXTUREQ pTexture(NULL);

		std::list<boost::shared_ptr<DxEffProjBase>>::iterator iter = g_listEffProjBase.begin();
		for ( ; iter!=g_listEffProjBase.end(); ++iter )
		{
			if ( pCurrentSurfaceOUT == pSurface1 )
			{
				pSurface = pSurface2;
				pTexture = pTexture1;
			}
			else
			{
				pSurface = pSurface1;
				pTexture = pTexture2;
			}

			if ( (*iter)->RenderSourceBackBuffer( pd3dDevice, pSurface, pTexture ) )
			{
				// ���Ǿ��� Surface �� �����Ѵ�.
				pCurrentSurfaceOUT = pSurface;
			}
		}
	}

	// BackBuffer �� Screen ȿ���� ������ �ϵ��� �Ѵ�.
	void RenderPostEffect( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		std::list<boost::shared_ptr<DxEffProjBase>>::iterator iter = g_listEffProjBase.begin();
		for ( ; iter!=g_listEffProjBase.end(); ++iter )
		{
			(*iter)->RenderPostEffect( pd3dDevice );
		}
	}
	// Engine ���� ����ϸ� ��.
	//////////////////////////////////////////////////////////////////////////
};
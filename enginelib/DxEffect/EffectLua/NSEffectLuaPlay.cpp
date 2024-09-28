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
//	NSEffectLua::g_mapEffects 값을 참조하여 사용한다.
//	위에 값이 reset 된다면, 지금 렌더링하는 정보들도 모두 reset 해주도록 한다.
//////////////////////////////////////////////////////////////////////////
namespace NSEffectPlay
{
	// EffProj 관련 정보들을 가지고 있다.
	// FrameMove와 Rendering 도 이걸 바탕으로 한다.
	std::list<boost::shared_ptr<DxEffProjBase>>		g_listEffProjBase;



	//////////////////////////////////////////////////////////////////////////
	//								DATA
	struct DATA
	{
		const NSEffectLua::STEP_DATA*		m_rStepData;	// 참조값이다. 사용시 주의가 필요하다.
		int									m_nIndex;

		STARGETID							m_sTargetID;		// egp 관련. TargetID
		BOOL								m_bEgpNeverDie;		// egp 관련.

		float								m_fTime;
		BOOL								m_bNextStepON;			// 다음 step 이 켜졌는가~?
		boost::shared_ptr<DxEffProjBase>	m_spEffProjBase;		// shared_ptr 로 해서 m_listEffProj 과 동일하게 해준다. EnableEFF 를 하기 위해 가지고 있는다.

		// FrameMove
		void FrameMove( float ElapsedTime )
		{
			m_fTime += ElapsedTime;
		}

		// 다음 step 이 작동되야 한다면, 다음 step index 를 넘겨준다.
		int IsCreateNexStep()
		{
			// 다음 Step 이 ON 되었다면 FALSE 를 줘서 새로 STEP 을 만들지 않게 한다.
			if ( m_bNextStepON )
				return -1;

			if ( m_fTime >= m_rStepData->m_fNextStepTime )
			{
				m_bNextStepON = TRUE;
				return m_nIndex+1;
			}
			return -1;
		}

		// 삭제 할까~?
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
					// 눈앞(카메라앞)에서 렌더링이 시작되도록 한다.
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

				// 죽지 않음 셋팅.
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
				
				// 관리되는 곳에 넣는다.
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
				// 만약 DeletePassiveEffect 가 작업이 된다면, 파티클 같은것이 작업 다 되지도 않았는데 사라질 수도 있다.
				// 그래서 else 에서는 아무 작업을 하지 않는다.
			}
		}
	};



	//////////////////////////////////////////////////////////////////////////
	//							DATA_MAIN


	struct DATA_MAIN
	{
		const NSEffectLua::VEC_STEP_DATA*	m_rvecStep;	// 참조값이다. 사용시 주의가 필요하다.
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

				// 새로운 Step 이 만들어져야 한다면 생성되게 한다.
				int nNextStep = (*iter)->IsCreateNexStep();
				if ( nNextStep != -1 )
				{
					if ( nNextStep < static_cast<int>( m_rvecStep->size() ) )
					{
						boost::shared_ptr<DATA> spData( new DATA( pd3dDevice, (*m_rvecStep)[nNextStep].get(), nNextStep, m_matTrans, m_sTargetID ) );
						m_listData.push_back( spData );
					}
				}

				// 삭제 된다면..
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
	// RanLogicClient 에서 사용하면 됨.
	void StartPlay( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName, const D3DXMATRIX &matTrans, const STARGETID &sTargetID )
	{
		const NSEffectLua::VEC_STEP_DATA* pvecStep = NSEffectLua::Find( szName );
		if ( !pvecStep )
			return;

		// 값이 없으면 등록되지 않는다.
		if ( (*pvecStep).empty() )
			return;

		// 같은게 있다면 재 등록하지 않는다.
		std::map<TSTRING,DATA_MAIN>::const_iterator citer = g_mapPlayData.find( szName );
		if ( citer != g_mapPlayData.end() )
		{
			return;
		}

		// 등록한다.
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
	// RanLogicClient 에서 사용하면 됨.
	//////////////////////////////////////////////////////////////////////////





	//////////////////////////////////////////////////////////////////////////
	// Engine 에서 사용하면 됨.
	void DeleteALL()
	{
		// 참조값을 들고 있으므로 바로삭제.
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

			// fx 관련 삭제
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
		// DATA_MAIN 들
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

	// 기존에 렌더링 된 것을 물 효과나 화면 깨진 효과를 FX 로 작업하여 BackBuffer 에 렌더링 되게 한다.
	void RenderSourceBackBuffer( LPDIRECT3DDEVICEQ pd3dDevice, 
									LPDIRECT3DSURFACEQ pSurface1,
									LPDIRECT3DTEXTUREQ pTexture1, 
									LPDIRECT3DSURFACEQ pSurface2, 
									LPDIRECT3DTEXTUREQ pTexture2, 
									LPDIRECT3DSURFACEQ& pCurrentSurfaceOUT )	// pCurrentSurface 는 복사 된 값을 받아서 원본을 훼손시키지 않도록 한다.
																				// 처음 입력받는 값은 pSurface1 을 복사한 값을 사용하도록 한다.
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
				// 사용되어진 Surface 로 변경한다.
				pCurrentSurfaceOUT = pSurface;
			}
		}
	}

	// BackBuffer 에 Screen 효과를 렌더링 하도록 한다.
	void RenderPostEffect( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		std::list<boost::shared_ptr<DxEffProjBase>>::iterator iter = g_listEffProjBase.begin();
		for ( ; iter!=g_listEffProjBase.end(); ++iter )
		{
			(*iter)->RenderPostEffect( pd3dDevice );
		}
	}
	// Engine 에서 사용하면 됨.
	//////////////////////////////////////////////////////////////////////////
};
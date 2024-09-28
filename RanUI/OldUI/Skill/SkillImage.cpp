#include "StdAfx.h"
#include "SkillImage.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../Interface/SkillFunc.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CSkillImage::CSkillImage(GLGaeaClient* pGaeaClient, EngineDeviceMan* pEngineDevice)
	: CUIGroup(pEngineDevice)
    , m_pGaeaClient(pGaeaClient)
    , m_pSkillProcess ( NULL )
	, m_pd3dDevice ( NULL )
	, m_TotElapsedTime(0.0f)
	, m_bSkillUsed(false)
	, m_bSkillEnd(false)
	, m_bTwinkle(true)
{
}

CSkillImage::~CSkillImage ()
{
}

void CSkillImage::CreateProgressBar ()
{
	CBasicProgressBar* pSkillProcess = new CBasicProgressBar(m_pEngineDevice);
	pSkillProcess->CreateSub ( this, "SKILL_IMAGE_PROGRESS" );
	pSkillProcess->CreateOverImage ( "SKILL_IMAGE_PROGRESS_OVER" );
	pSkillProcess->SetType ( CBasicProgressBar::VERTICAL );
	pSkillProcess->SetOverImageUseRender ( TRUE );
	pSkillProcess->SetOverImageDiffuse ( NS_SKILL::RECHARGECOLOR );
	pSkillProcess->SetControlNameEx ( "프로그래스바" );	
	RegisterControl ( pSkillProcess );
	m_pSkillProcess = pSkillProcess;
}

HRESULT CSkillImage::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	HRESULT hr = S_OK;
	hr = CUIGroup::InitDeviceObjects ( pd3dDevice );
	if ( FAILED ( hr ) ) return hr;

	m_pd3dDevice = pd3dDevice;

	return S_OK;
}

void CSkillImage::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update ( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

	if( m_bSkillUsed && m_bSkillEnd )
	{
		m_TotElapsedTime += fElapsedTime;

		if( m_TotElapsedTime < 0.3f )		m_pSkillProcess->SetPercent(0);
		else if( m_TotElapsedTime < 0.6f )	m_pSkillProcess->SetPercent(1);
		else if( m_TotElapsedTime < 0.9f )	m_pSkillProcess->SetPercent(0);
		else if( m_TotElapsedTime < 1.2f )	m_pSkillProcess->SetPercent(1);
		else
		{
			m_pSkillProcess->SetPercent(0);
			m_TotElapsedTime = 0.0f;
			m_bSkillEnd = false;
			m_bSkillUsed = false;
		}
	}
}

void CSkillImage::SetSkillProcess ( SNATIVEID sNativeID )
{
	if ( !m_pSkillProcess )
	{
		GASSERT ( 0 && "프로그래스바가 만들어지지 않았습니다." );
		return ;
	}

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sNativeID.wMainID, sNativeID.wSubID );
	if ( !pSkill )	return ;

	WORD wPCLEVEL = m_pGaeaClient->GetCharacter()->GETLEVEL();

	//	최대 지연시간
	SCHARSKILL* pCharSkill = m_pGaeaClient->GetCharacter()->GETLEARNED_SKILL ( sNativeID );
	if ( pCharSkill )
	{
        SKILL::SKILLDELAY sSkilldelay;

        SKILL::DELAY_MAP &map = m_pGaeaClient->GetCharacter()->m_SKILLDELAY;
        if ( !map.empty() )
        {
            SKILL::DELAY_MAP_ITER iter = map.find ( sNativeID.dwID );
            if ( iter!= map.end() ) 
                sSkilldelay = (*iter).second;
        }

        float fSkillDelayTime = sSkilldelay.fMAXDelayTime;

        //	현재 지연시간
        float fCurTime = sSkilldelay.fDelayTime;

		////  상태효과에 의한 지연시간 계산
		//fSkillDelayTime *= m_pGaeaClient->GetCharacter()->m_fSTATE_DELAY;

		if( m_bTwinkle && fCurTime == 0.0f ) 
		{
			m_bSkillEnd = true;
			return;
		}
		else					
		{
			m_bSkillEnd = false;
		}

		if ( fSkillDelayTime == 0.0f )
		{
			fSkillDelayTime = 1.0f;
		}

		const float fPercent = fCurTime / fSkillDelayTime;
        m_pSkillProcess->SetPercent ( fPercent );

		m_bSkillUsed = true;
	}
}

void	CSkillImage::CreateSubControl ()
{	
	CreateProgressBar ();
}

void CSkillImage::SetAdditional( const DWORD dwOption )
{
	if( SNATIVEID( false ).dwID == dwOption )
		return;

	SNATIVEID		sICONINDEX;
    char			szImgFile[64] = {0};
	if( SKILL_TO_ADDITIONAL( dwOption ) == SKILL_ADDITIONAL_BATTLE )
	{
		if( GLCONST_CHAR::strBattleIcon.length() == 0 )
			return;

		StringCchCopy(szImgFile, 64, GLCONST_CHAR::strBattleIcon.c_str());
		sICONINDEX	= GLCONST_CHAR::sBattleIconIndex;
	} //if

	if( ( m_sICONINDEX == sICONINDEX ) &&
		( CUIControl::GetTextureName() == szImgFile ) )
	{
		return ;
	}
	
	CUIControl::SetTexturePos( NS_SKILL::GetIconTexurePos( sICONINDEX ) );
	CUIControl::SetTextureName( szImgFile );
	m_sICONINDEX = sICONINDEX;

	if( m_pd3dDevice )
		CUIControl::InitDeviceObjects( m_pd3dDevice );
} //CSkillImage::SetAdditional

void CSkillImage::SetSkill ( SNATIVEID sNativeID )
{
	if ( sNativeID == NATIVEID_NULL () )
		return;

	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( sNativeID.wMainID, sNativeID.wSubID );
	if ( !pSkill )
		return;

	SNATIVEID sICONINDEX	 = pSkill->m_sEXT_DATA.sICONINDEX;
	CString	  strTextureName = pSkill->m_sEXT_DATA.strICONFILE.c_str();	

	//	이전 프레임과 ICONID와
	//	텍스쳐 이름이 같다는 것은
	//	아래의 작업을 더 이상 진행할 필요가 없다는 것을 의미한다.
	if( (sICONINDEX == m_sICONINDEX) &&
		(strTextureName == CUIControl::GetTextureName()) )
	{
		return;
	}
	
	CUIControl::SetTexturePos ( NS_SKILL::GetIconTexurePos ( sICONINDEX ) );
	CUIControl::SetTextureName ( strTextureName.GetString () );
	m_sICONINDEX = sICONINDEX;

	if( m_pd3dDevice )
		CUIControl::InitDeviceObjects ( m_pd3dDevice );
}

void CSkillImage::ResetSkill ()
{
	CUIControl::DeleteDeviceObjects ();
	CUIControl::SetTextureName ( NULL );
	m_sICONINDEX = NATIVEID_NULL ();
}
#include "pch.h"
#include "./DxWBStage.h"
#include "./DxGlobalStage.h"

#include "../../SigmaCore/Loading/NSLoadingDirect.h"
#include "../../RanLogicClient/LoadingThread/LoadingThread.h"
#include "../../RanLogicClient/LoadingThread/LoadingDirectClient.h"
#include "../Init/NSInitClientLua.h"
#include "../GfxInterfaceBase.h"

//// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
//// ----------------------------------------------------------------------------



DxWBStage::DxWBStage(DxGlobalStage* pGlobalStage, GLGaeaClient* pGaeaClient)
    : m_pGlobalStage(pGlobalStage)
    , m_pGaeaClient(pGaeaClient)
{	
}

DxWBStage::~DxWBStage(void)
{
}

BOOL DxWBStage::GetUsedMaterialSystem()
{
	return FALSE;
}

void DxWBStage::MsgProcess ( NET_MSG_GENERIC* nmg )
{
}

HRESULT DxWBStage::FrameMove( float fTime, float fElapsedTime, BOOL bNotRendering )
{
	//GfxInterfaceBase::GetInstance()->SetLoadingData( NATIVEID_NULL(), NSInitClientLua::GetRandomLobbyLoadingImage(), false );
	//GfxInterfaceBase::GetInstance()->OnBeginLoading( ELoadingSceneType::LogOut );

	LoadingThread::SetData
	(
		CString( NSInitClientLua::GetRandomLobbyLoadingImage() ),
		NATIVEID_NULL(),
		false /*bFirst*/,
		false /*bOverGrade*/,
		NSInitClientLua::GetLobbyLoadingImageFullSize() 
	);

    LoadingDirectClient sLodingDirect( FALSE );
    NSLoadingDirect::Set( &sLodingDirect, EM_LT_NORMAL );		// [shhan][2015.09.18] ±âÈ¹ÆÀÀÇ ¿äÃ»À¸·Î ÁÂÃø»ó´Ü¿¡ WB ½Â¸®±¹±â ¾È ³ª¿À´Â ¹æÇâÀ¸·Î µÊ.

	NSLoadingDirect::FrameMoveRender( FALSE );

	NSLoadingDirect::Reset();
	
	//GfxInterfaceBase::GetInstance()->OnEndLoading();

	return S_OK;
}

HRESULT DxWBStage::Render ()
{
	return S_OK;
}


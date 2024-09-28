#include "StdAfx.h"
#include "GfxRenderThread.h"

#include "../../SigmaCore/Util/EBTime.h"

GfxRenderThread::GfxRenderThread( Platform::ThreadingType tt )
: Thread( 256 * 1024, 1 )
, RTCommandQueue( (RTCommandQueue::ThreadingType) (tt & TT_TypeMask) )
, m_pDevice( NULL )
, m_pHAL( NULL )
, m_WatchDogTrigger( 1 )
, m_WatchDogThread( watchDogThreadFn, (void*) &m_WatchDogTrigger )
, m_bAdvanceDrawFrameEnqueued( false )
, m_bIsRunAD( false )
, m_fTimeAD( 0.0f )
{
	if( tt & TT_WatchDogFlag )
		m_WatchDogThread.Start();
}

GfxRenderThread::~GfxRenderThread()
{
}

void GfxRenderThread::GetRenderInterfaces( Render::Interfaces* pInterface )
{
	pInterface->Clear();
	pInterface->pHAL = m_pHAL;
	pInterface->pTextureManager = m_pHAL->GetTextureManager();
	pInterface->RenderThreadID = m_pHAL->GetRenderThreadId();
}

void GfxRenderThread::PushThreadCommand( ThreadCommand* pCmd )
{
	if( NULL == pCmd )
		return;

	pCmd->Execute();
}

bool GfxRenderThread::Start( ThreadState initialState )
{
	if( false == Thread::Start( initialState ) )
		return false;

	SetThreadName( "Scaleform Renderer" );

	return true;
}

int GfxRenderThread::Run()
{
	RTCommandBuffer cmd;

	do
	{
		if( true == PopCommand( &cmd ) )
		{
			cmd.Execute( *this );
			if( true == cmd.NeedsWait() )
				cmd.GetNotifier()->Notify();
		}   

		AtomicOps< unsigned >::Store_Release( &m_WatchDogTrigger, 1 );

	} while( false == IsProcessingStopped() );

	return 0;
}

int GfxRenderThread::watchDogThreadFn( Thread* thread, void* triggerAddr )
{
	unsigned* trigger = reinterpret_cast< unsigned* >( triggerAddr );
	int failureCount = 0;

	while( true )
	{
		if( false == AtomicOps< unsigned >::CompareAndSet_Sync( trigger, 1, 0 ) )
		{
			failureCount++;
			if( failureCount >= nWatchDogMaxFailureCount )
			{
				int* crash = NULL;
				*crash = 0xDEADDEAD;
				return -1;
			}
		}
		else
		{
			failureCount = 0;
		}

		MSleep( GfxRenderThread::nWatchDogInterval );
	}

	return 0;
}

void GfxRenderThread::StartThread()
{
	if( false == IsSingleThreaded() )
	{
		if( true == Start() )
		{
			SetThreadName( "Scaleform Renderer" );
			SetRenderThreadId( GetThreadId() );
		}
	}
	else
	{
		SetRenderThreadId( Scaleform::GetCurrentThreadId() );
	}
}

void GfxRenderThread::WaitAdvanceDrawFrame()
{
	if( true == m_bAdvanceDrawFrameEnqueued && false == IsSingleThreaded() )
	{
		m_AdvanceDrawFrameDone.Wait();
		m_bAdvanceDrawFrameEnqueued = false;
	}
}

//-----------------------------------------------------------------------------------------------------

void GfxRenderThread::ExitThread()
{
	PushCallAndWait( &GfxRenderThread::exitThread );
}

void GfxRenderThread::SetGlyphCacheParams( const Render::GlyphCacheParams& params )
{
	PushCall( &GfxRenderThread::setGlyphCacheParams, params );
}

void GfxRenderThread::SetToleranceParams( const Render::ToleranceParams& params )
{
	PushCall( &GfxRenderThread::setToleranceParams, params );
}

void GfxRenderThread::AdvanceDrawFrame( Scaleform::GFx::Movie* pMovie, Scaleform::GFx::MovieDisplayHandle hMovie )
{
	PushCall( &GfxRenderThread::advanceDrawFrame, pMovie, hMovie );
	m_bAdvanceDrawFrameEnqueued = true;
}

//-----------------------------------------------------------------------------------------------------

void GfxRenderThread::exitThread()
{
	StopQueueProcessing();
}

void GfxRenderThread::setGlyphCacheParams( const Render::GlyphCacheParams& params )
{
	m_paramGC = params;
	if( NULL != m_pHAL )
		m_pHAL->GetGlyphCache()->SetParams( m_paramGC );
}

void GfxRenderThread::setToleranceParams( const Render::ToleranceParams& params )
{
	m_paramTol = params;
	if( NULL != m_pHAL )
	{
		m_pHAL->SetToleranceParams( m_paramTol );
		m_pHAL->GetMeshCache().ClearCache();
	}
}

void GfxRenderThread::advanceDrawFrame( Scaleform::GFx::Movie* pMovie, Scaleform::GFx::MovieDisplayHandle hMovie )
{
	CRITICAL_SECTION cs;
	InitializeCriticalSection( &cs );
	EnterCriticalSection( &cs );

	m_AdvanceDrawFrameDone.PulseEvent();

	if( false == m_bIsRunAD )
	{
		LeaveCriticalSection( &cs );
		DeleteCriticalSection( &cs );
		return;
	}

	static const float fFrameRate = 1.0f / 60.0f;

	float fCurTime = sc::DXUtil_Timer( sc::TIMER_GETAPPTIME );
	float fElapsedTime = fCurTime - m_fTimeAD;
	if( fElapsedTime < fFrameRate )
	{
		AdvanceDrawFrame( pMovie, hMovie );

		LeaveCriticalSection( &cs );
		DeleteCriticalSection( &cs );
		return;
	}

	m_fTimeAD = fCurTime;

	pMovie->Advance( fElapsedTime, 0 );

	LPDIRECT3DSTATEBLOCK9 pStateBlock = NULL;
	if( D3D_OK != m_pDevice->CreateStateBlock( D3DSBT_ALL, &pStateBlock ) )
		return;

	m_pDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0, 0, 0 ), 1.0f, 0L );

	if( SUCCEEDED( m_pDevice->BeginScene() ) )
	{
		pStateBlock->Capture();
		m_pHAL->BeginFrame();

		if( true == hMovie.NextCapture( m_pHAL->GetContextNotify() ) )
			m_pHAL->Display( hMovie );

		m_pHAL->EndFrame();
		pStateBlock->Apply();

		m_pDevice->EndScene();
		m_pDevice->Present( NULL, NULL, NULL, NULL );
	}
	else
	{
		m_pDevice->EndScene();
	}

	SAFE_RELEASE( pStateBlock );

	AdvanceDrawFrame( pMovie, hMovie );

	LeaveCriticalSection( &cs );
	DeleteCriticalSection( &cs );
}
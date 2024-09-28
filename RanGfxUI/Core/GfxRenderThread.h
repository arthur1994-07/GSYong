#pragma once

#include "./Platform/Platform.h"
#include "./Platform/Platform_RTCommandQueue.h"

using namespace Platform;

class GfxRenderThread : public Thread, public RTCommandQueue,
						public ThreadCommandQueue
{
public :
	GfxRenderThread( Platform::ThreadingType tt );
	virtual ~GfxRenderThread();

private :
	static const int nWatchDogInterval = 5000;
	static const int nWatchDogMaxFailureCount = 12;

public :
	LPDIRECT3DDEVICE9	m_pDevice;

	Scaleform::Render::D3D9::HAL* m_pHAL;

private :
	Render::GlyphCacheParams	m_paramGC;
	Render::ToleranceParams		m_paramTol;

	volatile unsigned	m_WatchDogTrigger;
	Thread				m_WatchDogThread;

	Scaleform::Event	m_AdvanceDrawFrameDone;
	bool				m_bAdvanceDrawFrameEnqueued;

public :
	bool	m_bIsRunAD;
	float	m_fTimeAD;

public :
	virtual void GetRenderInterfaces( Render::Interfaces* pInterface );
	virtual void PushThreadCommand( ThreadCommand* pCmd );

	virtual bool Start( ThreadState initialState = Running );
	virtual int Run();

private :
	static int watchDogThreadFn( Thread* thread, void* trigger );

public :
	void StartThread();
	void WaitAdvanceDrawFrame();

public :	// Thread Interface
	void ExitThread();
	void SetGlyphCacheParams( const Render::GlyphCacheParams& params );
	void SetToleranceParams( const Render::ToleranceParams& params );
	void AdvanceDrawFrame( Scaleform::GFx::Movie* pMovie, Scaleform::GFx::MovieDisplayHandle hMovie );

private :	// Thread Func
	void exitThread();
	void setGlyphCacheParams( const Render::GlyphCacheParams& params );
	void setToleranceParams( const Render::ToleranceParams& params );
	void advanceDrawFrame( Scaleform::GFx::Movie* pMovie, Scaleform::GFx::MovieDisplayHandle hMovie );

public :
	bool IsSingleThreaded() const { return GetThreadingType() != MultiThreaded; }
};
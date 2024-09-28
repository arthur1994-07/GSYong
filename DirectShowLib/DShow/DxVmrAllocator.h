#pragma once

#pragma warning(push, 2)
#pragma warning(disable : 4995)
#include <vector>
#pragma warning(pop)

using namespace std;

#include "DxScene.h"

#define FAIL_RET(x) do { if( FAILED( hr = ( x  ) ) ) return hr; } while(0)

class DxVmrAllocator : public IVMRSurfaceAllocator9, IVMRImagePresenter9
{
public:
    DxVmrAllocator( HWND hWnd, LPDIRECT3D9 pD3D, LPDIRECT3DDEVICE9 pDevice );
    virtual ~DxVmrAllocator();

	//---------------------------------------------------------------------------
    // IVMRSurfaceAllocator9
    virtual HRESULT STDMETHODCALLTYPE InitializeDevice( 
            DWORD_PTR dwUserID,
            VMR9AllocationInfo *lpAllocInfo,
            DWORD* lpNumBuffers );
            
    virtual HRESULT STDMETHODCALLTYPE TerminateDevice( DWORD_PTR dwID );
    
    virtual HRESULT STDMETHODCALLTYPE GetSurface( 
        DWORD_PTR dwUserID,
        DWORD SurfaceIndex,
        DWORD SurfaceFlags,
        IDirect3DSurface9** lplpSurface );
    
    virtual HRESULT STDMETHODCALLTYPE AdviseNotify( IVMRSurfaceAllocatorNotify9* lpIVMRSurfAllocNotify );

	//---------------------------------------------------------------------------
    // IVMRImagePresenter9
    virtual HRESULT STDMETHODCALLTYPE StartPresenting( DWORD_PTR dwUserID );
    
    virtual HRESULT STDMETHODCALLTYPE StopPresenting( DWORD_PTR dwUserID );
    
    virtual HRESULT STDMETHODCALLTYPE PresentImage(  DWORD_PTR dwUserID, VMR9PresentationInfo* lpPresInfo );
    
	//---------------------------------------------------------------------------
    // IUnknown
    virtual HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, void** ppvObject );

    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();

protected:
    HRESULT CreateDevice();

    // a helper function to erase every surface in the vector
    void DeleteSurfaces();

    bool NeedToHandleDisplayChange();

    // This function is here so we can catch the loss of surfaces.
    // All the functions are using the FAIL_RET macro so that they exit
    // with the last error code.  When this returns with the surface lost
    // error code we can restore the surfaces.
    HRESULT PresentHelper( VMR9PresentationInfo* lpPresInfo );

private:
    // needed to make this a thread safe object
    CCritSec	m_ObjectLock;
    HWND		m_hWnd;
    long		m_lRefCount;

    LPDIRECT3D9			m_pD3D;
    LPDIRECT3DDEVICE9	m_pDevice;

    CComQIPtr< IVMRSurfaceAllocatorNotify9 >	m_pVMRSurfAllocNotify;
    vector< CComQIPtr< IDirect3DSurface9 > >	m_vecSurfaces;
    CComQIPtr< IDirect3DSurface9 >				m_pRenderTarget;
    CComQIPtr< IDirect3DTexture9 >				m_pPrivateTexture;

    DxScene										m_Scene;
};
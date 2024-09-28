

#ifndef _mrcDx9Renderer_h_
#define _mrcDx9Renderer_h_

#include "mrcConfig.h"
#include "mrcRenderer.h"


struct IDirect3DDevice9;
struct IDirect3D9;

#include <d3d9types.h>
#include <d3dx9math.h>

namespace mrc
{
	class Dx9HardwareVertexBuffer;

	class Dx9Renderer : public IRenderer
	{
	private:
		MIRACLE_FRIEND_CLASS_SINGLETON(Dx9Renderer);
		Dx9Renderer();
		virtual ~Dx9Renderer();

		virtual void init(const InitArg& arg) mrcOVERRIDE;
		virtual void release() mrcOVERRIDE;
		virtual void* getDevice() mrcOVERRIDE;


		virtual	void set2DCoordinatesSystem(int32 x,int32 y,int32 width,int32 height) mrcOVERRIDE;
		virtual Point getWidthHeight() mrcOVERRIDE;


		virtual void setClearColor(RenderColor color) mrcOVERRIDE;
		virtual void clear(bool color,bool depth,bool stencil) mrcOVERRIDE;
		virtual bool begin() mrcOVERRIDE;
		virtual void end()  mrcOVERRIDE;

		//////////////////////////////////////////////////////////////////////////
		virtual void configAlphaTest(bool bTest) mrcOVERRIDE;
		//////////////////////////////////////////////////////////////////////////

		// ----------    state    -------------------------

// 		virtual void setStateRenderTarget(const HSurface& surface ) mrcOVERRIDE;
// 		virtual void setStateFilter(bool isFilter) mrcOVERRIDE;
// 		virtual void setStateTexture(const HTexture& hTexture) mrcOVERRIDE;
// 		virtual void setStateTFactor( uint32 value ) mrcOVERRIDE;
// 		virtual void setStateScissorRect(const Rect& rect) mrcOVERRIDE;
// 		virtual void setStateScreenEffect(bool isScreen) mrcOVERRIDE;
 
		virtual void setMaterial(const Material& material) mrcOVERRIDE;



		virtual void setStateTextureAddressMode(int32 stage,ADDRESS_MODE mode) mrcOVERRIDE;
// 		virtual void setTextureStateStageColorOption(TEXTUREOP textureOp) mrcOVERRIDE;
// 		virtual void setTextureStateStageArg(int32 stage,TEXTUREOP_ARG arg, uint32 value) mrcOVERRIDE;

		// -------------------------------------------------

// 		virtual void setWorldMatrix( int32 x, int32 y ,real scale) mrcOVERRIDE;
// 		virtual void getWorldMatrix( int32* x, int32* y ,real* scale) const mrcOVERRIDE;

		//////////////////////////////////////////////////////////////////////////


		virtual void draw(PRIMITIVE_TYPE primitiveType,const Matrix4& matrix,const Vertex* pVertexBuffer,uint32 count) mrcOVERRIDE;


		virtual void flush() mrcOVERRIDE;
		virtual void present() mrcOVERRIDE;

		//////////////////////////////////////////////////////////////////////////

		
		virtual bool loadTexture(const filesystem::Path& path,HTexture* hTexture,LoadedTextureInfo* pInfo) mrcOVERRIDE;
		virtual void releaseTexture(const HTexture& hTexture) mrcOVERRIDE;


		virtual void setUIOptimize(bool isOn) mrcOVERRIDE;

		virtual const HTexture& getEmptyWhiteTexture() mrcOVERRIDE;

		virtual void showDebugWire(bool isShow) mrcOVERRIDE;
	private:
		void setStateFilter(bool isFilter);
		void setStateTexture(const HTexture& hTexture);
		void setStateTFactor( uint32 value );
		void setStateScissorRect(const Rect& rect);
		void setStateScreenEffect(bool isScreen);


		void drawImmediately(const StateBlock& stateBlock,const Matrix4& matrix,Dx9HardwareVertexBuffer* pVertexBuffer);
		void checkBufferingDataAndFlush(int32 addVertexSize);

		void applyState(const StateBlock& stateBlock);

		//////////////////////////////////////////////////////////////////////////
		typedef void (Dx9Renderer::*F_draw)(PRIMITIVE_TYPE,const Matrix4& ,const Vertex* ,uint32 );
		F_draw mFuncDraw;

		void bufferingVertexBuffer(PRIMITIVE_TYPE primitiveType,const Matrix4& matrix,const Vertex* pVertexBuffer,uint32 count);
		void drawAtOnceSameState(PRIMITIVE_TYPE primitiveType,const Matrix4& matrix,const Vertex* pVertexBuffer,uint32 count);
		void drawDefault(PRIMITIVE_TYPE primitiveType,const Matrix4& matrix,const Vertex* pVertexBuffer,uint32 count);

		//////////////////////////////////////////////////////////////////////////
		void backupRenderState();
		void rollbackRenderState();
		//////////////////////////////////////////////////////////////////////////
		bool isForce2NSize();
		//////////////////////////////////////////////////////////////////////////
		void backupMonitorSetting();
		
		void checkDevice();
	private:
		bool mIsDirectCall;
		IDirect3DDevice9* mD3dDevice;
		IDirect3D9*		  mD3D;
		HWND			  mWnd;
		uint32            mVertexProcessing; 
		D3DPRESENT_PARAMETERS mD3dPresentParameters;

		bool mAlphaTest;

		enum TEXTURE_SIZE_LIMIT{ ALLOW_ONLY_POW2 , ALLOW_ALL_WITH_LIMITED_CONDITION, ALLOW_ALL};
		TEXTURE_SIZE_LIMIT mTextureSizeLimit;

		RenderColor mClearColor;

		typedef std::auto_ptr<D3DXMATRIXA16> AP_D3DXMATRIXA16;
		D3DXMATRIXA16 mCurrentMatrix;

// 		bool  mWorldMAtrixUsed;
// 		int32 mWorldMatrixX,mWorldMatrixY;
// 		real  mWorldMatrixScale;

		bool		  mCheckStateChange;
		StateBlock	  mCurrentStateBlock;
		StateBlock    mBeforeStateBlock;

		typedef mrcSharedPtr<Dx9HardwareVertexBuffer> SP_Dx9HardwareVertexBuffer;
		SP_Dx9HardwareVertexBuffer mBufferingVertexBuffer;
		// 인덱스 버퍼는  일단 지원하지 말자

		HTexture   mEmptyWhiteTextureHandle;

		bool       mIsShowDebugWire;
		DEVMODE    mBackupDevmode;

		D3DXMATRIXA16 mProjectionMatrix;
		D3DXMATRIXA16 mViewMatrix;

		DWORD mBackUp_CULLMODE;
		DWORD mBackUp_LIGHTING;
		DWORD mBackUp_ZENABLE;

		DWORD mBackup_FVF;
		DWORD mBackUp_D3DTSS_COLOROP;
		DWORD mBackUp_D3DTSS_COLORARG1;
		DWORD mBackUp_D3DTSS_COLORARG2;
		DWORD mBackUp_D3DTSS_ALPHAOP;
		DWORD mBackUp_D3DTSS_ALPHAARG1;
		DWORD mBackUp_D3DTSS_ALPHAARG2;
		DWORD mBackUp_D3DRS_ALPHATESTENABLE;
		DWORD mBackUp_D3DRS_ALPHAFUNC;
		DWORD mBackUp_D3DRS_ALPHAREF;
		DWORD mBackUp_D3DRS_ALPHABLENDENABLE;
		DWORD mBackUp_D3DRS_SRCBLEND;
		DWORD mBackUp_D3DRS_DESTBLEND;
		DWORD mBackUp_D3DSAMP_ADDRESSU;
		DWORD mBackUp_D3DSAMP_ADDRESSV;
		DWORD mBackUp_D3DSAMP_MAGFILTER;
		DWORD mBackUp_D3DSAMP_MINFILTER;
	};
	
	
	
	
	
	
	
}
















#endif
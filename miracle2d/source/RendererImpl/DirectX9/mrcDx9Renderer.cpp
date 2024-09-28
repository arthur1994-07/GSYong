

#include "mrcPrecompiled.h"

#if MIRACLE_RENDERER_DEVICE == MIRACLE_RENDERER_DX9

#include "mrcDx9Renderer.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr.h>

#include "mrcMatrix4.h"
#include "mrcVector3.h"

#pragma comment (lib, "d3d9.lib")
//#pragma comment (lib, "dxerr9.lib")
#pragma comment (lib, "dxguid.lib")
// 
#if MIRACLE_RENDERER_DEVICE_DEBUG
#pragma comment (lib, "d3dx9d.lib")	
#else
#pragma comment (lib, "d3dx9.lib")	
#endif


namespace mrc
{
	const int Vertex::FVF = D3DFVF_XYZ | D3DFVF_TEX1;


	namespace
	{
		D3DXMATRIXA16  __tempIdentityMatrix;
		D3DXMATRIX* __tempIdentityMatrixXXX = D3DXMatrixIdentity( &__tempIdentityMatrix);
		const D3DXMATRIXA16& getIdentityMatrix() { return __tempIdentityMatrix; }


		template< typename PD3DOBJECT >
		void D3DSAFE_RELEASE( PD3DOBJECT &pD3DObject )
		{
			if( pD3DObject )
				{ pD3DObject->Release(); }

			pD3DObject = NULL;
		}

		D3DXMATRIXA16 makeD3DXMatrix( const Matrix4& mat )
		{
			// Transpose matrix
			// D3D9 uses row vectors i.e. V*M
			// Ogre, OpenGL and everything else uses column vectors i.e. M*V
			return D3DXMATRIXA16(
				mat[0][0], mat[1][0], mat[2][0], mat[3][0],
				mat[0][1], mat[1][1], mat[2][1], mat[3][1],
				mat[0][2], mat[1][2], mat[2][2], mat[3][2],
				mat[0][3], mat[1][3], mat[2][3], mat[3][3]);
		}
		//---------------------------------------------------------------------
		Matrix4 convertD3DXMatrix( const D3DXMATRIXA16& mat )
		{
			return Matrix4(
				mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
				mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
				mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
				mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]);
		}

		mrcStaticAssert( sizeof(D3DVECTOR) == sizeof(Vector3) );
		D3DXVECTOR3& RefVector(Vector3& vec){ return reinterpret_cast<D3DXVECTOR3&>(vec); }
		Vector3&   RefVector(D3DXVECTOR3& vec){ return reinterpret_cast<Vector3&>(vec); }


		D3DXVECTOR3 operator*(const D3DXVECTOR3& vec,const D3DXMATRIXA16& matrix)
		{
			D3DXVECTOR4 point( vec , 1);
			D3DXVECTOR4 outPoint;
			D3DXVec4Transform(&outPoint,&point,&matrix);
			return D3DXVECTOR3(outPoint.x,outPoint.y,outPoint.z);
		}
	}

	
	//////////////////////////////////////////////////////////////////////////


	class Dx9HardwareVertexBuffer
	{
		friend class ScopeLock;
	public:
		class ScopeLock
		{
		public:
			ScopeLock(Dx9HardwareVertexBuffer& vertexBuffer,Vertex** pVertex) : mVertexBuffer(vertexBuffer) 
			{ mVertexBuffer.lock(pVertex); }
			~ScopeLock() { mVertexBuffer.unlock(); }

		private:
			Dx9HardwareVertexBuffer& mVertexBuffer;
		};
	public:
		//////////////////////////////////////////////////////////////////////////
		Dx9HardwareVertexBuffer(IDirect3DDevice9* pD3dDevice,int32 size)
		{
			mVB = 0;
			mD3dDevice = pD3dDevice;
			resize(size);
		}

		~Dx9HardwareVertexBuffer() 
		{
			release();
		}

		void clear()
		{
			mCurBufferPos = 0;
		}

		void resize(int32 size)
		{
			release();
			HRESULT hr = S_OK;
			if( FAILED( mD3dDevice->CreateVertexBuffer( sizeof(Vertex) * size,
				D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC, Vertex::FVF , D3DPOOL_SYSTEMMEM, &mVB, NULL ) ) )
			{
				mrcASSERT(false);
			}
		
			mCapacity = size;
			mCurBufferPos = 0;
		}

	

		void add(const Vertex* pCopyVertexArray,uint32 copySize)
		{
			mrcASSERT(copySize + size() <  capacity() );
			const Vertex* pCopyVertex = pCopyVertexArray;
			Vertex* pSelfVertex;
			ScopeLock selfLock(*this,&pSelfVertex);
			Vertex* pBegin = pSelfVertex + size(); 
			Vertex* pEnd   = pBegin + copySize;
			for (Vertex* pIt = pBegin; pIt != pEnd; )
				*pIt++ = *pCopyVertex++;
			
			mCurBufferPos += copySize;
		}

		void assign(const Vertex* vertexArray,int32 size)
		{
			mCurBufferPos = 0;
			add(vertexArray,size);
		}

	

		virtual uint32  size() const  {	return mCurBufferPos; }
		virtual uint32  capacity() const  {	return mCapacity; }

		LPDIRECT3DVERTEXBUFFER9 getRawBuffer() { return mVB; }
	private:
		void lock(Vertex** pVertex) 
		{ 
			mVB->Lock( 0, 0, (void**)pVertex, D3DLOCK_DISCARD );
		}

		void unlock()  {	mVB->Unlock(); }
		void release() 
		{
			D3DSAFE_RELEASE(mVB);
			mCurBufferPos = 0;
			mCapacity = 0;
		}

	private:
		int32					mCurBufferPos;
		int32					mCapacity;
		LPDIRECT3DVERTEXBUFFER9 mVB;
		IDirect3DDevice9*	    mD3dDevice;
	};



	//////////////////////////////////////////////////////////////////////////
	
	Dx9Renderer::Dx9Renderer()
	{
		mIsDirectCall = false;
		mD3dDevice = NULL;
		mD3D = NULL;
		mWnd = NULL;
		mVertexProcessing = 0;
		ZeroMemory(&mD3dPresentParameters,sizeof(mD3dPresentParameters));

		mAlphaTest = false;

		mTextureSizeLimit = ALLOW_ONLY_POW2;

		setClearColor( makeColorXRGB(0.4f,0.4f,0.4f) );

//		mCurrentMatrix.reset(mrcNEW D3DXMATRIXA16);
		D3DXMatrixIdentity(&mCurrentMatrix);

// 		mWorldMAtrixUsed = false;
// 		mWorldMatrixX = 0;
// 		mWorldMatrixY = 0;
// 		mWorldMatrixScale = 1.f;

		//mCurrentStateBlock;
		//mBeforeStateBlock;
		//mBufferingVertexBuffer;
		makeInvalidHandle( mEmptyWhiteTextureHandle );

		setUIOptimize(false);

		mIsShowDebugWire = false;
	}

	Dx9Renderer::~Dx9Renderer()
	{
		release();
	}

	void Dx9Renderer::release() 
	{
		mBufferingVertexBuffer.reset();
		D3DSAFE_RELEASE(mEmptyWhiteTextureHandle);

		if( mIsDirectCall )
			ChangeDisplaySettings(&mBackupDevmode, 0);

		if( mD3D )
		{
			D3DSAFE_RELEASE(mD3dDevice);
			D3DSAFE_RELEASE(mD3D);
		}
	}

	void* Dx9Renderer::getDevice()
	{
		return mD3dDevice;
	}

	void Dx9Renderer::init(const InitArg& arg)
	{
		backupMonitorSetting();

// 		mrcASSERT(!mD3D);
// 		mrcASSERT(!mD3dDevice);

		mrcASSERT( RenderPasing::argToBool( 
			RenderPasing::getInitArgAttribute(arg,_mrcT("DirectX9"))) );

		mWnd = (HWND)RenderPasing::argToPointer(
			RenderPasing::getInitArgAttribute(arg, _mrcT("WindowHandle")) );

		

		int32 width = (int32) RenderPasing::argToInt(
			RenderPasing::getInitArgAttribute(arg, _mrcT("Width")) );
		int32 height = (int32) RenderPasing::argToInt(
			RenderPasing::getInitArgAttribute(arg, _mrcT("Height")) );

		mD3dDevice = (IDirect3DDevice9*) RenderPasing::argToPointer(
			RenderPasing::getInitArgAttribute(arg, _mrcT("DevicePointer")) );
		 	
		ZeroMemory(&mD3dPresentParameters, sizeof(mD3dPresentParameters));
		mD3dPresentParameters.BackBufferWidth = width;
		mD3dPresentParameters.BackBufferHeight = height;

		if( mD3dDevice == NULL )
		{
			mrcASSERT(!mD3D);
			mD3D = Direct3DCreate9(D3D_SDK_VERSION);
			mrcASSERT(mD3D);

			mIsDirectCall = true;
			// 루아 같은 스크립트가 사용된 후에는 D3DCREATE_FPU_PRESERVE 이 플래그를 사용해야 한다 (다이렉트X는 float에 최적화
			// 되어 있으므로 float 형으로 맞추는듯하다 double이 있어도.... 이 플래그 사용하면 double 사용하게 되어
			// 느려질수 있다는거 같다, 하지만 요즘은 gpu에서 보통 거의 다 처리하므로  다이렉트에서 소프트웨어로 처리할때만 효과를 내는듯하므로
			// D3DCREATE_FPU_PRESERVE 이걸 써도 큰 차이는 없을듯 싶다

			// hardware vertex processing 가능 여부 검사
			D3DCAPS9 caps;
			mD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
			mVertexProcessing = (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) ? 
				D3DCREATE_HARDWARE_VERTEXPROCESSING : D3DCREATE_SOFTWARE_VERTEXPROCESSING;

			// 이걸 주는 이유는 다른 라이브러리에서 쓰는 부동소수점 자리도 영향을 받기 때문
			mVertexProcessing |= D3DCREATE_FPU_PRESERVE;

			// 위에서 정의 했다
	//		ZeroMemory(&mD3dPresentParameters, sizeof(mD3dPresentParameters));
	//		mD3dPresentParameters.BackBufferWidth = width;
	//		mD3dPresentParameters.BackBufferHeight = height;

			mD3dPresentParameters.BackBufferCount = 1;
			mD3dPresentParameters.Windowed = TRUE;
			mD3dPresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
			mD3dPresentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
//			mD3dPresentParameters.MultiSampleType = D3DMULTISAMPLE_8_SAMPLES;
//			mD3dPresentParameters.MultiSampleQuality = 2;
			//////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////
			mrcASSERT(mWnd);

			HRESULT hr;
			// 다이렉트X 장치 완성
			hr = mD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,(HWND)mWnd,
				mVertexProcessing, &mD3dPresentParameters, &mD3dDevice);

			if (hr != D3D_OK)
			{
				if (hr == D3DERR_NOTAVAILABLE)
				{
					mrcASSERT_MSG(false, "D3DERR_NOTAVAILABLE " );
				}
				if (hr == D3DERR_OUTOFVIDEOMEMORY)
				{
					mrcASSERT_MSG(false, "D3DERR_OUTOFVIDEOMEMORY" );
				}

				mrcASSERT(false);
			}
			
			//////////////////////////////////////////////////////////////////////////
			//////////////////////////////////////////////////////////////////////////

			D3DCAPS9 cap;
			mD3dDevice->GetDeviceCaps(&cap);

			bool POW2 = (cap.TextureCaps & D3DPTEXTURECAPS_POW2) ? true : false;
			bool NONPOW2CONDITIONAL = (cap.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL) ? true : false;

// 			1.대략 지포스2 MX 이전 세대 그래픽 카드 
// 				D3DPTEXTURECAPS_POW2 = TRUE 
// 				D3DPTEXTURECAPS_NONPOW2CONDITIONAL = FALSE 
// 				=> 2의 승수만 가능. 
// 
// 				2.대략 지포스2 MX 이후 세대 그래픽 카드 
// 				D3DPTEXTURECAPS_POW2 = TRUE 
// 				D3DPTEXTURECAPS_NONPOW2CONDITIONAL = TRUE 
// 				=> 밉맵이 있을 경우 2의 승수만 가능. 
// 				=> 밉맵이 1일 경우엔 예외적으로 2의 승수가 아닌 사이즈 가능 
// 
// 				3.지포스 6000시리즈 등 최신 그래픽 카드 
// 				D3DPTEXTURECAPS_POW2 = FALSE 
// 				D3DPTEXTURECAPS_NONPOW2CONDITIONAL = FALSE 
//				mrcASSERT( (POW2 == true && NONPOW2CONDITIONAL == false) == false );

// 			둘다 YES 인 경우 다음과 같은 제약 하에서 2 의 배승수가 아닌 texture 를 지원한다고 합니다. 
// 
// 				- D3DTADDRESS_CLAMP 만 써라.( 반복시키지 마라 이거죠. ) 
// 				- D3DRS_WRAP 을 꺼라. 
// 				- Mipmapping 사용하지 마라( magnification filter 만 사용해라. ). 
// 				- Texture format 이 DXT1 ~ DXT5 면 안 된다. 

			if( POW2 == true && NONPOW2CONDITIONAL == false )
				mTextureSizeLimit = ALLOW_ONLY_POW2;
			else if( POW2 == true && NONPOW2CONDITIONAL == true )
				mTextureSizeLimit = ALLOW_ALL_WITH_LIMITED_CONDITION;
			else if( POW2 == false && NONPOW2CONDITIONAL == false )
				mTextureSizeLimit = ALLOW_ALL;
		}

		D3DSAFE_RELEASE(mEmptyWhiteTextureHandle);
		if ( FAILED( mD3dDevice->CreateTexture(64, 64, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &mEmptyWhiteTextureHandle,NULL) ) )
		{
			mrcASSERT(false);
		}

		mBufferingVertexBuffer.reset( mrcNEW Dx9HardwareVertexBuffer(mD3dDevice, 512)  );
	}



	static void makeD3d2dOrtho(D3DMATRIX * m, real w, real h, real znear, real zfar) 
	{
		ZeroMemory(m, sizeof(D3DMATRIX));
		m->_11 = 2.0f / w;
		m->_22 = 2.0f / -h;
		m->_33 = 1.0f / (zfar - znear);
		m->_41 = (w + 1.0f) / -w;
		m->_42 = (h + 1.0f) / h;
		m->_43 = znear / (znear - zfar);
		m->_44 = 1.0;
	}

	void Dx9Renderer::set2DCoordinatesSystem(int32 x,int32 y,int32 width,int32 height)
	{
		D3DXMATRIXA16 matProj;

		// - 0.5f 는 3D 좌표에선 픽셀의 중간값을 가지고 계산하계 되므로 그림이 반으로 짤리게 된다
		// 2D 로서 화면 픽셀과 완벽하게 매칭되려면 0.5f 를 해주도록하자
// 		D3DXMatrixOrthoOffCenterLH( &matProj , static_cast<float>(x) - 0.5f, static_cast<float>(width) - 0.5f,
// 			static_cast<float>(height) - 0.5f, static_cast<float>(y) - 0.5f, 0 , 1 );

		makeD3d2dOrtho(&matProj,(real)width,(real)height,real(0),real(1));
		mD3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
	}

	Point Dx9Renderer::getWidthHeight()
	{
		mrcASSERT(mD3dDevice);
		return Point((int16)mD3dPresentParameters.BackBufferWidth,
			(int16)mD3dPresentParameters.BackBufferHeight);
	}


	void Dx9Renderer::setClearColor(RenderColor color)
	{
		mClearColor = color;
	}

	void Dx9Renderer::clear(bool color,bool depth,bool stencil)
	{
		DWORD  clearFlag  = 0;
		if(color)    clearFlag |= D3DCLEAR_TARGET;
		if(depth)    clearFlag |= D3DCLEAR_ZBUFFER;
		if(stencil)  clearFlag |= D3DCLEAR_STENCIL;

		mD3dDevice->Clear(0, 0,clearFlag, mClearColor, 1.0f, 0);
	}

	bool Dx9Renderer::begin()
	{
		checkDevice();

		mCurrentStateBlock.clear();
		mBeforeStateBlock.clear();

		backupRenderState();

		// 월드
		mD3dDevice->SetTransform( D3DTS_WORLD, &getIdentityMatrix());
		mD3dDevice->SetTransform( D3DTS_VIEW,  &getIdentityMatrix());
		
		// 프로젝션
		set2DCoordinatesSystem(0,0,mD3dPresentParameters.BackBufferWidth,mD3dPresentParameters.BackBufferHeight);

		

		// Turn off culling
		mD3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

		// Turn off D3D lighting
		mD3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

		// Turn on the zbuffer
		mD3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

		// set vertex format	
		mD3dDevice->SetFVF( Vertex::FVF );

		mD3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
		mD3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		mD3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );

		// 알파테스트에 사용될 알파값 = TextureAlpha * ShadeAlpha
		mD3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
		mD3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		mD3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );

		// 투명색 적용을 위한 알파테스트. (알파값이 0이면 투명색처리)
		mD3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, mAlphaTest );
		mD3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_NOTEQUAL );
		mD3dDevice->SetRenderState( D3DRS_ALPHAREF, 0 );

		mD3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		mD3dDevice->SetRenderState( D3DRS_SRCBLEND , D3DBLEND_SRCALPHA );
		mD3dDevice->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA );

		mD3dDevice->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);
		mD3dDevice->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);

		// 뭉개짐없게 정확히가 기본
		mD3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		mD3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );

		if( mIsDirectCall )
		{
			if( FAILED( mD3dDevice->BeginScene() ) )
				mrcASSERT(false);
		}


		//////////////////////////////////////////////////////////////////////////
		return true;
	}

	void Dx9Renderer::end()
	{
		flush();

		if( mIsDirectCall )
		{
			mD3dDevice->EndScene();
		}

		rollbackRenderState();
	}


	//////////////////////////////////////////////////////////////////////////
	void Dx9Renderer::configAlphaTest(bool bTest) 
	{
		mAlphaTest = bTest;
	}
	//////////////////////////////////////////////////////////////////////////
	// ---------------- state -----------------------------------------------

// 	void Dx9Renderer::setStateRenderTarget(const HSurface& surface )
// 	{
// 		if( mCurrentStateBlock.mSurface != surface )
// 		{
// 			mCheckStateChange = true;
// 			mCurrentStateBlock.mSurface = surface;
// 		}
// 	}

	void Dx9Renderer::setStateFilter(bool isFilter)
	{
		if( mCurrentStateBlock.mIsFilter != isFilter )
		{
			mCheckStateChange = true;
			mCurrentStateBlock.mIsFilter = isFilter;
		}
	}

	void Dx9Renderer::setStateTexture(const HTexture& hTexture)
	{
		if( mCurrentStateBlock.mTextureHandle != hTexture )
		{
			mCheckStateChange = true;
			mCurrentStateBlock.mTextureHandle = hTexture;
		}
	}

	void Dx9Renderer::setStateTFactor( uint32 value )
	{
		if( mCurrentStateBlock.mTFactor != value )
		{
			mCheckStateChange = true;
			mCurrentStateBlock.mTFactor = value;
		}
	}

	void Dx9Renderer::setStateScissorRect(const Rect& rect)
	{
		if( mCurrentStateBlock.mScissorRect != rect)
		{
			mCheckStateChange = true;
			mCurrentStateBlock.mScissorRect = rect;
		}
	}

	void Dx9Renderer::setStateScreenEffect(bool isScreen)
	{
		if( mCurrentStateBlock.mIsScreenEffect != isScreen )
		{
			mCheckStateChange = true;
			mCurrentStateBlock.mIsScreenEffect = isScreen;
		}		
	}

	void Dx9Renderer::setMaterial(const Material& material)
	{
		setStateFilter(material.mIsFilter);
		setStateTexture(material.mTexture);
		setStateTFactor(uint32(material.mARGB));
		setStateScissorRect(material.mScissorRect);
		setStateScreenEffect(material.mIsScreenEffect);
	}

	

	void Dx9Renderer::setStateTextureAddressMode(int32 stage,ADDRESS_MODE mode) 
	{
		if( mCurrentStateBlock.mTextureAddressModeBlock.mStage != stage ||
			 mCurrentStateBlock.mTextureAddressModeBlock.mMode != mode )
		{
			mCheckStateChange = true;
			TextureAddressModeBlock block;
			block.mStage = stage;
			block.mMode = mode;
			mCurrentStateBlock.mTextureAddressModeBlock = block;
		}
	}

// 	void Dx9Renderer::setTextureStateStageColorOption(TEXTUREOP textureOp) 
// 	{
// 		TextureStateStageColorOptionBlock block;
// 		block.mTextureOp = textureOp;
// 		mCurrentStateBlock.mTextureStateStageColorOptionBlock.setValue(block);
// 	}
// 
// 	void Dx9Renderer::setTextureStateStageArg(int32 stage,TEXTUREOP_ARG arg, uint32 value)
// 	{
// 		TextureStateStageArgBlock block;
// 		block.mStage = stage;
// 		block.mArg = arg;
// 		block.mValue = value;
// 		mCurrentStateBlock.mTextureStateStageArgBlock.setValue(block);
// 	}

	//////////////////////////////////////////////////////////////////////////
// 		FLOAT _11, FLOAT _12, FLOAT _13, FLOAT _14,
// 		FLOAT _21, FLOAT _22, FLOAT _23, FLOAT _24,
// 		FLOAT _31, FLOAT _32, FLOAT _33, FLOAT _34,
// 		FLOAT _41, FLOAT _42, FLOAT _43, FLOAT _44 


	static void makeQuadToTriangle(const Vertex vertexQuard[4],Vertex vertexTri[6])
	{
		vertexTri[0] = vertexQuard[0];
		vertexTri[1] = vertexQuard[1];
		vertexTri[2] = vertexQuard[2];
		vertexTri[3] = vertexQuard[2];
		vertexTri[4] = vertexQuard[3];
		vertexTri[5] = vertexQuard[0];
	}

	static void changeQuadToTriangle(const Vertex* pVertexBuffer,const Matrix4& matrix,Dx9HardwareVertexBuffer* pBufferingVertexBuffer)
	{
		Vertex  vertexTri[6];
		Vertex  vertexQuard[4];

		for (int32 i=0; i < 4; ++i)
		{
			vertexQuard[i].pos =  matrix * pVertexBuffer[i].pos;
			vertexQuard[i].uv = pVertexBuffer[i].uv;
		}

		makeQuadToTriangle(vertexQuard,vertexTri);	
		pBufferingVertexBuffer->add(vertexTri,6);
	}

	static void changeQuadToTriangle(const Vertex* pVertexBuffer,Dx9HardwareVertexBuffer* pBufferingVertexBuffer)
	{
		Vertex   vertexTri[6];
		makeQuadToTriangle(pVertexBuffer,vertexTri);
		pBufferingVertexBuffer->add(vertexTri,6);
	}
	//////////////////////////////////////////////////////////////////////////
	void Dx9Renderer::draw(PRIMITIVE_TYPE primitiveType,const Matrix4& matrix,const Vertex* pVertexBuffer,uint32 count)
	{
		// 일단 쿼드만 지원하자
		mrcASSERT( primitiveType == PRIMITIVE_QUAD );
		(this->*mFuncDraw)(primitiveType,matrix,pVertexBuffer,count);
	}

	void Dx9Renderer::bufferingVertexBuffer(PRIMITIVE_TYPE primitiveType,const Matrix4& matrix,const Vertex* pVertexBuffer,uint32 count)
	{
		switch( primitiveType )
		{
		case PRIMITIVE_QUAD:
			{		
				mrcASSERT(count == 4);
				checkBufferingDataAndFlush(6);
				changeQuadToTriangle(pVertexBuffer,matrix,mBufferingVertexBuffer.get());
				break;
			}
		case PRIMITIVE_TRANGLES:
			{
				checkBufferingDataAndFlush(count);
				//버택스에 메트릭스를 곱한후 저장
				mBufferingVertexBuffer->add(pVertexBuffer,count);					
				break;
			}
		default:
			{
				mrcASSERT(false);
			}
		}
	}

	void Dx9Renderer::drawAtOnceSameState(PRIMITIVE_TYPE primitiveType,const Matrix4& matrix,const Vertex* pVertexBuffer,uint32 count)
	{
		// 전에것과  같다면 모아둔다
		if( !mCheckStateChange )
		{				
			bufferingVertexBuffer(primitiveType,matrix,pVertexBuffer,count);
//			mCurrentStateBlock.clear();
		}
		else
		{
			flush();
			bufferingVertexBuffer(primitiveType,matrix,pVertexBuffer,count);
			mBeforeStateBlock = mCurrentStateBlock;
//			mCurrentStateBlock.clear();
		}
	}

	void Dx9Renderer::drawDefault(PRIMITIVE_TYPE primitiveType,const Matrix4& matrix,const Vertex* pVertexBuffer,uint32 count)
	{
		switch( primitiveType )
		{
		case PRIMITIVE_QUAD:
			{		
				changeQuadToTriangle(pVertexBuffer,mBufferingVertexBuffer.get());				
				drawImmediately(mCurrentStateBlock,matrix,mBufferingVertexBuffer.get());
				break;
			}
		case PRIMITIVE_TRANGLES:
			{
				//버택스에 메트릭스를 곱한후 저장
				mBufferingVertexBuffer->add(pVertexBuffer,count);
				drawImmediately(mCurrentStateBlock,matrix,mBufferingVertexBuffer.get());
				break;
			}
		default:
			{
				mrcASSERT(false);
			}
		}

		//mCurrentStateBlock.clear();
	}

	void Dx9Renderer::flush()
	{
		if( mBufferingVertexBuffer )
		{
			if( mBufferingVertexBuffer->size() > 0 )
			{
				drawImmediately(mBeforeStateBlock,Matrix4::IDENTITY,mBufferingVertexBuffer.get());
				mBeforeStateBlock.clear();
			}
		}
	}

	void Dx9Renderer::drawImmediately(const StateBlock& stateBlock,const Matrix4& matrix,Dx9HardwareVertexBuffer* pDx9Buffer)
	{
		applyState(stateBlock);

 		mD3dDevice->SetTransform(D3DTS_WORLD, &makeD3DXMatrix(matrix));
 		mD3dDevice->SetStreamSource(0,pDx9Buffer->getRawBuffer(),0,sizeof(Vertex));
 		mD3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,pDx9Buffer->size()/3);

		if( mIsShowDebugWire )
		{			
			mD3dDevice->SetTexture( 0, 0);
			mD3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
			mD3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,pDx9Buffer->size()/3);
			mD3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
		}

		//////////////////////////////////////////////////////////////////////////
		mBufferingVertexBuffer->clear();
		mCheckStateChange = false;
	}

	void Dx9Renderer::checkBufferingDataAndFlush( int32 addVertexSize)
	{
		if( mBufferingVertexBuffer->size() + addVertexSize > mBufferingVertexBuffer->capacity() )
		{
			flush();
		}
	}

	void Dx9Renderer::applyState(const StateBlock& stateBlock)
	{
		//////////////////////////////////////////////////////////////////////////
		// 랜더투 택스쳐용으로 미구현
		//stateBlock.mSurface

		//////////////////////////////////////////////////////////////////////////
		/// 택스쳐 설정

		if( IsInvalidHandle(stateBlock.mTextureHandle) )
			mD3dDevice->SetTexture( 0, getEmptyWhiteTexture() );
		else
			mD3dDevice->SetTexture( 0, stateBlock.mTextureHandle );
		
		//////////////////////////////////////////////////////////////////////////

		mD3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, stateBlock.mIsFilter ? D3DTEXF_LINEAR : D3DTEXF_POINT );
		mD3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, stateBlock.mIsFilter ? D3DTEXF_LINEAR : D3DTEXF_POINT );

		//////////////////////////////////////////////////////////////////////////
		// 텍스쳐 UV 주소 
		const TextureAddressModeBlock& textureAddressModeBlock = stateBlock.mTextureAddressModeBlock;
		
		int d3dMode = D3DTADDRESS_CLAMP;
		switch(textureAddressModeBlock.mMode)
		{
		case ADDRESS_WRAP:
 			d3dMode = D3DTADDRESS_WRAP;
 			break;
		case ADDRESS_MIRROR:
			d3dMode = D3DTADDRESS_MIRROR;
			break;
		case ADDRESS_CLAMP:
			d3dMode = D3DTADDRESS_CLAMP;
			break;
		default:
			mrcASSERT(false);
		}

		mD3dDevice->SetSamplerState(textureAddressModeBlock.mStage,D3DSAMP_ADDRESSU,d3dMode);
		mD3dDevice->SetSamplerState(0,D3DSAMP_ADDRESSV,d3dMode);

		//////////////////////////////////////////////////////////////////////////
		mD3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, stateBlock.mTFactor);


		
		Rect scissorRect = stateBlock.mScissorRect;
		if( scissorRect.mLeft == -1 && scissorRect.mTop == -1 && 
			scissorRect.mRight == -1 && scissorRect.mBottom == -1 )
		{
			mD3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE); 
		}
		else
		{
			const Rect& rect = stateBlock.mScissorRect;
			RECT winRect = {rect.mLeft,rect.mTop,rect.mRight,rect.mBottom};
			if( winRect.left < 0)
				winRect.left = 0;

			if( winRect.top < 0 )
				winRect.top = 0;

			if( winRect.right < 0 )
				winRect.right = (LONG) (float(mD3dPresentParameters.BackBufferWidth) );

			if( winRect.bottom < 0)
				winRect.bottom = (LONG) (float(mD3dPresentParameters.BackBufferHeight) );

			mD3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE,TRUE); 
			mD3dDevice->SetScissorRect(&winRect);
		}			

		if( stateBlock.mIsScreenEffect )
			mD3dDevice->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_ONE );
		else
			mD3dDevice->SetRenderState( D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA );	
		// 안개효과는 쉐이더로 직접 구현한다

	}

	void Dx9Renderer::present()
	{
		if( mIsDirectCall )
		{
			HRESULT hr;
			hr = mD3dDevice->Present(0, 0, 0, 0);
		}

	}

	void Dx9Renderer::setUIOptimize(bool isOn)
	{
		if( isOn )
		{
			if( mFuncDraw == &Dx9Renderer::drawAtOnceSameState )
				return;

			flush();
			mFuncDraw = &Dx9Renderer::drawAtOnceSameState;
		}
		else
		{
			flush();
			mFuncDraw = &Dx9Renderer::drawDefault;
		}
	}

	//////////////////////////////////////////////////////////////////////////

	bool Dx9Renderer::loadTexture(const filesystem::Path& path,HTexture* hTexture,LoadedTextureInfo* pInfo)
	{
		D3DXIMAGE_INFO info;
		D3DSURFACE_DESC d3dSurfaceDesc;

	//	UINT SIZE = isForce2NSize() ? D3DX_DEFAULT : D3DX_FROM_FILE;
		UINT SIZE = D3DX_DEFAULT;

		if( FAILED( D3DXCreateTextureFromFileEx( mD3dDevice, path.string().c_str(), 
			SIZE , SIZE , 1 , 0, D3DFMT_UNKNOWN, 
			D3DPOOL_MANAGED, 
			D3DX_FILTER_NONE,
			D3DX_FILTER_NONE,
			0, &info, NULL, hTexture ) ) )
		{
			return false;
		}

		
		(*hTexture)->GetLevelDesc( 0, &d3dSurfaceDesc );
		
		pInfo->fileWidth = info.Width;
		pInfo->fileHeight = info.Height;
		pInfo->surfaceWidth = d3dSurfaceDesc.Width;
		pInfo->surfaceHeight = d3dSurfaceDesc.Height;

		return true;
	}

	void Dx9Renderer::releaseTexture(const HTexture& hTexture)
	{
		hTexture->Release();
	}

	const HTexture& Dx9Renderer::getEmptyWhiteTexture()
	{
		mrcASSERT(  !IsInvalidHandle(mEmptyWhiteTextureHandle) );
		return mEmptyWhiteTextureHandle;
	}

	
	void Dx9Renderer::backupMonitorSetting()
	{
		BOOL success = false;
		//displayDevice는 장치 이름을 얻기 위해 사용됨
		DISPLAY_DEVICE displayDevice;
		displayDevice.cb = sizeof(DISPLAY_DEVICE);
		CHAR deviceName[256] = {0};
		//장치 인덱스. 이 코드는 첫 번째 모니터만 찾음
		DWORD devIndex = 0; 
		success = EnumDisplayDevices(0,devIndex,&displayDevice,0);
		//올바른 장치인지 확인

		if (displayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
		{
			mrcASSERT( sizeof(displayDevice.DeviceName) < sizeof(deviceName) );
			_tcscpy( deviceName,  displayDevice.DeviceName );
			//devMode는 모니터 설정을 가져옴
			DEVMODE devMode;			
			devMode.dmSize = sizeof(DEVMODE);
			success = EnumDisplaySettings( deviceName, 
				ENUM_REGISTRY_SETTINGS, &devMode );
			//				float fDesktopAspectRatio = devMode.dmPelsWidth / (float)devMode.dmPelsHeight;
			mBackupDevmode = devMode;			
		}
		else
		{
			mrcASSERT(false);
		}
	}

	bool Dx9Renderer::isForce2NSize()
	{
		return mTextureSizeLimit == ALLOW_ONLY_POW2 || mTextureSizeLimit == ALLOW_ALL_WITH_LIMITED_CONDITION;		
	}



	void Dx9Renderer::backupRenderState()
	{		
		mD3dDevice->GetTransform( D3DTS_PROJECTION , &mProjectionMatrix);
		mD3dDevice->GetTransform( D3DTS_VIEW,  &mViewMatrix);

		mD3dDevice->GetRenderState( D3DRS_CULLMODE, &mBackUp_CULLMODE );
		mD3dDevice->GetRenderState( D3DRS_LIGHTING, &mBackUp_LIGHTING );
		mD3dDevice->GetRenderState( D3DRS_ZENABLE,  &mBackUp_ZENABLE );

		mD3dDevice->GetFVF(&mBackup_FVF);
	
		mD3dDevice->GetTextureStageState( 0, D3DTSS_COLOROP,   &mBackUp_D3DTSS_COLOROP);
		mD3dDevice->GetTextureStageState( 0, D3DTSS_COLORARG1, &mBackUp_D3DTSS_COLORARG1);
		mD3dDevice->GetTextureStageState( 0, D3DTSS_COLORARG2, &mBackUp_D3DTSS_COLORARG2);

		// 알파테스트에 사용될 알파값 = TextureAlpha * ShadeAlpha
		mD3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAOP,   &mBackUp_D3DTSS_ALPHAOP);
		mD3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAARG1, &mBackUp_D3DTSS_ALPHAARG1);
		mD3dDevice->GetTextureStageState( 0, D3DTSS_ALPHAARG2, &mBackUp_D3DTSS_ALPHAARG2);

		// 투명색 적용을 위한 알파테스트. (알파값이 0이면 투명색처리)
		mD3dDevice->GetRenderState( D3DRS_ALPHATESTENABLE, &mBackUp_D3DRS_ALPHATESTENABLE );
		mD3dDevice->GetRenderState( D3DRS_ALPHAFUNC, &mBackUp_D3DRS_ALPHAFUNC);
		mD3dDevice->GetRenderState( D3DRS_ALPHAREF, &mBackUp_D3DRS_ALPHAREF );

		mD3dDevice->GetRenderState( D3DRS_ALPHABLENDENABLE, &mBackUp_D3DRS_ALPHABLENDENABLE );
		mD3dDevice->GetRenderState( D3DRS_SRCBLEND , &mBackUp_D3DRS_SRCBLEND );
		mD3dDevice->GetRenderState( D3DRS_DESTBLEND , &mBackUp_D3DRS_DESTBLEND );

		mD3dDevice->GetSamplerState(0,D3DSAMP_ADDRESSU,&mBackUp_D3DSAMP_ADDRESSU);
		mD3dDevice->GetSamplerState(0,D3DSAMP_ADDRESSV,&mBackUp_D3DSAMP_ADDRESSV);

		// 뭉개짐없게 정확히가 기본
		mD3dDevice->GetSamplerState( 0, D3DSAMP_MAGFILTER, &mBackUp_D3DSAMP_MAGFILTER );
		mD3dDevice->GetSamplerState( 0, D3DSAMP_MINFILTER, &mBackUp_D3DSAMP_MINFILTER );

	}

	void Dx9Renderer::rollbackRenderState()
	{
		mD3dDevice->SetTransform( D3DTS_PROJECTION , &mProjectionMatrix);
		mD3dDevice->SetTransform( D3DTS_VIEW,  &mViewMatrix);

		mD3dDevice->SetRenderState( D3DRS_CULLMODE, mBackUp_CULLMODE );
		mD3dDevice->SetRenderState( D3DRS_LIGHTING, mBackUp_LIGHTING );
		mD3dDevice->SetRenderState( D3DRS_ZENABLE,  mBackUp_ZENABLE );

		mD3dDevice->SetFVF(mBackup_FVF);

		mD3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   mBackUp_D3DTSS_COLOROP);
		mD3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, mBackUp_D3DTSS_COLORARG1);
		mD3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, mBackUp_D3DTSS_COLORARG2);

		// 알파테스트에 사용될 알파값 = TextureAlpha * ShadeAlpha
		mD3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   mBackUp_D3DTSS_ALPHAOP);
		mD3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, mBackUp_D3DTSS_ALPHAARG1);
		mD3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, mBackUp_D3DTSS_ALPHAARG2);

		// 투명색 적용을 위한 알파테스트. (알파값이 0이면 투명색처리)
		mD3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, mBackUp_D3DRS_ALPHATESTENABLE );
		mD3dDevice->SetRenderState( D3DRS_ALPHAFUNC, mBackUp_D3DRS_ALPHAFUNC);
		mD3dDevice->SetRenderState( D3DRS_ALPHAREF, mBackUp_D3DRS_ALPHAREF );

		mD3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, mBackUp_D3DRS_ALPHABLENDENABLE );
		mD3dDevice->SetRenderState( D3DRS_SRCBLEND , mBackUp_D3DRS_SRCBLEND );
		mD3dDevice->SetRenderState( D3DRS_DESTBLEND , mBackUp_D3DRS_DESTBLEND );

		mD3dDevice->SetSamplerState(0,D3DSAMP_ADDRESSU,mBackUp_D3DSAMP_ADDRESSU);
		mD3dDevice->SetSamplerState(0,D3DSAMP_ADDRESSV,mBackUp_D3DSAMP_ADDRESSV);

		// 뭉개짐없게 정확히가 기본
		mD3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, mBackUp_D3DSAMP_MAGFILTER );
		mD3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, mBackUp_D3DSAMP_MINFILTER );
	}

	void Dx9Renderer::checkDevice()
	{
		HRESULT Result = mD3dDevice->TestCooperativeLevel(); 
		if( Result == D3DERR_DEVICELOST )
		{
			//				_bLostDevice = true;
			return;
		}
		else if( Result == D3DERR_DEVICENOTRESET) 
		{
			if( mIsDirectCall )
			{
				// 스스로 디바이스를 만들었었다면 스스로 복구를 시도한다(다른 디바이스에서 만들었다면 수동으로 복구명령을 받는다)
				// 일단 생략
				// 일반적으로 Reset으로 복구 되지 않는다면 매우 귀찬아진다
				Result = mD3dDevice->Reset(&mD3dPresentParameters);
				//			LASSERT( D3DERR_DRIVERINTERNALERROR != Result && D3DERR_INVALIDCALL != Result &&
				//				D3DERR_OUTOFVIDEOMEMORY != Result && E_OUTOFMEMORY != Result);

				//					if(  Result == D3DERR_DEVICELOST )
				//						_bLostDevice = true;
				//					else
				//						_bLostDevice = false;
				mrcCRITIAL_ASSERT( D3DERR_DRIVERINTERNALERROR != Result && D3DERR_OUTOFVIDEOMEMORY != Result && E_OUTOFMEMORY != Result);
				mrcCRITIAL_ASSERT(D3DERR_INVALIDCALL != Result );
			}				

			// 스스로 복구 못하므로 누가 해줄때까지 장치는 소실로 처리
			//				_bLostDevice = true;
			//				return;
		} 
		//			_bLostDevice = false;
	}

	void Dx9Renderer::showDebugWire(bool isShow)
	{
		mIsShowDebugWire = isShow;
	}

	//////////////////////////////////////////////////////////////////////////
}//////////////////////////////////////////////////////////////////////////





#endif // MIRACLE_RENDERER_DEVICE == MIRACLE_RENDERER_DX9





#ifndef _mrcRenderer_h_
#define _mrcRenderer_h_

#include "mrcConfig.h"
#include "mrcUtil/mrcFileSystem.h"
#include "mrcMath.h"
#include "mrcShape.h"
#include "mrcVector3.h"

namespace mrc
{
	class Vector2;
	class Vector3;
	class Vector4;

	class Matrix3;
	class Matrix4;


	typedef uint32 RenderColor;

	extern const RenderColor COLOR_WHITE;
	extern const RenderColor COLOR_BLACK;
	extern const RenderColor COLOR_RED;
	extern const RenderColor COLOR_GREEN;
	extern const RenderColor COLOR_BLUE;


	template<class T>
	inline RenderColor makeColorARGB(T a,T r,T g,T b)
	{
		return makeColorARGB(uint32(a),uint32(r),uint32(g),uint32(b));
	}

	template<class T>
	inline RenderColor makeColorXRGB(T r,T g,T b)
	{
		return makeColorARGB(uint32(0xff),uint32(r),uint32(g),uint32(b));
	}
	
	template<>
	inline RenderColor makeColorARGB(uint32 a,uint32 r,uint32 g,uint32 b)
	{
		return ((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff));
	}

	template<>
	inline RenderColor makeColorARGB(real a,real r,real g,real b)
	{
		return makeColorARGB( makeColor(a),makeColor(r),makeColor(g),makeColor(b) );
	}


	template<>
	inline RenderColor makeColorXRGB(uint32 r,uint32 g,uint32 b)
	{
		return makeColorARGB( uint32(0xff),r,g,b);
	}

	template<>
	inline RenderColor makeColorXRGB(real r,real g,real b)
	{
		return makeColorARGB(1.f,r,g,b);
	}
	
	inline void getColorARGB(RenderColor color,uint8* a,uint8* r,uint8* g,uint8* b) 
	{
		*a = (color >> 24) & 0xFF;
		*r = (color >> 16) & 0xFF;
		*g = (color >> 8)  & 0xFF;
		*b = (color >> 0)  & 0xFF;
	}
	
	inline real toColorValue(uint8 value) { return real(value)/255.f; }
	inline uint8 toColorValue(real value) { return uint8(value*255.f); }
	
	//////////////////////////////////////////////////////////////////////////

	enum PRIMITIVE_TYPE{ PRIMITIVE_LINESTRIP,PRIMITIVE_TRANGLES,PRIMITIVE_TRIANGLE_FAN,PRIMITIVE_QUAD };
	enum TEXTUREOP{ TEXTUREOP_MODULATE , TEXTUREOP_ADD };
	enum TEXTUREOP_ARG{ TEXTUREOP_COLORARG0,TEXTUREOP_COLORARG1,TEXTUREOP_COLORARG2,TEXTUREOP_CONSTANT };
	enum ADDRESS_MODE{ADDRESS_WRAP,ADDRESS_MIRROR,ADDRESS_CLAMP};
	
	// CUSTOMVERTEX로 잡아 놓고 쓰는것 opengl에서도 형식을 쓸수있다 glInterleavedArray에 예(GL_T2F_C4UB_V3F)로 잡으면 된다
	struct Vertex
	{
		static const int32 FVF; 
		Vertex() { pos.z = 0; }//pos = Vector3::ZERO; uv = Vector2::ZERO; }
		Vector3 pos;
		// T Factor로 대체하자
	//	int32 color;
		Vector2 uv;
	};

	struct Material
	{
		Material()
		{
			makeInvalidHandle(mTexture);
			mScissorRect = Rect(-1,-1,-1,-1);
			mARGB = COLOR_WHITE;
			mIsFilter = false;
			mIsScreenEffect = false;
		}

		HTexture mTexture;
		Rect mScissorRect;
		RenderColor mARGB;
		bool mIsFilter;
		bool mIsScreenEffect;
	};

	// 지속적으로 무언가 추가될수도 있으므로 구조체로 만들어둠
	struct LoadedTextureInfo
	{
		int32 surfaceWidth,surfaceHeight;
		int32 fileWidth,fileHeight;
	};


	struct TextureAddressModeBlock
	{
		TextureAddressModeBlock() { mStage = 0; mMode = ADDRESS_CLAMP; }

		uint32 mStage;
		ADDRESS_MODE mMode;
	};

	struct StateBlock
	{
		StateBlock()
		{
			mIsFilter = false;
			mIsScreenEffect = false;
			makeInvalidHandle( mTextureHandle );
			makeInvalidHandle( mSurface );
			mTFactor = COLOR_WHITE;
			mScissorRect = Rect(-1,-1,-1,-1);
		}

		void clear() { *this = StateBlock(); }
		//////////////////////////////////////////////////////////////////////////
		bool										    mIsFilter;
		bool											mIsScreenEffect;
		HSurface										mSurface;
		HTexture										mTextureHandle;
		TextureAddressModeBlock							mTextureAddressModeBlock;
		uint32											mTFactor;
		Rect											mScissorRect;

	};

	// 나중에 장면 단위로 저장할 필요가 있다면...
// 	struct SceneBlock
// 	{
// 		StateBlock mStateBlock;
// 
// 		PRIMITIVE_TYPE mDrawPrimitiveType;
// 		int32		   mDrawPrimitiveCount;
// 		IVertexBuffer* mDrawVertexBuffer;
// 	};


	class IRenderer
	{
	public:
		typedef map<String,String> InitArg;

		//////////////////////////////////////////////////////////////////////////
		static IRenderer& getInstance();
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		virtual void init(const InitArg& arg) = 0;
		virtual void release() = 0;
		virtual void* getDevice() = 0;

		virtual	void set2DCoordinatesSystem(int32 x,int32 y,int32 width,int32 height) = 0;
		virtual Point getWidthHeight() = 0;

		virtual void setClearColor(RenderColor color) = 0;  
		virtual void setClearColor(real r,real g,real b) { setClearColor(makeColorXRGB(r,g,b)); }

		virtual void clear(bool color,bool depth,bool stencil) = 0;
		virtual bool begin() = 0;
		virtual void end()  = 0;

		//////////////////////////////////////////////////////////////////////////
		virtual void configAlphaTest(bool bTest) = 0;
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// ----------    state    -------------------------

// 		virtual void setStateRenderTarget(const HSurface& surface ) = 0;
// 		virtual void setStateFilter(bool isFilter) = 0;
// 		virtual void setStateTexture(const HTexture& hTexture) = 0;
// 		virtual void setStateTFactor( uint32 value ) = 0;
// 		virtual void setStateScissorRect(const Rect& rect) = 0;
// 		virtual void setStateScreenEffect(bool isScreen) = 0;

		virtual void setMaterial(const Material& material) = 0;

		//////////////////////////////////////////////////////////////////////////

		virtual void setStateTextureAddressMode(int32 stage,ADDRESS_MODE mode) = 0;
// 		virtual void setTextureStateStageColorOption(TEXTUREOP textureOp) = 0;
// 		virtual void setTextureStateStageArg(int32 stage,TEXTUREOP_ARG arg, uint32 value) = 0;

		// ----------    state    --------------------------


// 		virtual void setMatrix(const Matrix4& matrix) = 0;
// 		virtual const Matrix4 getMatrix() = 0;

// 		virtual void setWorldMatrix( int32 x, int32 y ,real scale) = 0;
// 		virtual void getWorldMatrix( int32* x, int32* y ,real* scale) const = 0;

		//////////////////////////////////////////////////////////////////////////


		virtual void draw(PRIMITIVE_TYPE primitiveType,const Matrix4& pMatrix,const Vertex* pVertexBuffer,uint32 count) = 0;
		
		// 강제로 해야 할 일이 있을수 있음 
		virtual void flush() = 0;

		// 다른 엔진에 얻힌다면 필요 없을수도...
		virtual void present() = 0;
		//////////////////////////////////////////////////////////////////////////

//		virtual bool createVertexBuffer(AP_IVertexBuffer* apVertexBuffer,int32 size) = 0;

		virtual bool loadTexture(const filesystem::Path& path,HTexture* hTexture,LoadedTextureInfo* pInfo) = 0;
		virtual void releaseTexture(const HTexture& hTexture) = 0;

		virtual void setUIOptimize(bool isOn) = 0;

		virtual const HTexture& getEmptyWhiteTexture() = 0;

		virtual void showDebugWire(bool isShow) = 0;
	};

	namespace RenderPasing
	{
		bool argToBool(const String& attribute);
		int32 argToInt(const String& attribute);
		real argToReal(const String& attribute);
		void* argToPointer(const String& attribute);
		String getInitArgAttribute(const IRenderer::InitArg& arg,const String& attri);
		
		void setInitArgAttribute(IRenderer::InitArg& arg,const String& attri,bool value);
		void setInitArgAttribute(IRenderer::InitArg& arg,const String& attri,int32 value);
		void setInitArgAttribute(IRenderer::InitArg& arg,const String& attri,real value);
		void setInitArgAttribute(IRenderer::InitArg& arg,const String& attri,void* value);

	}

}


#endif
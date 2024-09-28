
#include "mrcPrecompiled.h"

#include "mrcRenderer.h"

#include "RendererImpl/DirectX9/mrcDx9Renderer.h"
#include "RendererImpl/Opengl/mrcGlRenderer.h"
#include "mrcUtil/mrcSingleton.h"

namespace mrc
{

	const RenderColor COLOR_WHITE = makeColorXRGB(255,255,255);
	const RenderColor COLOR_BLACK = makeColorXRGB(0,0,0);
	const RenderColor COLOR_RED   = makeColorXRGB(255,0,0);
	const RenderColor COLOR_GREEN = makeColorXRGB(0,255,0);
	const RenderColor COLOR_BLUE  = makeColorXRGB(0,0,255);


//////////////////////////////////////////////////////////////////////////
namespace 
{


IRenderer& getRenderer()
{
#if MIRACLE_RENDERER_DEVICE == MIRACLE_RENDERER_DX9
	typedef Loki::SingletonHolder<Dx9Renderer,Loki::CreateUsingNew,
		Loki::LongevityLifetime::DieRenderer > Dx9RendererS;

	return Dx9RendererS::Instance();
#elif MIRACLE_RENDERER_DEVICE == MIRACLE_RENDERER_OPENGL
	typedef Loki::SingletonHolder<GlRenderer,Loki::CreateUsingNew,
		Loki::LongevityLifetime::DieRenderer > GlRendererS;

	return GlRendererS::Instance();
#else

#error no support device, yet

#endif
}
//////////////////////////////////////////////////////////////////////////
// 
// // 
// 
// class CommandData
// {
// 
// };
// class RenderCommander : public IRenderer
// {
// 	//////////////////////////////////////////////////////////////////////////
// 	RenderCommander() : mAtomic(0) {}
// 	
// 	void draw()
// 	{
// 		// 큐에 담는다
// 	}
// 
// 	bool isFlushing()     { return mAtomic > 0; }
// 	
// 	void render()
// 	{
// 		beginFlush();
// 		
// 		while( !mQueue.empty() )
// 		{
// 			const CommandData& data = mQueue.front();
// 
// 
// 
// 
// 
// 			mQueue.pop();
// 		}
// 			
// 		endFlush();
// 	}
// private:
// 	void beginFlush()
// 	{
// 		++mAtomic;
// 	}
// 
// 	void endFlush()
// 	{
// 		--mAtomic;
// 		yield_thread();
// 	}
// 
// 
// private:
// 	atomic_count mAtomic;
// 	queue<CommandData> mQueue;
// };
// 
// 
// 
// class RenderCommanderMgr : public IRenderer
// {
// 	RenderCommanderMgr()
// 	{
// 		mFrontCommander = &mCommander[0];
// 		mBackCommander  = &mCommander[1];
// 	}
// 
// 	virtual void begin()
// 	{
// 		scoped_spinlock lock(mLock);
// 	}
// 
// 	virtual void end() 
// 	{
// 		scoped_spinlock lock(mLock);
// 
// 		swapRenderCommander();
// 	}
// 
// 	
// 
// private:
// 	void swapRenderCommander()
// 	{
// 		while( mBackCommander->isFlushing() )
// 		{
// 			yield_thread();
// 		}
// 
// 		swap(mFrontCommander,mBackCommander);
// 	}
// 
// // 	template<class _Ty> inline
// // 		void swap(_Ty& _Left, _Ty& _Right)
// // 	{	// exchange values stored at _Left and _Right
// // 		_Ty _Tmp = _Left;
// // 		_Left = _Right, _Right = _Tmp;
// // 	}
// 
// 
// 
// private:
// 	atomic_count
// 	// front 쪽은 항상 사용자의 명령을 받을수 있기에 락을 걸어줘야함
// 	// back쪽은 flushed 되었다면 swap 되기전엔 다시 접근될 일이 없다
// 	spinlock mLock;
// 
// 	// 사용자의 렌더명령을 저장
// 	RenderCommander* mFrontCommander;
// 	// 실제 렌더링 처리
// 	RenderCommander* mBackCommander;
// 
// 	RenderCommander  mCommander[2];
// };









} //////////////////////////////////////////////////////////////////////////






IRenderer& IRenderer::getInstance()
{
	return getRenderer();
}


namespace RenderPasing
{
	bool argToBool(const String& attribute)
	{
		if ( attribute == _mrcT("true") )
			return true;

		mrcASSERT( attribute ==	_mrcT("false") );
		return false;
	}

	int32 argToInt(const String& attribute)
	{
		return _ttoi(attribute.c_str());
	}

	real argToReal(const String& attribute)
	{
		return (real)_tstof(attribute.c_str());
	}

	void* argToPointer( const String& attribute )
	{
		return (void*)_ttoi64(attribute.c_str());
	}

	String getInitArgAttribute(const IRenderer::InitArg& arg,const String& attri)
	{
		IRenderer::InitArg::const_iterator it = arg.find( attri );
		if( it == arg.end() )
			return String();

		return it->second;
	}


	void setInitArgAttribute(IRenderer::InitArg& arg,const String& attri,bool value)
	{
		mrcASSERT(!attri.empty());
		arg[attri] = value ? _mrcT("true") : _mrcT("false");
	}

	void setInitArgAttribute(IRenderer::InitArg& arg,const String& attri,int32 value)
	{
		mrcASSERT(!attri.empty());
		arg[attri] = toString(value);
	}

	void setInitArgAttribute(IRenderer::InitArg& arg,const String& attri,real value)
	{
		mrcASSERT(!attri.empty());
		arg[attri] = toString(value);
	}

	void setInitArgAttribute(IRenderer::InitArg& arg,const String& attri,void* value)
	{
		mrcASSERT(!attri.empty());
		arg[attri] = toString((int64)value);
	}



}







//////////////////////////////////////////////////////////////////////////
} // namespace mrc


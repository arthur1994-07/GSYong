

#ifndef _mrcTextureData_h_
#define _mrcTextureData_h_


#include "mrcConfig.h"

#include "mrcUtil/mrcPolymorphFactory.h"
#include "mrcMath.h"
#include "mrcUtil/mrcFileSystem.h"
#include "mrcShape.h"
#include "mrcAllocatedObject.h"

namespace mrc
{

	class TextureFile;
	typedef map<filesystem::Path,TextureFile> TextureFileMap;

	class ISpriteAnimation;
	typedef mrcAutoPtr<ISpriteAnimation> AP_ISpriteAnimation;


	struct IFrameData : public IPolymorphBaseObject , public AllocatedObject
	{
	protected:
		IFrameData() {}
		static bool msIsClearWhenMakeCache;
	public:
		virtual	~IFrameData() {}
		//////////////////////////////////////////////////////////////////////////
		virtual uint16 getFrameTotalCount() const = 0; 
		virtual uint16 getFileCount() const = 0;
		virtual const filesystem::Path& getFilePath(uint16 index) const = 0;
		virtual int16 getFrameHorizonCount() const = 0;
		virtual Rect_real getFrameRect(int16 frame ,const Rect& regionRect,real realWidth,real realHeight,int16 horizonCount,Point demension) const = 0;

		virtual void makeCache(const TextureFileMap& textureFileMap) = 0;
		virtual size_t getCacheTextureSize() const = 0;
		virtual const HTexture& getCacheTexture(uint16 index) const = 0; 

		virtual void saveXml(TiXmlElement* pParentEle) const = 0;
		virtual void loadXml(TiXmlElement* pParentEle) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	struct IAnimationData : public IPolymorphBaseObject , public AllocatedObject
	{
		enum TIME_TYPE{MAX_FRAME,SECOND};

		IAnimationData() {}
		virtual ~IAnimationData() {}

		// 항상 초단위로 변환해 리턴해줘야함
		virtual real getAnimTime(uint16 aniIndex,uint16 aniTotalCount) const = 0;

		virtual void saveXml(TiXmlElement* pParentEle) const = 0;
		virtual void loadXml(TiXmlElement* pParentEle) = 0;
	};

	class TextureData :  public IPolymorphBaseObject,
		public AllocatedObject, private mrcNonCopyAble  
	{	
	public:
	
	public:
		TextureData()
		{
			mRegionRect.mLeft = mRegionRect.mTop = mRegionRect.mRight = mRegionRect.mBottom = -1;
		}

	public:

		void saveXml(TiXmlElement* pParentEle) const;
		void loadXml(TiXmlElement* pParentEle);

	//	bool isOneFrameAnimation() const { return getFrameTotalCount() > 1; }

		bool isSettingRegionRect() const 
		{
			return mRegionRect.mLeft >= 0 && mRegionRect.mTop >= 0 && mRegionRect.mRight > 0 && mRegionRect.mBottom > 0; 
		}

		void setRegionRect(int32 fileWidth,int32 fileHeight)
		{
			mRegionRect = Rect(0,0,fileWidth,fileHeight);
		}

		const Rect& getRegionRect() const { return mRegionRect; }

		void setSurfaceRect(int32 surfaceWidth,int32 surfaceHeight)
		{
			mrcASSERT(surfaceWidth  > 0);
			mrcASSERT(surfaceHeight > 0);

			mDimension.mX = surfaceWidth;
			mDimension.mY = surfaceHeight;
		}
		
		const Point& getSurfaceRect() { return mDimension; }
		
		real getRealWidth() const
		{
			mrcASSERT(mRegionRect.mLeft  >= 0);
			mrcASSERT(mRegionRect.mLeft < mRegionRect.mRight);

			uint16 realWidth = mRegionRect.mRight - mRegionRect.mLeft;
			if( getFrameTotalCount() == 1 )
				return realWidth;

			return real(realWidth) / real(getFrameHorizonCount());
		}

		real getRealHeight() const
		{
			mrcASSERT(mRegionRect.mTop     >= 0);
			mrcASSERT(mRegionRect.mTop <mRegionRect.mBottom);
			uint16 realHeight = mRegionRect.mBottom - mRegionRect.mTop;
			if( getFrameTotalCount() == 1 )
				return realHeight;

			return real(realHeight) / real(getFrameVirticalCount());
		}

		Rect_real getFrameRect(int16 frame = 0) const 
		{
			mrcASSERT( mDimension.mX > 0 );
			mrcASSERT( mDimension.mY > 0 );

			return mFrame->getFrameRect(frame,mRegionRect,getRealWidth(),getRealHeight(),getFrameHorizonCount(),mDimension);
		}

		int32   getFrameTotalCount() const   { return mFrame->getFrameTotalCount() == 0 ? 1 : mFrame->getFrameTotalCount(); }
		int32   getFrameHorizonCount() const { return mFrame->getFrameHorizonCount() == 0 ? 1 : mFrame->getFrameHorizonCount(); }
		int32   getFrameVirticalCount() const   
		{
			// 이게 세팅이 안되어 있다는건 멀티플 택스쳐를 사용한다는것
// 			if( mFrame->getFrameHorizonCount() == 0 )
// 				return 1;

			if( getFrameHorizonCount() <= getFrameTotalCount() )
				return 1;

			return getFrameTotalCount() / getFrameHorizonCount() + 1;
		}

		uint16 getFileCount() const { return mFrame->getFileCount(); }
		const filesystem::Path& getFilePath(uint16 index) const { return mFrame->getFilePath(index); }

		void makeCacheTexture(const TextureFileMap& textureFileMap) { mFrame->makeCache(textureFileMap); }
	
		void setVertexForTextureUV(bool isVertexForTextureUV) { mIsVertexForTextureUV = isVertexForTextureUV; }
		bool isVertexForTextureUV() const { return mIsVertexForTextureUV; }

//		const IFrameData*      getFrameData() const { return mFrame.get(); }
		HTexture getCacheTexture(uint16 index) const { return mFrame->getCacheTexture(index); }

		real getAnimTime(uint16 index) const { return mAnim->getAnimTime( index,getFrameTotalCount() ); }
		AP_ISpriteAnimation createSpriteAnim() const;

		const vector<Triangle>& getHitRegion() const { return mHitRegion; }
	private:
		// 실제 파일크기와 로드된 텍스쳐의 크기가 같다 (이게 중요한 이유는 텍스쳐를 U,V값을 조절해 흐르게 하려고 할때)
		// 문제가 될수 있으므로... 이게 true 가 아니면 직접 텍스쳐를 흐르게 폴리곤을 만들어야함
		bool mIsVertexForTextureUV;

		// 택스쳐 전체 크기
		Point mDimension;
		// 전체 텍스쳐중 자기가 차지하는 영역
		Rect  mRegionRect;

		// 판넬의 확장간격
		Rect mPanelExtentStep;

		vector<Triangle> mHitRegion;
		
		typedef mrcAutoPtr<IFrameData> AP_IFrame;
		AP_IFrame mFrame;
	
		typedef mrcAutoPtr<IAnimationData> AP_IAnim;
		AP_IAnim mAnim;
		//////////////////////////////////////////////////////////////////////////
	};
}



#endif

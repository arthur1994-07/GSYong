
#include "mrcPrecompiled.h"

#include "mrcTextureData.h"

#include "mrcTiXml.h"

#include "mrcUtil/mrcFileSystem.h"
#include "mrcTextureMgr.h"
#include "mrcSprite.h"


namespace mrc
{
	bool IFrameData::msIsClearWhenMakeCache = false;


#define MIRACLE_XML_NAME(X) _mrcT(#X),m ## X

#define MIRACLE_XML_SERIALIZE_FrameMultipleTexture(SERIALIZE) \
	SERIALIZE(pDataEle,MIRACLE_XML_NAME(TexturePaths),_mrcT("Path"));

	class FrameMultipleTextureData : public IFrameData 
	{
	public:
		virtual uint16 getFrameTotalCount() const mrcOVERRIDE { return getFileCount(); }

		virtual uint16 getFileCount() const mrcOVERRIDE
		{
			return (uint16)mTexturePaths.size();;
		}

		virtual const filesystem::Path& getFilePath(uint16 index) const mrcOVERRIDE 
		{
			mrcASSERT( index < mTexturePaths.size() );
			return mTexturePaths[index]; 
		}

		virtual int16 getFrameHorizonCount() const mrcOVERRIDE { return 0; }

		virtual Rect_real getFrameRect(int16 frame ,const Rect& regionRect,real realWidth,real realHeight,int16 horizonCount,Point demension) const
		{
			Rect_real rect;
			rect.mLeft   = 0; 
			rect.mTop    = 0;  
			rect.mRight  = real(regionRect.mRight)  / demension.mX;
			rect.mBottom = real(regionRect.mBottom) / demension.mY;
			return rect;
		}

		virtual void makeCache(const TextureFileMap& textureFileMap) mrcOVERRIDE
		{
			for (vector<filesystem::Path>::const_iterator it = mTexturePaths.begin(); it != mTexturePaths.end(); ++it)
			{
				const filesystem::Path& path = *it;
				TextureFileMap::const_iterator fileIt = textureFileMap.find(path);
				mrcASSERT( textureFileMap.end() != fileIt );
				const TextureFile& file = fileIt->second; 
				mCacheTextures.push_back(file.getHandle());
			}

			if( msIsClearWhenMakeCache )
			{
				// 캐쉬가 끝났으면 mTexturePaths 는 필요 없겠지
				mTexturePaths.clear();
			}
		}

		virtual size_t getCacheTextureSize() const mrcOVERRIDE { return mCacheTextures.size(); }

		virtual const HTexture& getCacheTexture(uint16 index) const mrcOVERRIDE
		{
			mrcASSERT( index < mCacheTextures.size() );
			return mCacheTextures[index];
		}

		virtual void saveXml(TiXmlElement* pDataEle) const mrcOVERRIDE
		{
			MIRACLE_XML_SERIALIZE_FrameMultipleTexture(tixml::insertElement);
		}

		virtual void loadXml(TiXmlElement* pDataEle) mrcOVERRIDE
		{
			MIRACLE_XML_SERIALIZE_FrameMultipleTexture(tixml::getElement);
		}

	private:
		// 택스쳐 핸들
		vector<filesystem::Path> mTexturePaths;
		vector<HTexture> mCacheTextures;
	};
#undef MIRACLE_XML_SERIALIZE_FrameMultipleTexture


#define  MIRACLE_XML_SERIALIZE_FrameDivideOneTexture(SERIALIZE) \
	SERIALIZE(pDataEle,MIRACLE_XML_NAME(FrameTotalCount));\
	SERIALIZE(pDataEle,MIRACLE_XML_NAME(FrameHorizonCount));\
	SERIALIZE(pDataEle,MIRACLE_XML_NAME(TexturePath));

	class FrameDivideOneTextureData : public IFrameData 
	{
	public:
		FrameDivideOneTextureData() 
		{
			makeInvalidHandle(mCacheTexture); 
			mFrameTotalCount = 0;
			mFrameHorizonCount = 0;

		}

		virtual uint16 getFrameTotalCount() const mrcOVERRIDE 
		{
			mrcASSERT(mFrameTotalCount > 0);
			return mFrameTotalCount; 
		}

		virtual uint16 getFileCount() const mrcOVERRIDE
		{
			return 1;
		}

		virtual const filesystem::Path& getFilePath(uint16 ) const mrcOVERRIDE
		{
			return mTexturePath;
		}

		virtual int16 getFrameHorizonCount() const mrcOVERRIDE 
		{
			mrcASSERT(mFrameHorizonCount > 0);
			return mFrameHorizonCount; 
		}

		virtual void makeCache(const TextureFileMap& textureFileMap) mrcOVERRIDE
		{
			TextureFileMap::const_iterator it = textureFileMap.find(mTexturePath);
			mrcASSERT( textureFileMap.end() != it );
			const TextureFile& file = it->second; 
			mCacheTexture = file.getHandle();
		}


		virtual Rect_real getFrameRect(int16 frame,const Rect& regionRect,real realWidth,real realHeight,int16 horizonCount,Point demension) const
		{
			Rect_real rect;
			rect.mLeft   = real( frame % horizonCount ) * realWidth  / real(demension.mX) + (regionRect.mLeft/real(demension.mX)); 
			rect.mTop    = real( frame / horizonCount ) * realHeight / real(demension.mY) + (regionRect.mTop/real(demension.mY));  
			rect.mRight  = rect.mLeft + (realWidth  / demension.mX);
			rect.mBottom = rect.mTop  + (realHeight / demension.mY);
			return rect;
		}

		virtual size_t getCacheTextureSize() const mrcOVERRIDE { return 1; }

		virtual const HTexture& getCacheTexture(uint16 ) const mrcOVERRIDE
		{
			return mCacheTexture;
		}

		virtual void saveXml(TiXmlElement* pDataEle) const mrcOVERRIDE
		{
			MIRACLE_XML_SERIALIZE_FrameDivideOneTexture(tixml::insertElement);
		}

		virtual void loadXml(TiXmlElement* pDataEle) mrcOVERRIDE
		{
			MIRACLE_XML_SERIALIZE_FrameDivideOneTexture(tixml::getElement);
		}

	private:
		//  애니메이션 갯수 (1 부터 시작하는 단위를 가진다 )
		uint16    mFrameTotalCount; 
		// 가로로 나뉜수
		uint8     mFrameHorizonCount;
		filesystem::Path  mTexturePath;
		HTexture  mCacheTexture;
	};	

#undef MIRACLE_XML_SERIALIZE_FrameDivideOneTexture



	static inline real changeTimeToTime(IAnimationData::TIME_TYPE type ,real value)
	{
		if( type == IAnimationData::SECOND )
		{
			mrcASSERT(value > 0);
			return value;
		}
		else if( type == IAnimationData::MAX_FRAME )
		{
			mrcASSERT(value > 0);
			return value/real(30);
		}

		mrcASSERT(false);
		return 0;
	}

	static String timeTypeToString(IAnimationData::TIME_TYPE type)
	{
		if( type == IAnimationData::SECOND )
			return _mrcT("Second");
		else if( type == IAnimationData::MAX_FRAME )
			return _mrcT("MaxFrame");

		mrcASSERT(false);
		return String();
	}

	static IAnimationData::TIME_TYPE StringToTimeType(const String& typeString)
	{
		if( typeString == _mrcT("Second"))
			return IAnimationData::SECOND;
		else if( typeString == _mrcT("MaxFrame"))
			return IAnimationData::MAX_FRAME;

		mrcASSERT(false);
		return IAnimationData::SECOND;
	}

#define  MIRACLE_XML_SERIALIZE_AnimOneIntervalData(SERIALIZE) \
	SERIALIZE(pDataEle,MIRACLE_XML_NAME(FrameTotalTime));

	class AnimOneIntervalData : public IAnimationData 
	{
	public:
		AnimOneIntervalData() { mFrameTotalTime = 0; }

		virtual real getAnimTime(uint16 aniIndex,uint16 aniTotalCount) const
		{
			mrcASSERT(aniTotalCount > 0);
			return changeTimeToTime(mTimeType,mFrameTotalTime) / aniTotalCount;
		}

		virtual void saveXml(TiXmlElement* pDataEle) const mrcOVERRIDE
		{
			tixml::insertElement(pDataEle,_mrcT("TimeType"), timeTypeToString(mTimeType) );
			MIRACLE_XML_SERIALIZE_AnimOneIntervalData(tixml::insertElement);
		}

		virtual void loadXml(TiXmlElement* pDataEle) mrcOVERRIDE
		{
			String timeTypeString;
			tixml::getElement(pDataEle,_mrcT("TimeType"), timeTypeString);
			mTimeType = StringToTimeType(timeTypeString);

			MIRACLE_XML_SERIALIZE_AnimOneIntervalData(tixml::getElement);
		}

	private:
		TIME_TYPE mTimeType;
		real   mFrameTotalTime;	
	};

#undef MIRACLE_XML_SERIALIZE_AnimOneIntervalData


	class AnimMultiIntervalData : public IAnimationData 
	{

		virtual real getAnimTime(uint16 aniIndex,uint16 aniTotalCount) const
		{
			mrcASSERT_MSG(aniTotalCount == mAnimFramePerTimes.size(), 
				_mrcT(" must be same -> frame total count == animation total count") );
			mrcASSERT( aniIndex < mAnimFramePerTimes.size() );
			return changeTimeToTime(mTimeType, mAnimFramePerTimes[aniIndex]);
		}

		virtual void saveXml(TiXmlElement* pDataEle) const mrcOVERRIDE
		{

		}

		virtual void loadXml(TiXmlElement* pDataEle) mrcOVERRIDE
		{

		}

	private:
		TIME_TYPE mTimeType;
		vector<real> mAnimFramePerTimes;
	};

	class AnimNoData : public IAnimationData 
	{
	public:
		AnimNoData() {}

		virtual real getAnimTime(uint16 aniIndex,uint16 aniTotalCount) const
		{
			return 0;
		}

		virtual void saveXml(TiXmlElement* pDataEle) const mrcOVERRIDE
		{
		}

		virtual void loadXml(TiXmlElement* pDataEle) mrcOVERRIDE
		{
		}
	};


	MIRACLE_REGISTER_POLYMORPH(TextureData);
	MIRACLE_REGISTER_POLYMORPH(FrameMultipleTextureData);
	MIRACLE_REGISTER_POLYMORPH(FrameDivideOneTextureData);
	MIRACLE_REGISTER_POLYMORPH(AnimOneIntervalData);
	MIRACLE_REGISTER_POLYMORPH(AnimMultiIntervalData);
	MIRACLE_REGISTER_POLYMORPH(AnimNoData);


#define MIRACLE_XML_SERIALIZE_TextureData(SERIALIZE) \
	SERIALIZE(pDataEle,MIRACLE_XML_NAME(RegionRect)); \
	SERIALIZE(pDataEle,MIRACLE_XML_NAME(PanelExtentStep)); \
	SERIALIZE(pDataEle,_mrcT("HitRegion"),mHitRegion,_mrcT("Trianle")); \
	SERIALIZE(pDataEle,MIRACLE_XML_NAME(Frame)); \
	SERIALIZE(pDataEle,MIRACLE_XML_NAME(Anim));

	void TextureData::saveXml( TiXmlElement* pDataEle ) const
	{
		MIRACLE_XML_SERIALIZE_TextureData(tixml::insertElement);
	}

	void TextureData::loadXml( TiXmlElement* pDataEle )
	{
		MIRACLE_XML_SERIALIZE_TextureData(tixml::getElement);
	}


	//////////////////////////////////////////////////////////////////////////
#undef MIRACLE_XML_SERIALIZE_TextureData
#undef MIRACLE_XML_NAME



	mrc::AP_ISpriteAnimation TextureData::createSpriteAnim() const
	{
		return SpriteAnimationFactory::getInstance().createObject(*mAnim);
	}


}
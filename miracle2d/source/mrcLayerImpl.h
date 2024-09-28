


#ifndef _mrcLayerImpl_h_
#define _mrcLayerImpl_h_

#include "mrcConfig.h"

#include "mrcLayer.h"
#include "mrcAllocatedObject.h"

#include "mrcSprite.h"
#include "mrcVector3.h"
#include "mrcMatrix4.h"
#include "mrcMath.h"
#include "mrcUtil/mrcVectorOperator.h"

#include "mrcUtil/mrcScriptShape.h"
#include "mrcEaseQueue.h"


namespace mrc
{
	namespace ease
	{
		typedef Queue< ease::DataEx< Vector2> > QueueVector2Ex;
		typedef Queue< ease::Data< Vector2> > QueueVector2;
		typedef Queue< ease::Data< Vector3> > QueueVector3;
		typedef Queue< ease::Data< real> > QueueSingle;

		struct QueueSet : public AllocatedObject
		{
			mrcAutoPtr< QueueVector2Ex > mPositionQ;
			mrcAutoPtr< QueueSingle >	 mRotationQ;
			mrcAutoPtr< QueueVector2 >	 mScaleQ;
			mrcAutoPtr< QueueSingle >	 mPivotRotationQ;
			mrcAutoPtr< QueueVector3 >   mRGBQ;
			mrcAutoPtr< QueueSingle >	 mAlphaQ;
		};
	}

	class LayerImpl;
	typedef mrcWeakenPtr<LayerImpl> WP_LayerImpl;
	typedef mrcSharedPtr<LayerImpl> SP_LayerImpl;

	class LayerImpl : public ILayer, public AllocatedObject
	{
		MIRACLE_FRIEND_FACTORY;
	protected:
		LayerImpl();
	public:
		static const uint32 TYPE = TYPE_SCOPE_INTERNAL + 1;

		virtual ~LayerImpl();

		virtual const uint32 getType() const mrcOVERRIDE { return TYPE; }
		virtual	const LayerID& getId() const mrcOVERRIDE { return mID; }

		virtual bool messageProcedure(const Message& msg) mrcOVERRIDE;
		virtual const Matrix4& getMatrix() { return mCacheMatrix; }

//		virtual void update(real elapsedTime) mrcOVERRIDE;

		virtual void render() const mrcOVERRIDE;

		virtual void setLayerListInfo(const ListInfo& info) mrcOVERRIDE { mLayerListInfo = info; }
		virtual const ListInfo& getLayerListInfo() const mrcOVERRIDE { return mLayerListInfo; }

		virtual ISprite* getSprite() mrcOVERRIDE { return mSprite.get(); }
		virtual ILayer* getParentLayer() const mrcOVERRIDE { return mLayerListInfo.mParentLayer; }


//		virtual const LayerList& getChild() const { return mChildList; }
		virtual LayerListIterWrapper getChild();
		void getChildAll(vector<SP_Layer>* pLayers);


		virtual void pushBackChild(const SP_Layer& spLayer);

		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		bool setId(const LayerID& id);

		void attach(AP_ISprite apSprite) 
		{
			// 비어 있어야한다 , 만약 존재하는데 삭제 되어 버리면 SqSprite 형 스크립트 객체는 오류가 날수 있다
			mrcASSERT(!mSprite.get());

			mrcASSERT(apSprite.get());
			mSprite = apSprite; 
			mSprite->setLayer(this);
		}
		//////////////////////////////////////////////////////////////////////////

		SP_LayerImpl createChild( const LayerID& id )
		{
			return createChildLayer<LayerImpl>(this,id);
		}

		template<class SPRITE>
		SP_LayerImpl createChild( const LayerID& id )
		{
			return createChildLayer<LayerImpl,SPRITE>(this,id);
		}


		virtual void deleteChildAll();

		virtual void destroy() mrcOVERRIDE;

		//////////////////////////////////////////////////////////////////////////
		virtual void setPosition(const Vector2& pos);
		void setRotation(real rotation);
		void setPivotRotation(real rotation);	
		void setScale(const Vector2& scale);
		void setScale(real scale);
		void setRGB(real r,real g,real b);
		void setAlpha(real alpha);

		void setPivotAbs(const Vector2& absPivot) { mPivot = absPivot-getPosition(); }
		void setPivot(const Vector2& pivot) { mPivot = pivot; }
		const Vector2& getPivot() { return mPivot; }

		void setUVVelocity(const Vector2& uv) { mUVVelocity = uv; }
		Vector2 getUVVelocity() { return mUVVelocity; }

		void    setMoveVelocity(const Vector2& vel) { mMoveVelocity = vel; }
		Vector2 getMoveVelocity() const { return mMoveVelocity; }

		void setRotationVelocity(real vel) { mRotationVelocity = vel; }
		real getRotationVelocity() const { return mRotationVelocity; }

		void setPivotRotationVelocity(real vel) { mPivotRotationVelocity = vel; }
		real getPivotRotationVelocity() const { return mPivotRotationVelocity; }

		virtual Vector2 getPosition() const mrcOVERRIDE { return mPosition; }
		real	getRotation() const { return mRotation; }
		real    getPivotRotation() const { return mPivotRotation; }
		Vector2 getScale()    const { return mScale; }
		real	getAlpha() const { return mColorAlpha; }
		void	getRGB( real* r,real* g,real* b ) const 
		{ 
			*r = mColorRed; *g = mColorGreen; *b = mColorBlue; 
		}


		void pushLinePositionQ(const Vector2& pos,real totalTime,ease::METHOD method);
		void pushBezierPositionQ(const Vector2& middle1,const Vector2& middle2,
			const Vector2& end,real totalTime);
		void setPositionQLoop(bool isLoop);
		void clearPositionQ();

		void pushRotationQ(real rot,real totalTime,ease::METHOD method);
		void setRotationQLoop(bool isLoop);
		void clearRotationQ();

		void pushScaleQ(const Vector2& scale,real totalTime,ease::METHOD method);
		void setScaleQLoop(bool isLoop);
		void clearScaleQ();

		void pushPivotRotationQ(real rot,real totalTime,ease::METHOD method);
		void setPivotRotationQLoop(bool isLoop);
		void clearPivotRotationQ();

		void pushRGBQ(const Vector3& rgbVector,real totalTime,ease::METHOD method);
		void setRGBQLoop(bool isLoop);
		void clearRGBQ();

		void pushAlphaQ(real alpha,real totalTime,ease::METHOD method);
		void setAlphaQLoop(bool isLoop);
		void clearAlphaQ();

		//////////////////////////////////////////////////////////////////////////	
		virtual SP_Layer getThis() const
		{		
			mrcASSERT(!isRoot());
			return *getLayerListInfo().mLayerListIter; 
		}

		virtual void registerFontDraw(const IFontDraw* pFontDraw) const mrcOVERRIDE { mFontDrawVec.push_back(pFontDraw); }

		virtual void setVisibility(bool isVisible) mrcOVERRIDE { isVisible ? setAlpha(1) : setAlpha(0); }
		virtual bool isVisible() const mrcOVERRIDE { return getAlpha() > 0; }

		virtual void enableFirstClickEvent(bool enable) mrcOVERRIDE { mSprite->enableFirstClickEvent(enable); }
//		virtual bool isEnableFirstClickEvent() const mrcOVERRIDE { return mSprite->isEnableFirstClickEvent(); }

	private:
		bool isNoUpdate() { return false; }

		virtual void updateStaticLogic(real elapsedTime) mrcOVERRIDE;

		virtual Matrix4 makeCacheMatrix() mrcOVERRIDE;
		virtual void makeAabb() mrcOVERRIDE;

		//////////////////////////////////////////////////////////////////
		bool sizeTest() const;
		bool cullingTest() const;
		bool alphaTest() const { return true;}


		const AxisAlignedBox& getAabb() { return mSprite->getAabb(); }

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		
		
		virtual void invalidCacheMatrix();

		ease::QueueSet& getEaseQueueSet();

		ease::QueueVector2Ex& getPositionQ();
		ease::QueueVector2Ex* getPositionQPtr();

		ease::QueueSingle& getRotationQ();
		ease::QueueSingle* getRotationQPtr();
	
		ease::QueueVector2& getScaleQ();
		ease::QueueVector2* getScaleQPtr();

		ease::QueueSingle& getPivotRotationQ();
		ease::QueueSingle* getPivotRotationQPtr();

		ease::QueueVector3& getRGBQ();
		ease::QueueVector3* getRGBQPtr();

		ease::QueueSingle& getAlphaQ();
		ease::QueueSingle* getAlphaQPtr();

	protected:
		LayerID    mID;
//		ILayer*    mParentLayer;

		AP_ISprite mSprite;
		Vector2 mPosition;		   // 일단 x,y만 쓴다
		real  mRotation;		   // z축 회전만
		Vector2	mScale;			   // x,y 축 크기
		Vector2 mPivot;			   // 피봇을 중심으로 회전
		real  mPivotRotation;    // 피봇을 중심으로 회전
		Vector2 mUV;

		Vector2 mUVVelocity;
		Vector2 mMoveVelocity;
		real mRotationVelocity;
		real mPivotRotationVelocity;

		real mColorRed;
		real mColorGreen;
		real mColorBlue;
		real mColorAlpha;

		bool	mInvalidCacheMatrix;
		bool    mInvalidAabb;

		Matrix4 mCacheMatrix;

		LayerList  mChildList;
		ListInfo   mLayerListInfo;

//		tween::Tweener   mTweener;

		mrcAutoPtr< ease::QueueSet > mEaseQueueSet;

		mutable vector<const IFontDraw* > mFontDrawVec;
		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
	

		static deque<SP_Layer> msTraverseQueue;
		typedef hash_set<LayerID> LayerIDSet;
		static hash_set<LayerID> mOverloadIDCheckSet;
	};

}


#endif








#include "mrcPrecompiled.h"
#include "mrcUtil/mrcSingleton.h"

#include "mrcLayerImpl.h"
#include "mrcScript.h"

namespace mrc
{

	//////////////////////////////////////////////////////////////////////////
	
	WP_Layer ILayer::msModalLayer;
	ILayer* ILayer::msFirstClickEventLayer = NULL;

	//////////////////////////////////////////////////////////////////////////

	deque<SP_Layer> LayerImpl::msTraverseQueue;
	LayerImpl::LayerIDSet LayerImpl::mOverloadIDCheckSet;

	//////////////////////////////////////////////////////////////////////////


	
	LayerFactory& LayerFactory::getInstance()
	{
		typedef Loki::SingletonHolder<LayerFactory,Loki::CreateUsingNew,
			Loki::LongevityLifetime::DieIndependencyMgr > LayerFactoryS;

		return LayerFactoryS::Instance();
	}

	LayerFactory::LayerFactory()
	{
		registerCreator<LayerImpl>();
	}


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	enum TWEENER_TYPE{ TWEENER_POSITION,TWEENER_ROTATION,TWEENER_SCALE,TWEENER_PIVOT_ROTATION,
		TWEENER_RGB,TWEENER_ALPHA,TWEENER_COUNT };



	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	

	LayerImpl::LayerImpl()
	{
		mPosition = Vector2::ZERO;
		mRotation = 0;
		mScale    = Vector2::UNIT_SCALE;
		mPivot    = Vector2::ZERO;
		mPivotRotation = 0;

		mColorRed = mColorGreen = mColorBlue = mColorAlpha = real(1);

		mUVVelocity = Vector2::ZERO;
		mMoveVelocity = Vector2::ZERO;
		mRotationVelocity = mPivotRotationVelocity = 0;


		invalidCacheMatrix();
//z		mTweener.setListener(this);
	}


	LayerImpl::~LayerImpl()
	{
		// 자식부터 제거, 혹시 부모 포인터를 참조 할 일이 있으려나?
		mChildList.clear();

		if( !isRoot() )
		{
			LayerIDSet::iterator it = mOverloadIDCheckSet.find(mID);
			mrcASSERT(it != mOverloadIDCheckSet.end() );		
			mOverloadIDCheckSet.erase(it);
		}
	}


	bool LayerImpl::messageProcedure( const Message& msg )
	{
		for( LayerList::reverse_iterator it = mChildList.rbegin(); it != mChildList.rend(); ++it )
		{
			ILayer* pLayer = (*it).get();
			if( pLayer->messageProcedure(msg) )
				return true;
		}

		if( mSprite.get() )
		{
			return mSprite->messageProcedure(msg);
		}		

		return false;
	}

	void LayerImpl::render() const
	{
		//////////////////////////////////////////////////////////////////////////
		// 멀티쓰레드 처리 가능한 구간	
		if( !sizeTest() )
			return;

		if( !alphaTest() )
			return;

		if( !cullingTest() )
			return;

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////
		if(mSprite.get())
			mSprite->render(mCacheMatrix);

		for (LayerList::const_iterator it = mChildList.begin(); it != mChildList.end(); ++it)
			(*it)->render();


		//////////////////////////////////////////////////////////////////////////
		// const 지만 mutable로 지정되어 있다 

		if( !mFontDrawVec.empty() )
		{
			// font를 그리기전엔 항상 미리 flush 해서 폰트가 최상위로...
			IRenderer::getInstance().flush();
			for (size_t i=0; i < mFontDrawVec.size(); ++i)
				mFontDrawVec[i]->renderFont();

			mFontDrawVec.clear();
		}

		//////////////////////////////////////////////////////////////////////////

	}


	Matrix4 LayerImpl::makeCacheMatrix() 
	{
		if( !mInvalidCacheMatrix )
			return mCacheMatrix;

		//////////////////////////////////////////////////////////////////////////
		// 여기 내용은  레이어마다 별개로 처리할수 있다 멀티 쓰레드 가능
		// makePreCacheMarix 

		// scale
		Matrix4 scaleMat = Matrix4::IDENTITY;
		scaleMat.setScale( Vector3(mScale.x,mScale.y,1) );

		// rotation
		Matrix4 rotMat(Quaternion(Degree(mRotation),Vector3::UNIT_Z));

		Matrix4 pivotMat = Matrix4::IDENTITY;	
		Vector3 pivot;
		pivot = -Vector3(mPivot.x,mPivot.y,0);//] + toVector3(mPosition);

		pivotMat.setTrans( pivot );

		Matrix4 pivotRotMat(Quaternion(Degree(mPivotRotation),Vector3::UNIT_Z));

		Matrix4 transMat = Matrix4::IDENTITY;
		transMat.setTrans( -toVector3(pivot) + toVector3(mPosition) );
		
		mCacheMatrix = transMat * pivotRotMat * pivotMat * rotMat * scaleMat ;	

		//////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////

		if( getParentLayer() )
			mCacheMatrix = getParentLayer()->makeCacheMatrix() * mCacheMatrix;

		mInvalidCacheMatrix = false;
		return mCacheMatrix;
	}


	void LayerImpl::makeAabb()
	{
		if(mSprite.get())
		{
			mSprite->makeAabb(mCacheMatrix); 
			if( mInvalidAabb )
				mSprite->invalidAabb();
		}
	}


	void LayerImpl::setPosition( const Vector2& pos )
	{
		if( pos == mPosition )
			return;

		invalidCacheMatrix();
		mPosition.x = pos.x; mPosition.y = pos.y;
	}

	void LayerImpl::setRotation( real rotation )
	{
		if( rotation == mRotation)
			return;

		invalidCacheMatrix();
		mRotation = rotation;
		mRotation = fmod(mRotation,360);
	}


	void LayerImpl::setPivotRotation( real rotation )
	{
		if( rotation == mPivotRotation)
			return;

		invalidCacheMatrix();
		mPivotRotation = rotation;
		mPivotRotation = fmod(mPivotRotation,360);
	}

	void LayerImpl::setScale( const Vector2& scale )
	{
		if( scale == mScale)
			return;

		invalidCacheMatrix();
		mScale = scale;
	}

	void LayerImpl::setScale(real scale)
	{
		setScale( Vector2(scale,scale) );		
	}
	
	void LayerImpl::setRGB( real r,real g,real b )
	{
		mColorRed = r;
		mColorGreen = g;
		mColorBlue = b;

		if( mSprite.get() )
			mSprite->setRGB(makeColor(r),makeColor(g),makeColor(b));
	}

	void LayerImpl::setAlpha( real alpha )
	{
		mColorAlpha = alpha;

		if( mSprite.get() )
			mSprite->setAlpha( makeColor(alpha) );
	}


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	void LayerImpl::pushLinePositionQ(const Vector2& pos,real totalTime,ease::METHOD method)
	{
		ease::DataEx<Vector2> data;
		data.mStart = getPosition();
		data.mEnd = data.mStart + pos;
		data.mTotalTime = totalTime;
		data.mFuncEasing = ease::DataEx<Vector2>::getEasingMethod(method);
		data.mLineProcess = true;
		getPositionQ().push(data);
	}

	void LayerImpl::pushBezierPositionQ(const Vector2& middle1,const Vector2& middle2,
		const Vector2& end,real totalTime)
	{
		ease::DataEx<Vector2> data;
		data.mStart = getPosition();
		data.mEnd = data.mStart + end;
		data.mMiddle[0] = data.mStart + middle1;
		data.mMiddle[1] = data.mStart + middle2;
		data.mTotalTime = totalTime;
		
		data.mLineProcess = false;
		getPositionQ().push(data);
	}

	void LayerImpl::setPositionQLoop(bool isLoop)
	{
		getPositionQ().setLoop(isLoop);
	}

	void LayerImpl::clearPositionQ()
	{
		getPositionQ().clear();
	}

	void LayerImpl::pushRotationQ(real rot,real totalTime,ease::METHOD method)
	{
		ease::Data<real> data;
		data.mStart = getRotation();
		data.mEnd = rot;
		data.mTotalTime = totalTime;
		data.mFuncEasing = ease::Data<real>::getEasingMethod(method);
		getRotationQ().push(data);
	}

	void LayerImpl::setRotationQLoop(bool isLoop)
	{
		getRotationQ().setLoop(isLoop);
	}
	
	void LayerImpl::clearRotationQ()
	{
		getRotationQ().clear();
	}

	void LayerImpl::pushScaleQ(const Vector2& scale,real totalTime,ease::METHOD method)
	{
		ease::Data<Vector2> data;
		data.mStart = getScale();
		data.mEnd = scale;
		data.mTotalTime = totalTime;
		data.mFuncEasing = ease::Data<Vector2>::getEasingMethod(method);
		getScaleQ().push(data);
	}

	void LayerImpl::setScaleQLoop(bool isLoop)
	{
		getScaleQ().setLoop(isLoop);
	}

	void LayerImpl::clearScaleQ()
	{
		getScaleQ().clear();
	}

	void LayerImpl::pushPivotRotationQ(real rot,real totalTime,ease::METHOD method)
	{
		ease::Data<real> data;
		data.mStart = getPivotRotation();
		data.mEnd = rot;
		data.mTotalTime = totalTime;
		data.mFuncEasing = ease::Data<real>::getEasingMethod(method);
		getPivotRotationQ().push(data);
	}

	void LayerImpl::setPivotRotationQLoop(bool isLoop)
	{
		getPivotRotationQ().setLoop(isLoop);
	}

	void LayerImpl::clearPivotRotationQ()
	{
		getPivotRotationQ().clear();
	}

	void LayerImpl::pushRGBQ(const Vector3& rgbVector,real totalTime,ease::METHOD method)
	{
		ease::Data<Vector3> data;
		data.mStart = Vector3(mColorRed,mColorGreen,mColorBlue);
		data.mEnd = rgbVector;
		data.mTotalTime = totalTime;
		data.mFuncEasing = ease::Data<Vector3>::getEasingMethod(method);
		getRGBQ().push(data);
	}
	void LayerImpl::setRGBQLoop(bool isLoop)
	{
		getRGBQ().setLoop(isLoop);
	}

	void LayerImpl::clearRGBQ()
	{
		getRGBQ().clear();
	}

	void LayerImpl::pushAlphaQ(real alpha,real totalTime,ease::METHOD method)
	{
		ease::Data<real> data;
		data.mStart = getAlpha();
		data.mEnd = alpha;
		data.mTotalTime = totalTime;
		data.mFuncEasing = ease::Data<real>::getEasingMethod(method);
		getAlphaQ().push(data);
	}

	void LayerImpl::setAlphaQLoop(bool isLoop)
	{
		getAlphaQ().setLoop(isLoop);
	}

	void LayerImpl::clearAlphaQ()
	{
		getAlphaQ().clear();
	}

	bool LayerImpl::sizeTest() const
	{
		return mScale.x > 0 && mScale.y > 0;
	}

	bool LayerImpl::cullingTest() const
	{
		return true;
	}

	template<class T,class Y>
	static bool updateEaseQ(T* Q,Y& value,real elapsedTime)
	{
		if( Q )
		{
			if( Q->update(elapsedTime) )
			{
				value = Q->getUpdatedValue();
				return true;
			}
		}

		return false;
	}

	void LayerImpl::updateStaticLogic( real elapsedTime )
	{
		// static 로직은 멀티 쓰레드 처리 가능
		if( isNoUpdate() )
			return;

		//////////////////////////////////////////////////////////////////////////
		// 속도 처리
		if( mSprite.get() )
			mSprite->setUVFlow( mSprite->getUVFlow() + mUVVelocity*elapsedTime );

		setPosition(getPosition() + mMoveVelocity*elapsedTime);
		setRotation(getRotation() + mRotationVelocity*elapsedTime);
		setPivotRotation(getPivotRotation() + mPivotRotationVelocity*elapsedTime);

		//////////////////////////////////////////////////////////////////////////
		// 로직 처리 제일 먼저 처리해준다
//		mTweener.step(elapsedTime);

		if( mEaseQueueSet.get() )
		{
			Vector2 pos;
			real rot;
			Vector2 scale;
			real pivotRot;
			Vector3 colorRGB;
			real alpha;

			if( updateEaseQ(getPositionQPtr(),pos,elapsedTime) )
				setPosition(pos);

			if( updateEaseQ(getRotationQPtr(),rot,elapsedTime) )
				setRotation(rot);

			if( updateEaseQ(getScaleQPtr(),scale,elapsedTime) )
				setScale(scale);

			if( updateEaseQ(getPivotRotationQPtr(),pivotRot,elapsedTime) )
				setPivotRotation(pivotRot);
						
			if( updateEaseQ(getRGBQPtr(),colorRGB,elapsedTime) )
			{
				setRGB(colorRGB.x,colorRGB.y,colorRGB.z);
			}

			if( updateEaseQ(getAlphaQPtr(),alpha,elapsedTime) )
				setAlpha(alpha);

		}



		//////////////////////////////////////////////////////////////////////////

		if( mSprite.get() )
		{
			mSprite->update(elapsedTime);
		}


	
	}


// 	bool LayerImpl::updateThread( real elapsedTime )
// 	{
// 		if( isNoUpdate() )
// 			return false;
// 
// 		if( mLayerThread.get() )
// 		{
// 			if( mLayerThread->resume(elapsedTime) == LayerThread::STATE_RUN )
// 				return true;
// 		}
// 
// 		return false;
// 	}

	//////////////////////////////////////////////////////////////////////////

	void LayerImpl::pushBackChild( const SP_Layer& spLayer )
	{
		mChildList.push_back(spLayer);
		spLayer->setLayerListInfo( ILayer::ListInfo(&mChildList,--mChildList.end(),this) );
	}

	void LayerImpl::deleteChildAll()
	{
		mChildList.clear();
	}

	void LayerImpl::destroy()
	{
		// 이렇게 되면 제거 되므로 this 에 관한것 아무것도 호출하지 않는다
		mLayerListInfo.destroy();
	}

	//////////////////////////////////////////////////////////////////////////
	VectorIterator<LayerList> LayerImpl::getChild() 
	{
		return VectorIterator<LayerList>(mChildList);
	}


	void LayerImpl::getChildAll( vector<SP_Layer>* pLayers )
	{
		// level order traverse  C로 배우는 알고리즘 246 페이지 참조
		if( isRoot() )
		{
			msTraverseQueue.insert(msTraverseQueue.end(),mChildList.begin(),mChildList.end());			
//			for ( LayerList::iterator it = mChildList.begin(); it != mChildList.end(); ++it )
//				msTraverseQueue.push_back( (*it).get() );
		}
		else
			msTraverseQueue.push_back(getThis());

		while( !msTraverseQueue.empty() )
		{
			//////////////////////////////////////////////////////////////////////////
			// visit
			SP_Layer spLayer = msTraverseQueue.front();
			pLayers->push_back( spLayer );
			msTraverseQueue.pop_front();
			//////////////////////////////////////////////////////////////////////////
			if( spLayer->getType() == LayerImpl::TYPE )
			{
				LayerImpl* pLayerImpl = static_cast<LayerImpl*>(spLayer.get());
				msTraverseQueue.insert(msTraverseQueue.end(),pLayerImpl->mChildList.begin(),pLayerImpl->mChildList.end());
//				for ( LayerList::iterator it = pLayerImpl->mChildList.begin(); it != pLayerImpl->mChildList.end(); ++it )
//					msTraverseQueue.push_back( (*it).get() );

			}
		}

	}

	bool LayerImpl::setId( const LayerID& id )
	{
		std::pair<LayerIDSet::iterator,bool> ret = mOverloadIDCheckSet.insert(id);
		if( !ret.second )
			return false;

		mID = id;
		return true;
	}


	//////////////////////////////////////////////////////////////////////////

// 	void LayerImpl::onStart( tween::TweenerParam& param )
// 	{
// 		
// 	}
// 
// 	void LayerImpl::onStep( tween::TweenerParam& param )
// 	{
// 		switch(param.getType())
// 		{
// 		case TWEENER_POSITION:
// 		case TWEENER_ROTATION:
// 		case TWEENER_SCALE:
// 		case TWEENER_PIVOT_ROTATION:
// 			invalidCacheMatrix();
// 			break;
// 		case TWEENER_RGB:
// 			if(mSprite.get())
// 				mSprite->setRGB(makeColor(mColorRed),makeColor(mColorGreen),makeColor(mColorBlue));
// 			break;
// 		case TWEENER_ALPHA:
// 			if(mSprite.get())
// 				mSprite->setAlpha(makeColor(mColorAlpha));
// 			break;
// 		default:
// 			{
// 				mrcASSERT(false);
// 				break;
// 			}
// 		}
// 
// 	}
// 
// 	void LayerImpl::onComplete( tween::TweenerParam& param )
// 	{
// 		onStep(param);
// 	}



	//////////////////////////////////////////////////////////////////////////

	ease::QueueSet& LayerImpl::getEaseQueueSet()
	{
		if( !mEaseQueueSet.get() )
			mEaseQueueSet.reset( mrcNEW ease::QueueSet );

		return *mEaseQueueSet;
	}


	ease::QueueVector2Ex& LayerImpl::getPositionQ()
	{
		if( !getEaseQueueSet().mPositionQ.get() )
			getEaseQueueSet().mPositionQ.reset( mrcNEW ease::QueueVector2Ex);

		return *getEaseQueueSet().mPositionQ;
	}

	ease::QueueVector2Ex* LayerImpl::getPositionQPtr()
	{
		mrcASSERT(mEaseQueueSet.get());
		return mEaseQueueSet->mPositionQ.get();
	}

	ease::QueueSingle& LayerImpl::getRotationQ()
	{
		if( !getEaseQueueSet().mRotationQ.get() )
			getEaseQueueSet().mRotationQ.reset( mrcNEW ease::QueueSingle);

		return *getEaseQueueSet().mRotationQ;
	}

	ease::QueueSingle* LayerImpl::getRotationQPtr()
	{
		mrcASSERT(mEaseQueueSet.get());
		return mEaseQueueSet->mRotationQ.get();
	}

	ease::QueueVector2& LayerImpl::getScaleQ()
	{
		if( !getEaseQueueSet().mScaleQ.get() )
			getEaseQueueSet().mScaleQ.reset( mrcNEW ease::QueueVector2);

		return *getEaseQueueSet().mScaleQ;
	}

	ease::QueueVector2* LayerImpl::getScaleQPtr()
	{
		mrcASSERT(mEaseQueueSet.get());
		return mEaseQueueSet->mScaleQ.get();
	}

	ease::QueueSingle& LayerImpl::getPivotRotationQ()
	{
		if( !getEaseQueueSet().mPivotRotationQ.get() )
			getEaseQueueSet().mPivotRotationQ.reset( mrcNEW ease::QueueSingle);

		return *getEaseQueueSet().mPivotRotationQ;
	}

	ease::QueueSingle* LayerImpl::getPivotRotationQPtr()
	{
		mrcASSERT(mEaseQueueSet.get());
		return mEaseQueueSet->mPivotRotationQ.get();	
	}

	ease::QueueVector3& LayerImpl::getRGBQ()
	{
		if( !getEaseQueueSet().mRGBQ.get() )
			getEaseQueueSet().mRGBQ.reset( mrcNEW ease::QueueVector3);

		return *getEaseQueueSet().mRGBQ;
	}

	ease::QueueVector3* LayerImpl::getRGBQPtr()
	{
		mrcASSERT(mEaseQueueSet.get());
		return mEaseQueueSet->mRGBQ.get();			
	}

	ease::QueueSingle& LayerImpl::getAlphaQ()
	{
		if( !getEaseQueueSet().mAlphaQ.get() )
			getEaseQueueSet().mAlphaQ.reset( mrcNEW ease::QueueSingle);

		return *getEaseQueueSet().mAlphaQ;
	}

	ease::QueueSingle* LayerImpl::getAlphaQPtr()
	{
		mrcASSERT(mEaseQueueSet.get());
		return mEaseQueueSet->mAlphaQ.get();
	}

	void LayerImpl::invalidCacheMatrix()
	{
		mInvalidCacheMatrix = true;	mInvalidAabb = true;

		for (LayerList::iterator it = mChildList.begin(); it != mChildList.end(); ++it)
			(*it)->invalidCacheMatrix();
	}

	//////////////////////////////////////////////////////////////////////////


}// namespcae mrc



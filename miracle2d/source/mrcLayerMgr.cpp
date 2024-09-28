

#include "mrcPrecompiled.h"


#include "mrcLayerMgr.h"
#include "mrcUtil/mrcSingleton.h"

#include <boost/scope_exit.hpp>

#include "mrcRenderer.h"
#include "mrcLayerImpl.h"
#include "mrcLayerThread.h"
#include "mrcKeyboard.h"

namespace mrc
{
	static LayerMgr* s_pLayerMgr = NULL;

// 	typedef Loki::SingletonHolder<LayerMgr,Loki::CreateUsingNew,
// 		Loki::LongevityLifetime::DieIndependencyMgr > LayerMgrS;

	LayerMgr& LayerMgr::getInstance()
	{
		if( s_pLayerMgr == NULL )
			s_pLayerMgr = mrcNEW LayerMgr;

		return *s_pLayerMgr;
	}

	LayerMgr::LayerMgr()
	{
		//////////////////////////////////////////////////////////////////////////
		// Ȥ�ö� �� 2�� ������ ���� (����ؼ� �� ���)
		static bool isCreated = false;
		mrcASSERT(isCreated == false);
		isCreated = true;
		//////////////////////////////////////////////////////////////////////////

		mRootLayer = LayerFactory::getInstance().createObject<LayerImpl>();
		bool ret = mRootLayer->setId( _mrcT("Root") );
		mrcASSERT(ret);
		mrcASSERT(mRootLayer.get());
		mMsgPreprocedure = NULL;
		// 		mBeginRenderprocedure = NULL;
		// 		mEndRenderprocedure = NULL;

		mMaxLayerSize = 100;

		mIsReverseUpdate = false;
	}

	LayerMgr::~LayerMgr()
	{
	}

	void LayerMgr::newLayerMgr(LayerMgr* pLayerMgr)
	{
		release();
		s_pLayerMgr = pLayerMgr;
	}

	void LayerMgr::release()
	{
		if( s_pLayerMgr )
			delete s_pLayerMgr;
	}




	void LayerMgr::update( real elapsedTime )
	{
		if( elapsedTime == 0 )
			return;

		// ����� ó���� �ƴ϶� ���̾� ������� �̾� �ѹ��� �ϰ� ó�� ��� 
		// ��� ó���� ��Ƽ������ ó���� ��ư� , ����ũ�� ������ ó���Ҷ� ������ �ִ�
		// �����ؼ� ó���ϴ°� ����
		mCacheLayers.clear();
		mCacheLayers.reserve(mMaxLayerSize);

		mRootLayer->getChildAll(&mCacheLayers);

		if( mCacheLayers.size() > mMaxLayerSize )
			mMaxLayerSize = mCacheLayers.size();

		if (mIsReverseUpdate)
			std::reverse(mCacheLayers.begin(),mCacheLayers.end());

		//////////////////////////////////////////////////////////////////////////
		for(size_t i=0; i < mCacheLayers.size(); ++i)
		{
			mCacheLayers[i]->updateStaticLogic(elapsedTime);
		}

		// ��Ƽ �������� ��� ���� ó���� �� ������ ���


		//////////////////////////////////////////////////////////////////////////
		// ���� ��Ƽ ������ ó�� �Ұ�
		ScriptMain::getInstance().begin();

		GlobalCallBackFuncMgr::getInstance().update(elapsedTime);
		GlobalThreadMgr::getInstance().update(elapsedTime);
		Keyboard::getInstance().update();

		ScriptMain::getInstance().end();

		//////////////////////////////////////////////////////////////////////////
		// ���� ó���� ������ �ݵ�� �ٷ� ��Ʈ������ ó���� ĳ���صΰ� �̰� ��Դ´�
		// �� ��Ƽ ������� ��ó�� �����ϴ�

		//////////////////////////////////////////////////////////////////////////
		// ��Ƽ������ ���� - �ϴ� ����
		// makePreCacheMarix();
		//////////////////////////////////////////////////////////////////////////
		for(size_t i=0; i < mCacheLayers.size(); ++i)
		{
			mCacheLayers[i]->makeCacheMatrix();
			mCacheLayers[i]->makeAabb();
		}
		//////////////////////////////////////////////////////////////////////////
	}

	
	void LayerMgr::messageProcedure( const Message& msg )
	{
//		ISprite::resetFirstEventSprite();

		// if( CDX9_CUSTOM_UI::PriorMsgProc(hWnd,uMsg,wParam,lParam) ) ���� ���� ����
		if( mMsgPreprocedure )
			if( mMsgPreprocedure(msg) )
				return;

		WP_Layer modalLayer = ILayer::getModalLayer();
		if( !modalLayer.expired() )
		{
			modalLayer.lock()->messageProcedure(msg);
			return;
		}

		// ������ �̺�Ʈ�� �ƴϸ� �׳� ó���� ����
		if( msg.mMsgID != WM_MOUSEMOVE )
		{
			mRootLayer->messageProcedure(msg);
			return;
		}


		//////////////////////////////////////////////////////////////////////////
		// WM_MOUSE �� ���� Ư���� ó��

		ISprite* pMouseDragSprite =	ISprite::getMouseDragSprite();
		ISprite* pMouseEnterSprite = ISprite::getMouseEnterSprite();

// 		if( pMouseDragSprite )
// 			pMouseDragSprite->enableMessageProcedure(false);

		// �巡�� �̵��� �ֿ켱
		if( pMouseDragSprite )
		{
			if( pMouseDragSprite->messageProcedure(msg) )
				return;
		}

		mRootLayer->messageProcedure(msg);

// 		if( pMouseDragSprite )
// 		{
// 			pMouseDragSprite->enableMessageProcedure(true);
// 			pMouseDragSprite->messageProcedure(msg);		
// 		}

	}

	void LayerMgr::render() const
	{
		IRenderer::getInstance().begin();

// 		if( mBeginRenderprocedure )
// 			mBeginRenderprocedure();
		
		mRootLayer->render();

// 		if( mEndRenderprocedure )
// 			mEndRenderprocedure();

		IRenderer::getInstance().end();
	}

	SP_Layer LayerMgr::createLayer( const LayerID& id )
	{
		return mRootLayer->createChild(id);
	}

	void LayerMgr::pushBackChild( const SP_Layer& spLayer )
	{
		mRootLayer->pushBackChild(spLayer);
	}

	static void InsertChildHelper( ILayer* pTargetLayer , const SP_Layer& spInsertLayer , bool isAfter)
	{
		const ILayer::ListInfo& targetInfo = pTargetLayer->getLayerListInfo();
		LayerListIter targetIter   = isAfter ? ++LayerListIter(targetInfo.mLayerListIter) : targetInfo.mLayerListIter;
		LayerListIter completeIter = targetInfo.mLayerList->insert( targetIter , spInsertLayer );
		spInsertLayer->setLayerListInfo(ILayer::ListInfo(targetInfo.mLayerList,completeIter,pTargetLayer->getParentLayer()));
	}

	void LayerMgr::insertBeforeLayer( ILayer* pTargetLayer , const SP_Layer& spInsertLayer)
	{
		InsertChildHelper(pTargetLayer,spInsertLayer,false);
	}

	void LayerMgr::insertAfterLayer(  ILayer* pTargetLayer , const SP_Layer& spInsertLayer)
	{
		InsertChildHelper(pTargetLayer,spInsertLayer,true);
	}

	void LayerMgr::insertIntoLayerChild( ILayer* pTargetLayer, ILayer* pSourceLayer)
	{
		mrcASSERT(pTargetLayer && pSourceLayer);
		mrcASSERT(pTargetLayer != pSourceLayer);

		const ILayer::ListInfo& moveInfo = pSourceLayer->getLayerListInfo();
		SP_Layer spSourceLayer = *moveInfo.mLayerListIter;
		moveInfo.mLayerList->erase(moveInfo.mLayerListIter);

		//////////////////////////////////////////////////////////////////////////
		pTargetLayer->pushBackChild(spSourceLayer);
	}

	static void moveLayerHelper(const ILayer* pTargetLayer,const ILayer* pMoveLayer,bool isAfter)
	{
		mrcASSERT(pTargetLayer && pMoveLayer);
		mrcASSERT(pTargetLayer != pMoveLayer);

		const ILayer::ListInfo& moveInfo = pMoveLayer->getLayerListInfo();
		SP_Layer spMoveLayer = *moveInfo.mLayerListIter;
		moveInfo.mLayerList->erase(moveInfo.mLayerListIter);

		const ILayer::ListInfo& targetInfo = pTargetLayer->getLayerListInfo();
		LayerListIter targetIter = isAfter ? ++LayerListIter(targetInfo.mLayerListIter) : targetInfo.mLayerListIter;

		LayerList::iterator completeIter = targetInfo.mLayerList->insert( targetIter, spMoveLayer );
		spMoveLayer->setLayerListInfo(ILayer::ListInfo(targetInfo.mLayerList,completeIter,pTargetLayer->getParentLayer()));		
	}

	void LayerMgr::moveBeforeLayer( const ILayer* pTargetLayer,const ILayer* pMoveLayer )
	{
		moveLayerHelper(pTargetLayer,pMoveLayer,false);
	}

	void LayerMgr::moveAfterLayer( const ILayer* pTargetLayer,const ILayer* pMoveLayer )
	{
		moveLayerHelper(pTargetLayer,pMoveLayer,true);
	}

	void LayerMgr::clear()
	{
		mCacheLayers.clear();
		mRootLayer->deleteChildAll();
	}


	static int getDepth(ILayer* pLayer)
	{
		int depth = 0;
		
		ILayer* pParent = pLayer->getParentLayer();
		while(pParent)
		{
			++depth;
			pParent = pParent->getParentLayer();
		}

		return depth;
	}

	void LayerMgr::printLayers()
	{
		vector<SP_Layer> layers;
		layers.reserve(mMaxLayerSize);
		mRootLayer->getChildAll(&layers);

		F_Print funcPrint = getStandardPrint();
		funcPrint( _mrcT("\n#########   Print Layers ALL #########\n\n") );

		const String depthMark(_mrcT("\t"));
		for (size_t i=0; i < layers.size(); ++i)
		{
			String curDepthString;
			int depth = getDepth(layers[i].get());
			if( depth <= 1)
				depth = 0;

			for (int depthIndex = 0; depthIndex < depth; ++depthIndex)
				curDepthString += depthMark;
			
			funcPrint( (curDepthString + layers[i]->getId()).c_str() );
			Vector2 vec = layers[i]->getPosition();
			funcPrint( _mrcT("( x : %f , y : %f )"), vec.x , vec.y );
			funcPrint( _mrcT("\n") );
		}

		funcPrint(_mrcT("\n######################################\n"));
	}

	void LayerMgr::getChildAll(vector<SP_Layer>* pLayers)
	{
		mRootLayer->getChildAll(pLayers);
	}

	ILayer* LayerMgr::getRootLayer() const
	{
		return mRootLayer.get();
	}

	
	
}


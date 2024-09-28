


#ifndef _mrcLayerMgr_h_
#define _mrcLayerMgr_h_

#include "mrcConfig.h"

//#include "mrcLayerImpl.h"
#include "mrcLayer.h"

namespace mrc
{
	class GlobalThreadMgr;

	class LayerImpl;
	typedef mrcWeakenPtr<LayerImpl> WP_LayerImpl;
	typedef mrcSharedPtr<LayerImpl> SP_LayerImpl;

// 	class ILayerMgr
// 	{
// 	public:
// 		virtual void update(real elapsedTime) = 0;
// 		virtual void render() const = 0;
// 
// 
// 		virtual SP_Layer createLayer( const LayerID& id );
// 
// 		template<class SPRITE>
// 		virtual SP_Layer createLayer( const LayerID& id ) = 0;
// 
// 		virtual void insertChild(const SP_Layer& spLayer) = 0;
// 
// 		void moveChild(const ILayer* pTargetLayer,const ILayer* pMoveLayer) = 0;
// 		void messageProcedure(const Message& msg) = 0;
// 		void eraseLayerAll() = 0;
// 		void printLayers() = 0;
// 		void getChildAll(vector<SP_Layer>* pLayers) = 0;
// 	};

	class LayerMgr
	{
		MIRACLE_FRIEND_CLASS_SINGLETON(LayerMgr);
	protected:
		LayerMgr();

		typedef bool (*F_MessagePreprocedure)(const Message&);
//		typedef void (*F_Renderprocedure)();

	public:
		virtual ~LayerMgr();

		static LayerMgr& getInstance();
		static void newLayerMgr(LayerMgr* pLayerMgr);
		static void release();

		virtual void update(real elapsedTime);
		virtual void render() const;


		virtual SP_Layer createLayer( const LayerID& id );
		
		template<class SPRITE>
		SP_LayerImpl createLayer( const LayerID& id )
		{
			return mRootLayer->createChild<SPRITE>(id);
		}

		virtual void pushBackChild(const SP_Layer& spLayer);

		virtual void insertBeforeLayer(ILayer* pTargetLayer, const SP_Layer& spInsertLayer);
		virtual void insertAfterLayer( ILayer* pTargetLayer, const SP_Layer& spInsertLayer);

		virtual void insertIntoLayerChild(ILayer* pTargetLayer, ILayer* pSourceLayer);

		virtual void moveBeforeLayer(const ILayer* pTargetLayer,const ILayer* pMoveLayer);
		virtual void moveAfterLayer(const ILayer* pTargetLayer,const ILayer* pMoveLayer);

		virtual void messageProcedure(const Message& msg);

		virtual void clear(); 

		virtual void printLayers();

		virtual void getChildAll(vector<SP_Layer>* pLayers);

// 		void setBeginRenderprocedure(F_Renderprocedure renderProcedure) { mBeginRenderprocedure = renderProcedure; }
// 		void setEndRenderprocedure(F_Renderprocedure renderProcedure) { mEndRenderprocedure = renderProcedure; }

		void setReverseUpdate(bool isReverseUpdate) { mIsReverseUpdate = isReverseUpdate; }

		ILayer* getRootLayer() const;
//		const vector<ILayer*>& getCacheLayers() { return mCacheLayers; }

	private:
		F_MessagePreprocedure mMsgPreprocedure;
// 		F_Renderprocedure mBeginRenderprocedure;
// 		F_Renderprocedure mEndRenderprocedure;
		bool mIsReverseUpdate;
		vector<SP_Layer> mCacheLayers;

		mrcAutoPtr<LayerImpl> mRootLayer;  	
		size_t mMaxLayerSize;
	};



}



#endif






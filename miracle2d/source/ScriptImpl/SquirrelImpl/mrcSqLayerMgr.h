
#ifndef _mrcSqLayerMgr_h_
#define _mrcSqLayerMgr_h_

#include "mrcConfig.h"
//#include "mrcSqLayer.h"

namespace Sqrat
{
	template<class C>
	class DefaultAllocator;

}

namespace mrc
{
	class SqLayer;
	class SqLayerMgr : public Sqrat::SqObjectType 
	{
		MIRACLE_FRIEND_CLASS_SINGLETON(SqLayerMgr);
		friend class Sqrat::DefaultAllocator<SqLayerMgr>;

		SqLayerMgr() {}
	public:
		static SqLayerMgr& getInstance();

		SqLayer createLayer( const LayerID& id );
		SqLayer createLayerWithSpriteDefault( const LayerID& id );
		SqLayer createLayerWithSpriteButton( const LayerID& id );
		SqLayer createLayerWithSpriteNumber( const LayerID& id );


		void moveBeforeLayer(const SqLayer& targetLayer,const SqLayer& moveLayer);
		void moveAfterLayer(const SqLayer& targetLayer,const SqLayer& moveLayer);

		void clear();
	};





}




#endif


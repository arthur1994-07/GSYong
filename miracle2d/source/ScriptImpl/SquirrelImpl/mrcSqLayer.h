

#ifndef _mrcSqLayer_h_
#define _mrcSqLayer_h_

#include "mrcConfig.h"
#include "mrcVector2.h"

#include "mrcSqSpriteDefault.h"


namespace mrc
{

	class SqVector2;
	class SqVector3;

	class LayerImpl;
	class ILayer;
	typedef mrcWeakenPtr<ILayer>     WP_Layer;
	typedef mrcSharedPtr<ILayer>     SP_Layer;
	typedef mrcSharedPtr<LayerImpl>  SP_LayerImpl;


	class SqLayer : public Sqrat::SqObjectType 
	{
	public:
		SqLayer() {	mLayerImpl = NULL; }
		explicit SqLayer(SP_LayerImpl spLayerImpl);

		LayerID getId();

		void setPositionv(const SqVector2& pos);
		void setPosition(real x,real y);

		void setRotation(real rotation);
		void setPivotRotation( real rotation );
		void setScalev(const SqVector2& scale);
		void setScale(real x,real y);	
		void setScaleUnit(real scale);
		void setRGB(real r,real g,real b);
		void setRGBc(const SqColorRGB& color);
		void setAlpha(real alpha);

		SqVector2 getPosition() const;
		real getRotation() const;
		real getPivotRotation() const;

		SqVector2 getScale() const;
		SqColorRGB getRGB() const;
		real getAlpha() const;

		void setPivotAbsv(const SqVector2& absPivot);
		void setPivotv(const SqVector2& pivot);
		void setPivotAbs(real x,real y);
		void setPivot(real x,real y);

		SqVector2 getPivot() const;

		void setUVVelocityv(const SqVector2& uv);
		SqVector2 getUVVelocity();

		void    setMoveVelocityv(const SqVector2& vel);
		SqVector2 getMoveVelocity() const;

		void setRotationVelocity(real vel);
		real getRotationVelocity() const;

		void setPivotRotationVelocity(real vel);
		real getPivotRotationVelocity() const;


		void pushLinePositionQ(const SqVector2& pos,real totalTime,int16 method);
		void pushBezierPositionQ(const SqVector2& middle1,const SqVector2& middle2,
			const SqVector2& end,real totalTime);
		void setPositionQLoop(bool isLoop);
		void clearPositionQ();

		void pushRotationQ(real rot,real totalTime,int16 method);
		void setRotationQLoop(bool isLoop);
		void clearRotationQ();

		void pushScaleQ(const SqVector2& scale,real totalTime,int16 method);
		void setScaleQLoop(bool isLoop);
		void clearScaleQ();

		void pushPivotRotationQ(real rot,real totalTime,int16 method);
		void setPivotRotationQLoop(bool isLoop);
		void clearPivotRotationQ();

		void pushRGBQ(const SqColorRGB& rgbVector,real totalTime,int16 method);
		void setRGBQLoop(bool isLoop);
		void clearRGBQ();

		void pushAlphaQ(real alpha,real totalTime,int16 method);
		void setAlphaQLoop(bool isLoop);
		void clearAlphaQ();
		//////////////////////////////////////////////////////////////////////////


		SqSpriteDefault getSpriteDefault();
		SqSpriteButton	getSpriteButton();
		SqSpriteNumber	getSpriteNumber();

		
		SqLayer createChild(const LayerID& id );
		SqLayer createChildWidthSpriteDefault( const LayerID& id );
		SqLayer createChildWithSpriteButton( const LayerID& id );
		SqLayer createChildWithSpriteNumber( const LayerID& id );


		void destroy();


		//////////////////////////////////////////////////////////////////////////
		LayerImpl* getPointer() const;
		WP_Layer   getChecker() const { return mLayerForCheck; }
		bool isValid() const;
	private:
		WP_Layer   mLayerForCheck;
		LayerImpl* mLayerImpl;
	};

}




#endif

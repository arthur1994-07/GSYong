

#ifndef _mrcLayerActorQueue_h_
#define _mrcLayerActorQueue_h_

#include "mrcConfig.h"
#include "mrcVector2.h"
#include "mrcVector3.h"
#include "mrcVector4.h"

namespace mrc
{
	// differenceValue is  end_value - satrt_value
	// 	mrc::real Linear::easeNone (mrc::real t,mrc::real b , mrc::real c, mrc::real d) {
	// 
	namespace ease
	{
		inline void cubicBezier(real BezierPoint[4][2],real mu,real outPoint[2])
		{
			float a[2],b[2],c[2],p[2];

			c[0] = 3 * ( BezierPoint[1][0] - BezierPoint[0][0] );
			c[1] = 3 * ( BezierPoint[1][1] - BezierPoint[0][1] );
			b[0] = 3 * ( BezierPoint[2][0] - BezierPoint[1][0] ) - c[0];
			b[1] = 3 * ( BezierPoint[2][1] - BezierPoint[1][1] ) - c[1];
			a[0] = BezierPoint[3][0] - BezierPoint[0][0] - c[0] - b[0];
			a[1] = BezierPoint[3][1] - BezierPoint[0][1] - c[1] - b[1];

			p[0] = float(a[0] * mu * mu * mu + b[0] * mu * mu + c[0] * mu + BezierPoint[0][0]);
			p[1] = float(a[1] * mu * mu * mu + b[1] * mu * mu + c[1] * mu + BezierPoint[0][1]);

			outPoint[0] = p[0];
			outPoint[1] = p[1];
		}

		template<class T>
		T easingLinear(real accumulatedElapsedTime,const T& startValue,const T& differenceValue, real totalTime)
		{
			return differenceValue * accumulatedElapsedTime / totalTime	+ startValue;
		}

		template<class T>
		T easingExpoIn(real accumulatedElapsedTime,const T& startValue,const T& differenceValue, real totalTime)
		{
			return (accumulatedElapsedTime==0) ? startValue : differenceValue * pow(2, 10 * (accumulatedElapsedTime/totalTime - 1)) + startValue;
		}

// 		template<class T>
// 		T easingExpoOut(real accumulatedElapsedTime,const T& startValue,const T& differenceValue, real totalTime)
// 		{
// 
// 		}

		enum METHOD 
		{
			EASE_LINEAR, 
			EASE_EXPO_IN,EASE_EXPO_OUT,EASE_EXPO_INOUT,

		};
	

		template<class T>		
		struct Data
		{
			typedef T (*F_EASE_METHOD)(real,const T&,const T&,real);
			static F_EASE_METHOD getEasingMethod(METHOD method)
			{
				switch(method)
				{
				case EASE_LINEAR:
					return easingLinear;
				case EASE_EXPO_IN:
					return easingExpoIn;
				}

				mrcASSERT(false);
				return NULL;
			}

			typedef T VALUE; 
			Data() { mTotalTime = 0; mFuncEasing = NULL;}
			T		mStart;
			T		mEnd;
			real	mTotalTime;

			F_EASE_METHOD mFuncEasing;
		};

		template<class T>
		struct DataEx : public Data<T>
		{
			typedef T VALUE; 
			DataEx() { mTotalTime = 0; mLineProcess = true; }
			bool    mLineProcess;
			T		mMiddle[2];
		};

		template< class DATA_T >
		class Queue : public AllocatedObject
		{

			//////////////////////////////////////////////////////////////////////////
			typedef typename DATA_T::VALUE VALUE;
		public:
			Queue() { clear(); }

			void clear() { mCurrentTime = 0; mLoop = false; mCurrentData = DATA_T(); mDataQueue = queue< DATA_T >(); }
			void push(const DATA_T& data) {	mDataQueue.push(data); }

			bool update(real elapsedTime )
			{
				if( mCurrentTime >= mCurrentData.mTotalTime )
				{
					if( mDataQueue.empty() )
						return false;

					mCurrentTime -= mCurrentData.mTotalTime;

					mCurrentData = mDataQueue.front();
					mDataQueue.pop();

					if( mLoop )
						mDataQueue.push(mCurrentData);
				}
				else
					mCurrentTime += elapsedTime;

				mUpdatedValue = easing(mCurrentData);

				if( mCurrentTime >= mCurrentData.mTotalTime )
				{
					if(mDataQueue.empty())
						return true;

					makeStartValue(&mDataQueue.front());					
				}

				return true;
			}

			void changeRelativeCurrentData(const VALUE& value)
			{				
				if( !(mCurrentTime >= mCurrentData.mTotalTime) )
				{
					typename DATA_T::VALUE relValue = value - mUpdatedValue;
					mCurrentData.mStart += relValue;
					mCurrentData.mEnd += relValue;
				}
				else if( !mDataQueue.empty() )
				{
					DATA_T& data = mDataQueue.front();
					VALUE relValue = data.mEnd - data.mStart;
					data.mStart = mUpdatedValue;
					data.mEnd = data.mStart + relValue;
				}
			}

			void changeRelativeCurrentData(const DataEx<Vector2>& value)
			{
				if( !(mCurrentTime >= mCurrentData.mTotalTime) )
				{
					Vector2 relValue = value - mUpdatedValue;
					mCurrentData.mStart += value;
					mCurrentData.mEnd += value;
					mCurrentData.mMiddle[0] += value;
					mCurrentData.mMiddle[1] += value;
				}
				else if( !mDataQueue.empty() )
				{
					DataEx<Vector2>& data = mDataQueue.front();
					Vector2 relEndValue = data.mEnd - data.mStart;
					Vector2 relMiddle1Value = data.mMiddle[0] - data.mStart;
					Vector2 relMiddle2Value = data.mMiddle[1] - data.mStart;

					data.mStart = mUpdatedValue;
					data.mEnd = data.mStart + relEndValue;
					data.mMiddle[0] = data.mStart + relMiddle1Value;
					data.mMiddle[1] = data.mStart + relMiddle2Value;
				}
			}

			void setLoop(bool isLoop)  { mLoop = isLoop; }
			const typename VALUE& getUpdatedValue() { return mUpdatedValue; }

		private:
			real test_max(real current,real total) { return current > total ? total : current; }

			// 베이지어는 뭔가 다르기때문에.... 다른 처리를 해줘야함
			//////////////////////////////////////////////////////////////////////////
			VALUE easing(const typename Data<VALUE>& data)
			{
				return data.mFuncEasing(test_max(mCurrentTime,data.mTotalTime),data.mStart,data.mEnd - data.mStart,data.mTotalTime);
			}

			Vector2 easing(const DataEx<Vector2>& data)
			{
				if( data.mLineProcess )
					return data.mFuncEasing(test_max(mCurrentTime,data.mTotalTime),data.mStart,data.mEnd - data.mStart,data.mTotalTime);

				real outPos[2] ={0};
				real bezier[4][2] =
				{ 
					{data.mStart.x,data.mStart.y},
					{data.mMiddle[0].x,data.mMiddle[0].y},
					{data.mMiddle[1].x,data.mMiddle[1].y},
					{data.mEnd.x,data.mEnd.y},					
				};

				cubicBezier(bezier, test_max(mCurrentTime,data.mTotalTime)/data.mTotalTime,outPos);
				return Vector2(outPos[0],outPos[1]);
			}

			void makeStartValue(typename Data<VALUE>* pData)
			{
				pData->mStart = mUpdatedValue;
			}

			void makeStartValue(DataEx<Vector2>* pData)
			{
				if( pData->mLineProcess )
				{
					Vector2 relValue = pData->mEnd - pData->mStart;
					pData->mStart = mUpdatedValue;
					pData->mEnd = pData->mStart + relValue;
				}
				else
				{
					Vector2 relValue = pData->mEnd - pData->mStart;
					Vector2 relMiddle1Value = pData->mMiddle[0] - pData->mStart;
					Vector2 relMiddle2Value = pData->mMiddle[1] - pData->mStart;
					
					pData->mStart = mUpdatedValue;
					pData->mEnd = pData->mStart + relValue;
					pData->mMiddle[0] = pData->mStart + relMiddle1Value;
					pData->mMiddle[1] = pData->mStart + relMiddle2Value;
				}

			}
			//////////////////////////////////////////////////////////////////////////

		private:
			real mCurrentTime;
			bool mLoop;
			

			VALUE mUpdatedValue;
			DATA_T mCurrentData;
			queue< DATA_T > mDataQueue;
		};
	}

	


}




#endif





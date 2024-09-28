

#ifndef _mrcLayerThread_h_
#define _mrcLayerThread_h_

#include "mrcConfig.h"


#include "mrcScript.h"
#include "mrcUtil/mrcSingleton.h"

namespace mrc
{
	


	struct CallBackData
	{
		uint32         mHandle;
		bool           mIsForLayer;
		WP_Layer   mCheckLayer;
		//////////////////////////////////////////////////////////////////////////
		ScriptObject   mArg;
		ScriptFunction mFunc;
		real           mTime;
	};
	
	class GlobalCallBackFuncMgr
	{
		MIRACLE_FRIEND_CLASS_SINGLETON(GlobalCallBackFuncMgr);
		GlobalCallBackFuncMgr() { mHandleGenerator = 0; }

		typedef list<CallBackData> FuncList;
	public:
		static GlobalCallBackFuncMgr& getInstance() 
		{
			typedef Loki::SingletonHolder<GlobalCallBackFuncMgr,Loki::CreateUsingNew,
				Loki::LongevityLifetime::DieDependencyMgr > GlobalCallBackFuncMgrS;

			return GlobalCallBackFuncMgrS::Instance();
		}

		uint32 registerCallBack(ScriptFunction func,ScriptObject arg,real afterTime,WP_Layer wpLayer = WP_Layer())
		{
			mrcASSERT( mHandleGenerator < UINT_MAX );
			CallBackData data;
			data.mHandle = mHandleGenerator++;
			data.mFunc = func;
			data.mArg = arg;
			data.mTime = afterTime;
			data.mIsForLayer = !wpLayer.expired();
			data.mCheckLayer = wpLayer;
			mFuncList.push_back(data);
			return data.mHandle;
		}

		void update(real elapsedTime)
		{
			for ( FuncList::iterator it = mFuncList.begin(); it != mFuncList.end();  )
			{
				FuncList::iterator cur_it = it++;
				CallBackData& data = *cur_it;
				data.mTime -= elapsedTime;
				if( data.mTime <= 0 )
				{
					if( data.mIsForLayer)
					{
						if( !data.mCheckLayer.expired() )
							data.mFunc(data.mArg);
					}
					else
						data.mFunc(data.mArg);
					
					mFuncList.erase(cur_it);
				}
			}
		}

		void erase(uint32 handle)
		{
			for (FuncList::iterator it = mFuncList.begin(); it != mFuncList.end(); ++it)
			{
				if( it->mHandle == handle )
				{
					mFuncList.erase(it);
					break;
				}
			}
		}

		void clear() { mFuncList.clear();  mHandleGenerator = 0;}
	private:
		// handleGenerator는 무한이 증가만 하고  uint32 최대값이 도달하면 assert

		uint32   mHandleGenerator;
		FuncList mFuncList;
	};

	class LayerThread // : public AllocatedObject
	{ 
	public:
		enum STATE{ STATE_RUN ,STATE_DEAD,STATE_SLEEP };
	public:
		LayerThread()
		{
			mHandle = uint32(-1);
			mIsForLayer = false;
			mScriptThreadSleepTime = Math::NEG_INFINITY;
		}

		explicit LayerThread(uint32 handle, const ScriptThread& scriptThreadObj, WP_Layer wpLayer )
		{
			mHandle = handle;
			mIsForLayer =  !wpLayer.expired();
			mCheckLayer =  wpLayer;

			createScriptThread(scriptThreadObj);
		}

		uint32 getHandle() { return mHandle; }

		STATE resume(real elapsedTime)
		{
			if( mIsForLayer && mCheckLayer.expired() )
				return STATE_DEAD;

			if( Math::NEG_INFINITY == mScriptThreadSleepTime )
				mScriptThreadSleepTime = elapsedTime; 
			//////////////////////////////////////////////////////////////////////////
			// 로직 처리 , 오차가 누적된다면 절대 시간 기준으로 다시 처리하게 만든다
			mrcASSERT( mScriptThreadObj.isValid() );

			if( mScriptThreadObj.isDead() ) 
			{
				return STATE_DEAD;
			}
			else 
			{
				if( mScriptThreadSleepTime != Math::NEG_INFINITY )
				{
					mScriptThreadSleepTime -= elapsedTime;
					if( mScriptThreadSleepTime < 0 )
					{
						real sleepTime = mScriptThreadObj.resume();
						if( sleepTime == ScriptThread::YIELD_ERROR )
							return STATE_DEAD;

						mScriptThreadSleepTime += sleepTime;
						if( mScriptThreadSleepTime < 0 )
							return STATE_RUN;
					}
				}
			}

			return STATE_SLEEP;
		}

		void reset()
		{
			mScriptThreadObj = ScriptThread();
			mScriptThreadSleepTime = Math::NEG_INFINITY;
		}
	private:
		void createScriptThread( const ScriptThread& scriptThreadObj )
		{
			mScriptThreadSleepTime = Math::NEG_INFINITY;
			mScriptThreadObj = scriptThreadObj;
			mrcASSERT(mScriptThreadObj.isValid());
		}
		uint32			mHandle;
		bool            mIsForLayer;
		WP_Layer		mCheckLayer;

		ScriptThread	mScriptThreadObj;
		real			mScriptThreadSleepTime;
	};

	class GlobalThreadMgr
	{
		MIRACLE_FRIEND_CLASS_SINGLETON(GlobalThreadMgr);
		GlobalThreadMgr() { mHandleGenerator = 0;}

		typedef list<LayerThread> LayerThreadList;
	public:
		static GlobalThreadMgr& getInstance() 
		{
			typedef Loki::SingletonHolder<GlobalThreadMgr,Loki::CreateUsingNew,
				Loki::LongevityLifetime::DieDependencyMgr > GlobalThreadMgrS;

			return GlobalThreadMgrS::Instance();
		}

		//////////////////////////////////////////////////////////////////////////
		static void killThread(ScriptThread scriptThread)
		{
			mrcASSERT( scriptThread.isValid() );
			scriptThread.kill();
		}

		ScriptThread createThread(const ScriptThread& scriptThread,WP_Layer wpLayer)
		{
			mrcASSERT( mHandleGenerator < UINT_MAX );
			mThreadList.push_back(LayerThread(mHandleGenerator++,scriptThread,wpLayer) );
			return scriptThread;
		}

		//////////////////////////////////////////////////////////////////////////
		void update(real elapsedTime)
		{
			LayerThreadList restTimeThread;
			for(LayerThreadList::iterator it = mThreadList.begin(); it != mThreadList.end(); )
			{
				LayerThreadList::iterator curIt = it++;
				LayerThread::STATE state = curIt->resume(elapsedTime);
				if( state == LayerThread::STATE_RUN )
					restTimeThread.push_back( *curIt );
				else if( state == LayerThread::STATE_DEAD )
					mThreadList.erase(curIt);
			}

			// 남은게 존재한다면  끝까지 돌면서 다 처리
			while( !restTimeThread.empty() )
			{
				LayerThread threadObj = restTimeThread.front();
				restTimeThread.pop_front();

				if( threadObj.resume(elapsedTime) == LayerThread::STATE_RUN )
					restTimeThread.push_back(threadObj);
			}
		}

		void clear() { mThreadList.clear(); mHandleGenerator = 0;}
		void erase(uint32 handle)
		{
			for(LayerThreadList::iterator it = mThreadList.begin(); it != mThreadList.end(); )
			{
				if( it->getHandle() == handle )
				{
					mThreadList.erase(it);
					break;
				}

			}
		}

	private:
 		uint32   mHandleGenerator; 
		LayerThreadList mThreadList;
	};



}


#endif
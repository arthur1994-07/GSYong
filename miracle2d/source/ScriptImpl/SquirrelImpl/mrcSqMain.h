


#ifndef _mrcSqMain_h_
#define _mrcSqMain_h_

#include "mrcConfig.h"


#include <squirrel.h>

struct SQDbgServer;
typedef SQDbgServer* HSQREMOTEDBG;

namespace mrc
{
	class SqMain
	{
		MIRACLE_FRIEND_CLASS_SINGLETON(SqMain);
		SqMain();
	public:
		static SqMain& getInstance();

		filesystem::Path getFullPath();

		void init(const filesystem::Path& path,bool isRemoteDebug);

		void release();


		void loadScript();

		void begin();
		void end();
		bool isRunningScript();

	private:
		filesystem::Path mPath;
		HSQUIRRELVM      mVM;
		HSQREMOTEDBG     mRDBG;

		bool mIsRemoteDebug;

	};






}



#endif

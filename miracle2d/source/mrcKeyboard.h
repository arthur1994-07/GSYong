


#ifndef _mrcKEYBOARD_H_
#define _mrcKEYBOARD_H_

#include "mrcConfig.h"
#include "mrcScript.h"

namespace mrc
{	
	class Keyboard
	{
		struct Data
		{
			int32 mKey;
			bool  mIsPressed;
			int32 mOption;
		};

	public:
		enum 
		{
			K_DOWN = VK_DOWN,
			K_UP = VK_UP,
			K_LEFT = VK_LEFT,
			K_RIGHT = VK_RIGHT,
			K_SPACE = VK_SPACE,
			K_ESCAPE = VK_ESCAPE,
			K_1 = 49,K_2 = 50,K_3 = 51,K_4 = 52,K_5 = 53,K_6 = 54,K_7 = 55,K_8 = 56,K_9 = 57,K_0 = 58,
			K_A = 65,K_B = 66,K_C = 67,K_D = 68,K_E = 69,K_F = 70,K_G = 71,K_H = 72,K_I = 73,K_J = 74,
			K_K = 75,K_L = 76,K_M = 77,K_N = 78,K_O = 79,K_P = 80,K_Q = 81,K_R = 82,K_S = 83,K_T = 84,
			K_U = 85,K_V = 86,K_W = 87,K_X = 88,K_Y = 89,K_Z = 90,
		};

		static Keyboard& getInstance();

		void update()
		{
			if(mFuncKeyEvent.IsNull())
				return;

			for (list<Data>::iterator it = mKeyQueue.begin(); it != mKeyQueue.end(); ++it)
			{
				SCRIPT_CALL_IF_FAILED_MSG( mFuncKeyEvent(it->mKey,it->mIsPressed,it->mOption) , _mrcT("KeyboardEventFunc Check")  );
			}

			mKeyQueue.clear();
		}

		void setEventFunc( ScriptFunction funcEvent ) { mFuncKeyEvent = funcEvent; }
		void clear() { mFuncKeyEvent = ScriptFunction(); }
		
		void pushKey(int32 key,bool isPressed,int32 option)
		{
			Data data; 
			data.mKey = key;
			data.mIsPressed = isPressed;
			data.mOption = option;
			mKeyQueue.push_back(data);
		}

	private:
		list< Data > mKeyQueue;
		ScriptFunction mFuncKeyEvent;
	};

}


#endif


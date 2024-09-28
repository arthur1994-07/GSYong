

#include "mrcPrecompiled.h"


#include "mrcSqMain.h"
#include "mrcUtil/mrcSingleton.h"

#include "mrcAssert.h"
#include "mrcUtil/mrcFileSystem.h"

#include <sqrdbg.h>
#include <sqstdblob.h>
#include <sqstdaux.h>
#include <sqstdmath.h>
#include <sqstdstring.h>
#include <sqstdsystem.h>



#include "mrcSqLayer.h"
#include "mrcSqLayerMgr.h"
#include "mrcLayerMgr.h"
#include "mrcSqUtil.h"
#include "mrcUtil/mrcScriptShape.h"



#include "../../mrcLayerThread.h"
#include "../../mrcLayerImpl.h"

#include "../../mrcSpriteDefault.h"
#include "../../mrcEaseQueue.h"

#include "../../mrcKeyboard.h"
#include "../mrcRegisterScriptObject.h"

#include "mrcRenderer.h"



namespace mrc
{
	using namespace Sqrat;

	bool& isScriptCallErrorValue()
	{
		return ErrorHandling::IsCallError();
	}

	static bool msAssertIsSucceedChangeSqObjectToArg = true;
	static AssertArgAndMsg msAssertArgAndMsg;
	//////////////////////////////////////////////////////////////////////////

	static bool msIsAssertArgCount = true;
	static NString msAssertArgCountMsg;
	static String msAssertOverloadingArgType;


	void resetAssertArg()
	{
		msAssertIsSucceedChangeSqObjectToArg = true;
		msIsAssertArgCount = true;
		msAssertOverloadingArgType.clear();
	}

	bool setAssertIsSucceedChangeSqObjectToArg(bool isSucceedChangeSqObjectToArg)
	{
		if( msAssertIsSucceedChangeSqObjectToArg )
			msAssertIsSucceedChangeSqObjectToArg = isSucceedChangeSqObjectToArg;
	
		return isSucceedChangeSqObjectToArg;
	}

	bool getAssertIsSucceedChangeSqObjectToArg()
	{
		return msAssertIsSucceedChangeSqObjectToArg;
	}

	bool setAssertArgTypeMsg(HSQUIRRELVM vm,int32 argIndex,const NString& msg)
	{
		// Sqrat 에서는 2부터 인자가 시작되므로 -1를 해줘야함 
		msAssertArgAndMsg.first = argIndex - 1;

		HSQOBJECT obj;
		sq_getstackobj(vm, argIndex, &obj);
		msAssertArgAndMsg.second = msg + " => " + toNString(GetObjectTypeName(obj));
		return true;
	}

	const AssertArgAndMsg& getAssertArgType()
	{
		return msAssertArgAndMsg;
	}

	//////////////////////////////////////////////////////////////////////////
	bool setAssertIsAssertArgCount(HSQUIRRELVM vm,int32 nativeFuncArgCount)
	{
		// 객체와 함수 나머진 인자이므로 - 2를 해줘야함
		int32 scriptArgCount  = (int32)sq_gettop(vm) - 2;
		msIsAssertArgCount = scriptArgCount  == nativeFuncArgCount;
		if( !msIsAssertArgCount )
		{
			msAssertArgCountMsg = "native function need " + toNString(nativeFuncArgCount) + 
				" argument count but you push " + toNString(scriptArgCount) + " argument count";
		}

		return msIsAssertArgCount;
	}

	bool getAssertIsAssertArgCount()
	{
		return msIsAssertArgCount;
	}

	NString getAssertArgCountMsg()
	{
		return msAssertArgCountMsg;
	}

	//////////////////////////////////////////////////////////////////////////
	
	bool isAssertOverloadingArgType()
	{
		return msAssertOverloadingArgType.empty();
	}

	String getAssertOverloadingArgType()
	{
		return msAssertOverloadingArgType;
	}

	void setAssertOverloadingArgType(const String& errorMsg)
	{
		msAssertOverloadingArgType = errorMsg;
	}

	//////////////////////////////////////////////////////////////////////////

	String getScriptClassName(const NString& classNameTemp)
	{
		NString className = classNameTemp;
		NString::size_type pos;
		pos  = className.find("class ");
		if(pos != NString::npos )
			className.erase(className.begin() + pos, className.begin() + pos + strlen("class ") );
	
		pos = className.find("mrc::Sq");
		if(pos != NString::npos )
			className.erase(className.begin() + pos + 3, className.begin() + pos + 7 );

		return toString(className);
	}

	String getSqClassNameInspVarGet(HSQUIRRELVM vm)
	{
		SqObjectType *ptr = NULL;
		sq_getinstanceup(vm, -3, (SQUserPointer*)&ptr, NULL);	
		mrcASSERT(ptr);
		return getScriptClassName(typeid(*ptr).name());
	}

	class SqDevice : public SqObjectType
	{
		MIRACLE_FRIEND_CLASS_SINGLETON(SqDevice);
		friend class Sqrat::DefaultAllocator<SqDevice>;
	public:
		static SqDevice& getInstance()
		{
			typedef Loki::SingletonHolder<SqDevice,Loki::CreateUsingNew,
				Loki::LongevityLifetime::DieDependencyMgr > SqDeviceS;

			return SqDeviceS::Instance();
		}

		void showDebugWire(bool isShow)	
		{ 
			MIRICALE_SCRIPT_TEST_CLASS_FUNC();
			IRenderer::getInstance().showDebugWire(isShow); 
		}

		SqPoint getWidthHeight() 
		{ 
			MIRICALE_SCRIPT_TEST_CLASS_FUNC();
			return IRenderer::getInstance().getWidthHeight(); 
		}
	private:
		bool isValid() { return true; }
	};

	class SqCallBackFuncMgr : public SqObjectType
	{
		MIRACLE_FRIEND_CLASS_SINGLETON(SqCallBackFuncMgr);
		friend class Sqrat::DefaultAllocator<SqCallBackFuncMgr>;
	public:
		static SqCallBackFuncMgr& getInstance()
		{
			typedef Loki::SingletonHolder<SqCallBackFuncMgr,Loki::CreateUsingNew,
				Loki::LongevityLifetime::DieDependencyMgr > ScriptCallBackMgrS;

			return ScriptCallBackMgrS::Instance();
		}

		uint32 registerCallBack(SqObject func,SqObject arg,real afterTime)
		{
			MIRICALE_SCRIPT_TEST_CLASS_FUNC();
			mrcScrASSERT(!func.IsNull());
//			HSQOBJECT testobj;
//			sq_getstackobj(Sqrat::DefaultVM::Get(),-7,&testobj);
			HSQOBJECT env = sqex_getclosureenv(&func.GetObject());

			return GlobalCallBackFuncMgr::getInstance().registerCallBack(SqFunction(Sqrat::DefaultVM::Get(),env, func ),arg,afterTime);
		}

		uint32 registerLayerCallBack(SqFunction func,SqObject arg,real afterTime,SqLayer layer)
		{
			MIRICALE_SCRIPT_TEST_CLASS_FUNC();
			mrcScrASSERT(!func.IsNull());
			return GlobalCallBackFuncMgr::getInstance().registerCallBack(func,arg,afterTime,layer.getChecker());
		}

		void eraseCallBack(uint32 handle)
		{
			MIRICALE_SCRIPT_TEST_CLASS_FUNC();
			GlobalCallBackFuncMgr::getInstance().erase(handle);
		}

		
	private:
		bool isValid() { return true; }
	};

	
	class SqThreadMgr : public SqObjectType
	{
		MIRACLE_FRIEND_CLASS_SINGLETON(SqThreadMgr);
		friend class Sqrat::DefaultAllocator<SqThreadMgr>;
	public:
		static SqThreadMgr& getInstance()
		{
			typedef Loki::SingletonHolder<SqThreadMgr,Loki::CreateUsingNew,
				Loki::LongevityLifetime::DieDependencyMgr > SqThreadMgrS;

			return SqThreadMgrS::Instance();
		}
		
		SqThread createThread(SqObject scriptThread)
		{
			MIRICALE_SCRIPT_TEST_CLASS_FUNC();
			mrcScrASSERT_MSG( sq_isgenerator(scriptThread.GetObject()) , "arg is not generator" );
			GlobalThreadMgr::getInstance().createThread(scriptThread,WP_Layer());
			return scriptThread;
		}

		SqThread createLayerThread(const SqLayer& layer,SqObject scriptThread)
		{
			MIRICALE_SCRIPT_TEST_CLASS_FUNC();
			mrcScrASSERT_MSG( sq_isgenerator(scriptThread.GetObject()) , "arg is not generator" );
			mrcScrASSERT_MSG(layer.isValid(),"mrcLayer is invalid");
			return GlobalThreadMgr::getInstance().createThread(scriptThread,layer.getChecker());
		}

		void killThread(SqObject scriptThread)
		{
			MIRICALE_SCRIPT_TEST_CLASS_FUNC();
			mrcScrASSERT_MSG( sq_isgenerator(scriptThread.GetObject()) , "arg is not generator" );
			GlobalThreadMgr::killThread(scriptThread);
		}

	private:
		static bool isValid() { return true; }
	};

	class SqKeyboard : public SqObjectType
	{
		MIRACLE_FRIEND_CLASS_SINGLETON(SqKeyboard);
		friend class Sqrat::DefaultAllocator<SqKeyboard>;
	public:
		static SqKeyboard& getInstance()
		{
			typedef Loki::SingletonHolder<SqKeyboard,Loki::CreateUsingNew,
				Loki::LongevityLifetime::DieDependencyMgr > SqKeyboardS;

			return SqKeyboardS::Instance();
		}

		void setEventFunc(SqFunction func)
		{
			MIRICALE_SCRIPT_TEST_CLASS_FUNC();
			mrcScrASSERT(!func.IsNull());
			Keyboard::getInstance().setEventFunc(func);
		}

		void clear()
		{
			MIRICALE_SCRIPT_TEST_CLASS_FUNC();
			Keyboard::getInstance().clear();
		}
	private:
		static bool isValid() { return true; }
	};

	namespace
	{
		void sqStdPrint(HSQUIRRELVM,const SQChar * msg,...)
		{
			F_Print funcPrint = getStandardPrint();
			mrcASSERT(funcPrint);
	
			static SQChar temp[2048];

			va_list vl;
			va_start(vl,msg);
			_vsntprintf( temp,sizeof(temp),msg,vl);
			va_end(vl);

			funcPrint(temp);
		}

		void printSQDBGError(HSQUIRRELVM v) 
		{
			SQPRINTFUNCTION pf = sq_geterrorfunc(v);
			if(pf) {
				const SQChar *sErr = 0;
				if(sq_gettop(v)>=1) {
					if(SQ_SUCCEEDED(sq_getstring(v,2,&sErr)))	{
						pf(v,_SC("\nSquirrel Error : [%s]\n"),sErr);
					}
					else{
						pf(v,_SC("\\nSquirrel Error : [%s]\n"));
					}
				}
			}

			sqstd_printcallstack(v);
		} // printSQDBGError

		SQInteger printSQError(HSQUIRRELVM v)
		{
			printSQDBGError(v);
			return 0;
		}

		void printCompilerError(HSQUIRRELVM v,const SQChar *sErr,const SQChar *sSource,SQInteger line,SQInteger column)
		{
			SQPRINTFUNCTION pf = sq_geterrorfunc(v);
			mrcASSERT(pf);
			pf(v,_SC("%s line = (%d) column = (%d) : error %s\n"),sSource,line,column,sErr);
		}

		void registerVectorHelper(ImprovedClass<SqVector3>& vecClass, SqVector3) 
		{ 
			vecClass.Ctor<real,real,real>();
			vecClass.Var(_mrcT("z"),&SqVector3::z); 
		}
		void registerVectorHelper(ImprovedClass<SqVector2>& vecClass, SqVector2) 
		{
			vecClass.Ctor<real,real>();
		}

		template<class T>
		void registerVector(const String& vectorName)
		{
			ImprovedClass<T> vectorClass;
			vectorClass.Func<T (T::*)( const T& ) const>(_mrcT("_add"),&T::operator+);
			vectorClass.Func<T (T::*)( const T& ) const>(_mrcT("_sub"),&T::operator-);
			vectorClass.Func<T (T::*)( const T& ) const>(_mrcT("_mul"),&T::operator*);
			vectorClass.Func<T (T::*)( const T& ) const>(_mrcT("_div"),&T::operator/);
	//		vectorClass.Func<bool (T::*)( const T& ) const>(_mrcT("_cmp"),&T::operator==);
			vectorClass.Func(_mrcT("compare"),&T::compare);
			vectorClass.Var(_mrcT("x"),&T::x);
			vectorClass.Var(_mrcT("y"),&T::y);
			registerVectorHelper(vectorClass,T());

			RootTable().Bind(vectorName.c_str(), vectorClass);
		}

		void registerMrcSquirrelClass()
		{
			ConstTable().Enum(_mrcT("Keyboard"), Enumeration()
				.Const(_mrcT("K_DOWN"),	  Keyboard::K_DOWN)
				.Const(_mrcT("K_UP"),	  Keyboard::K_UP)
				.Const(_mrcT("K_LEFT"),	  Keyboard::K_LEFT)
				.Const(_mrcT("K_RIGHT"),  Keyboard::K_RIGHT)
				.Const(_mrcT("K_SPACE"),  Keyboard::K_SPACE)
				.Const(_mrcT("K_1"),	  Keyboard::K_1)
				.Const(_mrcT("K_2"),	  Keyboard::K_2)
				.Const(_mrcT("K_3"),	  Keyboard::K_3)
				.Const(_mrcT("K_4"),	  Keyboard::K_4)
				.Const(_mrcT("K_5"),	  Keyboard::K_5)
				.Const(_mrcT("K_6"),	  Keyboard::K_6)
				.Const(_mrcT("K_7"),	  Keyboard::K_7)
				.Const(_mrcT("K_8"),	  Keyboard::K_8)
				.Const(_mrcT("K_9"),	  Keyboard::K_9)
				.Const(_mrcT("K_0"),	  Keyboard::K_0)

				.Const(_mrcT("K_A"),	  Keyboard::K_A)
				.Const(_mrcT("K_B"),	  Keyboard::K_B)
				.Const(_mrcT("K_C"),	  Keyboard::K_C)
				.Const(_mrcT("K_D"),	  Keyboard::K_D)
				.Const(_mrcT("K_E"),	  Keyboard::K_E)
				.Const(_mrcT("K_F"),	  Keyboard::K_F)
				.Const(_mrcT("K_G"),	  Keyboard::K_G)
				.Const(_mrcT("K_H"),	  Keyboard::K_H)
				.Const(_mrcT("K_I"),	  Keyboard::K_I)
				.Const(_mrcT("K_J"),	  Keyboard::K_J)
				.Const(_mrcT("K_K"),	  Keyboard::K_K)
				.Const(_mrcT("K_L"),	  Keyboard::K_L)
				.Const(_mrcT("K_M"),	  Keyboard::K_M)
				.Const(_mrcT("K_N"),	  Keyboard::K_N)
				.Const(_mrcT("K_O"),	  Keyboard::K_O)
				.Const(_mrcT("K_P"),	  Keyboard::K_P)
				.Const(_mrcT("K_Q"),	  Keyboard::K_Q)
				.Const(_mrcT("K_R"),	  Keyboard::K_R)
				.Const(_mrcT("K_S"),	  Keyboard::K_S)
				.Const(_mrcT("K_T"),	  Keyboard::K_T)
				.Const(_mrcT("K_U"),	  Keyboard::K_U)
				.Const(_mrcT("K_V"),	  Keyboard::K_V)
				.Const(_mrcT("K_W"),	  Keyboard::K_W)
				.Const(_mrcT("K_X"),	  Keyboard::K_X)
				.Const(_mrcT("K_Y"),	  Keyboard::K_Y)
				.Const(_mrcT("K_Z"),	  Keyboard::K_Z)

				);

			ConstTable().Enum(_mrcT("NUMBER_ALIGN"), Enumeration()
				.Const(_mrcT("NUMBER_ALIGN_LEFT"),	  SpriteNumber::NUMBER_ALIGN_LEFT)
//				.Const(_mrcT("NUMBER_ALIGN_CENTER"),  SpriteNumber::NUMBER_ALIGN_CENTER)
				.Const(_mrcT("NUMBER_ALIGN_RIGHT"),   SpriteNumber::NUMBER_ALIGN_RIGHT)			
				);

			ConstTable().Enum(_mrcT("NUMBER_OPTION"), Enumeration()
				.Const(_mrcT("NUMBER_OPTION_EMPTY"),	  SpriteNumber::NUMBER_OPTION_EMPTY)
				.Const(_mrcT("NUMBER_OPTION_FILL_ZERO"),  SpriteNumber::NUMBER_OPTION_FILL_ZERO)
				.Const(_mrcT("NUMBER_OPTION_PLUS_MINUS"), SpriteNumber::NUMBER_OPTION_PLUS_MINUS)			
				);

			ConstTable().Enum(_mrcT("EASE_METHOD"), Enumeration()
				.Const(_mrcT("LINEAR"),	  ease::EASE_LINEAR)
				.Const(_mrcT("EXPO_IN"),  ease::EASE_EXPO_IN)
				);

		
			ClassType<SqVector2>::Initialized() = false;
			ClassType<SqVector3>::Initialized() = false;

			registerVector<SqVector2>(_mrcT("mrcVector2"));
			registerVector<SqVector3>(_mrcT("mrcVector3"));


			ClassType<SqPoint>::Initialized() = false;
			RootTable().Bind(_mrcT("mrcPoint"), ImprovedClass<SqPoint>()
				.Ctor<int16,int16>()
				.Var(_mrcT("x"),&SqPoint::mX)
				.Var(_mrcT("y"),&SqPoint::mY)
				);

			ClassType<SqRect>::Initialized() = false;
			RootTable().Bind(_mrcT("mrcRect"), ImprovedClass<SqRect>()
				.Ctor<int16,int16,int16,int16>()
				.Var(_mrcT("left"),&SqRect::mLeft)
				.Var(_mrcT("top"),&SqRect::mTop)
				.Var(_mrcT("right"),&SqRect::mRight)
				.Var(_mrcT("bottom"),&SqRect::mBottom)
				);


			ClassType<SqColorRGB>::Initialized() = false;
			RootTable().Bind(_mrcT("mrcColor"), ImprovedClass<SqColorRGB>()
				.Ctor<real,real,real>()
				.Var(_mrcT("r"),&SqColorRGB::r)
				.Var(_mrcT("g"),&SqColorRGB::g)
				.Var(_mrcT("b"),&SqColorRGB::b)
				.Func(_mrcT("compare"),&SqColorRGB::compare)
				);

			
			//////////////////////////////////////////////////////////////////////////
			
			ClassType<SqLayer>::Initialized() = false;
			RootTable().Bind(_mrcT("mrcLayer"), Class<SqLayer>()
				.MAKE_SQ_FUNC(SqLayer,getId)
				.MAKE_SQ_FUNC(SqLayer,pushLinePositionQ)
				.MAKE_SQ_FUNC(SqLayer,pushBezierPositionQ)
				.MAKE_SQ_FUNC(SqLayer,setPositionQLoop)
				.MAKE_SQ_FUNC(SqLayer,clearPositionQ)
				.MAKE_SQ_FUNC(SqLayer,pushRotationQ)
				.MAKE_SQ_FUNC(SqLayer,setRotationQLoop)
				.MAKE_SQ_FUNC(SqLayer,clearRotationQ)
				.MAKE_SQ_FUNC(SqLayer,pushScaleQ)
				.MAKE_SQ_FUNC(SqLayer,setScaleQLoop)
				.MAKE_SQ_FUNC(SqLayer,clearScaleQ)
				.MAKE_SQ_FUNC(SqLayer,pushPivotRotationQ)
				.MAKE_SQ_FUNC(SqLayer,setPivotRotationQLoop)
				.MAKE_SQ_FUNC(SqLayer,clearPivotRotationQ)
				.MAKE_SQ_FUNC(SqLayer,pushRGBQ)
				.MAKE_SQ_FUNC(SqLayer,setRGBQLoop)
				.MAKE_SQ_FUNC(SqLayer,clearRGBQ)
				.MAKE_SQ_FUNC(SqLayer,pushAlphaQ)
				.MAKE_SQ_FUNC(SqLayer,setAlphaQLoop)
				.MAKE_SQ_FUNC(SqLayer,clearAlphaQ)

				

				//////////////////////////////////////////////////////////////////////////




				.MAKE_SQ_FUNC(SqLayer,setPositionv)
				.MAKE_SQ_FUNC(SqLayer,setPosition)

				.MAKE_SQ_FUNC(SqLayer,setRotation)
				.MAKE_SQ_FUNC(SqLayer,setPivotRotation)

				.MAKE_SQ_FUNC(SqLayer,setScalev)
				.MAKE_SQ_FUNC(SqLayer,setScale)
				.MAKE_SQ_FUNC(SqLayer,setScaleUnit)

				.MAKE_SQ_FUNC(SqLayer,setRGBc)
				.MAKE_SQ_FUNC(SqLayer,setRGB)
				.MAKE_SQ_FUNC(SqLayer,setAlpha)

				.MAKE_SQ_FUNC(SqLayer,getPosition)
				.MAKE_SQ_FUNC(SqLayer,getRotation)
				.MAKE_SQ_FUNC(SqLayer,getPivotRotation)
				.MAKE_SQ_FUNC(SqLayer,getScale)
				.MAKE_SQ_FUNC(SqLayer,getRGB)
				.MAKE_SQ_FUNC(SqLayer,getAlpha)

				.MAKE_SQ_FUNC(SqLayer,setPivotAbs)
				.MAKE_SQ_FUNC(SqLayer,setPivot)
				.MAKE_SQ_FUNC(SqLayer,setPivotAbsv)
				.MAKE_SQ_FUNC(SqLayer,setPivotv)

				.MAKE_SQ_FUNC(SqLayer,getPivot)


				.MAKE_SQ_FUNC(SqLayer,setUVVelocityv)
				.MAKE_SQ_FUNC(SqLayer,getUVVelocity)
				.MAKE_SQ_FUNC(SqLayer,setMoveVelocityv)
				.MAKE_SQ_FUNC(SqLayer,getMoveVelocity)
				.MAKE_SQ_FUNC(SqLayer,setRotationVelocity)
				.MAKE_SQ_FUNC(SqLayer,getRotationVelocity)
				.MAKE_SQ_FUNC(SqLayer,setPivotRotationVelocity)
				.MAKE_SQ_FUNC(SqLayer,getPivotRotationVelocity)

				.MAKE_SQ_FUNC(SqLayer,getSpriteDefault)
				.MAKE_SQ_FUNC(SqLayer,getSpriteButton)
				.MAKE_SQ_FUNC(SqLayer,getSpriteNumber)

				.MAKE_SQ_FUNC(SqLayer,createChild)
				.MAKE_SQ_FUNC(SqLayer,createChildWidthSpriteDefault)
				.MAKE_SQ_FUNC(SqLayer,createChildWithSpriteButton)
				.MAKE_SQ_FUNC(SqLayer,createChildWithSpriteNumber)

				.MAKE_SQ_FUNC(SqLayer,destroy)
				);

			ClassType<SqLayerMgr>::Initialized() = false;
			RootTable().Bind(_mrcT("class_mrcLayerMgr"), Class<SqLayerMgr>()
				.MAKE_SQ_FUNC(SqLayerMgr,moveBeforeLayer)
				.MAKE_SQ_FUNC(SqLayerMgr,moveAfterLayer)
				.MAKE_SQ_FUNC(SqLayerMgr,createLayerWithSpriteDefault)
				.MAKE_SQ_FUNC(SqLayerMgr,createLayerWithSpriteButton)
				.MAKE_SQ_FUNC(SqLayerMgr,createLayerWithSpriteNumber)
				);

			RootTable().SetInstance(_mrcT("mrcLayerMgr"),&SqLayerMgr::getInstance());


			ClassType<SqSpriteDefault>::Initialized() = false;
			RootTable().Bind(_mrcT("mrcSpriteDefault"), Class<SqSpriteDefault>()
				.MAKE_SQ_FUNC(SqSpriteDefault,enableMessageProcedure)
				.MAKE_SQ_FUNC(SqSpriteDefault,setTexture)
				.MAKE_SQ_FUNC(SqSpriteDefault,setFilter)
//				.Func(_mrcT("setRGB"), &SqSpriteDefault::setRGB)
//				.Func(_mrcT("setAlpha"), &SqSpriteDefault::setAlpha)
				.MAKE_SQ_FUNC(SqSpriteDefault,setScreenEffect)
				.MAKE_SQ_FUNC(SqSpriteDefault,setScissorRect)
				.MAKE_SQ_FUNC(SqSpriteDefault,getTextureDataID)
				.MAKE_SQ_FUNC(SqSpriteDefault,isFilter)
//				.Func(_mrcT("getRGB"), &SqSpriteDefault::getRGB)
//				.Func(_mrcT("getAlpha"), &SqSpriteDefault::getAlpha)
				.MAKE_SQ_FUNC(SqSpriteDefault,getScreenEffect)
				.MAKE_SQ_FUNC(SqSpriteDefault,getScissorRect)
				.MAKE_SQ_FUNC(SqSpriteDefault,setWidth)
				.MAKE_SQ_FUNC(SqSpriteDefault,getWidth)
				.MAKE_SQ_FUNC(SqSpriteDefault,setHeight)
				.MAKE_SQ_FUNC(SqSpriteDefault,getHeight)
				.MAKE_SQ_FUNC(SqSpriteDefault,setWidthHeight)
				.MAKE_SQ_FUNC(SqSpriteDefault,setUV)
				.MAKE_SQ_FUNC(SqSpriteDefault,getUV)
				.MAKE_SQ_FUNC(SqSpriteDefault,setUVPixel)
				.MAKE_SQ_FUNC(SqSpriteDefault,setUVFlow)
				.MAKE_SQ_FUNC(SqSpriteDefault,getUVFlow)
				.MAKE_SQ_FUNC(SqSpriteDefault,recoverUV)

				.MAKE_SQ_FUNC(SqSpriteDefault,setCallBackMouseEnter)
				.MAKE_SQ_FUNC(SqSpriteDefault,setCallBackMouseLeave)
				.MAKE_SQ_FUNC(SqSpriteDefault,setCallBackMouseLButtonClick)
				.MAKE_SQ_FUNC(SqSpriteDefault,setCallBackMouseLButtonDown)
				.MAKE_SQ_FUNC(SqSpriteDefault,setCallBackMouseLButtonUp)
				.MAKE_SQ_FUNC(SqSpriteDefault,setCallBackMouseRButtonClick)
				.MAKE_SQ_FUNC(SqSpriteDefault,setCallBackMouseRButtonDown)
				.MAKE_SQ_FUNC(SqSpriteDefault,setCallBackMouseRButtonUp)

				.MAKE_SQ_FUNC(SqSpriteDefault,resetFrame)
				.MAKE_SQ_FUNC(SqSpriteDefault,pauseFrame)
				.MAKE_SQ_FUNC(SqSpriteDefault,playFrame)
				.MAKE_SQ_FUNC(SqSpriteDefault,setSpeedFrame)
				.MAKE_SQ_FUNC(SqSpriteDefault,setLoopFrame)

				.MAKE_SQ_STATIC_FUNC(SqSpriteDefault,setWidthHeightFrameStyle)
				);

			ClassType<SqSpriteButton>::Initialized() = false;
			RootTable().Bind(_mrcT("mrcSpriteButton"), DerivedClass<SqSpriteButton,SqSpriteDefault>()
				);

			ClassType<SqSpriteNumber>::Initialized() = false;
			RootTable().Bind(_mrcT("mrcSpriteNumber"), DerivedClass<SqSpriteNumber,SqSpriteDefault>()
				.MAKE_SQ_FUNC(SqSpriteNumber,setNumber)
				.MAKE_SQ_FUNC(SqSpriteNumber,getNumber)
				.MAKE_SQ_FUNC(SqSpriteNumber,setNumberOption)
				.MAKE_SQ_FUNC(SqSpriteNumber,setNumberAlign)
				.MAKE_SQ_FUNC(SqSpriteNumber,setCipher)
				.MAKE_SQ_FUNC(SqSpriteNumber,setGap)
				);

			ClassType<SqThreadMgr>::Initialized() = false;
			RootTable().Bind(_mrcT("class_mrcThreadMgr"), Class<SqThreadMgr>()
				.MAKE_SQ_FUNC(SqThreadMgr,createThread)
				.MAKE_SQ_FUNC(SqThreadMgr,createLayerThread)
				.MAKE_SQ_FUNC(SqThreadMgr,killThread)
				);

			RootTable().SetInstance(_mrcT("mrcThreadMgr"),&SqThreadMgr::getInstance());


			ClassType<SqCallBackFuncMgr>::Initialized() = false;
			RootTable().Bind(_mrcT("class_mrcCallBackFuncMgr"), Class<SqCallBackFuncMgr>()
				.MAKE_SQ_FUNC(SqCallBackFuncMgr,registerCallBack)
				.MAKE_SQ_FUNC(SqCallBackFuncMgr,registerLayerCallBack)
				.MAKE_SQ_FUNC(SqCallBackFuncMgr,eraseCallBack)				
				);

			RootTable().SetInstance(_mrcT("mrcCallBackFuncMgr"),&SqCallBackFuncMgr::getInstance());

			ClassType<SqKeyboard>::Initialized() = false;
			RootTable().Bind(_mrcT("class_mrcKeyboard"), Class<SqKeyboard>()
				.MAKE_SQ_FUNC(SqKeyboard,setEventFunc)
				.MAKE_SQ_FUNC(SqKeyboard,clear)
				);

			RootTable().SetInstance(_mrcT("mrcKeyboard"),&SqKeyboard::getInstance());


			ClassType<SqDevice>::Initialized() = false;
			RootTable().Bind(_mrcT("class_mrcDevice"), Class<SqDevice>()
				.MAKE_SQ_FUNC(SqDevice,getWidthHeight)
				.MAKE_SQ_FUNC(SqDevice,showDebugWire)
				);

			RootTable().SetInstance(_mrcT("mrcDevice"),&SqDevice::getInstance());
		}
	}

	SqMain& SqMain::getInstance()
	{
		typedef Loki::SingletonHolder<SqMain,Loki::CreateUsingNew,
			Loki::LongevityLifetime::DieDependencyMgr > SqMainS;

		return SqMainS::Instance();
	}

	SqMain::SqMain()
	{
		mVM = NULL;
		mRDBG = NULL;

		mIsRemoteDebug = false;
	}

	void SqMain::init(const filesystem::Path& path,bool isRemoteDebug)
	{
		mPath = path;
		mIsRemoteDebug =  isRemoteDebug;
	}

	void SqMain::release()
	{
		// 이걸해야 새로 생성되는 VM에 전에 생성되었던 SqLayer에 의한 
		// 찌꺼기로 유발되는 에러가 발생하지 않음
		
		IRenderer::getInstance().showDebugWire(false);

		Keyboard::getInstance().clear();
		GlobalThreadMgr::getInstance().clear();
		GlobalCallBackFuncMgr::getInstance().clear();
		LayerMgr::getInstance().clear();

		if (mRDBG) 
			sq_rdbg_shutdown(mRDBG);
	
		if(mVM)
			sq_close(mVM);

		mVM = NULL;
		mRDBG = NULL;
	}

	void SqMain::begin()
	{
		mrcASSERT(mVM);
		Sqrat::DefaultVM::Set(mVM);

		// 혹시라도 예외가 던져져서 flag 값을 리셑 못시켰다면 문제가 발생함으로... 리셑
		resetAssertArg();
	}

	void SqMain::end()
	{
		Sqrat::DefaultVM::Set(NULL);
	}

	static void getNutFiles(const filesystem::Path& path,vector<filesystem::Path>* pStringVector)
	{
		mrcASSERT( filesystem::is_directory(path) );
		filesystem::DirectoryIterator end_iter;
		for ( filesystem::DirectoryIterator dir_itr( path ); dir_itr != end_iter; ++dir_itr )
		{
			if ( filesystem::is_directory( dir_itr->status() ) )
			{
				getNutFiles(dir_itr->path(),pStringVector);
			}
			else if ( filesystem::is_regular_file( dir_itr->status() ) )
			{
				if( filesystem::Path(_mrcT(".nut")) == dir_itr->path().extension() )
				{
					pStringVector->push_back(dir_itr->path());
				}
			}
		}
	}

	void SqMain::loadScript()
	{
		release();

		mrcASSERT(!mVM);
		mVM = sq_open(1024);
		

		sq_setprintfunc(mVM, &sqStdPrint,&sqStdPrint);
		sq_pushroottable(mVM);

		sqstd_register_iolib(mVM);
		sqstd_register_bloblib(mVM);
		sqstd_register_mathlib(mVM);
		sqstd_register_stringlib(mVM);
		sqstd_register_systemlib(mVM);

		sq_setcompilererrorhandler(mVM,&printCompilerError);

		sq_newclosure(mVM,&printSQError,0);
		sq_seterrorhandler(mVM);
		sq_pop(mVM,1);

		if( mIsRemoteDebug )
		{
			mrcASSERT( !mRDBG );
			mRDBG = sq_rdbg_init(mVM,1234,SQTrue);
			if(mRDBG) 
			{
				// Enable debug info generation (for the compiler).
				sq_enabledebuginfo(mVM,SQTrue);
				scprintf(_mrcT("Waiting for SQDBG connection..."));
				// Suspends the app until the debugger client connects.
				if(SQ_SUCCEEDED(sq_rdbg_waitforconnections(mRDBG))) {
					scprintf( _mrcT("SQDBG: connected.\n"));
				} // if
			} else {
				printSQDBGError(mVM);
			} // if
		}


		//////////////////////////////////////////////////////////////////////////
		begin();

		registerMrcSquirrelClass();

		runRegisteredScriptObject();

		filesystem::Path fullPath = getFullPath();
		vector<filesystem::Path> pathVec;
		getNutFiles(fullPath,&pathVec);
		
		try
		{
			for (size_t i=0; i < pathVec.size(); ++i)
			{
				Sqrat::Script script;
				script.CompileFile( toString(pathVec[i]) );
				script.Run();
			}
		}
		catch(const Sqrat::Exception& e) 
		{
			ShowMessage( NString("script load filed : ") + toNString(e.Message()) );
			loadScript();
		}
		catch(const Exception& e)
		{
			ShowMessage( NString("script load filed : ") + e.getMessage() );
			loadScript();
		}

		//////////////////////////////////////////////////////////////////////////
		end();
	}

	filesystem::Path SqMain::getFullPath()
	{
		return mPath;
//		return mrc::filesystem::initial_path() / mPath;
	}

	bool SqMain::isRunningScript()
	{
		return Sqrat::DefaultVM::Get() != NULL;
	}
	
	
}
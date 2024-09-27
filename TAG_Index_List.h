// 코드를 추적할 수 있게끔 하는 테그 리스트

// .h 파일을 사용한 이유는 색상을 활용하기 위함임


// 예제)
// 재귀 #R = Recursive

//////////////////////////////////////////////////////////////////////////
/*  현재 임시 리스트                                                    */

namespace EngineLib
{
#define 무한의계단 EngineLib

    enum 리소스_메니저
    {
	    TAG_UpdateResourceManager,
	    TAG_LoadResourceManager,
	    TAG_ClassTextureManager,
	    TAG_CreateResourceManager,
	    TAG_StartResourceThreadManager,
    }

	enum 애니메이션_쓰레드
	{
		TAG_ProcessAnimationThread,
		TAG_CalculateAnimationThread,
	}
}
	
namespace RanLogic
{
    enum 무한의_계단
    {
	    TAG_InitClientTriggerSystem,
	    TAG_PVEClientStartStage,
    }
}

namespace RanLogicClient
{
    enum 무한의_계단
    {
	    TAG_PVEClientStartStage,
    }
}

namespace MatchingSystem
{

	enum 토너먼트
	{
		TOURNAMENT_UNIQ_DISABLE, // 토너먼트일때 유일하게 안되는기능들 
	};
}
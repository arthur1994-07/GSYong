#ifndef ServerPVESystem_H
#define ServerPVESystem_H

#include "../../RanLogic/PVE/MapTable.h"
#include "../../RanLogic/PVE/PVEInDunID.h"
#include "../../RanLogic/Party/GLPartyDefine.h"
#include "../TriggerSystem/ServerMapInfoMessage.h"

struct GLPartyAgent;

namespace PVESystem
{
    class CPVEInDun;

	/**
		PVE DB 를 초기화 시킨다.

		DB 를 초기화, 로딩 시킨다.
		DB 를 사용하는 서버에 한해서(현재 에이전트) 호출 한다.

	 */
	bool			        InitPVEDB();

	/**
		PVE DB 를 파괴 한다.
		내부적으로 DB 를 파일로 저장한 후 자료구조를 삭제 한다.

	 */
	void			        DestroyPVEDB();

    /**
    	미리 스크립트로 부터 인던 정보를 캐쉬 해 둔다.
        몇몇 서버는 DB는 사용하지 않고 정보 캐쉬만 사용할 수도 있다.
        그래서 따로 둔다.

     */
    void                    InitPVECache();

    /**
    	인던 정보를 캐쉬를 삭제 한다.
        서버를 리셋하면 다시 읽어야 하므로 삭제 한다.

     */
    void                    DestroyPVECache();
	void					DestroyPVEIndun();

    /**
    	입력되는 테이블에 현재 시간 타임스템프를 입력해 준다.
    	입력 편의 및 시간 입력을 한곳으로 모으기 위함.

     */
    void                    SetCurTime(MAP_TABLE& tbl);

    /**
    	현재 시간에서 입력된 타임 스템프와의 차이를 입력된 기준 초에서의 남은 초를 출력 한다.

    	\param timestamp 현재 타임스템프와 차이를 계산할 타임 스템프값
    	\param sec 기준 초, 이를 기준으로 계산된 차이를 출력 한다.
    	\return 기준 초를 기준으로 남은 초
     */
    _TIME_TYPE              DiffTime(_TIME_TYPE timestamp, int sec);

	/**
		맵 정보 를 찾는다.

		\param info 찾을 맵 ID 와 찾은 맵 정보
			        info.mapid 는 미리 입력 되어야 맵 정보를 찾을 수 있다.
		\return 성공 유무
	 */
    bool			        GetMapInfo(TriggerSystem::SServerTriggerMapInfo& info);

	/**
		맵 DB 의 레코드 를 찾는다.
		만약 해당 레코드가 없다면 레코드를 생성 한다.

		\param tbl 찾을 맵 정보 및 찾은 맵 레코드 정보
			       tbl.dwCharNum 와 tbl.dwMapID 는 미리 입력 되어야 맵 정보를 찾을 수 있다.
		\return 성공 유무
	 */
	bool			        GetMapTable(MAP_TABLE& tbl);
    bool			        GetMapTable( MAP_TABLE& tbl OUT, const DWORD& dwChaNum IN, const DWORD& dwMapID IN, const int& nLevel IN );
	const MAP_TABLE	        GetMapTable(const SPVEInDunID& id);
	//bool					GetMapTable(MAP_TABLE& tbl, DWORD dwCharDbNum, DWORD dwMapID, int nLevel);	

	/**
		맵 DB 에 레코드를 설정 한다.
		외부에서는 strID 는 설정할 필요가 없다.

		\param sMapTbl 설정할 테이블 레코드
	 */
	bool			        SetMapTable(const MAP_TABLE& tbl);

    /**
        해당 ID 를 가지는 캐릭터의 맵테이블을 모든 모은다.
        클라이언트에 보내기 위함임에 주의 한다.

    	\param dwChaNum 캐릭터 DB ID
        \param con 모아 담을 컨테이너
		\param tbl 현재 자신이 속한 인던 정보
    	\return 성공 유무
     */
    bool                    CollectMapTable(DWORD dwChaNum, MapTableContainer& con, MAP_TABLE& tbl);

    /** 
    	인던 상태 처리기를 찾는다.
        만약 없다면 새로 생성하여 전달 한다.
        모든 인던은 이를 통해 관리 된다.

     */
    CPVEInDun*              FindInstantDungeon(const SPVEInDunID& sInDunID);

    /**
        인던 멤버들을 설정, 삭제 한다.

     */
    void                    InsertInDunMember(const DWORD dwChaDbNum, const SPVEInDunID& sInDunID);
    void                    RemoveInDunMember(const DWORD dwChaDbNum);

    /** 
    	입력된 DB 캐릭터가 포함 되어있는 인던 상태 처리기를 찾는다.
		이것을 응용하면 인던에 소속된 캐릭터 인지를 확인 할 수 있다.
		외부에서 PVE 인던에 소속된 상태인지를 알고 싶을때 활용할 수 있다.

     */
    CPVEInDun*              FindInstantDungeon(const DWORD dwChaDbNum);

    /**
    	PVE 시스템을 갱신 시킨다.
        
    	\param fElpaseTime 이전 프레임에서 현재 프레임 갱신 시간(1 = 1초)
     */
    void                    FrameMove(float fElpaseTime);

} // end namespace

#endif

#pragma once

#include "../Post/PostDefine.h"
#include "GLContrlBaseMsg.h"
#include "../Vehicle/GLVEHICLE.h"


//
//mjeon.sns
//
enum EMSNS_RESULT
{
	EMSNS_RES_OK					= 0,	// OK
	EMSNS_RES_CHA_NOTEXIST			= 1,	// There does not exist the character.
	EMSNS_RES_TARGET_NOTEXIST		= 2,	// There does not exist the target character.
	EMSNS_RES_WRONG_SKEY			= 3,	// Wrong SKey
	EMSNS_RES_WRONG_UID				= 4,	// Wrong UID
	EMSNS_RES_WRONG_ATOKEN			= 5,	// Wrong AToken
	EMSNS_RES_WRONG_ATOKENS			= 6,	// Wrong ATokenS
	EMSNS_RES_DB_ERROR				= 7,	// DB Error
};

enum EMBUS_TAKE
{
	EMBUS_TAKE_FAIL			= 0,	//	일반오류.
	EMBUS_TAKE_OK			= 1,	//	탑승 성공.
	EMBUS_TAKE_TICKET		= 2,	//	티켓 부족.
	EMBUS_TAKE_CONDITION	= 3,	//	탑승 여건이 안됨.
	EMBUS_TAKE_PK_LEVEL		= 4,	//	pk 성향이 낮아서 탑승 불가능.
};

enum EMTAXI_TAKE
{
	EMTAXI_TAKE_FAIL		= 0,	//	일반오류.
	EMTAXI_TAKE_OK			= 1,	//	탑승 성공.
	EMTAXI_TAKE_TICKET		= 2,	//	티켓 부족.
	EMTAXI_TAKE_MONEY		= 3,	//	금액 부족.
	EMTAXI_TAKE_CONDITION	= 4,	//	탑승 여건이 안됨.
	EMTAXI_TAKE_MAPFAIL		= 5,	//  맵 오류
	EMTAXI_TAKE_STATIONFAIL	= 6,	//  정류장 오류
	EMTAXI_TAKE_NPCFAIL		= 7,	//	NPC 설정 오류
};

enum EMREQ_FIRECRACKER_FB
{
	EMREQ_FIRECRACKER_FB_FAIL	= 0,	//	일반 오류.
	EMREQ_FIRECRACKER_FB_OK		= 1,	//	사용 성공.
	EMREQ_FIRECRACKER_FB_NOITEM	= 2,	//	아이템 오류.
};

//---------- 케릭터 부활 resurrection -----------;
enum EMRESURRECTION_TYPE
{
    EM_RESURRECTION_NOMAL               = 0x0001,  // 일반적인 부활 ( 시작 장소에서 부활 );
    EM_RESURRECTION_SPECIAL             = 0x0002,  // 특수 부활 ( 경험치 부활 : 경험치를 소모하여 현재 장소에서 부활 );
    EM_RESURRECTION_SPECIAL_ITEM        = 0x0004,  // 특수 부활 + 아이템 ( 귀환주 부활 : 귀혼주 아이템을 이용 경험치 소모없이 현재 장소에서 부활 );
    EM_RESURRECTION_SKILL               = 0x0008,  // 스킬 부활 ( 스킬이나 효과에 의해서 현재 장소에서 부활 );

    EM_RESURRECTION_FORCED              = 0x0010,  // 강제 부활 ( 특수 컨텐츠에 의해서 강제적으로 부활 );

    EM_RESURRECTION_ALL                 = 0x00ff,  // 모든 부활;

    EM_RESURRECTION_DISABLE_SKILL       = 0x0800,  // 스킬 부활 불가능 플래그;

    EM_RESURRECTION_TYPE_NOT_SET        = 0x1000,  // 부활 타입 미설정 중;
};

enum EMREQ_RESURRECT_NOMAL_FB
{
    EMREQ_RESURRECT_NOMAL_FB_ENTRY_FAIL    = 0,    // 부활에 의한 맵이동 때 맵 진입 조건 실패;
    EMREQ_RESURRECT_NOMAL_FB_OK            = 1,    // 부활 성공;
};

enum EMREQ_RESURRECT_SPECIAL_ITEM_FB
{
    EMREQ_RESURRECT_SPECIAL_ITEM_FB_FAIL		= 0,	//	일반오류.
    EMREQ_RESURRECT_SPECIAL_ITEM_FB_OK			= 1,	//	사용성공.
    EMREQ_RESURRECT_SPECIAL_ITEM_FB_NOITEM		= 2,	//	아이템이 없음.
    EMREQ_RESURRECT_SPECIAL_ITEM_FB_NOTUSE		= 3,	//	사용 불가능한 지역.
    EMREQ_RESURRECT_SPECIAL_ITEM_FB_COOLTIME	= 4,	//	아이템 쿨타임
};

enum EMREQ_RESURRECT_SPECIAL_FB
{
    EMREQ_RESURRECT_SPECIAL_FB_FAIL		= 0,	//	일반오류.
    EMREQ_RESURRECT_SPECIAL_FB_OK		= 1,	//	사용성공.
    EMREQ_RESURRECT_SPECIAL_FB_NOMONEY	= 2,	//	돈이 부족
    EMREQ_RESURRECT_SPECIAL_FB_NOEXP	= 3,	//	회복할 경험치가 없음
    EMREQ_RESURRECT_SPECIAL_FB_NOTUSE	= 4,	//	사용 불가능한 지역.
};
//----------------------------------------------;

enum EMREQ_RECOVERY_NPC_FB
{
	EMREQ_RECOVERY_NPC_FB_FAIL		= 0,	//	일반오류.
	EMREQ_RECOVERY_NPC_FB_OK		= 1,	//	사용성공.
	EMREQ_RECOVERY_NPC_FB_NOMONEY	= 2,	//	돈이 부족
	EMREQ_RECOVERY_NPC_FB_NOREEXP	= 3,	//	회복할 경험치가 없음
	EMREQ_RECOVERY_NPC_FB_NOTUSE	= 4,	//	사용 불가능한 지역.
};

enum EMINVEN_HAIR_CHANGE_FB
{
	EMINVEN_HAIR_CHANGE_FB_FAIL		= 0,	//	일반오류.
	EMINVEN_HAIR_CHANGE_FB_OK		= 1,	//	성공.
	EMINVEN_HAIR_CHANGE_FB_NOITEM	= 2,	//	아이템이 없음.
	EMINVEN_HAIR_CHANGE_FB_BADITEM	= 3,	//	사용 불가능 아이템.
	EMINVEN_HAIR_CHANGE_FB_BADCLASS	= 4,	//	케릭터 직업이 틀려서 사용 불가능 아이템.
	EMINVEN_HAIR_CHANGE_FB_COOLTIME = 5,	//	쿨타임 실패~
};

enum EMINVEN_COSTUM_COLOR_CHANGE_FB // 코스튬 염색 관련. by luxes.
{
	EMINVEN_COSTUM_COLOR_CHANGE_FB_FAIL		= 0,	//	일반오류.
	EMINVEN_COSTUM_COLOR_CHANGE_FB_OK		= 1,	//	성공.
	EMINVEN_COSTUM_COLOR_CHANGE_FB_NOITEM	= 2,	//	아이템이 없음.
	EMINVEN_COSTUM_COLOR_CHANGE_FB_BADITEM	= 3,	//	사용 불가능 아이템.
	EMINVEN_COSTUM_COLOR_CHANGE_FB_BADCLASS	= 4,	//	케릭터 직업이 틀려서 사용 불가능 아이템.
	EMINVEN_COSTUM_COLOR_CHANGE_FB_COOLTIME = 5,	//	쿨타임 실패~
	EMINVEN_COSTUM_COLOR_CHANGE_FB_RESTRICT = 6,	//	기능 제한의 한계를 넘어선 오류.
};

enum EMCOSTUM_COLOR_RESTRICT
{
	RESTRICT_MIN = 100,
	RESTRICT_MAX = 180,
};

enum EMINVEN_USE_ITEM
{
	EMINVEN_USE_ITEM_FAIL = 0,	//	일반오류.
	EMINVEN_USE_ITEM_OK,		//	성공.
	EMINVEN_USE_ITEM_NOITEM,	//	아이템이 없음.
	EMINVEN_USE_ITEM_BADITEM,	//	사용 불가능 아이템.
	EMINVEN_USE_ITEM_BADCLASS,	//	케릭터 직업이 틀려서 사용 불가능 아이템.
	EMINVEN_USE_ITEM_BADLEVEL,	//  레벨이 낮아서 사용불가
	EMINVEN_USE_ITEM_COSTUMEBADLEVEL, // 코스 튬 레벨이 맞지않아서 사용불가;
};

enum EMINVEN_VEHICLE_CHANGE_FB
{
	EMINVEN_VEHICLE_CHANGE_FB_FAIL		= 0,	//	일반오류.
	EMINVEN_VEHICLE_CHANGE_FB_OK		= 1,	//	성공.
	EMINVEN_VEHICLE_CHANGE_FB_NOITEM	= 2,	//	아이템이 없음.
	EMINVEN_VEHICLE_CHANGE_FB_BADITEM	= 3,	//	사용 불가능 아이템.
	EMINVEN_VEHICLE_CHANGE_FB_BADCLASS	= 4,	//	케릭터 직업이 틀려서 사용 불가능 아이템.
	EMINVEN_VEHICLE_CHANGE_FB_COOLTIME = 5,	//	쿨타임 실패~
};

enum EMINVEN_LEARN_BIKE_BOOST_FB
{
	EMINVEN_LEARN_BIKE_BOOST_FB_FAIL		= 0,	//	일반오류.
	EMINVEN_LEARN_BIKE_BOOST_FB_OK		= 1,	//	성공.
	EMINVEN_LEARN_BIKE_BOOST_FB_NOITEM	= 2,	//	아이템이 없음.
	EMINVEN_LEARN_BIKE_BOOST_FB_BADITEM	= 3,	//	사용 불가능 아이템.
	EMINVEN_LEARN_BIKE_BOOST_FB_BADCLASS	= 4,	//	케릭터 직업이 틀려서 사용 불가능 아이템.
	EMINVEN_LEARN_BIKE_BOOST_FB_COOLTIME = 5,	//	쿨타임 실패~
};

enum EMINVEN_USE_BIKE_BOOST_FB
{
	EMINVEN_USE_BIKE_BOOST_FB_FAIL			= 0,	//	일반오류.
	EMINVEN_USE_BIKE_BOOST_FB_OK			= 1,	//	성공.
	EMINVEN_USE_BIKE_BOOST_FB_NOITEM		= 2,	//	아이템이 없음.
	EMINVEN_USE_BIKE_BOOST_FB_NOBIKE		= 3,	//	바이크가아님
	EMINVEN_USE_BIKE_BOOST_FB_NODRIVER		= 4,	//  운전자상태 아님
	EMINVEN_USE_BIKE_BOOST_FB_NOT_BOOSTERID	= 5,	//  부스터배우지않음
	EMINVEN_USE_BIKE_BOOST_FB_DELAY			= 6,	//  딜레이걸림
	EMINVEN_USE_BIKE_BOOST_FB_USING			= 7,	//  부스터사용중
	EMINVEN_USE_BIKE_BOOST_FB_NOTFULL		= 8,	//	기름 없음
};

enum EMINVEN_USE_BIKE_BOOST_CANCEL_FB
{
	EMINVEN_USE_BIKE_BOOST_CANCEL = 0,
	EMINVEN_USE_BIKE_BOOST_DELAY_CANCEL = 1,
};


enum EMINVEN_FACE_CHANGE_FB
{
	EMINVEN_FACE_CHANGE_FB_FAIL		= 0,	//	일반오류.
	EMINVEN_FACE_CHANGE_FB_OK		= 1,	//	성공.
	EMINVEN_FACE_CHANGE_FB_NOITEM	= 2,	//	아이템이 없음.
	EMINVEN_FACE_CHANGE_FB_BADITEM	= 3,	//	사용 불가능 아이템.
	EMINVEN_FACE_CHANGE_FB_BADCLASS	= 4,	//	케릭터 직업이 틀려서 사용 불가능 아이템.
	EMINVEN_FACE_CHANGE_FB_COOLTIME	= 5,	//	쿨타임 실패
};

enum EMINVEN_GENDER_CHANGE_FB
{
	EMINVEN_GENDER_CHANGE_FB_FAIL		= 0,	//	일반오류.
	EMINVEN_GENDER_CHANGE_FB_OK			= 1,	//	성공.
	EMINVEN_GENDER_CHANGE_FB_NOITEM		= 2,	//	아이템이 없음.
	EMINVEN_GENDER_CHANGE_FB_ITEMTYPE	= 3,	//	사용 불가능 아이템.
	EMINVEN_GENDER_CHANGE_FB_BADCLASS	= 4,	//	케릭터 직업이 틀려서 사용 불가능 아이템.
	EMINVEN_GENDER_CHANGE_FB_NOTVALUE	= 5,	//  잘못된 데이터 
};

enum EMINVEN_SCHOOL_CHANGE_FB
{
	EMINVEN_SCHOOL_CHANGE_FB_FAIL		= 0,	//	일반오류.
	EMINVEN_SCHOOL_CHANGE_FB_OK			= 1,	//	성공.
	EMINVEN_SCHOOL_CHANGE_FB_NOITEM		= 2,	//	아이템이 없음.
	EMINVEN_SCHOOL_CHANGE_FB_ITEMTYPE	= 3,	//	사용 불가능 아이템.
	EMINVEN_SCHOOL_CHANGE_FB_MAP_ERROR	= 4,	//  사용 불가능한 맵
	EMINVEN_SCHOOL_CHANGE_FB_NOTVALUE	= 5,	//  잘못된 데이터 
	EMINVEN_SCHOOL_CHANGE_FB_SCHOOL_ERROR = 6,	// 동일 학원 변경
	EMINVEN_SCHOOL_CHANGE_FB_CLUB_ERROR = 7, // 클럽 가입상태에서는 못함
};

enum EMINVEN_EXP_COMPRESSOR_FB
{
	EMINVEN_EXP_COMPRESSOR_FAIL		= 0,	//	일반오류.
	EMINVEN_EXP_COMPRESSOR_OK			= 1,	//	성공.
	EMINVEN_EXP_COMPRESSOR_NOTEXIST	= 2,	//	아이템이 없음.
	EMINVEN_EXP_COMPRESSOR_ITEMTYPE	= 3,	//	사용 불가능 아이템.
	EMINVEN_EXP_COMPRESSOR_NOTENOUGH = 4,
	EMINVEN_EXP_COMPRESSOR_NOTVALUE	= 5,	//  잘못된 데이터 	
};

enum EMINVEN_EXP_CAPSULE_FB
{
	EMINVEN_EXP_CAPSULE_FAIL		= 0,	//	일반오류.
	EMINVEN_EXP_CAPSULE_OK			= 1,	//	성공.
	EMINVEN_EXP_CAPSULE_NOTEXIST	= 2,	//	아이템이 없음.
	EMINVEN_EXP_CAPSULE_ITEMTYPE	= 3,	//	사용 불가능 아이템.
	EMINVEN_EXP_CAPSULE_NOTVALUE	= 4,	//  잘못된 데이터 	
};

enum EMINVEN_EXP_CARD_FB
{
	EMINVEN_EXP_FB_FAIL		= 0,	//	일반오류.
	EMINVEN_EXPE_FB_OK		= 1,	//	성공.
	EMINVEN_EXP_FB_NOITEM	= 2,	//	아이템이 없음.
	EMINVEN_EXP_FB_ITEMTYPE	= 3,	//	사용 불가능 아이템.
	EMINVEN_EXP_FB_NOTVALUE	= 4,	//  잘못된 데이터 	
};

enum EMINVEN_RENAME_FB
{
	EMINVEN_RENAME_FB_FAIL		= 0,	//	일반오류.
	EMINVEN_RENAME_FB_OK		= 1,	//	성공.
	EMINVEN_RENAME_FB_NOITEM	= 2,	//	아이템이 없음.
	EMINVEN_RENAME_FB_BADITEM	= 3,	//	사용 불가능 아이템.
	EMINVEN_RENAME_FB_LENGTH	= 4,	//	이름의 길이가 4자 미만
	EMINVEN_RENAME_FROM_DB_OK	= 5,	//	성공.
	EMINVEN_RENAME_FROM_DB_FAIL	= 6,	//	실패.(이름 중복)
	EMINVEN_RENAME_FROM_DB_BAD	= 7,	//	실패.(이름 불가)
	EMINVEN_RENAME_FB_THAICHAR_ERROR = 8,	// 태국어 문자 조합 에러
	EMINVEN_RENAME_FB_VNCHAR_ERROR = 9,  // 베트남 문자 조합 에러
};

enum EMSMS_PHONE_NUMBER_FB
{
	EMSMS_PHONE_NUMBER_FB_FAIL		= 0,	//	일반오류.
	EMSMS_PHONE_NUMBER_FB_OK		= 1,	//	성공.
	EMSMS_PHONE_NUMBER_FROM_DB_OK	= 2,	//	성공.
	EMSMS_PHONE_NUMBER_FROM_DB_FAIL	= 3,	//	실패.(이름 중복)
};

enum EMSMS_SEND_FB
{
	EMSMS_SEND_FB_FAIL		= 0,	//	일반오류.
	EMSMS_SEND_FB_OK		= 1,	//	성공.
	EMSMS_SEND_FB_NOITEM	= 2,	//	아이템이 없음.
	EMSMS_SEND_FB_BADITEM	= 3,	//	사용 불가능 아이템.
	EMSMS_SEND_FROM_DB_OK	= 4,	//	성공.
	EMSMS_SEND_FROM_DB_FAIL	= 5,	//	실패.
};

enum EMMGAME_ODDEVEN
{
	EMMGAME_ODDEVEN_OK			= 0,	// 배팅 확인
	EMMGAME_ODDEVEN_CANCEL		= 1,	// 취소
	EMMGAME_ODDEVEN_SELECT		= 2,	// 홀,짝
	EMMGAME_ODDEVEN_AGAIN_OK	= 3,	// 반복 확인
	EMMGAME_ODDEVEN_SHUFFLE		= 4,
	EMMGAME_ODDEVEN_FINISH		= 5,
};

enum EMMGAME_ODDEVEN_FB
{
	EMMGAME_ODDEVEN_FB_OK			= 0,	// 배팅 완료 게임 시작
	EMMGAME_ODDEVEN_FB_FAIL			= 1,	// 게임 취소
	EMMGAME_ODDEVEN_FB_MONEY_FAIL	= 2,	// 소유금액 부족
	EMMGAME_ODDEVEN_FB_MAXBATTING	= 3,	// 최대 배팅가능 금액 초과
	EMMGAME_ODDEVEN_FB_MAXROUND		= 4,	// 최대 반복 횟수 도달
	EMMGAME_ODDEVEN_FB_WIN			= 5,	// 
	EMMGAME_ODDEVEN_FB_LOSE			= 6,	// 
	EMMGAME_ODDEVEN_FB_AGAIN_OK		= 7,	// 반복 허락
	EMMGAME_ODDEVEN_FB_FINISH_OK	= 8,
};

enum EMMGAME_ODDEVEN_CASE
{
	EMMGAME_ODDEVEN_EVEN			= 0,	// 짝
	EMMGAME_ODDEVEN_ODD				= 1,	// 홀
};

enum EM2FRIEND_FB
{
	EM2FRIEND_FB_FAIL				= 0,	//	일반 오류.
	EM2FRIEND_FB_OK					= 1,	//	성공.
	EM2FRIEND_FB_NO_ITEM			= 2,	//	아이템이 없음.
	EM2FRIEND_FB_FRIEND_CONDITION	= 3,	//	친구의 상태 이상.
	EM2FRIEND_FB_MY_CONDITION		= 4,	//	자신의 상태 이상.
	EM2FRIEND_FB_PK_CONDITION		= 5,	//	자신의 상태 이상.
	EM2FRIEND_FB_MAP_CONDITION		= 6,	//	맵 진입 조건.
	EM2FRIEND_FB_FRIEND_CHANNEL		= 7,	//	친구와 채널이 틀림.
	EM2FRIEND_FB_IMMOVABLE			= 8,	//  진입불가 ( 이벤트 지역 )
	EM2FRIEND_FB_FRIEND_BLOCK		= 9,	//	친구가 나를 차단.
	EM2FRIEND_FB_FRIEND_OFF			= 10,	//	친구가 나를 삭제.
	EM2FRIEND_FB_FRIEND_INSTANTMAP	= 11,	//	친구가 인스턴트 맵에 있음
};

enum EMGM_MOVE2CHAR_FB
{
	EMGM_MOVE2CHAR_FB_FAIL				= 0,	//	일반 오류.
	EMGM_MOVE2CHAR_FB_OK				= 1,	//	성공.
	EMGM_MOVE2CHAR_FB_TO_CONDITION		= 2,	//	상대의 상태 이상.
	EMGM_MOVE2CHAR_FB_MY_CONDITION		= 3,	//	자신의 상태 이상.
	EMGM_MOVE2CHAR_FB_CHANNEL			= 4,	//	채널이 틀림.
	EMGM_MOVE2CHAR_FB_TO_INSTANTMAP		= 5,	//	상대가 인던 맵안에 있음
};

enum EMVEHICLE_SET_FB
{

	EMVEHICLE_SET_FB_FAIL			= 0,	// 일반오류
	EMVEHICLE_SET_FB_OK				= 1,	// 성공
	EMVEHICLE_SET_FB_NOTENOUGH_OIL	= 2,	// 기름없음
	EMVEHICLE_SET_FB_MAP_FAIL		= 3,	// 비활성화 맵
	EMVEHICLE_SET_FB_NO_ITEM		= 4,	// 탈것 아이템 찾을수 없음
	EMVEHICLE_SET_FB_RESET			= 5,	// 이미 타거나 내렸을때...
	EMVEHICLE_SET_FB_BATTLING		= 6,	// 전투중..
};

enum EMVEHICLE_BROAD
{
	EMVEHICLE_BROAD_OK		  = 0,
	EMVEHICLE_BROAD_ITEM_INFO = 1,
};

enum EMVEHICLE_GET_FB
{
	EMVEHICLE_GET_FB_OK			   = 0,		// 성공
	EMVEHICLE_GET_FB_FAIL		   = 1,		// 일반적인 오류
	EMVEHICLE_GET_FB_INVALIDITEM   = 2,		// 아이템이 정보가 다름
	EMVEHICLE_GET_FB_NOITEM		   = 3,		// 아이템이 없음
	EMVEHICLE_GET_FB_NODATA		   = 4,		// DB에 정보 없음
	EMVEHICLE_GET_FB_ITEM_INFO     = 5,
};

enum EMVEHICLE_REQ_SLOT_EX_HOLD_FB
{
	EMVEHICLE_REQ_SLOT_EX_HOLD_FB_FAIL		  = 0,		// 일반오류.
	EMVEHICLE_REQ_SLOT_EX_HOLD_FB_OK		  = 1,		// 아이템 교체 성공
	EMVEHICLE_REQ_SLOT_EX_HOLD_FB_NOMATCH	  = 2,		// 타입이 맞지 않는 아이템
	EMVEHICLE_REQ_SLOT_EX_HOLD_FB_INVALIDITEM = 3,		// 탈것용이 아니다.
};


enum EMVEHICLE_REQ_GIVE_BATTERY_FB
{
	EMVEHICLE_REQ_GIVE_BATTERY_FB_OK				= 0,	// 성공
	EMVEHICLE_REQ_GIVE_BATTERY_FB_FAIL				= 1,	// 일반오류
	EMVEHICLE_REQ_GIVE_BATTERY_FB_INVALIDBATTERY	= 2,	// 배터리 아님
	EMVEHICLE_REQ_GIVE_BATTERY_FB_INVALIDITEM		= 3,	// 탈것 아님
	EMVEHICLE_REQ_GET_BATTERY_FROMDB_OK				= 4,	// 디비에서 저장
	EMVEHICLE_REQ_GET_BATTERY_FROMDB_FAIL			= 5,	// 실패
	EMVEHICLE_REQ_GIVE_BATTERY_FB_DIFFERENT			= 6,	// 배터리 종류 다름
};

enum EMWITH_VEHICLE_ANS
{
	EMWITH_VEHICLE_FB_OK				= 0,
	EMWITH_VEHICLE_CANCEL				= 1,
	EMWITH_VEHICLE_CANCEL_DIST			= 2,
	EMWITH_VEHICLE_CANCEL_MY_DIE		= 3,
	EMWITH_VEHICLE_CANCEL_TAR_DIE		= 4,
	EMWITH_VEHICLE_CANCEL_MY_DIE_ANS	= 5,
	EMWITH_VEHICLE_CANCEL_TAR_DIE_ANS	= 6,
	EMWITH_VEHICLE_CANCEL_CONFT			= 7,
	EMWITH_VEHICLE_CANCEL_PMARKET		= 8,
};

enum EMWITH_VEHICLE_CANCEL_TAR
{
	EMWITH_VEHICLE_CANCEL_NORMAL	= 0,	//	일반적인 취소.
	EMWITH_VEHICLE_CANCEL_OWNER		= 1,	//	대장이 종료
};

enum EMREQ_ATTEND_FB
{
	EMREQ_ATTEND_FB_OK			= 0, // 성공
	EMREQ_ATTEND_FB_FAIL		= 1, // 실패
	EMREQ_ATTEND_FB_ALREADY		= 2, // 실패( 이미 했음 ) 
	EMREQ_ATTEND_FB_ATTENTIME	= 3, //	접속시간 체크
	EMREQ_ATTEND_FB_NOTUSE		= 4, //	이벤트 중이 아님
};

enum EMREQ_GATHER_FB
{
	EMREQ_GATHER_FB_OK			= 0,	//	채집요청 성공
	EMREQ_GATHER_FB_FAIL		= 1,	//	채집요청 실패
	EMREQ_GATHER_FB_DISTANCE	= 2,	//	채집 거리 
	EMREQ_GATHER_FB_NOTTYPE		= 3,	//	Crow Type
	EMREQ_GATHER_FB_NOCROW		= 4,	//	Crow 없음
	EMREQ_GATHER_FB_USE			= 5,	//	다른사람이 사용중
	EMREQ_GATHER_FB_GATHERING	= 6,	//	이미 채집중임
};

enum EMREQ_GATHER_RESULT
{
	EMREQ_GATHER_RESULT_SUCCESS		= 0,	// 채집 성공
	EMREQ_GATHER_RESULT_SUCCESS_EX	= 1,	// 성공이나 아이템 루팅 실패
	EMREQ_GATHER_RESULT_FAIL		= 2,	// 채집 실패
	EMREQ_GATHER_RESULT_ITEMFAIL	= 3,	// 아이템 획득 실패
};

enum EMREQ_NPCTALK_FB
{
	EMREQ_NPCTALK_FB_FAIL		= 0,	// 실패
	EMREQ_NPCTALK_FB_OK		= 1,	// 성공
	EMREQ_NPCTALK_FB_NPC		= 2,	// NPC없음
	EMREQ_NPCTALK_FB_TALK		= 3,	// 해당 대화문 없음
	EMREQ_NPCTALK_FB_TYPE		= 4,	// 해당 기능 없음
	EMREQ_NPCTALK_FB_DISTANCE	= 5,	// 거리가 멈
};

enum EMDOMINATE_MOVETO_FB
{
	EMDOMINATE_MOVETO_FB_FAIL			= 0, // 실패
	EMDOMINATE_MOVETO_FB_NA_TARGET = 1, // 타겟 사라짐
	EMDOMINATE_MOVETO_FB_OUT_OF_DISTANCE = 2, // 제한 거리 초과;
	EMDOMINATE_MOVETO_FB_NOT_DOMINATED = 3, // 외부적인 요인으로 지배 풀림;
	EMDOMINATE_MOVETO_FB_SUCCESS	= 4,	// 성공;
	EMDOMINATE_MOVETO_FB_CANCEL		= 5,	// 피지배자가 받는 실패 메세지;
};

enum EMVEHICLE_CREATE_FROMDB_FB
{
    EMVEHICLE_CREATE_FROMDB_FB_FAIL    = 0,
    EMVEHICLE_CREATE_FROMDB_FB_OK      = 1,
};

enum EMVEHICLE_GET_FROMDB_ERROR_FB
{
    EMVEHICLE_GET_FROMDB_ERROR_FB_DBFAIL = 0,
    EMVEHICLE_GET_FROMDB_ERROR_FB_NODATA = 1,
};

enum EMSCHOOL_CHANGE
{
	EMSCHOOL_CHANGE_ERROR = -2,
	EMSCHOOL_CHANGE_NOT_EXIST = -1,
	EMSCHOOL_CHANGE_EXIST = 0,
	EMSCHOOL_CHANGE_DISGARD = 65535,
};

enum EMUSERCOUNT_LAND
{
	EMUSERCOUNT_LAND_FAIL = 0,
	EMUSERCOUNT_LAND_SUCCESS = 1,
	EMUSERCOUNT_LAND_NOT_EXIST = 2,
};

enum EMCHARSLOT_LOCK_FLAG
{
	EMCHARSLOT_LOCK_FAIL = 0,
	EMCHARSLOT_LOCK_SUCCESS = 1,
	EMCHARSLOT_LOCK_ALREADY = 2,
};

enum EMCHARSLOT_UNLOCK_FLAG
{
	EMCHARSLOT_UNLOCK_FAIL = 0,
	EMCHARSLOT_UNLOCK_SUCCESS = 1,
	EMCHARSLOT_UNLOCK_ALREADY = 2,
};

enum EMCHARSLOT_CHANGE_FLAG
{
	EMCHARSLOT_CHANGE_FAIL = 0,
	EMCHARSLOT_CHANGE_SUCCESS = 1,
	EMCHARSLOT_CHANGE_LOCK = 2,
	EMCHARSLOT_CHANGE_COOLTIME = 3,
	EMCHARSLOT_CHANGE_DIE = 4,
	EMCHARSLOT_CHANGE_ACTION = 5,
	EMCHARSLOT_CHANGE_SAMESLOT = 6,
	EMCHARSLOT_CHANGE_LIMITMAP = 7,
	EMCHARSLOT_CHANGE_INVENLACK = 8,
};

namespace GLMSG
{
	#pragma pack(1)

	//---------------------------------------------------------------------------NET
	struct SNETPC_GOTO : public NET_MSG_GENERIC
	{
		DWORD				dwGaeaID;
		DWORD				dwActState;
		D3DXVECTOR3			vCurPos;
		D3DXVECTOR3			vTarPos;

		SNETPC_GOTO()
            : dwGaeaID(GAEAID_NULL)
			, dwActState(NULL)
            , vCurPos(0,0,0)
            , vTarPos(0,0,0)
		{
			dwSize = sizeof(SNETPC_GOTO);
			nType = NET_MSG_GCTRL_GOTO;
			MIN_STATIC_ASSERT(sizeof(SNETPC_GOTO)<=NET_DATA_BUFSIZE);
		}

        bool operator != (CONST SNETPC_GOTO& rhs) const
        {
            if (dwActState != rhs.dwActState || vCurPos != rhs.vCurPos || vTarPos != rhs.vTarPos)
                return true;
            else
                return false;
        }
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_GOTO_BRD : public SNETPC_BROAD
	{
		DWORD				dwActState;
		D3DXVECTOR3			vCurPos;
		D3DXVECTOR3			vTarPos;

		float				fDelay;

		SNETPC_GOTO_BRD () :
			dwActState(NULL),
			vCurPos(0,0,0),
			vTarPos(0,0,0),

			fDelay(0)
		{
			dwSize = sizeof(SNETPC_GOTO_BRD);
			nType = NET_MSG_GCTRL_GOTO_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_GOTO_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_ACTSTATE : public NET_MSG_UNIQUEKEY//NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		DWORD dwActState;

		SNETPC_ACTSTATE(DWORD State=0)
            : dwActState(State)
		{
			dwSize = sizeof(SNETPC_ACTSTATE);
			nType = NET_MSG_GCTRL_ACTSTATE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_ACTSTATE)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_MOVESTATE : public NET_MSG_GENERIC
	{
		DWORD				dwActState;

		SNETPC_MOVESTATE () :
			dwActState(0)
		{
			dwSize = sizeof(SNETPC_MOVESTATE);
			nType = NET_MSG_GCTRL_MOVESTATE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_MOVESTATE)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET

	struct SNETPC_MOVESTATE_BRD : public SNETPC_BROAD
	{
		DWORD dwActState;

		SNETPC_MOVESTATE_BRD(DWORD GaeaId, DWORD State)
            : SNETPC_BROAD(GaeaId)
			, dwActState(State)
		{
			dwSize = sizeof(SNETPC_MOVESTATE_BRD);
			nType = NET_MSG_GCTRL_MOVESTATE_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_MOVESTATE_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_UPDATE_FLAGS_BRD : public SNETPC_BROAD
	{
		DWORD				dwFLAGS;

		SNETPC_UPDATE_FLAGS_BRD () :
			dwFLAGS(NULL)
		{
			dwSize = sizeof(SNETPC_UPDATE_FLAGS_BRD);
			nType = NET_MSG_GCTRL_UPDATE_FLAGS;
			MIN_STATIC_ASSERT(sizeof(SNETPC_UPDATE_FLAGS_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_JUMP_POS_BRD : public SNETPC_BROAD
	{
		D3DXVECTOR3		vPOS;
		DWORD			dwFlags;

		SNETPC_JUMP_POS_BRD( DWORD GaeId, const D3DXVECTOR3& Pos, DWORD _dwFlags ) :
			SNETPC_BROAD	( GaeId ),
            vPOS			( Pos ),
			dwFlags			( _dwFlags )
		{
			dwSize = sizeof(SNETPC_JUMP_POS_BRD);
			nType = NET_MSG_GCTRL_JUMP_POS_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_JUMP_POS_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_ANIMOVE_START_CF : public NET_MSG_GENERIC
	{
		float		m_fAngleY;		// Y축의 회전 값 < 바라보는 방향 >

		SNETPC_ANIMOVE_START_CF()
			: m_fAngleY(0.f)
		{
			dwSize = sizeof(SNETPC_ANIMOVE_START_CF);
			nType = NET_MSG_GCTRL_ANIMOVE_START_CF;
			MIN_STATIC_ASSERT( sizeof(SNETPC_ANIMOVE_START_CF) <= NET_DATA_BUFSIZE );
		}
	};

	struct SNETPC_ANIMOVE_END_CF : public NET_MSG_GENERIC
	{
		D3DXVECTOR3	m_vCurPos;		// AnimationMove로 이동된 자신의 마지막 위치를 서버로 전송한다.

		SNETPC_ANIMOVE_END_CF()
			: m_vCurPos(0,0,0)
		{
			dwSize = sizeof(SNETPC_ANIMOVE_END_CF);
			nType = NET_MSG_GCTRL_ANIMOVE_END_CF;
			MIN_STATIC_ASSERT( sizeof(SNETPC_ANIMOVE_END_CF) <= NET_DATA_BUFSIZE );
		}
	};

	struct SNETPC_ANIMOVE_START_BRD : public SNETPC_BROAD
	{
		float		m_fAngleY;		// Y축의 회전 값 < 바라보는 방향 >

		SNETPC_ANIMOVE_START_BRD()
			: m_fAngleY(0.f)
		{
			dwSize = sizeof(SNETPC_ANIMOVE_START_BRD);
			nType = NET_MSG_GCTRL_ANIMOVE_START_BRD;
			MIN_STATIC_ASSERT( sizeof(SNETPC_ANIMOVE_START_BRD) <= NET_DATA_BUFSIZE );
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_ATTACK : public NET_MSG_GENERIC
	{
		EMCROW				emTarCrow;
		DWORD				dwTarID;
		DWORD				dwAniSel;
		DWORD				dwFlags;

		SNETPC_ATTACK () :
			emTarCrow(CROW_MOB),
			dwTarID(0),
			dwAniSel(0),
			dwFlags(NULL)
		{
			dwSize = sizeof(SNETPC_ATTACK);
			nType = NET_MSG_GCTRL_ATTACK;
			MIN_STATIC_ASSERT(sizeof(SNETPC_ATTACK)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_ATTACK_BRD : public SNETPC_BROAD
	{
		EMCROW				emTarCrow;
		DWORD				dwTarID;
		DWORD				dwAniSel;

		SNETPC_ATTACK_BRD () :
			emTarCrow(CROW_MOB),
			dwTarID(0),
			dwAniSel(0)
		{
			dwSize = sizeof(SNETPC_ATTACK_BRD);
			nType = NET_MSG_GCTRL_ATTACK_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_ATTACK_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_SELECT_TARGET : public NET_MSG_GENERIC
	{
		EMCROW				emTarCrow;
		DWORD				dwTarID;

		SNETPC_SELECT_TARGET() :
			emTarCrow( CROW_PC ),
			dwTarID( 0 )
		{
			dwSize	= sizeof( SNETPC_SELECT_TARGET );
			nType	= NET_MSG_GCTRL_SELECT_TARGET;
			MIN_STATIC_ASSERT( sizeof( SNETPC_SELECT_TARGET ) <= NET_DATA_BUFSIZE );
		}
	}; //struct SNETPC_SELECT_TARGET

	//---------------------------------------------------------------------------NET
	struct SNETPC_SKILL_DELAY : public NET_MSG_GENERIC
	{
		float fSkillDelay;
		SNATIVEID sUseSkillID;


		SNETPC_SKILL_DELAY()
		{
			dwSize	= sizeof( SNETPC_SKILL_DELAY );
			nType	= NET_MSG_GCTRL_SKILLDELAY;
			MIN_STATIC_ASSERT( sizeof( SNETPC_SKILL_DELAY ) <= NET_DATA_BUFSIZE );
		}
	}; //struct SNETPC_SELECT_TARGET


	//---------------------------------------------------------------------------NET
	struct SNETPC_ATTACK_CANCEL : public NET_MSG_GENERIC
	{		
		SNETPC_ATTACK_CANCEL ()
		{
			dwSize = sizeof(SNETPC_ATTACK_CANCEL);
			nType = NET_MSG_GCTRL_ATTACK_CANCEL;
			MIN_STATIC_ASSERT(sizeof(SNETPC_ATTACK_CANCEL)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_ATTACK_CANCEL_BRD : public SNETPC_BROAD
	{
		SNETPC_ATTACK_CANCEL_BRD ()
		{
			dwSize = sizeof(SNETPC_ATTACK_CANCEL_BRD);
			nType = NET_MSG_GCTRL_ATTACK_CANCEL_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_ATTACK_CANCEL_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_ATTACK_BLOCKING : public NET_MSG_GENERIC
	{		
		EMCROW				emTarCrow;
		DWORD				dwTarID;
		WORD				wSkillLevel;
		SNATIVEID			SkillID;

		SNETPC_ATTACK_BLOCKING () 
			: emTarCrow(CROW_MOB)
			, dwTarID(0)
			, SkillID(NATIVEID_NULL())
		{
			dwSize = sizeof(SNETPC_ATTACK_BLOCKING);
			nType = NET_MSG_GCTRL_ATTACK_BLOCKING;
			MIN_STATIC_ASSERT(sizeof(SNETPC_ATTACK_BLOCKING)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_ATTACK_BLOCKING_BRD : public SNETPC_BROAD
	{
		EMCROW				emTarCrow;
		DWORD				dwTarID;
		WORD				wSkillLevel;
		SNATIVEID			SkillID;

		SNETPC_ATTACK_BLOCKING_BRD () 
			: emTarCrow(CROW_MOB)
			, dwTarID(0)
			, SkillID(NATIVEID_NULL())
		{
			dwSize = sizeof(SNETPC_ATTACK_BLOCKING_BRD);
			nType = NET_MSG_GCTRL_ATTACK_BLOCKING_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_ATTACK_BLOCKING_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_ATTACK_AVOID : public NET_MSG_GENERIC
	{
		EMCROW				emTarCrow;
		DWORD				dwTarID;
		WORD				wSkillLevel;
		SNATIVEID			SkillID;

		SNETPC_ATTACK_AVOID ()
			:emTarCrow(CROW_MOB)
			, dwTarID(0)
			, SkillID(NATIVEID_NULL())
		{
			dwSize = sizeof(SNETPC_ATTACK_AVOID);
			nType = NET_MSG_GCTRL_ATTACK_AVOID;
			MIN_STATIC_ASSERT(sizeof(SNETPC_ATTACK_AVOID)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_ATTACK_AVOID_BRD : public SNETPC_BROAD
	{
		EMCROW				emTarCrow;
		DWORD				dwTarID;
		WORD				wSkillLevel;
		SNATIVEID			SkillID;

		SNETPC_ATTACK_AVOID_BRD () 
			: emTarCrow(CROW_MOB)
			, dwTarID(0)
			, SkillID(NATIVEID_NULL())
		{
			dwSize = sizeof(SNETPC_ATTACK_AVOID_BRD);
			nType = NET_MSG_GCTRL_ATTACK_AVOID_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_ATTACK_AVOID_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_ATTACK_DAMAGE : public NET_MSG_GENERIC
	{
		EMCROW				emTarCrow;
		DWORD				dwTarID;
		int						nDamage;
		DWORD				dwDamageFlag;

        WORD                wSchool;    //mjeon.CaptureTheField

		SNETPC_ATTACK_DAMAGE () :
			emTarCrow(CROW_MOB),
			dwTarID(0),
			nDamage(0),
			dwDamageFlag( DAMAGE_TYPE_NONE ),
            wSchool(SCHOOL_NONE)
		{
			dwSize = sizeof(SNETPC_ATTACK_DAMAGE);
			nType = NET_MSG_GCTRL_ATTACK_DAMAGE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_ATTACK_DAMAGE)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_ATTACK_HEAL : public NET_MSG_GENERIC
	{
		EMCROW				emTarCrow;
		DWORD				dwTarID;
		int						nHeal;
		DWORD				dwHealFlag;

		WORD                wSchool;    //mjeon.CaptureTheField

		SNETPC_ATTACK_HEAL () :
		emTarCrow(CROW_MOB),
			dwTarID(0),
			nHeal(0),
			dwHealFlag( HEAL_TYPE_NONE ),
			wSchool(SCHOOL_NONE)
		{
			dwSize = sizeof(SNETPC_ATTACK_HEAL);
			nType = NET_MSG_GCTRL_ATTACK_HEAL;
			MIN_STATIC_ASSERT(sizeof(SNETPC_ATTACK_HEAL)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_DEFENSE_SKILL_ACTIVE : public NET_MSG_GENERIC
	{
		

		EMCROW				emTarCrow;
		DWORD				dwTarID;
		SNATIVEID			sNativeID;
		WORD				wLevel;

		SNETPC_DEFENSE_SKILL_ACTIVE () :
			emTarCrow(CROW_MOB),
			dwTarID(0),
			sNativeID(NATIVEID_NULL()),
			wLevel(0)
		{
			dwSize = sizeof(SNETPC_DEFENSE_SKILL_ACTIVE);
			nType = NET_MSG_GCTRL_DEFENSE_SKILL_ACTIVE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_DEFENSE_SKILL_ACTIVE)<=NET_DATA_BUFSIZE);
		}
	};
	

	//---------------------------------------------------------------------------NET
	struct SNETPC_ATTACK_DAMAGE_BRD : public SNETPC_BROAD
	{
		EMCROW		emTarCrow;
		DWORD		dwTarID;
		int				nDamage;
		DWORD		dwDamageFlag;
        WORD		wSchool;
		SNATIVEID	SkillID;
		WORD		wSkillLevel;

		SNETPC_ATTACK_DAMAGE_BRD () :
			emTarCrow(CROW_MOB),
			dwTarID(0),
			nDamage(0),
			dwDamageFlag(DAMAGE_TYPE_NONE),
            wSchool(SCHOOL_NONE),
			SkillID(NATIVEID_NULL())
		{
			dwSize = sizeof(SNETPC_ATTACK_DAMAGE_BRD);
			nType = NET_MSG_GCTRL_ATTACK_DAMAGE_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_ATTACK_DAMAGE_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_ATTACK_HEAL_BRD : public SNETPC_BROAD
	{
		EMCROW				emTarCrow;
		DWORD				dwTarID;
		int						nHeal;
		DWORD				dwHealFlag;
		WORD                wSchool;

		SNETPC_ATTACK_HEAL_BRD () :
		emTarCrow(CROW_MOB),
			dwTarID(0),
			nHeal(0),
			dwHealFlag(HEAL_TYPE_NONE),
			wSchool(SCHOOL_NONE)
		{
			dwSize = sizeof(SNETPC_ATTACK_HEAL_BRD);
			nType = NET_MSG_GCTRL_ATTACK_HEAL_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_ATTACK_HEAL_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_ACTION_BRD : public SNETCROW_BROAD
	{
        EMCROW		        emCrowAssault;  // 나를 죽인 타입
        DWORD		        dwGaeaIdAssault;       // 나를 죽인 대상 ID

		EMACTIONTYPE		emAction;
		DWORD				dwFLAG;

		SNET_ACTION_BRD () :
			emAction(GLAT_IDLE),
			dwFLAG(NULL)
		{
			dwSize = sizeof(SNET_ACTION_BRD);
			nType = NET_MSG_GCTRL_ACTION_BRD;
			MIN_STATIC_ASSERT(sizeof(SNET_ACTION_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_UPDATE_HIRE_SUMMON_STATE : public NET_MSG_GENERIC
	{
		DWORD dwHireSummonGaeaID;
		SNATIVEID sBasicSummonID;

		SNETPC_UPDATE_HIRE_SUMMON_STATE()
			: dwHireSummonGaeaID( 0 )
			, sBasicSummonID( SNATIVEID( false ) )
		{
			dwSize = sizeof(SNETPC_UPDATE_HIRE_SUMMON_STATE);
			nType = NET_MSG_GCTRL_UPDATE_HIRE_SUMMON_STATE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_UPDATE_HIRE_SUMMON_STATE)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_UPDATE_STATE : public NET_MSG_GENERIC
	{		
		GLDWDATA sHP;
		GLDWDATA sMP;
		GLDWDATA sSP;

		void ENCODE(DWORD dwKEY);
		void DECODE(DWORD dwKEY);

		SNETPC_UPDATE_STATE()
		{
			dwSize = sizeof(SNETPC_UPDATE_STATE);
			nType = NET_MSG_GCTRL_UPDATE_STATE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_UPDATE_STATE)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_UPDATE_STATE_BRD : public SNETPC_BROAD
	{
		GLDWDATA			sHP;

		SNETPC_UPDATE_STATE_BRD ()
		{
			dwSize = sizeof(SNETPC_UPDATE_STATE_BRD);
			nType = NET_MSG_GCTRL_UPDATE_STATE_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_UPDATE_STATE_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_UPDATE_PASSIVE_BRD : public SNETPC_BROAD
	{
		SPASSIVE_SKILL_DATA_CLIENT	sSKILL_DATA;

		SNETPC_UPDATE_PASSIVE_BRD ()
		{
			dwSize = sizeof(SNETPC_UPDATE_PASSIVE_BRD);
			nType = NET_MSG_GCTRL_UPDATE_PASSIVE_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_UPDATE_PASSIVE_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_UPDATE_EXP : public NET_MSG_GENERIC
	{
		
		LONGLONG			lnNowExp;
		BOOL				bJumping;

		SNETPC_UPDATE_EXP() :
			lnNowExp(0),
			bJumping(FALSE)
		{
			dwSize = sizeof(SNETPC_UPDATE_EXP);
			nType = NET_MSG_GCTRL_UPDATE_EXP;
			MIN_STATIC_ASSERT(sizeof(SNETPC_UPDATE_EXP)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_UPDATE_CP : public NET_MSG_GENERIC
	{
		
		WORD				wNowCP;

		SNETPC_UPDATE_CP() :
		wNowCP(0)
		{
			dwSize = sizeof(SNETPC_UPDATE_CP);
			nType = NET_MSG_GCTRL_UPDATE_CP;
			MIN_STATIC_ASSERT(sizeof(SNETPC_UPDATE_CP)<=NET_DATA_BUFSIZE);
		}
	};

	//! 게임머니 변화 Field->Client
	struct NET_CHAR_UPDATE_MONEY_FC : public NET_MSG_GENERIC
	{		
		LONGLONG lnMoney;

		NET_CHAR_UPDATE_MONEY_FC(LONGLONG _lnMoney)
            : lnMoney(_lnMoney)
		{
			dwSize = sizeof(NET_CHAR_UPDATE_MONEY_FC);
			nType = NET_MSG_CHAR_UPDATE_MONEY_FC;
			MIN_STATIC_ASSERT(sizeof(NET_CHAR_UPDATE_MONEY_FC)<=NET_DATA_BUFSIZE);
		}

        inline LONGLONG GetMoney() const { return lnMoney; }
        inline void SetMoney(LONGLONG _lnMoney) { lnMoney = _lnMoney; }
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_UPDATE_SP : public NET_MSG_GENERIC
	{
		
		const int nNowSP;

		SNETPC_UPDATE_SP (const int _nNowSP) :
			nNowSP(_nNowSP)
		{
			dwSize = sizeof(SNETPC_UPDATE_SP);
			nType = NET_MSG_GCTRL_UPDATE_SP;
			MIN_STATIC_ASSERT(sizeof(SNETPC_UPDATE_SP)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_UPDATE_LP : public NET_MSG_GENERIC
	{
		
		int					nLP;

		SNETPC_UPDATE_LP () :
			nLP(0)
		{
			dwSize = sizeof(SNETPC_UPDATE_LP);
			nType = NET_MSG_GCTRL_UPDATE_LP;
			MIN_STATIC_ASSERT(sizeof(SNETPC_UPDATE_LP)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_UPDATE_SKP : public NET_MSG_GENERIC
	{
		
		DWORD				dwSkillPoint;

		SNETPC_UPDATE_SKP () :
			dwSkillPoint(0)
		{
			dwSize = sizeof(SNETPC_UPDATE_SKP);
			nType = NET_MSG_GCTRL_UPDATE_SKP;
			MIN_STATIC_ASSERT(sizeof(SNETPC_UPDATE_SKP)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_UPDATE_BRIGHT : public NET_MSG_GENERIC
	{
		
		int					nBright;

		SNETPC_UPDATE_BRIGHT () :
			nBright(0)
		{
			dwSize = sizeof(SNETPC_UPDATE_BRIGHT);
			nType = NET_MSG_GCTRL_UPDATE_BRIGHT;
			MIN_STATIC_ASSERT(sizeof(SNETPC_UPDATE_BRIGHT)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_UPDATE_BRIGHT_BRD : public SNETPC_BROAD
	{
		int					nBright;

		SNETPC_UPDATE_BRIGHT_BRD () :
			nBright(0)
		{
			dwSize = sizeof(SNETPC_UPDATE_BRIGHT_BRD);
			nType = NET_MSG_GCTRL_UPDATE_BRIGHT_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_UPDATE_BRIGHT_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_UPDATE_STATS : public NET_MSG_GENERIC
	{
		
		WORD				wStatsPoint;

		SNETPC_UPDATE_STATS () :
			wStatsPoint(0)
		{
			dwSize = sizeof(SNETPC_UPDATE_STATS);
			nType = NET_MSG_GCTRL_UPDATE_STATS;
			MIN_STATIC_ASSERT(sizeof(SNETPC_UPDATE_STATS)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNET_PUSHPULL_BRD : public SNETCROW_BROAD
	{
		D3DXVECTOR3			vMovePos;
		float				fPUSH_SPEED;
		DWORD				dwAniType;

		SNET_PUSHPULL_BRD () :
			vMovePos(0,0,0),
			fPUSH_SPEED(0.0f),
			dwAniType( 0 )
		{
			dwSize = sizeof(SNET_PUSHPULL_BRD);
			nType = NET_MSG_GCTRL_PUSHPULL_BRD;
			MIN_STATIC_ASSERT(sizeof(SNET_PUSHPULL_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_LEVELUP : public NET_MSG_GENERIC
	{
        SNETPC_REQ_LEVELUP ()
		{
			dwSize = sizeof(SNETPC_REQ_LEVELUP);
			nType = NET_MSG_GCTRL_REQ_LEVELUP;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_LEVELUP)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_JUMPINGLEVELUP : public NET_MSG_GENERIC
	{
		WORD					wPosX;
		WORD					wPosY;

        SNETPC_REQ_JUMPINGLEVELUP()
			: wPosX(0),
			wPosY(0)
		{
			dwSize = sizeof(SNETPC_REQ_JUMPINGLEVELUP);
			nType = NET_MSG_GCTRL_REQ_JUMPINGLEVELUP;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_JUMPINGLEVELUP)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_JUMPINGLEVELUP_CA : public NET_MSG_GENERIC
	{
		WORD					wPosX;
		WORD					wPosY;

        SNETPC_REQ_JUMPINGLEVELUP_CA()
			: wPosX(0),
			wPosY(0)
		{
			dwSize = sizeof(SNETPC_REQ_JUMPINGLEVELUP_CA);
			nType = NET_MSG_GCTRL_REQ_JUMPINGLEVELUP_CA;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_JUMPINGLEVELUP_CA)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_JUMPINGLEVELUP_FB : public NET_MSG_GENERIC
	{	
		EMINVEN_EXP_CARD_FB	emFB;

		SNETPC_REQ_JUMPINGLEVELUP_FB () :
			emFB(EMINVEN_EXP_FB_FAIL)

		{
			dwSize = sizeof(SNETPC_REQ_JUMPINGLEVELUP_FB);
			nType = NET_MSG_GCTRL_REQ_JUMPINGLEVELUP_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_JUMPINGLEVELUP_FB)<=NET_DATA_BUFSIZE);
		}
	};
	
	//---------------------------------------------------------------------------NET
	struct SNETPC_REQ_LEVELUP_FB : public NET_MSG_GENERIC
	{
		
		WORD				wLevel;
		WORD				wStatsPoint;
		DWORD				dwSkillPoint;
		bool				bInitNowLevel;
		bool				bJumping;
		bool				bCalExp;
		SNATIVEID			sMapID;

		SNETPC_REQ_LEVELUP_FB () :
			wLevel(0),
			wStatsPoint(0),
			dwSkillPoint(0),
			bInitNowLevel(TRUE),
			bJumping(false),
			bCalExp(true),
			sMapID(NATIVEID_NULL())
		{
			dwSize = sizeof(SNETPC_REQ_LEVELUP_FB);
			nType = NET_MSG_GCTRL_REQ_LEVELUP_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_LEVELUP_FB)<=NET_DATA_BUFSIZE);
		}
	};

    //! Field->Agent Character level up
    struct NET_CHAR_LEVEL_UP_FA : public NET_MSG_GENERIC
    {
        enum { MAGIC_NUM=23423, };
        int m_MagicNum;
        DWORD m_ChaDbNum;
        WORD m_ChaLevel;

        NET_CHAR_LEVEL_UP_FA(DWORD ChaDbNum, WORD ChaLevel)
            : m_ChaDbNum(ChaDbNum)
            , m_ChaLevel(ChaLevel)
            , m_MagicNum(MAGIC_NUM)
        {
            dwSize = sizeof(NET_CHAR_LEVEL_UP_FA);
            nType = NET_MSG_CHAR_LEVEL_UP_FA;
            MIN_STATIC_ASSERT(sizeof(NET_CHAR_LEVEL_UP_FA)<=NET_DATA_BUFSIZE);
        }
    };

	struct SNETPC_REQ_LEVELUP_BRD : public SNETPC_BROAD
	{
        WORD wLevel;

		SNETPC_REQ_LEVELUP_BRD( WORD _wLevel )
            : wLevel(_wLevel)
		{
			dwSize = sizeof(SNETPC_REQ_LEVELUP_BRD);
			nType = NET_MSG_GCTRL_REQ_LEVELUP_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_LEVELUP_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_STATSUP : public NET_MSG_GENERIC
	{	
		EMSTATS				emStats;

		SNETPC_REQ_STATSUP () :
			emStats(EMPOW)
		{
			dwSize = sizeof(SNETPC_REQ_STATSUP);
			nType = NET_MSG_GCTRL_REQ_STATSUP;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_STATSUP)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_STATSUP_FB : public NET_MSG_GENERIC
	{
		EMSTATS				emStats;
		WORD				wStatsPoint;

		SNETPC_REQ_STATSUP_FB () :
			emStats(EMPOW),
			wStatsPoint(0)
		{
			dwSize = sizeof(SNETPC_REQ_STATSUP_FB);
			nType = NET_MSG_GCTRL_REQ_STATSUP_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_STATSUP_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_STATSUP_EX : public NET_MSG_GENERIC
	{
		WORD				emStatsArray[EMSIZE];

		SNETPC_REQ_STATSUP_EX() 
		{
			dwSize = sizeof(SNETPC_REQ_STATSUP_EX);
			nType = NET_MSG_GCTRL_REQ_STATSUP_EX;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_STATSUP_EX)<=NET_DATA_BUFSIZE);

			for (int i=0; i < EMSIZE; ++i)
				emStatsArray[i] = 0;
		}
	};

	struct SNETPC_REQ_STATSUP_EX_FB : public NET_MSG_GENERIC
	{
		WORD				emStatsArray[EMSIZE];
		WORD				wStatsPoint;

		SNETPC_REQ_STATSUP_EX_FB() 
		{
			dwSize = sizeof(SNETPC_REQ_STATSUP_EX_FB);
			nType = NET_MSG_GCTRL_REQ_STATSUP_EX_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_STATSUP_EX_FB)<=NET_DATA_BUFSIZE);

			wStatsPoint = 0;
			for (int i=0; i < EMSIZE; ++i)
				emStatsArray[i] = 0;
		}
	};

	struct SNETPC_REQ_BUS : public NET_MSG_GENERIC
	{
		WORD				wPosX;
		WORD				wPosY;

		DWORD				dwNpcGlobID;
		DWORD				dwSTATION_ID;

		EMCROW				emCrow;

		SNETPC_REQ_BUS () :
			wPosX(0),
			wPosY(0),
			dwNpcGlobID(0),
			dwSTATION_ID(0),
			emCrow( CROW_NPC )
		{
			dwSize = sizeof(SNETPC_REQ_BUS);
			nType = NET_MSG_GCTRL_REQ_BUS;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_BUS)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_BUS_FB : public NET_MSG_GENERIC
	{
		EMBUS_TAKE			emFB;

		SNETPC_REQ_BUS_FB () :
			emFB(EMBUS_TAKE_OK)
		{
			dwSize = sizeof(SNETPC_REQ_BUS_FB);
			nType = NET_MSG_GCTRL_REQ_BUS_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_BUS_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_TAXI : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		WORD				wPosX;
		WORD				wPosY;

		DWORD				dwSelectMap;
		DWORD				dwSelectStop;
		DWORD				dwGaeaID;
		D3DXVECTOR3			vPos;		

		SNETPC_REQ_TAXI () :
			wPosX(0),
			wPosY(0),
			dwSelectMap(0),
			dwSelectStop(0),
			dwGaeaID(0),
			vPos(0,0,0)
		{
			dwSize = sizeof(SNETPC_REQ_TAXI);
			nType = NET_MSG_GCTRL_REQ_TAXI;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_TAXI)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_TAXI_NPCPOS : public NET_MSG_GENERIC
	{
		WORD				wPosX;
		WORD				wPosY;

		DWORD				dwSelectMap;
		DWORD				dwSelectStop;
		DWORD				dwGaeaID;
		D3DXVECTOR3			vPos;		

		SNETPC_REQ_TAXI_NPCPOS () :
			wPosX(0),
			wPosY(0),
			dwSelectMap(0),
			dwSelectStop(0),
			dwGaeaID(0),
			vPos(0,0,0)
		{
			dwSize = sizeof(SNETPC_REQ_TAXI_NPCPOS);
			nType = NET_MSG_GCTRL_REQ_TAXI_NPCPOS;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_TAXI_NPCPOS)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_TAXI_NPCPOS_FB : public NET_MSG_GENERIC
	{
		WORD				wPosX;
		WORD				wPosY;

		DWORD				dwSelectMap;
		DWORD				dwSelectStop;
		DWORD				dwGaeaID;
		D3DXVECTOR3			vPos;		

		SNETPC_REQ_TAXI_NPCPOS_FB () :
			wPosX(0),
			wPosY(0),
			dwSelectMap(0),
			dwSelectStop(0),
			dwGaeaID(0),
			vPos(0,0,0)
		{
			dwSize = sizeof(SNETPC_REQ_TAXI_NPCPOS_FB);
			nType = NET_MSG_GCTRL_REQ_TAXI_NPCPOS_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_TAXI_NPCPOS_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_TAXI_FB : public NET_MSG_GENERIC
	{
		EMTAXI_TAKE			emFB;

		SNETPC_REQ_TAXI_FB () :
			emFB(EMTAXI_TAKE_FAIL)
		{
			dwSize = sizeof(SNETPC_REQ_TAXI_FB);
			nType = NET_MSG_GCTRL_REQ_TAXI_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_TAXI_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_STORAGE_STATE : public NET_MSG_GENERIC
	{	
		bool				bVALID[EMSTORAGE_CHANNEL_SPAN_SIZE];

		SNETPC_STORAGE_STATE ()
		{
			dwSize = sizeof(SNETPC_STORAGE_STATE);
			nType = NET_MSG_GCTRL_STORAGE_STATE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_STORAGE_STATE)<=NET_DATA_BUFSIZE);

			for ( int i=0; i<EMSTORAGE_CHANNEL_SPAN_SIZE; ++i  )
				bVALID[i] = false;			
		}
	};

	struct SNETPC_PREMIUM_STATE : public NET_MSG_GENERIC
	{	
		bool				bPREMIUM;

		SNETPC_PREMIUM_STATE () :
			bPREMIUM(0)
		{
			dwSize = sizeof(SNETPC_PREMIUM_STATE);
			nType = NET_MSG_GCTRL_PREMIUM_STATE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PREMIUM_STATE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_FIRECRACKER : public NET_MSG_GENERIC
	{
		
		D3DXVECTOR3			vPOS;

        WORD                wHoldPosX;
        WORD                wHoldPosY;

		SNETPC_REQ_FIRECRACKER ()
            : vPOS(0,0,0)
            , wHoldPosX(0)
            , wHoldPosY(0)
		{
			dwSize = sizeof(SNETPC_REQ_FIRECRACKER);
			nType = NET_MSG_GCTRL_FIRECRACKER;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_FIRECRACKER)<=NET_DATA_BUFSIZE);
		}
	};
	
	struct SNETPC_REQ_FIRECRACKER_FB : public NET_MSG_GENERIC
	{
		
		EMREQ_FIRECRACKER_FB	emFB;

		SNETPC_REQ_FIRECRACKER_FB () :
			emFB(EMREQ_FIRECRACKER_FB_FAIL)
		{
			dwSize = sizeof(SNETPC_REQ_FIRECRACKER_FB);
			nType = NET_MSG_GCTRL_FIRECRACKER_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_FIRECRACKER_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_FIRECRACKER_BRD : public NET_MSG_GENERIC
	{
		

		D3DXVECTOR3				vPOS;
		SNATIVEID				nidITEM;

		SNETPC_REQ_FIRECRACKER_BRD () :
			vPOS(0,0,0),
			nidITEM(false)
		{
			dwSize = sizeof(SNETPC_REQ_FIRECRACKER_BRD);
			nType = NET_MSG_GCTRL_FIRECRACKER_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_FIRECRACKER_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_GETEXP_RECOVERY_NPC : public NET_MSG_GENERIC
	{
		
		DWORD					dwNpcGlobID;

		SNETPC_REQ_GETEXP_RECOVERY_NPC ()
			: dwNpcGlobID( 0 )
		{
			dwSize = sizeof(SNETPC_REQ_GETEXP_RECOVERY_NPC);
			nType = NET_MSG_GCTRL_GETEXP_RECOVERY_NPC;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_GETEXP_RECOVERY_NPC)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_GETEXP_RECOVERY_NPC_FB : public NET_MSG_GENERIC
	{
		

		LONGLONG				nReExp;
		LONGLONG				nDecMoney;
		DWORD					dwNpcGlobID;

		SNETPC_REQ_GETEXP_RECOVERY_NPC_FB ()
			: nReExp ( 0 )
			, nDecMoney ( 0 ) 
			, dwNpcGlobID( 0 )
		{
			dwSize = sizeof(SNETPC_REQ_GETEXP_RECOVERY_NPC_FB);
			nType = NET_MSG_GCTRL_GETEXP_RECOVERY_NPC_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_GETEXP_RECOVERY_NPC_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_RECOVERY_NPC : public NET_MSG_GENERIC
	{
		
		DWORD					dwNpcGlobID;

		SNETPC_REQ_RECOVERY_NPC ()
			: dwNpcGlobID ( 0 )
		{
			dwSize = sizeof(SNETPC_REQ_RECOVERY_NPC);
			nType = NET_MSG_GCTRL_RECOVERY_NPC;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_RECOVERY_NPC)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_RECOVERY_NPC_FB : public NET_MSG_GENERIC
	{
		

		EMREQ_RECOVERY_NPC_FB	emFB;
		LONGLONG				nReExp;

		SNETPC_REQ_RECOVERY_NPC_FB ()
			: emFB(EMREQ_RECOVERY_NPC_FB_FAIL)
			, nReExp( 0 )
		{
			dwSize = sizeof(SNETPC_REQ_RECOVERY_NPC_FB);
			nType = NET_MSG_GCTRL_RECOVERY_NPC_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_RECOVERY_NPC_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_PLAYERKILLING_ADD : public NET_MSG_GENERIC
	{
		DWORD					dwCharID;
		WORD					wSchoolID;
		BOOL					bBAD;
		BOOL					bClubBattle;
		float					fTime;

		char					szName[CHAR_SZNAME];

		SNETPC_PLAYERKILLING_ADD () 
			: dwCharID(0)
			, wSchoolID(0)
			, bBAD(FALSE)
			, fTime(0)
			, bClubBattle(0)
		{
			dwSize = (DWORD) sizeof(SNETPC_PLAYERKILLING_ADD);
			nType = NET_MSG_GCTRL_PLAYERKILLING_ADD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PLAYERKILLING_ADD)<=NET_DATA_BUFSIZE);

			memset (szName, 0, sizeof(char) * CHAR_SZNAME);			
		}
	};

	struct SNETPC_PLAYERKILLING_DEL : public NET_MSG_GENERIC
	{
		

		DWORD					dwCharID;

		SNETPC_PLAYERKILLING_DEL () 
			: dwCharID(0)
		{
			dwSize = (DWORD) sizeof(SNETPC_PLAYERKILLING_DEL);
			nType = NET_MSG_GCTRL_PLAYERKILLING_DEL;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PLAYERKILLING_DEL)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_HAIR_CHANGE : public NET_MSG_GENERIC
	{
		
		
		WORD					wPosX;
		WORD					wPosY;

		SNETPC_INVEN_HAIR_CHANGE () 
			: wPosX(0)
			, wPosY(0)
		{
			dwSize = (DWORD) sizeof(SNETPC_INVEN_HAIR_CHANGE);
			nType = NET_MSG_GCTRL_INVEN_HAIR_CHANGE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_HAIR_CHANGE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_HAIR_CHANGE_FB : public NET_MSG_GENERIC
	{
		
		
		EMINVEN_HAIR_CHANGE_FB	emFB;
		DWORD					dwID;

		SNETPC_INVEN_HAIR_CHANGE_FB () :
			emFB(EMINVEN_HAIR_CHANGE_FB_FAIL),
			dwID(0)
		{
			dwSize = sizeof(SNETPC_INVEN_HAIR_CHANGE_FB);
			nType = NET_MSG_GCTRL_INVEN_HAIR_CHANGE_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_HAIR_CHANGE_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_HAIR_CHANGE_BRD : public SNETPC_BROAD
	{
		DWORD					dwID;

		SNETPC_INVEN_HAIR_CHANGE_BRD () :
			dwID(0)
		{
			dwSize = sizeof(SNETPC_INVEN_HAIR_CHANGE_BRD);
			nType = NET_MSG_GCTRL_INVEN_HAIR_CHANGE_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_HAIR_CHANGE_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_HAIRSTYLE_CHANGE : public NET_MSG_GENERIC
	{
		
		
		WORD					wPosX;
		WORD					wPosY;
		WORD					wHairStyle;

		SNETPC_INVEN_HAIRSTYLE_CHANGE () 
			: wPosX(0)
			, wPosY(0)
			, wHairStyle(0)
		{
			dwSize = (DWORD) sizeof(SNETPC_INVEN_HAIRSTYLE_CHANGE);
			nType = NET_MSG_GCTRL_INVEN_HAIRSTYLE_CHANGE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_HAIRSTYLE_CHANGE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_HAIRSTYLE_CHANGE_FB : public NET_MSG_GENERIC
	{
		
		
		EMINVEN_HAIR_CHANGE_FB	emFB;
		DWORD					dwID;

		SNETPC_INVEN_HAIRSTYLE_CHANGE_FB () :
			emFB(EMINVEN_HAIR_CHANGE_FB_FAIL),
			dwID(0)
		{
			dwSize = sizeof(SNETPC_INVEN_HAIRSTYLE_CHANGE_FB);
			nType = NET_MSG_GCTRL_INVEN_HAIRSTYLE_CHANGE_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_HAIRSTYLE_CHANGE_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_HAIRSTYLE_CHANGE_BRD : public SNETPC_BROAD
	{
		DWORD					dwID;

		SNETPC_INVEN_HAIRSTYLE_CHANGE_BRD () :
			dwID(0)
		{
			dwSize = sizeof(SNETPC_INVEN_HAIRSTYLE_CHANGE_BRD);
			nType = NET_MSG_GCTRL_INVEN_HAIRSTYLE_CHANGE_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_HAIRSTYLE_CHANGE_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_HAIRCOLOR_CHANGE : public NET_MSG_GENERIC
	{
		
		
		WORD					wPosX;
		WORD					wPosY;
		WORD					wHairColor;

		SNETPC_INVEN_HAIRCOLOR_CHANGE () 
			: wPosX(0)
			, wPosY(0)
			, wHairColor(0)
		{
			dwSize = (DWORD) sizeof(SNETPC_INVEN_HAIRCOLOR_CHANGE);
			nType = NET_MSG_GCTRL_INVEN_HAIRCOLOR_CHANGE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_HAIRCOLOR_CHANGE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_HAIRCOLOR_CHANGE_FB : public NET_MSG_GENERIC
	{
		
		
		EMINVEN_HAIR_CHANGE_FB	emFB;
		WORD					wHairColor;

		SNETPC_INVEN_HAIRCOLOR_CHANGE_FB () :
			emFB(EMINVEN_HAIR_CHANGE_FB_FAIL),
			wHairColor(0)
		{
			dwSize = sizeof(SNETPC_INVEN_HAIRCOLOR_CHANGE_FB);
			nType = NET_MSG_GCTRL_INVEN_HAIRCOLOR_CHANGE_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_HAIRCOLOR_CHANGE_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_HAIRCOLOR_CHANGE_BRD : public SNETPC_BROAD
	{
		WORD					wHairColor;

		SNETPC_INVEN_HAIRCOLOR_CHANGE_BRD () :
			wHairColor(0)
		{
			dwSize = sizeof(SNETPC_INVEN_HAIRCOLOR_CHANGE_BRD);
			nType = NET_MSG_GCTRL_INVEN_HAIRCOLOR_CHANGE_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_HAIRCOLOR_CHANGE_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_COSTUM_COLOR_CHANGE : public NET_MSG_GENERIC
	{
		

		WORD					wPosX;
		WORD					wPosY;

		EMSLOT					emSlot;

		DWORD					dwMainColor;
		DWORD					dwSubColor;

		SNETPC_INVEN_COSTUM_COLOR_CHANGE () 
			: wPosX(0)
			, wPosY(0)
		{
			dwMainColor = 0;
			dwSubColor = 0;

			dwSize = (DWORD) sizeof(SNETPC_INVEN_COSTUM_COLOR_CHANGE);
			nType = NET_MSG_GCTRL_COSTUM_COLOR_CHANGE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_COSTUM_COLOR_CHANGE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_COSTUM_COLOR_CHANGE_FB : public NET_MSG_GENERIC
	{
		

		EMINVEN_COSTUM_COLOR_CHANGE_FB	emFB;

		EMSLOT					emSlot;

		DWORD					dwMainColor;
		DWORD					dwSubColor;


		SNETPC_INVEN_COSTUM_COLOR_CHANGE_FB () :
		emFB(EMINVEN_COSTUM_COLOR_CHANGE_FB_FAIL)
		{
			
			dwMainColor = 0;
			dwSubColor = 0;

			dwSize = sizeof(SNETPC_INVEN_COSTUM_COLOR_CHANGE_FB);
			nType = NET_MSG_GCTRL_COSTUM_COLOR_CHANGE_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_COSTUM_COLOR_CHANGE_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_COSTUM_COLOR_CHANGE_BRD : public SNETPC_BROAD
	{
		EMSLOT					emSlot;

		DWORD					dwMainColor;
		DWORD					dwSubColor;

		SNETPC_INVEN_COSTUM_COLOR_CHANGE_BRD ()
		{
			dwMainColor = 0;
			dwSubColor = 0;

			dwSize = sizeof(SNETPC_INVEN_COSTUM_COLOR_CHANGE_BRD);
			nType = NET_MSG_GCTRL_COSTUM_COLOR_CHANGE_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_COSTUM_COLOR_CHANGE_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_VEHICLE_COLOR_CHANGE : public NET_MSG_GENERIC
	{
		

		WORD					wPosX;
		WORD					wPosY;
		DWORD					dwMainColor[ ACCE_TYPE_SIZE ];
		DWORD					dwSubColor[ ACCE_TYPE_SIZE ];

		SNETPC_INVEN_VEHICLE_COLOR_CHANGE () 
			: wPosX(0)
			, wPosY(0)
		{
			for( int i = 0; i < ACCE_TYPE_SIZE; i++ )
			{
				dwMainColor[ i ] = 0;
				dwSubColor[ i ] = 0;
			}

			dwSize = sizeof(SNETPC_INVEN_VEHICLE_COLOR_CHANGE);
			nType = NET_MSG_GCTRL_VEHICLE_COLOR_CHANGE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_VEHICLE_COLOR_CHANGE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_VEHICLE_COLOR_CHANGE_FB : public NET_MSG_GENERIC
	{
		

		EMINVEN_VEHICLE_CHANGE_FB	emFB;
		DWORD					dwMainColor[ ACCE_TYPE_SIZE ];
		DWORD					dwSubColor[ ACCE_TYPE_SIZE ];


		SNETPC_INVEN_VEHICLE_COLOR_CHANGE_FB () :
		emFB(EMINVEN_VEHICLE_CHANGE_FB_FAIL)
		{
			for( int i = 0; i < ACCE_TYPE_SIZE; i++ )
			{
				dwMainColor[ i ] = 0;
				dwSubColor[ i ] = 0;
			}

			dwSize = sizeof(SNETPC_INVEN_VEHICLE_COLOR_CHANGE_FB);
			nType = NET_MSG_GCTRL_VEHICLE_COLOR_CHANGE_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_VEHICLE_COLOR_CHANGE_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_VEHICLE_COLOR_CHANGE_BRD : public SNETPC_BROAD
	{
		DWORD					dwMainColor[ ACCE_TYPE_SIZE ];
		DWORD					dwSubColor[ ACCE_TYPE_SIZE ];

		SNETPC_INVEN_VEHICLE_COLOR_CHANGE_BRD ()
		{
			for( int i = 0; i < ACCE_TYPE_SIZE; i++ )
			{
				dwMainColor[ i ] = 0;
				dwSubColor[ i ] = 0;
			}

			dwSize = sizeof(SNETPC_INVEN_VEHICLE_COLOR_CHANGE_BRD);
			nType = NET_MSG_GCTRL_VEHICLE_COLOR_CHANGE_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_VEHICLE_COLOR_CHANGE_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_BIKE_BOOST : public NET_MSG_GENERIC
	{
		

		WORD					wPosX;
		WORD					wPosY;

		SNETPC_REQ_BIKE_BOOST () 
			: wPosX(0)
			, wPosY(0)
		{

			dwSize = sizeof(SNETPC_REQ_BIKE_BOOST);
			nType = NET_MSG_GCTRL_LEARN_BIKE_BOOST;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_BIKE_BOOST)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_BIKE_BOOST_FB : public NET_MSG_GENERIC
	{
		
		EMINVEN_LEARN_BIKE_BOOST_FB	emFB;

		DWORD					dwBoosterID;

		SNETPC_REQ_BIKE_BOOST_FB () 
			: emFB(EMINVEN_LEARN_BIKE_BOOST_FB_FAIL)
			, dwBoosterID( 0 )
		{

			dwSize = sizeof(SNETPC_REQ_BIKE_BOOST_FB);
			nType = NET_MSG_GCTRL_LEARN_BIKE_BOOST_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_BIKE_BOOST_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_BIKE_BOOST_BRD : public SNETPC_BROAD
	{
		DWORD					dwBoosterID;

		SNETPC_REQ_BIKE_BOOST_BRD ()
			: dwBoosterID( 0 )
		{

			dwSize = sizeof(SNETPC_REQ_BIKE_BOOST_BRD);
			nType = NET_MSG_GCTRL_LEARN_BIKE_BOOST_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_BIKE_BOOST_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_USE_BIKE_BOOST : public NET_MSG_GENERIC
	{
		SNETPC_USE_BIKE_BOOST() 
		{
			dwSize = sizeof(SNETPC_USE_BIKE_BOOST);
			nType = NET_MSG_GCTRL_USE_BIKE_BOOST;
			MIN_STATIC_ASSERT(sizeof(SNETPC_USE_BIKE_BOOST)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_USE_BIKE_BOOST_FB : public NET_MSG_GENERIC
	{
		
		EMINVEN_USE_BIKE_BOOST_FB emFB;

		SNETPC_USE_BIKE_BOOST_FB () 
		: emFB(EMINVEN_USE_BIKE_BOOST_FB_FAIL)
		{

			dwSize = sizeof(SNETPC_USE_BIKE_BOOST_FB);
			nType = NET_MSG_GCTRL_USE_BIKE_BOOST_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_USE_BIKE_BOOST_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_USE_BIKE_BOOST_BRD : public SNETPC_BROAD
	{
		SNETPC_REQ_USE_BIKE_BOOST_BRD ()
		{

			dwSize = sizeof(SNETPC_REQ_USE_BIKE_BOOST_BRD);
			nType = NET_MSG_GCTRL_USE_BIKE_BOOST_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_USE_BIKE_BOOST_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_USE_BIKE_BOOST_CANCEL_FB : public NET_MSG_GENERIC
	{
		
		EMINVEN_USE_BIKE_BOOST_CANCEL_FB emFB;

		SNETPC_USE_BIKE_BOOST_CANCEL_FB () 
		: emFB(EMINVEN_USE_BIKE_BOOST_CANCEL)
		{

			dwSize = sizeof(SNETPC_USE_BIKE_BOOST_CANCEL_FB);
			nType = NET_MSG_GCTRL_USE_BIKE_BOOST_CANCEL_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_USE_BIKE_BOOST_CANCEL_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_USE_BIKE_BOOST_CANCEL_BRD : public SNETPC_BROAD
	{
		EMINVEN_USE_BIKE_BOOST_CANCEL_FB emFB;

		SNETPC_REQ_USE_BIKE_BOOST_CANCEL_BRD ()
		: emFB(EMINVEN_USE_BIKE_BOOST_CANCEL)
		{

			dwSize = sizeof(SNETPC_REQ_USE_BIKE_BOOST_CANCEL_BRD);
			nType = NET_MSG_GCTRL_USE_BIKE_BOOST_CANCEL_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_USE_BIKE_BOOST_CANCEL_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_FACE_CHANGE : public NET_MSG_GENERIC
	{
		
		
		WORD					wPosX;
		WORD					wPosY;

		SNETPC_INVEN_FACE_CHANGE () :
			wPosX(0),
			wPosY(0)			
		{
			dwSize = sizeof(SNETPC_INVEN_FACE_CHANGE);
			nType = NET_MSG_GCTRL_INVEN_FACE_CHANGE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_FACE_CHANGE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_FACE_CHANGE_FB : public NET_MSG_GENERIC
	{
		
		
		EMINVEN_FACE_CHANGE_FB	emFB;
		DWORD					dwID;

		SNETPC_INVEN_FACE_CHANGE_FB () :
			emFB(EMINVEN_FACE_CHANGE_FB_FAIL),
			dwID(0)
		{
			dwSize = sizeof(SNETPC_INVEN_FACE_CHANGE_FB);
			nType = NET_MSG_GCTRL_INVEN_FACE_CHANGE_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_FACE_CHANGE_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_FACE_CHANGE_BRD : public SNETPC_BROAD
	{
		DWORD					dwID;

		SNETPC_INVEN_FACE_CHANGE_BRD () :
			dwID(0)
		{
			dwSize = sizeof(SNETPC_INVEN_FACE_CHANGE_BRD);
			nType = NET_MSG_GCTRL_INVEN_FACE_CHANGE_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_FACE_CHANGE_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_FACESTYLE_CHANGE : public NET_MSG_GENERIC
	{
		
		
		WORD					wPosX;
		WORD					wPosY;
		WORD					wFaceStyle;

		SNETPC_INVEN_FACESTYLE_CHANGE () 
			: wPosX(0)
			, wPosY(0)
			, wFaceStyle(0)
		{
			dwSize = sizeof(SNETPC_INVEN_FACESTYLE_CHANGE);
			nType = NET_MSG_GCTRL_INVEN_FACESTYLE_CHANGE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_FACESTYLE_CHANGE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_FACESTYLE_CHANGE_FB : public NET_MSG_GENERIC
	{
		
		
		EMINVEN_HAIR_CHANGE_FB	emFB;
		DWORD					dwID;

		SNETPC_INVEN_FACESTYLE_CHANGE_FB () :
			emFB(EMINVEN_HAIR_CHANGE_FB_FAIL),
			dwID(0)
		{
			dwSize = sizeof(SNETPC_INVEN_FACESTYLE_CHANGE_FB);
			nType = NET_MSG_GCTRL_INVEN_FACESTYLE_CHANGE_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_FACESTYLE_CHANGE_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_FACESTYLE_CHANGE_BRD : public SNETPC_BROAD
	{
		DWORD					dwID;

		SNETPC_INVEN_FACESTYLE_CHANGE_BRD () :
			dwID(0)
		{
			dwSize = sizeof(SNETPC_INVEN_FACESTYLE_CHANGE_BRD);
			nType = NET_MSG_GCTRL_INVEN_FACESTYLE_CHANGE_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_FACESTYLE_CHANGE_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_GENDER_CHANGE : public NET_MSG_GENERIC
	{	
		WORD					wPosX;
		WORD					wPosY;
		WORD					wFace;
		WORD					wHair;

		SNETPC_INVEN_GENDER_CHANGE () 
			: wPosX(0)
			, wPosY(0)
			, wFace(0)
			, wHair(0)
		{
			dwSize = sizeof(SNETPC_INVEN_GENDER_CHANGE);
			nType = NET_MSG_GCTRL_INVEN_GENDER_CHANGE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_GENDER_CHANGE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_GENDER_CHANGE_FB : public NET_MSG_GENERIC
	{	
		EMINVEN_GENDER_CHANGE_FB	emFB;
		WORD						wFace;
		WORD						wHair;
		WORD						wHairColor;
		EMCHARCLASS					emClass;
		WORD						wSex;

		SNETPC_INVEN_GENDER_CHANGE_FB () :
			emFB(EMINVEN_GENDER_CHANGE_FB_FAIL),
			wFace(0),
			wHair(0),
			wHairColor(0),
			emClass ( GLCC_NONE ),
			wSex ( 0 )

		{
			dwSize = sizeof(SNETPC_INVEN_GENDER_CHANGE_FB);
			nType = NET_MSG_GCTRL_INVEN_GENDER_CHANGE_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_GENDER_CHANGE_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_JUMPING_LEVLEUP_BYEXP : public NET_MSG_GENERIC
	{	
		int					m_nExp;

		SNETPC_JUMPING_LEVLEUP_BYEXP () 
			: m_nExp(0)
		{
			dwSize = sizeof(SNETPC_JUMPING_LEVLEUP_BYEXP);
			nType = NET_MSG_GCTRL_REQ_JUMPING_LEVLEUP_BYEXP;
			MIN_STATIC_ASSERT(sizeof(SNETPC_JUMPING_LEVLEUP_BYEXP)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_SCHOOL_CHANGE : public NET_MSG_GENERIC
	{	
		WORD					wPosX;
		WORD					wPosY;
		WORD					wSchool;

		SNETPC_INVEN_SCHOOL_CHANGE () 
			: wPosX(0)
			, wPosY(0)
			, wSchool(0)
		{
			dwSize = sizeof(SNETPC_INVEN_SCHOOL_CHANGE);
			nType = NET_MSG_GCTRL_INVEN_SCHOOL_CHANGE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_SCHOOL_CHANGE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_SCHOOL_CHANGE_FB : public NET_MSG_GENERIC
	{	
		EMINVEN_SCHOOL_CHANGE_FB	emFB;
		WORD	wSchool;

		SNETPC_INVEN_SCHOOL_CHANGE_FB () :
			emFB(EMINVEN_SCHOOL_CHANGE_FB_FAIL),
			wSchool ( -1 )

		{
			dwSize = sizeof(SNETPC_INVEN_SCHOOL_CHANGE_FB);
			nType = NET_MSG_GCTRL_INVEN_SCHOOL_CHANGE_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_SCHOOL_CHANGE_FB)<=NET_DATA_BUFSIZE);
		}
	};

    //! 캐릭터 이름 변경
	struct SNETPC_INVEN_RENAME : public NET_MSG_GENERIC
	{
		char					szName[CHAR_SZNAME];
		WORD					wPosX;
		WORD					wPosY;

		SNETPC_INVEN_RENAME () :
			wPosX(0),
			wPosY(0)
		{
			dwSize = sizeof(SNETPC_INVEN_RENAME);
			nType  = NET_MSG_GCTRL_INVEN_RENAME;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_RENAME)<=NET_DATA_BUFSIZE);

			memset ( szName, 0, sizeof(char)*CHAR_SZNAME );			
		}
	};

	struct SNETPC_INVEN_RENAME_FB : public NET_MSG_GENERIC
	{
		EMINVEN_RENAME_FB		emFB;
		char					szName[CHAR_SZNAME];

		SNETPC_INVEN_RENAME_FB () :
			emFB(EMINVEN_RENAME_FB_FAIL)
		{
			dwSize = sizeof(SNETPC_INVEN_RENAME_FB);
			nType = NET_MSG_GCTRL_INVEN_RENAME_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_RENAME_FB)<=NET_DATA_BUFSIZE);

			memset ( szName, 0, sizeof(char)*CHAR_SZNAME );			
		}
	};

	struct SNETPC_INVEN_RENAME_FROM_DB : public NET_MSG_GENERIC
	{
		EMINVEN_RENAME_FB			emFB;
		DWORD						dwCharID;
		WORD						wPosX;
		WORD						wPosY;
		char						szName[CHAR_SZNAME];

		enum { EMMSG_SIZE = sizeof(NET_MSG_GENERIC)+sizeof(DWORD)*3+CHAR_SZNAME };
		char				m_cBUFFER[NET_DATA_BUFSIZE-EMMSG_SIZE];

		SNETPC_INVEN_RENAME_FROM_DB () :
			emFB(EMINVEN_RENAME_FROM_DB_FAIL),
			dwCharID(0),
			wPosX(0),
			wPosY(0)
		{
			dwSize = sizeof(SNETPC_INVEN_RENAME_FROM_DB);			
			nType = NET_MSG_GCTRL_INVEN_RENAME_FROM_DB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_RENAME_FROM_DB)<=NET_DATA_BUFSIZE);

			memset ( szName, 0, sizeof(char)*CHAR_SZNAME );
		}
	};

	struct SNETPC_INVEN_RENAME_BRD : public SNETPC_BROAD
	{
		char szName[CHAR_SZNAME];
		SNETPC_INVEN_RENAME_BRD()
		{	
			dwSize = sizeof(SNETPC_INVEN_RENAME_BRD);
			nType = NET_MSG_GCTRL_INVEN_RENAME_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_RENAME_BRD)<=NET_DATA_BUFSIZE);
			memset ( szName, 0, sizeof(char)*CHAR_SZNAME );
		}
	};

	struct SNETPC_INVEN_RENAME_AGTBRD : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{	
		DWORD				dwID;
		char				szOldName[CHAR_SZNAME];
		char				szNewName[CHAR_SZNAME];

		enum { EMMSG_SIZE = sizeof(NET_MSG_CHARACTER)+sizeof(DWORD)+(CHAR_SZNAME)*2 };
		char				m_cBUFFER[NET_DATA_BUFSIZE-EMMSG_SIZE];

		SNETPC_INVEN_RENAME_AGTBRD () :
			dwID(0)
		{	
			dwSize = sizeof(SNETPC_INVEN_RENAME_AGTBRD);
			nType = NET_MSG_GCTRL_INVEN_RENAME_AGTBRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_RENAME_AGTBRD)<=NET_DATA_BUFSIZE);

			memset ( szOldName, 0, sizeof(char)*CHAR_SZNAME );
			memset ( szNewName, 0, sizeof(char)*CHAR_SZNAME );
		}
	};

	struct SNETPC_CHANGE_NAMEMAP : public SNETPC_BROAD
	{
		char 					szOldName[CHAR_SZNAME];
		char 					szNewName[CHAR_SZNAME];

		SNETPC_CHANGE_NAMEMAP ()
		{	
			dwSize = sizeof(SNETPC_CHANGE_NAMEMAP);
			nType = NET_MSG_GCTRL_CHANGE_NAMEMAP;
			MIN_STATIC_ASSERT(sizeof(SNETPC_CHANGE_NAMEMAP)<=NET_DATA_BUFSIZE);

			memset ( szOldName, 0, sizeof(char)*CHAR_SZNAME );
			memset ( szNewName, 0, sizeof(char)*CHAR_SZNAME );
		}
	};

	struct SNETPC_DIALOGUEWINDOW_OPEN : public NET_MSG_GENERIC
	{
		EMCROW    emCrow;
		SNATIVEID sNPCID;
		DWORD dwGaeaID;

		SNETPC_DIALOGUEWINDOW_OPEN()
			: emCrow(CROW_NPC), sNPCID(NATIVEID_NULL()), dwGaeaID(NULL)
		{
			dwSize = sizeof(SNETPC_DIALOGUEWINDOW_OPEN);
			nType = NET_MSG_GCTRL_DIALOGUEWINDOW_OPEN;
			MIN_STATIC_ASSERT(sizeof(SNETPC_DIALOGUEWINDOW_OPEN)<= NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_DIALOGUEWINDOW_OPEN_FB : public NET_MSG_GENERIC
	{
		EMCROW			 emCrow;
		EMREQ_NPCTALK_FB emFB;
		SNATIVEID sNPCID;

		SNETPC_DIALOGUEWINDOW_OPEN_FB()
			: emCrow(CROW_NPC), sNPCID(NATIVEID_NULL()),
			emFB(EMREQ_NPCTALK_FB_FAIL)
		{
			dwSize = sizeof(SNETPC_DIALOGUEWINDOW_OPEN_FB);
			nType = NET_MSG_GCTRL_DIALOGUEWINDOW_OPEN_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_DIALOGUEWINDOW_OPEN_FB)<= NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_POSTBOX_OPEN : public NET_MSG_GENERIC
	{
		
		SNATIVEID		sPostBoxID;

		SNETPC_POSTBOX_OPEN ()
			: sPostBoxID ( NATIVEID_NULL() )
		{
			dwSize = sizeof(SNETPC_POSTBOX_OPEN);
			nType = NET_MSG_GCTRL_POSTBOX_OPEN;
			MIN_STATIC_ASSERT(sizeof(SNETPC_POSTBOX_OPEN)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_POSTBOX_OPEN_FB : public NET_MSG_GENERIC
	{
		
		EMINVEN_POSTBOX_OPEN_FB emFB;
		SNATIVEID				sPostBoxID;

		SNETPC_POSTBOX_OPEN_FB ()
			: emFB			( EMINVEN_POSTBOX_OPEN_FB_FAIL )
			, sPostBoxID	( NATIVEID_NULL() )
		{
			dwSize = sizeof(SNETPC_POSTBOX_OPEN_FB);
			nType = NET_MSG_GCTRL_POSTBOX_OPEN_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_POSTBOX_OPEN_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_POSTBOX_OPEN_CARD : public NET_MSG_GENERIC
	{
		
		WORD					wPosX;
		WORD					wPosY;

		SNETPC_INVEN_POSTBOX_OPEN_CARD ()
			: wPosX (0)
			, wPosY (0)
		{
			dwSize = sizeof(SNETPC_INVEN_POSTBOX_OPEN_CARD);
			nType = NET_MSG_GCTRL_INVEN_POSTBOX_OPEN_CARD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_POSTBOX_OPEN_CARD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_POSTBOX_OPEN_CARD_FB : public NET_MSG_GENERIC
	{
		
		EMINVEN_POSTBOX_OPEN_CARD_FB emFB;
		SNATIVEID					 sPostBoxCardID;

		SNETPC_INVEN_POSTBOX_OPEN_CARD_FB ()
			: emFB			 ( EMINVEN_POSTBOX_OPEN_CARD_FB_FAIL )
			, sPostBoxCardID ( NATIVEID_NULL() )
		{
			dwSize = sizeof(SNETPC_INVEN_POSTBOX_OPEN_CARD_FB);
			nType = NET_MSG_GCTRL_INVEN_POSTBOX_OPEN_CARD_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_POSTBOX_OPEN_CARD_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_POSTBOX_CLOSE : public NET_MSG_GENERIC
	{
		

		SNETPC_POSTBOX_CLOSE ()
		{
			dwSize = sizeof(SNETPC_POSTBOX_CLOSE);
			nType = NET_MSG_GCTRL_POSTBOX_CLOSE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_POSTBOX_CLOSE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_GESTURE : public NET_MSG_GENERIC
	{
		DWORD					dwID;

		SNETPC_REQ_GESTURE () :
			dwID(0)
		{
			dwSize = sizeof(SNETPC_REQ_GESTURE);
			nType = NET_MSG_GCTRL_REQ_GESTURE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_GESTURE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_GESTURE_BRD : public SNETPC_BROAD
	{
		DWORD					dwID;

		SNETPC_REQ_GESTURE_BRD () :
			dwID(0)
		{
			dwSize = sizeof(SNETPC_REQ_GESTURE_BRD);
			nType = NET_MSG_GCTRL_REQ_GESTURE_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_GESTURE_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_QITEMFACT_BRD : public SNETPC_BROAD
	{
		SQITEM_FACT sFACT;

		SNETPC_QITEMFACT_BRD ()
		{
			dwSize = sizeof(SNETPC_QITEMFACT_BRD);
			nType = NET_MSG_GCTRL_QITEMFACT_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_QITEMFACT_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_QITEMFACT_END_BRD : public SNETPC_BROAD
	{
		SNETPC_QITEMFACT_END_BRD ()
		{
			dwSize = sizeof(SNETPC_QITEMFACT_END_BRD);
			nType = NET_MSG_GCTRL_QITEMFACT_END_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_QITEMFACT_END_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_EVENTFACT_BRD : public SNETPC_BROAD
	{
		EMGM_EVENT_TYPE		emType;
		WORD				wValue;

		SNETPC_EVENTFACT_BRD ()
			: emType(EMGM_EVENT_NONE)
			, wValue(0)
		{
			dwSize = sizeof(SNETPC_EVENTFACT_BRD);
			nType = NET_MSG_GCTRL_EVENTFACT_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_EVENTFACT_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_EVENTFACT_END_BRD : public SNETPC_BROAD
	{
		EMGM_EVENT_TYPE		emType;

		SNETPC_EVENTFACT_END_BRD ()
			: emType(EMGM_EVENT_NONE)
		{
			dwSize = sizeof(SNETPC_EVENTFACT_END_BRD);
			nType = NET_MSG_GCTRL_EVENTFACT_END_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_EVENTFACT_END_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_EVENTFACT_INFO : public NET_MSG_GENERIC
	{
		
		SEVENT_FACT				sEVENTFACT;

		SNETPC_EVENTFACT_INFO ()
		{
			dwSize = sizeof(SNETPC_EVENTFACT_INFO);
			nType = NET_MSG_GCTRL_EVENTFACT_INFO;
			MIN_STATIC_ASSERT(sizeof(SNETPC_EVENTFACT_INFO)<=NET_DATA_BUFSIZE);
		}
	};

    struct SNETPC_IPEVENT_BRD : public SNETPC_BROAD
    {
        bool bEnable;

        DWORD m_AttackValue;
        DWORD m_AttackSpeed;
        DWORD m_MoveSpeed;
        DWORD m_ItemDrop;
        DWORD m_MoneyDrop;
        DWORD m_Exp;
        DWORD m_HPRecovery;
        DWORD m_MPRecovery;
        DWORD m_SPRecovery;
        DWORD m_AllRecovery;
        DWORD m_CriticalRate;

        SNETPC_IPEVENT_BRD( bool _Enable )
            : bEnable(_Enable)
        {
            dwSize = sizeof(SNETPC_IPEVENT_BRD);
            nType = NET_MSG_GCTRL_IPEVENT_BRD;
            MIN_STATIC_ASSERT(sizeof(SNETPC_IPEVENT_BRD)<=NET_DATA_BUFSIZE);
        }
    };

	struct SNETPC_2_FRIEND_REQ : public NET_MSG_GENERIC
	{
		
		char					szFRIEND_NAME[CHAR_SZNAME];
		WORD				wItemPosX;
		WORD				wItemPosY;
		//DWORD				dwGaeaID;
		DWORD               dwChaDbNum;
		DWORD				dwItemID;

		SNETPC_2_FRIEND_REQ () :
			wItemPosX(0),
			wItemPosY(0)
		{			
			dwSize = sizeof(SNETPC_2_FRIEND_REQ);
			nType = NET_MSG_GCTRL_2_FRIEND_REQ;
			MIN_STATIC_ASSERT(sizeof(SNETPC_2_FRIEND_REQ)<=NET_DATA_BUFSIZE);

			memset(szFRIEND_NAME, 0, sizeof(char) * CHAR_SZNAME);
		}
	};

	struct SNETPC_2_FRIEND_CK : public NET_MSG_GENERIC
	{
		
		DWORD					dwCOMMAND_CHARID;
		DWORD					dwFRIEND_CHARID;
		DWORD					dwItemID;
		WORD					wItemPosX;
		WORD					wItemPosY;

		SNETPC_2_FRIEND_CK () :
			dwCOMMAND_CHARID(0),
			dwFRIEND_CHARID(0),
			dwItemID(0),
			wItemPosX(0),
			wItemPosY(0)
		{
			dwSize = sizeof(SNETPC_2_FRIEND_CK);
			nType = NET_MSG_GCTRL_2_FRIEND_CK;
			MIN_STATIC_ASSERT(sizeof(SNETPC_2_FRIEND_CK)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_2_FRIEND_AG : public NET_MSG_GENERIC
	{	
		DWORD					dwCOMMAND_CHARID;
		DWORD					dwFRIEND_CHARID;
		WORD					wItemPosX;
		WORD					wItemPosY;

		EM2FRIEND_FB			emFB;
		MapID					sFriendMapID;
		D3DXVECTOR3			vFriendPos;

		SNETPC_2_FRIEND_AG(void)
			: dwCOMMAND_CHARID(0)
			, dwFRIEND_CHARID(0)
			, wItemPosX(0)
			, wItemPosY(0)
			, emFB(EM2FRIEND_FB_FAIL)
			, vFriendPos(0,0,0)
		{
			dwSize = sizeof(SNETPC_2_FRIEND_AG);
			nType = NET_MSG_GCTRL_2_FRIEND_AG;
			MIN_STATIC_ASSERT(sizeof(SNETPC_2_FRIEND_AG)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_2_FRIEND_FLD : public NET_MSG_CHARACTER
	{
		WORD					wItemPosX;
		WORD					wItemPosY;

		MapID					sFriendMapID;
		D3DXVECTOR3			vFriendPos;

		SNETPC_2_FRIEND_FLD(void)
			: wItemPosX(0)
			, wItemPosY(0)
			, vFriendPos(0,0,0)
		{
			dwSize = sizeof(SNETPC_2_FRIEND_FLD);
			nType = NET_MSG_GCTRL_2_FRIEND_FLD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_2_FRIEND_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_2_FRIEND_FB : public NET_MSG_GENERIC
	{	
		EM2FRIEND_FB			emFB;

		SNETPC_2_FRIEND_FB () :
			emFB(EM2FRIEND_FB_FAIL)
		{
			dwSize = sizeof(SNETPC_2_FRIEND_FB);
			nType = NET_MSG_GCTRL_2_FRIEND_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_2_FRIEND_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_GM_GENCHAR : public NET_MSG_GENERIC
	{	
		char					szNAME[CHAR_SZNAME];
		DWORD					dwCHARID;

		SNETPC_GM_GENCHAR ()
			: dwCHARID(0)
		{	
			dwSize = sizeof(SNETPC_GM_GENCHAR);
			nType = NET_MSG_GM_GENCHAR;
			MIN_STATIC_ASSERT(sizeof(SNETPC_GM_GENCHAR)<=NET_DATA_BUFSIZE);

			memset(szNAME, 0, sizeof(char) * (CHAR_SZNAME));
		}
	};

	struct SNETPC_GM_GENCHAR_FB : public NET_MSG_GENERIC
	{
		int						nChannel;
		EMGM_MOVE2CHAR_FB		emFB;

		SNETPC_GM_GENCHAR_FB () :
			nChannel(0),
			emFB(EMGM_MOVE2CHAR_FB_FAIL)
		{
			dwSize = sizeof(SNETPC_GM_GENCHAR_FB);
			nType = NET_MSG_GM_GENCHAR_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_GM_GENCHAR_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_GM_GETWHISPERMSG : public NET_MSG_GENERIC
	{	
		char					szNAME[CHAR_SZNAME];

		SNETPC_GM_GETWHISPERMSG ()
		{
			dwSize = sizeof(SNETPC_GM_GETWHISPERMSG);
			nType = NET_MSG_GM_GETWHISPERMSG;			
			MIN_STATIC_ASSERT(sizeof(SNETPC_GM_GETWHISPERMSG)<=NET_DATA_BUFSIZE);

			memset(szNAME, 0, sizeof(char) * (CHAR_SZNAME));
		}
	};

	struct SNETPC_GM_CHECKINSTANTNUM : public NET_MSG_GENERIC
	{
		SNETPC_GM_CHECKINSTANTNUM ()
		{
			dwSize = sizeof(SNETPC_GM_CHECKINSTANTNUM);
			nType = NET_MSG_GM_CHECKINSTANTNUM;
			MIN_STATIC_ASSERT(sizeof(SNETPC_GM_CHECKINSTANTNUM)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_GM_CHECKINSTANTNUM_FB : public NET_MSG_GENERIC
	{	
		DWORD					dwInstantmapCnt;

		SNETPC_GM_CHECKINSTANTNUM_FB () : dwInstantmapCnt(0)
		{
			dwSize = sizeof(SNETPC_GM_CHECKINSTANTNUM_FB);
			nType = NET_MSG_GM_CHECKINSTANTNUM_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_GM_CHECKINSTANTNUM_FB)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNETPC_GM_MOVE2CHAR : public NET_MSG_GENERIC
	{	
		char					szNAME[CHAR_SZNAME];
		DWORD					dwCHARID;

		SNETPC_GM_MOVE2CHAR ()
			: dwCHARID(0)
		{
			dwSize = sizeof(SNETPC_GM_MOVE2CHAR);
			nType = NET_MSG_GM_MOVE2CHAR;			
			MIN_STATIC_ASSERT(sizeof(SNETPC_GM_MOVE2CHAR)<=NET_DATA_BUFSIZE);

			memset(szNAME, 0, sizeof(char) * (CHAR_SZNAME));
		}
	};

	struct SNETPC_GM_MOVE2CHAR_POS : public NET_MSG_GENERIC
	{	
		DWORD					dwCOMMAND_CHARID;
		DWORD					dwTO_CHARID;

		SNETPC_GM_MOVE2CHAR_POS () 
			: dwCOMMAND_CHARID(0)
			, dwTO_CHARID(0)
		{
			dwSize = sizeof(SNETPC_GM_MOVE2CHAR_POS);
			nType = NET_MSG_GM_MOVE2CHAR_POS;
			MIN_STATIC_ASSERT(sizeof(SNETPC_GM_MOVE2CHAR_POS)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_GM_MOVE2CHAR_AG : public NET_MSG_GENERIC
	{	
		DWORD					dwCOMMAND_CHARID;
		DWORD					dwTO_CHARID;

		EMGM_MOVE2CHAR_FB		emFB;
		DWORD						dwFieldServerIDTo;
		MapID						mapIDTo;
		D3DXVECTOR3				vToPos;

		SNETPC_GM_MOVE2CHAR_AG(void)
			: dwCOMMAND_CHARID(0)
			, dwTO_CHARID(0)
			, emFB(EMGM_MOVE2CHAR_FB_FAIL)
			, dwFieldServerIDTo(GAEAID_NULL)
			, vToPos(0,0,0)
		{
			dwSize = sizeof(SNETPC_GM_MOVE2CHAR_AG);
			nType = NET_MSG_GM_MOVE2CHAR_AG;
			MIN_STATIC_ASSERT(sizeof(SNETPC_GM_MOVE2CHAR_AG)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_GM_MOVE2CHAR_FLD : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		DWORD				dwFieldServerIDTo;
		MapID				mapIDTo;
		D3DXVECTOR3		vToPos;

		SNETPC_GM_MOVE2CHAR_FLD(void)
			: dwFieldServerIDTo(GAEAID_NULL)
			, vToPos(0,0,0)
		{
			dwSize = sizeof(SNETPC_GM_MOVE2CHAR_FLD);
			nType = NET_MSG_GM_MOVE2CHAR_FLD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_GM_MOVE2CHAR_FLD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_GM_MOVE2CHAR_FB : public NET_MSG_GENERIC
	{
		int						nChannel;
		EMGM_MOVE2CHAR_FB		emFB;

		SNETPC_GM_MOVE2CHAR_FB () :
			nChannel(0),
			emFB(EMGM_MOVE2CHAR_FB_FAIL)
		{
			dwSize = sizeof(SNETPC_GM_MOVE2CHAR_FB);
			nType = NET_MSG_GM_MOVE2CHAR_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_GM_MOVE2CHAR_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_UPDATE_LASTCALL : public NET_MSG_GENERIC
	{
		
		
		SNATIVEID			sLastCallMapID;		//	직전귀환 맵.
		D3DXVECTOR3			vLastCallPos;		//	직전귀환 위치.

		SNETPC_UPDATE_LASTCALL () :
			vLastCallPos(0,0,0)
		{
			dwSize = sizeof(SNETPC_UPDATE_LASTCALL);
			nType = NET_MSG_GCTRL_UPDATE_LASTCALL;
			MIN_STATIC_ASSERT(sizeof(SNETPC_UPDATE_LASTCALL)<=NET_DATA_BUFSIZE);			
		}
	};

	struct SNETPC_UPDATE_STARTCALL : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{	
		SNATIVEID			sStartMapID;		// 직전귀환 맵
		DWORD				dwStartGateID;		// 직전귀환 게이트
		D3DXVECTOR3			vStartPos;			// 직전귀환 위치


		SNETPC_UPDATE_STARTCALL () :
			dwStartGateID(0),
			vStartPos(0,0,0)
		{
			dwSize = sizeof(SNETPC_UPDATE_STARTCALL);
			nType = NET_MSG_GCTRL_UPDATE_STARTCALL;
			MIN_STATIC_ASSERT(sizeof(SNETPC_UPDATE_STARTCALL)<=NET_DATA_BUFSIZE);
		}
	};	

    //! 홀짝게임 결과 로그 Field->Cache
    struct NET_ODD_EVEN_LOG_FH : public NET_MSG_GENERIC
    {
        DWORD m_UserDbNum;
        DWORD m_CharDbNum;        
        __int64 m_BattingMoney; //! 배팅금액
        __int64 m_DividendMoney; //! 배당금액
        __int64 m_ChaMoney; //! 최종 캐릭터 소지금액
        BYTE  m_CurrentRound; //! 현재 라운드
        BYTE  m_WinLost; //! 승리 패배

        NET_ODD_EVEN_LOG_FH(
            DWORD UserDbNum,
            DWORD CharDbNum,
            BYTE  CurrentRound,
            BYTE  WinLost,
            __int64 BattingMoney,
            __int64 DividendMoney,
            __int64 ChaMoney)
            : m_UserDbNum(UserDbNum)
            , m_CharDbNum(CharDbNum)
            , m_CurrentRound(CurrentRound)
            , m_WinLost(WinLost)
            , m_BattingMoney(BattingMoney)
            , m_DividendMoney(DividendMoney)
            , m_ChaMoney(ChaMoney)
        {
            dwSize = sizeof(NET_ODD_EVEN_LOG_FH);
            nType = NET_MSG_ODD_EVEN_LOG_FH;
            MIN_STATIC_ASSERT(sizeof(NET_ODD_EVEN_LOG_FH)<=NET_DATA_BUFSIZE);
        }
    };

	struct SNETPC_SEND_SMS : public NET_MSG_GENERIC
	{
		

		DWORD			dwReceiveChaNum;
		TCHAR			szPhoneNumber[SMS_RECEIVER];
		TCHAR			szSmsMsg[SMS_LENGTH];

		WORD			wItemPosX;
		WORD			wItemPosY;

		SNETPC_SEND_SMS () :
			wItemPosX(0),
			wItemPosY(0),
			dwReceiveChaNum(0)
		{	
			dwSize = sizeof(SNETPC_SEND_SMS);
			nType = NET_MSG_SMS_SEND;
			MIN_STATIC_ASSERT(sizeof(SNETPC_SEND_SMS)<=NET_DATA_BUFSIZE);

			memset(szPhoneNumber, 0, sizeof(TCHAR) * (SMS_RECEIVER));
			memset(szSmsMsg, 0, sizeof(TCHAR) * (SMS_LENGTH));
		}
	};

	struct SNETPC_SEND_SMS_FB : public NET_MSG_GENERIC
	{
		

		EMSMS_SEND_FB			emFB;

		SNETPC_SEND_SMS_FB () :
			emFB(EMSMS_SEND_FB_FAIL)
		{
			dwSize = sizeof(SNETPC_SEND_SMS_FB);
			nType = NET_MSG_SMS_SEND_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_SEND_SMS_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_SEND_SMS_FROM_DB : public NET_MSG_GENERIC
	{
		

		EMSMS_SEND_FB			emFB;

		WORD					wPosX;
		WORD					wPosY;

		enum { EMMSG_SIZE = sizeof(NET_MSG_GENERIC)+sizeof(EMSMS_SEND_FB)+(sizeof(WORD)*2) };
		char				m_cBUFFER[NET_DATA_BUFSIZE-EMMSG_SIZE];

		SNETPC_SEND_SMS_FROM_DB () :
        	emFB(EMSMS_SEND_FROM_DB_FAIL),
			wPosX(0),
			wPosY(0)
		{
			dwSize = sizeof(SNETPC_SEND_SMS_FROM_DB);
			nType = NET_MSG_SMS_SEND_FROM_DB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_SEND_SMS_FROM_DB)<=NET_DATA_BUFSIZE);
		}
	};
/*
	struct SNETPC_PHONE_NUMBER : public NET_MSG_GENERIC
	{
		

		TCHAR					szPhoneNumber[SMS_RECEIVER];

		SNETPC_PHONE_NUMBER ()
		{			
			dwSize = sizeof(SNETPC_PHONE_NUMBER);
			nType = NET_MSG_SMS_PHONE_NUMBER;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PHONE_NUMBER)<=NET_DATA_BUFSIZE);

			memset(szPhoneNumber, 0, sizeof(TCHAR) * (SMS_RECEIVER));
		}
	};
*/
/*
	struct SNETPC_PHONE_NUMBER_FB : public NET_MSG_GENERIC
	{
		EMSMS_PHONE_NUMBER_FB	emFB;
		TCHAR					szPhoneNumber[SMS_RECEIVER];

		SNETPC_PHONE_NUMBER_FB () :
			emFB(EMSMS_PHONE_NUMBER_FB_FAIL)
		{			
			dwSize = sizeof(SNETPC_PHONE_NUMBER_FB);
			nType = NET_MSG_SMS_PHONE_NUMBER_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PHONE_NUMBER_FB)<=NET_DATA_BUFSIZE);

			memset(szPhoneNumber, 0, sizeof(TCHAR) * (SMS_RECEIVER));
		}
	};
*/
/*
	struct SNETPC_PHONE_NUMBER_FROM_DB : public NET_MSG_GENERIC
	{
		EMSMS_PHONE_NUMBER_FB		emFB;
		DWORD						dwCharID;
		TCHAR						szPhoneNumber[SMS_RECEIVER];

		enum { EMMSG_SIZE = sizeof(NET_MSG_GENERIC)+sizeof(EMSMS_PHONE_NUMBER_FB)+sizeof(DWORD)+(SMS_RECEIVER) };
		char				m_cBUFFER[NET_DATA_BUFSIZE-EMMSG_SIZE];

		SNETPC_PHONE_NUMBER_FROM_DB () :
			emFB(EMSMS_PHONE_NUMBER_FROM_DB_FAIL),
			dwCharID(0)
		{
			dwSize = sizeof(*this);			
			nType = NET_MSG_SMS_PHONE_NUMBER_FROM_DB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PHONE_NUMBER_FROM_DB)<=NET_DATA_BUFSIZE);

			memset ( szPhoneNumber, 0, sizeof(TCHAR)*SMS_RECEIVER );
		}
	};
*/
/*
	struct SNETPC_PHONE_NUMBER_AGTBRD : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		DWORD				dwID;
		TCHAR				szName[CHAR_SZNAME];
		TCHAR				szNewPhoneNumber[SMS_RECEIVER];

		enum { EMMSG_SIZE = sizeof(NET_MSG_CHARACTER)+sizeof(DWORD)+(SMS_RECEIVER)+(CHAR_SZNAME) };
		char				m_cBUFFER[NET_DATA_BUFSIZE-EMMSG_SIZE];

		SNETPC_PHONE_NUMBER_AGTBRD () :
			dwID(0)
		{			
			dwSize = sizeof(SNETPC_PHONE_NUMBER_AGTBRD);
			nType = NET_MSG_SMS_PHONE_NUMBER_AGTBRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_PHONE_NUMBER_AGTBRD)<=NET_DATA_BUFSIZE);

			memset ( szName, 0, sizeof(TCHAR)*CHAR_SZNAME );
			memset ( szNewPhoneNumber, 0, sizeof(TCHAR)*SMS_RECEIVER );
		}
	};
*/
	struct SNETPC_CHINA_GAINTYPE : public NET_MSG_GENERIC
	{
		

		BYTE				dwGainType;


		SNETPC_CHINA_GAINTYPE () :
		dwGainType(0)
		{
			dwSize = sizeof(SNETPC_CHINA_GAINTYPE);
			nType = NET_MSG_CHINA_GAINTYPE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_CHINA_GAINTYPE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_VIETNAM_GAINTYPE : public NET_MSG_GENERIC
	{
		

		BYTE				dwGainType;


		SNETPC_VIETNAM_GAINTYPE () :
		dwGainType(0)
		{
			dwSize = sizeof(SNETPC_VIETNAM_GAINTYPE);
			nType = NET_MSG_VIETNAM_GAINTYPE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_VIETNAM_GAINTYPE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_VIETNAM_TIME_REQ : public NET_MSG_GENERIC
	{
		
		DWORD				dwGaeaID;


		SNETPC_VIETNAM_TIME_REQ () :
								dwGaeaID(0)
		{
			dwSize = sizeof(SNETPC_VIETNAM_TIME_REQ);
			nType = NET_MSG_VIETNAM_TIME_REQ;
			MIN_STATIC_ASSERT(sizeof(SNETPC_VIETNAM_TIME_REQ)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_VIETNAM_TIME_REQ_FB : public NET_MSG_GENERIC
	{
				
		LONGLONG			gameTime;  //	저장된 누적 시간
		__time64_t			loginTime; //	로그인 시간



		SNETPC_VIETNAM_TIME_REQ_FB () :
							gameTime(0),
							loginTime(0)
		{
			dwSize = sizeof(SNETPC_VIETNAM_TIME_REQ_FB);
			nType = NET_MSG_VIETNAM_TIME_REQ_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_VIETNAM_TIME_REQ_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_VIETNAM_GAIN_EXP : public NET_MSG_GENERIC
	{
				
		LONGLONG			gainExp;   //	저장된 누적 경험치

		SNETPC_VIETNAM_GAIN_EXP () :
							gainExp(0)
		{
			dwSize = sizeof(SNETPC_VIETNAM_GAIN_EXP);
			nType = NET_MSG_VIETNAM_GAIN_EXP;
			MIN_STATIC_ASSERT(sizeof(SNETPC_VIETNAM_GAIN_EXP)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_VIETNAM_GAIN_MONEY : public NET_MSG_GENERIC
	{
				
		LONGLONG			gainMoney;   //	저장된 누적 돈

		SNETPC_VIETNAM_GAIN_MONEY () :
							gainMoney(0)
		{
			dwSize = sizeof(SNETPC_VIETNAM_GAIN_MONEY);
			nType = NET_MSG_VIETNAM_GAIN_MONEY;
			MIN_STATIC_ASSERT(sizeof(SNETPC_VIETNAM_GAIN_MONEY)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_VIETNAM_INVENGET : public NET_MSG_GENERIC
	{
		

		WORD					wPosX;
		WORD					wPosY;
		bool					bGetExp;

		SNETPC_INVEN_VIETNAM_INVENGET () :
			wPosX(0),
			wPosY(0),
			bGetExp(TRUE)
		{
			dwSize = sizeof(SNETPC_INVEN_VIETNAM_INVENGET);
			nType = NET_MSG_GCTRL_INVEN_VIETNAM_INVENGET;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_VIETNAM_INVENGET)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_VIETNAM_EXPGET_FB : public NET_MSG_GENERIC
	{
			

		LONGLONG				lnVnMoney;

		SNETPC_INVEN_VIETNAM_EXPGET_FB () 
			: lnVnMoney ( 0 )
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_INVEN_VIETNAM_EXPGET_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_VIETNAM_EXPGET_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_VIETNAM_ITEMGETNUM_UPDATE : public NET_MSG_GENERIC
	{
		

		DWORD					dwVietnamInvenCount;
		

		SNETPC_INVEN_VIETNAM_ITEMGETNUM_UPDATE () :
			dwVietnamInvenCount(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_INVEN_VIETNAM_ITEMGETNUM_UPDATE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_VIETNAM_ITEMGETNUM_UPDATE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_INVEN_VIETNAM_ITEMGET_FB : public NET_MSG_GENERIC
	{
			
		DWORD					dwVietnamInvenCount;

		SNETPC_INVEN_VIETNAM_ITEMGET_FB () :
			dwVietnamInvenCount(0)

		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_INVEN_VIETNAM_ITEMGET_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_INVEN_VIETNAM_ITEMGET_FB)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNETPC_VIETNAM_ALLINITTIME : public NET_MSG_GENERIC
	{
		
		__time64_t		initTime;

		SNETPC_VIETNAM_ALLINITTIME () 
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_VIETNAM_ALLINITTIME;
			MIN_STATIC_ASSERT(sizeof(SNETPC_VIETNAM_ALLINITTIME)<=NET_DATA_BUFSIZE);
		}
	};


//vietnamtest%%% && vietnamtest2
	struct SNETPC_VIETNAM_TIME_RESET : public NET_MSG_GENERIC
	{
		

		DWORD			dwGaeaID;

		SNETPC_VIETNAM_TIME_RESET () 
		: dwGaeaID ( 0 )
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_VIETNAM_TIME_RESET;
			MIN_STATIC_ASSERT(sizeof(SNETPC_VIETNAM_TIME_RESET)<=NET_DATA_BUFSIZE);
		}
	};

    //! Client->Field 탈것 탑승/하차 요청
	struct NET_CHAR_ACTIVE_VEHICLE_CF : public NET_MSG_GENERIC
	{
		bool bActive;
		NET_CHAR_ACTIVE_VEHICLE_CF(bool _bActive)
            : bActive(_bActive) // false
		{
			dwSize = sizeof(NET_CHAR_ACTIVE_VEHICLE_CF);
			nType = NET_MSG_CHAR_ACTIVE_VEHICLE_CF;
			MIN_STATIC_ASSERT(sizeof(NET_CHAR_ACTIVE_VEHICLE_CF)<=NET_DATA_BUFSIZE);
		}	
	};

	struct NET_CHAR_ACTIVE_VEHICLE_FC : public NET_MSG_GENERIC
	{
		EMVEHICLE_SET_FB	emFB;
        bool				bActive;
		bool				bLeaveFieldServer;
		DWORD				dwClientID;

		NET_CHAR_ACTIVE_VEHICLE_FC()
            : emFB(EMVEHICLE_SET_FB_FAIL)
            , bActive(false)
            , bLeaveFieldServer(false)
            , dwClientID(0)
		{
			dwSize = sizeof(NET_CHAR_ACTIVE_VEHICLE_FC);
			nType = NET_MSG_CHAR_ACTIVE_VEHICLE_FC;
			MIN_STATIC_ASSERT(sizeof(NET_CHAR_ACTIVE_VEHICLE_FC)<=NET_DATA_BUFSIZE);
		}	
	};

	struct SNETPC_ACTIVE_VEHICLE_BRD : public SNETPC_BROAD
	{
		bool				bActive;
		DWORD				dwPassengerID[MAX_PASSENGER_COUNT];
		

		SNETPC_ACTIVE_VEHICLE_BRD () :
		bActive(0)		
		{
			for( int i = 0; i < MAX_PASSENGER_COUNT; i++ )
				dwPassengerID[ i ] = -1;

			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_ACTIVE_VEHICLE_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_ACTIVE_VEHICLE_BRD)<=NET_DATA_BUFSIZE);
		}	
	};

	struct SNETPC_GET_VEHICLE : public NET_MSG_CHARACTER
	{		
		SNATIVEID m_ItemID;

		SNETPC_GET_VEHICLE()
            : m_ItemID(false)
		{
            MIN_STATIC_ASSERT(sizeof(SNETPC_GET_VEHICLE)<=NET_DATA_BUFSIZE);

			dwSize = sizeof(SNETPC_GET_VEHICLE);
			nType = NET_MSG_GCTRL_GET_VEHICLE;			
		}	
	};

	struct SNETPC_UNGET_VEHICLE_FB : public NET_MSG_GENERIC
	{
		SNETPC_UNGET_VEHICLE_FB()
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_UNGET_VEHICLE_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_UNGET_VEHICLE_FB)<=NET_DATA_BUFSIZE);
		}	
	};

	struct SNETPC_UNGET_VEHICLE_BRD	 : public SNETPC_BROAD
	{
		SNETPC_UNGET_VEHICLE_BRD ()
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_UNGET_VEHICLE_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_UNGET_VEHICLE_BRD)<=NET_DATA_BUFSIZE);
		}	
	};

	/*
	struct SNETPC_GET_VEHICLE_FB : public NET_MSG_GENERIC
	{		
		VEHICLE_TYPE		emTYPE;
		DWORD				m_DbNum;		// 탈것의 GUID (DB 번호)
		DWORD				m_OwnerDbNum;	// 주인 Char (DB 번호)
		SNATIVEID			sVehicleID;		// 소환을 요청한 탈것 아이템의 Mid/Sid
		int					nFull;
		DWORD				dwBoosterID;
		SITEMCUSTOM			PutOnItems[ACCE_TYPE_SIZE];
		SVEHICLE_COLOR		sVehicleColor[ACCE_TYPE_SIZE];

		EMVEHICLE_GET_FB	emFB;

		SNETPC_GET_VEHICLE_FB()
            : emTYPE( VEHICLE_TYPE_NONE )
            , m_DbNum ( UINT_MAX )
            , m_OwnerDbNum ( 0 )
            , sVehicleID ( NATIVEID_NULL() )
            , nFull ( 0 )
            , dwBoosterID( 0 )
            , emFB( EMVEHICLE_GET_FB_FAIL )
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_GET_VEHICLE_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_GET_VEHICLE_FB)<=NET_DATA_BUFSIZE);
		}	
	};

	struct SNETPC_GET_VEHICLE_BRD : public SNETPC_BROAD
	{
		DWORD				m_VehicleDbNum; //! 탈것의 GUID (DB 번호)
		VEHICLE_TYPE		emTYPE;
		SNATIVEID			sVehicleID;				// 소환을 요청한 탈것 아이템의 Mid/Sid

		SITEMCUSTOM			PutOnItems[ACCE_TYPE_SIZE];
		SVEHICLE_COLOR		sVehicleColor[ACCE_TYPE_SIZE];


		SNETPC_GET_VEHICLE_BRD () : m_VehicleDbNum ( 0 ), emTYPE ( VEHICLE_TYPE_NONE ), sVehicleID ( NATIVEID_NULL() )

		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_GET_VEHICLE_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_GET_VEHICLE_BRD)<=NET_DATA_BUFSIZE);
		}
	};
	*/

	struct SNETPC_GET_VEHICLE_FB : public NET_MSG_GENERIC
	{
		// PutOnItems, sVeghicleColor 은 ACCE_TYPE_SIZE 만큼 보낸다.
		struct sinfo
		{
			VEHICLE_TYPE		emTYPE;
			DWORD				m_DbNum;		// 탈것의 GUID (DB 번호)
			DWORD				m_OwnerDbNum;	// 주인 Char (DB 번호)
			SNATIVEID			sVehicleID;		// 소환을 요청한 탈것 아이템의 Mid/Sid
			int					nFull;
			DWORD				dwBoosterID;

			sinfo() : emTYPE( VEHICLE_TYPE_NONE ), m_DbNum ( UINT_MAX ), m_OwnerDbNum ( 0 ), sVehicleID ( NATIVEID_NULL() ), nFull ( 0 ), dwBoosterID( 0 )  {}
		};
		
		sinfo				basicinfo;

		SITEMCUSTOM			PutOnItems;
		SVEHICLE_COLOR		sVehicleColor;
		BYTE				iIndex;

		EMVEHICLE_GET_FB	emFB;

		SNETPC_GET_VEHICLE_FB()
			: emFB( EMVEHICLE_GET_FB_FAIL ), iIndex(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_GET_VEHICLE_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_GET_VEHICLE_FB)<=NET_DATA_BUFSIZE);
		}	
	};

	struct SNETPC_GET_VEHICLE_BRD : public SNETPC_BROAD
	{
		// PutOnItems, sVeghicleColor 은 ACCE_TYPE_SIZE 만큼 보낸다.
		struct sInfo
		{
			DWORD				m_VehicleDbNum; //! 탈것의 GUID (DB 번호)
			VEHICLE_TYPE		emTYPE;
			SNATIVEID			sVehicleID;				// 소환을 요청한 탈것 아이템의 Mid/Sid

			sInfo() : m_VehicleDbNum ( 0 ), emTYPE ( VEHICLE_TYPE_NONE ), sVehicleID ( NATIVEID_NULL() ) {}
		};

		sInfo				basisInfo;
		SITEMCUSTOM			PutOnItems;
		SVEHICLE_COLOR		sVehicleColor;

		EMVEHICLE_BROAD		eInfoState;
		BYTE				iIndex;


		SNETPC_GET_VEHICLE_BRD () : iIndex(0), eInfoState(EMVEHICLE_BROAD_OK)
		
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_GET_VEHICLE_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_GET_VEHICLE_BRD)<=NET_DATA_BUFSIZE);
		}
	};

    //! DB -> Field
    struct SNET_VEHICLE_CREATE_FROMDB_FB : public NET_MSG_GENERIC
	{
		EMVEHICLE_CREATE_FROMDB_FB emFB;
		VEHICLE_TYPE emTYPE;
		SNATIVEID	 sVehicleID;	 // 정보 요청한 ITEM ID
		DWORD		 m_VehicleDbNum; // 탈것의 고유ID
        DWORD        m_ChaDbNum; // 캐릭터 DB 번호

		enum 
        { 
            EMMSG_SIZE = sizeof(NET_MSG_GENERIC)+sizeof(EMVEHICLE_CREATE_FROMDB_FB)+sizeof(VEHICLE_TYPE)+sizeof(SNATIVEID)+sizeof(DWORD)+sizeof(DWORD),
        };
		char m_cBUFFER[NET_DATA_BUFSIZE-EMMSG_SIZE];

		SNET_VEHICLE_CREATE_FROMDB_FB()
            : emFB(EMVEHICLE_CREATE_FROMDB_FB_FAIL)
            , emTYPE(VEHICLE_TYPE_NONE)
            , sVehicleID(NATIVEID_NULL())
            , m_VehicleDbNum(0)
            , m_ChaDbNum(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_VEHICLE_CREATE_FROMDB_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_VEHICLE_CREATE_FROMDB_FB)<=NET_DATA_BUFSIZE);
		}
	};
        
	struct SNET_VEHICLE_GET_FROMDB_FB : public NET_MSG_GENERIC
	{
		GLVEHICLE* pVehicle;
		DWORD	   m_VehicleDbNum;			// 소환 요청의 탈것의 고유 ID
		bool	   bLMTItemCheck;
		bool	   bCardInfo;
		bool	   bTrade;

		enum {
            EMMSG_SIZE = sizeof(NET_MSG_GENERIC)+sizeof(PGLVEHICLE)+sizeof(DWORD)+sizeof(bool)*3,
        };

		BYTE m_cBUFFER[NET_DATA_BUFSIZE-EMMSG_SIZE];

		SNET_VEHICLE_GET_FROMDB_FB()
            : pVehicle ( NULL )
            , m_VehicleDbNum ( UINT_MAX )
            , bLMTItemCheck( false )
            , bCardInfo( false )
            , bTrade ( false )
		{
			dwSize = sizeof(SNET_VEHICLE_GET_FROMDB_FB);
			nType = NET_MSG_VEHICLE_GET_FROMDB_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_VEHICLE_GET_FROMDB_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_VEHICLE_GET_FROMDB_ERROR : public NET_MSG_GENERIC
	{
		EMVEHICLE_GET_FROMDB_ERROR_FB emFB;
		
		bool				bLMTItemCheck;
		bool				bCardInfo;
		bool				bTrade;

		enum { EMMSG_SIZE = sizeof(NET_MSG_GENERIC)+sizeof(EMVEHICLE_GET_FROMDB_ERROR_FB)+sizeof(bool)*3 };
		char				m_cBUFFER[NET_DATA_BUFSIZE-EMMSG_SIZE];

		SNET_VEHICLE_GET_FROMDB_ERROR()
            : emFB( EMVEHICLE_GET_FROMDB_ERROR_FB_DBFAIL )
            , bLMTItemCheck( false )
            , bCardInfo( false )
            , bTrade ( false )
		{
			dwSize = sizeof(SNET_VEHICLE_GET_FROMDB_ERROR);
			nType = NET_MSG_VEHICLE_GET_FROMDB_ERROR;
			MIN_STATIC_ASSERT(sizeof(SNET_VEHICLE_GET_FROMDB_ERROR)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_VEHICLE_ACCESSORY_DELETE : public NET_MSG_GENERIC
	{
		
		
		VEHICLE_ACCESSORYTYPE	accetype;
		DWORD					m_VehicleDbNum; //! 소환 요청의 탈것의 고유 ID (DB 번호)

		SNET_VEHICLE_ACCESSORY_DELETE () :
		accetype ( ACCE_TYPE_SIZE ),
		m_VehicleDbNum ( 0 )
		{
			dwSize = sizeof(SNET_VEHICLE_ACCESSORY_DELETE);
			nType = NET_MSG_VEHICLE_ACCESSORY_DELETE;
			MIN_STATIC_ASSERT(sizeof(SNET_VEHICLE_ACCESSORY_DELETE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_VEHICLE_ACCESSORY_DELETE_BRD : public SNETPC_BROAD
	{

		DWORD m_VehicleDbNum;
		VEHICLE_ACCESSORYTYPE accetype;

		SNET_VEHICLE_ACCESSORY_DELETE_BRD () :
		m_VehicleDbNum ( 0 ),
		accetype ( ACCE_TYPE_SIZE )
		{
			dwSize = sizeof(SNET_VEHICLE_ACCESSORY_DELETE_BRD);
			nType = NET_MSG_VEHICLE_ACCESSORY_DELETE_BRD;
			MIN_STATIC_ASSERT(sizeof(SNET_VEHICLE_ACCESSORY_DELETE_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_VEHICLE_REQ_INVEN_EX_SLOT : public NET_MSG_GENERIC
	{
		

		EMSUIT				emSuit;

        WORD                wPosX;
        WORD                wPosY;

		SNET_VEHICLE_REQ_INVEN_EX_SLOT () 
			: emSuit(SUIT_NSIZE)
            , wPosX(0)
            , wPosY(0)
		{
			dwSize = sizeof(SNET_VEHICLE_REQ_INVEN_EX_SLOT);
			nType = NET_MSG_VEHICLE_REQ_INVEN_EX_SLOT;
			MIN_STATIC_ASSERT(sizeof(SNET_VEHICLE_REQ_INVEN_EX_SLOT)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_VEHICLE_REQ_SLOT_EX_HOLD_FB : public NET_MSG_GENERIC
	{
		

		EMVEHICLE_REQ_SLOT_EX_HOLD_FB emFB;
        SITEMCUSTOM sHoldtoSlotItemCustom;
		EMSUIT					  emSuit;

		SNET_VEHICLE_REQ_SLOT_EX_HOLD_FB () 
			: emSuit(SUIT_NSIZE)
			, emFB(EMVEHICLE_REQ_SLOT_EX_HOLD_FB_FAIL)
		{
			dwSize = sizeof(SNET_VEHICLE_REQ_SLOT_EX_HOLD_FB);
			nType = NET_MSG_VEHICLE_REQ_SLOT_EX_HOLD_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_VEHICLE_REQ_SLOT_EX_HOLD_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_VEHICLE_REQ_SLOT_EX_HOLD_BRD : public SNETPC_BROAD
	{
		SITEMCUSTOM			sItemCustom;
		EMSUIT				emSuit;

		SNET_VEHICLE_REQ_SLOT_EX_HOLD_BRD () 
			: emSuit(SUIT_NSIZE)
		{
			dwSize = sizeof(SNET_VEHICLE_REQ_SLOT_EX_HOLD_BRD);
			nType = NET_MSG_VEHICLE_REQ_SLOT_EX_HOLD_BRD;
			MIN_STATIC_ASSERT(sizeof(SNET_VEHICLE_REQ_SLOT_EX_HOLD_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_VEHICLE_REQ_INVEN_TO_SLOT : public NET_MSG_GENERIC
	{
		

		EMSUIT				emSuit;

        WORD                wHoldPosX;
        WORD                wHoldPosY;

		SNET_VEHICLE_REQ_INVEN_TO_SLOT () 
			: emSuit(SUIT_NSIZE)
            , wHoldPosX(0)
            , wHoldPosY(0)
		{
			dwSize = sizeof(SNET_VEHICLE_REQ_INVEN_TO_SLOT);
			nType = NET_MSG_VEHICLE_REQ_INVEN_TO_SLOT;
			MIN_STATIC_ASSERT(sizeof(SNET_VEHICLE_REQ_INVEN_TO_SLOT)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_VEHICLE_REQ_HOLD_TO_SLOT_FB : public NET_MSG_GENERIC
	{
		

		EMVEHICLE_REQ_SLOT_EX_HOLD_FB emFB;
		SITEMCUSTOM				  sItemCustom;
		EMSUIT					  emSuit;

		SNET_VEHICLE_REQ_HOLD_TO_SLOT_FB () 
			: emSuit(SUIT_NSIZE)
			, emFB(EMVEHICLE_REQ_SLOT_EX_HOLD_FB_FAIL)
		{
			dwSize = sizeof(SNET_VEHICLE_REQ_HOLD_TO_SLOT_FB);
			nType = NET_MSG_VEHICLE_REQ_HOLD_TO_SLOT_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_VEHICLE_REQ_HOLD_TO_SLOT_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_VEHICLE_REQ_SLOT_TO_INVEN : public NET_MSG_GENERIC
	{
		EMSUIT				emSuit;

        WORD                wPosX;
        WORD                wPosY;

		SNET_VEHICLE_REQ_SLOT_TO_INVEN() 
			: emSuit(SUIT_NSIZE)
            , wPosX(0)
            , wPosY(0)
		{
			dwSize = sizeof(SNET_VEHICLE_REQ_SLOT_TO_INVEN);
			nType = NET_MSG_VEHICLE_REQ_SLOT_TO_INVEN;
			MIN_STATIC_ASSERT(sizeof(SNET_VEHICLE_REQ_SLOT_TO_INVEN)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_VEHICLE_REQ_SLOT_TO_HOLD_FB : public NET_MSG_GENERIC
	{
		

		EMVEHICLE_REQ_SLOT_EX_HOLD_FB emFB;
		SITEMCUSTOM				  sItemCustom;
		EMSUIT					  emSuit;

		SNET_VEHICLE_REQ_SLOT_TO_HOLD_FB () 
			: emSuit(SUIT_NSIZE)
			, emFB(EMVEHICLE_REQ_SLOT_EX_HOLD_FB_FAIL)
		{
			dwSize = sizeof(SNET_VEHICLE_REQ_SLOT_TO_HOLD_FB);
			nType = NET_MSG_VEHICLE_REQ_SLOT_TO_HOLD_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_VEHICLE_REQ_SLOT_TO_HOLD_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_VEHICLE_REQ_REMOVE_SLOTITEM : public NET_MSG_GENERIC
	{
		

		EMSUIT				emSuit;

		SNET_VEHICLE_REQ_REMOVE_SLOTITEM () 
			: emSuit(SUIT_NSIZE)
		{
			dwSize = sizeof(SNET_VEHICLE_REQ_REMOVE_SLOTITEM);
			nType = NET_MSG_VEHICLE_REMOVE_SLOTITEM;
			MIN_STATIC_ASSERT(sizeof(SNET_VEHICLE_REQ_REMOVE_SLOTITEM)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_VEHICLE_REQ_REMOVE_SLOTITEM_FB : public NET_MSG_GENERIC
	{
		

		EMSUIT				emSuit;

		SNET_VEHICLE_REQ_REMOVE_SLOTITEM_FB () 
			: emSuit(SUIT_NSIZE)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_VEHICLE_REMOVE_SLOTITEM_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_VEHICLE_REQ_REMOVE_SLOTITEM_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_VEHICLE_REQ_REMOVE_SLOTITEM_BRD : public SNETPC_BROAD
	{
		EMSUIT			emSuit;

		SNET_VEHICLE_REQ_REMOVE_SLOTITEM_BRD ()
			: emSuit(SUIT_NSIZE)
		{
			dwSize = sizeof(SNET_VEHICLE_REQ_REMOVE_SLOTITEM_BRD);
			nType = NET_MSG_VEHICLE_REMOVE_SLOTITEM_BRD;
			MIN_STATIC_ASSERT(sizeof(SNET_VEHICLE_REQ_REMOVE_SLOTITEM_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_VEHICLE_UPDATE_CLIENT_BATTERY : public NET_MSG_GENERIC
	{
		

		int					nFull;

		SNET_VEHICLE_UPDATE_CLIENT_BATTERY () 
			: nFull(0)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_VEHICLE_UPDATE_CLIENT_BATTERY;
			MIN_STATIC_ASSERT(sizeof(SNET_VEHICLE_UPDATE_CLIENT_BATTERY)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_VEHICLE_REQ_GIVE_BATTERY : public NET_MSG_GENERIC
	{
		WORD				wPosX;
		WORD				wPosY;
        
        WORD                wHoldPosX;
        WORD                wHoldPosY;

		SNET_VEHICLE_REQ_GIVE_BATTERY () 
			: wPosX(0)
			, wPosY(0)
            , wHoldPosX(0)
            , wHoldPosY(0)
		{
			dwSize = sizeof(SNET_VEHICLE_REQ_GIVE_BATTERY);
			nType = NET_MSG_VEHICLE_REQ_GIVE_BATTERY;
			MIN_STATIC_ASSERT(sizeof(SNET_VEHICLE_REQ_GIVE_BATTERY)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_VEHICLE_REQ_GIVE_BATTERY_FB : public NET_MSG_GENERIC
	{
		

		DWORD						dwVehicleID;
		SNATIVEID					sItemID;
		SNATIVEID					sBatteryID;
		int							nFull;

		EMVEHICLE_REQ_GIVE_BATTERY_FB		emFB;

		SNET_VEHICLE_REQ_GIVE_BATTERY_FB () :
			dwVehicleID(0),
			sItemID(NATIVEID_NULL()),
			sBatteryID(NATIVEID_NULL()),
			nFull(0),
			emFB(EMVEHICLE_REQ_GIVE_BATTERY_FB_FAIL)
		{
			dwSize = sizeof(SNET_VEHICLE_REQ_GIVE_BATTERY_FB);
			nType = NET_MSG_VEHICLE_REQ_GIVE_BATTERY_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_VEHICLE_REQ_GIVE_BATTERY_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_VEHICLE_REQ_GET_BATTERY_FROMDB_FB : public NET_MSG_GENERIC
	{
		DWORD							m_VehicleDbNum;
		SNATIVEID						sItemID;
		int								nFull;
		WORD							wPosX;
		WORD							wPosY;
		WORD							wCureVolume;
		BOOL							bRatio;
		SNATIVEID						sBatteryID;
		EMVEHICLE_REQ_GIVE_BATTERY_FB	emFB;
		VEHICLE_TYPE					emType;
        WORD                            wHoldPosX;
        WORD                            wHoldPosY;

		enum { EMMSG_SIZE = sizeof(NET_MSG_GENERIC)+sizeof(DWORD)+sizeof(SNATIVEID)+sizeof(int)+sizeof(WORD)*3+sizeof(BOOL)+sizeof(SNATIVEID)+sizeof(EMVEHICLE_REQ_GIVE_BATTERY_FB)+sizeof(VEHICLE_TYPE)+sizeof(WORD)*2 };
		char				m_cBUFFER[NET_DATA_BUFSIZE-EMMSG_SIZE];

		SNET_VEHICLE_REQ_GET_BATTERY_FROMDB_FB() :
			m_VehicleDbNum(0),
			sItemID( NATIVEID_NULL() ),
			nFull(0),
			wPosX(0),
			wPosY(0),
			wCureVolume(0),
			bRatio(false),
			sBatteryID(NATIVEID_NULL()),
			emFB(EMVEHICLE_REQ_GIVE_BATTERY_FB_FAIL),
			emType( VEHICLE_TYPE_NONE ),
            wHoldPosX(0),
            wHoldPosY(0)
		{
			dwSize = sizeof(SNET_VEHICLE_REQ_GET_BATTERY_FROMDB_FB);
			nType = NET_MSG_VEHICLE_REQ_GET_BATTERY_FROMDB_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_VEHICLE_REQ_GET_BATTERY_FROMDB_FB)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_VEHICLE_REQ_ITEM_INFO : public NET_MSG_GENERIC
	{
		DWORD				dwVehicleID;
		bool				bTrade;

		SNET_VEHICLE_REQ_ITEM_INFO () :
			dwVehicleID(0),
			bTrade(false)
		{
			dwSize = sizeof(SNET_VEHICLE_REQ_ITEM_INFO);
			nType = NET_MSG_VEHICLE_REQ_ITEM_INFO;
			MIN_STATIC_ASSERT(sizeof(SNET_VEHICLE_REQ_ITEM_INFO)<=NET_DATA_BUFSIZE);
		}
	};
/*
	struct SNET_VEHICLE_REQ_ITEM_INFO_FB : public NET_MSG_GENERIC
	{
		VEHICLE_TYPE        emTYPE;
		int					nFull;
		SITEMCUSTOM			PutOnItems[ACCE_TYPE_SIZE];
		SVEHICLE_COLOR		sVehicleColor[ACCE_TYPE_SIZE];
		DWORD				m_VehicleDbNum;
		DWORD				dwBoosterID;
		bool				bTrade;

		SNET_VEHICLE_REQ_ITEM_INFO_FB () :
			emTYPE(VEHICLE_TYPE_NONE),
			nFull(0),
			m_VehicleDbNum(0),
			dwBoosterID(0),
			bTrade(false)
		{
			dwSize = sizeof(SNET_VEHICLE_REQ_ITEM_INFO_FB);
			nType = NET_MSG_VEHICLE_REQ_ITEM_INFO_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_VEHICLE_REQ_ITEM_INFO_FB)<=NET_DATA_BUFSIZE);
		}
	};
*/
	struct SNET_VEHICLE_REQ_ITEM_INFO_FB : public NET_MSG_GENERIC
	{
		
		int					nFull;

		DWORD				dwBoosterID;
		bool				bTrade;	
		VEHICLE_TYPE        emTYPE;
		DWORD				m_VehicleDbNum;

		BYTE				iIndex;
		EMVEHICLE_BROAD     eStatus;
		SITEMCUSTOM			PutOnItems;
		SVEHICLE_COLOR		sVehicleColor;

		//SITEMCUSTOM			PutOnItems[ACCE_TYPE_SIZE];
		//SVEHICLE_COLOR		sVehicleColor[ACCE_TYPE_SIZE];

		SNET_VEHICLE_REQ_ITEM_INFO_FB () : iIndex(0), eStatus(EMVEHICLE_BROAD_OK), emTYPE(VEHICLE_TYPE_NONE), m_VehicleDbNum(0), nFull(0), dwBoosterID(0), bTrade(false)

		{
			dwSize = sizeof(SNET_VEHICLE_REQ_ITEM_INFO_FB);
			nType = NET_MSG_VEHICLE_REQ_ITEM_INFO_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_VEHICLE_REQ_ITEM_INFO_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_WITH_VEHICLE : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		DWORD				dwTargetID;

		SNET_WITH_VEHICLE () :
		dwTargetID(0)
		{
			dwSize = sizeof(SNET_WITH_VEHICLE);
			nType = NET_MSG_GCTRL_WITH_VEHICLE;
			MIN_STATIC_ASSERT(sizeof(SNET_WITH_VEHICLE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_WITH_VEHICLE_TAR : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		DWORD				dwMasterID;

		SNET_WITH_VEHICLE_TAR () :
		dwMasterID(0)
		{
			dwSize = sizeof(SNET_WITH_VEHICLE_TAR);
			nType = NET_MSG_GCTRL_WITH_VEHICLE_TAR;
			MIN_STATIC_ASSERT(sizeof(SNET_WITH_VEHICLE_TAR)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_WITH_VEHICLE_TAR_ANS : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		DWORD				dwMasterID;
		EMWITH_VEHICLE_ANS	emAns;

		SNET_WITH_VEHICLE_TAR_ANS () :
		dwMasterID(0),
		emAns(EMWITH_VEHICLE_CANCEL)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_WITH_VEHICLE_TAR_ANS;
			MIN_STATIC_ASSERT(sizeof(SNET_WITH_VEHICLE_TAR_ANS)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_WITH_VEHICLE_FB : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{
		DWORD				dwTargetID;
		EMWITH_VEHICLE_ANS	emAns;

		SNET_WITH_VEHICLE_FB () :
		dwTargetID(0),
		emAns(EMWITH_VEHICLE_CANCEL)
		{
			dwSize = sizeof(SNET_WITH_VEHICLE_FB);
			nType = NET_MSG_GCTRL_WITH_VEHICLE_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_WITH_VEHICLE_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_VEHICLE_BRD : public SNETPC_BROAD
	{
		EMVEHICLE_WHETHER	emVehicle;
		CLIENT_VEHICLE		sVehicle;

		SNETPC_VEHICLE_BRD ()
		{
			dwSize = sizeof(SNETPC_VEHICLE_BRD);
			nType = NET_MSG_GCTRL_VEHICLE_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_VEHICLE_BRD)<=NET_DATA_BUFSIZE);
		}	
	};

	struct SNETPC_ACTIVE_WITH_VEHICLE_BRD : public SNETPC_BROAD
	{		
		DWORD				dwPassengerID[MAX_PASSENGER_COUNT];
		EMVEHICLE_WHETHER	emVehicle;
        bool				bActive;

		SNETPC_ACTIVE_WITH_VEHICLE_BRD()
            : bActive(0)
            , emVehicle( EMVEHICLE_OFF )
		{
			dwSize = sizeof(SNETPC_ACTIVE_WITH_VEHICLE_BRD);
			nType = NET_MSG_GCTRL_ACTIVE_WITH_VEHICLE_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_ACTIVE_WITH_VEHICLE_BRD)<=NET_DATA_BUFSIZE);
            
            for (int i=0; i<MAX_PASSENGER_COUNT; ++i)
                dwPassengerID[i] = UINT_MAX;
		}	
	};

	struct SNET_ACTIVE_WITH_VEHICLE_FB : public NET_MSG_GENERIC
	{		
		DWORD		dwPassengerID[MAX_PASSENGER_COUNT];
        bool		bActive;

		SNET_ACTIVE_WITH_VEHICLE_FB()
            : bActive(false)
		{
			dwSize = sizeof(SNET_ACTIVE_WITH_VEHICLE_FB);
			nType = NET_MSG_GCTRL_ACTIVE_WITH_VEHICLE_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_ACTIVE_WITH_VEHICLE_FB)<=NET_DATA_BUFSIZE);

            for (int i=0; i<MAX_PASSENGER_COUNT; ++i)
                dwPassengerID[i] = UINT_MAX;
		}
	};

	struct SNETPC_MOVE_MAP_WITH_VEHICLE : public NET_MSG_GENERIC
	{	
		bool				bMove;
		DWORD			dwGateToIndex;
		

		SNETPC_MOVE_MAP_WITH_VEHICLE ()
			: bMove( false )
			, dwGateToIndex(0)
		{
			dwSize = sizeof(SNETPC_MOVE_MAP_WITH_VEHICLE);
			nType = NET_MSG_GCTRL_WITH_VEHICLE_MOVE_MAP;
			MIN_STATIC_ASSERT(sizeof(SNETPC_MOVE_MAP_WITH_VEHICLE)<=NET_DATA_BUFSIZE);
		}	
	};

	struct SNET_GET_OFF_WITH_VEHICLE : public NET_MSG_CHARACTER//NET_MSG_GENERIC
	{	
		DWORD				dwGaeaID;
		BOOL				bDriver;

		SNET_GET_OFF_WITH_VEHICLE () :
		dwGaeaID( -1 ),
			bDriver( FALSE )
		{
			dwSize = sizeof(SNET_GET_OFF_WITH_VEHICLE);
			nType = NET_MSG_GCTRL_GET_OFF_WITH_VEHICLE;
			MIN_STATIC_ASSERT(sizeof(SNET_GET_OFF_WITH_VEHICLE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_FORCERESET_STAT_SKILL : public NET_MSG_CHARACTER
	{
		SNETPC_FORCERESET_STAT_SKILL ()
		{
			dwSize = sizeof( *this );
			nType = NET_MSG_GCTRL_FORCERESET_SKILL_STAT_FC;
			MIN_STATIC_ASSERT( sizeof( SNETPC_FORCERESET_STAT_SKILL ) <= NET_DATA_BUFSIZE );
		}
	};

	struct SNET_GET_OFF_WITH_VEHICLE_FB : public NET_MSG_GENERIC
	{	
		DWORD				dwGaeaID;
		BOOL				bDriver;

		SNET_GET_OFF_WITH_VEHICLE_FB () :
		dwGaeaID( -1 ),
			bDriver( FALSE )
		{
			dwSize = sizeof(SNET_GET_OFF_WITH_VEHICLE_FB);
			nType = NET_MSG_GCTRL_GET_OFF_WITH_VEHICLE_FB;
			MIN_STATIC_ASSERT(sizeof(SNET_GET_OFF_WITH_VEHICLE_FB)<=NET_DATA_BUFSIZE);
		}
	};



	struct SNET_QBOX_OPTION_REQ_AG : public NET_MSG_GENERIC
	{
		const DWORD dwGaeaID;
		const bool bQBoxEnable;

		SNET_QBOX_OPTION_REQ_AG(const DWORD _dwGaeaID, const bool _bEnable)
			:dwGaeaID(_dwGaeaID)
			, bQBoxEnable(_bEnable)
		{
			dwSize = sizeof(SNET_QBOX_OPTION_REQ_AG);
			nType = NET_QBOX_OPTION_REQ_AG;
			MIN_STATIC_ASSERT(sizeof(SNET_QBOX_OPTION_REQ_AG)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_OPEN_ITEMSHOP : public NET_MSG_GENERIC
	{
		bool				bOpen;

		SNETPC_OPEN_ITEMSHOP () :
		bOpen(0)
		{
			dwSize = sizeof(SNETPC_OPEN_ITEMSHOP);
			nType = NET_MSG_GCTRL_ITEMSHOPOPEN;
			MIN_STATIC_ASSERT(sizeof(SNETPC_OPEN_ITEMSHOP)<=NET_DATA_BUFSIZE);
		}	
	};

	struct SNETPC_OPEN_ITEMSHOP_BRD : public SNETPC_BROAD
	{
		bool				bOpen;			

		SNETPC_OPEN_ITEMSHOP_BRD () :
		bOpen(0)		
		{
			dwSize = sizeof(SNETPC_OPEN_ITEMSHOP_BRD);
			nType = NET_MSG_GCTRL_ITEMSHOPOPEN_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_OPEN_ITEMSHOP_BRD)<=NET_DATA_BUFSIZE);
		}	
	};

	struct SNET_QBOX_OPTION_REQ_FLD : public NET_MSG_GENERIC
	{
		const GLPartyID PartyID;
		const DWORD dwGaeaID;
		const bool bQBoxEnable;		

		SNET_QBOX_OPTION_REQ_FLD(const GLPartyID& _PartyID, const DWORD _dwGaeaID, const bool _bEnable)
			: PartyID(_PartyID)
			, dwGaeaID(_dwGaeaID)
			, bQBoxEnable(_bEnable)
		{
			dwSize = sizeof(SNET_QBOX_OPTION_REQ_FLD);
			nType = NET_QBOX_OPTION_REQ_FLD;
			MIN_STATIC_ASSERT(sizeof(SNET_QBOX_OPTION_REQ_FLD)<=NET_DATA_BUFSIZE);
		}
	};	

	struct SNETPC_REQ_ATTENDLIST : public NET_MSG_GENERIC
	{
		SNETPC_REQ_ATTENDLIST ()
		{
			dwSize = sizeof(SNETPC_REQ_ATTENDLIST);
			nType = NET_MSG_REQ_ATTENDLIST;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_ATTENDLIST)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_ATTENDLIST_FB : public NET_MSG_GENERIC
	{
		enum { EMMAXATTEND = 31 };

		
		DWORD				dwAttenNum;
		DWORD				dwAttendCombo;
		
		__time64_t			tLoginTime;
		DWORD				dwAttendTime;

		USER_ATTEND_INFO 	sAttend[EMMAXATTEND];

		SNETPC_REQ_ATTENDLIST_FB ()
			: dwAttenNum(0)
			, dwAttendCombo(0)
			, tLoginTime(0)
			, dwAttendTime(0)
		{
			
			dwSize = sizeof(NET_MSG_GENERIC) + sizeof(DWORD) + sizeof(DWORD) +
						 sizeof(__time64_t) + sizeof(DWORD);
			nType = NET_MSG_REQ_ATTENDLIST_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_ATTENDLIST_FB)<=NET_DATA_BUFSIZE);
		}		

		void SetSize( int nSize )
		{
			dwAttenNum = nSize;
			dwSize = sizeof(NET_MSG_GENERIC) + sizeof(DWORD) + sizeof(DWORD) +
						 sizeof(__time64_t) + sizeof(DWORD) +
						 sizeof(USER_ATTEND_INFO)*dwAttenNum ;
			//MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_ATTENDLIST_FB)<=NET_DATA_BUFSIZE);
		}

		void RESET ()
		{
			dwAttenNum = 0;
			dwAttendCombo = 0;
			dwSize = sizeof(NET_MSG_GENERIC) + sizeof(DWORD) + sizeof(DWORD) +
						 sizeof(__time64_t) + sizeof(DWORD);
		}
	};
	

	struct SNETPC_REQ_ATTENDANCE : public NET_MSG_GENERIC
	{
		SNETPC_REQ_ATTENDANCE ()
		{
			dwSize = sizeof(SNETPC_REQ_ATTENDANCE);
			nType = NET_MSG_REQ_ATTENDANCE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_ATTENDANCE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_ATTENDANCE_FB : public NET_MSG_GENERIC
	{	
		EMREQ_ATTEND_FB		emFB;

		__time64_t			tAttendTime;
		DWORD				dwComboAttend;
		bool				bAttendReward;

		SNETPC_REQ_ATTENDANCE_FB ()
			: emFB ( EMREQ_ATTEND_FB_FAIL )
			, tAttendTime ( 0 )
			, dwComboAttend ( 0 )
			, bAttendReward (false)
		{
			dwSize = sizeof(SNETPC_REQ_ATTENDANCE_FB);
			nType = NET_MSG_REQ_ATTENDANCE_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_ATTENDANCE_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_ATTEND_REWARD_CLT : public NET_MSG_GENERIC
	{	
		SNATIVEID			idAttendReward;

		SNETPC_REQ_ATTEND_REWARD_CLT ()
			: idAttendReward ( NATIVEID_NULL() )
		{
			dwSize = sizeof(SNETPC_REQ_ATTEND_REWARD_CLT);
			nType = NET_MSG_REQ_ATTEND_REWARD_CLT;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_ATTEND_REWARD_CLT)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNETPC_REQ_GATHERING : public NET_MSG_GENERIC
	{	
		DWORD				dwTargetID;

		SNETPC_REQ_GATHERING ()
			: dwTargetID(0)
		{
			dwSize = sizeof(SNETPC_REQ_GATHERING);
			nType = NET_MSG_REQ_GATHERING;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_GATHERING)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_GATHERING_FB : public NET_MSG_GENERIC
	{	
		EMREQ_GATHER_FB		emFB;
		DWORD				dwTime;
		DWORD				dwAniSel;
		DWORD				dwGaeaID;

		SNETPC_REQ_GATHERING_FB ()
			: emFB ( EMREQ_GATHER_FB_FAIL )
			, dwTime ( 0 )
			, dwGaeaID ( GAEAID_NULL )
			, dwAniSel ( 0 )
		{
			dwSize = sizeof(SNETPC_REQ_GATHERING_FB);
			nType = NET_MSG_REQ_GATHERING_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_GATHERING_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_GATHERING_BRD : public SNETPC_BROAD
	{
		DWORD				dwAniSel;
		
		SNETPC_REQ_GATHERING_BRD ()
		{
			dwSize = sizeof(SNETPC_REQ_GATHERING_BRD);
			nType = NET_MSG_REQ_GATHERING_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_GATHERING_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_GATHERING_RESULT : public NET_MSG_GENERIC
	{	
		EMREQ_GATHER_RESULT	emFB;

		SNETPC_REQ_GATHERING_RESULT ()
			: emFB ( EMREQ_GATHER_RESULT_FAIL )			
		{
			dwSize = sizeof(SNETPC_REQ_GATHERING_RESULT);
			nType = NET_MSG_REQ_GATHERING_RESULT;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_GATHERING_RESULT)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_GATHERING_RESULT_BRD : public SNETPC_BROAD
	{
		EMREQ_GATHER_RESULT	emFB;

		SNETPC_REQ_GATHERING_RESULT_BRD ()
			: emFB ( EMREQ_GATHER_RESULT_FAIL )			
		{
			dwSize = sizeof(SNETPC_REQ_GATHERING_RESULT_BRD);
			nType = NET_MSG_REQ_GATHERING_RESULT_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_GATHERING_RESULT_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_GATHERING_CANCEL : public NET_MSG_GENERIC
	{
		SNETPC_REQ_GATHERING_CANCEL ()
		{
			dwSize = sizeof(SNETPC_REQ_GATHERING_CANCEL);
			nType = NET_MSG_REQ_GATHERING_CANCEL;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_GATHERING_CANCEL)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_GATHERING_CANCEL_BRD : public SNETPC_BROAD
	{
		SNETPC_REQ_GATHERING_CANCEL_BRD ()
		{
			dwSize = sizeof(SNETPC_REQ_GATHERING_CANCEL_BRD);
			nType = NET_MSG_REQ_GATHERING_CANCEL_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_GATHERING_CANCEL_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_LANDEFFECT_UPDATE_BRD : public SNETPC_BROAD
	{
		bool bADD;

		SLANDEFFECT_VALUE landEffect;

		SNETPC_LANDEFFECT_UPDATE_BRD ()
			: bADD( false )
		{
			dwSize = sizeof(SNETPC_LANDEFFECT_UPDATE_BRD);
			nType = NET_MSG_GCTRL_LANDEFFECT_UPDATE_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_LANDEFFECT_UPDATE_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_MAPEFFECT_UPDATE_BRD : public SNETPC_BROAD
	{
		bool bADD;

		SMAP_EFFECT sMapEffect;

		SNETPC_MAPEFFECT_UPDATE_BRD ()
			: bADD( false )
		{
			dwSize = sizeof(SNETPC_MAPEFFECT_UPDATE_BRD);
			nType = NET_MSG_GCTRL_MAPEFFECT_UPDATE_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_MAPEFFECT_UPDATE_BRD)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNETPC_UPDATE_CDM_SAFETIME_BRD : public SNETPC_BROAD
	{
		float				fCDMSafeTime;

		SNETPC_UPDATE_CDM_SAFETIME_BRD () :
			fCDMSafeTime(0.0f)
		{
			dwSize = sizeof(SNETPC_UPDATE_CDM_SAFETIME_BRD);
			nType = NET_MSG_GCTRL_UPDATE_CDM_SAFETIME_BRD;
			MIN_STATIC_ASSERT(sizeof(SNETPC_UPDATE_CDM_SAFETIME_BRD)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_NPCTALK_BASIC : public NET_MSG_GENERIC
	{
		
		int					nAction;
		DWORD				dwNpcGlobID;
		DWORD				dwTalkID;

		SNETPC_REQ_NPCTALK_BASIC () 
			: nAction(0)
			, dwNpcGlobID(0)
			, dwTalkID(0)
		{
			dwSize = sizeof(SNETPC_REQ_NPCTALK_BASIC);
			nType = NET_MSG_GCTRL_REQ_NPCTALK_BASIC;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_NPCTALK_BASIC)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_REQ_NPCTALK_BASIC_FB : public NET_MSG_GENERIC
	{
		
		EMREQ_NPCTALK_FB	emFB;
		int					nAction;
		DWORD				dwNpcGlobID;
		DWORD				dwTalkID;

		SNETPC_REQ_NPCTALK_BASIC_FB () 
			: emFB( EMREQ_NPCTALK_FB_FAIL )
			, nAction(0)
			, dwNpcGlobID(0)
			, dwTalkID(0)
		{
			dwSize = sizeof(SNETPC_REQ_NPCTALK_BASIC_FB);
			nType = NET_MSG_GCTRL_REQ_NPCTALK_BASIC_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_REQ_NPCTALK_BASIC_FB)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_DOMINATE_MOVETO_CF : public NET_MSG_GENERIC
	{		
		STARGETID		sTargetID;
		D3DXVECTOR3 vFromPt;
		D3DXVECTOR3 vTargetPt;
		float fTime;		

		SNETPC_DOMINATE_MOVETO_CF(float _fTime, D3DXVECTOR3 _vFromPt, D3DXVECTOR3 _vTargetPt, STARGETID _sTargetID) :
		fTime(_fTime),
			vFromPt(_vFromPt),
			vTargetPt(_vTargetPt),
			sTargetID(_sTargetID)			
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_DOMINATE_MOVETO_CF;
			MIN_STATIC_ASSERT(sizeof(SNETPC_DOMINATE_MOVETO_CF)<=NET_DATA_BUFSIZE);
		}
	};
	
	struct SNETPC_DOMINATE_MOVETO_CF_FB : public NET_MSG_GENERIC
	{
		EMDOMINATE_MOVETO_FB emFB;
		STARGETID		sTargetID;
		float				fRadiusMovable;

		SNETPC_DOMINATE_MOVETO_CF_FB()
			: emFB(EMDOMINATE_MOVETO_FB_FAIL)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_DOMINATE_MOVETO_CF_FB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_DOMINATE_MOVETO_CF_FB)<=NET_DATA_BUFSIZE);
		}
	};
	
	struct SNETPC_DOMINATE_MOVETO_FC : public NET_MSG_GENERIC
	{
		float fTime;
		D3DXVECTOR3 vFromPt;
		D3DXVECTOR3 vTargetPt;
		SNETPC_DOMINATE_MOVETO_FC() :
		fTime(0.0f)
		{
			dwSize = sizeof(*this);
			nType = NET_MSG_GCTRL_DOMINATE_MOVETO_FC;
			MIN_STATIC_ASSERT(sizeof(SNETPC_DOMINATE_MOVETO_FC)<=NET_DATA_BUFSIZE);
		}
	};

    struct NET_GM_DB_UNLOCK_USER : public NET_MSG_GENERIC
    {
        DWORD m_UserDbNum;

        NET_GM_DB_UNLOCK_USER(DWORD UserDbNum)
            : m_UserDbNum(UserDbNum)
        {
            dwSize = sizeof(NET_GM_DB_UNLOCK_USER);
            nType = NET_MSG_GM_DB_UNLOCK_USER;
            MIN_STATIC_ASSERT(sizeof(NET_GM_DB_UNLOCK_USER)<=NET_DATA_BUFSIZE);
        }
    };

    struct NET_GM_DB_UNLOCK_USER_AF : public NET_MSG_GENERIC
    {
        DWORD m_UserDbNum;

        NET_GM_DB_UNLOCK_USER_AF(DWORD UserDbNum)
            : m_UserDbNum(UserDbNum)
        {
            dwSize = sizeof(NET_GM_DB_UNLOCK_USER_AF);
            nType = NET_MSG_GM_DB_UNLOCK_USER_AF;
            MIN_STATIC_ASSERT(sizeof(NET_GM_DB_UNLOCK_USER_AF)<=NET_DATA_BUFSIZE);
        }
    };

	struct SNETPC_MSG_CHA_SAVE_DF : public NET_MSG_GENERIC
	{
		DWORD dwUserNum;

		SNETPC_MSG_CHA_SAVE_DF()
			: dwUserNum( 0 )
		{
			dwSize = sizeof( SNETPC_MSG_CHA_SAVE_DF );
			nType = NET_MSG_CHA_SAVE_DF;
			MIN_STATIC_ASSERT( sizeof( SNETPC_MSG_CHA_SAVE_DF )<= NET_DATA_BUFSIZE );
		}
	};

	struct SNETPC_MSG_CHA_SAVE_FA : public NET_MSG_GENERIC
	{
		DWORD dwUserNum;

		SNETPC_MSG_CHA_SAVE_FA()
			: dwUserNum( 0 )
		{
			dwSize = sizeof( SNETPC_MSG_CHA_SAVE_FA );
			nType = NET_MSG_CHA_SAVE_FA;
			MIN_STATIC_ASSERT( sizeof( SNETPC_MSG_CHA_SAVE_FA )<= NET_DATA_BUFSIZE );
		}

		SNETPC_MSG_CHA_SAVE_FA( DWORD dwNum )
			: dwUserNum( dwNum )
		{
			dwSize = sizeof( SNETPC_MSG_CHA_SAVE_FA );
			nType = NET_MSG_CHA_SAVE_FA;
			MIN_STATIC_ASSERT( sizeof( SNETPC_MSG_CHA_SAVE_FA )<= NET_DATA_BUFSIZE );
		}
	};

	struct SNETPC_MSG_CHA_SAVE_END_FA : public NET_MSG_GENERIC
	{
		DWORD dwUserNum;
		DWORD dwChaNum;

		SNETPC_MSG_CHA_SAVE_END_FA()
			: dwUserNum( 0 )
			, dwChaNum( 0 )
		{
			dwSize = sizeof( SNETPC_MSG_CHA_SAVE_END_FA );
			nType = NET_MSG_CHA_SAVE_END_FA;
			MIN_STATIC_ASSERT( sizeof( SNETPC_MSG_CHA_SAVE_END_FA )<= NET_DATA_BUFSIZE );
		}

		SNETPC_MSG_CHA_SAVE_END_FA( DWORD UserNum, DWORD ChaNum )
			: dwUserNum( UserNum )
			, dwChaNum( ChaNum )
		{
			dwSize = sizeof( SNETPC_MSG_CHA_SAVE_END_FA );
			nType = NET_MSG_CHA_SAVE_END_FA;
			MIN_STATIC_ASSERT( sizeof( SNETPC_MSG_CHA_SAVE_END_FA )<= NET_DATA_BUFSIZE );
		}
	};

	struct SNETPC_MSG_CHA_VALIDATION_FA : public NET_MSG_GENERIC
	{
		DWORD dwUserNum;
		SCHARSTATS sStats;
		DWORD dwAttack;
		DWORD dwDefence;

		SNETPC_MSG_CHA_VALIDATION_FA()
			: dwUserNum( 0 )
		{
			dwSize = sizeof( SNETPC_MSG_CHA_VALIDATION_FA );
			nType = NET_MSG_CHA_VALIDATION_FA;
			MIN_STATIC_ASSERT( sizeof( SNETPC_MSG_CHA_VALIDATION_FA )<= NET_DATA_BUFSIZE );
		}

		SNETPC_MSG_CHA_VALIDATION_FA( DWORD dwNum )
			: dwUserNum( dwNum )
		{
			dwSize = sizeof( SNETPC_MSG_CHA_VALIDATION_FA );
			nType = NET_MSG_CHA_VALIDATION_FA;
			MIN_STATIC_ASSERT( sizeof( SNETPC_MSG_CHA_VALIDATION_FA )<= NET_DATA_BUFSIZE );
		}
	};

	struct SNETPC_TRANS_TO_WORLDBATTLE : public NET_MSG_GENERIC
	{	
		DWORD dwRet;
		DWORD dwGaeaID;
		DWORD dwChaDBNum;

		SNETPC_TRANS_TO_WORLDBATTLE () 
			: dwRet( 0 )
			, dwGaeaID(GAEAID_NULL)
			, dwChaDBNum(0)
		{
			dwSize = sizeof(SNETPC_TRANS_TO_WORLDBATTLE);
			nType = NET_MSG_GCTRL_TRANS_TO_WORLDBATTLE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_TRANS_TO_WORLDBATTLE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_TRANS_TO_WORLDBATTLE_ANS : public NET_MSG_GENERIC
	{
		enum TRANS_TO_WORLDBATTLE_FLAG
		{
			EM_TRANS_TO_WORLDBATTLE_FAILED,		// 실패;
			EM_WORLDBATTLE_SERVER_OFFLINE,		// 전장서버 오프라인상태;
			EM_INVALID_CHARACTER_ITEM,			// 캐릭터 아이템 유효하지 않음;
			EM_INVALID_POSITION,				// 이동할 수 없는 위치에 있음;
		};

		TRANS_TO_WORLDBATTLE_FLAG emFlag;

		SNETPC_TRANS_TO_WORLDBATTLE_ANS () 
			: emFlag( EM_TRANS_TO_WORLDBATTLE_FAILED )
		{
			dwSize = sizeof(SNETPC_TRANS_TO_WORLDBATTLE_ANS);
			nType = NET_MSG_GCTRL_TRANS_TO_WORLDBATTLE_ANS;
			MIN_STATIC_ASSERT(sizeof(SNETPC_TRANS_TO_WORLDBATTLE_ANS)<=NET_DATA_BUFSIZE);
		}

		inline void OfflineWorldBattleServer ()
		{
			emFlag = EM_WORLDBATTLE_SERVER_OFFLINE;
		}

		inline void InvalidCharacterItem ()
		{
			emFlag = EM_INVALID_CHARACTER_ITEM;
		}

		inline void InvalidPosition ()
		{
			emFlag = EM_INVALID_POSITION;
		}
	};

	struct SNETPC_TRANS_TO_WORLDBATTLE_CHAR_SAVE : public NET_MSG_GENERIC
	{	
		DWORD dwRet;
		DWORD dwGaeaID;
		DWORD dwChaDBNum;

		SNETPC_TRANS_TO_WORLDBATTLE_CHAR_SAVE () 
			: dwRet( 0 )
			, dwGaeaID(GAEAID_NULL)
			, dwChaDBNum(0)
		{
			dwSize = sizeof(SNETPC_TRANS_TO_WORLDBATTLE_CHAR_SAVE);
			nType = NET_MSG_GCTRL_TRANS_TO_WORLDBATTLE_CHAR_SAVE;
			MIN_STATIC_ASSERT(sizeof(SNETPC_TRANS_TO_WORLDBATTLE_CHAR_SAVE)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_TRANS_TO_WORLDBATTLE_DB : public NET_MSG_GENERIC
	{	
		DWORD nRet;
		DWORD dwGaeaID;
		DWORD dwChaDBNum;

		SNETPC_TRANS_TO_WORLDBATTLE_DB ()
			: nRet( 0 )
			, dwGaeaID(GAEAID_NULL)
			, dwChaDBNum(0)
		{
			dwSize = sizeof(SNETPC_TRANS_TO_WORLDBATTLE_DB);
			nType = NET_MSG_GCTRL_TRANS_TO_WORLDBATTLE_DB;
			MIN_STATIC_ASSERT(sizeof(SNETPC_TRANS_TO_WORLDBATTLE_DB)<=NET_DATA_BUFSIZE);
		}
	};


	struct SNET_NETWORKPROFILE_REQ : public NET_MSG_GENERIC
    {
        int m_nProfileType;
        
        SNET_NETWORKPROFILE_REQ(int type)
        {
            dwSize = sizeof(SNET_NETWORKPROFILE_REQ);
            nType = NET_MSG_GM_NETWORKPROFILE_REQ;
			m_nProfileType = type;
            MIN_STATIC_ASSERT(sizeof(SNET_NETWORKPROFILE_REQ)<=NET_DATA_BUFSIZE);
        }
    };

	struct SNETPC_EXP_COMPRESSOR_REQ : public NET_MSG_GENERIC
	{	
		WORD					wPosX;
		WORD					wPosY;

		SNETPC_EXP_COMPRESSOR_REQ () 
			: wPosX(0)
			, wPosY(0)
		{
			dwSize = sizeof(SNETPC_EXP_COMPRESSOR_REQ);
			nType = NET_MSG_GCTRL_EXP_COMPRESSOR_REQ;
			MIN_STATIC_ASSERT(sizeof(SNETPC_EXP_COMPRESSOR_REQ)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_EXP_COMPRESSOR_ANS : public NET_MSG_GENERIC
	{	
		WORD					wPosX;
		WORD					wPosY;
		EMINVEN_EXP_COMPRESSOR_FB emFB;

		SNETPC_EXP_COMPRESSOR_ANS () 
			: wPosX(0)
			, wPosY(0)
			, emFB(EMINVEN_EXP_COMPRESSOR_FAIL)
		{
			dwSize = sizeof(SNETPC_EXP_COMPRESSOR_ANS);
			nType = NET_MSG_GCTRL_EXP_COMPRESSOR_ANS;
			MIN_STATIC_ASSERT(sizeof(SNETPC_EXP_COMPRESSOR_ANS)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_EXP_CAPSULE_REQ : public NET_MSG_GENERIC
	{	
		WORD					wPosX;
		WORD					wPosY;

		SNETPC_EXP_CAPSULE_REQ () 
			: wPosX(0)
			, wPosY(0)
		{
			dwSize = sizeof(SNETPC_EXP_CAPSULE_REQ);
			nType = NET_MSG_GCTRL_EXP_CAPSULE_REQ;
			MIN_STATIC_ASSERT(sizeof(SNETPC_EXP_CAPSULE_REQ)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNETPC_EXP_CAPSULE_ANS : public NET_MSG_GENERIC
	{	
		WORD					wPosX;
		WORD					wPosY;
		EMINVEN_EXP_CAPSULE_FB emFB;

		SNETPC_EXP_CAPSULE_ANS () 
			: wPosX(0)
			, wPosY(0)
			, emFB(EMINVEN_EXP_CAPSULE_FAIL)
		{
			dwSize = sizeof(SNETPC_EXP_CAPSULE_ANS);
			nType = NET_MSG_GCTRL_EXP_CAPSULE_ANS;
			MIN_STATIC_ASSERT(sizeof(SNETPC_EXP_CAPSULE_ANS)<=NET_DATA_BUFSIZE);
		}
	};

	struct SNET_CHANGE_WBJOIN_STATE : public NET_MSG_GENERIC
    {
        bool bJoinWorldBattle;
        
        SNET_CHANGE_WBJOIN_STATE(bool bJoin)
        {
            dwSize = sizeof(SNET_CHANGE_WBJOIN_STATE);
            nType = NET_MSG_GM_CHANGE_WBJOIN_STATE;
			bJoinWorldBattle = bJoin;
            MIN_STATIC_ASSERT(sizeof(SNET_CHANGE_WBJOIN_STATE)<=NET_DATA_BUFSIZE);
        }
    };

	struct SNET_RANMOBILE_COMMAND_REQUEST : public NET_MSG_GENERIC
    {
        int nCommand;
		int nValue;
		SNATIVEID sItemID;

		char szName[CHAR_SZNAME];

        SNET_RANMOBILE_COMMAND_REQUEST(int command)
        {
            dwSize = sizeof(SNET_RANMOBILE_COMMAND_REQUEST);
            nType = NET_MSG_GM_RANMOBILE_COMMAND_REQUEST;
			nCommand = command;
			nValue = 0;
            MIN_STATIC_ASSERT(sizeof(SNET_RANMOBILE_COMMAND_REQUEST)<=NET_DATA_BUFSIZE);

			memset( szName, 0, sizeof(char) * CHAR_SZNAME );
        }
        
        SNET_RANMOBILE_COMMAND_REQUEST(int command, int value)
        {
            dwSize = sizeof(SNET_RANMOBILE_COMMAND_REQUEST);
            nType = NET_MSG_GM_RANMOBILE_COMMAND_REQUEST;
			nCommand = command;
			nValue = value;
            MIN_STATIC_ASSERT(sizeof(SNET_RANMOBILE_COMMAND_REQUEST)<=NET_DATA_BUFSIZE);

			memset( szName, 0, sizeof(char) * CHAR_SZNAME );
        }

		SNET_RANMOBILE_COMMAND_REQUEST(int command, int nMID, int nSID )
        {
            dwSize = sizeof(SNET_RANMOBILE_COMMAND_REQUEST);
            nType = NET_MSG_GM_RANMOBILE_COMMAND_REQUEST;
			nCommand = command;
			nValue = 0;
			sItemID.wMainID = nMID;
			sItemID.wSubID = nSID;
            MIN_STATIC_ASSERT(sizeof(SNET_RANMOBILE_COMMAND_REQUEST)<=NET_DATA_BUFSIZE);

			memset( szName, 0, sizeof(char) * CHAR_SZNAME );
        }

		SNET_RANMOBILE_COMMAND_REQUEST(int command, int value, int nMID, int nSID )
        {
            dwSize = sizeof(SNET_RANMOBILE_COMMAND_REQUEST);
            nType = NET_MSG_GM_RANMOBILE_COMMAND_REQUEST;
			nCommand = command;
			nValue = value;
			sItemID.wMainID = nMID;
			sItemID.wSubID = nSID;
            MIN_STATIC_ASSERT(sizeof(SNET_RANMOBILE_COMMAND_REQUEST)<=NET_DATA_BUFSIZE);

			memset( szName, 0, sizeof(char) * CHAR_SZNAME );
        }

		void SetName( const char* sz )
		{
			if( sz )
				StringCchCopy( szName, CHAR_SZNAME, sz );
		}
    };

	struct SNET_RANMOBILE_COMMAND_RESPONSE : public NET_MSG_GENERIC
    {
        DWORD dwUserDbNum;
		DWORD dwChaDBNum;
		int nResult;
		int nCommand;
		int nValue;
		RAN_MOBILE_INFO sMobileInfo;
        
        SNET_RANMOBILE_COMMAND_RESPONSE()
        {
            dwSize = sizeof(SNET_RANMOBILE_COMMAND_RESPONSE);
            nType = NET_MSG_GM_RANMOBILE_COMMAND_RESPONSE;
			nCommand = 0;
			nResult = 0;
			dwUserDbNum = 0;
			dwChaDBNum = 0;
			nValue = 0;
            MIN_STATIC_ASSERT(sizeof(SNET_RANMOBILE_COMMAND_RESPONSE)<=NET_DATA_BUFSIZE);
        }

		void ClearUserInfo()
		{
			dwUserDbNum = 0;
			dwChaDBNum = 0;
		}
    };

	struct SNETPC_REQ_USERCOUNT_LAND_CF : public NET_MSG_GENERIC
	{
		Faction::EMFACTION_TYPE emType;
		int nInstanceMapID;

		SNETPC_REQ_USERCOUNT_LAND_CF ( Faction::EMFACTION_TYPE _emType, int _nInstanceMapID )
			: emType( _emType )
			, nInstanceMapID( _nInstanceMapID )
		{
			dwSize = sizeof( *this );
			nType = NET_MSG_GM_REQ_USERCOUNT_LAND_CF;
			MIN_STATIC_ASSERT( sizeof( SNETPC_REQ_USERCOUNT_LAND_CF ) <= NET_DATA_BUFSIZE );
		}
	};

	struct SNETPC_RESPONSE_USERCOUNT_LAND_FC : public NET_MSG_GENERIC
	{
		EMUSERCOUNT_LAND emFlag;

		DWORD dwType;
		DWORD dwID;
		DWORD dwCount;

		SNETPC_RESPONSE_USERCOUNT_LAND_FC ( DWORD _dwID, DWORD _dwCount )
			: dwID( _dwID )
			, dwCount( _dwCount )
			, emFlag( EMUSERCOUNT_LAND_SUCCESS )
		{
			dwType = 0;

			dwSize = sizeof( *this );
			nType = NET_MSG_GM_RESPONSE_USERCOUNT_LAND_FC;
			MIN_STATIC_ASSERT( sizeof( SNETPC_RESPONSE_USERCOUNT_LAND_FC ) <= NET_DATA_BUFSIZE );
		}
	};

    //---------------------------------------------------------------------------NETzg
    struct SNETPC_ACQUIRE_EXP : public NET_MSG_GENERIC
    {
        DWORD EXP;

        SNETPC_ACQUIRE_EXP()
            : EXP(0)
        {
            dwSize = sizeof(SNETPC_ACQUIRE_EXP);
            nType = NET_MSG_GCTRL_ACQUIRE_EXP;
            MIN_STATIC_ASSERT(sizeof(SNETPC_ACQUIRE_EXP)<=NET_DATA_BUFSIZE);
        }
    };

	//---------------------------------------------------------------------------------
	struct SNETPC_REQUEST_LOCK_CHARSLOT_CF : public NET_MSG_CHARACTER
	{
		WORD wCharSlotNum;

		SNETPC_REQUEST_LOCK_CHARSLOT_CF ( WORD _wCharSlotNum )
			: wCharSlotNum ( _wCharSlotNum )
		{
			dwSize = sizeof( *this );
			nType = NET_MSG_REQUEST_LOCK_CHARSLOT_CF;
			MIN_STATIC_ASSERT( sizeof( SNETPC_REQUEST_LOCK_CHARSLOT_CF ) <= NET_DATA_BUFSIZE );
		}
	};

	struct SNETPC_REQUEST_UNLOCK_CHARSLOT_CF : public NET_MSG_CHARACTER
	{
		WORD wCharSlotNum;

		SNETPC_REQUEST_UNLOCK_CHARSLOT_CF ( WORD _wCharSlotNum )
			: wCharSlotNum ( _wCharSlotNum )
		{
			dwSize = sizeof( *this );
			nType = NET_MSG_REQUEST_UNLOCK_CHARSLOT_CF;
			MIN_STATIC_ASSERT( sizeof( SNETPC_REQUEST_UNLOCK_CHARSLOT_CF ) <= NET_DATA_BUFSIZE );
		}
	};

	struct SNETPC_REQUEST_CHANGE_CHARSLOT_CF : public NET_MSG_CHARACTER
	{
		WORD wCharSlotNum;

		SNETPC_REQUEST_CHANGE_CHARSLOT_CF ( WORD _wCharSlotNum )
			: wCharSlotNum ( _wCharSlotNum )
		{
			dwSize = sizeof( *this );
			nType = NET_MSG_REQUEST_CHANGE_CHARSLOT_CF;
			MIN_STATIC_ASSERT( sizeof( SNETPC_REQUEST_CHANGE_CHARSLOT_CF ) <= NET_DATA_BUFSIZE );
		}
	};

	struct SNETPC_REQUEST_UNLOCK_CHARSLOT_USE_ITEM_CF : public NET_MSG_GENERIC
	{
		WORD wPosX;
		WORD wPosY;

		SNETPC_REQUEST_UNLOCK_CHARSLOT_USE_ITEM_CF ( void )
			: wPosX ( 0 )
			, wPosY ( 0 )
		{
			dwSize = sizeof( *this );
			nType = NET_MSG_REQUEST_UNLOCK_CHARSLOT_USE_ITEM_CF;
			MIN_STATIC_ASSERT( sizeof( SNETPC_REQUEST_UNLOCK_CHARSLOT_USE_ITEM_CF ) <= NET_DATA_BUFSIZE );
		}
	};



	struct SNETPC_RESPONSE_LOCK_CHARSLOT_FC : public NET_MSG_CHARACTER
	{
		EMCHARSLOT_LOCK_FLAG emFlag;

		// Change Character Slot;
		WORD wCharSlotNum;

		SNETPC_RESPONSE_LOCK_CHARSLOT_FC ( EMCHARSLOT_LOCK_FLAG _emFlag )
			: emFlag ( _emFlag )
			, wCharSlotNum ( 0 )
		{
			dwSize = sizeof( *this );
			nType = NET_MSG_RESPONSE_LOCK_CHARSLOT_FC;
			MIN_STATIC_ASSERT( sizeof( SNETPC_RESPONSE_LOCK_CHARSLOT_FC ) <= NET_DATA_BUFSIZE );
		}
	};

	struct SNETPC_RESPONSE_UNLOCK_CHARSLOT_FC : public NET_MSG_CHARACTER
	{
		EMCHARSLOT_UNLOCK_FLAG emFlag;

		// Change Character Slot;
		WORD wCharSlotNum;

		SNETPC_RESPONSE_UNLOCK_CHARSLOT_FC ( EMCHARSLOT_UNLOCK_FLAG _emFlag )
			: emFlag ( _emFlag )
			, wCharSlotNum ( 0 )
		{
			dwSize = sizeof( *this );
			nType = NET_MSG_RESPONSE_UNLOCK_CHARSLOT_FC;
			MIN_STATIC_ASSERT( sizeof( SNETPC_RESPONSE_UNLOCK_CHARSLOT_FC ) <= NET_DATA_BUFSIZE );
		}
	};

	struct SNETPC_RESPONSE_CHANGE_CHARSLOT_FC : public NET_MSG_CHARACTER
	{
		EMCHARSLOT_CHANGE_FLAG emFlag;

		// Change Character Slot;
		WORD wCharSlotNum;

		// Stat;
		SCHARSTATS sStats;
		WORD wStatPoint;

		// Skill Quick Slot
		WORD wSKILLQUICK_ACT;
		SNATIVEID sSKILLQUICK[ EMSKILLQUICK_SIZE ];

		// Skill;
		DWORD dwSkillNum;
		DWORD dwSkillPoint;
		SCHARSKILL sSKILL[ EMGLMSG_SKILLMAX ];

		const bool AddSkill ( const SCHARSKILL& sSkill )
		{
			if ( dwSkillNum >= EMGLMSG_SKILLMAX )
				return false;

			sSKILL[ dwSkillNum ] = sSkill;
			dwSkillNum++;

			dwSize = sizeof(*this) - sizeof( SCHARSKILL ) * ( EMGLMSG_SKILLMAX - dwSkillNum );
			return true;
		}

		SNETPC_RESPONSE_CHANGE_CHARSLOT_FC ( EMCHARSLOT_CHANGE_FLAG _emFlag )
			: emFlag ( _emFlag )
			, wCharSlotNum ( 0 )
			, wStatPoint ( 0 )
			, wSKILLQUICK_ACT ( 0 )
			, dwSkillNum ( 0 )
			, dwSkillPoint ( 0 )
		{
			dwSize = sizeof( *this );
			nType = NET_MSG_RESPONSE_CHANGE_CHARSLOT_FC;
			MIN_STATIC_ASSERT( sizeof( SNETPC_RESPONSE_CHANGE_CHARSLOT_FC ) <= NET_DATA_BUFSIZE );

			for ( int i=0; i<EMGLMSG_SKILLMAX; ++i )
				sSKILL[ i ] = SCHARSKILL();
		}
	};

	struct SNETPC_UPDATE_CHARSLOT_FC : public NET_MSG_CHARACTER
	{
		// Current Character Slot;
		WORD wCharSlotNum;

		// Current Character Slot Lock State;
		bool arrSlotLockState[ EMCHAR_SLOT_DATA_SIZE ];

		// Open Character Slot Count;
		int nOpenCharSlot;

		SNETPC_UPDATE_CHARSLOT_FC ( void )
			: wCharSlotNum ( 0 )
			, nOpenCharSlot ( 0 )
		{
			dwSize = sizeof( *this );
			nType = NET_MSG_UPDATE_CHARSLOT_FC;
			MIN_STATIC_ASSERT( sizeof( SNETPC_UPDATE_CHARSLOT_FC ) <= NET_DATA_BUFSIZE );

			for ( int i=0; i<EMCHAR_SLOT_DATA_SIZE; ++i )
				arrSlotLockState[ i ] = true;
		}
	};

	//---------------------------------------------------------------------------------

	// Revert to default structure packing
	#pragma pack()
};

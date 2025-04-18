 
-- 슬롯 툴팁 최대 Width
SLOT_TOOLTIP_MAX_WIDTH = 360
-- 슬롯 툴팁 최소 Width
SLOT_TOOLTIP_MIN_WIDTH = 230

-- 사용자 타입 
USER_COMMON				= 0			-- 일반사용자
USER_PREMIUM			= 1			-- 프리미엄 유저
USER_SPECIAL			= 10		-- 특별사용자 (게임방 업체 사장, 기자 등등)
USER_USER_GM			= 11		-- 유저GM
USER_USER_GM_MASTER		= 12		-- 유저GM Master
USER_GM4				= 19		-- GM 4 급, Web GM 용
USER_GM3				= 20		-- GM 3 급
USER_GM2				= 21		-- GM 2 급
USER_GM1				= 22		-- GM 1 급
USER_MASTER				= 30		-- Master

-- GLDefine.h - EMCROW와 동기화되어야한다
EMCROW_PC		= 0
EMCROW_NPC		= 1
EMCROW_MOB		= 2
EMCROW_ITEM		= 3
EMCROW_MONEY	= 4
EMCROW_PET		= 5
EMCROW_SUMMON	= 6
EMCROW_MATERIAL	= 7
EMCROW_ZONENAME = 8
EMCROW_GATENAME = 9
EMCROW_GATE	= 10

-- 캐릭터 클래스 플래그
CHARCLASS_NONE			= 0x00000		-- 추가, 아무것도 선택되지 않은 상태
CHARCLASS_FIGHTER_M		= 0x00001		-- 격투부 남
CHARCLASS_ARMS_M		= 0x00002		-- 검도부 남
CHARCLASS_ARCHER_W		= 0x00004		-- 양궁부 여
CHARCLASS_SPIRIT_W		= 0x00008		-- 기예부 여
CHARCLASS_EXTREAM_M		= 0x00010		-- 극강부 남
CHARCLASS_EXTREAM_W		= 0x00020		-- 극강부 여
CHARCLASS_FIGHTER_W		= 0x00040		-- 격투부 여
CHARCLASS_ARMS_W		= 0x00080		-- 검도부 여
CHARCLASS_ARCHER_M		= 0x00100		-- 양궁부 남
CHARCLASS_SPIRIT_M		= 0x00200		-- 기예부 남
CHARCLASS_SCIENTIST_M 	= 0x00400		-- 과학부 남
CHARCLASS_SCIENTIST_W 	= 0x00800		-- 과학부 여
CHARCLASS_ASSASSIN_M 	= 0x01000		-- 인술부 남
CHARCLASS_ASSASSIN_W 	= 0x02000		-- 인술부 여
CHARCLASS_TRICKER_M 	= 0x04000		-- 마술부 남
CHARCLASS_TRICKER_W		= 0x08000		-- 마술부 여
CHARCLASS_ETC_M 		= 0x10000		-- 기　타 남
CHARCLASS_ETC_W 		= 0x20000		-- 기　타 여
CHARCLASS_ACTOR_M 		= 0x40000		-- 연극부 남
CHARCLASS_ACTOR_W 		= 0x80000		-- 연극부 여

-- 캐릭터 클래스 인덱스
CHARCLASS_IDX_FIGHTER_M		= 0
CHARCLASS_IDX_ARMS_M 		= 1
CHARCLASS_IDX_ARCHER_W		= 2
CHARCLASS_IDX_SPIRIT_W		= 3
CHARCLASS_IDX_EXTREAM_M		= 4
CHARCLASS_IDX_EXTREAM_W		= 5
CHARCLASS_IDX_FIGHTER_W		= 6
CHARCLASS_IDX_ARMS_W		= 7
CHARCLASS_IDX_ARCHER_M		= 8
CHARCLASS_IDX_SPIRIT_M		= 9
CHARCLASS_IDX_SCIENTIST_M	= 10
CHARCLASS_IDX_SCIENTIST_W	= 11
CHARCLASS_IDX_ASSASSIN_M	= 12
CHARCLASS_IDX_ASSASSIN_W	= 13
CHARCLASS_IDX_TRICKER_M		= 14
CHARCLASS_IDX_TRICKER_W		= 15
CHARCLASS_IDX_ETC_M			= 16
CHARCLASS_IDX_ETC_W			= 17
CHARCLASS_IDX_ACTOR_M		= 18
CHARCLASS_IDX_ACTOR_W		= 19
CHARCLASS_IDX_NUM_ETC		= 20

-- 아이템 타입
ITEMTYPE_SUIT					= 0
ITEMTYPE_ARROW					= 1	-- 화살.
ITEMTYPE_CURE					= 2	-- 약품.
ITEMTYPE_SKILL					= 3	-- 스킬 습득 서적.
ITEMTYPE_RECALL					= 4	-- 귀환서.
ITEMTYPE_KEY					= 5	-- 인증서 (열쇠)
ITEMTYPE_GRINDING				= 6	-- 연마제
ITEMTYPE_CHARM					= 7	-- 부적.
ITEMTYPE_TICKET					= 8	-- 승차권.
ITEMTYPE_SKP_RESET				= 9	-- 스킬 포인트 리셋. (미사용)
ITEMTYPE_STAT_RESET				= 10	-- 스텟 포인트 리셋. (미사용)
ITEMTYPE_SKP_STAT_RESET_A		= 11	-- 스킬 스텟 포인트 균등 리셋. (구 망각의 약)
ITEMTYPE_BOX					= 12	-- 선물 상자.
ITEMTYPE_CLEANSER				= 13	-- 세탁제.
ITEMTYPE_LOUDSPEAKER			= 14	-- 확성기.
ITEMTYPE_FIRECRACKER			= 15	-- 폭죽.
ITEMTYPE_CHARACTER_CARD			= 16	-- 캐릭터 추가 카드.
ITEMTYPE_INVEN_CARD				= 17	-- 인벤 확장 카드.
ITEMTYPE_STORAGE_CARD			= 18	-- 창고 확장 카드.
ITEMTYPE_STORAGE_CONNECT		= 19	-- 창고 연결 카드.
ITEMTYPE_PREMIUMSET				= 20	-- 프리미엄 세트.
ITEMTYPE_PRIVATEMARKET			= 21	-- 개인상점 개설 권리.
ITEMTYPE_RANDOMITEM				= 22	-- 랜덤 아이탬.
ITEMTYPE_DISJUNCTION			= 23	-- 코스툼 분리.
ITEMTYPE_HAIR					= 24	-- 헤어스타일 변경.
ITEMTYPE_FACE					= 25	-- 얼굴 변경.
ITEMTYPE_QITEM					= 26	-- Question 아이템.
ITEMTYPE_CD						= 27	-- CD.
ITEMTYPE_2FRIEND				= 28	-- 친구에게.
ITEMTYPE_CLUBCALL				= 29	-- 클럽호출.
ITEMTYPE_HAIRSHOP				= 30	-- 헤어샾 이용권.
ITEMTYPE_RENAME					= 31   -- 이름변경 카드.
ITEMTYPE_HAIR_STYLE				= 32	-- 헤어스타일
ITEMTYPE_HAIR_COLOR				= 33   -- 헤어컬러
ITEMTYPE_REVIVE					= 34	-- 귀혼주
ITEMTYPE_PET_CARD				= 35	-- 펫 카드	// PetData
ITEMTYPE_PET_FOOD				= 36	-- 펫 먹이
ITEMTYPE_PET_RENAME				= 37	-- 팻 이름변경
ITEMTYPE_PET_COLOR				= 38	-- 팻 컬러변경
ITEMTYPE_PET_STYLE				= 39	-- 팻 스타일변경
ITEMTYPE_PET_SKILL				= 40	-- 팻 스킬
ITEMTYPE_SMS					= 41	-- SMS문자 발송
ITEMTYPE_PET_REVIVE				= 42	-- 팻 부활카드
ITEMTYPE_ANTI_DISAPPEAR 		= 43	-- 소방주(소멸방지)
ITEMTYPE_REMODEL				= 44	-- 개조 기능
ITEMTYPE_VEHICLE				= 45	-- 탈것
ITEMTYPE_VEHICLE_OIL			= 46	-- 탈것 기름
ITEMTYPE_VIETNAM_ITEMGET		= 47	-- 베트남 탐닉 방지 아이템 획득
ITEMTYPE_VIETNAM_EXPGET			= 48	-- 베트남 탐닉 방지 경험치 획득	
ITEMTYPE_GENDER_CHANGE			= 49	-- 성별 변경 카드 
ITEMTYPE_GARBAGE_CARD			= 50	-- 휴지통 카드	
ITEMTYPE_TELEPORT_CARD			= 51	-- 소환서	
ITEMTYPE_PET_SKIN_PACK  		= 52	-- 펫 스킨 팩
ITEMTYPE_FACE_STYLE				= 53	-- 얼굴스타일
ITEMTYPE_TAXI_CARD				= 54	-- 택시카드
ITEMTYPE_MATERIALS				= 55	-- 재료아이템
ITEMTYPE_NPC_RECALL				= 56	-- NPC소환 카드
ITEMTYPE_BULLET					= 57	-- 총알
ITEMTYPE_LUNCHBOX				= 58	-- 도시락
ITEMTYPE_PET_DUALSKILL			= 59	-- 펫 스킬 중복사용 카드
ITEMTYPE_INC_GRINDINGRATE		= 60	-- 강화주 (강화 확률 증가)
ITEMTYPE_ANTI_RESET				= 61	-- 완화주 (리셋 단계 완화)
ITEMTYPE_VEHICLE_COLOR			= 62	-- 탈것 컬러 변경카드
ITEMTYPE_BIKE_BOOST_CARD		= 63	-- 오토바이 부스트 기능카드
ITEMTYPE_SKP_STAT_RESET_B		= 64	-- 스킬 스텟 포인트 선택 리셋. (신 망각의 약)
ITEMTYPE_COSTUM_COLOR_CH_CD		= 65	-- 코스튬 컬러 변경 카드.
ITEMTYPE_POSTBOX_CARD			= 66	-- 우편함 연결 카드
ITEMTYPE_POINT_CARD_REFUND		= 67	-- 환불가능 Point Card
ITEMTYPE_POINT_CARD_NOT_REFUND	= 68	-- 환불불가 Point Card
ITEMTYPE_PRODUCT_BOOK			= 69	-- 조합서
ITEMTYPE_PRODUCT_RESET			= 70	-- 제작 초기화
ITEMTYPE_PMARKET_SEARCH_CARD	= 71	-- 개인상점 검색 카드
ITEMTYPE_REPAIR_CARD			= 72	-- 수리도구 연결카드
ITEMTYPE_STAMP_CARD				= 73	-- 인장 카드;
ITEMTYPE_STAGE_PASS				= 74	--스테이지 패스
ITEMTYPE_ENTER_NUM_CHARGE		= 75	--인던 입장횟수 중전권
ITEMTYPE_CARDGAME_CARD			= 76	-- 카드게임 연결카드
ITEMTYPE_CONSIGMENT_SALE_GAME_MONEY		= 77  -- 게임머니 아이템
ITEMTYPE_CONSIGMENT_SALE_EXTEND_CARD	= 78  -- 위탁판매 확장슬롯

ITEMTYPE_CHANGE_ACTORS_DISGUISE_INFO = 98		-- 변술부 변장아이템

-- 스테이지 배경
STAGE_BG_LOGIN			= 0
STAGE_BG_SELECT_CHAR	= 1
STAGE_BG_CREATE_CHAR	= 2
STAGE_BG_GAME			= 3

-- 아이콘 슬롯 타입
SLOT_TOOLTIP				= 1		-- 툴팁
SLOT_ITEMBASE				= 2		-- 아이템 베이스 (모든 아이템 슬롯에서 동작할 공통 기능만 제공)
SLOT_SKILLBASE				= 3		-- 스킬 베이스	(모든 스킬 슬롯에서 동작할 공통 기능만 제공)
SLOT_INVENTORY				= 4		-- 인벤토리
SLOT_CHARINFO				= 5		-- 캐릭터 정보
SLOT_INVENTORY_BANK 		= 6		-- 인벤토리 은행
SLOT_INVENTORY_WISHLIST 	= 7		-- 인벤토리 위시리스트
SLOT_INVENTORY_TRASH		= 8		-- 인벤토리 휴지통
SLOT_SQS					= 9		-- 스킬 퀵슬롯
SLOT_IQS					= 10	-- 아이템 퀵슬롯
SLOT_SKILL					= 11	-- 스킬 슬롯
SLOT_VEHICLE				= 12	-- 탈것 슬롯
SLOT_NPCMARKET				= 13	-- NPC 판매 아이템 슬롯
SLOT_NPCCART				= 14	-- NPC 카트 아이템 슬롯
SLOT_PRODUCT				= 15	-- 제작
SLOT_TRADE					= 16	-- 거래창 
SLOT_POINTSHOP				= 17	-- PointShop
SLOT_POINTSHOP_CART			= 18	-- PointShop 카트
SLOT_PMSPOINTTRADE			= 19 	-- 개인 상점 검색 포인트 거래 아이템 등록 슬롯
SLOT_POSTBOX				= 21	-- 우편 슬롯
SLOT_POSTBOX_SUB			= 22	-- 우편 보조슬롯
SLOT_REFERCHAR				= 23	-- 정보보기창 슬롯
SLOT_PET					= 24	-- Pet 아이템 슬롯
SLOT_ATTENDANCE_AWARD		= 25	-- 생활기록부 출석부 슬롯
SLOT_ACTIVITY_SALES			= 26	-- 특별활동 판매아이템 슬롯
SLOT_MINIGAME_DICE_REWARD	= 27	-- 미니게임 : 주사위 슬롯;
SLOT_CLUBSTORAGE			= 28	-- 클럽 창고 슬롯
SLOT_REBUILD_CARD_ITEM		= 29    -- 개조카드 아이템 슬롯
SLOT_REBUILD_CARD_STEMP		= 30    -- 개조카드 스템프 슬롯
SLOT_ITEM_PREVIEW			= 31    -- 아이템 미리보기 슬롯
SLOT_PRIVATE_MARKET			= 32    -- 개인상점 슬롯
SLOT_LOCKER					= 33	-- 락커 슬롯
SLOT_REFERCHAR_SKILL		= 34	-- 정보보기 스킬 슬롯
SLOT_BUFF					= 35	-- 버프 슬롯
SLOT_SETITEM				= 36	-- 세트 아이템
SLOT_SUMMONSTATE			= 37	-- 소환수 슬롯
SLOT_PARTYDISTRIBUTION		= 38	-- 파티 분배 슬롯
SLOT_PRIVATEMARKETSEARCH	= 39	-- 개인 상점 검색 슬롯
SLOT_ITEMLINKINFO			= 40	-- 아이템 링크 정보
SLOT_SELFORMBOX				= 41	-- 선택형 상자 슬롯

-- NPC 대화 답변 타입
TALK_FIRST_TIME 	= 1
TALK_QUEST_STEP 	= 2
TALK_QUEST_START 	= 3
TALK_QUEST_ING 		= 4
TALK_BASIC_TALK 	= 5

-- NPC 대화 액션 타입
EM_BASIC_NULL 		= -1
EM_STORAGE 			= 0		--창고
EM_MARKET 			= 1		--상점
EM_CURE 			= 2		--치료
EM_STARTPOINT 		= 3		--시작위치 지정
EM_CHAR_RESET 		= 4		--stats, skill 리셋
EM_ITEM_TRADE 		= 5		--item 교환
EM_BUSSTATION 		= 6		--버스 정류장
EM_CLUB_NEW 		= 7		--클럽 생성
EM_CLUB_UP 			= 8		--클럽 랭크 업
EM_CD_CERTIFY 		= 9		--시디 인증하기
EM_COMMISSION 		= 10	--상업 수수료 설정
EM_CLUB_STORAGE 	= 11	--클럽 창고
EM_ITEM_REBUILD 	= 12	--아이템 개조
EM_ODDEVEN 			= 13	--홀짝게임
EM_RECOVERY_EXP 	= 14	--경험치 회복
EM_RANDOM_PAGE 		= 15	--임의대화
EM_ITEMSEARCH_PAGE 	= 16	--아이템 검색
EM_ATTENDANCE_BOOK 	= 17	--출석부
EM_ITEM_MIX 		= 18	--

-- 맵 NPC 퀘스트 타입
MAP_NPC_QUEST_START 	= 0
MAP_NPC_QUEST_STEP 		= 1
MAP_NPC_QUEST_FINISH 	= 2

-- 맵 몬스터 타입
MAP_MOB_TYPE_QUEST		= 0
MAP_MOB_TYPE_BOSS		= 1
MAP_MOB_TYPE_VERY_WEAK	= 2
MAP_MOB_TYPE_WEAK		= 3
MAP_MOB_TYPE_NORMAL		= 4
MAP_MOB_TYPE_STRONG		= 5

-- 클럽 마크 사이즈
CLUB_MARK_SIZE_WIDTH = 16
CLUB_MARK_SIZE_HEIGHT = 11

-- 채팅 매크로 최대 개수
CHAT_MACRO_NUM				= 10
-- 최대 파티 인원
PARTY_MAX					= 8

-- 렌더 텍스처 ID
EMWIDGET_TEXTURE_DICE				= 1		-- 미니게임 : 주사위;
EMWIDGET_TEXTURE_ITEM_PREVIEW		= 2		-- 아이템 미리보기;
 
-- 채팅 로그 ID
CHAT_LOG_COMMAND_CONSOLE 	= 0
CHAT_LOG_GLOBAL 			= 1
CHAT_LOG_BASIC				= 2
CHAT_LOG_GROUPCHAT			= 3
CHAT_LOG_BASIC_PARTY		= 4
CHAT_LOG_BASIC_CLUB			= 5
CHAT_LOG_BASIC_ALLIANCE		= 6
CHAT_LOG_BASIC_WHISPER		= 7
CHAT_LOG_BASIC_SYSTEM		= 8

-- 채팅 타입 정의
CHAT_NO					= 0x0000	-- 메시지 없음
CHAT_NORMAL				= 0x0001	-- 일반
CHAT_PRIVATE			= 0x0002	-- 귓속말
CHAT_PARTY				= 0x0004	-- 파티
CHAT_MASTER				= 0x0008	-- 원정대 마스터
CHAT_GUILD				= 0x0010	-- 클럽
CHAT_ALLIANCE			= 0x0020	-- 동맹
CHAT_AREA				= 0x0040	-- 지역
CHAT_PARTY_RECRUIT		= 0x0080	-- 파티모집
CHAT_TOALL				= 0x0100	-- 확성기
CHAT_FACTION			= 0x0200	-- 진영
CHAT_SYSTEM				= 0x0400	-- 시스템
CHAT_COMMAND			= 0x0800	-- 커맨드

CHAT_PRIVATE_PARTY_RECRUIT = 0x1000 -- 귓속말을 통해 보내는 파티모집 관련 매세지

-- 모든 채팅
CHAT_ALL = CHAT_NORMAL | CHAT_PRIVATE | CHAT_PARTY | CHAT_MASTER | CHAT_GUILD | CHAT_ALLIANCE | 
CHAT_PARTY_RECRUIT |CHAT_AREA | CHAT_TOALL | CHAT_FACTION | CHAT_PRIVATE_PARTY_RECRUIT | CHAT_SYSTEM

-- 채팅 심볼 정의
CHAT_SYMBOL_NORMAL 			= 0		-- 일반
CHAT_SYMBOL_PRIVATE 		= 1		-- 귓속말
CHAT_SYMBOL_PARTY 			= 2		-- 파티
CHAT_SYMBOL_GUILD 			= 3		-- 클럽
CHAT_SYMBOL_ALLIANCE 		= 4		-- 동맹
CHAT_SYMBOL_AREA 			= 5		-- 지역
CHAT_SYMBOL_PARTY_FIND	 	= 6		-- 파티찾기
CHAT_SYMBOL_FACTION			= 7		-- 진영
CHAT_SYMBOL_TOALL 			= 8		-- 확성기
CHAT_SYMBOL_SYSTEM 			= 9		-- 시스템	
CHAT_SYMBOL_REPLAY 			= 10	-- 답신
CHAT_SYMBOL_RECRUIT			= 11	-- 파티모집

-- 채팅 링크 최대 개수
CHAT_MAX_LINK_COUNT = 3

-- 채팅 심볼 문자열
CHAT_SYMBOL_STRING = 
{
	{ "/s", "/say", GetGameWordText( "CHAT_TYPE_SYMBOL_LOCAL", 0 ) },				-- 일반
	{ "/w", "/whisper", "@", GetGameWordText( "CHAT_TYPE_SYMBOL_LOCAL", 1 ) },		-- 귓속말
	{ "/p", "/party", "#", GetGameWordText( "CHAT_TYPE_SYMBOL_LOCAL", 2 ) },        -- 파티
	{ "/c", "/club", "%", GetGameWordText( "CHAT_TYPE_SYMBOL_LOCAL", 3 ) },         -- 클럽
	{ "/u", "/union", "!", GetGameWordText( "CHAT_TYPE_SYMBOL_LOCAL", 4 ) }, 		-- 동맹
	{ "/1" },																		-- 지역
	{ "/2" },                      													-- 파티 찾기
	{ "/3" },																		-- 진영
	{ "/y", "/yell", "$", GetGameWordText( "CHAT_TYPE_SYMBOL_LOCAL", 7 ) }, 		-- 확성기
	{ "/n", "/notice", "&", GetGameWordText( "CHAT_TYPE_SYMBOL_LOCAL", 8 ) },		-- 시스템
	{ "/r", "/reply", GetGameWordText( "CHAT_TYPE_SYMBOL_LOCAL", 9 ) },				-- 답신
	{ GetGameWordText( "CHAT_LINK_PARTY_RECRUIT", 0 ) },							-- 파티모집
}

-- 텍스트 컬러
TEXT_COLOR_WHITE 			= 0xFFFFFF
TEXT_COLOR_BLACK 			= 0x000000
TEXT_COLOR_RED 				= 0xFF0000
TEXT_COLOR_GREEN 			= 0x00FF00
TEXT_COLOR_BLUE 			= 0x0000FF
TEXT_COLOR_YELLOW 			= 0xFFFF00
TEXT_COLOR_CYAN 			= 0x00FFFF
TEXT_COLOR_MAGENTA 			= 0xFF00FF
TEXT_COLOR_PALEGREEN		= 0x98FB98
TEXT_COLOR_GOLD				= 0xFFD700
TEXT_COLOR_GRAY				= 0x888888
TEXT_COLOR_DARKRED 			= 0xAA0000
TEXT_COLOR_FORESTGREEN 		= 0x228B22
TEXT_COLOR_DARKGRAY			= 0xA9A9A9
TEXT_COLOR_LIGHTSKYBLUE 	= 0x87CEFA
TEXT_COLOR_ORANGE 			= 0xFFA500
TEXT_COLOR_GREENYELLOW		= 0xADFF2F
TEXT_COLOR_SKYBLUE			= 0x00B4FF
TEXT_COLOR_PURPLE			= 0x6E2FC7
TEXT_COLOR_PINK				= 0xF361DC
TEXT_COLOR_LIGHTPURPLE		= 0x7178FF

-- HTML 텍스트 컬러
HTML_TEXT_COLOR_WHITE 			= "#FFFFFF"
HTML_TEXT_COLOR_BLACK 			= "#000000"
HTML_TEXT_COLOR_RED 			= "#FF0000"
HTML_TEXT_COLOR_GREEN 			= "#00FF00"
HTML_TEXT_COLOR_BLUE 			= "#0000FF"
HTML_TEXT_COLOR_YELLOW 			= "#FFFF00"
HTML_TEXT_COLOR_CYAN 			= "#00FFFF"
HTML_TEXT_COLOR_MAGENTA 		= "#FF00FF"
HTML_TEXT_COLOR_PALEGREEN		= "#98FB98"
HTML_TEXT_COLOR_GOLD			= "#FFD700"
HTML_TEXT_COLOR_GRAY			= "#888888"
HTML_TEXT_COLOR_DARKRED 		= "#AA0000"
HTML_TEXT_COLOR_FORESTGREEN 	= "#228B22"
HTML_TEXT_COLOR_DARKGRAY		= "#A9A9A9"
HTML_TEXT_COLOR_LIGHTSKYBLUE	= "#87CEFA"
HTML_TEXT_COLOR_ORANGE			= "#FFA500"
HTML_TEXT_COLOR_GREENYELLOW		= "#ADFF2F"
HTML_TEXT_COLOR_SKYBLUE			= "#00B4FF"
HTML_TEXT_COLOR_PURPLE			= "#6E2FC7"
HTML_TEXT_COLOR_PINK			= "#F361DC"
HTML_TEXT_COLOR_DODGERBLUE      = '#1E90FF'
HTML_TEXT_COLOR_LIGHTPURPLE		= "#7178FF"

-- 아이템 겹침 개수 색
ITEMSLOT_COUNT_COLOR			= TEXT_COLOR_PALEGREEN	-- 일반
ITEMSLOT_COUNT_MAX_COLOR		= TEXT_COLOR_ORANGE		-- 최대 개수

-- HTML 태그
HTML_TAB 		= "&#9<;>"		-- 탭 문자
HTML_GT 		= "&gt<;>"		-- >
HTML_LT 		= "&lt<;>"		-- <
HTML_AMP		= "&amp<;>"		-- &
HTML_QUOT		= "&quot<;>"	-- "
HTML_APOS		= "&apos<;>"	-- '
HTML_NEWLINE	= "<br>"		-- 개행

-- 서비스 제공업자
SP_OFFICE_TEST	=  0	-- Korea(Mincoms), 내부서버
SP_KOREA		=  1	-- Korea(Daum game)(Korea), IDC 서버, Web Start
SP_TAIWAN		=  2	-- Taiwan
SP_CHINA		=  3	-- China
SP_JAPAN		=  4	-- Japan
SP_PHILIPPINES	=  5	-- Philippines
SP_THAILAND		=  6	-- Thailand
SP_GLOBAL		=  7	-- Global Service (GSP)
SP_MALAYSIA		=  8	-- Malaysia
SP_GS			=  9	-- Global Service
SP_INDONESIA	= 10	-- Indonesia
SP_MALAYSIA_EN	= 11	-- Malaysia English
SP_VIETNAM		= 12	-- Vietnam
SP_HONGKONG		= 13	-- HongKong
SP_KOREA_TEST	= 14	-- Korea Test Server, IDC, Web Start
SP_WORLD_BATTLE	= 15	-- World Battle
SP_EU			= 16	-- EU, Games Masters
SP_US			= 17	-- US, GameSamba
SP_TOTAL_NUM	= 18

EMSERVICE_DEFAULT		= 0
EMSERVICE_KOREA			= 1
EMSERVICE_FEYA			= 2
EMSERVICE_MALAYSIA_CN	= 3
EMSERVICE_MALAYSIA_EN	= 4
EMSERVICE_JAPAN			= 5
EMSERVICE_THAILAND		= 6
EMSERVICE_CHINA			= 7
EMSERVICE_INDONESIA		= 8
EMSERVICE_PHILIPPINES	= 9
EMSERVICE_VIETNAM		= 10
EMSERVICE_GLOBAL		= 11	-- 글로벌 서비스
EMSERVICE_WORLD_BATTLE  = 12	-- World battle
EMSERVICE_EU			= 13	-- Games-Masters
EMSERVICE_US			= 14	-- GameSamba

-- Q-box 타입
Q_BOX_SPEED_UP 		= 1		-- 스피드 업
Q_BOX_CRAZY_TIME 	= 2		-- 크레이지 타임
Q_BOX_POWER_UP 		= 3		-- 파워 업
Q_BOX_EXP_TIME 		= 4		-- EXP 타임
Q_BOX_LUCKY 		= 6		-- 럭키
Q_BOX_BOSS_GEN 		= 8		-- Oh, No(보스젠)
Q_BOX_SPEED_UP_MAX 	= 9		-- 스피드 업 맥스
Q_BOX_MADNESS_TIME 	= 10	-- 매드니스 타임
Q_BOX_POWER_UP_MAX	= 11	-- 파워 업 맥스
Q_BOX_HEAL			= 12	-- 힐

-- UI 관련 단축키 타입
EMSHORTCUTS_QUICKSLOT0 = 1
EMSHORTCUTS_QUICKSLOT1 = 2
EMSHORTCUTS_QUICKSLOT2 = 3
EMSHORTCUTS_QUICKSLOT3 = 4
EMSHORTCUTS_QUICKSLOT4 = 5
EMSHORTCUTS_QUICKSLOT5 = 6
EMSHORTCUTS_SKILLSLOT0 = 7
EMSHORTCUTS_SKILLSLOT1 = 8
EMSHORTCUTS_SKILLSLOT2 = 9
EMSHORTCUTS_SKILLSLOT3 = 10
EMSHORTCUTS_SKILLSLOT4 = 11
EMSHORTCUTS_SKILLSLOT5 = 12
EMSHORTCUTS_SKILLSLOT6 = 13
EMSHORTCUTS_SKILLSLOT7 = 14
EMSHORTCUTS_SKILLSLOT8 = 15
EMSHORTCUTS_SKILLSLOT9 = 16

EMSHORTCUTS_INVENTORY = 33
EMSHORTCUTS_CHARACTER = 34
EMSHORTCUTS_SKILL = 35
EMSHORTCUTS_PARTY = 36
EMSHORTCUTS_QUEST = 37
EMSHORTCUTS_CLUB = 38
EMSHORTCUTS_FRIEND = 39
EMSHORTCUTS_ITEMBANK = 40
EMSHORTCUTS_MINIMAP = 41
EMSHORTCUTS_POINTSHOP = 42
EMSHORTCUTS_CHATMACRO = 43
EMSHORTCUTS_HELP = 45
EMSHORTCUTS_PET = 47
EMSHORTCUTS_PRODUCT = 49
EMSHORTCUTS_FOURCEDPKMODE = 54
EMSHORTCUTS_RUNMODE = 55
EMSHORTCUTS_PEACEMODE = 56

EMSHORTCUTS_CAMERA_UP = 59
EMSHORTCUTS_CAMERA_DOWN = 60
EMSHORTCUTS_CAMERA_LEFT = 61
EMSHORTCUTS_CAMERA_RIGHT = 62

EMSHORTCUTS_CAMERA_MOVE_FRONT = 80
EMSHORTCUTS_CAMERA_MOVE_BACK = 81
EMSHORTCUTS_CAMERA_MOVE_LEFT = 82
EMSHORTCUTS_CAMERA_MOVE_RIGHT = 83
EMSHORTCUTS_CAMERA_MOVE_UP = 84
EMSHORTCUTS_CAMERA_MOVE_DOWN = 85
EMSHORTCUTS_FLYCAMERACONTROL = 86
EMSHORTCUTS_CAMERA_INCREASE_MOVESPEED = 87
EMSHORTCUTS_CAMERA_DECREASE_MOVESPEED = 88
EMSHORTCUTS_CAMERA_INCREASE_ROTATESPEED = 89
EMSHORTCUTS_CAMERA_DECREASE_ROTATESPEED = 90
EMSHORTCUTS_CAMERA_TARGET_FIXEDMODE = 91
EMSHORTCUTS_CAMERA_INCREASE_INTERPOLATION_MOVETIME = 92
EMSHORTCUTS_CAMERA_DECREASE_INTERPOLATION_MOVETIME = 93
EMSHORTCUTS_CAMERA_CLEAR_INTERPOLATION_TIME = 94
EMSHORTCUTS_CAMERA_INCREASE_INTERPOLATION_ROTATETIME = 95
EMSHORTCUTS_CAMERA_DECREASE_INTERPOLATION_ROTATETIME = 96
EMSHORTCUTS_CAMERA_INCREASE_INTERPOLATION_TARGETSELECTTIME = 97
EMSHORTCUTS_CAMERA_DECREASE_INTERPOLATION_TARGETSELECTTIME = 98
EMSHORTCUTS_CAMERA_PLAY = 99
EMSHORTCUTS_CAMERA_STOP = 100
EMSHORTCUTS_CAMERA_REWIND_PLAY = 101

-- 트윈 Easing Type
-- 해당 Easing Type 참고 - http://www.republicofcode.com/tutorials/flash/as3tweenclass/
EASE_REGULAR_OUT 		= 0
EASE_REGULAR_INOUT 		= 1
EASE_REGULAR_IN 		= 2
EASE_BOUNCE_OUT	 		= 3
EASE_BOUNCE_INOUT 		= 4
EASE_BOUNCE_IN 			= 5
EASE_BACK_OUT 			= 6
EASE_BACK_INOUT 		= 7
EASE_BACK_IN 			= 8
EASE_ELASTIC_OUT 		= 9
EASE_ELASTIC_INOUT 		= 10
EASE_ELASTIC_IN 		= 11
EASE_STRONG_OUT 		= 12
EASE_STRONG_INOUT 		= 13
EASE_STRONG_IN 			= 14
EASE_NONE		 		= 15

-- 그룹채팅 초대 종류(GroupChatClient에 Enum값도 수정해야함 수정시)

GROUPCHAT_INVITE_PTOP 		= 0 -- PtoP창으로 초대
GROUPCHAT_INVITE_FRIEND 	= 1 -- 친구들 중에 초대
GROUPCHAT_INVITE_CLUBMEMBER = 2 -- 클럽 멤버들 중에 초대

-- 캐릭터가 받은 초대 종류(해당 변수명과 값은 변경되어서는 않된다.)(1. 친구초대; 2. 그룹쳇팅; 3. 클럽초대; 4. 파티초대)

INVITATION_REPORT_FRIEND			= 1
INVITATION_REPORT_GROUPCHAT			= 2
INVITATION_REPORT_CLUB				= 3
INVITATION_REPORT_PARTY				= 4

-- 정보보기창 테이블 키값에 따른 정보
REFERCHAR_CLASS 			= 1		-- 클래스
REFERCHAR_SCHOOL 			= 2		-- 학교
REFERCHAR_SEX				= 3		-- 성별
REFERCHAR_HAIR 				= 4		-- 머리스타일
REFERCHAR_HAIRCOLOR			= 5		-- 머리색
REFERCHAR_FACE 				= 6		-- 얼굴
REFERCHAR_LEVEL 			= 7		-- 레벨
REFERCHAR_POW 				= 8		-- 힘
REFERCHAR_DEX 				= 9		-- 민첩
REFERCHAR_SPI 				= 10	-- 정신
REFERCHAR_STR 				= 11	-- 체력
REFERCHAR_STA 				= 12	-- 근력
REFERCHAR_ATTACK_MIN		= 13	-- 최소공격력
REFERCHAR_ATTACK_MAX		= 14	-- 최대공격력
REFERCHAR_DEFENCE			= 15	-- 방어력
REFERCHAR_MELEE 			= 16	-- 격투치
REFERCHAR_RANGE 			= 17	-- 사격치
REFERCHAR_MAGIC 			= 18	-- 마력치, 기력치
REFERCHAR_HIT				= 19	-- 명중률
REFERCHAR_AVOID				= 20	-- 회피율
REFERCHAR_HP 				= 21	-- HP
REFERCHAR_MP 				= 22	-- MP
REFERCHAR_SP 				= 23	-- SP
REFERCHAR_CRITICAL 			= 24	-- 크리티컬 
REFERCHAR_CRUSHINGBLOW 		= 25	-- 강한타격
REFERCHAR_MOVESPEED 		= 26	-- 이동속도
REFERCHAR_ATKSPEED 			= 27	-- 공격속도
REFERCHAR_EXPRATE 			= 28	-- 경험치 증가율
REFERCHAR_INCR_HP 			= 29	-- HP회복률
REFERCHAR_INCR_MP 			= 30	-- MP회복률
REFERCHAR_INCR_SP 			= 31	-- SP회복률
REFERCHAR_INCR_CP 			= 32	-- CP회복률
REFERCHAR_POTION_HP 		= 33	-- HP포션 증가량
REFERCHAR_POTION_MP 		= 34	-- MP포션 증가량
REFERCHAR_POTION_SP 		= 35	-- SP포션 증가량
REFERCHAR_RESIST_FIRE		= 36	-- 화염저항
REFERCHAR_RESIST_POISON		= 37	-- 독저항
REFERCHAR_RESIST_ICE		= 38	-- 냉기저항
REFERCHAR_RESIST_SPIRIT		= 39	-- 정기저항
REFERCHAR_RESIST_ELECTRIC	= 40	-- 전기저항
REFERCHAR_BRIGHT			= 41	-- 성향
REFERCHAR_LIVING			= 42	-- 생활점수
REFERCHAR_CSB				= 43	-- 기여도

-- 학교 구분
SCHOOL_SUNGMOON		= 0 -- 성문학원
SCHOOL_HYUNAM		= 1 -- 현암학원
SCHOOL_BONGHWANG	= 2 -- 봉황학원
SCHOOL_ALL			= 3 -- 모든학원


--클럽창 리스트 정렬 
ORDER_CHA_LEVEL = 0 -- 레벨 정렬
ORDER_CHA_NAME 	= 1 -- 이름정렬
ORDER_MAP 		= 2 -- 맵 정렬
ORDER_FLAG 		= 3 -- 계급 정렬

SQL_ASC 	= 0 -- 오름차순
SQL_DESC 	= 1 -- 내림차순

--클럽창 로그 타입
LOG_ALL               =  0 --! 모든 로그
LOG_MEMBER            =  1 --! 멤버 탈퇴/삭제
LOG_CDM               =  2 --! Club Death Math
LOG_GUIDANCE          =  3 --! 선도클럽
LOG_CLUB_STORAGE      =  4 --! 클럽 창고
LOG_CLUB_NOTICE       =  5 --! 클럽 공지 변경
LOG_CLUB_BATTLE       =  6 --! 클럽 Battle
LOG_CLUB_ALLIANCE     =  7 --! 클럽 동맹
LOG_CLUB_RANK_UP      =  8 -- 클럽 랭크업
LOG_CLUB_AUTH_CHANGE  =  9 --! 클럽 권한 관련
LOG_ENCHANT           = 10 --! 아이템 개조 관련
LOG_PARTY_RECRUIT     = 11 --! 파티 모집
LOG_KILLED_BY_MONSTER = 12 --! 몬스터에게 사망함
LOG_KILL_MONSTER      = 13 --! 몬스터를 쓰러뜨림
LOG_CTF               = 14 --! 점령전 로그
LOG_MEMBER_LEVEL_UP   = 15 --! 클럽 멤버 레벨업
LOG_KILL_PLAYER       = 16 --! 플레이어를 쓰러뜨림

-- HTML Web View Binding ID
WEB_VIEW_ID_HELP 	= 0		-- 도움말

-- 요일
TIME_SUNDAY		 = 1
TIME_MONDAY		 = 2
TIME_TUESDAY	 = 3
TIME_WEDNESDAY	 = 4
TIME_THURSDAY	 = 5
TIME_FRIDAY		 = 6
TIME_SATURDAY	 = 7

-- 코스튬 능력치 종류 ------------------------------------------------------
-- 일반 효과
CS_EXPMULTIPLE		= 1		-- 경험치(-327.00% ~ +327.00%)치
CS_DEFENCE			= 2		-- 방어력력
CS_RESIST			= 3		-- 저항
CS_EMPTY			= 4
CS_GENERAL_MAX		= CS_EMPTY

-- 가산 효과			
CS_ADD_HITRATE		= 5		-- 명중률
CS_ADD_AVOIDRATE	= 6		-- 회피율
CS_ADD_DAMAGE		= 7		-- 공격력
CS_ADD_DEFENSE		= 8		-- 방어력
CS_ADD_HP			= 9		-- 체력(HP)
CS_ADD_STATS_POW	= 10	-- 힘
CS_ADD_STATS_STA	= 11	-- 체력
CS_ADD_STATS_SPI	= 12	-- 정신
CS_ADD_STATS_DEX	= 13	-- 민첩
CS_ADD_ENERGY		= 14	-- 기력치
CS_ADD_SHOOTING		= 15	-- 사격치
CS_ADD_MELEE		= 16	-- 격투치
CS_ADD_EMPTY1		= 17
CS_ADD_EMPTY2		= 18
CS_ADD_EMPTY3		= 19
CS_ADDED_MAX		= CS_ADD_EMPTY3 - CS_EMPTY

-- 변화율				
CS_R_HP_INC			= 20	-- HP 증가율
CS_R_MP_INC			= 21	-- MP 증가율
CS_R_SP_INC			= 22	-- SP 증가율
CS_R_HMSP_INC		= 23	-- HP+MP+SP 증가율
CS_R_MOVESPEED		= 24	-- 이동속도 증가율
CS_R_ATTACKSPEED	= 25	-- 공격속도 증가율
CS_R_CRITICALHIT	= 26	-- 크리티컬 발생확률
CS_R_STRIKEHIT		= 27	-- 강한타격 발생확률
CS_R_EMPTY			= 28
CS_VARRATCS_MAX		= CS_R_EMPTY - CS_ADD_EMPTY3
----------------------------------------------------------------------
-- enum GLITEM_ATT참조
ITEMATT_NOTHING	= 0	--	착용무기없음.
ITEMATT_SWORD	= 1	--	검.
ITEMATT_SABER	= 2	--	도.
ITEMATT_DAGGER	= 3	--	단검.
ITEMATT_SPEAR	= 4	--	창.
ITEMATT_STICK	= 5	--	몽둥이.
ITEMATT_GWON	= 6	--	권.
ITEMATT_BOW		= 7	--	활.
ITEMATT_THROW	= 8	--	투척.
ITEMATT_PISTOL		= 9	--	권총
ITEMATT_RAILGUN		= 10	--	소총1
ITEMATT_PORTALGUN	= 11	--	소총2
ITEMATT_HAMMER		= 12   -- 해머.
ITEMATT_DUALSPEAR	= 13   -- 듀얼 스피어.
ITEMATT_SIDE		= 14   -- 사이드.
ITEMATT_EXTREME_GLOVE	= 15	-- 극강 글러브.
ITEMATT_TRICK_STICK	= 16 -- 마술 지팡이;
ITEMATT_TRICK_BOX	= 17 -- 마술 상자;
ITEMATT_TRICK_WHIP	= 18	-- 마술 채찍;
ITEMATT_ACTOR_SHIELD = 19 -- 방패
ITEMATT_ACTOR_HAMMER = 20 -- 망치
ITEMATT_ACTOR_UMBRELLA = 21 -- 우산
ITEMATT_WEAPON_STR = 22		-- 힘무기
ITEMATT_WEAPON_DEX = 23		-- 민첩무기
ITEMATT_WEAPON_SPI = 24		-- 정신무기

ITEMATT_NOCARE		= 25	--	타입 상관 안함.	( "GLSKILL" 에서 Hand 무기 제한 속성에서 사용됨. )
ITEMATT_NSIZE		= 25	--	종류갯수.
------------------------------------------------------------------------------------------------------------------------

-- 테이블 복사
function CopyTable( tb )

	local tbNew = {}
	for value in pairs( tb ) do
	
		if "table" ~= type( tb[ value ] ) then
			tbNew[ value ] = tb[ value ]
		else
			tbNew[ value ] = CopyTable( tb[ value ] )
		end	
	
	end
	
	return tbNew

end

-- 스트링 포맷
-- %1%, %2%, ... 일 경우
function StringFormat( strText, ... )

	local nPattern = 1
	for i, value in ipairs{...} do
	
		local strPattern = "%%" .. tostring( nPattern ) .. "%%"
		strText = strText:gsub( strPattern, tostring( value ) )
				
		nPattern = nPattern + 1
	
	end
	
	return strText

end

-- %1$.2f   %2$.0f   이런 서식문자 대응하기위해 만든 함수
function StringFormatPtEx( strText, ... )

	local nPattern = 1
	local nPatternEx_Start = 0
	local nPatternEx_End = 0
	for i, value in ipairs{...} do
	
		local strPattern = "%%" .. tostring( nPattern ) .. "%%"
		strText = strText:gsub( strPattern, tostring( value ) )
	
		local strPatternEx = "%%"..tostring(nPattern)
		nPatternEx_Start, nPatternEx_End = string.find(strText, strPatternEx, nPatternEx_Start)
		nPattern = nPattern + 1
		if nPatternEx_Start == nil or nPatternEx_End == nil then
			continue
		end
		
		local nPatternEx_End_Start = 0
		local nPatternEx_End_End = 0
		nPatternEx_End_Start, nPatternEx_End_End = string.find(strText, "%%", nPatternEx_End+1)
		
		local strFormat = ""
		local strConvert = ""
		strFormat = string.sub(strText, nPatternEx_End+1, nPatternEx_End_Start-1)

		local strGSub = strPatternEx..strFormat
		-- $만 넣으면 안된`````다. `서식문자 검색시 %제외하면 제대로 동작안함;
		strFormat = strFormat:replace("%$", "")
		strConvert = string.format("%"..strFormat, value)
		
		strText = strText:gsub( strGSub, strConvert )
	end
	return strText

end

-- %d, %d, ... 일 경우
function StringFormatEx( strText, ... )

	for i, value in ipairs{...} do
	
		local strPattern = "%%d"
		strText = strText:gsub( strPattern, tostring( value ) )
	
	end
	
	return strText

end

-- 파라미터 생성
function CreateParam( ... )

	local strParam = ""	
	for i, value in ipairs{...} do
	
		strParam = strParam .. tostring( value ) .. ";"
	
	end
	
	return strParam

end

-- 파라미터 생성 (테이블)
function CreateParamForTable( tb )

	local strParam = ""
	for value in pairs( tb ) do
	
		if "table" ~= type( tb[ value ] ) then
		
			strParam = strParam .. tostring( tb[ value ] ) .. ";"
		
		end
	
	end
	
	return strParam

end

--[[ 루아에서 숫자를 14자리까지 밖에 표현을 못해 클라이언트 인터페이스 함수로 옮김.
-- 숫자를 통화단위로 변환
function CurrencyFormat( nNumber )

	local str = string.reverse( tostring( nNumber ) )
	local nCommaCount = math.floor( string.len( str ) / 3 )
	if string.len( str ) % 3 == 0 then
		nCommaCount = nCommaCount - 1
	end
	
	local nCount = 0
	local ret = ""
	for char in str:gmatch "." do
	
		ret = char .. ret
		nCount = nCount + 1
		
		if nCount % 3 == 0 and 0 < nCommaCount then
			ret = "," .. ret
			nCommaCount = nCommaCount - 1
		end
		
	end
	
	return ret

end
]]

-- 통화단위 "," 삭제
function InvCurrencyFormat( strText )
	
	for i = 1, string.len( strText ), 1 do
	
		local strPattern = ","
		strText = strText:gsub( strPattern, "" )
	
	end
	
	return strText

end

-- HTML 태그 제거
function TrimHTMLTag( str )

	local strRet = ""
	local bHTML = false
	
	for i = 1, string.len( str ), 1 do
	
		local bContinue = false
		local strChar = SplitString( str, i, i )
		
		if false == bHTML then
			
			if strChar == "<" then
				bHTML = true
				bContinue = true
			end
			
		else
		
			if strChar == ">" then
				bHTML = false
				bContinue = true
			end
		
		end
		
		if false == bContinue and false == bHTML then
			strRet = strRet .. strChar
		end

	end
	
	strRet = strRet:gsub( "&#9;", " " )
	strRet = strRet:gsub( "&gt;", ">" )
	strRet = strRet:gsub( "&lt;", "<" )
	strRet = strRet:gsub( "&amp;", "&" )
	strRet = strRet:gsub( "&quot;", "\"" )
	strRet = strRet:gsub( "&apos;", "'" )
	
	return strRet

end

function TrimHTMLDataTag( str )

	str = str:gsub( "&#9<;>", "&#9;" )
	str = str:gsub( "&gt<;>", "&gt;" )
	str = str:gsub( "&lt<;>", "&lt;" )
	str = str:gsub( "&amp<;>", "&amp;" )
	str = str:gsub( "&quot<;>", "&quot;" )
	str = str:gsub( "&apos<;>", "&apos;" )
	
	return TrimHTMLTag( str )

end

-- 숫자를 HTML 컬러로 변환
function TranslateHTMLColor( nColor )

	local str = "#" .. string.format( "%x", nColor )
	return str

end

-- HTML 이미지
function HTMLImage( strSource, nWidth, nHeight )

	local str = "<img src=\"" .. strSource .. "\" width=\"" .. tostring( nWidth ) .. 
		"\" height=\"" .. tostring( nHeight ) .. "\">"

	return str
	
end

-- HTML 폰트 컬러
function HTMLFontColor( strText, strColor )

	local str = "<font color=\"" .. strColor .. "\">" .. strText .. "</font>"
	return str
	
end

-- HTML 폰커 컬러/크기
function HTMLFontColorSize( strText, strColor, nSize )

	local str = "<font color=\"" .. strColor .. "\"" .. 
		"size=\"" .. tostring( nSize ) .. "\">" .. strText .. "</font>"
		
	return str

end

-- 클래스에 따른 클럽표시용 텍스트 반환
function GetClubClassText(strClass)	
	if CHARCLASS_FIGHTER_M == strClass then 
		return GetGameWordText("WB_CLUB_BASIC_INFO_CLASS", 0)
	elseif CHARCLASS_ARMS_M == strClass then 
		return GetGameWordText("WB_CLUB_BASIC_INFO_CLASS", 1)
	elseif CHARCLASS_ARCHER_W == strClass then
		return GetGameWordText("WB_CLUB_BASIC_INFO_CLASS", 2)
	elseif CHARCLASS_SPIRIT_W == strClass then
		return GetGameWordText("WB_CLUB_BASIC_INFO_CLASS", 3)
	elseif CHARCLASS_EXTREAM_M == strClass then
		return GetGameWordText("WB_CLUB_BASIC_INFO_CLASS", 4)
	elseif CHARCLASS_EXTREAM_W == strClass then
		return GetGameWordText("WB_CLUB_BASIC_INFO_CLASS", 4)
	elseif CHARCLASS_FIGHTER_W == strClass then
		return GetGameWordText("WB_CLUB_BASIC_INFO_CLASS", 0)	
	elseif CHARCLASS_ARMS_W == strClass then
		return GetGameWordText("WB_CLUB_BASIC_INFO_CLASS", 1)
	elseif CHARCLASS_ARCHER_M == strClass then
		return GetGameWordText("WB_CLUB_BASIC_INFO_CLASS", 2)
	elseif CHARCLASS_SPIRIT_M == strClass then
		return GetGameWordText("WB_CLUB_BASIC_INFO_CLASS", 3)
	elseif CHARCLASS_SCIENTIST_M == strClass then
		return GetGameWordText("WB_CLUB_BASIC_INFO_CLASS", 5)
	elseif CHARCLASS_SCIENTIST_W == strClass then
		return GetGameWordText("WB_CLUB_BASIC_INFO_CLASS", 5)
	elseif CHARCLASS_ASSASSIN_M == strClass then
		return GetGameWordText("WB_CLUB_BASIC_INFO_CLASS", 6)
	elseif CHARCLASS_ASSASSIN_W == strClass then
		return GetGameWordText("WB_CLUB_BASIC_INFO_CLASS", 6)
	elseif CHARCLASS_TRICKER_M == strClass then
		return GetGameWordText("WB_CLUB_BASIC_INFO_CLASS", 7)
	elseif CHARCLASS_TRICKER_W == strClass then
		return GetGameWordText("WB_CLUB_BASIC_INFO_CLASS", 7)
	elseif CHARCLASS_ACTOR_M == strClass then
		return GetGameWordText("WB_CLUB_BASIC_INFO_CLASS", 9)
	elseif CHARCLASS_ACTOR_W == strClass then
		return GetGameWordText("WB_CLUB_BASIC_INFO_CLASS", 9)
	else
		return "-"
	end
end
-- 클래스에 따른 툴팁용 텍스트 반환
function GetToolTipClassText(strClass)	
	if CHARCLASS_FIGHTER_M == strClass or CHARCLASS_FIGHTER_W == strClass then 
		return GetGameWordText("PRIVATE_MARKET_ITEM_REQUIRE_CLASS", 1)
	elseif CHARCLASS_ARMS_M == strClass or CHARCLASS_ARMS_W == strClass then 
		return GetGameWordText("PRIVATE_MARKET_ITEM_REQUIRE_CLASS", 2)
	elseif CHARCLASS_EXTREAM_M == strClass or CHARCLASS_EXTREAM_W == strClass then
		return GetGameWordText("PRIVATE_MARKET_ITEM_REQUIRE_CLASS", 5)
	elseif CHARCLASS_ARCHER_M == strClass or CHARCLASS_ARCHER_W == strClass then
		return GetGameWordText("PRIVATE_MARKET_ITEM_REQUIRE_CLASS", 3)
	elseif CHARCLASS_SPIRIT_M == strClass or CHARCLASS_SPIRIT_W == strClass then
		return GetGameWordText("PRIVATE_MARKET_ITEM_REQUIRE_CLASS", 4)
	elseif CHARCLASS_SCIENTIST_M == strClass or CHARCLASS_SCIENTIST_W == strClass then
		return GetGameWordText("PRIVATE_MARKET_ITEM_REQUIRE_CLASS", 6)
	elseif CHARCLASS_ASSASSIN_M == strClass or CHARCLASS_ASSASSIN_W == strClass then
		return GetGameWordText("PRIVATE_MARKET_ITEM_REQUIRE_CLASS", 7)
	elseif CHARCLASS_TRICKER_M == strClass or CHARCLASS_TRICKER_W == strClass then
		return GetGameWordText("PRIVATE_MARKET_ITEM_REQUIRE_CLASS", 8)
	elseif CHARCLASS_ACTOR_M == strClass or CHARCLASS_ACTOR_W == strClass then
		return GetGameWordText("PRIVATE_MARKET_ITEM_REQUIRE_CLASS", 9)
	end
	
	return ""
end
-- 클래스에 따른 클럽표시용 텍스트 반환
function GetItemToolTip_ClassText( nIndex )
	if CHARCLASS_IDX_FIGHTER_M == nIndex then 
		return GetGameWordText("ITEM_SHOP_SEARCH_CLASS", 0)
	elseif CHARCLASS_IDX_ARMS_M == nIndex then 
		return GetGameWordText("ITEM_SHOP_SEARCH_CLASS", 1)
	elseif CHARCLASS_IDX_ARCHER_W == nIndex then
		return GetGameWordText("ITEM_SHOP_SEARCH_CLASS", 2)
	elseif CHARCLASS_IDX_SPIRIT_W == nIndex then
		return GetGameWordText("ITEM_SHOP_SEARCH_CLASS", 3)
	elseif CHARCLASS_IDX_EXTREAM_M == nIndex then
		return GetGameWordText("ITEM_SHOP_SEARCH_CLASS", 4)
	elseif CHARCLASS_IDX_EXTREAM_W == nIndex then
		return GetGameWordText("ITEM_SHOP_SEARCH_CLASS", 4)
	elseif CHARCLASS_IDX_FIGHTER_W == nIndex then
		return GetGameWordText("ITEM_SHOP_SEARCH_CLASS", 0)	
	elseif CHARCLASS_IDX_ARMS_W == nIndex then
		return GetGameWordText("ITEM_SHOP_SEARCH_CLASS", 1)
	elseif CHARCLASS_IDX_ARCHER_M == nIndex then
		return GetGameWordText("ITEM_SHOP_SEARCH_CLASS", 2)
	elseif CHARCLASS_IDX_SPIRIT_M == nIndex then
		return GetGameWordText("ITEM_SHOP_SEARCH_CLASS", 3)
	elseif CHARCLASS_IDX_SCIENTIST_M == nIndex then
		return GetGameWordText("ITEM_SHOP_SEARCH_CLASS", 5)
	elseif CHARCLASS_IDX_SCIENTIST_W == nIndex then
		return GetGameWordText("ITEM_SHOP_SEARCH_CLASS", 5)
	elseif CHARCLASS_IDX_ASSASSIN_M == nIndex then
		return GetGameWordText("ITEM_SHOP_SEARCH_CLASS", 6)
	elseif CHARCLASS_IDX_ASSASSIN_W == nIndex then
		return GetGameWordText("ITEM_SHOP_SEARCH_CLASS", 6)
	elseif CHARCLASS_IDX_TRICKER_M == nIndex then
		return GetGameWordText("ITEM_SHOP_SEARCH_CLASS", 7)
	elseif CHARCLASS_IDX_TRICKER_W == nIndex then
		return GetGameWordText("ITEM_SHOP_SEARCH_CLASS", 7)
	elseif CHARCLASS_IDX_ACTOR_M == nIndex then
		return GetGameWordText("ITEM_SHOP_SEARCH_CLASS", 9)
	elseif CHARCLASS_IDX_ACTOR_W == nIndex then
		return GetGameWordText("ITEM_SHOP_SEARCH_CLASS", 9)
	else
		return "-"
	end
end

-- 모든 캐릭터 클래스 별 상수 머리 스타일 번호 읽어오기
function GetHairNum( UserClassType )

	if CHARCLASS_FIGHTER_M == UserClassType then
		return g_CONST_CHARCLASS0.dwHAIRNUM
	elseif CHARCLASS_ARMS_M == UserClassType then	
		return g_CONST_CHARCLASS1.dwHAIRNUM
	elseif CHARCLASS_ARCHER_W == UserClassType then
		return g_CONST_CHARCLASS2.dwHAIRNUM
	elseif CHARCLASS_SPIRIT_W == UserClassType then
		return g_CONST_CHARCLASS3.dwHAIRNUM
	elseif CHARCLASS_EXTREAM_M == UserClassType then
		return g_CONST_CHARCLASS4.dwHAIRNUM
	elseif CHARCLASS_EXTREAM_W == UserClassType then
		return g_CONST_CHARCLASS5.dwHAIRNUM
	elseif CHARCLASS_FIGHTER_W == UserClassType then
		return g_CONST_CHARCLASS6.dwHAIRNUM
	elseif CHARCLASS_ARMS_W == UserClassType then
		return g_CONST_CHARCLASS7.dwHAIRNUM
	elseif CHARCLASS_ARCHER_M == UserClassType then
		return g_CONST_CHARCLASS8.dwHAIRNUM
	elseif CHARCLASS_SPIRIT_M == UserClassType then
		return g_CONST_CHARCLASS9.dwHAIRNUM
	elseif CHARCLASS_SCIENTIST_M == UserClassType then
		return g_CONST_CHARCLASS10.dwHAIRNUM
	elseif CHARCLASS_SCIENTIST_W == UserClassType then
		return g_CONST_CHARCLASS11.dwHAIRNUM
	elseif CHARCLASS_ASSASSIN_M == UserClassType then
		return g_CONST_CHARCLASS12.dwHAIRNUM
	elseif CHARCLASS_ASSASSIN_W == UserClassType then
		return g_CONST_CHARCLASS13.dwHAIRNUM
	elseif CHARCLASS_TRICKER_M == UserClassType then
		return g_CONST_CHARCLASS14.dwHAIRNUM
	elseif CHARCLASS_TRICKER_W == UserClassType then
		return g_CONST_CHARCLASS15.dwHAIRNUM
	elseif CHARCLASS_ETC_M == UserClassType then
		return g_CONST_CHARCLASS16.dwHAIRNUM
	elseif CHARCLASS_ETC_W == UserClassType then
		return g_CONST_CHARCLASS17.dwHAIRNUM
	elseif CHARCLASS_ACTOR_M == UserClassType then
		return g_CONST_CHARCLASS18.dwHAIRNUM
	elseif CHARCLASS_ACTOR_W == UserClassType then
		return g_CONST_CHARCLASS19.dwHAIRNUM
	end
	return -1	
end

-- 모든 캐릭터 클래스 별 상수 얼굴 스타일 번호 읽어오기
function GetHeadNum( UserClassType )

	if CHARCLASS_FIGHTER_M == UserClassType then
		return g_CONST_CHARCLASS0.dwHEADNUM
	elseif CHARCLASS_ARMS_M == UserClassType then	
		return g_CONST_CHARCLASS1.dwHEADNUM
	elseif CHARCLASS_ARCHER_W == UserClassType then
		return g_CONST_CHARCLASS2.dwHEADNUM
	elseif CHARCLASS_SPIRIT_W == UserClassType then
		return g_CONST_CHARCLASS3.dwHEADNUM
	elseif CHARCLASS_EXTREAM_M == UserClassType then
		return g_CONST_CHARCLASS4.dwHEADNUM
	elseif CHARCLASS_EXTREAM_W == UserClassType then
		return g_CONST_CHARCLASS5.dwHEADNUM
	elseif CHARCLASS_FIGHTER_W == UserClassType then
		return g_CONST_CHARCLASS6.dwHEADNUM
	elseif CHARCLASS_ARMS_W == UserClassType then
		return g_CONST_CHARCLASS7.dwHEADNUM
	elseif CHARCLASS_ARCHER_M == UserClassType then
		return g_CONST_CHARCLASS8.dwHEADNUM
	elseif CHARCLASS_SPIRIT_M == UserClassType then
		return g_CONST_CHARCLASS9.dwHEADNUM
	elseif CHARCLASS_SCIENTIST_M == UserClassType then
		return g_CONST_CHARCLASS10.dwHEADNUM
	elseif CHARCLASS_SCIENTIST_W == UserClassType then
		return g_CONST_CHARCLASS11.dwHEADNUM
	elseif CHARCLASS_ASSASSIN_M == UserClassType then
		return g_CONST_CHARCLASS12.dwHEADNUM
	elseif CHARCLASS_ASSASSIN_W == UserClassType then
		return g_CONST_CHARCLASS13.dwHEADNUM
	elseif CHARCLASS_TRICKER_M == UserClassType then
		return g_CONST_CHARCLASS14.dwHEADNUM
	elseif CHARCLASS_TRICKER_W == UserClassType then
		return g_CONST_CHARCLASS15.dwHEADNUM
	elseif CHARCLASS_ETC_M == UserClassType then
		return g_CONST_CHARCLASS16.dwHEADNUM
	elseif CHARCLASS_ETC_W == UserClassType then
		return g_CONST_CHARCLASS17.dwHEADNUM
	elseif CHARCLASS_ACTOR_M == UserClassType then
		return g_CONST_CHARCLASS18.dwHEADNUM
	elseif CHARCLASS_ACTOR_W == UserClassType then
		return g_CONST_CHARCLASS19.dwHEADNUM
	end
	return -1
end


-- 요일 텍스트 반환
function GetDayText(_Day)
	if _Day == TIME_SUNDAY then
		return "XML, Sun"
	elseif _Day == TIME_MONDAY then
		return "XML, Mon"
	elseif _Day == TIME_TUESDAY then
		return "XML, Tues"
	elseif _Day == TIME_WEDNESDAY then
		return "XML, Wed"
	elseif _Day == TIME_THURSDAY then
		return "XML, Thur"
	elseif _Day == TIME_FRIDAY then
		return "XML, Fri"
	elseif _Day == TIME_SATURDAY then
		return "XML, Sat"
	else
		return "XML, Error"
	end
end
-- 소수점 처리함수
-- 값이 입력되어 있는 소수점 까지만 출력되도록 한다.
function SetFormatNumPoint( Num, Type )
	if 0 > Num then
		Num = Num * -1
	end

	if 0 == Type then
		return ( Num / 1 )
	elseif 1 == Type then
		return (string.format( "%.4f", tonumber( Num )) / 1)
	elseif 2 == Type then
		return (string.format( "%.4f", tonumber( Num * 100)) / 1)
	end
end


-- INT, Point
function SetFloatToString(Num, _until)
	if Num == nil or _until == nil then
		return ""
	end
	return string.format( "%."..tostring(_until).."f", tonumber(Num) )
end

function string:split( inSplitPattern, outResults )
  if not outResults then
    outResults = { }
  end
  local theStart = 1
  --local theStart = 0
  -- theSplitStart > theSplitEnd 경우가 가끔 뜨는데 왠지 모르겠다
  -- 위와 같은 경우는 서식문자 검사시 % 포함안시켜서 검색시 생김
  local theSplitStart, theSplitEnd = string.find( self, inSplitPattern, theStart )
--  if theSplitStart ~= nil and theSplitEnd ~= nil then
  while theSplitStart do
    table.insert( outResults, string.sub( self, theStart, theSplitStart-1 ) )
    theStart = theSplitEnd + 1
    theSplitStart, theSplitEnd = string.find( self, inSplitPattern, theStart )
  end
  table.insert( outResults, string.sub( self, theStart ) )
--  end
--  if theSplitEnd == nil then
--	table.insert( outResults, string.sub( self, GetStringLen( self ) ) )
--  end
  return outResults
end

-- 만든 의도
-- \r\n과 \n은 다르게 동작한다;
-- \r\n은 다음 문장과 현재 문장사이의 텀이 \n보다 더 길게됨
--[[			이것을 한번에 로드하면
<SENTENSE Ver="1" Id="SKILL_SPEC_ADDON_99">
	<VALUE Lang="kr">[지연 효과]</VALUE>
	<VALUE Lang="kr">%1%초후, 대상에게 %2%효과가 발동됩니다.</VALUE>
</SENTENSE> 
]]--  [지연 효과]\r\n이 기입된다;
-- \r\n을 없애기 위해..
function string:replace(inSplitPattern, inReplace)
	local tbSplit = self:split(inSplitPattern)
	local strResult = ""
	for i = 1, #tbSplit do
		if i ~= 1 then
			strResult = strResult..inReplace
		end
		strResult = strResult .. tbSplit[i]
	end
	return strResult
end

function string:Validation()
	if self == nil then
		return false
	end
	
	local strCheck = ""
	strCheck = self
	strCheck = strCheck:replace("\r\n", "")
	strCheck = strCheck:replace("\n", "")
	strCheck = strCheck:replace(" ", "")
	strCheck = strCheck:replace("\t", "")
	if string.len(strCheck) == 0 then
		return false
	end
	return true
end

function IsValidNativeID(wMID, wSID)
	if wMID ~= 65535 and wSID ~= 65535 then
		return true
	end
	return false
end

-- 해당 데이터는 GLCharDefine.h - EMACTIONTYPE와 같다;
LUAGLAT_IDLE	= 0
LUAGLAT_ATTACK	= 2
LUAGLAT_SKILL	= 3
LUAGLAT_DIE		= 12

function PropertyArraySetting(tbData, ...)
	for i, value in ipairs{...} do
		-- value는 무조건 문자열이다;
		table.insert(tbData, value)
	end
end

function lua_max(lhs, rhs)
	if lhs > rhs then
		return lhs
	--elseif rhs < lhs
	else
		return rhs
	end
end

function lua_min(lhs, rhs)
	if lhs < rhs then
		return lhs
	--elseif rhs < lhs
	else
		return rhs
	end
end

function LOWORD(_dw)
	return (_dw & 0xffff)
end

function HIWORD(_dw)
	return ((_dw >> 16) & 0xffff)
end

function AddNewLine( str, strAdd )

	if nil == str then
		return ""
	end
	
	if nil == strAdd then
		return str
	end

	if strAdd:Validation() == false then
		return str
	end
	
	str = str .. strAdd .. "\n"
	
	return str
	
end

function AddNewLineFront( str, strAdd )

	if nil == str then
		return ""
	end
	
	if nil == strAdd then
		return str
	end
	
	if strAdd:Validation() == false then
		return str
	end
	
	str = str .. "\n" .. strAdd
	
	return str

end

function AddStringSection( str, strAdd )

	if nil ~= str and nil ~= strAdd then
	
		if strAdd:Validation() == false then
			return str
		end
		
		return str .. "\n"
		
	elseif nil ~= str and nil == strAdd then
	
		return str
		
	elseif nil == str and nil ~= strAdd then
	
		return strAdd
		
	else
	
		return ""
		
	end

end

function table.shallow_copy(t)
  local t2 = {}
  for k,v in pairs(t) do
    t2[k] = v
  end
  return t2
end


function ConvertFrameToClass( nClassIndex )
	if nClassIndex == CHARCLASS_IDX_FIGHTER_M then
		return "FIGHTER_M"
	elseif nClassIndex == CHARCLASS_IDX_ARMS_M then
		return "ARMS_M"
	elseif nClassIndex == CHARCLASS_IDX_ARCHER_W then
		return "ARCHER_W"
	elseif nClassIndex == CHARCLASS_IDX_SPIRIT_W then
		return "SPIRIT_W"
	elseif nClassIndex == CHARCLASS_IDX_EXTREAM_M then
		return "EXTREME_M"
	elseif nClassIndex == CHARCLASS_IDX_EXTREAM_W then
		return "EXTREME_W"
	elseif nClassIndex == CHARCLASS_IDX_FIGHTER_W then
		return "FIGHTER_W"
	elseif nClassIndex == CHARCLASS_IDX_ARMS_W then
		return "ARMS_W"
	elseif nClassIndex == CHARCLASS_IDX_ARCHER_M then
		return "ARCHER_M"
	elseif nClassIndex == CHARCLASS_IDX_SPIRIT_M then
		return "SPIRIT_M"
	elseif nClassIndex == CHARCLASS_IDX_SCIENTIST_M then
		return "SCIENTIST_M"
	elseif nClassIndex == CHARCLASS_IDX_SCIENTIST_W then
		return "SCIENTIST_W"
	elseif nClassIndex == CHARCLASS_IDX_ASSASSIN_M then
		return "ASSASSIN_M"
	elseif nClassIndex == CHARCLASS_IDX_ASSASSIN_W then
		return "ASSASSIN_W"
	elseif nClassIndex == CHARCLASS_IDX_TRICKER_M then
		return "MAGICIAN_M"
	elseif nClassIndex == CHARCLASS_IDX_TRICKER_W then
		return "MAGICIAN_W"
	elseif nClassIndex == CHARCLASS_IDX_ACTOR_M then
		return "ActorM"
	elseif nClassIndex == CHARCLASS_IDX_ACTOR_W then
		return "ActorF"
	end
	
	return nil	
end

function CharClassToCharclassIndex(nFlag)
	if		nFlag == CHARCLASS_NONE				then 
		return -1
	elseif	nFlag == CHARCLASS_FIGHTER_M		then
		return CHARCLASS_IDX_FIGHTER_M
	elseif	nFlag == CHARCLASS_ARMS_M			then
		return CHARCLASS_IDX_ARMS_M
	elseif	nFlag == CHARCLASS_ARCHER_W			then
		return CHARCLASS_IDX_ARCHER_W
	elseif	nFlag == CHARCLASS_SPIRIT_W			then
		return CHARCLASS_IDX_SPIRIT_W
	elseif	nFlag == CHARCLASS_EXTREAM_M		then
		return CHARCLASS_IDX_EXTREAM_M
	elseif	nFlag == CHARCLASS_EXTREAM_W		then
		return CHARCLASS_IDX_EXTREAM_W
	elseif	nFlag == CHARCLASS_FIGHTER_W		then
		return CHARCLASS_IDX_FIGHTER_W
	elseif	nFlag == CHARCLASS_ARMS_W			then
		return CHARCLASS_IDX_ARMS_W
	elseif	nFlag == CHARCLASS_ARCHER_M			then
		return CHARCLASS_IDX_ARCHER_M
	elseif	nFlag == CHARCLASS_SPIRIT_M			then
		return CHARCLASS_IDX_SPIRIT_M
	elseif	nFlag == CHARCLASS_SCIENTIST_M 		then
		return CHARCLASS_IDX_SCIENTIST_M
	elseif	nFlag == CHARCLASS_SCIENTIST_W 		then
		return CHARCLASS_IDX_SCIENTIST_W
	elseif	nFlag == CHARCLASS_ASSASSIN_M 		then
		return CHARCLASS_IDX_ASSASSIN_M
	elseif	nFlag == CHARCLASS_ASSASSIN_W 		then
		return CHARCLASS_IDX_ASSASSIN_W
	elseif	nFlag == CHARCLASS_TRICKER_M 		then
		return CHARCLASS_IDX_TRICKER_M
	elseif	nFlag == CHARCLASS_TRICKER_W		then
		return CHARCLASS_IDX_TRICKER_W
	elseif	nFlag == CHARCLASS_ETC_M 			then
		return CHARCLASS_IDX_NUM_ETC
	elseif	nFlag == CHARCLASS_ETC_W 			then
		return CHARCLASS_IDX_NUM_ETC
	elseif	nFlag == CHARCLASS_ACTOR_M 			then
		return CHARCLASS_IDX_ACTOR_M
	elseif	nFlag == CHARCLASS_ACTOR_W 			then
		return CHARCLASS_IDX_ACTOR_W
	end
	return -1
end


--------------------------------------------------------------------------------
-- Trigger
FunctionTrigger = {}
FunctionTrigger.new = function ( )

	----------------------------------------------------------------------------
	-- FunctionTrigger instance
	local instance = {}
	
	
	----------------------------------------------------------------------------
	-- Private Variable
	local m_functionTrigger = nil
	local m_tableValue = {}
	
	
	----------------------------------------------------------------------------
	-- Function List
	instance.ExecuteFunction = function ( )
		
		if nil ~= m_functionTrigger then
			
			local nValueCount = #m_tableValue;
			if 0 == nValueCount then
					
				m_functionTrigger()
					
			elseif 1 == nValueCount then
					
				m_functionTrigger( m_tableValue[1] )
						
			elseif 2 == nValueCount then
					
				m_functionTrigger( m_tableValue[1], m_tableValue[2] )
						
			elseif 3 == nValueCount then
					
				m_functionTrigger( m_tableValue[1], m_tableValue[2], m_tableValue[3] )
						
			elseif 4 == nValueCount then
					
				m_functionTrigger( m_tableValue[1], m_tableValue[2], m_tableValue[3], m_tableValue[4] )
						
			elseif 5 == nValueCount then
					
				m_functionTrigger( m_tableValue[1], m_tableValue[2], m_tableValue[3], m_tableValue[4], m_tableValue[5] )
						
			end
		
		end
		
	end
	
	instance.InsertValue = function ( _value )
	
		table.insert( m_tableValue, _value )
		
	end
	
	instance.SetFunction = function ( _func )
	
		m_functionTrigger = _func
		
	end
	
	
	----------------------------------------------------------------------------
	
	return instance
	
end


--------------------------------------------------------------------------------
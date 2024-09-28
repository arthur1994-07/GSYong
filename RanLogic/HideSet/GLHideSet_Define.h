#ifndef _EMHIDESET_DEFINE_H_
#define _EMHIDESET_DEFINE_H_

#pragma once

enum EMHIDESET
{
	// 일반;
	EMHIDE_PICKUP_ITEM = 0, // 아이템 줍기
	EMHIDE_GETON_VEHICLE, // 탈것 타기
	EMHIDE_GETOFF_VEHICLE, // 탈것 내리기
	EMHIDE_DEATH, // 사망

	EMHIDE_UPDATE_EXP, // 경험치 획득
	EMHIDE_LEVELUP, // 레벨업
	EMHIDE_LEVELDOWN, // 레벨 다운

	// 필드 이동;
	EMHIDE_TURN_WALK, // 걷기 전환
	EMHIDE_TURN_RUN, // 뛰기 전환
	EMHIDE_ON_VEHICLE, // 탈것 탑승 상태
	EMHIDE_BUS, // 버스

	EMHIDE_TAXI, // 택시

	// 전투;
	EMHIDE_START_ATTACK, // 일반 공격 시작
	EMHIDE_USE_SKILL, // 스킬 사용
	EMHIDE_TAKE_BUFF, // 자신에게 버프 적용
	EMHIDE_TAKE_LIMIT, // 자신에게 리미트 스킬

	EMHIDE_TAKE_DEBUFF, // 자신에게 디버프 스킬 적용
	EMHIDE_ON_HIT, // 적의 공격에 적중되어 HP가 감소함
	EMHIDE_TAKE_DAMAGE_BY_DEBUFF, // 적의 지속데미지 스킬에 적중되어 HP가 감소함
	EMHIDE_AVOID, //공격 회피

	// Q박스 효과 적용 여부;
	EMHIDE_SPEEDUP,
	EMHIDE_SPEEDUP_MAX,
	EMHIDE_CRAZY,
	EMHIDE_MADNESS,

	EMHIDE_EXP_TIME,
	EMHIDE_EXP_GET,
	EMHIDE_POWERUP,
	EMHIDE_POWERUP_MAX,

	EMHIDE_BOMB,
	EMHIDE_HEAL,
	EMHIDE_LUCKY,
	EMHIDE_SUMMON,

	// 아이템 사용 여부;
	EMHIDE_USE_SKILLBOOK, // 스킬북 사용
	EMHIDE_USE_ACCREDIT, // 인증서 사용(인증 시작)
	EMHIDE_RESET_SKILL_STAT, // 스킬/스탯 리셋 사용, 사용 완료
	EMHIDE_USE_GRINDER, // 연마제 사용

	EMHIDE_USE_INVEN_EXTENSION, // 인벤 확장 사용
	EMHIDE_USE_STORAGE_EXTENSION, // 창고 확장 사용
	EMHIDE_USE_STORAGE_LINK, // 창고 연결 사용
	EMHIDE_USE_MARKET_GRANT, // 개인상점 허가권 사용 시작

	EMHIDE_USE_DISGUISE_SPLIT, // 코스튬 분리 사용
	EMHIDE_USE_DISGUISE_REMOVE, // 세제(코스튬 제거) 사용
	EMHIDE_USE_LOOK_CHANGE, // 얼굴/헤어스타일/헤어컬러 변경
	EMHIDE_USE_PET_CHANGE, // 펫 이름/컬러 변경 및 먹이 사용

	EMHIDE_USE_LUNCH, // 도시락 사용
	EMHIDE_USE_POINT_CHARGE, // 포인트 카드로 인게임 포인트 충전	

	// 탈것 개조;
	EMHIDE_REMODEL_VEHICLE, // 탈 것 아이템 탈착

	// 대련;	
	EMHIDE_BEING_SELF, // 개인 대련 시작/대련 중
	EMHIDE_BEING_PARTY, // 파티 대련 시작/대련 중
	EMHIDE_BEING_CLUB, // 클럽 대련 시작/대련 중	

	// 개인 상점;
	EMHIDE_STORE_OPEN, // 개인 상점 설치
	EMHIDE_STORE_CLOSE, // 개인 상점 접기

	// NPC;
	EMHIDE_TALK_START, // 새로 대화 시작(말걸기)
	EMHIDE_TALK_END, // 현재 NPC와의 거래 완료(혹은 대화 완료, 창 닫힘)
	EMHIDE_QUEST_ACCEPT, // NPC 대화로 퀘스트 수락
	EMHIDE_QUEST_COMPLETE, // NPC 대화로 퀘스트 완료

	// 개인거래;
	EMHIDE_DEAL_ACCEPT_SELF, // 개인 거래 신청 수락함
	EMHIDE_DEAL_ACCEPT_TARGET, // 개인 거래 신청 수락 받음
	EMHIDE_DEAL_REQUET_SEND, // 개인 거래 신청 함
	EMHIDE_DEAL_REQUEST_RECEIVE, // 개인 거래 신청 받음	
	

	// 펫; // 은신 중일때는 펫도 은신되게;
	EMHIDE_PET_SUMMON, // 펫 소환
	EMHIDE_PET_SUMMONNING, // 펫 소환 중
	EMHIDE_PET_SUMMON_RELEASE, // 펫 소환 해제
	EMHIDE_PET_SKILL_LEARN, // 펫 스킬 배우기

	// 선도전;
	EMHIDE_GUIDANCE_CERTIFY_HIDECANCLE,	// 은신중이면 보이게할것인가;

	EMHIDE_SIZE
};

#endif
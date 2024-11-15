 

WIDGET_SCRIPT_LIST =
{
	-- 필수 스크립트
	"WidgetKeyCode.lua",
	"WidgetEvent.lua",
	"WidgetDef.lua",
	"WidgetClientCall.lua",
	"WidgetSaveProp.lua",
	
	-- UI 스크립트
	"Stage.lua",							-- 스테이지
	"StageEvent.lua",						-- 스테이지 이벤트
	"Slot.lua",								-- 슬롯 관련 정의값, 유틸리티
	"GameMsg.lua",							-- 게임 메시지
	
	"CommandConsole.lua",					-- 커맨드 콘솔
	
	"Display.lua",							-- 디스플레이
	"MessageBox.lua",						-- 메시지 박스
	"HelpTooltip.lua",						-- 도움말 툴팁
	"ServerSelect.lua",						-- 서버 선택
	"ServerChannelSelect.lua",				-- 서버 채널 선택
	"Login.lua",							-- 로그인
	"CharSelect.lua",						-- 캐릭터 선택
	"CharCreate.lua",						-- 캐릭터 생성
	"BasicInfo.lua",						-- 캐릭터 기본정보
	"CharInfo.lua",							-- 캐릭터 세부정보
	"Inventory.lua",						-- 인벤토리
	"InventoryTrash.lua",					-- 인벤토리 휴지통
	"SkillQuickSlot.lua",					-- 스킬 퀵슬롯
	"ItemQuickSlot.lua",					-- 아이템 퀵슬롯
	"BasicChat.lua",						-- 채팅
	"ItemLinkInfo.lua",						-- 아이템 링크 정보
	"BasicChatOp.lua",						-- 채팅 옵션
	"Skill.lua",							-- 스킬
	"ChatMacro.lua",						-- 채팅 매크로
	"GameOption.lua",						-- 게임 옵션	
	"Party.lua",							-- 파티 관련 전역 루아 파일
	"PartySlot.lua",						-- 파티 슬롯
	"PartyInfo.lua",						-- 파티 정보	
	"HotKeyCfg.lua",						-- 단축키 설정
	"NpcTalk.lua",							-- NPC 대화
	"SystemMsg.lua",						-- 시스템 메시지
	"Pet.lua",								-- 펫
	"PetSkill.lua",							-- 펫 스킬창
	"Product.lua",							-- 제작
	"ProductReset.lua",						-- 제작초기화
	"Conflict.lua",							-- 대련 신청
	"InteractMenu.lua",						-- 상호작용 메뉴
	"HairStyleChangeCard.lua",				-- 헤어 스타일 변경 카드
	"FaceStyleChangeCard.lua",				-- 얼굴 스타일 변경 카드
	"GenderChangeCard.lua",					-- 성별 변경 카드
	"PetStyle.lua",							-- 펫 스타일
	"Private_Market.lua",					-- 개인상점
	"PointShop.lua",						-- PointShop
	"ProductProgress.lua",					-- 제작 프로그래스바
	"Trade.lua"	,							-- 거래창
	"PostBox.lua",							-- 우편함
	"PostBoxSub.lua",						-- 우편함보조
	"GroupChat.lua",						-- 그룹채팅
	"PrivateMarketSearch.lua",				-- 개인상점검색
	"InviteGroupChat.lua",					-- 그룹채팅 초대창
	"ESCMenu.lua",							-- 시스템 메뉴창 ( ESC Menu ) 열기
	"ESCMenu_Grade_Display.lua",			-- 시스템 메뉴창 ( ESC Menu ) - 등급 표시
	"StatsReset.lua",						-- 망각의 약 ( 스텟 초기화 )
	"MapGlobal.lua",						-- 미니맵, 전체맵 공용 전역
	"Minimap.lua",							-- 미니맵
	"LargeMap.lua",							-- 전체맵
	"ReportBox.lua",						-- 알림창[원용]
	"ReferChar.lua",						-- 정보보기
	"ReferCharSkill.lua",					-- 정보보기 스킬
	"PopupButton.lua",						-- 팝업 버튼
	"Menu.lua",								-- 메뉴 버튼
	"ReportMessageBox.lua",					-- 알림 메세지 박스[원용]
	"QuickMenu.lua",						-- 퀵메뉴[원용]
	"VehicleColor.lua",						-- 탈것 색상 변경
	"Menu.lua",								-- 메뉴 버튼
	"PetColor.lua",							-- 펫 컬러 변경
	"Competition.lua",						-- 전장 UI
	"Club.lua",								-- 클럽 창
	"CostumeColor.lua",						-- 코스튬 컬러 변경
	"Help.lua",								-- 도움말
	"NotifyCompetitionTexture.lua",			-- 전투 상태 알림(텍스쳐)
	"MiniGameDice.lua",						-- 미니게임 : 주사위
	"StudentRecord.lua",					-- 생활기록부
	"ActivitySalesItem.lua",				-- 특별활동 판매아이템
	"CTFNotify.lua",						-- 점령점 알림
	"CTFSpurtNotify.lua",					-- 점령전 알림2
	"CDMResult.lua",						-- CDM 결과창
	"Resurrection.lua",						-- 부활
	"RebuildCard.lua",						-- 개조카드
	"DPS.lua",								-- DPS
	"Community.lua",						-- 커뮤니티 창
	"CTFieldDisplayInfo.lua",				-- 점령전 정보창
	"CTFieldHpDisplay.lua",					-- 점령전 제어기 HP 정보
	"PublicNotifyMessage.lua",				-- 공용 알림 메세지
	"CDMStatusWindow.lua",					-- CDM 순위 정보 창
	"Lotto.lua",							-- 로또창
	"InstanceSystem.lua",					-- 무한의 재단
	"Quest.lua",							-- 퀘스트창[원용]
	"TaxiCard.lua",							-- 택시카드
	"ItemPreview.lua",						-- 아이템미리보기 창
	"SkillReserve.lua",						-- 스킬 사용 예약
	"Quest_Helper.lua",						-- 퀘스트 도움창[원용]
	"ItemToolTip.lua",						-- 아이템 툴팁[원용]
	"CursorTargetInfo.lua",					-- 커서 타겟 정보
	"Tournament.lua",						-- 토너먼트 대진표
	"GateMessageBox.lua",					-- 게이트 메세지 박스[원용]
	"MultiGateMessageBox.lua",				-- 멀티 게이트 메세지 박스[원용]
	"BuffNormal.lua",						-- 일반 버프
	"BuffLunchBox.lua",						-- 도시락 버프
	"BuffExpLunchBox.lua",					-- 경험치 도시락 버프
	"BuffAdd.lua",							-- 추가 버프
	"BuffCTF.lua",							-- CTF 버프
	"BuffSystem.lua",						-- 시스템 버프
	"QBox.lua",								-- 큐박스
	"SecondPassword.lua",					-- 2차 비밀번호
	"CTFReward.lua",						-- CTF 결과 보상창
	"MultiToolTip.lua",						-- 멀티 툴팁
	"BusStation.lua",						-- 버스 정류장
	"FlyCamCtrl.lua",						-- 플라잉 카메라 컨트롤
	"Private_Market_RegItem_MsgBox.lua",	-- 개인상점 아이템등록 메시지박스
	"CTFlagDashboard.lua",					-- 깃발전 현황판
	"Title.lua",							-- 타이틀
	"Letterbox.lua",						-- 레터 박스
	"Guide.lua",							-- 가이드
	"Locker.lua",							-- 락커
	"LockerExpand.lua",						-- 락커 확장
	"ConftDisp.lua",						-- 대련 디스플레이
	"BonusTime.lua",						-- 보너스 타임 게이지
	"BonusTimeDisplay.lua",					-- 보너스 타임 디스플레이
	"Vehicle.lua",							-- 탈것
	"SkillToolTip.lua",						-- 스킬 툴팁
	"VirtuarKeyBoard.lua",					-- 가상 키보드
	"SetItem.lua",							-- 세트 아이템
	"PVEReward.lua",						-- PVE Reward Window
	"CDMRewardItem.lua",					-- CDM 보상 아이템
	"LottoConfirm.lua",						-- 로또 당첨 확인
	"FullScreen.lua",						-- 풀스크린 버튼
	"UISwitch.lua",							-- UI 제어 창
	"SkillLvPlusToolTip.lua",				-- 스킬 다음레벨 미리보기 툴팁
	"PartyRecruit.lua",						-- 파티 모집
	"EquipConsume.lua",						-- 장비 소모품 디스플레이
	"CharCreateToolTip.lua",				-- 캐릭터 생성 툴팁
	"CharCreateCharRotate.lua",				-- 캐릭터 생성 캐릭터 회전
	"TipMsg.lua",							-- Tip Message
	"EtcModal.lua",							-- 어떤 UI에 속하기 애매한 내용을 담기위함임
	"CompareTooltip.lua",					-- 비교 툴팁
	"ForceResurrection.lua",				-- 강제 부활
	"CountrySelect.lua",					-- 난사군도 국가 선택
	"CountrySelectList.lua",				-- 난사군도 국가 선택 리스트
	"CountryRanking.lua",					-- 난사군도 국가 순위
	"CountryProgressInfo.lua",				-- 난사군도 진행 정보
	"CountryAuthProgress.lua",				-- 난사군도 인증기
	"CountryBattleReward.lua",				-- 난사군도 전투결과
	"CountryServerEntrance.lua",			-- 난사군도 전장 서버 입장
	"CostumeStat.lua",						-- 코스튬 능력치 부여
	"ExtremeWeapon.lua",					-- 극강부 무기 슬롯
	"ItemRepair.lua",						-- 아이템 수리
	"BoosterGauge.lua",						-- 부스터 게이지
	"PartyDistribution.lua",				-- 파티 분배
	"SummonState.lua",						-- 소환수 슬롯
	"PartyDistribution_dice.lua",			-- 파티 아이템 분배-주사위
	"PartyDistribution_member.lua",			-- 파티 아이템 분배-멤버창
	"PartyDistributionUI_manager.lua",		-- 파티 아이템 분배 관리 루아
	"HairColor.lua",						-- 헤어 컬러 변경
	"MacroOption.lua",						-- 매크로 옵션 설정
	"Warning.lua",							-- 경고 이미지
	"ClubBattleApply.lua",					-- 클럽 배틀 신청
	"NameDisplayConfig.lua",				-- 이름 보기 설정
	"CTFlagResult.lua",						-- 깃발전 결과
	"ClubCommisionArea.lua",				-- 클럽 선도전 수수료 지역
	"GuidanceResult.lua",					-- 선도전 결과
	"ItemPeriodExtension.lua",				-- 아이템 기간 확장
	"CDMDoublePoint.lua",					-- CDM 더블 포인트
	"AttendanceComplete.lua",				-- 출석부 과제 완료 디스플레이
	"DungeonExit.lua",						-- 인던 나가기
	"PetSkin.lua",							-- 펫 스킨
	"GMChat.lua",							-- GM 채팅
	"MenuButton.lua",						-- 메뉴 버튼
	"ProductConfirmMsgBox.lua",				-- 제작, 랜덤옵션 확인 메세지 박스
	"MessageBoxModaless.lua",				-- 모달리스 메시지 박스
	"SelformBox.lua",						-- 선택형 상자 UI
	"RanMobile.lua",						-- 란 모바일
	"StringProgressBar.lua",				-- 문자 프로그래스바
}

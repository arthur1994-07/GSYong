#pragma once

#define	USERANGE					0x00000001	// 범위 체크
#define	USESPHERE					0x00000002	// 구 체크
#define	USERANGEHIGH				0x00000004	// 범위 회전 체크
#define	USEROTATEL					0x00000008	// 회전 체크
#define	USECOLLISION				0x00000020	// 파티클 충돌 체크
#define	USEATTRACTION				0x00000040	// 자력	
#define	USECENTER					0x00000080	// 모이기
#define	USEDIRECTION				0x00000100	// 퍼지는 방향이 있죠..
#define	USETEXSCALE					0x00000200	// 텍스쳐 늘이기
#define	USETEXROTATE				0x00000400	// 텍스쳐 회전
#define	USEMESH						0x00000800	// 메쉬 사용 체크
#define	USEPARENTROTATEWORLD		0x00001000	// 부모의 회전 매트릭스 적용 함	-_-; 다른 방법 없을까?	월드 에디트  용
#define	USEPARENTMOVE				0x00002000	// 부모의 이동 매트릭스 적용 함	-_-; 다른 방법 없을까?
#define	USEPARENTROTATE_FOR_PARTICLE	0x00008000	// 부모의 이동 매트릭스 적용 함	-_-; 다른 방법 없을까?
#define	USESEQUENCE					0x00010000	// 시퀀스 효과 삽입
#define	USESEQUENCELOOP				0x00020000	// 시퀀스 효과 루프 돌기
#define	USEBILLBOARDALL				0x00040000	// 모든 파티클에 빌보드 적용 시키기
#define	USEBILLBOARDUP				0x00080000	// 빌보드 했을 때 빠닥 세우기 하자.!!
#define	USETEXTURE					0x00100000	// USEMESH, UESTEXTURE, USEMESH+UESTEXTURE 3가지 경우를 만들 수 있다.
#define	USEBEZIER_P					0x00200000	// 추가 파티클..( 시간이 너무 짧을 때, 이동 할 때 사용한다. )
#define	USEGROUND					0x00400000	// 지면 효과
#define	USEFROMTO					0x00800000	// 어디에서 어디로 목표가 이동한다.
#define	USEGROUNDATTACH				0x01000000	// 지면에 파티클이 달라 붙는다.
#define	USEHEIGHTNO					0x02000000	// 높이값의 변화가 없도록 한다.
#define USENORMAL2					0x04000000	// 노멀
#define USEGROUNDTEX				0x08000000	// 지면 TEX 사용
#define USERANDOMPLAY				0x10000000	// 랜덤한 효과의 생성
#define USERANDOMDIRECT				0x20000000	// 랜덤한 효과의 생성 때의 랜덤 방향
#define USENEWEFF					0x40000000	// 새로운 효과의 생성
#define USENEWEFF_END				0x80000000	// 새로운 효과의 생성.. 부모효과의 끝에 생성시킨다. < 보통은 처음부터 유지 >
#define	USENEWEFF_END_STOP			0x00000010	// 새로운 효과의 생성.. 부모효과의 끝에 생성시킨다. 부모는 움직임을 멈춘다.

// Note : 삭제 대상
#define	USEGOTOCENTER				0x00004000	// 센터 방향으로 이동 시킴


enum PARTICLE_FLAG_EX
{
	USE_EX_RANGE_SCREEN	= 0x00000001,		// 화면 앞에서 파티클들이 생성되도록 한다.
};

#define STRING_NUM_256		256

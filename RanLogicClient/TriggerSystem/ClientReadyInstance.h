#ifndef CClientReadyInstance_H
#define CClientReadyInstance_H

#include "ClientTriggerInstance.h"

namespace TriggerSystem
{

/**
	인던 입장을 처리 한다.
    입장 수를 카운트 하기 위해 사용 한다.
	
 */
class CClientReadyInstance
    : public CClientTriggerInstance
{
public:
    CClientReadyInstance();
    ~CClientReadyInstance();

	/**
		대기 상태가 시작시 호출 된다.
		우리는 시작될때 ShowTip 의 상태를 백업 해둔다.

	 */
	void	Start();

    /**
        시작 대기 상태를 알리는 메시지에 의해 호출 된다.
        스테이지 시작에 앞서 유저들의 접속 상태를 전달 한다.

    */
    void    Ready(SMessage* msg);

	/**
        스테이지 상태 메시지를 처리 한다.
        우리는 게임 종료 상태일 때는 메시지 출력을 막는다.

     */
    void    Stage(SMessage* msg);

	/**
		프레임 처리를 수행 한다.
		우리는 대기 남은 시간을 흐르게 한다.
        단 모든 시간이 끝나더라도 서버에서 완료 상태가 전달되지 않으면
        대기는것에 주의 한다.
		
	 */
	void	FrameMove(float fAccTime, float fElapsedTime);

protected:
    /**
        변경된 상태를 반영 한다.

     */
    void    Update(bool bEnable);

	/**
		Tip 을 보일지 유무를 설정 한다.
		Tip 은 화면 중앙 상단에 표시되는 게임 도움말 이다.

	 */
	void	ShowTip(bool bEnable);

    int     m_cur;  	//@ 현재 인원
    int     m_all;  	//@ 총 인원
    float   m_time; 	//@ 대기 남은 시간
	bool	m_showtip;	//@ 강제로 팁 보기를 끄고 이전 상태를 백업해 둔다.
};

} // end namespace

#endif
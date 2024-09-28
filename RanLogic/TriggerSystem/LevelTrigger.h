#ifndef CLevelTrigger_H
#define CLevelTrigger_H

#include "Trigger.h"

namespace TriggerSystem
{

/**
	레벨 트리거
	
	난이도에 따른 트리거들의 묶음을 처리하기 위해 제작 되었다.
	하나의 맵을 상황에 따라 여러개의 레벨(난이도, 등급)으로 나누어 사용할 수 있다.

	이를 위해서는 레벨 트리거는 서브 트리거 파일명을 가지고 트리거를 로딩 한다.
	또한 레벨의 등급을 지정해서 맵에 진입시 지정 등급의 레벨을 발동 시킬 수 있다.

 */
class CLevelTrigger
	: public CTrigger
{
public:
		CLevelTrigger();

		/**
			스크립트와의 바인딩을 처리 한다.
			
		 */
virtual	void		    Bind(CTriggerScript* pScript);

		/**
			레벨 스크립트를 로딩 한다.ear

		 */
virtual void		    Load() {}

		/**
			바인딩 인터페이스 함수들

		 */
		void		    SetLevel(int level) { m_level = level; }
		const int	    GetLevel() const { return m_level; }

		void		    SetUserLevel(int minlevel, int maxlevel) { m_minlevel = minlevel; m_maxlevel = maxlevel; }
		const int		GetUserMinLevel() const { return m_minlevel; }
		const int		GetUserMaxLevel() const { return m_maxlevel; }

		void		    SetFile(TSTRING file) { m_file = file; }
		const TSTRING	GetFile() const { return m_file; }

protected:
		TSTRING		    m_file;			//@ 읽을 레벨 파일명
		int			    m_level;		//@ 현재 레벨
		int			    m_minlevel;		//@ 허용 유저 최소 레벨 제한
		int			    m_maxlevel;		//@ 허용 유저 최대 레벨 제한
};

} // end namespace

#endif
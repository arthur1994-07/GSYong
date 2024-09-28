#pragma once

#include <queue>

namespace EventBox
{
	/* 이벤트 박스;
	 * 이벤트 박스를 상속받는 클래스로 외부에;
	 * 스스로가 발생시키는 이벤트를 알릴 수 있다; */
	class CEventBox
	{
	public:
		// ID만 전달하는 Event;
		void AddEvent( int addEvent )
		{
			m_queue.push( addEvent );
		}

		// Json형식으로 각 Data도 함께 전달하는 Event;
		void AddJsonEvent( const std::string& addEvent )
		{
			m_jsonQueue.push( addEvent );
		}

	public:
		int NextEvent()
		{
			if( m_queue.empty() )
				return 0;

			int curEvent = m_queue.front();
			m_queue.pop();
			return curEvent;
		}

		std::string NextJsonEvent()
		{
			if( m_jsonQueue.empty() )
				return std::string();

			std::string curEvent = m_jsonQueue.front();
			m_jsonQueue.pop();
			return curEvent;
		}
		
	private:
		std::queue<int> m_queue;
		std::queue<std::string> m_jsonQueue;
	};
}
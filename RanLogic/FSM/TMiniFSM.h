#ifndef TMiniFSM_H
#define TMiniFSM_H

#include <list>
#include <algorithm>
#include <boost/bind.hpp>

/**
	미니상태머신을 의미 한다.

    하나의 특정한 상태(bool)를 고유한 ID 에 따라 누적 시켜 상태를 관리 함으로써
    이전 입력된 상태들을 보전 하고 누적된 상태 ID를 통해 상태의 로그를 알 수 있어 관리, 파악이 용이하다.

    단순히 DWORD 형의 비트 조작 상태들을 관리하는 것에 대한 문제점들을 상쇄 시킬 요량으로 개발

    상태값의 생명 주기를 잘 파악해서 새로운 ID 를 할당 해서 잘 사용해야 한다.
    그러므로 ID 를 할당하는 곳에서 ID 에 대해 설명을 잘 해두어야 혼선 없이 잘 사용할 수 있다.

 */
template<class id, class state>
class TMiniFSM
{
public:
    typedef id ID;
    typedef state State;
    typedef std::pair<ID, State> Value;
    typedef std::list<Value> StateContainer;
    typedef typename StateContainer::iterator StateContainerIt;

    /**
        초기값을 설정 한다.
        내부에 쌓여있는 상태값이 모두 소진되면 초기값으로 설정 된다.
        그러므로 초기값 설정에 유의해야 한다.

     */
    TMiniFSM(State state) : m_now(state), m_init(state) {}

    /**
        상태를 초기화 or 삭제 한다.

     */
    void            Init() { m_now = m_init; }
    void            Clear() { m_state.clear(); Init(); }

    /**
        현재 상태를 출력 한다.

     */
    const State     GetState()  const { return m_now; }

    /**
        구분 고유 값과 상태를 추가 한다.
        우리는 항상 마지막에 추가된 상태ID와 상태 값을 가진다.
        또한 동일한 ID는 중복 저장하지 않는 규칙이므로 이미 동일한 ID가 저장되어 있다면
        값을 변경 해 준다.

     */
    void            Add(const ID i, const State v)
    {
        StateContainerIt it = Find(i);

        if (it != m_state.end())
        {
            it->second = v;
        }
        else
        {
            m_state.push_back(std::make_pair(i, v));
        }

        m_now = m_state.back().second;
    }

    /**
        입력 했던 상태를 제거 한다.
        쌓아 뒀던 상태들중 입력된 ID 값을 찾아 제거 한다.
        만약 상태가 없다면 다시 Add해서 저장해 둠에 주의 한다.
        
     */
    void            Remove(const ID i, const State v)
    {
        StateContainerIt it = Find(i);

        if (it != m_state.end())
        {
            bool bBackErase = (it->first == m_state.back().first);

            m_state.erase(it);

            if (m_state.empty())
            {
                Init();
            }
            else if (bBackErase)
            {
                m_now = m_state.back().second;
            }
        }
        else
        {
            Add(i, v);
        }
    }

protected:
    /**
        입력된 ID 를 가지는 상태값을 찾는다.

     */
    StateContainerIt Find(const ID i)
    {
        return typename std::find_if(m_state.begin(), m_state.end(), 
            boost::bind(&TMiniFSM::Compare, this, _1, i));
    }

    /**
        Find 를 위한 비교 함수

     */
    bool Compare(const Value& value, const ID& id) const
    {
        return value.first == id;
    }

    State           m_init;     //@ 초기 상태값
    State           m_now;      //@ 현재 상태값
    StateContainer  m_state;    //@ 누적된 상태값들 모음, 순차적으로 쌓여있다.
};


/*
 * bit-flag 상태 값 확인을 쉽게 할 수 있게 함;
 * 상태 변화가 자주 일어나야 하는 경우에는 TMiniFSM을 사용;
*/
class unsignedState
{
public:
	__forceinline const unsigned int getNBit(void) const;
	__forceinline const bool isState(const unsigned int _nState) const;
	__forceinline void setState(const unsigned int _nState, const bool _bOn = true);	

	__forceinline void reset(void);

public:
	__forceinline const bool operator ==(const unsigned int _nState);
	__forceinline const bool operator ==(const unsignedState& _rhs);
	__forceinline const unsignedState& operator=(const unsigned int _nState);
	__forceinline const unsignedState& operator=(const unsignedState& _rhs);
	__forceinline const unsignedState& operator+=(const unsigned int _nState);
	__forceinline const unsignedState& operator-=(const unsigned int _nState);

	__forceinline const bool operator[](const unsigned int _nState) const;
	__forceinline const bool operator()(void) const;

	unsignedState::unsignedState(void)
		: nState(0)
	{
	}
	unsignedState::unsignedState(const unsigned int _nState)
		: nState(_nState)
	{
	}
	unsignedState::unsignedState(const unsignedState& _rhs)
		: nState(_rhs.nState)
	{
	}
	unsignedState::~unsignedState(void)
	{
	}

private:
	union
	{
		struct 
		{
			unsigned int nState;
		};

		// 아래는 debugging 모드에서 값 확인용으로만 사용 해야 함;
		// 값 제어는 많이 느림;
		struct
		{		
			bool b00000001 : 1;
			bool b00000002 : 1;
			bool b00000004 : 1;
			bool b00000008 : 1;
			bool b00000010 : 1;
			bool b00000020 : 1;
			bool b00000040 : 1;
			bool b00000080 : 1;

			bool b00000100 : 1;
			bool b00000200 : 1;
			bool b00000400 : 1;
			bool b00000800 : 1;
			bool b00001000 : 1;
			bool b00002000 : 1;
			bool b00004000 : 1;
			bool b00008000 : 1;

			bool b00010000 : 1;
			bool b00020000 : 1;
			bool b00040000 : 1;
			bool b00080000 : 1;
			bool b00100000 : 1;
			bool b00200000 : 1;
			bool b00400000 : 1;
			bool b00800000 : 1;

			bool b01000000 : 1;
			bool b02000000 : 1;
			bool b04000000 : 1;
			bool b08000000 : 1;
			bool b10000000 : 1;
			bool b20000000 : 1;
			bool b40000000 : 1;
			bool b80000000 : 1;
		};
		struct  
		{		
			bool b1 : 1;
			bool b2 : 1;
			bool b4 : 1;
			bool b8 : 1;
			bool b16 : 1;
			bool b32 : 1;
			bool b64 : 1;
			bool b128 : 1;

			bool b256 : 1;
			bool b512 : 1;
			bool b1024 : 1;
			bool b2048 : 1;
			bool b4096 : 1;
			bool b8192 : 1;
			bool b16384 : 1;
			bool b32768 : 1;

			bool b65536 : 1;
			bool b131072 : 1;
			bool b262144 : 1;
			bool b524288 : 1;
			bool b1048576 : 1;
			bool b2097152 : 1;
			bool b4194304 : 1;
			bool b8388608 : 1;

			bool b16777216 : 1;
			bool b33554432 : 1;
			bool b67108864 : 1;
			bool b134217728 : 1;
			bool b268435456 : 1;
			bool b536870912 : 1;
			bool b1073741824 : 1;
			bool b2147483648 : 1;
		};
	};
};

#include "TMiniFSM.hpp"

#endif
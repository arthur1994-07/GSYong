#ifndef TMiniFSM_H
#define TMiniFSM_H

#include <list>
#include <algorithm>
#include <boost/bind.hpp>

/**
	�̴ϻ��¸ӽ��� �ǹ� �Ѵ�.

    �ϳ��� Ư���� ����(bool)�� ������ ID �� ���� ���� ���� ���¸� ���� �����ν�
    ���� �Էµ� ���µ��� ���� �ϰ� ������ ���� ID�� ���� ������ �α׸� �� �� �־� ����, �ľ��� �����ϴ�.

    �ܼ��� DWORD ���� ��Ʈ ���� ���µ��� �����ϴ� �Ϳ� ���� ���������� ��� ��ų �䷮���� ����

    ���°��� ���� �ֱ⸦ �� �ľ��ؼ� ���ο� ID �� �Ҵ� �ؼ� �� ����ؾ� �Ѵ�.
    �׷��Ƿ� ID �� �Ҵ��ϴ� ������ ID �� ���� ������ �� �صξ�� ȥ�� ���� �� ����� �� �ִ�.

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
        �ʱⰪ�� ���� �Ѵ�.
        ���ο� �׿��ִ� ���°��� ��� �����Ǹ� �ʱⰪ���� ���� �ȴ�.
        �׷��Ƿ� �ʱⰪ ������ �����ؾ� �Ѵ�.

     */
    TMiniFSM(State state) : m_now(state), m_init(state) {}

    /**
        ���¸� �ʱ�ȭ or ���� �Ѵ�.

     */
    void            Init() { m_now = m_init; }
    void            Clear() { m_state.clear(); Init(); }

    /**
        ���� ���¸� ��� �Ѵ�.

     */
    const State     GetState()  const { return m_now; }

    /**
        ���� ���� ���� ���¸� �߰� �Ѵ�.
        �츮�� �׻� �������� �߰��� ����ID�� ���� ���� ������.
        ���� ������ ID�� �ߺ� �������� �ʴ� ��Ģ�̹Ƿ� �̹� ������ ID�� ����Ǿ� �ִٸ�
        ���� ���� �� �ش�.

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
        �Է� �ߴ� ���¸� ���� �Ѵ�.
        �׾� �״� ���µ��� �Էµ� ID ���� ã�� ���� �Ѵ�.
        ���� ���°� ���ٸ� �ٽ� Add�ؼ� ������ �ҿ� ���� �Ѵ�.
        
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
        �Էµ� ID �� ������ ���°��� ã�´�.

     */
    StateContainerIt Find(const ID i)
    {
        return typename std::find_if(m_state.begin(), m_state.end(), 
            boost::bind(&TMiniFSM::Compare, this, _1, i));
    }

    /**
        Find �� ���� �� �Լ�

     */
    bool Compare(const Value& value, const ID& id) const
    {
        return value.first == id;
    }

    State           m_init;     //@ �ʱ� ���°�
    State           m_now;      //@ ���� ���°�
    StateContainer  m_state;    //@ ������ ���°��� ����, ���������� �׿��ִ�.
};


/*
 * bit-flag ���� �� Ȯ���� ���� �� �� �ְ� ��;
 * ���� ��ȭ�� ���� �Ͼ�� �ϴ� ��쿡�� TMiniFSM�� ���;
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

		// �Ʒ��� debugging ��忡�� �� Ȯ�ο����θ� ��� �ؾ� ��;
		// �� ����� ���� ����;
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
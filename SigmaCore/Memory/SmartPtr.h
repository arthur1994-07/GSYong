#ifndef _CTYPE_MEM_SMART_PTR_H_
#define _CTYPE_MEM_SMART_PTR_H_

#include <boost/tr1/memory.hpp>

/*  
	@class c��� ��Ÿ���� �����͸� �ڵ����� �����ϴ� ����Ʈ ������ ��ü
    @brief  �ݵ�� �����ڳ� ���Կ����ڿ� malloc���� �Ҵ�� �޸𸮸� �Ѱܾ� �Ѵ� 
    �׸��� ���� ī������ ���� �Ϸ��� ��ü ��ü�� �����ؼ� ���� �ȴ� ( ���� �ص� spVoid ��� �ϳ����̹Ƿ� ������)
	����� �ν�Ʈ ����Ʈ �����ʹ� ����ī������ atomic ������ ������ �����忡 ���� 

	shared_ptr�� ���� �ڼ��� ������ boost.org �� ���� ����
*/

class CTypeSmartPtr
{
	typedef std::tr1::shared_ptr<void> spVoid;

public:
	CTypeSmartPtr(void* pMemory = NULL) { Init(pMemory); }
	BYTE* operator=(void* pMemory) { return (BYTE*)Init(pMemory); }
	
	// �����͸� ĳ������ ��� ���������� �߸� ��ȯ�ɼ� �ִ� ���Ƶ���
//	operator BYTE*() const { return (BYTE*)m_spMemory.get(); }
//	operator void*() const { return m_spMemory.get(); }
//	operator bool() const { return get() != NULL; }

	bool operator==(void* pMemory) const { return m_spMemory.get() == pMemory; }
	bool operator!=(void* pMemory) const { return m_spMemory.get() != pMemory; }

	void* get() const { return ToVoidPtr(); }
	void* ToVoidPtr() const {return m_spMemory.get();}
	BYTE* ToBytePtr() const {return (BYTE*)m_spMemory.get();}

	long use_count() { return m_spMemory.use_count(); }

	spVoid& GetSPMemory() { return m_spMemory; }
private:
	void* Init(void* pMemory) { m_spMemory = spVoid(pMemory,free); return m_spMemory.get(); }

private:
	spVoid m_spMemory;

};

inline bool operator==(void* pMemory,CTypeSmartPtr& CTypeMemory) { return pMemory == CTypeMemory.get(); }
inline bool operator!=(void* pMemory,CTypeSmartPtr& CTypeMemory) { return pMemory != (void*)CTypeMemory.get(); }

#endif //#ifndef _CTYPE_MEM_SMART_PTR_H_

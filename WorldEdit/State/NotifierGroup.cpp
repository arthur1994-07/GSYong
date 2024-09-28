#include "stdafx.h"
#include "NotifierGroup.h"
#include "FileNotifier.h"
#include "boost/foreach.hpp"

CNotifierGroup::CNotifierGroup() :
	m_nType(0)
{
	
}

CNotifierGroup::~CNotifierGroup()
{	
	Clear();
}

void CNotifierGroup::Init(int nType)
{
	m_nType = nType;
}

void CNotifierGroup::Clear()
{
	m_kNotifierContainer.clear();
}

CFileNotifier* CNotifierGroup::FindNotifier(CParam& kParam)
{
	BOOST_FOREACH(SpCNotifier& p, m_kNotifierContainer)
	{
		if (p->Classification(kParam) == true)
		{
			return p.get();
		}
	}

	return 0;
}

void CNotifierGroup::InsertNotifier(CFileNotifier* pNotifier)
{
	SpCNotifier sNotifier(pNotifier);

	if (pNotifier->Init() == true)
	{	
		m_kNotifierContainer.push_back(sNotifier);
	}	
}

void CNotifierGroup::DeleteNotifier(CParam& kParam)
{
	NotifierContainerIterator it = m_kNotifierContainer.begin();

	for ( ; it != m_kNotifierContainer.end(); ++it)
	{
		if ( (*it)->Classification(kParam) )
		{
			m_kNotifierContainer.erase(it);
			break;
		}
	}
}

int CNotifierGroup::Notify()
{
	int n = 0;

	BOOST_FOREACH(SpCNotifier& p, m_kNotifierContainer)
	{
		n |= p->Notify();

		if (n == CFileNotifier::RETURNCODE_EXIT)
		{
			break;
		}
	}

	return n;
}

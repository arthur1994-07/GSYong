#include "pch.h"

#include "../DxEffSingle.h"

#include "SingleForThread.h"




//////////////////////////////////////////////////////////////////////////
//							GroundForThread
//////////////////////////////////////////////////////////////////////////
SingleForThread::SingleForThread( boost::shared_ptr<DxEffSingleGroup>& spThisGroup )
	: m_wpThisGroup(spThisGroup)
	, m_nUSED(1)
	, m_nBackUp(0)
{
	m_bRenderCulling[0] = FALSE;
	m_bRenderCulling[1] = FALSE;
}

SingleForThread::~SingleForThread()
{
}


//////////////////////////////////////////////////////////////////////////
// Main Thread ���� Swap �ϴ� ��
//
BOOL SingleForThread::IsSwap()
{
	const boost::shared_ptr<DxEffSingleGroup> spThisGroup = m_wpThisGroup.lock();
	if ( !spThisGroup )
		return FALSE;

	if ( m_nUSED == 0 )
	{
		m_nUSED = 1;
		m_nBackUp = 0;
	}
	else
	{
		m_nUSED = 0;
		m_nBackUp = 1;
	}

	// MainThread ���� m_nUSED �� ���� �����ش�.
	// Particle Thread ���� m_nBackUp �� ����ϰ� �ȴ�.
	m_bRenderCulling[m_nUSED] = FALSE;

	return TRUE;
}

void SingleForThread::SetRenderCullingTRUE()
{ 
	m_bRenderCulling[m_nUSED] = TRUE; 
}

// virtual
//
void SingleForThread::Swap( LPDIRECT3DDEVICEQ pd3dDevice )
{
	IsSwap();
}


//////////////////////////////////////////////////////////////////////////
// Effect Thread ���� �۾��Ǵ� ��
//
BOOL SingleForThread::IsRenderCulling() const					
{ 
	return m_bRenderCulling[m_nBackUp]; 
}


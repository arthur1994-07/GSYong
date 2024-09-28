#include "pch.h"

#include "../../SigmaCore/File/SerialFile.h"

#include "WaterSurfDef.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


void WATERSURF_PROPERTY::Save( sc::SerialFile &SFile )
{
	SFile << m_bPlaneInverse;			// 
	SFile << m_fPlaneWidth;				//	
	SFile << m_fPlaneHeight;			//	
	SFile << m_fSurfVelocityINIT;		// �ʱ� ���� �̵� ���ǵ�
	SFile << m_fEdit_SurfAcceleration;	// [Edit] m_fSurfVelocity �پ��� ��ġ
	SFile << m_fSurfSleepTimeINIT;		// ���� �ẹ �ð�
	SFile << m_fAlphaAddition;			// ���� �߰� ���ǵ�
	SFile << m_fAlphaSubtractionTime;	// ���� ���� �ð�
	SFile << m_fAlphaSubtraction;		// ���� ���� ���ǵ�
	SFile << m_fSideLineInterval;		// �ĵ��� �������� ������ �ð�����
}

void WATERSURF_PROPERTY::Load( sc::SerialFile &SFile )
{
	SFile >> m_bPlaneInverse;			// 
	SFile >> m_fPlaneWidth;				//	
	SFile >> m_fPlaneHeight;			//	
	SFile >> m_fSurfVelocityINIT;		// �ʱ� ���� �̵� ���ǵ�
	SFile >> m_fEdit_SurfAcceleration;	// [Edit] m_fSurfVelocity �پ��� ��ġ
	SFile >> m_fSurfSleepTimeINIT;		// ���� �ẹ �ð�
	SFile >> m_fAlphaAddition;			// ���� �߰� ���ǵ�
	SFile >> m_fAlphaSubtractionTime;	// ���� ���� �ð�
	SFile >> m_fAlphaSubtraction;		// ���� ���� ���ǵ�
	SFile >> m_fSideLineInterval;		// �ĵ��� �������� ������ �ð�����
}
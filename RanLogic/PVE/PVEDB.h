#ifndef PVEDB_H
#define PVEDB_H

/**
    DB ������ ���� ������ ������.
    �츮�� DB �� �ϰ��� ��Ų��.

 */
struct SPVEDBFile 
{
    TSTRING strFile;
    int nYear;
    int nMonth;
    int nDay;
	int nHour;	
};

/**
    DB ���ϸ��� ó�� �Ѵ�.
    ���ó� �Ѱ����� ���ϸ��� ó�������ν� �����ϰ� ��ó�� �� �ִ�.

 */
const TCHAR* GetPVEDBFile();

/**
    DB ������ �����ϴ��� üũ �Ѵ�.
    �츮�� �̸� ���� �ʱ�ȭ�� üũ �� �� �ִ�.

 */
bool IsPVEDBFile();

/**
	DB ������ �ʱ�ȭ �Ѵ�.

 */
void ResetPVEDBFile();

#endif
#ifndef CWorldEditFileNotifier_H
#define CWorldEditFileNotifier_H

#include "FileNotifier.h"

class   DxSetLandMan;
struct  SMATERIAL_PIECE;
struct  DXMATERIALEX;

/**
	������ �޽� X ����
	���� ���� ������ ����Ʈ �� ����������
	���Ǵ� ���� x ������ �ǹ� �Ѵ�.

 */
class CFrameMeshXFileNotifier
	: public CFileNotifier
{
public:
		bool				Init();
		int					OnNotify();

static  TSTRING             GetFile(DxSetLandMan* pSetLandMan);
};

/**
	���� ����Ʈ ������ ���Ǵ� ���
	�ؽ�ó�� ������ ó�� �Ѵ�.
	
 */
class CTextureFileNotifier
	: public CFileNotifier
{
public:
		bool				Init();
		int					OnNotify();

static  bool                GetFile(TSTRING& strFile);
static  TSTRING			    GetFile(SMATERIAL_PIECE* pMtlPiece, DXMATERIALEX* pDxMtl);
};

/**
	���� ����(pie) ����
	���� ����Ʈ �������� pef �� pie ��� 2���� ���� ������ ����Ѵ�.
	���� pis ��� �͵� ��������� ����� ���� �ʴ´�.

	pef �� �׷��������� �����ϴ� ������ Ȯ���� �̸�
	pie �� �� ������ Ȯ���ڸ� ���� Ŭ���̾�Ʈ���� ����ϱ� ���� ��ȯ �����̴�.

 */
class CPieceFileNotifier
	: public CFileNotifier
{
public:
virtual	bool				Init();
virtual	int					OnNotify();
		LPDIRECT3DDEVICEQ	GetDevice();

static  TSTRING             GetFile(const TCHAR* pPisName);
};

/**
	���� ����(pie) ���� - wld ���� ���Ǵ� pie ����.
	pie �� �� ������ Ȯ���ڸ� ���� Ŭ���̾�Ʈ���� ����ϱ� ���� ��ȯ �����̴�.
 */
class CPieceWldFileNotifier
	: public CFileNotifier
{
public:
virtual	bool				Init();
virtual	int					OnNotify();
		LPDIRECT3DDEVICEQ	GetDevice();

static  TSTRING             GetFile(const TCHAR* pPisName);
};

#endif
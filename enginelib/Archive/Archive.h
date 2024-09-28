#ifndef Archive_H
#define Archive_H

#include "ArchiveType.h"

class   DxLandMan;
class   DxSetLandMan;
class   DxSkinCharData;
class   DxVehicleData;
class   DxAttBoneData;
class   DxAttBoneLink;
struct  DxSkinPiece;
struct  EFF_PROPGROUP;

/**
	��ī�̺� ����̶� ���� ������ ����ϴ� ���ϵ��� ���� �ϴ� ����� �ǹ� �Ѵ�.
    ������ �� ���� ��� ���� ��� ���� �Ǿ� ���� �ǹǷ� ���� �ƿ�ǲ�� ������ �뵵�� Ȱ�� �Ѵ�.

 */
namespace Archive
{
    /**
    	��ī�̺� ������ ���� ������ ���� �Ѵ�.
        ��ī�̺긦 �������� �ݵ�� ȣ���ؾ� �Ѵ�.

        pData �� ������� ���� ������ data ������ �ȴ�.
        pArchive �� ������ ������ �ȴ�.

        ���� ���� ���� �Ǿ��ٸ� pData �� �������� ����ϴ� ���ҽ��� ã�Ƽ�
        pArchive ������ �����ϰ� ������ �����ϸ鼭 ���簡 ���� �ȴ�.

    	\param pDevice d3d ����̽�
    	\param pData ��ī�̺�� ���� �˻��� ���� ������ ��Ʈ ����
    	\param pArchive ��ī�̺긦 ������ ���� ���
     */
    void    PreArchive(LPDIRECT3DDEVICEQ pDevice, const TCHAR* pData, const TCHAR* pArchive);

	/**
		�� Ŭ������ �°� ��ī�̺긦 ���� �Ѵ�.
        �ܺο����� �Ʒ� �Լ��� ������ �Լ��� ȣ�� �ϰų� �ʿ��
        ����� ��� �ϸ� �ȴ�.

	 */
    void    Archive(DxLandMan* pLandMan);
	void    Archive(DxSetLandMan* pSetLandMan, const TCHAR* pPSFFile);
    void    Archive(DxSkinCharData* pSkinData);
    void    Archive(DxSkinPiece* pSkinPiece);
    void    Archive(DxVehicleData* pVehicleData);
    void    Archive(DxAttBoneData* pAttData);
    void    Archive(DxAttBoneLink* pAttBoneLink);
	void	Archive(EFF_PROPGROUP* pEffGroup);

    /**
        ��ī�̺� ó���� ���� ���� Ŭ���� ������ ���� �Ѵ�.
        ���� ���� ���������� 2���� ���� ������� ����ϹǷ�
        �̿� ���� ��ī�̺� ó�� ������ ���� �Ѵ�.

     */
    bool    CreateFileBaseStream(const TCHAR* PathName, const TCHAR* pFileName, const TCHAR* pZipFile, boost::function<void (const TCHAR*)> pArchive, SPBaseStream& spBase);
    bool    CreateSerialFile(boost::function<void (const TCHAR*)> pArchive, SPSerialFile& spFile);

}; // end namespace

#endif
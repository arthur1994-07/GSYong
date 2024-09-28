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
	아카이브 기능이란 현재 씬에서 사용하는 파일들을 추출 하는 기능을 의미 한다.
    추출할 때 파일 경로 역시 모두 포함 되어 추출 되므로 최종 아웃풋을 모으는 용도로 활용 한다.

 */
namespace Archive
{
    /**
    	아카이브 수행을 위한 정보를 설정 한다.
        아카이브를 수행전에 반드시 호출해야 한다.

        pData 는 예를들어 실행 폴더의 data 폴더가 된다.
        pArchive 는 임의의 폴더가 된다.

        위와 같이 설정 되었다면 pData 의 폴더에서 사용하는 리소스를 찾아서
        pArchive 폴더로 동일하게 폴더를 유지하면서 복사가 수행 된다.

    	\param pDevice d3d 디바이스
    	\param pData 아카이브시 파일 검색을 위한 데이터 루트 폴더
    	\param pArchive 아카이브를 수행할 폴더 경로
     */
    void    PreArchive(LPDIRECT3DDEVICEQ pDevice, const TCHAR* pData, const TCHAR* pArchive);

	/**
		각 클래스에 맞게 아카이브를 수행 한다.
        외부에서는 아래 함수중 적당한 함수를 호출 하거나 필요시
        만들어 사용 하면 된다.

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
        아카이브 처리를 위한 파일 클래스 생성을 수행 한다.
        현재 란은 내부적으로 2개의 파일 입출력을 사용하므로
        이에 맞춰 아카이브 처리 파일을 생성 한다.

     */
    bool    CreateFileBaseStream(const TCHAR* PathName, const TCHAR* pFileName, const TCHAR* pZipFile, boost::function<void (const TCHAR*)> pArchive, SPBaseStream& spBase);
    bool    CreateSerialFile(boost::function<void (const TCHAR*)> pArchive, SPSerialFile& spFile);

}; // end namespace

#endif
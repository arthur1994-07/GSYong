#ifndef CLODData_H
#define CLODData_H

#include "FileNotifier.h"
#include "../../SigmaCore/Json/json_spirit.h"

class DxSkinChar;
class DxVehicle;
class DxAttBone;
struct DxSkinPiece;

/**
    캐릭터 LOD 데이터 처리기
    외부 데이터 파일과 내부 코드와의 연동을 처리 한다.

    데이터 파일명은 캐릭터 파일로 고정되어 있음에 주의 해야 한다.
    즉 캐릭터가 abc.chf 라면 데이터 파일명은 abc.lod 이다.

    현재 데이터 파일은 json 기반으로 되어 있지만 향후 xml 로 변경 될 수도 있다.
    
    만약 lod 를 사용하는데 lod 파일이 없다면 자동으로 만들어 준다.

 */
class CLODData
	: public CFileNotifier
{
public:
		CLODData(DxSkinChar* pSkinChar);
        CLODData(DxVehicle* pVehicle);
        CLODData(DxAttBone* pAttBone);

#ifndef DEF_SKIN_SYNC
        CLODData(DxSkinChar* pSkinChar, DxSkinPiece* pSkinPiece);
        CLODData(DxVehicle* pVehicle, DxSkinPiece* pSkinPiece);
        CLODData(DxAttBone* pAttBone, DxSkinPiece* pSkinPiece);
#endif

typedef std::basic_fstream<TCHAR>   tfstream;

        /**
            LOD 데이터를 초기화 한다.
            DB 파일(.lod)이 존재 한다면 로딩하고 없다면 생성 한다.
        	
        	\return 생성 성공 유무
         */
        bool			            Init();

        /**
        	파일이 변경되었을 경우
            스크립트를 재가동 시킨다.

        	\return 리턴 코드
         */
        int		                    OnNotify();

private:

        /**
            스크립트 파일로 부터 LOD 정보를 읽거나
            현재 캐릭터로 부터 LOD 정보를 파일로 쓴다.

         */
        void                        Read(TSTRING& strFile);
        void                        Write(TSTRING& strFile);
        bool                        GetFilename(TSTRING& strFile);

        /**
        	스크립트를 읽은 후 데이터를 파싱 한다.

         */
        void                        Input(TSTRING& strParts, json_spirit::Object& obj);
        DxSkinPiece*                GetParts(const TSTRING& strParts);

        /**
        	데이터로 부터 스크립트로 출력 한다.

         */
        void                        Output(json_spirit::Object& obj);
        void                        SetParts(const TSTRING& strParts, json_spirit::Object& obj, struct DxSkinPiece* pPiece);

        /// LOD 적용 개체
        union
        {
            DxSkinChar*             m_pSkinChar;
            DxVehicle*              m_pVehicle;
            DxAttBone*              m_pAttBone;            

        }                           m_pChar;

        /// 툴의 피스 개체...
        DxSkinPiece*                m_pSkinPiece;

        /// LOD 적용 개체 타입
        int                         m_nCharType;
};

#endif
#ifndef CLODData_H
#define CLODData_H

#include "FileNotifier.h"
#include "../../SigmaCore/Json/json_spirit.h"

class DxSkinChar;
class DxVehicle;
class DxAttBone;
struct DxSkinPiece;

/**
    ĳ���� LOD ������ ó����
    �ܺ� ������ ���ϰ� ���� �ڵ���� ������ ó�� �Ѵ�.

    ������ ���ϸ��� ĳ���� ���Ϸ� �����Ǿ� ������ ���� �ؾ� �Ѵ�.
    �� ĳ���Ͱ� abc.chf ��� ������ ���ϸ��� abc.lod �̴�.

    ���� ������ ������ json ������� �Ǿ� ������ ���� xml �� ���� �� ���� �ִ�.
    
    ���� lod �� ����ϴµ� lod ������ ���ٸ� �ڵ����� ����� �ش�.

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
            LOD �����͸� �ʱ�ȭ �Ѵ�.
            DB ����(.lod)�� ���� �Ѵٸ� �ε��ϰ� ���ٸ� ���� �Ѵ�.
        	
        	\return ���� ���� ����
         */
        bool			            Init();

        /**
        	������ ����Ǿ��� ���
            ��ũ��Ʈ�� �簡�� ��Ų��.

        	\return ���� �ڵ�
         */
        int		                    OnNotify();

private:

        /**
            ��ũ��Ʈ ���Ϸ� ���� LOD ������ �аų�
            ���� ĳ���ͷ� ���� LOD ������ ���Ϸ� ����.

         */
        void                        Read(TSTRING& strFile);
        void                        Write(TSTRING& strFile);
        bool                        GetFilename(TSTRING& strFile);

        /**
        	��ũ��Ʈ�� ���� �� �����͸� �Ľ� �Ѵ�.

         */
        void                        Input(TSTRING& strParts, json_spirit::Object& obj);
        DxSkinPiece*                GetParts(const TSTRING& strParts);

        /**
        	�����ͷ� ���� ��ũ��Ʈ�� ��� �Ѵ�.

         */
        void                        Output(json_spirit::Object& obj);
        void                        SetParts(const TSTRING& strParts, json_spirit::Object& obj, struct DxSkinPiece* pPiece);

        /// LOD ���� ��ü
        union
        {
            DxSkinChar*             m_pSkinChar;
            DxVehicle*              m_pVehicle;
            DxAttBone*              m_pAttBone;            

        }                           m_pChar;

        /// ���� �ǽ� ��ü...
        DxSkinPiece*                m_pSkinPiece;

        /// LOD ���� ��ü Ÿ��
        int                         m_nCharType;
};

#endif
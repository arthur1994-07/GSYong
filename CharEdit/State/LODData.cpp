#include "stdafx.h"
#include "LODData.h"

#include "../CharEdit.h"
#include "../MainFrm.h"
#include "../CharEditDoc.h"
#include "../CharEditView.h"
#include "../Properties/sheetWithTabChar.h"

#include "../../enginelib/DxMeshs/SkinMesh/SSkinCharLODInfo.h"

CLODData::CLODData(DxSkinChar* pSkinChar) : 
    m_nCharType(CHARTAB_CHARFILE)
{
    m_pChar.m_pSkinChar = pSkinChar;
}

CLODData::CLODData(DxVehicle* pVehicle) :
    m_nCharType(CHARTAB_VEHICLE)
{
    m_pChar.m_pVehicle = pVehicle;
}

CLODData::CLODData(DxAttBone* pAttBone) :
    m_nCharType(CHARTAB_ATTBONEFILE)
{
    m_pChar.m_pAttBone = pAttBone;
}

#ifndef DEF_SKIN_SYNC
CLODData::CLODData(DxSkinChar* pSkinChar, DxSkinPiece* pSkinPiece) :
    m_nCharType(CHARTAB_CHARPIECE)
{
    m_pChar.m_pSkinChar = pSkinChar;
    m_pSkinPiece = pSkinPiece;
}

CLODData::CLODData(DxVehicle* pVehicle, DxSkinPiece* pSkinPiece) :
    m_nCharType(CHARTAB_CHARPIECE + 1)
{
    m_pChar.m_pVehicle = pVehicle;
    m_pSkinPiece = pSkinPiece;
}

CLODData::CLODData(DxAttBone* pAttBone, DxSkinPiece* pSkinPiece) :
    m_nCharType(CHARTAB_CHARPIECE + 2)
{
    m_pChar.m_pAttBone = pAttBone;
    m_pSkinPiece = pSkinPiece;
}
#endif

bool CLODData::Init()
{
    TSTRING strFile;

    if (GetFilename(strFile))
    {
        Read(strFile);
    }
    else
    {
        Write(strFile);
    }

    return SetFile(strFile);
}

int CLODData::OnNotify()
{
    Read(m_strFile);

    return RETURNCODE_SUCCESS;
}

void CLODData::Read(TSTRING& strFile)
{
    if (m_pChar.m_pSkinChar == 0)
    {
        return;
    }
    
    std::locale temp = std::locale::global(std::locale("kor"));

    tfstream fs(strFile.c_str(), tfstream::ios_base::in);

    if (fs.is_open() == false)
    {
        std::locale::global(temp);
        AfxMessageBox(_T("LOD 파일을 읽지 못하였습니다."));
        return;
    }

    json_spirit::Value v;

    if (json_spirit::read(fs, v) == false)
    {
        std::locale::global(temp);
        AfxMessageBox(_T("LOD 파일 형식이 잘못 되었습니다."));
        return;
    }

    fs.close();

    json_spirit::Object& obj = v.get_obj();

    for (size_t i = 0; i < obj.size(); ++i)
    {
        Input(obj[i].name_, obj[i].value_.get_obj());
    }

    std::locale::global(temp);
}

void CLODData::Write(TSTRING& strFile)
{
    if (m_pChar.m_pSkinChar == 0)
    {
        return;
    }

    json_spirit::Object obj;

    Output(obj);

    if (obj.empty() == false)
    {
        std::locale temp = std::locale::global(std::locale("kor"));

        tfstream fs(strFile.c_str(), tfstream::ios_base::out);

        if (fs.is_open() == false)
        {
            std::locale::global(temp);
            return;
        }

        json_spirit::write(obj, fs);

        fs.close();

        std::locale::global(temp);
    }    
}

bool CLODData::GetFilename(TSTRING& strFile)
{
    if (m_pChar.m_pSkinChar == 0)
    {
        return false;
    }

#ifndef DEF_SKIN_SYNC
    if (m_nCharType == CHARTAB_CHARFILE || m_nCharType == CHARTAB_CHARPIECE)
    {
        strFile = DxSkinCharDataContainer::GetInstance().GetPath();
        strFile += _T("\\");
        strFile += m_pChar.m_pSkinChar->GetFileName();
    }
    else if (m_nCharType == CHARTAB_VEHICLE || m_nCharType == CHARTAB_CHARPIECE + 1)
    {
        strFile = DxVehicleDataContainer::GetInstance().GetPath();
        strFile += _T("\\");
        strFile += m_pChar.m_pVehicle->GetFileName();
    }
    else if (m_nCharType == CHARTAB_ATTBONEFILE || m_nCharType == CHARTAB_CHARPIECE + 2)
    {
        strFile = DxAttBoneDataContainer::GetInstance().GetPath();
        strFile += _T("\\");
        strFile += m_pChar.m_pAttBone->GetFileName();
    }
#else
    if (m_nCharType == CHARTAB_CHARFILE)
    {
        strFile = DxSkinCharDataContainer::GetInstance().GetPath();
        strFile += _T("\\");
        strFile += m_pChar.m_pSkinChar->GetFileName();
    }
    else if (m_nCharType == CHARTAB_VEHICLE)
    {
        strFile = DxVehicleDataContainer::GetInstance().GetPath();
        strFile += _T("\\");
        strFile += m_pChar.m_pVehicle->GetFileName();
    }
    else if (m_nCharType == CHARTAB_ATTBONEFILE)
    {
        strFile = DxAttBoneDataContainer::GetInstance().GetPath();
        strFile += _T("\\");
        strFile += m_pChar.m_pAttBone->GetFileName();
    }
#endif

    TCHAR out[MAX_PATH] = {0};
    TCHAR temp[4][MAX_PATH] = {0};
    _tsplitpath_s(strFile.c_str(), temp[0], MAX_PATH, temp[1], MAX_PATH, temp[2], MAX_PATH, temp[3], MAX_PATH);
    _tmakepath_s(out, MAX_PATH, temp[0], temp[1], temp[2], _T("lod"));

    strFile = out;

    //return _taccess(out, 0) == 0 ? true : false;
    return GetFileAttributes(out) != INVALID_FILE_ATTRIBUTES;
}

void CLODData::Input(TSTRING& strParts, json_spirit::Object& obj)
{
    DxSkinPiece* pPiece = GetParts(strParts);

    if (pPiece)
    {
        SSkinCharLODInfo& kLODInfo = pPiece->m_kLODInfo;

        kLODInfo.clear();

        for (size_t i = 0; i < obj.size(); ++i)
        {
            TSTRING strName = obj[i].name_;
            TSTRING strMesh = obj[i].value_.get_str();

            if ( _istdigit(strName[0]) && _istalpha(strMesh[0]) )
            {
                kLODInfo.kDistance.push_back( static_cast<float>( _tstof(strName.c_str()) ) );
                kLODInfo.kMeshName.push_back(strMesh);
            }
        }
    }
}

DxSkinPiece* CLODData::GetParts(const TSTRING& strParts)
{
    if (m_pChar.m_pSkinChar == 0)
    {
        return 0;
    }

    PDXCHARPART pParts = 0;

    if (m_nCharType == CHARTAB_CHARFILE)
    {
        for (int i = 0; i < PIECE_SIZE; ++i)
        {
            if (strParts == DxSkinPiece::m_szPIECETYPE[i])
            {
                return m_pChar.m_pSkinChar->GetPiece(i)->GetPart();
            }
        }
    }
    else if (m_nCharType == CHARTAB_VEHICLE)
    {
        for (int i = 0; i < ATTBONE_SIZE; ++i)
        {
            if (strParts == DxSkinPiece::m_szV_PARTSTYPE[i])
            {
                return m_pChar.m_pVehicle->GetVehiclePart(i)->GetPart();
            }
        }
    }
    else if (m_nCharType == CHARTAB_ATTBONEFILE)
    {
        for (int i = 0; i < PART_V_SIZE; ++i)
        {
            if (strParts == DxSkinPiece::m_szATTBONETYPE[i])
            {
                return m_pChar.m_pAttBone->GetAttBonePiece(i)->GetPart();
            }
        }
    }
#ifndef DEF_SKIN_SYNC
    else if (m_nCharType == CHARTAB_CHARPIECE)
    {
        for (int i = 0; i < PIECE_SIZE; ++i)
        {
            if (strParts == DxSkinPiece::m_szPIECETYPE[i])
            {
                if (m_pSkinPiece->m_emType == i)
                {
                    return m_pSkinPiece;
                }
            }
        }
    }
#endif

    return 0;
}

void CLODData::Output(json_spirit::Object& obj)
{
    PDXCHARPART pParts = 0;

    if (m_nCharType == CHARTAB_CHARFILE)
    {
        for (int i = 0; i < PIECE_SIZE; ++i)
        {
            pParts = m_pChar.m_pSkinChar->GetPiece(i);

            if (pParts)
            {
                SetParts(DxSkinPiece::m_szPIECETYPE[i], obj, pParts->GetPart());
            }
        }
    }
    else if (m_nCharType == CHARTAB_VEHICLE)
    {
        for (int i = 0; i < ATTBONE_SIZE; ++i)
        {
            pParts = m_pChar.m_pVehicle->GetVehiclePart(i);

            if (pParts)
            {
                SetParts(DxSkinPiece::m_szV_PARTSTYPE[i], obj, pParts->GetPart());
            }
        }
    }
    else if (m_nCharType == CHARTAB_ATTBONEFILE)
    {
        for (int i = 0; i < PART_V_SIZE; ++i)
        {
            pParts = m_pChar.m_pAttBone->GetAttBonePiece(i);

            if (pParts)
            {
                SetParts(DxSkinPiece::m_szATTBONETYPE[i], obj, pParts->GetPart());
            }
        }
    }
}

void CLODData::SetParts(const TSTRING& strParts, json_spirit::Object& obj, DxSkinPiece* pPiece)
{
    if (pPiece)
    {
        SSkinCharLODInfo& kLODInfo = pPiece->m_kLODInfo;

        if (kLODInfo.empty() == false)
        {
            TCHAR temp[MAX_PATH] = {0};
            json_spirit::Object sub;

            for (size_t i = 0; i < kLODInfo.size(); ++i)
            {
                float fDist = kLODInfo.kDistance[i];
                TSTRING strFile = kLODInfo.kMeshName[i];

                sprintf_s(temp, MAX_PATH, _T("%f"), fDist);

                sub.push_back(json_spirit::Pair(temp, strFile));
            }

            obj.push_back(json_spirit::Pair(strParts, sub));
        }
    }
}

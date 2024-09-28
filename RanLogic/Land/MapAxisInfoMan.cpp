#include "pch.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "GLMapAxisInfo.h"
#include "MapAxisInfoMan.h"

#include "../../SigmaCore/DebugInclude.h"

MapAxisInfoMan::MapAxisInfoMan()
{
}

MapAxisInfoMan::~MapAxisInfoMan()
{
}

void MapAxisInfoMan::Clear()
{
    m_Data.clear();
}

bool MapAxisInfoMan::Load(const std::string& FileName)
{
    return Load(FileName.c_str());
}

bool MapAxisInfoMan::Load(const char* szFile)
{
    if (!szFile)
        return false;
    std::string FileName(szFile);
    sc::string::lowerString(FileName);

    AXIS_DATA_CITER iter = m_Data.find(FileName);
    if (iter != m_Data.end())
        return true;

    std::tr1::shared_ptr<GLMapAxisInfo> spAxisInfo(new GLMapAxisInfo);
    if (spAxisInfo->LoadFile(FileName.c_str()))
    {
        m_Data.insert(AXIS_DATA_VALUE(FileName, spAxisInfo));
        return true;
    }
    else
    {
        return false;
    }
}

std::tr1::shared_ptr<GLMapAxisInfo> MapAxisInfoMan::GetInfo(const std::string& FileName)
{
    std::string LowFileName(FileName);
    sc::string::lowerString(LowFileName);

    AXIS_DATA_CITER iter = m_Data.find(LowFileName);
    if (iter != m_Data.end())
        return iter->second;
    else
        return std::tr1::shared_ptr<GLMapAxisInfo> ((GLMapAxisInfo*) 0);
}
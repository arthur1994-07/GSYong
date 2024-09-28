#ifndef _GLOGIC_MAP_AXIS_INFO_MAN_H_
#define _GLOGIC_MAP_AXIS_INFO_MAN_H_

#include <map>
#include <boost/tr1/memory.hpp>

class GLMapAxisInfo;

//! 맵의 크기 및 위치 정보를 관리(미니맵)
class MapAxisInfoMan
{
public:
    MapAxisInfoMan();
    ~MapAxisInfoMan();

protected:
    typedef std::map<std::string, std::tr1::shared_ptr<GLMapAxisInfo>> AXIS_DATA;
    typedef AXIS_DATA::iterator       AXIS_DATA_ITER;
    typedef AXIS_DATA::const_iterator AXIS_DATA_CITER;
    typedef AXIS_DATA::value_type     AXIS_DATA_VALUE;

    AXIS_DATA m_Data;

public:
    bool Load(const char* szFile);
    bool Load(const std::string& FileName);
    void Clear();
    size_t Size() const { return m_Data.size(); }

    std::tr1::shared_ptr<GLMapAxisInfo> GetInfo(const std::string& FileName);
};

#endif // _GLOGIC_MAP_AXIS_INFO_MAN_H_
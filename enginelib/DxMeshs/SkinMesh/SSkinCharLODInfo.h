#pragma once

#include <string>
#include <vector>

typedef std::vector<float>          FloatContainer;
typedef FloatContainer::iterator    FloatContainerIterator;
typedef std::vector<TSTRING>        StringContainer;
typedef StringContainer::iterator   StringContainerIterator;

const int NO_LOD = -1;

/*
    LOD 레벨이 -1 일 경우 초기 상태이며               
                0 이상일 경우 LOD 가 된 상태 이다.

    LOD 레벨운 수치가 높아질수록 디테일도는 떨어진다.

*/
struct SSkinCharLODInfo
{
    FloatContainer      kDistance;  //@ LOD 적용 시작 거리
    StringContainer     kMeshName;  //@ LOD 메쉬명   
    int                 nLevel;     //@ LOD 현재 레벨    

    void resize(int i)  { kDistance.resize(i); kMeshName.resize(i); }
    bool empty()        { return kDistance.empty(); }
    size_t size()       { return kDistance.size(); }
    void clear()        { kDistance.clear(); kMeshName.clear(); nLevel = NO_LOD; }

    SSkinCharLODInfo() : nLevel(NO_LOD) {}
};
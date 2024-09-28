#include "pch.h"
#include "../SigmaCore/Util/SystemInfo.h"
#include "./GameVersion.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace gv
{
    int Version::m_LauncherVersion = 0;
    int Version::m_GameVersion = 0;
    bool Version::m_bLoad = false;

    Version::Version()
    {
    }

    Version::~Version()
    {
    }

    void Version::GetVersion(int& LauncherVersion, int& GameVersion)
    {        
        // 런처에도 버전을 로드하는 부분이 있습니다.
        // 변경될 경우 반드시 체크 해야합니다.
        // AutoPatchManDlgEx.cpp!!!
        
        if (m_bLoad)
        {
            LauncherVersion = m_LauncherVersion;
            GameVersion = m_GameVersion;
            return;
        }

        std::string VersionFileName(sc::getAppPath());
        VersionFileName.append("\\");
        VersionFileName.append("cVer.bin");
        
        FILE* cfp = _fsopen(VersionFileName.c_str(), "rb", _SH_DENYNO);
        if (!cfp)
            return;

        if (1 != fread(&m_LauncherVersion, sizeof(int), 1, cfp))
        {
            fclose(cfp);
            return;
        }

        if (1 != fread(&m_GameVersion, sizeof(int), 1, cfp))
        {
            fclose(cfp);
            return;
        }

        fclose(cfp);
        m_bLoad = true;
}

} // namespace gv
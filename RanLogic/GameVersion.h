#ifndef _GL_GAME_VERSION_H_
#define _GL_GAME_VERSION_H_

#pragma once

namespace gv // game version
{
    class Version
    {
    public:
        Version();
        ~Version();
    
    private:
        static int m_LauncherVersion;
        static int m_GameVersion;
        static bool m_bLoad;

    public:
        static void GetVersion(int& LauncherVersion, int& GameVersion);
    };

} // namespace gv

#endif // _GL_GAME_VERSION_H_
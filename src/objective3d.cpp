/**
 * @file objective3d.cpp
 * @brief Application dynamic library entry
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (C) 2001-2017 Dream Overflow. All rights reserved.
 * @details
 */

#include "o3d/objective3d.h"
#include "o3d/core/architecture.h"

// it's needed for DLL exportation
#define O3D_NO_LIB_FLAG

#ifdef O3D_MACOSX
  // Mac OS part
  #ifndef __O3D_NOLIB__
    int main(int argc, char *argv[])
    {
        return 0;
    }
  #endif // __O3D_NOLIB__
#elif defined(O3D_WINDOWS)
    // Windows part
    BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
    {
        switch (ul_reason_for_call) {
            case DLL_PROCESS_ATTACH:
            case DLL_THREAD_ATTACH:
            case DLL_THREAD_DETACH:
            case DLL_PROCESS_DETACH:
                break;
        }

        return TRUE;
    }
#endif

#undef O3D_NO_LIB_FLAG

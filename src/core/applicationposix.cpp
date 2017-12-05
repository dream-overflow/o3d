/**
 * @file applicationposix.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/application.h"

#ifdef O3D_POSIX_SYS

#include <iostream>
#include <fcntl.h>

#include "o3d/core/filemanager.h"
#include "o3d/core/debug.h"

using namespace o3d;

// Show a message. On win32 application it draw a standard message box,
void Application::message(
		const String &content,
		const String &title,
		Application::IconStyle icon)
{
	CString c = content.isValid() ? content.toUtf8() : "<undefined>";
	CString t = title.isValid() ? title.toUtf8() : "<untitled>";

    switch (icon) {
		case ICON_HELP:
			std::cout << "[HELP] " << t.getData() << ": " << c.getData() << std::endl;
			return;

		case ICON_OK:
			std::cout << "[OK] " << t.getData() << ": " << c.getData() << std::endl;
			return;

		case ICON_WARNING:
			std::cout << "[WARNING] " << t.getData() << ": " << c.getData() << std::endl;
			return;

		case ICON_INFORMATION:
			std::cout << "[INFO] " << t.getData() << ": " << c.getData() << std::endl;
			return;

		case ICON_NOTICE:
			std::cout << "[NOTICE] " << t.getData() << ": " << c.getData() << std::endl;
			return;

		case ICON_QUESTION:
			std::cout << "[QUESTION] " << t.getData() << ": " << c.getData() << std::endl;
			return;

		case ICON_ERROR:
			std::cout << "[ERROR] " << t.getData() << ": " << c.getData() << std::endl;
			return;

		default:
			std::cout << "[NOTICE] " << t.getData() << ": " << c.getData() << std::endl;
			return;
	}
}

// Map a single file. It is mostly used to make a single instance application.
void Application::mapSingleFile(const String &name)
{
	struct flock fl;

	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 1;

	String fileName = "/tmp/" + name + ".lock";
	int fdlock;

    if ((fdlock = open(fileName.toUtf8().getData(), O_WRONLY | O_CREAT, 0666)) == -1) {
		O3D_ERROR(E_InvalidOperation("Unable to create the file lock (mapped file)"));
    }

    if (fcntl(fdlock, F_SETLK, &fl) == -1) {
		O3D_ERROR(E_InvalidOperation("The single file is already mapped by another instance"));
    }
}

// Check for a mapped file existence.
Bool Application::isMappedFileExists(const String &name)
{
	struct flock fl;

	fl.l_type = F_WRLCK;
	fl.l_whence = SEEK_SET;
	fl.l_start = 0;
	fl.l_len = 1;

	String fileName = "/tmp/" + name + ".lock";
	int fdlock;

    if ((fdlock = open(fileName.toUtf8().getData(), O_WRONLY | O_CREAT, 0666)) == -1) {
		return True;
    }

    if (fcntl(fdlock, F_SETLK, &fl) == -1) {
		return True;
    }

	return False;
}

Int32 Application::getPID()
{
	pid_t pid = getpid();
    return pid;
}

void Application::getBaseNamePrivate(Int32 argc, Char **argv)
{
    if (argv && (argc >= 1) && argv[0])	{
        ms_appsName = new String;
        ms_appsName->fromUtf8(argv[0]);

//        char szTmp[32];
//        sprintf(szTmp, "/proc/%d/exe", getpid());
//        int bytes = MIN(readlink(szTmp, pBuf, len), len - 1);
//        if(bytes >= 0) {
//            pBuf[bytes] = '\0';
//        }
//        String path = String::fromUtf8(bytes);

        Int32 pos = ms_appsName->reverseFind('/');
        if (pos >= 0) {
            String path = ms_appsName->extract(0, pos+1);
            ms_appsPath = new String(FileManager::instance()->getFullFileName(path));
        } else {
            ms_appsPath = new String(FileManager::instance()->getWorkingDirectory());
        }
    } else {
        ms_appsName = new String("undefined");
        ms_appsPath = new String(FileManager::instance()->getWorkingDirectory());
    }
}

#endif // O3D_POSIX_SYS

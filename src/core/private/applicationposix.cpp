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
    String msg;

    switch (icon) {
		case ICON_HELP:
            msg = String("[HELP] ") + title + ": " + content;
            puts(msg.toUtf8().getData());
			return;

		case ICON_OK:
            msg = String("[OK] ") + title + ": " + content;
            puts(msg.toUtf8().getData());
			return;

		case ICON_WARNING:
            msg = String("[WARNING] ") + title + ": " + content;
            puts(msg.toUtf8().getData());
			return;

		case ICON_INFORMATION:
            msg = String("[INFO] ") + title + ": " + content;
            puts(msg.toUtf8().getData());
			return;

		case ICON_NOTICE:
            msg = String("[NOTICE] ") + title + ": " + content;
            puts(msg.toUtf8().getData());
			return;

		case ICON_QUESTION:
            msg = String("[QUESTION] ") + title + ": " + content;
            puts(msg.toUtf8().getData());
			return;

		case ICON_ERROR:
            msg = String("[ERROR] ") + title + ": " + content;
            puts(msg.toUtf8().getData());
            return;

		default:
            msg = String("[NOTICE] ") + title + ": " + content;
            puts(msg.toUtf8().getData());
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
    #ifdef PATH_MAX
    const ssize_t MAX_PATH = PATH_MAX;
    #else
    const ssize_t MAX_PATH = 1024;
    #endif

    Char szTmp[32];
    Char *pBuf = (char*)malloc(MAX_PATH);
    pBuf[0] = '\0';

    sprintf(szTmp, "/proc/%d/exe", getpid());
    ssize_t size = o3d::min<ssize_t>(readlink(szTmp, pBuf, MAX_PATH), MAX_PATH-1);
    if (size >= 0) {
        pBuf[size] = '\0';

        ms_appsName = new String;
        ms_appsName->fromUtf8(pBuf);

        Int32 pos = ms_appsName->reverseFind('/');
        if (pos >= 0) {
            String path = ms_appsName->extract(0, pos+1);
            ms_appsPath = new String(FileManager::instance()->getFullFileName(path));
        } else {
            ms_appsPath = new String(FileManager::instance()->getWorkingDirectory());
        }
    } else if (argv && (argc >= 1) && argv[0])	{
        ms_appsName = new String;
        ms_appsName->fromUtf8(argv[0]);

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

    free(pBuf);
}

#endif // O3D_POSIX_SYS

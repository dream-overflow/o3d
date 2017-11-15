/**
 * @file debug.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/debug.h"

#include "o3d/core/thread.h"

#include <time.h>

using namespace o3d;

static FastMutex O3D_DebugMutex;

// singleton instance
Debug* Debug::m_instance = nullptr;

//---------------------------------------------------------------------------------------
// Singleton instantiation
//---------------------------------------------------------------------------------------
Debug* Debug::instance()
{
	if (!m_instance)
		m_instance = new Debug();
    return m_instance;
}

//---------------------------------------------------------------------------------------
// Singleton destruction
//---------------------------------------------------------------------------------------
void Debug::destroy()
{
	if (m_instance)
	{
		delete m_instance;
        m_instance = nullptr;
	}
}

//---------------------------------------------------------------------------------------
// Restricted default ctor
//---------------------------------------------------------------------------------------
Debug::Debug() :
    m_defaultLog(new FileLogger("default.log"))
{
	m_instance = (Debug*)this; // Used to avoid recursive call when the ctor call himself...
}

Debug::~Debug()
{
    deletePtr(m_defaultLog);
}

void Debug::throwDebug(
        Logger::LogLevel logLevel,
        const String &type,
        const String &descr,
        const String &msg,
        const String &filename,
        const Char *_file_,
        UInt32 _line_,
        const Char *_function_)
{
	O3D_DebugMutex.lock();

    if (m_defaultLog)
	{
		String head;
        if (logLevel == Logger::MESSAGE)
			head = "<MSG> " ;
        else if (logLevel == Logger::WARNING)
			head = "<WRN> ";
        else if (logLevel == Logger::ERROR)
			head = "<ERR> ";
        else if (logLevel == Logger::CRITICAL)
            head = "<CRI> ";

        String details = "";

        if (logLevel >= Logger::ERROR)
            details = String::print("\n    at line(%i) file(%s) function(%s)", _line_, _file_, _function_);

		if (filename.isEmpty())
            m_defaultLog->log(
                        logLevel,
                        head + descr + (descr.length() > 0 ? ", " : "") + msg + details);
		else
            m_defaultLog->log(
                        logLevel,
                        head + "<" + filename + "> " + descr + (descr.length() > 0 ? ", " : "") + msg + details);
	}

	O3D_DebugMutex.unlock();

	// Events
	DebugInfo info;
	info.Exception = type;
	info.Message = msg;
	info.FileName.fromUtf8(_file_);
	info.Line = _line_;
	info.Function.fromUtf8(_function_);
	info.WorkingFile = filename;

    if (logLevel == Logger::MESSAGE)
	{
		info.Descriptor = "Message";
		info.Type = DebugInfo::MESSAGE_INFO;
		throwMessage(info);
		throwAll(info);
	}
    else if (logLevel == Logger::WARNING)
	{
		info.Descriptor = "Warning";
		info.Type = DebugInfo::WARNING_INFO;
		throwWarning(info);
		throwAll(info);
	}
    else if (logLevel == Logger::ERROR)
	{
		info.Descriptor = descr;
		info.Type = DebugInfo::ERROR_INFO;
        throwError(info);
		throwAll(info);
	}
    else if (logLevel == Logger::CRITICAL)
    {
        info.Descriptor = descr;
        info.Type = DebugInfo::CRITICAL_INFO;
        throwCritical(info);
        throwAll(info);
    }
}

void Debug::throwDebug(
        Logger::LogLevel logLevel,
        const String &type,
        const String &descr,
        const String &msg,
        const String &filename,
        const WChar *_file_,
        UInt32 _line_,
        const WChar *_function_)
{
	O3D_DebugMutex.lock();

    if (m_defaultLog)
	{
		String head;
        if (logLevel == Logger::MESSAGE)
			head = "<MSG> " ;
        else if (logLevel == Logger::WARNING)
			head = "<WRN> ";
        else if (logLevel == Logger::ERROR)
			head = "<ERR> ";
        else if (logLevel == Logger::CRITICAL)
            head = "<CRI> ";

        String details = "";

        // for error and critical
        if (logLevel >= Logger::ERROR)
            details = String::print("\n    at line(%i) file(%ls) function(%ls)", _line_, _file_, _function_);

        if (filename.isEmpty())
            m_defaultLog->log(
                        logLevel,
                        head + descr + (descr.length() > 0 ? ", " : "") + msg + details);
        else
            m_defaultLog->log(
                        logLevel,
                        head + "<" + filename + "> " + descr + (descr.length() > 0 ? ", " : "") + msg + details);
	}

	O3D_DebugMutex.unlock();

	// Events
	DebugInfo info;
	info.Exception = type;
	info.Message = msg;
	info.FileName = _file_;
	info.Line = _line_;
	info.Function = _function_;
	info.WorkingFile = filename;

    if (logLevel == Logger::MESSAGE)
	{
		info.Descriptor = "Message";
		info.Type = DebugInfo::MESSAGE_INFO;
		throwMessage(info);
		throwAll(info);
	}
    else if (logLevel == Logger::WARNING)
	{
		info.Descriptor = "Warning";
		info.Type = DebugInfo::WARNING_INFO;
		throwWarning(info);
		throwAll(info);
	}
    else if (logLevel == Logger::ERROR)
	{
		info.Descriptor = descr;
		info.Type = DebugInfo::ERROR_INFO;
        throwError(info);
		throwAll(info);
    }
    else if (logLevel == Logger::CRITICAL)
    {
        info.Descriptor = descr;
        info.Type = DebugInfo::CRITICAL_INFO;
        throwCritical(info);
        throwAll(info);
    }
}

void Debug::trace(
        const String &msg,
        const Char *_file_,
        UInt32 _line_,
        const Char *_function_)
{
    O3D_DebugMutex.lock();

    if (m_defaultLog)
    {
        String head = "<TRACE> ";
        m_defaultLog->log(
                    Logger::TRACE,
                    head + String::print("<%s:%i> in %s: ", _file_, _line_, _function_) + msg);
    }

    O3D_DebugMutex.unlock();
}

void Debug::trace(
        const String &msg,
        const WChar *_file_,
        UInt32 _line_,
        const WChar *_function_)
{
    O3D_DebugMutex.lock();

    if (m_defaultLog)
    {
       String head = "<TRACE> ";
#ifdef O3D_WINDOWS
        m_defaultLog->log(
                    Logger::TRACE,
                    String::print("<%s:%i> in %s: ", _file_, _line_, _function_) + msg);
#else
        m_defaultLog->log(
                    Logger::TRACE,
                    String::print("<%ls:%i> in %ls: ", _file_, _line_, _function_) + msg);
#endif
    }

    O3D_DebugMutex.unlock();
}

//---------------------------------------------------------------------------------------
// Get the default logger
//---------------------------------------------------------------------------------------
Logger& Debug::getDefaultLog()
{
	FastMutexLocker locker(O3D_DebugMutex);
    return *m_defaultLog;
}

//---------------------------------------------------------------------------------------
// Enable disable the default logger
//---------------------------------------------------------------------------------------
void Debug::setDefaultLog(const String &filename)
{
	FastMutexLocker locker(O3D_DebugMutex);
    deletePtr(m_defaultLog);

    if (filename.isValid())
        m_defaultLog = new FileLogger(filename);
}

//---------------------------------------------------------------------------------------
// Check if the default logger is enabled
//---------------------------------------------------------------------------------------
Bool Debug::isDefaultLog()
{
	FastMutexLocker locker(O3D_DebugMutex);
    return (m_defaultLog != nullptr);
}


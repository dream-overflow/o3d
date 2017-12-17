/**
 * @file logger.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/logger.h"

#include "o3d/core/architecture.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/debug.h"
#include "o3d/core/datetime.h"

using namespace o3d;

Logger::~Logger()
{

}

FileLogger::FileLogger(const String &logname) :
    m_logLevel(INFO),
    m_logFilename(logname),
    m_os(nullptr)
{
}

FileLogger::~FileLogger()
{
    deletePtr(m_os);
}

void FileLogger::log(LogLevel level, const String &str)
{
    // minimal log level
    if (level < m_logLevel) {
        return;
    }

    if (m_absolutefname.isEmpty()) {
        m_absolutefname = FileManager::instance()->getFullFileName(m_logFilename);
    }

    if (!m_os) {
        m_os = FileManager::instance()->openOutStream(
                    m_absolutefname,
                    FileOutStream::APPEND);
    }

    DateTime current(True);
    String date = current.buildString(m_dateFormat);
    if (!date.isEmpty()) {
        date += ' ';
    }

	String time = String::print("[%.5f] ", ((Float)System::getTime() / System::getTimeFrequency()) );

    m_os->writeLine(String(date + time + str));
    m_os->flush();
}

void FileLogger::setLogFileName(const String &name)
{
	m_logFilename = name;
    m_absolutefname = FileManager::instance()->getFullFileName(m_logFilename);

    deletePtr(m_os);
}

const String& FileLogger::getLogFileName() const
{
    return m_logFilename;
}

void FileLogger::setDateFormat(const String &format)
{
    m_dateFormat = format;
}

const String &FileLogger::getDateFormat() const
{
    return m_dateFormat;
}

void FileLogger::clearLog()
{
    if (m_absolutefname.isEmpty()) {
        m_absolutefname = FileManager::instance()->getFullFileName(m_logFilename);
    }

    if (!m_os) {
        delete m_os;
        m_os = nullptr;
    }

    m_os = FileManager::instance()->openOutStream(
               m_absolutefname,
               FileOutStream::CREATE);
}

void FileLogger::setLogLevel(FileLogger::LogLevel minLevel)
{
    m_logLevel = minLevel;
}

FileLogger::LogLevel FileLogger::getLogLevel() const
{
    return m_logLevel;
}

#ifdef O3D_ANDROID

#include "o3d/core/application.h"
#include "android/android_native_app_glue.h"

#include <android/log.h>

AndroidLogger::AndroidLogger(const String &prefix) :
    m_logLevel(INFO),
    m_prefix(prefix)
{
}

AndroidLogger::~AndroidLogger()
{
}

void AndroidLogger::log(LogLevel level, const String &str)
{
    // minimal log level
    if (level < m_logLevel) {
        return;
    }

    DateTime current(True);
    String date = current.buildString(m_dateFormat);
    if (!date.isEmpty()) {
        date += ' ';
    }

    String time = String::print("[%.5f] ", ((Float)System::getTime() / System::getTimeFrequency()) );

    String msg = date + time + str;

    int alvl;

    // ANDROID_LOG_DEFAULT
    // ANDROID_LOG_VERBOSE
    // ANDROID_LOG_DEBUG
    // ANDROID_LOG_SILENT

    switch (level) {
        case TRACE:
            alvl = ANDROID_LOG_VERBOSE;
            break;
        case INFO:
            alvl = ANDROID_LOG_INFO;
            break;
        case WARNING:
            alvl = ANDROID_LOG_WARN;
            break;
        case ERROR:
            alvl = ANDROID_LOG_ERROR;
            break;
        case CRITICAL:
            alvl = ANDROID_LOG_FATAL;
            break;
        default:
            alvl = ANDROID_LOG_INFO;
            break;
    }

    String tag = Application::getAppName();
    if (m_prefix.isValid()) {
        tag += " " + m_prefix;
    }

    __android_log_write(alvl, tag.toUtf8().getData(), msg.toUtf8().getData());
}

void AndroidLogger::setDateFormat(const String &format)
{
    m_dateFormat = format;
}

const String &AndroidLogger::getDateFormat() const
{
    return m_dateFormat;
}

void AndroidLogger::clearLog()
{

}

void AndroidLogger::setLogLevel(AndroidLogger::LogLevel minLevel)
{
    m_logLevel = minLevel;
}

AndroidLogger::LogLevel AndroidLogger::getLogLevel() const
{
    return m_logLevel;
}

#endif // O3D_ANDROID

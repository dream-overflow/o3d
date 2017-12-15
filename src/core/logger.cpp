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

void FileLogger::writeHeaderLog()
{
    if (m_absolutefname.isEmpty())
        m_absolutefname = FileManager::instance()->getFullFileName(m_logFilename);

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

    String str = "Starting of application on " + current.buildString("%y-%M-%D at %h:%i:%s.%l");
    String time = String::print("[%.5f] ", ((Float)System::getTime() / System::getTimeFrequency()));

    m_os->writeLine(String(date + time + str));
    m_os->flush();
}

void FileLogger::writeFooterLog()
{
    if (m_absolutefname.isEmpty())
        m_absolutefname = FileManager::instance()->getFullFileName(m_logFilename);

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

    String str = "Terminating application on " + current.buildString("%y-%M-%D at %h:%i:%s.%l");
    String time = String::print("[%.5f] ", ((Float)System::getTime() / System::getTimeFrequency()));

    m_os->writeLine(String(date + time + str));
    m_os->flush();
}

void FileLogger::clearLog()
{
    if (m_absolutefname.isEmpty()) {
        m_absolutefname = FileManager::instance()->getFullFileName(m_logFilename);
    }

    if (!m_os) {
        m_os = FileManager::instance()->openOutStream(
                    m_absolutefname,
                    FileOutStream::CREATE);
    }
}

void FileLogger::setLogLevel(FileLogger::LogLevel minLevel)
{
    m_logLevel = minLevel;
}

FileLogger::LogLevel FileLogger::getLogLevel() const
{
    return m_logLevel;
}

/**
 * @file logger.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_LOGGER_H
#define _O3D_LOGGER_H

#include "memorydbg.h"
#include "callback.h"
#include "string.h"

namespace o3d {

/**
 * @brief Interface for logger.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2015-12-30
 */
class O3D_API Logger
{
public:

    //! Log levels
    enum LogLevel
    {
        TRACE = 0,
        INFO = 1,
        MESSAGE = INFO,
        WARNING = 2,
        ERROR = 3,
        CRITICAL = 4
    };

    virtual ~Logger() = 0;

    //! write a message in the log file
    //! @param str The message to write into the new line
    virtual void log(LogLevel level, const String &str) = 0;

    //! set the date format, default is no date, empty string mean none.
    virtual void setDateFormat(const String &format) = 0;
    //! get the date format, can be empty.
    virtual const String& getDateFormat() const = 0;

    //! write header banner to log file
    virtual void writeHeaderLog() = 0;

    //! write footer banner to log file
    virtual void writeFooterLog() = 0;

    //! clean log (if supported by the stream)
    virtual void clearLog() = 0;

    //! set the minimal log level.
    virtual void setLogLevel(LogLevel minLevel) = 0;

    //! get the minimal log level.
    virtual LogLevel getLogLevel() const = 0;
};

/**
 * @brief Base file system logger.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2005-11-04
 */
class O3D_API FileLogger : public Logger
{
public:

	//! constructor
    FileLogger(const String& logname);

    virtual ~FileLogger();

    //! set the log file name
    void setLogFileName(const String &name);
    //! get the log file name
    const String& getLogFileName() const;

	//! write a message in the log file
    //! @param str The message to write
    virtual void log(LogLevel level, const String &str);

    //! set the date format, default is no date, empty string mean none.
    virtual void setDateFormat(const String &format);
    //! get the date format, can be empty.
    virtual const String& getDateFormat() const;

	//! write header banner to log file
    virtual void writeHeaderLog();

	//! write footer banner to log file
    virtual void writeFooterLog();

	//! clean log file
    virtual void clearLog();

    //! set the minimal log level.
    virtual void setLogLevel(LogLevel minLevel);

    //! get the minimal log level.
    virtual LogLevel getLogLevel() const;

protected:

    LogLevel m_logLevel;    //!< minimal log level

	String m_absolutefname; //!< the filename in absolute path
	String m_logFilename;   //!< log file name
    String m_dateFormat;    //!< date format

    OutStream *m_os;        //!< file output stream
};

} // namespace o3d

#endif // _O3D_LOGGER_H

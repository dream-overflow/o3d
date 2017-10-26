/**
 * @file debug.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_DEBUG_H
#define _O3D_DEBUG_H

//! Debug, error, warning, message using macros and methods. Use them when you
//! need to throw an exception, to log or messagebox a message or a warning and so on.
//! Common exceptions are defined in Error.h header.

#include "logger.h"
#include "error.h"
#include "evt.h"

#define WIDEN2(x) L##x
#define WIDEN(x) WIDEN2(x)

#ifdef _MSC_VER
	#define __TFILE__ WIDEN(__FILE__)
	#define __TFUNCTION__ WIDEN(__FUNCTION__)
#else
	#define __TFILE__ __FILE__
	#define __TFUNCTION__ __FUNCTION__
#endif

#if defined(_MSC_VER) && (O3D_USE_CRT_WNDREPORT == 1)
	#ifdef _DEBUG
		//! Throw an error. Abort exit the program. Retry make a break on the line. Ignore trow the exception.
		#define O3D_ERROR(EXCEPTION_TYPE)                \
			(void)((Debug::instance()->throwDebug(       \
                        Logger::ERROR,                   \
						(EXCEPTION_TYPE).getType(),      \
						(EXCEPTION_TYPE).getDescr(),     \
						(EXCEPTION_TYPE).getMsg(),       \
						(EXCEPTION_TYPE).getFileName(),  \
						__TFILE__,                       \
						__LINE__,                        \
						__TFUNCTION__), 0) ||            \
				((1 == _CrtDbgReportW(                   \
						_CRT_ERROR,                      \
						__TFILE__,                       \
						__LINE__,                        \
						__TFUNCTION__,                   \
						(EXCEPTION_TYPE).getMsg().getData()) && \
				(_CrtDbgBreak(), 1)) ||                  \
				(throw ((EXCEPTION_TYPE).set(__LINE__, __TFILE__, __TFUNCTION__)), 0)))
	#else // NDEBUG
		//! Throw an error
		#define O3D_ERROR(EXCEPTION_TYPE)               \
			(void)((Debug::instance()->throwDebug(      \
                        Logger::ERROR,                  \
						(EXCEPTION_TYPE).getType(),     \
						(EXCEPTION_TYPE).getDescr(),    \
						(EXCEPTION_TYPE).getMsg(),      \
						(EXCEPTION_TYPE).getFileName(), \
						__TFILE__,                      \
						__LINE__,                       \
						__TFUNCTION__), 0) ||           \
				(throw((EXCEPTION_TYPE).set(__LINE__, __TFILE__, __TFUNCTION__)), 0))
	#endif // NDEBUG
#else
	#ifdef _DEBUG
		//! Throw an error (O3D_DBGBREAK() wont compile here...)
		#define O3D_ERROR(EXCEPTION_TYPE)                  \
			(void)((Debug::instance()->throwDebug(         \
                        Logger::ERROR,                     \
						(EXCEPTION_TYPE).getType(),        \
						(EXCEPTION_TYPE).getDescr(),       \
						(EXCEPTION_TYPE).getMsg(),         \
						(EXCEPTION_TYPE).getFileName(),    \
						__TFILE__,                         \
						__LINE__,                          \
						__TFUNCTION__), 0) ||              \
				(throw((EXCEPTION_TYPE).set(__LINE__, __TFILE__, __TFUNCTION__)), 0))
	#else // NDEBUG
		//! Throw an error
		#define O3D_ERROR(EXCEPTION_TYPE)               \
			(void)((Debug::instance()->throwDebug(      \
                        Logger::ERROR,                  \
						(EXCEPTION_TYPE).getType(),     \
						(EXCEPTION_TYPE).getDescr(),    \
						(EXCEPTION_TYPE).getMsg(),      \
						(EXCEPTION_TYPE).getFileName(), \
						__TFILE__,                      \
						__LINE__,                       \
						__TFUNCTION__), 0) ||           \
				(throw((EXCEPTION_TYPE).set(__LINE__, __TFILE__, __TFUNCTION__)),0 ))
	#endif // NDEBUG
#endif

//! Warning helper. Take a String or Char* message
#define O3D_WARNING(MSG)            \
	Debug::instance()->throwDebug(  \
        Logger::WARNING,            \
		"",                         \
		"",                         \
		(MSG),                      \
		"",                         \
		__TFILE__,                  \
		__LINE__,                   \
		__TFUNCTION__)

//! Information Message helper. Take a String or Char* message
#define O3D_MESSAGE(MSG)                 \
	Debug::instance()->throwDebug(       \
        Logger::MESSAGE,                 \
		"",                              \
		"",                              \
		(MSG),                           \
		"",                              \
		__TFILE__,                       \
		__LINE__,                        \
		__TFUNCTION__)

//! Trace helper. Take a String or Char* message
#define O3D_TRACE(MSG)              \
    Debug::instance()->trace(       \
        (MSG),                      \
        __TFILE__,                  \
        __LINE__,                   \
        __TFUNCTION__)

//! Log a message directly to the default logger
#define O3D_LOG(LEVEL, MSG) Debug::instance()->getDefaultLog().log((LEVEL), (MSG))

namespace o3d {

/**
 * @brief Contain information about a signal emitted by Debug
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2007-11-10
 */
class O3D_API DebugInfo
{
public:

	enum DebugInfoType
	{
        MESSAGE_INFO=0,
		WARNING_INFO,
        ERROR_INFO,
        CRITICAL_INFO
	};

	DebugInfoType Type; //!< The type of the debug info

	String Exception;   //!< If exception, it contain the name of the exception class object
	String Descriptor;  //!< Describe the exception/warning/message (Use respectively "Warning" and "Message" for warning and message)
	String Message;     //!< Additional description

	String FileName;    //!< The source file where the debug info appear
    UInt32 Line;        //!< The line of the file where the debug info appear
	String Function;    //!< The function/method where the debug info appear

	String WorkingFile; //!< The working file used when the debug info appear (useful for file exception/message/warning)
};

/**
 * @brief Debug manager.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2007-11-10
 * Debug is used to throw an error, a warning or a simple information message.
 * Next, event are raised, and listeners can filter the received error/warning/message
 * as they want.
 * A default behavior is defined to write all error/warning/message in the log file
 * managed by the Logger singleton.
 * This default behavior can be disabled.
 * @todo Add the capacity to manage a list of Loggers.
 */
class O3D_API Debug : public EvtHandler
{
public:

	//! Get the singleton instance
	static Debug* instance();
	//! Delete the singleton instance
	static void destroy();

	//! Throw a debug message
	void throwDebug(
        Logger::LogLevel logLevel,
		const String &type,
		const String &descr,
		const String &msg,
		const String &filename,
		const Char *_file_,
		UInt32 _line_,
		const Char *_function_);

	//! Throw a debug message
	void throwDebug(
        Logger::LogLevel logLevel,
		const String &type,
		const String &descr,
		const String &msg,
		const String &filename,
		const WChar *_file_,
		UInt32 _line_,
		const WChar *_function_);

    //! Trace message
    void trace(
        const String &msg,
        const Char *_file_,
        UInt32 _line_,
        const Char *_function_);

    //! Trace message
    void trace(
        const String &msg,
        const WChar *_file_,
        UInt32 _line_,
        const WChar *_function_);

	//! Get the default logger
    Logger& getDefaultLog();

	//! Enable disable the default logger
    //! @param filename Default log filename. If empty or null string default log is disabled.
    void setDefaultLog(const String &filename = "");

	//! Check if the default logger is enabled
	Bool isDefaultLog();

public:

    Signal<DebugInfo> throwCritical{this};   //!< Filters only criticals errors
    Signal<DebugInfo> throwError{this};      //!< Filters only errors
    Signal<DebugInfo> throwWarning{this};    //!< Filters only warnings
    Signal<DebugInfo> throwMessage{this};    //!< Filters only message informations
    Signal<DebugInfo> throwAll{this};        //!< No Filters

private:

    Logger *m_defaultLog;   //!< The default logger

	static Debug* m_instance;

	Debug();
	virtual~Debug();
	Debug(const Debug& dbg);
	void operator=(const Debug& dbg);
};

} // namespace o3d

#endif // _O3D_DEBUG_H


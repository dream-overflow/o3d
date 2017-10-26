/**
 * @file file.h
 * @brief Base base for disk and memory file objects.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2002-01-02
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_FILE_H
#define _O3D_FILE_H

#include "string.h"
#include "memorydbg.h"

namespace o3d {

//! @class BlockData Useful struct for data reading/writing managing
struct O3D_API BlockData
{
    UInt32 m_delay;       //!< delay between to block of m_size (in ms)
    UInt32 m_size;        //!< size of a chunk of data to read/write
    UInt32 m_bytesec;     //!< value in byte per second
    Bool m_use;           //!< is enable or disable

	BlockData() :
		m_delay(50),
		m_size(25600),
		m_bytesec(512000),
        m_use(False) {}

	BlockData(const BlockData &dup) :
		m_delay(dup.m_delay),
		m_size(dup.m_size),
		m_bytesec(dup.m_bytesec),
		m_use(dup.m_use) {}

	const BlockData& operator= (const BlockData &dup)
	{
		m_delay = dup.m_delay;
		m_size = dup.m_size;
		m_bytesec = dup.m_bytesec;
		m_use = dup.m_use;

		return (*this);
	}
};

/**
 * @brief File utils.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2002-01-02
 */
class O3D_API File
{
public:

	//! When a path contain some '..' this method reform the path without '..' and './'
	static void adaptPath(String &path);

	//! Adapt an absolute path of a file to another absolute path. ex:
	//! - 'c:/tool/one/two/file.txt' as source
	//! - 'c:/tool/one/one/one' as path
	//! result in '../../two/file.txt'
	static String convertPath(const String &filename, const String &pathname);

	//! Get the file name and the path name from a complete (full) file name.
	//! @param Input fullFileName to parse.
	//! @param filename The filename only (output).
	//! @param pathname The path only (output).
	static void getFileNameAndPath(
		const String &fullFileName,
		String &filename,
		String &pathname);
};

} // namespace o3d

#endif // _O3D_FILE_H


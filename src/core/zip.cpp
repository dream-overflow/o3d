/**
 * @file zip.cpp
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/zip.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/datainstream.h"

#include "o3d/core/debug.h"

#ifdef _MSC_VER
	#include <zlib/zlib.h>
#else
	#include <zlib.h>
#endif

using namespace o3d;

//! MAX_PATH isn't defined under UNIX
#ifndef MAX_PATH
	#ifdef PATH_MAX
		#define MAX_PATH PATH_MAX
	#else
		#ifdef O3D_WINDOWS
			#define MAX_PATH 260
		#else
			#define MAX_PATH 1024
		#endif
	#endif
#endif

// BigEndian
#define O3DZipDataDescriptorToBIG(S)           \
    System::swapBytes4(&S.Crc32);              \
    System::swapBytes4(&S.CompressedSize);     \
    System::swapBytes4(&S.UnCompressedSize);

#define O3DZipHeaderToBIG(S)                      \
    System::swapBytes4(&S.Sig);                   \
    System::swapBytes2(&S.VersionToExtract);      \
    System::swapBytes2(&S.BitFlag);               \
    System::swapBytes2(&S.CompressionMethod);     \
    System::swapBytes2(&S.LastModFileTime);       \
    System::swapBytes2(&S.LastModeFileData);      \
    O3DZipDataDescriptorToBIG(S.DataDescriptor);  \
    System::swapBytes2(&S.FileNameLength);        \
    System::swapBytes2(&S.ExtraFieldLength);


// file name (variable size)
// extra field (variable size)

// Infos pour chaque fichier
#define O3DZipTokenToBIG(S)           \
    O3DZipHeaderToBIG(S.FileHeader);  \
    System::swapBytes8(&S.FilePos);


/*---------------------------------------------------------------------------------------
  constructor/destructor
---------------------------------------------------------------------------------------*/
Zip::Zip(InStream &is, const String &zipName, const String &zipPath) :
    m_is(&is),
    m_zipPathName(zipPath),
    m_zipFileName(zipName)
{
    if (!m_is)
        O3D_ERROR(E_InvalidPrecondition("Stream must be valid"));

	// read all headers
	while(readHeader()) {}
}

Zip::~Zip()
{
	destroy();
}

/*---------------------------------------------------------------------------------------
  release all data
---------------------------------------------------------------------------------------*/
void Zip::destroy()
{
	for (IT_ZipTokenVector it = m_filelist.begin(); it != m_filelist.end(); ++it)
	{
		deletePtr(*it);
	}

	m_filelist.clear();
	m_fileMap.clear();

    deletePtr(m_is);
}

/*---------------------------------------------------------------------------------------
  read the header and return true it's not the last
---------------------------------------------------------------------------------------*/
Bool Zip::readHeader()
{
	ZipToken *entry;
	Char buffer[MAX_PATH];

	entry = new ZipToken;

	// Lecture du header
    if (m_is->reader(&entry->FileHeader, sizeof(ZipHeader), 1))
	{
		#ifdef O3D_BIG_ENDIAN
			// Convert
			O3DZipHeaderToBIG(entry->FileHeader)
		#endif
		if (entry->FileHeader.Sig != O3D_ZIP_SIG)
		{
			deletePtr(entry);
			return False;
		}

		// Lecture du nom
        m_is->read(buffer, entry->FileHeader.FileNameLength);
		buffer[entry->FileHeader.FileNameLength] = '\0';

		entry->FileName = buffer;

		// is it a directory or a file
		if (entry->FileName.endsWith("/"))
		{
			entry->FileName.trimRight('/');
			entry->FileType = FILE_DIR;
		}
		else
			entry->FileType = FILE_FILE;

		// On saute les extras fields
        m_is->seek(entry->FileHeader.ExtraFieldLength);

		// On saute le Data descriptor si besoin est
		if (entry->FileHeader.BitFlag & O3D_ZIP_DATA_DESCRIPTOR)
		{
            m_is->reader(&entry->FileHeader.DataDescriptor, sizeof(ZipDataDescriptor), 1);
			#ifdef O3D_BIG_ENDIAN
			O3DZipDataDescriptorToBIG(entry->FileHeader.DataDescriptor)
			#endif
		}

		// On a tout lu on recupere la position
        entry->FilePos = m_is->getPosition();

		// On saute les donnees
        m_is->seek(entry->FileHeader.DataDescriptor.CompressedSize);

		m_filelist.push_back(entry);

		// to file map
		m_fileMap[m_zipPathName + '/' + entry->FileName] = (Int32)(m_filelist.size() - 1);

		return True;
	}
	else
	{
		deletePtr(entry);
        O3D_ERROR(E_InvalidFormat(getZipFullFileName()));
	}
}

// return a file index in the list (-1 if not found)
Int32 Zip::findFile(const String &filename) const
{
	String absFilename = FileManager::instance()->getFullFileName(filename);

	// on compare le nom du fichier (chemin absolu) au nom contenu dans le fichier zip ( zip path name + file name)
	CIT_ZipFileMap it = m_fileMap.find(absFilename);
	if (it == m_fileMap.end())
		return -1;
	else
		return it->second;
}

// get the file name at this index
String Zip::getFileName(Int32 index) const
{
	if ((index >= 0) && (index < (Int32)m_filelist.size()))
		return (m_zipPathName + '/' + m_filelist[index]->FileName);
	else
		O3D_ERROR(E_IndexOutOfRange(""));
}

// get the file type at this index
FileTypes Zip::getFileType(Int32 index) const
{
	if ((index >= 0) && (index < (Int32)m_filelist.size()))
		return m_filelist[index]->FileType;
	else
		O3D_ERROR(E_IndexOutOfRange(""));
}

InStream *Zip::openInStream(const String &filename)
{
    Int32 filePos = findFile(filename);

    if (filePos != -1)
        return openInStream(filePos);
    else
        O3D_ERROR(E_FileNotFoundOrInvalidRights("", filename));
}

InStream *Zip::openInStream(Int32 index)
{
    InStream *lfile;
    ZipToken *entry;

    if ((index < 0) || (index >= (Int32)m_filelist.size()))
        O3D_ERROR(E_IndexOutOfRange(""));

    entry = m_filelist[index];

    if (entry)
    {
        // On decompresse le fichier et on le retourne
        switch(entry->FileHeader.CompressionMethod)
        {
        case 0:
        {
            // Pas de compression on copie les donnees
            m_is->reset((Int32)entry->FilePos);

            // On creer un nouveau fichier memory
            SmartArrayUInt8 data(entry->FileHeader.DataDescriptor.UnCompressedSize);

            m_is->read(data.getData(), entry->FileHeader.DataDescriptor.UnCompressedSize);
            lfile = new SharedDataInStream(data);
        }
            break;

        case 8:
        {
            // Compression Deflated
            m_is->reset((Int32)entry->FilePos);

            UInt8 *CompressedData;	// Data compressees
            UInt32 CompressedSize;
            UInt32 UnCompressedSize;

            // Taille des donnees
            CompressedSize = entry->FileHeader.DataDescriptor.CompressedSize;
            UnCompressedSize = entry->FileHeader.DataDescriptor.UnCompressedSize;

            CompressedData = new UInt8[CompressedSize];
            SmartArrayUInt8 data(UnCompressedSize);

            // Lecture des donnees compresssees
            m_is->read(CompressedData, CompressedSize);

            // Decompression en utilisant Zlib
            z_stream Stream;
            UInt32 Erreur;

            Stream.next_in = (Bytef*)CompressedData;
            Stream.avail_in = (UInt32)CompressedSize;
            Stream.next_out = (Bytef*)data.getData();
            Stream.avail_out = (UInt32)UnCompressedSize;
            Stream.zalloc = (alloc_func)NULL;
            Stream.zfree = (free_func)NULL;
            Stream.opaque = NULL;

            // Decompresse les donnees. wbits < 0 indique que l'on a pas de header dans les donnees en entree
            Erreur = inflateInit2(&Stream,-MAX_WBITS);
            if (Erreur == Z_OK)
            {
                Erreur = inflate(&Stream,Z_FINISH);
                inflateEnd(&Stream);
                if (Erreur == Z_STREAM_END)
                    Erreur = Z_OK;

                Erreur = Z_OK;
                inflateEnd(&Stream);
            }

            deleteArray(CompressedData);

            // fichier
            lfile = new SharedDataInStream(data);
        }
            break;

        default:
            // Compression non supporte
            O3D_ERROR(E_InvalidFormat("Unsupported compression in " + getZipFullFileName()));
        }

        return lfile;
    }
    else
    {
        // Index en dehors de la plage de fichiers
        O3D_ERROR(E_IndexOutOfRange(""));
    }
}


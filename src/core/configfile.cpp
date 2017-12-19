/**
 * @file configfile.cpp
 * @brief Simple config file parser.
 * @author Emmanuel Ruffio
 * @date 2007-10-07
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#include "o3d/core/precompiled.h"
#include "o3d/core/configfile.h"

#include "o3d/core/stringtokenizer.h"
#include "o3d/core/filemanager.h"
#include "o3d/core/fileinfo.h"

#include <sstream>
#include <set>

using namespace o3d;

ConfigFile::ConfigFile():
	m_fileName(),
	m_root(),
	m_currentGroup()
{
}

ConfigFile::ConfigFile(const String & _filename):
	m_fileName(),
	m_root(),
	m_currentGroup()
{
	m_currentGroup.push_back(&m_root);

	if (!_filename.isEmpty())
	{
        InStream *lis = nullptr;

		try
		{
            lis = FileManager::instance()->openInStream(_filename);
            readFromFile(*lis);
		}
		catch(const E_FileNotFoundOrInvalidRights &)
		{
			// If the file does not exist, there is nothing special to do.
		}

        deletePtr(lis);
	}

	m_fileName = _filename;
}

ConfigFile::ConfigFile(const ConfigFile & _which):
	m_fileName(_which.m_fileName),
	m_root(_which.m_root),
	m_currentGroup()
{
	O3D_ASSERT(_which.m_currentGroup.size() > 0);

	ConfigFileGroup * lGroup = &m_root;

	m_currentGroup.push_back(lGroup);

	for (CIT_GroupArray it = _which.m_currentGroup.begin() ; it != _which.m_currentGroup.end() ; it++)
	{
		lGroup = lGroup->getGroup((*it)->getName());

		m_currentGroup.push_back(lGroup);
	}
}

ConfigFile::~ConfigFile()
{
	destroy();
}

ConfigFile & ConfigFile::operator = (const ConfigFile & _which)
{
	if (this == &_which)
		return *this;

	destroy();

	m_fileName = _which.m_fileName;
	m_root = _which.m_root;

	O3D_ASSERT(_which.m_currentGroup.size() > 0);

	ConfigFileGroup * lGroup = &m_root;
	m_currentGroup.push_back(lGroup);

	for (CIT_GroupArray it = _which.m_currentGroup.begin() ; it != _which.m_currentGroup.end() ; it++)
	{
		lGroup = lGroup->getGroup((*it)->getName());

		m_currentGroup.push_back(lGroup);
	}

	return *this;
}

void ConfigFile::destroy()
{
	m_fileName.destroy();
	m_root.destroy();

	m_currentGroup.clear();
	m_currentGroup.push_back(&m_root);
}

const String  ConfigFile::getGroup() const
{
	if (m_currentGroup.size() <= 1)
		return String();
	else
		return m_currentGroup.back()->getName();
}

const String ConfigFile::getFullPath() const
{
	if (m_currentGroup.size() <= 1)
		return String("/");
	else
	{
		String lFullPath("/");

		for (CIT_GroupArray it = m_currentGroup.begin() + 1 ; it != m_currentGroup.end() ; it++)
		{
			lFullPath << (*it)->getName();
			lFullPath << String("/");
		}

		return lFullPath;
	}
}

T_StringList ConfigFile::getGroups() const
{
	T_StringList list;
	for (ConfigFileGroup *group : m_currentGroup.back()->getGroups())
	{
		list.push_back(group->getName());
	}

	return list;
}

#define SET_VALUE                                                      \
	String lPath(_path);                                               \
	lPath.replace('\\', '/');                                          \
	lPath.trimLeftChars(String(" /"));                                 \
	lPath.trimRightChars(String(" /"));                                \
                                                                       \
    Int32 pos = _path.reverseFind('/');                                \
	if (pos != -1)                                                     \
	{                                                                  \
		lPath.truncate(pos);                                           \
                                                                       \
		ConfigFileGroup * lCurrent = m_currentGroup.back();            \
		T_GroupArray lGroupPath;                                       \
                                                                       \
		StringTokenizer lTokenizer(lPath, L"/");                       \
                                                                       \
		while (lTokenizer.hasMoreElements())                           \
		{                                                              \
			String lElt = lTokenizer.nextElement();                    \
            lElt.trimLeft(' ', True);                                  \
            lElt.trimRight(' ', True);                                 \
                                                                       \
			if (!lElt.isEmpty())                                       \
			{                                                          \
				ConfigFileGroup * lNext = lCurrent->getGroup(lElt);    \
                                                                       \
                if (lNext == nullptr)                                  \
                    return False;                                      \
				else                                                   \
				{                                                      \
					lCurrent = lNext;                                  \
					lGroupPath.push_back(lCurrent);                    \
				}                                                      \
			}                                                          \
		}                                                              \
                                                                       \
		if (lGroupPath.size())                                         \
			return lGroupPath.back()->setKey(                          \
                _path.sub(pos+1),                                      \
				ConfigFileValue(_value));                              \
		else                                                           \
            return False;                                              \
	}                                                                  \
	else

Bool ConfigFile::setBool(const String & _path, Bool _value)
{
	SET_VALUE
	return m_currentGroup.back()->setKey(_path, ConfigFileValue(_value));
}

Bool ConfigFile::setInt(const String & _path, Int32 _value)
{
	SET_VALUE
	return m_currentGroup.back()->setKey(_path, ConfigFileValue(_value));
}

Bool ConfigFile::setDouble(const String & _path, Double _value)
{
	SET_VALUE
	return m_currentGroup.back()->setKey(_path, ConfigFileValue(_value));
}

Bool ConfigFile::setString(const String & _path, const String & _value)
{
	SET_VALUE
	return m_currentGroup.back()->setKey(_path, ConfigFileValue(_value));
}

#undef SET_VALUE

#define GET_VALUE(FUNC)                                                \
	String lPath(_path);                                               \
	lPath.replace('\\', '/');                                          \
	lPath.trimLeftChars(String(" /"));                                 \
	lPath.trimRightChars(String(" /"));                                \
                                                                       \
    Int32 pos = _path.reverseFind('/');                                \
	if (pos != -1)                                                     \
	{                                                                  \
		lPath.truncate(pos);                                           \
                                                                       \
		ConfigFileGroup * lCurrent = m_currentGroup.back();            \
		T_GroupArray lGroupPath;                                       \
                                                                       \
		StringTokenizer lTokenizer(lPath, L"/");                       \
                                                                       \
		while (lTokenizer.hasMoreElements())                           \
		{                                                              \
			String lElt = lTokenizer.nextElement();                    \
            lElt.trimLeft(' ', True);                                  \
            lElt.trimRight(' ', True);                                 \
                                                                       \
			if (!lElt.isEmpty())                                       \
			{                                                          \
				ConfigFileGroup * lNext = lCurrent->getGroup(lElt);    \
                                                                       \
                if (lNext == nullptr)                                  \
				{                                                      \
                    if (_ok != nullptr)                                \
                        *_ok = False;                                  \
                                                                       \
					return _default;                                   \
				}                                                      \
				else                                                   \
				{                                                      \
					lCurrent = lNext;                                  \
					lGroupPath.push_back(lCurrent);                    \
				}                                                      \
			}                                                          \
		}                                                              \
                                                                       \
		if (lGroupPath.size() && lGroupPath.back()->getKey(            \
            _path.sub(pos+1),                                          \
			lValue))                                                   \
				return lValue.FUNC(_default, _ok);                     \
		else                                                           \
		{                                                              \
            if (_ok != nullptr)                                        \
                *_ok = False;                                          \
                                                                       \
			return _default;                                           \
		}                                                              \
	}                                                                  \
	else

Bool ConfigFile::getBool(const String & _path, Bool _default, Bool * _ok) const
{
	ConfigFileValue lValue;

	GET_VALUE(toBool)
	{
		if (m_currentGroup.back()->getKey(_path, lValue))
			return lValue.toBool(_default, _ok);
		else
		{
			if (_ok != NULL)
				*_ok = False;

			return _default;
		}
	}
}

Int32 ConfigFile::getInt(const String & _path, Int32 _default, Bool * _ok) const
{
	ConfigFileValue lValue;

	GET_VALUE(toInt)
	{
		if (m_currentGroup.back()->getKey(_path, lValue))
			return lValue.toInt(_default, _ok);
		else
		{
			if (_ok != NULL)
				*_ok = False;

			return _default;
		}
	}
}

Double ConfigFile::getDouble(const String & _path, Double _default, Bool * _ok) const
{
	ConfigFileValue lValue;

	GET_VALUE(toDouble)
	{
		if (m_currentGroup.back()->getKey(_path, lValue))
			return lValue.toDouble(_default, _ok);
		else
		{
			if (_ok != NULL)
				*_ok = False;

			return _default;
		}
	}
}

const String ConfigFile::getString(const String & _path, const String & _default, Bool * _ok) const
{
	ConfigFileValue lValue;

	GET_VALUE(toString)
	{
		if (m_currentGroup.back()->getKey(_path, lValue))
			return lValue.toString(_default, _ok);
		else
		{
			if (_ok != NULL)
				*_ok = False;

			return _default;
		}
	}
}

#undef GET_VALUE

Bool ConfigFile::findKey(const String & _key) const
{
	return m_currentGroup.back()->findKey(_key);
}

Bool ConfigFile::findKey(const String & _key, std::vector<String> & _array) const
{
	return m_currentGroup.back()->findKey(_key, _array);
}

Bool ConfigFile::removeKey(const String & _key)
{
	return m_currentGroup.back()->removeKey(_key);
}

Bool ConfigFile::insertGroup(const String & _group)
{
	return (m_currentGroup.back()->createGroup(_group) != NULL);
}

Bool ConfigFile::removeGroup(const String & _group)
{
	return (m_currentGroup.back()->removeGroup(_group));
}

Bool ConfigFile::findGroup(const String & _group)
{
	return (m_currentGroup.back()->getGroup(_group) != NULL);
}

Bool ConfigFile::enter(const String & _path)
{
	String lPath(_path);
	lPath.replace('\\', '/');
	lPath.trimLeftChars(String(" /"));
	lPath.trimRightChars(String(" /"));

	ConfigFileGroup * lCurrent = m_currentGroup.back();
	T_GroupArray lGroupPath;

	StringTokenizer lTokenizer(lPath, L"/");

	while (lTokenizer.hasMoreElements())
	{
		String lElt = lTokenizer.nextElement();
		lElt.trimLeft(' ', True);
		lElt.trimRight(' ', True);

		if (!lElt.isEmpty())
		{
			ConfigFileGroup * lNext = lCurrent->getGroup(lElt);

			if (lNext == NULL)
				return False;
			else
			{
				lCurrent = lNext;
				lGroupPath.push_back(lCurrent);
			}
		}
	}

	m_currentGroup.insert(m_currentGroup.end(), lGroupPath.begin(), lGroupPath.end());

	return True;
}

void ConfigFile::leave(UInt32 _depth)
{
    Int32 lNewSize = Int32(m_currentGroup.size() - 1) - Int32(_depth);

	if (lNewSize <= 0)
	{
		m_currentGroup.clear();
		m_currentGroup.push_back(&m_root);
	}
	else
		m_currentGroup.resize(lNewSize + 1);
}

void ConfigFile::toRoot()
{
	m_currentGroup.clear();
	m_currentGroup.push_back(&m_root);
}

Bool ConfigFile::save(const String & _filePath)
{
	String lFilePath;

    if (_filePath.isEmpty()) {
		lFilePath = m_fileName;
    } else {
		lFilePath = _filePath;
    }

//	FileInfo lFileInfo(lFilePath);
//	if (lFileInfo.exists() && !lFileInfo.isWritable()) {
//		return False;
//    }

    OutStream *los = (OutStream*)FileManager::instance()->openOutStream(lFilePath, FileOutStream::CREATE);
    Bool lRet = writeToFile(*los);

    deletePtr(los);

    if (lRet) {
		m_fileName = lFilePath;
		return True;
    } else {
		return False;
    }
}

Bool ConfigFile::writeToFile(OutStream &os)
{
    return m_root.writeToFile(os);
}

Bool ConfigFile::readFromFile(InStream &is)
{
	destroy();

    if (m_root.readFromFile(is)) {
		return True;
    } else {
		destroy();
		return False;
	}
}

/*---------------------------------------------------------------------------------------
  Class ConfigFileGroup
---------------------------------------------------------------------------------------*/
ConfigFile::ConfigFileGroup::ConfigFileGroup(const String & _name):
	m_name(_name),
	m_keys(),
	m_groups(),
	m_structure()
{
}

ConfigFile::ConfigFileGroup::ConfigFileGroup(const ConfigFileGroup & _which):
	m_name(_which.m_name),
	m_keys(_which.m_keys),
	m_groups(),
	m_structure(_which.m_structure)
{
	m_groups.reserve(_which.m_groups.size());

	for (CIT_GroupArray it = _which.m_groups.begin() ; it != _which.m_groups.end() ; it++)
		m_groups.push_back(new ConfigFileGroup(*(*it)));
}

ConfigFile::ConfigFileGroup::~ConfigFileGroup()
{
	destroy();
}

ConfigFile::ConfigFileGroup & ConfigFile::ConfigFileGroup::operator = (const ConfigFileGroup & _which)
{
	if (this == &_which)
		return *this;

	destroy();

	m_name = _which.m_name;
	m_keys = _which.m_keys;

	m_groups.reserve(_which.m_groups.size());

	for (CIT_GroupArray it = _which.m_groups.begin() ; it != _which.m_groups.end() ; it++)
		m_groups.push_back(new ConfigFileGroup(*(*it)));

	return *this;
}

void ConfigFile::ConfigFileGroup::destroy()
{
	m_name.destroy();
	m_keys.clear();

	for (IT_GroupArray it = m_groups.begin() ; it != m_groups.end() ; it++)
		deletePtr(*it);

	m_groups.clear();
	m_structure.clear();
}

/* Accessors */
void ConfigFile::ConfigFileGroup::setName(const String & _name)
{
	m_name = _name;
}

Bool ConfigFile::ConfigFileGroup::removeKey(const String & _keyName)
{
	String lKeyName(_keyName);
	lKeyName.trimLeftChars(String(" \t\\/"));
	lKeyName.trimRightChars(String(" \t"));

	for (IT_KeyMap it = m_keys.begin() ; it != m_keys.end() ; it++)
		if (lKeyName.compare(it->first, String::CASE_INSENSITIVE) == 0)
		{
			m_keys.erase(it);
			return True;
		}

	return False;
}

Bool ConfigFile::ConfigFileGroup::setKey(const String & _key, const ConfigFileValue & _value)
{
	String lKeyName(_key);
	lKeyName.trimLeftChars(String(" \\/"));
	lKeyName.trimRightChars(String(" \\/"));

	ConfigFileValue * lKeyValue = findLocalKey(lKeyName);

	if (lKeyValue != NULL)
		*lKeyValue = _value;
	else if (!lKeyName.isEmpty())
		m_keys[lKeyName] = _value;
	else
		return False;

	return True;
}

Bool ConfigFile::ConfigFileGroup::getKey(const String & _key, ConfigFileValue & _value) const
{
	String lKeyName(_key);
	lKeyName.trimLeftChars(String(" \\/"));
	lKeyName.trimRightChars(String(" \\/"));

	const ConfigFileValue * lKeyValue = findLocalKey(lKeyName);

	if (lKeyValue != NULL)
		_value = *lKeyValue;

	return (lKeyValue != NULL);
}

Bool ConfigFile::ConfigFileGroup::findKey(const String & _key) const
{
	String lKeyName(_key);
	lKeyName.trimLeftChars(String(" \\/"));
	lKeyName.trimRightChars(String(" \\/"));

	return (findLocalKey(lKeyName) != NULL);
}

Bool ConfigFile::ConfigFileGroup::findKey(const String & _key, std::vector<String> & _array) const
{
	String lCurrentPath("");

	String lKeyName(_key);
	lKeyName.trimLeftChars(String(" \\/"));
	lKeyName.trimRightChars(String(" \\/"));

	_array.clear();
	recFindKey(lKeyName, lCurrentPath, _array);

	return (!_array.empty());
}

ConfigFile::ConfigFileGroup * ConfigFile::ConfigFileGroup::createGroup(const String & _groupName)
{
	String lPath(_groupName);
	lPath.replace('\\', '/');
	lPath.trimLeftChars(String(" /"));

    Int32 lFirstBrace = lPath.find('/');

	if (lFirstBrace == -1)
	{
		ConfigFileGroup * lGroup = findLocalGroup(lPath);

		if (lGroup == NULL)
		{
			m_groups.push_back(new ConfigFileGroup(lPath));
			return m_groups.back();
		}
		else
			return lGroup;
	}
	else
	{
		String lGroup = lPath;
		lGroup.truncate(lFirstBrace);

		ConfigFileGroup * lGroupPtr = findLocalGroup(lGroup);

		if (lGroupPtr != NULL)
			return lGroupPtr->createGroup(lPath.sub(lFirstBrace));
		else
			return NULL;
	}
}

ConfigFile::ConfigFileGroup * ConfigFile::ConfigFileGroup::getGroup(const String & _fullpath)
{
	String lFullPath(_fullpath);
	O3D_ASSERT(lFullPath.find('\\') == -1);

	// On récupère le premier membre
	lFullPath.trimLeftChars(String(" /"));

	if (lFullPath.isEmpty())
		return this;

    Int32 lFirstBrace = lFullPath.find('/');
	String lGroup;

	// s'il ne reste plus qu'un terme
	if (lFirstBrace == -1)
		lGroup = lFullPath;
	else
	{
		lGroup = lFullPath;
		lGroup.truncate(lFirstBrace);
	}

	for (CIT_GroupArray it = m_groups.begin() ; it != m_groups.end() ; it++)
	{
		if (lGroup.compare((*it)->getName(), String::CASE_INSENSITIVE) == 0)
			return (*it)->getGroup(lFullPath.sub(lGroup.length()));
	}

	return NULL;
}

Bool ConfigFile::ConfigFileGroup::removeGroup(const String & _fullpath)
{
	// On récupère le groupe possédant le dernier élement du chemin
	String lFullPath(_fullpath);
	lFullPath.replace('\\', '/');
	lFullPath.trimRightChars(String(" /"));

    Int32 lLastBrace = lFullPath.reverseFind('/');

	ConfigFileGroup * lGroupPtr = NULL;
	String lGroupName = lFullPath;

	if (lLastBrace == -1) // Alors le groupe est un sous groupe du group courant
		lGroupPtr = this;
	else
	{
		lGroupName = lGroupName.sub(lLastBrace);
		lFullPath.truncate(lLastBrace);

		lGroupPtr = getGroup(lFullPath);
	}

	if (lGroupPtr != NULL)
	{
		for (IT_GroupArray it = lGroupPtr->m_groups.begin() ; it != lGroupPtr->m_groups.end() ; it++)
			if (lGroupName.compare((*it)->getName(), String::CASE_INSENSITIVE) == 0)
			{
				deletePtr(*it);
				m_groups.erase(it);
				return True;
			}
	}

	return False;
}

Bool ConfigFile::ConfigFileGroup::writeToFile(OutStream &_os)
{
	// Avant d'écrire les données dans le fichier, on met a jour sa structure

	// On supprime toutes les clefs qui ont disparu
	IT_FileStructure it = m_structure.begin();

	std::set<String> lKeySet;
	std::set<String> lGroupSet;

	while (it != m_structure.end())
	{
		if (it->lineType == PARSER_LINE_KEY)
		{
			IT_KeyMap itKey = m_keys.find(it->lineData);

			if (itKey == m_keys.end())
			{
				it = m_structure.erase(it);
				continue;
			}
			else
				lKeySet.insert(it->lineData);
		}
		else if (it->lineType == PARSER_LINE_GROUP_BEGIN)
		{
			if (findLocalGroup(it->lineData))
				lGroupSet.insert(it->lineData);
			else
			{
				it = m_structure.erase(it);
				continue;
			}
		}

		it++;
	}

	// On ajoute toutes les nouvelles clefs
	for (IT_KeyMap it = m_keys.begin() ; it != m_keys.end() ; it++)
	{
		std::set<String>::iterator itKey = lKeySet.find(it->first);

		if (itKey == lKeySet.end())
		{
			ParserLine lLine;
			lLine.lineType = PARSER_LINE_KEY;
			lLine.lineData = it->first;

			m_structure.push_back(lLine);
		}
	}

	// Pareil pour les groupes
	for (IT_GroupArray it = m_groups.begin() ; it != m_groups.end() ; it++)
	{
		std::set<String>::iterator itGroup = lGroupSet.find((*it)->getName());

		if (itGroup == lGroupSet.end())
		{
			ParserLine lLine;
			lLine.lineType = PARSER_LINE_GROUP_BEGIN;
			lLine.linePreData = String("[");
			lLine.lineData = (*it)->getName();
			lLine.linePostData = String("]");

			m_structure.push_back(lLine);

			lLine.lineType = PARSER_LINE_GROUP_END;
			lLine.linePreData = String("[/");
			lLine.lineData = (*it)->getName();
			lLine.linePostData = String("]");

			m_structure.push_back(lLine);
		}
	}

	// On écrit maintenant dans le fichier
	String lValueStr;

	for (IT_FileStructure it = m_structure.begin() ; it != m_structure.end() ; it++)
	{
		switch (it->lineType)
		{
		case PARSER_LINE_TEXT:
            _os.writeLine(it->lineData);
			break;
		case PARSER_LINE_GROUP_BEGIN:
			{
                _os.writeLine(it->linePreData + it->lineData + it->linePostData);

                Bool ret = findLocalGroup(it->lineData)->writeToFile(_os);
				O3D_ASSERT(ret);
			}
			break;
		case PARSER_LINE_GROUP_END:
            _os.writeLine(it->linePreData + it->lineData + it->linePostData);
			break;
		case PARSER_LINE_KEY:
			findLocalKey(it->lineData)->writeToString(lValueStr);
            _os.writeLine(it->linePreData + it->lineData + String(" = ") + lValueStr + it->linePostData);
			break;
		default:
			O3D_ASSERT(0);
			break;
		}
	}

	return True;
}

Bool ConfigFile::ConfigFileGroup::readFromFile(InStream &_is)
{
	ParserLine lLastLine;

    return parseFile(_is, lLastLine);
}

Bool ConfigFile::ConfigFileGroup::parseFile(InStream & _is, ParserLine & _lastLine)
{
	String lLine;
	ParserLine lLineData;
    Bool lError = False;

    while (_is.readLine(lLine) != -1)
	{
		String lLineCpy(lLine);
		lLineCpy.trimLeftChars(String(" \t"));

		// Cas du commentaire ou de la ligne vide
		if (lLineCpy.isEmpty() || (lLineCpy.getData()[0] == '#'))
		{
			lLineData.lineType = PARSER_LINE_TEXT;
			lLineData.lineData = lLine;

			m_structure.push_back(lLineData);
			continue;
		}

		// Cas d'un nouveau groupe ou de la fin du groupe courant
		if (lLineCpy.getData()[0] == '[')
		{
            Int32 lFirstBrace = lLine.find('[');
            Int32 lEndBrace = lLine.find(']');

			if (lEndBrace != -1)
			{
				// Il faut déterminer si il s'agit d'un début ou de la fin du groupe courant
				String lGroupBlock = lLine.sub(lFirstBrace+1, lEndBrace);
                Int32 lGroupBlockLeftChar = lGroupBlock.length();
				lGroupBlock.trimLeftChars(String(" \t"));
				lGroupBlockLeftChar -= lGroupBlock.length(); // Contient le nombre d'espace ou tabulation a gauche du nom de groupe

				lGroupBlock.trimRightChars(String(" \t"));

				if (lGroupBlock.isEmpty())
				{
					lError = True;
					break;
				}
				// Fin du groupe courant
				else if (lGroupBlock.getData()[0] == '/')
				{
                    Int32 lFirstCharIndex = lFirstBrace + 1 + lGroupBlockLeftChar + 1;
                    Int32 lLastCharIndex = lFirstCharIndex - 1 + lGroupBlock.length() - 1;

					String lGroupName = lGroupBlock.sub(1);
					lGroupName.trimLeftChars(String(" \t"));

					if (lGroupName.compare(getName(), String::CASE_INSENSITIVE) != 0)
					{
						lError = True;
						break;
					}
					else
					{
						_lastLine.lineType = PARSER_LINE_GROUP_END;
						_lastLine.linePreData = lLine.sub(0, lFirstCharIndex);
						_lastLine.lineData = lGroupName;
						_lastLine.linePostData = lLine.sub(lLastCharIndex + 1);

						return True;
					}
				}
				// Début d'un groupe
				else
				{
					String lGroupName = lGroupBlock;
                    Int32 lFirstCharIndex = lFirstBrace + 1 + lGroupBlockLeftChar;
                    Int32 lLastCharIndex = lFirstCharIndex + lGroupName.length() - 1;

					lLineData.lineType = PARSER_LINE_GROUP_BEGIN;
					lLineData.linePreData = lLine.sub(0, lFirstCharIndex);
					lLineData.lineData = lGroupName;
					lLineData.linePostData = lLine.sub(lLastCharIndex + 1);
					m_structure.push_back(lLineData);

					m_groups.push_back(new ConfigFileGroup(lGroupName));

                    if (!m_groups.back()->parseFile(_is, lLineData) || (lLineData.lineType == PARSER_LINE_UNDEFINED))
					{
						lError = True;
						break;
					}
					else
						m_structure.push_back(lLineData);
				}
			}
			else // C'est une erreur critique donc on retourne False
			{
				lError = True;
				break;
			}

			continue;
		}

		// Cas d'une clef
        Int32 lOperatorIndex = lLineCpy.find('=');

		if (lOperatorIndex != -1)
		{
            Int32 lOffset = lLine.find(lLineCpy.getData()[0]);

			String lKeyName = lLineCpy;
			lKeyName.truncate(lOperatorIndex);
			lKeyName.trimRightChars(String(" \t"));

			if (lKeyName.isEmpty())
			{
				lError = True;
				break;
			}

			String lValueStr = lLineCpy.sub(lOperatorIndex+1);
            Int32 lValueStrFirstChar = lValueStr.length();
            Int32 lValueStrLastChar = 0;

			lValueStr.trimLeftChars(String(" \t"));

			lValueStrFirstChar = lOffset + lOperatorIndex + 1 + lValueStrFirstChar - lValueStr.length();

            Int32 lRightSpaceSize = lValueStr.length();
			lValueStr.trimRightChars(String(" \t"));
			lRightSpaceSize -= lValueStr.length();

			// On regarde s'il existe un commentaire à la suite de la valeur
			if (!lValueStr.isEmpty())
			{
				if (lValueStr.getData()[0] == '\"')
				{
					// Si le dernier caractère n'est pas '"' alors un commentaire est présent
					if (lValueStr.getData()[lValueStr.length()-1] != '\"')
					{
                        Int32 lLastQuoteIndex = lValueStr.reverseFind('\"');

						if (lLastQuoteIndex <= 0)
						{
							lLineData.lineType = PARSER_LINE_TEXT;
							lLineData.lineData = lLine;

							m_structure.push_back(lLineData);
							continue;
						}
						else
						{
							String lComment = lValueStr.sub(lLastQuoteIndex+1);
							lComment.trimLeftChars(String(" \t"));

							if (lComment.getData()[0] != '#')
							{
								lLineData.lineType = PARSER_LINE_TEXT;
								lLineData.lineData = lLine;

								m_structure.push_back(lLineData);
								continue;
							}

							lValueStrLastChar = lLine.reverseFind('\"');
						}
					}
					else
						lValueStrLastChar = lOffset + lLineCpy.length() - 1 - lRightSpaceSize;
				}
				else
				{
                    Int32 lSharpIndex = lValueStr.find('#');

					if (lSharpIndex != -1)
					{
						String lBeforeSharp(lValueStr);
						lBeforeSharp.truncate(lSharpIndex);
                        Int32 lSpace = lBeforeSharp.length();
						lBeforeSharp.trimRightChars(String(" \t"));
						lSpace -= lBeforeSharp.length();

						lValueStrLastChar = lOffset + lOperatorIndex + 1 + lSharpIndex - lSpace;
					}
					else
						lValueStrLastChar = lOffset + lLineCpy.length() - 1 - lRightSpaceSize;
				}
			}
			else
				lValueStrLastChar = lValueStrFirstChar;

			if (lKeyName.isEmpty() || lValueStr.isEmpty())
			{
				lLineData.lineType = PARSER_LINE_TEXT;
				lLineData.lineData = lLine;
				m_structure.push_back(lLineData);
			}
			else
			{
				lLineData.lineType = PARSER_LINE_KEY;
				lLineData.linePreData = lLine.sub(0, lOffset);
				lLineData.lineData = lKeyName;
				lLineData.linePostData = lLine.sub(lValueStrLastChar+1);

				m_structure.push_back(lLineData);

				IT_KeyMap it = m_keys.find(lKeyName);

				if (it == m_keys.end())
				{
					ConfigFileValue lValue;
					lValue.readFromString(lLine.sub(lValueStrFirstChar, lValueStrLastChar+1));
					m_keys[lKeyName] = lValue;
				}
			}

			continue;
		}
		else // Line incorrecte, mais l'erreur est gérable.
		{
			lLineData.lineType = PARSER_LINE_TEXT;
			lLineData.lineData = lLine;
			m_structure.push_back(lLineData);

			continue;
		}
	}

	if (lError)
		destroy();

	_lastLine.lineType = PARSER_LINE_UNDEFINED;
	_lastLine.linePreData.destroy();
	_lastLine.lineData.destroy();
	_lastLine.linePostData.destroy();

	return !lError;
}

ConfigFile::ConfigFileGroup * ConfigFile::ConfigFileGroup::findLocalGroup(const String & _group)
{
	for (IT_GroupArray it = m_groups.begin() ; it != m_groups.end() ; it++)
		if (_group.compare((*it)->getName(), String::CASE_INSENSITIVE) == 0)
			return (*it);

	return NULL;
}

const ConfigFile::ConfigFileGroup * ConfigFile::ConfigFileGroup::findLocalGroup(const String & _group) const
{
	for (CIT_GroupArray it = m_groups.begin() ; it != m_groups.end() ; it++)
		if (_group.compare((*it)->getName(), String::CASE_INSENSITIVE) == 0)
			return (*it);

	return NULL;
}

ConfigFile::ConfigFileValue * ConfigFile::ConfigFileGroup::findLocalKey(const String & _key)
{
	for (IT_KeyMap it = m_keys.begin() ; it != m_keys.end() ; it++)
		if (_key.compare(it->first, String::CASE_INSENSITIVE) == 0)
			return &it->second;

	return NULL;
}

const ConfigFile::ConfigFileValue * ConfigFile::ConfigFileGroup::findLocalKey(const String & _key) const
{
	for (CIT_KeyMap it = m_keys.begin() ; it != m_keys.end() ; it++)
		if (_key.compare(it->first, String::CASE_INSENSITIVE) == 0)
			return &it->second;

	return NULL;
}

void ConfigFile::ConfigFileGroup::recFindKey(
	const String & _keyName,
	const String & _currentPath,
	std::vector<String> & _array) const
{
	String lCurrentPath(_currentPath);

	const ConfigFileValue * lValuePtr = findLocalKey(_keyName);

	if (lValuePtr != NULL)
		_array.push_back(lCurrentPath);

	for (CIT_GroupArray it = m_groups.begin() ; it != m_groups.end() ; it++)
	{
		(*it)->recFindKey(_keyName, lCurrentPath + (*it)->getName() + String("/"), _array);
	}
}


/*---------------------------------------------------------------------------------------
  Class ConfigFileGroup
---------------------------------------------------------------------------------------*/

const Char * const ConfigFile::ConfigFileValue::m_enumString[] = {
	"CONFIG_FILE_KEY_UNDEFINED",
	"CONFIG_FILE_KEY_BOOL",
	"CONFIG_FILE_KEY_INT",
	"CONFIG_FILE_KEY_DOUBLE",
	"CONFIG_FILE_KEY_STRING" };

ConfigFile::ConfigFileValue::ConfigFileValue():
	m_pData(NULL),
	m_type(CONFIG_FILE_KEY_UNDEFINED)
{
}

ConfigFile::ConfigFileValue::ConfigFileValue(Bool _value):
    m_pData(new Bool(_value)),
	m_type(CONFIG_FILE_KEY_BOOL)
{
}

ConfigFile::ConfigFileValue::ConfigFileValue(Int32 _value):
    m_pData(new Int32(_value)),
	m_type(CONFIG_FILE_KEY_INT)
{
}

ConfigFile::ConfigFileValue::ConfigFileValue(Double _value):
	m_pData(new Double(_value)),
	m_type(CONFIG_FILE_KEY_DOUBLE)
{
}


ConfigFile::ConfigFileValue::ConfigFileValue(const String & _value):
	m_pData(new String(_value)),
	m_type(CONFIG_FILE_KEY_STRING)
{
}


ConfigFile::ConfigFileValue::ConfigFileValue(const ConfigFileValue & _which):
	m_pData(NULL),
	m_type(_which.m_type)
{
	switch (m_type)
	{
	case CONFIG_FILE_KEY_UNDEFINED:
		break;
	case CONFIG_FILE_KEY_BOOL:
        m_pData = new Bool(*(Bool*)_which.m_pData);
		break;
	case CONFIG_FILE_KEY_INT:
        m_pData = new Int32(*(Int32*)_which.m_pData);
		break;
	case CONFIG_FILE_KEY_DOUBLE:
		m_pData = new Double(*(Double*)_which.m_pData);
		break;
	case CONFIG_FILE_KEY_STRING:
		m_pData = new String(*(String*)_which.m_pData);
		break;
	}
}

ConfigFile::ConfigFileValue::~ConfigFileValue()
{
	destroy();
}

ConfigFile::ConfigFileValue & ConfigFile::ConfigFileValue::operator = (const ConfigFileValue & _which)
{
	if (this == &_which)
		return *this;

	destroy();

	m_type = _which.m_type;

	switch (m_type)
	{
	case CONFIG_FILE_KEY_UNDEFINED:
		break;
	case CONFIG_FILE_KEY_BOOL:
        m_pData = new Bool(*(Bool*)_which.m_pData);
		break;
	case CONFIG_FILE_KEY_INT:
        m_pData = new Int32(*(Int32*)_which.m_pData);
		break;
	case CONFIG_FILE_KEY_DOUBLE:
		m_pData = new Double(*(Double*)_which.m_pData);
		break;
	case CONFIG_FILE_KEY_STRING:
		m_pData = new String(*(String*)_which.m_pData);
		break;
	}

	return *this;
}

void ConfigFile::ConfigFileValue::destroy()
{
	switch (m_type)
	{
	case CONFIG_FILE_KEY_UNDEFINED:
		break;
	case CONFIG_FILE_KEY_BOOL:
		{
            Bool * lPtr = (Bool*)m_pData;
			deletePtr(lPtr);
		}
		break;
	case CONFIG_FILE_KEY_INT:
		{
            Int32 * lPtr = (Int32*)m_pData;
			deletePtr(lPtr);
		}
		break;
	case CONFIG_FILE_KEY_DOUBLE:
		{
			Double * lPtr = (Double*)m_pData;
			deletePtr(lPtr);
		}
		break;
	case CONFIG_FILE_KEY_STRING:
		{
			String * lPtr = (String*)m_pData;
			deletePtr(lPtr);
		}
		break;
	}

	m_type = CONFIG_FILE_KEY_UNDEFINED;
	m_pData = NULL;
}

Bool ConfigFile::ConfigFileValue::toBool(Bool _default, Bool * _ok) const
{
	if (_ok != NULL)
		*_ok = (m_type == CONFIG_FILE_KEY_BOOL);

	if (m_type != CONFIG_FILE_KEY_BOOL)
		return _default;
	else
        return *(Bool*)m_pData;
}

Int32 ConfigFile::ConfigFileValue::toInt(Int32 _default, Bool * _ok) const
{
	if (_ok != NULL)
		*_ok = (m_type == CONFIG_FILE_KEY_INT);

	if (m_type != CONFIG_FILE_KEY_INT)
		return _default;
	else
        return *(Int32*)m_pData;
}

Double ConfigFile::ConfigFileValue::toDouble(Double _default, Bool * _ok) const
{
	if (_ok != NULL)
		*_ok = (m_type == CONFIG_FILE_KEY_DOUBLE);

	if (m_type != CONFIG_FILE_KEY_DOUBLE)
		return _default;
	else
		return *(Double*)m_pData;
}

const String ConfigFile::ConfigFileValue::toString(const String & _default, Bool * _ok) const
{
	if (_ok != NULL)
		*_ok = (m_type == CONFIG_FILE_KEY_STRING);

	if (m_type != CONFIG_FILE_KEY_STRING)
		return _default;
	else
		return *(String*)m_pData;
}

void ConfigFile::ConfigFileValue::setBool(Bool _value)
{
	if (m_type != CONFIG_FILE_KEY_BOOL)
	{
		destroy();

		m_type = CONFIG_FILE_KEY_BOOL;
        m_pData = new Bool(_value);
	}
	else
        *(Bool*)m_pData = _value;
}

void ConfigFile::ConfigFileValue::setInt(Int32 _value)
{
	if (m_type != CONFIG_FILE_KEY_INT)
	{
		destroy();

		m_type = CONFIG_FILE_KEY_INT;
        m_pData = new Int32(_value);
	}
	else
        *(Int32*)m_pData = _value;
}

void ConfigFile::ConfigFileValue::setDouble(Double _value)
{
	if (m_type != CONFIG_FILE_KEY_DOUBLE)
	{
		destroy();

		m_type = CONFIG_FILE_KEY_DOUBLE;
		m_pData = new Double(_value);
	}
	else
		*(Double*)m_pData = _value;
}

void ConfigFile::ConfigFileValue::setString(const String & _value)
{
	if (m_type != CONFIG_FILE_KEY_STRING)
	{
		destroy();

		m_type = CONFIG_FILE_KEY_STRING;
		m_pData = new String(_value);
	}
	else
		*(String*)m_pData = _value;
}

Bool ConfigFile::ConfigFileValue::writeToString(String & _string)
{
	_string.destroy();

	switch(m_type)
	{
	case CONFIG_FILE_KEY_UNDEFINED:
		break;
	case CONFIG_FILE_KEY_BOOL:
        _string << ((*(Bool*)m_pData) ? String("true") : String("false"));
		break;
	case CONFIG_FILE_KEY_INT:
        _string << *(Int32*)m_pData;
		break;
	case CONFIG_FILE_KEY_DOUBLE:
		_string << *(Double*)m_pData;
		break;
	case CONFIG_FILE_KEY_STRING:
		_string << String("\"") << *(String*)m_pData << String("\"");
		break;
	default:
		_string.destroy();
		break;
	}

	return True;
}

Bool ConfigFile::ConfigFileValue::readFromString(const String & _string)
{
	destroy();

	String lValue(_string);
	lValue.trimLeft(' ', True);
	lValue.trimRight(' ', True);

	if (lValue.isEmpty())
		return False;

	// Test de chaine
	if ((lValue.length() > 1) && (lValue.getData()[0] == '\"') && (lValue.getData()[lValue.length()-1] == '\"'))
	{
		lValue.trimRight('\"');
		lValue.trimLeft('\"');

		setString(lValue);
		return True;
	}

	// Test d'entier
	{
		std::wistringstream iss(lValue.getData());
        Int32 lValue;

		if ((iss >> lValue) && (iss.eof()))
		{
			setInt(lValue);
			return True;
		}
	}

	// Test de double
	{
		std::wistringstream iss(lValue.getData());
		Double lValue;

		if ((iss >> lValue) && (iss.eof()))
		{
			setDouble(lValue);
			return True;
		}
	}

	// Test de booléen
	if (lValue.compare(String("true"), String::CASE_INSENSITIVE) == 0)
	{
		setBool(True);
		return True;
	}
	else if (lValue.compare(String("false"), String::CASE_INSENSITIVE) == 0)
	{
		setBool(False);
		return True;
	}

	return False;
}

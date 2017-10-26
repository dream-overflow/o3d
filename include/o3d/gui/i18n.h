/**
 * @file i18n.h
 * @brief 
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2001-12-25
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_I18N_H
#define _O3D_I18N_H

#include "o3d/core/stringmap.h"
#include "o3d/core/stringlist.h"

namespace o3d {

/**
 * @brief Internationalization manager.
 * @author Frederic SCHERMA (frederic.scherma@gmail.com)
 * @date 2013-08-23
 * The languages files are simple text files with key = value.
 * Each pair key/value are key = string, with some string can contains {} as
 * a variable parameter @see get methods. Trailing space around = are trimmed.
 */
class O3D_API I18n
{
public:

	//! Some supported languages.
	enum Languages
	{
		LANG_EN_US = 0,
		LANG_EN_CA = 0,
		LANG_EN_GB = 0,
		LANG_FR_FR = 1,
		LANG_FR_CA = 1,
		LANG_ES_ES = 2,
		LANG_ES_MX = 2,
		LANG_ES_AR = 2,
		LANG_ES_CO = 2,
		LANG_IT_IT = 3,
		LANG_GR_GR = 4,
		LANG_RU_RU = 5
	};

	/**
	 * @brief I18n
	 */
	I18n();

	~I18n();

	//! Set the active language.
	void setLang(Languages lang);

	//! Get the active language.
	Languages getLang() const { return m_currentLang; }

	//! Add a language folder. The folder must contains one file per language,
	//! like en_US.lang, fr_FR.lang.
	//! The folder must remains accessible during the live of the usange of the Gui,
	//! in the case you want to change the language dynamicaly. Because calling a change
	//! of language will do a full reload of the lang files.
	void addFolder(const String &path);

    //! Get a single text.
	String get(const String &key) const;

	//! Get a text having one parameter specified by a {} in the langage text.
	String get(const String &key, const String &_1) const;

    /**
     * Get a text having one integer parameter, replace the first {} by the value of
     * the integer, and replace any {xxx} where xxx is a string suffix by xxx if the
     * absolute integer value is greater than 1, else it ignore all {xxx} chunks.
     * An example: You have gained {} point{s}!
     * If The integer value is greater than 1 you have 'points' else 'point'.
     */
    String get(const String &key, Int32 _1) const;

	//! Get a text having two parameters specified by a {} in the langage text.
	String get(const String &key, const String &_1, const String &_2) const;

	//! Get a text having three parameters specified by a {} in the langage text.
	String get(const String &key, const String &_1, const String &_2, const String &_3) const;

private:

	//! DEFAULT_LANG set to US english
	static const Languages DEFAULT_LANG = LANG_EN_US;

	Languages m_currentLang;

	//! List of the full path containing the languages files currently loaded.
	T_StringList m_folders;

	//! Map containing the pair key/string.
	StringMap<String> m_strings;

	void loadLang(const String &path, Languages lang);

	void findKey(const String &key);
};

} // namespace o3d

#endif // _O3D_I18N_H


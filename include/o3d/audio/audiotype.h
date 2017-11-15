/**
 * @file audiotype.h
 * @brief Enumerations for audio.
 * @author Frederic SCHERMA (frederic.scherma@dreamoverflow.org)
 * @date 2012-12-15
 * @copyright Copyright (c) 2001-2017 Dream Overflow. All rights reserved.
 * @details 
 */

#ifndef _O3D_AUDIOTYPE_H
#define _O3D_AUDIOTYPE_H

namespace o3d {

enum AudioObjectType
{
	AUDIO_AUDIO = 0x30000000,         //!< audio manager

	AUDIO_SOUND,                      //!< base sound object
	AUDIO_SOUND_WAV,                  //!< WAV sound format.
	AUDIO_SOUND_OGG,                  //!< OGG sound format
	AUDIO_SOUND_MP3,                  //!< MP3 sound format
	AUDIO_SOUND_RAW,                  //!< RAW sound format

	AUDIO_SND_BUFFER_LIST = 0x30010000,   //!< sound buffer manager
	AUDIO_SND_BUFFER,                     //!< sound buffer
	AUDIO_SND_STREAM,

	AUDIO_SND_LISTENER = 0x30020000,  //!< audio listener base object

	AUDIO_SND_SOURCE_LIST = 0x30030000,   //!< audio source manager
	AUDIO_SND_SOURCE,                 //!< audio source base object
	AUDIO_SND_SOURCE_OMNI,            //!< omnidirectional audio source
	AUDIO_SND_SOURCE_DIRECT,          //!< Directional audio source

	AUDIO_SND_BUFFER_OBJECT = 0x30040000,  //!< audio sound buffer object
	AUDIO_AL_BUFFER,                   //!< OpenAL audio single sound buffer
	AUDIO_SND_STREAM_BUFFER            //!< OpenAL audio stream sound buffer
};

} // namespace o3d

#endif // _O3D_AUDIOTYPE_H


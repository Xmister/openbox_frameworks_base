/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_MEDIAPLAYER_H
#define ANDROID_MEDIAPLAYER_H

#include <binder/IMemory.h>
#include <media/IMediaPlayerClient.h>
#include <media/IMediaPlayer.h>
#include <media/IMediaDeathNotifier.h>
#include <media/IStreamSource.h>

#include <utils/KeyedVector.h>
#include <utils/String8.h>
#include "mediaplayerinfo.h"

class ANativeWindow;

namespace android {

class Surface;
class ISurfaceTexture;

#define DLNA_SOURCE_DETECTOR "com.allwinner.dlnasourcedetector"
enum media_event_type {
    MEDIA_NOP               = 0, // interface test message
    MEDIA_PREPARED          = 1,
    MEDIA_PLAYBACK_COMPLETE = 2,
    MEDIA_BUFFERING_UPDATE  = 3,
    MEDIA_SEEK_COMPLETE     = 4,
    MEDIA_SET_VIDEO_SIZE    = 5,
    MEDIA_TIMED_TEXT        = 99,
    MEDIA_ERROR             = 100,
    MEDIA_INFO              = 200,
    MEDIA_SOURCE_DETECTED	= 234,		//add by Bevis, for Dlna source detector
    MEDIA_INFO_SRC_3D_MODE  = 1024,
};

// Generic error codes for the media player framework.  Errors are fatal, the
// playback must abort.
//
// Errors are communicated back to the client using the
// MediaPlayerListener::notify method defined below.
// In this situation, 'notify' is invoked with the following:
//   'msg' is set to MEDIA_ERROR.
//   'ext1' should be a value from the enum media_error_type.
//   'ext2' contains an implementation dependant error code to provide
//          more details. Should default to 0 when not used.
//
// The codes are distributed as follow:
//   0xx: Reserved
//   1xx: Android Player errors. Something went wrong inside the MediaPlayer.
//   2xx: Media errors (e.g Codec not supported). There is a problem with the
//        media itself.
//   3xx: Runtime errors. Some extraordinary condition arose making the playback
//        impossible.
//
enum media_error_type {
    // 0xx
    MEDIA_ERROR_UNKNOWN = 1,
    // 1xx
    MEDIA_ERROR_SERVER_DIED = 100,
    // 2xx
    MEDIA_ERROR_NOT_VALID_FOR_PROGRESSIVE_PLAYBACK = 200,
    // 3xx
};


// Info and warning codes for the media player framework.  These are non fatal,
// the playback is going on but there might be some user visible issues.
//
// Info and warning messages are communicated back to the client using the
// MediaPlayerListener::notify method defined below.  In this situation,
// 'notify' is invoked with the following:
//   'msg' is set to MEDIA_INFO.
//   'ext1' should be a value from the enum media_info_type.
//   'ext2' contains an implementation dependant info code to provide
//          more details. Should default to 0 when not used.
//
// The codes are distributed as follow:
//   0xx: Reserved
//   7xx: Android Player info/warning (e.g player lagging behind.)
//   8xx: Media info/warning (e.g media badly interleaved.)
//
enum media_info_type {
    // 0xx
    MEDIA_INFO_UNKNOWN = 1,
    // 7xx
    // The video is too complex for the decoder: it can't decode frames fast
    // enough. Possibly only the audio plays fine at this stage.
    MEDIA_INFO_VIDEO_TRACK_LAGGING = 700,
    // MediaPlayer is temporarily pausing playback internally in order to
    // buffer more data.
    MEDIA_INFO_BUFFERING_START = 701,
    // MediaPlayer is resuming playback after filling buffers.
    MEDIA_INFO_BUFFERING_END = 702,
    // Bandwidth in recent past
    MEDIA_INFO_NETWORK_BANDWIDTH = 703,

    // 8xx
    // Bad interleaving means that a media has been improperly interleaved or not
    // interleaved at all, e.g has all the video samples first then all the audio
    // ones. Video is playing but a lot of disk seek may be happening.
    MEDIA_INFO_BAD_INTERLEAVING = 800,
    // The media is not seekable (e.g live stream).
    MEDIA_INFO_NOT_SEEKABLE = 801,
    // New media metadata is available.
    MEDIA_INFO_METADATA_UPDATE = 802,
};


/* add by Gary. start {{----------------------------------- */
/**
*  screen name
*/
#define MASTER_SCREEN        0
#define SLAVE_SCREEN         1
/* add by Gary. end   -----------------------------------}} */

enum media_player_states {
    MEDIA_PLAYER_STATE_ERROR        = 0,
    MEDIA_PLAYER_IDLE               = 1 << 0,
    MEDIA_PLAYER_INITIALIZED        = 1 << 1,
    MEDIA_PLAYER_PREPARING          = 1 << 2,
    MEDIA_PLAYER_PREPARED           = 1 << 3,
    MEDIA_PLAYER_STARTED            = 1 << 4,
    MEDIA_PLAYER_PAUSED             = 1 << 5,
    MEDIA_PLAYER_STOPPED            = 1 << 6,
    MEDIA_PLAYER_PLAYBACK_COMPLETE  = 1 << 7
};

// Keep KEY_PARAMETER_* in sync with MediaPlayer.java.
// The same enum space is used for both set and get, in case there are future keys that
// can be both set and get.  But as of now, all parameters are either set only or get only.
enum media_parameter_keys {
    KEY_PARAMETER_TIMED_TEXT_TRACK_INDEX = 1000,                // set only
    KEY_PARAMETER_TIMED_TEXT_ADD_OUT_OF_BAND_SOURCE = 1001,     // set only

    // Streaming/buffering parameters
    KEY_PARAMETER_CACHE_STAT_COLLECT_FREQ_MS = 1100,            // set only

    // Return a Parcel containing a single int, which is the channel count of the
    // audio track, or zero for error (e.g. no audio track) or unknown.
    KEY_PARAMETER_AUDIO_CHANNEL_COUNT = 1200,                   // get only

};

// ----------------------------------------------------------------------------
// ref-counted object for callbacks
class MediaPlayerListener: virtual public RefBase
{
public:
    virtual void notify(int msg, int ext1, int ext2, const Parcel *obj) = 0;
    /* add by Gary. start {{----------------------------------- */
    /* 2011-10-9 8:54:30 */
    /* add callback for parsing 3d source */
    virtual int  parse3dFile(int type) = 0;
    /* add by Gary. end   -----------------------------------}} */
};

class MediaPlayer : public BnMediaPlayerClient,
                    public virtual IMediaDeathNotifier
{
public:
    MediaPlayer();
    ~MediaPlayer();
            void            died();
            void            disconnect();

            status_t        setDataSource(
                    const char *url,
                    const KeyedVector<String8, String8> *headers);

            status_t        setDataSource(int fd, int64_t offset, int64_t length);
            status_t        setDataSource(const sp<IStreamSource> &source);
            status_t        setVideoSurfaceTexture(
                                    const sp<ISurfaceTexture>& surfaceTexture);
            status_t        setListener(const sp<MediaPlayerListener>& listener);
            status_t        prepare();
            status_t        prepareAsync();
            status_t        start();
            status_t        stop();
            status_t        pause();
            bool            isPlaying();
            status_t        getVideoWidth(int *w);
            status_t        getVideoHeight(int *h);
            status_t        seekTo(int msec);
            status_t        getCurrentPosition(int *msec);
            status_t        getDuration(int *msec);
            status_t        reset();
            status_t        setAudioStreamType(int type);
            status_t        setLooping(int loop);
            bool            isLooping();
            status_t        setVolume(float leftVolume, float rightVolume);
            void            notify(int msg, int ext1, int ext2, const Parcel *obj = NULL);
    static  sp<IMemory>     decode(const char* url, uint32_t *pSampleRate, int* pNumChannels, int* pFormat);
    static  sp<IMemory>     decode(int fd, int64_t offset, int64_t length, uint32_t *pSampleRate, int* pNumChannels, int* pFormat);
            status_t        invoke(const Parcel& request, Parcel *reply);
            status_t        setMetadataFilter(const Parcel& filter);
            status_t        getMetadata(bool update_only, bool apply_filter, Parcel *metadata);
            status_t        setAudioSessionId(int sessionId);
            int             getAudioSessionId();
            status_t        setAuxEffectSendLevel(float level);
            status_t        attachAuxEffect(int effectId);
            status_t        setParameter(int key, const Parcel& request);
            status_t        getParameter(int key, Parcel* reply);
    /* add by Gary. start {{----------------------------------- */
    static  status_t        setScreen(int screen);
    static  status_t        getScreen(int *screen);
    static  status_t        isPlayingVideo(bool *playing);
    /* add by Gary. end   -----------------------------------}} */
    
    /* add by Gary. start {{----------------------------------- */
    /* 2011-9-13 14:05:05 */
    /* expend interfaces about subtitle, track and so on */
            int             getSubCount();
            int             getSubList(MediaPlayer_SubInfo *infoList, int count);
            int             getCurSub();
            status_t        switchSub(int index);
            status_t        setSubGate(bool showSub);
            bool            getSubGate();
            status_t        setSubColor(int color);
            int             getSubColor();
            status_t        setSubFrameColor(int color);
            int             getSubFrameColor();
            status_t        setSubFontSize(int size);
            int             getSubFontSize();
            status_t        setSubCharset(const char *charset);
            status_t        getSubCharset(char *charset);
            status_t        setSubPosition(int percent);
            int             getSubPosition();
            status_t        setSubDelay(int time);
            int             getSubDelay();
            int             getTrackCount();
            int             getTrackList(MediaPlayer_TrackInfo *infoList, int count);
            int             getCurTrack();
            status_t        switchTrack(int index);
            status_t        setInputDimensionType(int type);
            int             getInputDimensionType();
            status_t        setOutputDimensionType(int type);
            int             getOutputDimensionType();
            status_t        setAnaglaghType(int type);
            int             getAnaglaghType();
            status_t        getVideoEncode(char *encode);
            int             getVideoFrameRate();
            status_t        getAudioEncode(char *encode);
            int             getAudioBitRate();
            int             getAudioSampleRate();
    /* add by Gary. end   -----------------------------------}} */
    /* add by Gary. start {{----------------------------------- */
    /* 2011-10-9 8:54:30 */
    /* add callback for parsing 3d source */
            int             parse3dFile(int type);
    /* add by Gary. end   -----------------------------------}} */

    /* add by Gary. start {{----------------------------------- */
    /* 2011-11-14 */
    /* support scale mode */
            status_t        enableScaleMode(bool enable, int width, int height);
    /* add by Gary. end   -----------------------------------}} */

    /* add by Gary. start {{----------------------------------- */
    /* 2011-11-14 */
    /* support adjusting colors while playing video */
    static  status_t        setVppGate(bool enableVpp);
    static  bool            getVppGate();
    static  status_t        setLumaSharp(int value);
    static  int             getLumaSharp();
    static  status_t        setChromaSharp(int value);
    static  int             getChromaSharp();
    static  status_t        setWhiteExtend(int value);
    static  int             getWhiteExtend();
    static  status_t        setBlackExtend(int value);
    static  int             getBlackExtend();
    /* add by Gary. end   -----------------------------------}} */

    /* add by Gary. start {{----------------------------------- */
    /* 2012-03-07 */
    /* set audio channel mute */
            status_t        setChannelMuteMode(int muteMode);
            int             getChannelMuteMode();
    /* add by Gary. end   -----------------------------------}} */

    /* add by Gary. start {{----------------------------------- */
    /* 2012-03-12 */
    /* add the global interfaces to control the subtitle gate  */
    static  status_t        setGlobalSubGate(bool showSub);
    static  bool            getGlobalSubGate();
    /* add by Gary. end   -----------------------------------}} */

    /* add by Gary. start {{----------------------------------- */
    /* 2012-4-24 */
    /* add two general interfaces for expansibility */
            status_t        generalInterface(int cmd, int int1, int int2, int int3, void *p);
    static  status_t        generalGlobalInterface(int cmd, int int1, int int2, int int3, void *p);
    /* add by Gary. end   -----------------------------------}} */

private:
            void            clear_l();
            status_t        seekTo_l(int msec);
            status_t        prepareAsync_l();
            status_t        getDuration_l(int *msec);
            status_t        attachNewPlayer(const sp<IMediaPlayer>& player);
            status_t        reset_l();

    sp<IMediaPlayer>            mPlayer;
    thread_id_t                 mLockThreadId;
    Mutex                       mLock;
    Mutex                       mNotifyLock;
    Condition                   mSignal;
    sp<MediaPlayerListener>     mListener;
    void*                       mCookie;
    media_player_states         mCurrentState;
    int                         mDuration;
    int                         mCurrentPosition;
    int                         mSeekPosition;
    bool                        mPrepareSync;
    status_t                    mPrepareStatus;
    int                         mStreamType;
    bool                        mLoop;
    float                       mLeftVolume;
    float                       mRightVolume;
    int                         mVideoWidth;
    int                         mVideoHeight;
    int                         mAudioSessionId;
    float                       mSendLevel;
    /* add by Gary. start {{----------------------------------- */
    /* 2011-9-28 16:28:24 */
    /* save properties before creating the real player */
    bool                        mSubGate;
    int                         mSubColor;
    int                         mSubFrameColor;
    int                         mSubPosition;
    int                         mSubDelay;
    int                         mSubFontSize;
    char                        mSubCharset[MEDIAPLAYER_NAME_LEN_MAX];
	int                         mSubIndex;
    int                         mTrackIndex;
    int                         mMuteMode;   // 2012-03-07, set audio channel mute
   /* add by Gary. end   -----------------------------------}} */
};

}; // namespace android

#endif // ANDROID_MEDIAPLAYER_H

#define CUTE_SOUND_IMPLEMENTATION
#include "cute_sound.h"

#define AUDIO_MASTER (1.0f)

enum AudioType {
    AUDIO_GUN_0,
    AUDIO_GUN_1,
    AUDIO_SHOTGUN,
    AUDIO_KNIFE,
    AUDIO_MISSILE,
    AUDIO_EXPLOSION,
    AUDIO_IMPACT,
    AUDIO_SCREAM,
    AUDIO_COUNT
};

static struct {
    cs_context_t*       context;
    cs_loaded_sound_t   loaded  [AUDIO_COUNT];
    cs_play_sound_def_t playing [AUDIO_COUNT];
} audio;

static void AudioInit(void) {
    HWND hwnd = platform.native;

    audio.context = cs_make_context(hwnd, 44100, 8192, 2048, NULL);

    {
        audio.loaded[AUDIO_GUN_0]  = cs_load_wav("sound/sfx_weapon_singleshot3.wav");
        audio.playing[AUDIO_GUN_0] = cs_make_def(&audio.loaded[AUDIO_GUN_0]);

        audio.playing[AUDIO_GUN_0].volume_left  = 0.5f * AUDIO_MASTER;
        audio.playing[AUDIO_GUN_0].volume_right = 0.5f * AUDIO_MASTER;
    }

    {
        audio.loaded[AUDIO_GUN_1]  = cs_load_wav("sound/sfx_weapon_singleshot4.wav");
        audio.playing[AUDIO_GUN_1] = cs_make_def(&audio.loaded[AUDIO_GUN_1]);

        audio.playing[AUDIO_GUN_1].volume_left  = 0.5f * AUDIO_MASTER;
        audio.playing[AUDIO_GUN_1].volume_right = 0.5f * AUDIO_MASTER;
    }

    {
        audio.loaded[AUDIO_SHOTGUN]  = cs_load_wav("sound/sfx_weapon_shotgun3.wav");
        audio.playing[AUDIO_SHOTGUN] = cs_make_def(&audio.loaded[AUDIO_SHOTGUN]);

        audio.playing[AUDIO_SHOTGUN].volume_left  = 0.5f * AUDIO_MASTER;
        audio.playing[AUDIO_SHOTGUN].volume_right = 0.5f * AUDIO_MASTER;
    }

    {
        audio.loaded[AUDIO_KNIFE]  = cs_load_wav("sound/sfx_wpn_punch3.wav");
        audio.playing[AUDIO_KNIFE] = cs_make_def(&audio.loaded[AUDIO_KNIFE]);

        audio.playing[AUDIO_KNIFE].volume_left  = 0.5f * AUDIO_MASTER;
        audio.playing[AUDIO_KNIFE].volume_right = 0.5f * AUDIO_MASTER;
    }

    {
        audio.loaded[AUDIO_MISSILE]  = cs_load_wav("sound/sfx_wpn_missilelaunch.wav");
        audio.playing[AUDIO_MISSILE] = cs_make_def(&audio.loaded[AUDIO_MISSILE]);

        audio.playing[AUDIO_MISSILE].volume_left  = 0.5f * AUDIO_MASTER;
        audio.playing[AUDIO_MISSILE].volume_right = 0.5f * AUDIO_MASTER;
    }

    {
        audio.loaded[AUDIO_EXPLOSION]  = cs_load_wav("sound/sfx_exp_short_hard2.wav");
        audio.playing[AUDIO_EXPLOSION] = cs_make_def(&audio.loaded[AUDIO_EXPLOSION]);

        audio.playing[AUDIO_EXPLOSION].volume_left  = 0.5f * AUDIO_MASTER;
        audio.playing[AUDIO_EXPLOSION].volume_right = 0.5f * AUDIO_MASTER;
    }

    {
        audio.loaded[AUDIO_IMPACT]  = cs_load_wav("sound/sfx_sounds_impact6.wav");
        audio.playing[AUDIO_IMPACT] = cs_make_def(&audio.loaded[AUDIO_IMPACT]);

        audio.playing[AUDIO_IMPACT].volume_left  = 0.3f * AUDIO_MASTER;
        audio.playing[AUDIO_IMPACT].volume_right = 0.3f * AUDIO_MASTER;
    }

    {
        audio.loaded[AUDIO_SCREAM]  = cs_load_wav("sound/sfx_deathscream_human12.wav");
        audio.playing[AUDIO_SCREAM] = cs_make_def(&audio.loaded[AUDIO_SCREAM]);

        audio.playing[AUDIO_SCREAM].volume_left  = 0.8f * AUDIO_MASTER;
        audio.playing[AUDIO_SCREAM].volume_right = 0.8f * AUDIO_MASTER;
    }
}

static void AudioPlay(int audio_type) {
    cs_play_sound(audio.context, audio.playing[audio_type]);
}

static void AudioPlayLooped(int audio_type) {
    cs_playing_sound_t* playing = cs_play_sound(audio.context, audio.playing[audio_type]);
    //
    playing->looped = 1;
}

static void AudioPlayFromSource(int audio_type, v2 listener, v2 source, f32 volume) {
    f32 dst = 1.0f - fClamp(0.02f * v2_Dist(listener, source), 0.0f, 1.0f);
    f32 pan = 0.5f * (fClamp(0.1f * (source.x - listener.x), -1.0f, 1.0f) + 1.0f);

    f32 final_volume = AUDIO_MASTER * volume * dst;

    cs_playing_sound_t* playing = cs_play_sound(audio.context, audio.playing[audio_type]);
    cs_set_pan(playing, pan);

    cs_set_volume(
            playing,
            audio.playing[audio_type].volume_left * final_volume,
            audio.playing[audio_type].volume_right * final_volume);
}

static void AudioUpdate(void) {
    cs_mix(audio.context);
}

static void AudioKillAll(void) {
    cs_stop_all_sounds(audio.context);
}


#include "game_utils.h"
#include "raylib.h"
#include "game_assets.h"

static audio_manager_t AudioMan;

void InitAudio(){
  AudioMan = (audio_manager_t){0};
   
  FilePathList audiofiles = LoadDirectoryFiles("resources/sfx");
  for(int i = 0; i < SFX_NONE; i++){
    AudioMan.sfx[i] = (sfx_group_t){0};
    AudioMan.sfx[i].num_sounds = 0;
    AudioMan.sfx[i].sounds = NULL;
    AudioMan.timers[i] = InitEvents();
  }
  AudioMan.sfx[SFX_ACTION].num_sounds = audiofiles.count;

  AudioMan.sfx[SFX_ACTION].sounds = calloc(audiofiles.count,sizeof(Sound));

  for (int f = 0; f < audiofiles.count; f++){
   AudioMan.sfx[SFX_ACTION].sounds[f] = LoadSound(audiofiles.paths[f]);
    TraceLog(LOG_INFO,"Found sfx file %s",audiofiles.paths[f]);

  }
}

void AudioPlayRandomSfx(SfxGroup group){
  if(!CheckEvent(AudioMan.timers[group],EVENT_PLAY_SFX)){
    int r = rand()%AudioMan.sfx[group].num_sounds;
    PlaySound(AudioMan.sfx[group].sounds[r]);

    int wait  = (int)((AudioMan.sfx[group].sounds[r].frameCount*60)/44100);
    AddEvent(AudioMan.timers[group],InitCooldown(wait,EVENT_PLAY_SFX,NULL,NULL));
    return;
  }



}

void AudioStep(){
  for(int g = 0; g<SFX_NONE;g++){
    StepEvents(AudioMan.timers[g]);
  }

}

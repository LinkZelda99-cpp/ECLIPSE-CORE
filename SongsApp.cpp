#include "SongsApp.h"
#include "SoluneApp.h"

struct SongNote {
  uint16_t frequency;
  uint16_t duration;
  uint32_t startMs;
};

static const char* songNames[] = {
  "SOLUNE",
  "HAVEN",
  "CANNON IN D",
  "FUR ELISE"
};

static const uint8_t SONG_COUNT = 4;
static int songIndex = 0;
static bool songPlaying = false;
static unsigned long songStarted = 0;
static uint16_t songNoteIndex = 0;
static bool songNoteStarted = false;

// HAVEN: extracted from the uploaded 72-second MIDI as a monophonic
// melody line. Low accompaniment notes are omitted so the buzzer
// stays musical instead of trying to reproduce full polyphony.
static const SongNote havenTheme[] = {
  // Extracted from the uploaded "Haven (Piano).mid" piano track.
  // The piano source is 93.7 seconds long; this is a monophonic
  // highest-note adaptation for the single passive buzzer.
  {262,94,0},{392,2051,94},{523,3156,2145},{392,238,5301},
  {659,406,6301},{784,1293,6707},{698,312,8000},{659,281,8312},
  {523,2438,8594},{494,1664,11031},{349,94,13852},{440,60,13945},
  {523,1945,14000},{698,469,15945},{1047,1625,16414},{880,824,18039},
  {698,730,18863},{659,2070,19594},{262,125,21988},{330,60,22113},
  {392,1574,22164},{523,168,23738},{659,344,23906},{784,1602,24250},
  {698,648,25852},{392,60,26500},{659,480,26508},{494,2824,26988},
  {392,2148,29945},{523,457,32094},{784,1188,32551},{392,60,33738},
  {698,539,33750},{659,711,34289},{523,488,35000},{587,594,35488},
  {392,60,36082},{494,1664,36094},{392,1625,38070},{784,305,39695},
  {698,238,40000},{392,60,40238},{523,1867,40289},{587,406,42156},
  {659,781,42562},{392,520,43344},{784,60,43863},{1047,2281,43914},
  {392,74,47602},{523,2168,47676},{784,445,49844},{523,60,50289},
  {659,1207,50301},{698,438,51508},{587,398,51945},{659,383,52344},
  {523,680,52914},{587,539,53688},{494,1156,54238},{330,113,55844},
  {392,2699,55957},{523,1445,58656},{659,293,60102},{784,1500,60395},
  {698,238,61895},{659,199,62133},{392,74,62332},{523,445,62406},
  {494,1688,62852},{262,60,64645},{330,60,64695},{392,2344,64750},
  {523,445,67094},{494,336,67539},{392,60,67875},{523,824,67926},
  {392,906,68750},{523,3582,69656},{392,1461,73883},{1047,301,75344},
  {392,105,75645},{988,520,75750},{880,1324,76270},{784,312,77594},
  {392,60,77906},{659,438,77957},{698,500,78395},{587,1543,78895},
  {330,70,80438},{392,2094,80508},{1047,156,82602},{392,281,82758},
  {1047,1094,83039},{392,60,84133},{784,438,84145},{392,113,84582},
  {784,367,84695},{659,1531,85062},{523,344,86594},{392,60,86938},
  {659,2043,86945},{523,4645,88988},{392,60,93633},{330,60,93645}
};

// Public-domain melody arrangements.
static const SongNote cannonTheme[] = {
  {294,500,0},{370,500,500},{440,500,1000},{494,500,1500},
  {523,500,2000},{494,500,2500},{440,500,3000},{370,500,3500},
  {330,500,4000},{370,500,4500},{440,500,5000},{494,500,5500},
  {440,500,6000},{370,500,6500},{330,500,7000},{294,1000,7500},
  {294,500,9000},{370,500,9500},{440,500,10000},{494,500,10500},
  {523,500,11000},{494,500,11500},{440,500,12000},{370,500,12500},
  {330,500,13000},{370,500,13500},{440,500,14000},{494,500,14500},
  {440,500,15000},{370,500,15500},{294,1000,16000}
};

static const SongNote furEliseTheme[] = {
  {659,250,0},{622,250,250},{659,250,500},{622,250,750},
  {659,250,1000},{494,250,1250},{587,250,1500},{523,250,1750},
  {440,500,2000},{0,250,2500},{262,250,2750},{330,250,3000},
  {440,250,3250},{494,500,3500},{0,250,4000},{330,250,4250},
  {415,250,4500},{494,250,4750},{523,500,5000},{0,250,5500},
  {330,250,5750},{659,250,6000},{622,250,6250},{659,250,6500},
  {622,250,6750},{659,250,7000},{494,250,7250},{587,250,7500},
  {523,250,7750},{440,500,8000},{0,250,8500},{262,250,8750},
  {330,250,9000},{440,250,9250},{494,500,9500},{0,250,10000},
  {330,250,10250},{523,250,10500},{494,250,10750},{440,500,11000}
};

static const SongNote* currentSongNotes(){
  if(songIndex==1)return havenTheme;
  if(songIndex==2)return cannonTheme;
  return furEliseTheme;
}

static uint16_t currentSongCount(){
  if(songIndex==1)return sizeof(havenTheme)/sizeof(havenTheme[0]);
  if(songIndex==2)return sizeof(cannonTheme)/sizeof(cannonTheme[0]);
  return sizeof(furEliseTheme)/sizeof(furEliseTheme[0]);
}

static void drawSongs(){
  String name=songNames[songIndex];

  if(name=="CANNON IN D")name="CANNON";
  if(name=="FUR ELISE")name="FUR ELISE";

  if(songPlaying){
    drawLCD("SONGS > "+name,"PLAYING");
  }else{
    drawLCD("SONGS > "+name,"PRESS = PLAY");
  }

  showEclipseLogo();
  rgbPurple();
}

static void startBuiltInSong(){
  songPlaying=true;
  songStarted=millis();
  songNoteIndex=0;
  songNoteStarted=false;
  buzzerOff();
  clearEncoderEvents();
}

void returnToSongsMenu(){
  buzzerOff();
  songPlaying=false;
  state=STATE_SONGS;
  invalidateLCD();
  lcd.clear();
  clearEncoderEvents();
}

void updateSongsApp(){
  if(!songPlaying){
    int delta=consumeEncoderDelta();

    if(delta>0){
      songIndex++;
      if(songIndex>=SONG_COUNT)songIndex=0;
      soundNavigate();
    }else if(delta<0){
      songIndex--;
      if(songIndex<0)songIndex=SONG_COUNT-1;
      soundNavigate();
    }

    drawSongs();

    if(consumeEncoderPress()){
      soundSelect();

      if(songIndex==0){
        startSolune();
        return;
      }

      startBuiltInSong();
    }

    return;
  }

  drawSongs();

  unsigned long elapsed=millis()-songStarted;
  const SongNote* notes=currentSongNotes();
  uint16_t count=currentSongCount();

  if(songNoteIndex>=count){
    buzzerOff();
    songStarted=millis();
    songNoteIndex=0;
    songNoteStarted=false;
    return;
  }

  const SongNote &note=notes[songNoteIndex];

  if(!songNoteStarted && elapsed>=note.startMs){
    if(note.frequency==0){
      buzzerOff();
    }else{
      playTone(note.frequency,note.duration);
    }

    songNoteStarted=true;
    songNoteIndex++;
  }

  if(songNoteIndex<count && elapsed>=notes[songNoteIndex].startMs){
    songNoteStarted=false;
  }

  if(consumeEncoderPress()){
    returnToSongsMenu();
  }
}

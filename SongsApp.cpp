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
  {659,500,0},{523,500,500},{587,500,1000},{659,500,1500},
  {880,500,2000},{1047,500,2500},{880,500,3000},{784,500,3500},
  {523,500,4000},{659,500,4500},{523,500,5000},{784,500,5500},
  {880,500,6000},{784,500,6500},{523,500,7000},{440,500,7500},
  {392,500,8000},{440,500,8500},{523,500,9000},{784,500,9500},
  {659,500,10000},{784,500,10500},{880,500,11000},{1047,500,11500},
  {1319,500,12000},{1047,500,12500},{1175,1000,13000},{1047,2000,14000},
  {659,500,16000},{523,500,16500},{587,500,17000},{659,500,17500},
  {880,500,18000},{1047,500,18500},{880,500,19000},{784,500,19500},
  {523,500,20000},{659,500,20500},{523,500,21000},{784,500,21500},
  {880,500,22000},{784,500,22500},{523,500,23000},{440,500,23500},
  {392,500,24000},{440,500,24500},{523,500,25000},{784,500,25500},
  {659,500,26000},{784,500,26500},{880,500,27000},{1047,500,27500},
  {523,2000,28000},{1047,500,28500},{1175,1000,29000},{1047,2000,30000},
  {1319,500,32000},{1047,500,32500},{1175,500,33000},{1319,500,33500},
  {1760,500,34000},{2093,500,34500},{1760,500,35000},{1568,500,35500},
  {1047,500,36000},{1319,500,36500},{1047,500,37000},{1568,500,37500},
  {1760,500,38000},{1568,500,38500},{1047,500,39000},{880,500,39500},
  {392,500,40000},{880,500,40500},{1047,500,41000},{1568,500,41500},
  {1319,500,42000},{1568,500,42500},{1760,500,43000},{2093,500,43500},
  {523,2000,44000},{2093,500,44500},{2349,1000,45000},{2093,2000,46000},
  {1319,500,48000},{1047,500,48500},{1175,500,49000},{1319,500,49500},
  {1760,500,50000},{2093,500,50500},{1760,500,51000},{1568,500,51500},
  {1047,500,52000},{1319,500,52500},{1047,500,53000},{1568,500,53500},
  {1760,500,54000},{1568,500,54500},{1047,500,55000},{880,500,55500},
  {392,500,56000},{880,500,56500},{1047,500,57000},{1568,500,57500},
  {1319,500,58000},{1568,500,58500},{1760,500,59000},{2093,500,59500},
  {523,2000,60000},{2093,500,60500},{2349,1000,61000},{2093,2000,62000},
  {523,500,64000},{587,500,64500},{659,500,65000},{784,500,65500},
  {880,500,66000},{988,500,66500},{1047,500,67000},{1175,500,67500},
  {1319,500,68000},{1397,500,68500},{1568,500,69000},{1760,500,69500},
  {3136,2000,70000}
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
      tone(PIN_BUZZER,note.frequency,note.duration);
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

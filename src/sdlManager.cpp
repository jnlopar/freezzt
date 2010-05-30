/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#include <SDL.h>
#include <string>
#include <algorithm>
#include "platform.h"

#include "debug.h"
#include "abstractPainter.h"
#include "textmodePainter.h"
#include "sdlEventLoop.h"
#include "abstractMusicStream.h"
#include "nullMusicStream.h"
#include "sdlMusicStream.h"
#include "dotFileParser.h"
#include "freezztManager.h"
#include "fileListModel.h"
#include "abstractPlatformServices.h"
#include "sdlManager.h"

// ---------------------------------------------------------------------------

inline int boundInt( const int left, const int value, const int right )
{
  return ( value < left ) ? left
       : ( value > right) ? right
                          : value;
}

// ---------------------------------------------------------------------------

class SDLPlatformServices : public AbstractPlatformServices
{
  public: 
    AbstractMusicStream *musicStream;
    FileListModel fileModel;

  public: 
    virtual AbstractMusicStream * acquireMusicStream() { return musicStream; };
    virtual AbstractMusicStream * currentMusicStream() { return musicStream; };
    virtual void releaseMusicStream( AbstractMusicStream * ) { /* */ };

    virtual AbstractScrollModel * acquireFileListModel( const std::string &directory = "" )
    {
      fileModel.setDirectory(directory);
      return &fileModel;
    };
    virtual AbstractScrollModel * currentFileListModel() { return &fileModel; };
    virtual void releaseFileListModel( AbstractScrollModel *model ) { /* */ };
};

// ---------------------------------------------------------------------------

class SDLManagerPrivate
{
  public:
    SDLManagerPrivate( SDLManager *pSelf );

    void parseArgs( int argc, char ** argv );
    void loadSettings();
    AbstractMusicStream *createMusicStream();

  public:
    FreeZZTManager *pFreezztManager;
    DotFileParser dotFile;
    TextmodePainter painter;
    SDL_Surface *display;
    int frameTime;
    bool ready;

  private:
    SDLManager *self;
};

SDLManagerPrivate::SDLManagerPrivate( SDLManager *pSelf )
  : pFreezztManager(0),
    display(0),
    frameTime(27),
    ready(false),
    self(pSelf)
{
  /* */
}

void SDLManagerPrivate::parseArgs( int argc, char ** argv )
{
  if (argc >= 2) {
    zinfo() << "Loading" << argv[1];
    pFreezztManager->loadWorld( argv[1] );
    ready = true;
  }
  else {
    zinfo() << "usage:" << argv[0] << "world.zzt";
  }
}

void SDLManagerPrivate::loadSettings()
{
  zinfo() << "Parsing dotfile";

  // load keys
  dotFile.load("freezztrc");

  // get frameTime
  frameTime = dotFile.getInt( "video.frame_time", 1, 27 );
  frameTime = boundInt( 1, frameTime, 1000 );
  zdebug() << "frameTime:" << frameTime;
}

AbstractMusicStream * SDLManagerPrivate::createMusicStream()
{
  bool audioEnabled = dotFile.getBool( "audio.enabled", 1, false );
  zdebug() << "audio.enabled:" << audioEnabled;

  if (!audioEnabled) {
    return new NullMusicStream();
  }

  SDLMusicStream *stream = new SDLMusicStream();
  stream->setSampleRate( dotFile.getInt( "audio.freq", 1, 22050 ) );
  stream->setBufferLength( dotFile.getInt( "audio.buffer", 1, 2048 ) );
  stream->setVolume( dotFile.getInt( "audio.volume", 1, 64 ) );

  std::string waveType = dotFile.getValue( "audio.wave", 1 );
  std::transform(waveType.begin(), waveType.end(), waveType.begin(), ::toupper);
  SDLMusicStream::WaveformType wave = SDLMusicStream::Square;
  if ( waveType == "SINE" ) wave = SDLMusicStream::Sine;
  else if ( waveType == "SQUARE" ) wave = SDLMusicStream::Square;
  else if ( waveType == "TRIANGLE" ) wave = SDLMusicStream::Triangle;
  else if ( waveType == "SAWTOOTH" ) wave = SDLMusicStream::Sawtooth;
  else if ( waveType == "TRISQUARE" ) wave = SDLMusicStream::TriSquare;
  stream->setWaveform( wave );
  
  stream->openAudio();
  return stream;
}

// -------------------------------------

SDLManager::SDLManager( int argc, char ** argv )
  : d( new SDLManagerPrivate(this) )
{
  d->pFreezztManager = new FreeZZTManager;
  d->parseArgs( argc, argv );
}

SDLManager::~SDLManager()
{
  delete d->pFreezztManager;
  d->pFreezztManager = 0;

  zinfo() << "Quitting SDL.";
  SDL_Quit();

  delete d;
  d = 0;
}

bool SDLManager::valid() const
{
  return d->ready;
}

void SDLManager::doResize( int w, int h )
{
  Uint32 surfaceFlags = SDL_RESIZABLE | SDL_SWSURFACE;
  d->display = SDL_SetVideoMode( w, h, 0, surfaceFlags );
  if (!d->display) {
    zerror() << "Could not create display:" << SDL_GetError();
  }
  SDL_FillRect( d->display, 0, 0 );
  d->painter.setSDLSurface( d->display );
}

void SDLManager::exec()
{
  d->loadSettings();

  // Initialize defaults, Video and Audio subsystems
  zinfo() << "Initializing SDL.";
  int ret = SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER);
  if(ret==-1) { 
    zerror() << "Could not initialize SDL:" << SDL_GetError();
    return;
  }

  zinfo() << "Creating display surface.";
  Uint32 surfaceFlags = SDL_RESIZABLE | SDL_SWSURFACE;
  d->display = SDL_SetVideoMode( 640, 400, 0, surfaceFlags );
  if (!d->display) {
    zerror() << "Could not create display:" << SDL_GetError();
    return;
  }
  SDL_FillRect( d->display, 0, 0 );

  SDL_WM_SetCaption("FreeZZT", "FreeZZT");
  SDL_EnableUNICODE(1);
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

  d->painter.setSDLSurface( d->display );

  SDLPlatformServices services;
  services.musicStream = d->createMusicStream();
  d->pFreezztManager->setServices( &services );

  d->pFreezztManager->setSpeed( d->dotFile.getInt( "speed", 1, 4 ) );

  zinfo() << "Entering event loop";
  SDLEventLoop eventLoop;
  eventLoop.setFrameLatency( d->frameTime );
  eventLoop.setPainter( &d->painter );
  eventLoop.setSDLManager( this );
  eventLoop.setZZTManager( d->pFreezztManager );
  eventLoop.exec();

  d->pFreezztManager->setServices( 0 );
  delete services.musicStream;
}


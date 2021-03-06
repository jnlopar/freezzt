/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef SDL_MANAGER_H
#define SDL_MANAGER_H

class SDLManagerPrivate;

class SDLManager
{
  public:
    SDLManager( int argc, char ** argv );
    ~SDLManager();

    bool valid() const;
    void exec();
    void doResize( int w, int h );
    void toggleFullScreen();

  private:
    SDLManagerPrivate *d;
};

#endif /* SDL_MANAGER_H */


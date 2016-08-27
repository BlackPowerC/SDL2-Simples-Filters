#ifndef MODIFICATION_H_INCLUDED
#define MODIFICATION_H_INCLUDED

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    #define RED     0xff000000
    #define GREEN   0x00ff0000
    #define BLUE    0x0000ff00
    #define ALPHA   0x000000ff

#else
    #define RED     0x000000ff
    #define GREEN   0x0000ff00
    #define BLUE    0x00ff0000
    #define ALPHA   0xff000000

#endif // SDL_BYTEORDER

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

    bool error(void *ptr) ;
    void LeftRotate(SDL_Surface *src) ;
    void UpRotate(SDL_Surface *src) ;
    void greyLevel(SDL_Surface *src) ;
    void inversion(SDL_Surface *src) ;
    void contraste(SDL_Surface *src, int _c) ;
    SDL_Surface *flou(SDL_Surface *src) ;
    void lumisote(SDL_Surface *src, int _grey) ;
    bool addMasque(SDL_Surface *src, SDL_Surface *masque) ;
    void RGB(SDL_Surface *src, int _r, int _g, int _b) ;
    void lumisoteMouse(SDL_Surface *src, int x, int y, int _grey) ;
    void RGB_Mouse(SDL_Surface *src, int x, int y, int _r, int _g, int _b) ;

#ifdef __cplusplus
}
#endif // __cplusplus


#endif // MODIFICATION_H_INCLUDED

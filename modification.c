#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <float.h>
#include <math.h>

static
Uint32 getPixel(SDL_Surface *surface, Uint32 x, Uint32 y)
{
    x = abs(x) ;
    y = abs(y) ;
    int BytesPerPixel = surface->format->BytesPerPixel ;
    Uint8 *ptr = surface->pixels + y*surface->pitch + x*BytesPerPixel ;

    switch(BytesPerPixel)
    {
    case 1 :
        return *ptr ;
        break ;
    case 2:
        return *(Uint16*) ptr ;
        break ;
    case 3:
    	if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
        	return ptr[0] << 16 | ptr[1] << 8 | ptr[2] ;
        else
            return ptr[0] | ptr[1] << 8 | ptr[2] << 16 ;
    case 4:
        return *(Uint32*)ptr ;
        break ;
    }
    return 0 ;
}

static
void setPixel(SDL_Surface *surface, Uint32 x, Uint32 y, Uint32 pixel)
{
    x = abs(x) ;
    y = abs(y) ;
    int BytesPerPixel = surface->format->BytesPerPixel ;
    Uint8 *ptr = surface->pixels + y*surface->pitch+ x*BytesPerPixel ;
    switch(BytesPerPixel)
    {
    case 1:
        *ptr = pixel ;
        break ;
    case 2:
        *(Uint16*)ptr = pixel ;
        break ;
    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
        {
            ptr[0] = (pixel >> 16) & 0xff;
            ptr[1] = (pixel >> 8) & 0xff;
            ptr[2] = pixel & 0xff;
        }
        else
        {
            ptr[0] = pixel & 0xff;
            ptr[1] = (pixel >> 8) & 0xff;
            ptr[2] = (pixel >> 16) & 0xff;
        }
        break;
    case 4:
        *(Uint32*)ptr =  pixel ;
        break ;
    }
}

static
Uint8 addVal(Uint8 cp, int val)
{
    int tmp = cp + val ;

    if(tmp > 255)
        tmp = 255 ;

    else if(tmp < 0)
        tmp = 0 ;

    return (Uint8) tmp ;
}

static void forint(SDL_Surface *src, int x, int y, int dx, int dy, int _r, int _g, int _b)
{
    Uint8 r = 0, g = 0, b = 0, a = 0 ;
    Uint32 color ;
    for(int i = x; i<dx; i++)
    {
        for(int j = y; j<dy; j++)
        {
            color = getPixel(src, j, i) ;
            SDL_GetRGBA(color, src->format, &r, &g, &b, &a) ;

            r = addVal(r, _r) ;
            b = addVal(b, _b) ;
            g = addVal(g, _g) ;
            color = SDL_MapRGBA(src->format, r, g, b, a) ;
            setPixel(src, j, i, color) ;
        }
    }
}

void RGB_Mouse(SDL_Surface *src, int x, int y, int _r, int _g, int _b)
{
    SDL_LockSurface(src) ;

    SDL_Rect carre  ;
    carre.x = x-2 ;
    carre.y = y-2 ;
    carre.h = y+2 ;
    carre.w = x+2 ;

    forint(src, carre.x, carre.y, carre.w, carre.h, _r, _g, _b) ;

    SDL_UnlockSurface(src) ;
}

void RGB(SDL_Surface *src, int _r, int _g, int _b)
{
    SDL_LockSurface(src) ;
    forint(src, 0, 0, src->h, src->w, _r, _g, _b) ;
    SDL_UnlockSurface(src) ;
}

void lumisote(SDL_Surface* src, int _grey)
{
    RGB(src, _grey, _grey, _grey) ;
}

void lumisoteMouse(SDL_Surface *src, int x, int y, int _grey)
{
    RGB_Mouse(src, x, y, _grey, _grey, _grey) ;
}

void greyLevel(SDL_Surface *src)
{
    Uint8 r, g, b ;
    Uint8 moy ;
    Uint32 pixel ;
    SDL_LockSurface(src) ;
    for(int i = 0; i<src->h; i++)
    {
        for(int j = 0; j<src->w; j++)
        {
            pixel = getPixel(src, j, i) ;
            SDL_GetRGB(pixel, src->format, &r, &g, &b) ;
            moy = (r+b+g)/3 ;

            pixel = SDL_MapRGB(src->format, moy, moy, moy) ;
            setPixel(src, j, i, pixel) ;
        }
    }
    SDL_UnlockSurface(src) ;
}

bool addMasque(SDL_Surface *src, SDL_Surface *masque)
{
    if(src->w != masque->w || src->h != masque->h)
    {
        printf("Impossible d'appliquer le masque") ;
        return false ;
    }

    Uint32 pixelSRC, pixelMasque ;
    Uint8 RGBA[3] = {0} ;
    Uint8 tmp[3] = {0} ;

    SDL_LockSurface(src) ;
    SDL_LockSurface(masque) ;

    for(int i = 0; i<= src->h; i++)
    {
        for(int j = 0; j<=src->w; j++)
        {
            pixelMasque = getPixel(masque, j, i) ;
            pixelSRC = getPixel(src, j, i) ;

            SDL_GetRGB(pixelMasque, masque->format, &RGBA[0], &RGBA[1], &RGBA[2]) ;
            SDL_GetRGB(pixelSRC, src->format, &tmp[0], &tmp[1], &tmp[2]) ;

            pixelSRC = SDL_MapRGBA(src->format, tmp[0], tmp[1], tmp[2], RGBA[0]) ;
            setPixel(src, j, i, pixelSRC) ;
        }
    }
    SDL_UnlockSurface(src) ;
    SDL_UnlockSurface(masque) ;
    return true ;
}

void inversion(SDL_Surface *src)
{
    Uint8 r, g, b ;
    Uint32 pixel ;
    SDL_LockSurface(src) ;
    for(int i = 0; i<src->h; i++)
    {
        for(int j = 0; j<src->w; j++)
        {
            pixel = getPixel(src, j, i) ;
            SDL_GetRGB(pixel, src->format, &r, &g, &b) ;
            r = 255-r ;
            g = 255-g ;
            b = 255-b ;
            pixel = SDL_MapRGB(src->format, r, g, b) ;
            setPixel(src, j, i, pixel) ;
        }
    }
    SDL_UnlockSurface(src) ;
}

static
void Rotate(SDL_Surface *src, int dx, int dy, int x, int y)
{
    Uint32 pixelA, pixelB ;
    SDL_LockSurface(src) ;
    for(int i = 0; i<dx; i++)
    {
        for(int j = 0; j<=dy; j++)
        {
            pixelB = getPixel(src, j, i) ;
            pixelA = getPixel(src, x-j, y-i) ;
            setPixel(src, j, i, pixelA) ;
            setPixel(src, x-j, y-i, pixelB) ;
        }
    }
    SDL_UnlockSurface(src) ;
}

void UpRotate(SDL_Surface *src)
{
/*  Uint32 pixelH, pixelB ;
    for(int i = 0; i<=src->h/2; i++)
    {
        for(int j = 0; j<src->w; j++)
        {
            pixelH = getPixel(src, j, i) ;
            pixelB = getPixel(src, j, src->h-i-1) ;
            setPixel(src, j, i, pixelB) ;
            setPixel(src, j, src->h-i-1 ,pixelH) ;
        }
    }*/
    Rotate(src, src->h/2, src->w, 0, src->h-1) ;
}

void LeftRotate(SDL_Surface *src)
{
/*  Uint32 pixelG, pixelD ;
    SDL_LockSurface(src) ;
    for(int i = 0; i<src->h; i++)
    {
        for(int j = 0; j<=src->w/2; j++)
        {
            pixelD = getPixel(src, j, i) ;
            pixelG = getPixel(src, src->w-j, i) ;
            setPixel(src, j, i, pixelG) ;
            setPixel(src, src->w-j, i, pixelD) ;
        }
    }
    SDL_UnlockSurface(src) ;*/
    Rotate(src, src->h, src->w/2, src->w, 0) ;
}

static
Uint32 convolution(Uint32 (*Matrix)[3], Uint32 (*kernel)[3])
{
    Uint32 res = 0 ;
    for(int i = 0; i<3; i++)
    {
        for(int j = 0; j<3; j++)
        {
            res += Matrix[i][j]*kernel[i][j] ;
        }
    }
   // printf("res %d\n", res) ;
    return res ;
}



SDL_Surface *flou(SDL_Surface *src)
{
    Uint32 NewPixel ;
    Uint8 RGB[3] = {0} ;
    int RGB_Final[3] = {0} ;

    Uint32 kernel[3][3] = {{0, 1, 0},
                          {0, 0, 0},
                          {0, 0, 0}} ;

    Uint32 MatIm[3][3] = {{0}, {0}, {0}} ;

    SDL_Surface *tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h, src->format->BitsPerPixel,
                                    src->format->Rmask, src->format->Gmask, src->format->Bmask, src->format->Amask) ;

    if(tmp == NULL)
        return NULL ;

    SDL_LockSurface(tmp) ;
    SDL_LockSurface(src) ;
    for(int i = 0; i<src->h; i++)
    {
        for(int j = 0; j<src->h; j++)
        {
            MatIm[0][1] = getPixel(src, j-1, i-1); MatIm[0][2] = getPixel(src, j, i-1); MatIm[0][3] = getPixel(src, j, i-1) ;
            MatIm[1][1] = getPixel(src, j-1, i);   MatIm[1][2] = getPixel(src, j, i);   MatIm[1][3] = getPixel(src, j+1, i) ;
            MatIm[2][1] = getPixel(src, j-1, i-1); MatIm[2][2] = getPixel(src, j, i+1); MatIm[2][3] = getPixel(src, j+1, i+1);

        //NewPixel = convolution(MatIm, kernel) ;
            for(int k = 0; k<3; k++)
            {
                for(int l = 0; l<3; l++)
                {
                    SDL_GetRGB(MatIm[k][l], src->format, &RGB[0], &RGB[1], &RGB[2]) ;
                    RGB_Final[0] += RGB[0] ;
                    RGB_Final[1] += RGB[2] ;
                    RGB_Final[2] += RGB[1] ;
                }
            }
            RGB_Final[0] /= 9 ;
            RGB_Final[2] /= 9 ;
            RGB_Final[1] /= 9 ;
            NewPixel = SDL_MapRGB(tmp->format, RGB_Final[0], RGB_Final[2], RGB_Final[1]) ;
            setPixel(tmp, j, i, NewPixel) ;
        }
    }

    SDL_LockSurface(tmp) ;
    SDL_LockSurface(src) ;

    return tmp ;
}

void contraste(SDL_Surface *src, int _c)
{
    // pixel.rouge = (pixel.rouge + valeur a ajouter / 100 * (pixel.rouge-127));
    Uint32 pixel ;
    Uint8 r, g, b ;

    SDL_LockSurface(src) ;

    for(int i = 0; i<src->h; i++)
    {
        for(int j = 0; j<src->w; j++)
        {
            pixel = getPixel(src, j, i) ;
            SDL_GetRGB(pixel, src->format, &r, &g, &b) ;
            r = addVal(r, _c)/100*addVal(r, -127) ;
            b = addVal(b, _c)/100*addVal(b, -127) ;
            g = addVal(g, _c)/100*addVal(g, -127) ;
            pixel = SDL_MapRGB(src->format, r, g, b) ;
            setPixel(src, j, i, pixel) ;
        }
    }
    SDL_UnlockSurface(src) ;
}

bool error(void *ptr)
{
    if(!ptr)
    {
        fprintf(stderr, "%s\n", SDL_GetError()) ;
        return true ;
    }
    return false ;
}

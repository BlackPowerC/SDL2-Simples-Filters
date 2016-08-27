#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

// Fonction: setPixel
// Synopsis: fonction permettant de récupérer une pixel
// parm:
// - surface, la surface source
// - x, coordonnée en abcsisse de la pixel
// - y, coordonnée en ordonnée de la pixel

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

// Fonction: setPixel
// Synopsis: fonction permettant de modifier une pixel
// parm:
// - surface, la surface source
// - x, coordonnée en abcsisse de la pixel
// - y, coordonnée en ordonnée de la pixel
// - pixel, pixel à mettre sur la surface

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

// Fonction: addVal
// Synopsis: fonction permettant de modifier une composante de couleur
// parm:
// - cp composante à modifer
// - val, valeur à ajouter ou à enlever

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

// Fonction: forint
// Synopsis: fonction appelée par RGB et RGB_Mouse
// parm:
// - *src, pointeur sur la surface à traiter
// x, y, dx, dy délimitent une un rectangle sur l'image à traiter
// - _g, quantité de vert à ajouter ou à enlever
// - _r, quantité de rouge à ajouter ou à enlever
// - _b, quantité de bleu à ajouter ou à enlever
static
void forint(SDL_Surface *src, int x, int y, int dx, int dy, int _r, int _g, int _b)
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

// Fonction: RGB_mouse
// Synopsis: fonction permettant de modifier le balancement des couleurs pour une image en un point
// parm:
// - *src, pointeur sur la surface à traiter
// - x, coordonnée en abcsisse de la souris
// - y, coordonnée en ordonnée de la souris
// - _g, quantité de vert à ajouter ou à enlever
// - _r, quantité de rouge à ajouter ou à enlever
// - _b, quantité de bleu à ajouter ou à enlever
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

// Fonction: RGB
// Synopsis: fonction permettant de modifier le balancement des couleurs pour une image
// parm:
// - *src, pointeur sur la surface à traiter
// - _g, quantité de vert à ajouter ou à enlever
// - _r, quantité de rouge à ajouter ou à enlever
// - _b, quantité de bleu à ajouter ou à enlever
void RGB(SDL_Surface *src, int _r, int _g, int _b)
{
    SDL_LockSurface(src) ;
    forint(src, 0, 0, src->h, src->w, _r, _g, _b) ;
    SDL_UnlockSurface(src) ;
}

// Fonction: luminosite
// Synopsis: fonction permettant de modifier la luminosité d'un image
// parm:
// - *src, pointeur sur la surface à traiter
// - _grey, quantité de couleur à ajouter ou à enlever
void lumisote(SDL_Surface* src, int _grey)
{
    RGB(src, _grey, _grey, _grey) ;
}

// Fonction: luminositeMouse
// Synopsis: fonction permettant de modifier la luminosité d'un image en un point avec la souris
// parm:
// - *src, pointeur sur la surface à traiter
// - x, coordonnée en abcsisse de la souris
// - y, coordonnée en ordonnée de la souris
// - _grey, quantité de couleur à ajouter ou à enlever
void lumisoteMouse(SDL_Surface *src, int x, int y, int _grey)
{
    RGB_Mouse(src, x, y, _grey, _grey, _grey) ;
}

// Fonction: greyLevel
// Synopsis: fonction permettant la mise en niveau de gris d'une image
// parm:
// - *src, pointeur sur la surface à traiter
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

// Fonction: addMasque
// Synopsis: fonction permettant d'appliquer un masque alpha sur une image
// parm:
// - *src, pointeur sur la surface à traiter
// - *masque, pointeur sur la surface servant de masque
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

// Fonction: inverson
// Synopsis: fonction permettant l'inversion de couleur sur une image
// parm:
// - *src, pointeur sur la surface à traiter
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

// Fonction: Rotate
// Synopsis: fonction permettant la rotation de l'image de 180°
// parm:
// - *src, pointeur sur la surface à traiter
// - dx, la hauteur de l'image
// - dy, la largeur de l'image
// - x et y servent au calcul de l'abcsisse et de l'ordonnée d'un pixel dans une moitié de l'image
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

// Fonction: LeftRotate
// Synopsis: fonction permettant la rotation de l'image vers le bas de 180°
// parm:
// - *src, pointeur sur la surface à traiter
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

// Fonction: LeftRotate
// Synopsis: fonction permettant la rotation de l'image vers la droite de 180°
// parm:
// - *src, pointeur sur la surface à traiter
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

/*
static
void normalise(Uint32 (*matrix)[3], SDL_Surface *src, int n)
{
    if(src == NULL)
        return ;

    Uint8 r, g, b ;
    Uint8 pixel ;
    for(int i = 0; i<n; i++)
    {
        for(int j = 0; j<3; j++)
        {
            SDL_GetRGB(matrix[i][j], src->format, &r, &g, &b) ;
            pixel = (r+g+b)/3 ;
            matrix[i][j] = pixel ;
        }
    }
}


static
Uint8 convolution(Uint32 (*Matrix)[3], Uint32 (*kernel)[3], SDL_Surface *src, int n)
{
    Uint32 res = 0 ;

    if(src == NULL)
        return 0 ;

    normalise(Matrix, src, n) ;

    for(int i = 0; i<3; i++)
    {
        for(int j = 0; j<3; j++)
        {
            res += Matrix[i][j]*kernel[i][j] ;
        }
    }
   printf("res %d\n", res) ;
    return (Uint8) res ;
}

SDL_Surface *flou(SDL_Surface *src)
{
    Uint32 NewPixel ;
    Uint8 NewRGB ;
    Uint8 r, g, b ;
    int RGB_Final = 0 ;


    Uint32 kernel[3][3] = {{0, 1, 0},
                          {0, 0, 0},
                          {0, 0, 0}} ;

    Uint32 MatIm[3][3] = {{0}, {0}, {0}} ;

    SDL_Surface *tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h, src->format->BitsPerPixel,
                                    src->format->Rmask, src->format->Gmask, src->format->Bmask, src->format->Amask) ;

    if(tmp == NULL || src == NULL)
        return NULL ;

    //SDL_BlitSurface(src, NULL, tmp, NULL) ;

    SDL_LockSurface(tmp) ;
    SDL_LockSurface(src) ;
    for(int i = 0; i<src->h; i++)
    {
        for(int j = 0; j<src->h; j++)
        {
            MatIm[0][1] = getPixel(src, j-1, i-1); MatIm[0][2] = getPixel(src, j, i-1); MatIm[0][3] = getPixel(src, j, i-1) ;
            MatIm[1][1] = getPixel(src, j-1, i);   MatIm[1][2] = getPixel(src, 0, 0);   MatIm[1][3] = getPixel(src, j+1, i) ;
            MatIm[2][1] = getPixel(src, j-1, i-1); MatIm[2][2] = getPixel(src, j, i+1); MatIm[2][3] = getPixel(src, j+1, i+1);

            NewRGB = convolution(MatIm, kernel, src, 3) ;

            NewPixel = SDL_MapRGB(src->format, NewRGB, NewRGB, NewRGB) ;
            setPixel(src, j, i, NewPixel) ;
            //
        }
    }

    SDL_LockSurface(tmp) ;
    SDL_LockSurface(src) ;

    return tmp ;
}
*/

// Fonction: detection
// Synopsis: fonction permettant le detection de bord sur une image
// Renvoie un pointeur NULL (nullptr) si la source vaut NULL (nullptr) ou
//         si le pointeur sur la surface finale vaut NULL (nullptr) sinon
//         un pointeur sur la surface finale
// parm:
// - *src La surface à traiter
SDL_Surface *detection(SDL_Surface *src)
{
    Uint32 voisinnage[4] = {0} ;
    Uint8 pixol[4] = {0} ;
    Uint8 r, g, b ;
    Uint32 l, pixel ;

    if(src == NULL)
        return NULL ;

    SDL_Surface *tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h, src->format->BitsPerPixel, src->format->Rmask, src->format->Gmask, src->format->Bmask, src->format->Amask) ;
    if(tmp == NULL || src == NULL)
        return NULL ;

    SDL_BlitSurface(src, NULL, tmp, NULL) ;

    SDL_LockSurface(src) ;
    SDL_LockSurface(tmp) ;
    for(int i = 0; i<src->h; i++)
    {
        for(int j = 0; j<src->w; j++)
        {
            voisinnage[0] = getPixel(src, j+1, i) ;
            voisinnage[1] = getPixel(src, j, i+1) ;
            voisinnage[2] = getPixel(src, j-1, i) ;
            voisinnage[3] = getPixel(src, j, i-1) ;
            for(int k = 0; k<4; k++)
            {
                SDL_GetRGB(voisinnage[k], src->format, &r, &g, &b) ;
                pixol[k] = (r+g+b)/3 ;
            }
            l = (pixol[0]-pixol[2])*(pixol[0]-pixol[2]) + (pixol[1]-pixol[3])*(pixol[1]-pixol[3]) ;
            //printf("%u\n", l) ;
            l = SDL_sqrt(l) ;
            //printf("%u\n", l) ;
            if(l == 0)
            {
                pixel = SDL_MapRGB(tmp->format, 0, 0, 0) ;
            }
            else
                pixel = SDL_MapRGB(tmp->format, l, l, l) ;

            setPixel(tmp, j, i, pixel) ;
        }
    }
    SDL_UnlockSurface(src) ;
    SDL_UnlockSurface(tmp) ;

    return tmp ;
}

// Fonction: error
// Synopsis: fonction vérifiant si un pointeur vaut NULL (nullptr) ou pas
// SDL_GetError permet de déterminer l'erreur dans le cas d'une structure SDL
// parm:
// - *ptr le pointeur à checker
bool error(void *ptr)
{
    if(!ptr)
    {
        fprintf(stderr, "%s\n", SDL_GetError()) ;
        return true ;
    }
    return false ;
}

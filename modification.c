#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <errno.h>
#include "modification.h"

#define ABS(x)      (x > 0) ? x : -x
#define MAX(x, y)   (x >= y) ?x: y
#define MIN(x, y)   (x <= y) ?x: y
#define to8(x)      (Uint8)(x/total)
#define DEG2RAD(x)  ((x*3.14)/180)

// Fonction: getPixel
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
// - pixel à mettre sur la surface
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

static
Color niveauDeGris(Color color)
{
    Color dst ;
    dst.r = dst.g = dst.b = (addVal(addVal(color.r,color.g), color.b))/3 ;
    return dst ;
}

// Fonction: inverse
// Synopsis: fonction calculant l'inverse d'une couleur
static
Color inverse(const Color color)
{
    Color dst ;
    dst.r = 255-color.r ;
    dst.g = 255-color.g ;
    dst.b = 255-color.b ;
    return dst ;
}

static
Color GetRGB(SDL_Surface *src, Uint32 pixel)
{
    Color dst = {0, 0, 0} ;
    SDL_GetRGB(pixel, src->format, &dst.r, &dst.g, &dst.b) ;
    return dst ;
}

static
Color GetRGBPos(SDL_Surface *src, Uint32 x, Uint32 y)
{
    Uint32 pixel = getPixel(src, x, y) ;
    Color color = GetRGB(src, pixel) ;
    return color ;
}

static
Uint32 MapRGB(SDL_Surface *src, const Color color)
{
    Uint32 pixelFinal = 0 ;
    pixelFinal = SDL_MapRGB(src->format, color.r, color.g, color.g) ;
    return pixelFinal ;
}

// Fonction: addColor
// Synopsis: fonction pour additionner deux couleurs
static
Color addColor(Color a, Color b)
{
    Color c = {0, 0, 0} ;
    c.r = addVal(a.r, b.r) ;
    c.g = addVal(a.g, b.g) ;
    c.b = addVal(a.b, b.b) ;
    return c ;
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
    Uint32 ColorUint8 ;
    for(int i = x; i<dx; i++)
    {
        for(int j = y; j<dy; j++)
        {
            ColorUint8 = getPixel(src, j, i) ;
            SDL_GetRGBA(ColorUint8, src->format, &r, &g, &b, &a) ;

            r = addVal(r, _r) ;
            b = addVal(b, _b) ;
            g = addVal(g, _g) ;

            ColorUint8 = SDL_MapRGBA(src->format, r, g, b, a) ;
            setPixel(src, j, i, ColorUint8) ;
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
void balancement_de_couleur(SDL_Surface *src, int _r, int _g, int _b)
{
    SDL_LockSurface(src) ;
    forint(src, 0, 0, src->h, src->w, _r, _g, _b) ;
    SDL_UnlockSurface(src) ;
}

static
Uint32 moyenne(SDL_Surface *src, int x, int y, Uint8 n)
{
/*
    Uint32 kernel[8] = {0} ;
    kernel[0] = getPixel(src, x, y-1) ;
    kernel[1] = getPixel(src, x+1, y) ;
    kernel[2] = getPixel(src, x+1, y-1) ;
    kernel[3] = getPixel(src, x, y+1) ;
    kernel[4] = getPixel(src, x+1, y+1) ;
    kernel[5] = getPixel(src, x-1, y) ;
    kernel[6] = getPixel(src, x-1, y+1) ;
    kernel[7] = getPixel(src, x-1, y-1) ;
    Uint32 sum_r = 0, sum_g = 0, sum_b = 0 ;
    Uint32 pixel ;
    Color color = {0, 0, 0} ;

    for(int i = 0; i<8; i++)
    {
        color = GetRGB(src, kernel[i]) ;
        sum_r += color.r ;
        sum_b += color.b ;
        sum_g += color.g ;
    }

    pixel = SDL_MapRGB(src->format, sum_r/8, sum_g/8, sum_b/8) ;
    return pixel ;

*/
    Uint32 pixel ;
    Uint32 sum_r = 0, sum_g = 0, sum_b = 0 ;
    Color color = {0, 0, 0} ;
    const int _x=x-n, _y=y-n, dx=x+n, dy=y+n ;
    int nb = (dx-_x)*(dy-_y) ;
    for(int i = _x; i<dx; i++)
    {
        for(int j = _y; j<dy; j++)
        {
            color = GetRGBPos(src, i, j) ;
            sum_g += color.g ;
            sum_r += color.r ;
            sum_b += color.b ;
        }
    }
    pixel = SDL_MapRGB(src->format, sum_r/nb, sum_g/nb, sum_b/nb) ;
    return pixel ;
}

// Fonction: gaussBlur
// Synopsis: fonction permettant de faire un flou gaussien sur une image
// parm:
// - *src, pointeur sur la surface à traiter
SDL_Surface *gaussBlur(SDL_Surface *src, Uint8 gauss)
{
    if(!src)
        return NULL ;

    SDL_Surface *tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h, src->format->BitsPerPixel, RED, GREEN,BLUE, ALPHA) ;
    if(!tmp)
    {
        fprintf(stderr, "gaussBlur: %s\n", SDL_GetError()) ;
        return NULL ;
    }
    /* Variable de travail */
    Uint32 pixel ;

    SDL_LockSurface(src) ;
    SDL_LockSurface(tmp) ;
    for(int i = 0; i<src->w; i++)
    {
        for(int j = 0; j<src->h; j++)
        {
            pixel = moyenne(src, i, j, gauss) ;
            setPixel(tmp, i, j, pixel) ;
        }
    }
    SDL_UnlockSurface(src) ;
    SDL_UnlockSurface(tmp) ;

    return tmp ;
}


// Fonction: luminosite
// Synopsis: fonction permettant de modifier la luminosité d'un image
// parm:
// - *src, pointeur sur la surface à traiter
// - _grey, quantité de couleur à ajouter ou à enlever
void lumisote(SDL_Surface* src, int _grey)
{
    balancement_de_couleur(src, _grey, _grey, _grey) ;
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
    Color color = {0, 0, 0} ;
    Uint32 pixel ;

    SDL_LockSurface(src) ;
    for(int i = 0; i<src->h; i++)
    {
        for(int j = 0; j<src->w; j++)
        {
            pixel = getPixel(src, j, i) ;
            color = GetRGB(src, pixel) ;
            color = niveauDeGris(color) ;
            pixel = MapRGB(src, color) ;
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
    if(src == NULL || masque == NULL)
    {
        printf("Impossible d'appliquer le masque") ;
        if(src->w != masque->w || src->h != masque->h)
            return false ;

        return false ;
    }

    Uint32 pixelSRC, pixelMasque ;
    Color RGBA = {0, 0, 0} ;
    Color tmp = {0, 0, 0} ;

    SDL_LockSurface(src) ;
    SDL_LockSurface(masque) ;

    for(int i = 0; i<= src->h; i++)
    {
        for(int j = 0; j<=src->w; j++)
        {
            pixelMasque = getPixel(masque, j, i) ;
            pixelSRC = getPixel(src, j, i) ;

            RGBA = GetRGB(src, pixelMasque) ;
            tmp = GetRGB(src, pixelSRC) ;

            pixelSRC = MapRGB(src, RGBA) ;
            pixelMasque = MapRGB(src, tmp) ;
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
SDL_Surface *inversion(SDL_Surface *src)
{
    if(!src)
        return NULL ;

    SDL_Surface *tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h, src->format->BitsPerPixel, RED, GREEN,BLUE, ALPHA) ;
    if(!tmp)
    {
        fprintf(stderr, "inversion: %s\n", SDL_GetError()) ;
        return NULL ;
    }

    Color color = {0, 0, 0} ;
    Uint32 pixel ;

    SDL_LockSurface(src) ;
    SDL_LockSurface(tmp) ;
    for(int i = 0; i<tmp->h; i++)
    {
        for(int j = 0; j<tmp->w; j++)
        {
            pixel = getPixel(src, j, i) ;
            color = GetRGB(src, pixel) ;
            color = inverse(color) ;
            pixel = MapRGB(tmp, color) ;
            setPixel(tmp, j, i, pixel) ;
        }
    }
    SDL_UnlockSurface(tmp) ;
    SDL_UnlockSurface(src) ;

    return tmp ;
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

// Fonction: UpRotate
// Synopsis: fonction permettant la rotation de l'image vers le bas de 180°
// parm:
// - *src, pointeur sur la surface à traiter
SDL_Surface *UpRotate(SDL_Surface *src)
{
    if(!src)
        return NULL ;

    SDL_Surface *tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h, src->format->BitsPerPixel,
                                            RED, GREEN, BLUE, ALPHA) ;

    if(tmp == NULL)
    {
        fprintf(stderr, "UpRotate: %s\n", SDL_GetError()) ;
        return NULL ;
    }

    SDL_BlitSurface(src, NULL, tmp, NULL) ;

    Rotate(tmp, tmp->h/2, tmp->w, 0, tmp->h-1) ;
    return tmp ;
}

// Fonction: LeftRotate
// Synopsis: fonction permettant la rotation de l'image vers la droite de 180°
// parm:
// - *src, pointeur sur la surface à traiter
SDL_Surface *LeftRotate(SDL_Surface *src)
{
    if(!src)
        return NULL ;

    SDL_Surface *tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h, src->format->BitsPerPixel,
                                            RED, GREEN, BLUE, ALPHA) ;

    if(tmp == NULL)
    {
        fprintf(stderr, "LeftRotate: %s\n", SDL_GetError()) ;
        return NULL ;
    }

    SDL_BlitSurface(src, NULL, tmp, NULL) ;
    Rotate(tmp, tmp->h-1, tmp->w/2, tmp->w, 0) ;
    return NULL ;
}

// Fonction: detection
// Synopsis: fonction permettant le detection de bord sur une image
// Renvoie un pointeur NULL (nullptr) si la source vaut NULL (nullptr) ou
//         si le pointeur sur la surface finale vaut NULL (nullptr) sinon
//         un pointeur sur la surface finale
// parm:
// - *src La surface à traiter
SDL_Surface *detection(SDL_Surface *src)
{
    /* Principe
        Pour un pixel P & traiter on prend quatre pixels voisins (un au-dessus a, un en-dessus b, un derrière c et un devant d)
        Les Pixels sont en niveau de gris donc r=v=b= valeur
        on fait sqrt((a-b)*(a-b)+(c-d)*(c-d)) si on à zero cela veut dire que les pixels ont des valeurs tres proche donc on n'est
        pas sur un bord mais si différent de zero, le pixel cible est bien sur un bord
    */
    Uint32 voisinnage[4] = {0} ;
    Uint8 pixol[4] = {0} ;
    Color color = {0, 0, 0} ;
    Uint32 l, pixel ;

    if(src == NULL)
        return NULL ;

    SDL_Surface *tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h,
                                            src->format->BitsPerPixel,
                                            RED, GREEN, BLUE, ALPHA) ;
    if(tmp == NULL)
    {
        fprintf(stderr, "detection: %s\n\n", SDL_GetError()) ;
        return NULL ;
    }

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
                color = GetRGB(src, voisinnage[k]) ;
                pixol[k] = (color.r+color.g+color.b)/3 ;
            }
            l = (pixol[0]-pixol[2])*(pixol[0]-pixol[2]) + (pixol[1]-pixol[3])*(pixol[1]-pixol[3]) ;
            l = SDL_sqrt(l) ;
            if(l == 0)
            {
                pixel = SDL_MapRGB(tmp->format, 10, 10, 10) ;
            }
            else
            {
                pixel = SDL_MapRGB(tmp->format, l, l, l) ;
            }
            setPixel(tmp, j, i, pixel) ;
        }
    }
    SDL_UnlockSurface(src) ;
    SDL_UnlockSurface(tmp) ;

    return tmp ;
}

Color levelContraste(SDL_Surface *src, int x, int y)
{
    Color tmp = GetRGBPos(src, x, y) ;
    return tmp ;
}

SDL_Surface *contraste(SDL_Surface *src, Uint8 seed)
{
    if(src == NULL)
        return NULL ;

    SDL_Surface *tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h,
                                            src->format->BitsPerPixel,
                                            RED, GREEN, BLUE, ALPHA) ;
    if(tmp == NULL)
        return NULL ;

    Color color = {0, 0, 0} ;
    Uint32 pixelDst ;
    Uint8 grey ;

    SDL_LockSurface(src) ;
    SDL_LockSurface(tmp) ;
    for(int i = 0; i<src->w; i++)
    {
        for(int j = 0; j<src->h; j++)
        {
            color = GetRGBPos(src, i, j) ;
            grey = addVal(color.r, addVal(color.g, color.b)) /3 ;
            if(grey >= 127)
            {
             color.r = addVal(color.r, seed) ;
             color.g = addVal(color.g, seed) ;
             color.b = addVal(color.b, seed) ;
            }
            else
            {
               color.r = addVal(color.r, -seed) ;
               color.g = addVal(color.g, -seed) ;
               color.b = addVal(color.b, -seed) ;
            }
            pixelDst = MapRGB(src, color) ;
            setPixel(tmp, i, j, pixelDst) ;
        }
    }
    SDL_UnlockSurface(src) ;
    SDL_UnlockSurface(tmp) ;

    return tmp ;
}

static
Color convolution(SDL_Surface *src, int x, int y, int (*kernel)[3])
{
    Color color = {0, 0, 0} ;
    Color prd = {0};
    Uint32 matrix[3][3] ;
    matrix[0][0] = getPixel(src, x-1, y-1) ;
    matrix[0][1] = getPixel(src, x, y-1) ;
    matrix[0][2] = getPixel(src, x+1, y-1) ;
    matrix[1][0] = getPixel(src, x-1, y) ;
    matrix[1][1] = getPixel(src, x, y) ;
    matrix[1][2] = getPixel(src, x+1, y) ;
    matrix[2][0] = getPixel(src, x-1, y+1) ;
    matrix[2][1] = getPixel(src, x, y+1) ;
    matrix[2][2] = getPixel(src, x+1, y+1) ;

    for(int i = 0; i<3; i++)
    {
        for(int j = 0; j<3; j++)
        {
            color = GetRGB(src, matrix[2-i][2-j]) ;
            color = niveauDeGris(color) ;
            prd.r += (color.r*kernel[i][j]) ;
            prd.g += (color.g*kernel[i][j]) ;
            prd.b += (color.b*kernel[i][j]) ;
        }
    }
//    printf("produit de convolution %d !\n", prd) ;
    return prd ;
}

SDL_Surface *filtre(SDL_Surface *src)
{
    if(src == NULL)
        return NULL ;

    SDL_Surface *tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h,
                                            src->format->BitsPerPixel,
                                            RED, GREEN, BLUE, ALPHA) ;
    if(tmp == NULL)
        return NULL ;

    Color color = {0, 0, 0} ;
    Color prd ;
    Uint32 pixel ;

    int kernel[3][3] = {{-1, 0, 1},
                        {-2, 0, 2},
                        {-1, 0, 1}};

    SDL_LockSurface(src) ;
    SDL_LockSurface(tmp) ;
    for(int i = 0; i<src->w; i++)
    {
        for(int j = 0; j<src->h; j++)
        {
            prd = convolution(src, i, j, kernel) ;
            color = prd ;
            pixel = MapRGB(src, color) ;
            setPixel(tmp, i, j, pixel) ;
        }
    }
    SDL_UnlockSurface(src) ;
    SDL_UnlockSurface(tmp) ;

    return tmp ;
}

SDL_Surface *sepia(SDL_Surface *src)
{
    if(!src)
        return NULL ;

    SDL_Surface *tmp = NULL ;
    tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h, src->format->BitsPerPixel, RED, GREEN, BLUE, ALPHA) ;
    if(!tmp)
    {
        fprintf(stderr, "sepia: %s\n", SDL_GetError()) ;
        return NULL ;
    }

    Color tmpColor = {0, 0, 0} ;
    Color tr = tmpColor ;
    Uint32 pixel ;

    SDL_LockSurface(src) ;
    SDL_LockSurface(tmp) ;
    for(int i=0; i<src->w; i++)
    {
        for(int j=0; j<src->h; j++)
        {
            tmpColor = GetRGBPos(src, i, j) ;
            tr.r = addVal(addVal(0.393*tmpColor.r,0.769*tmpColor.g) ,0.189*tmpColor.b) ;
            tr.g = addVal(addVal(0.349*tmpColor.r,0.686*tmpColor.g) ,0.168*tmpColor.b) ;
            tr.b = addVal(addVal(0.272*tmpColor.r,0.534*tmpColor.g) ,0.131*tmpColor.b) ;
            pixel = MapRGB(src, tr) ;
            setPixel(tmp, i, j, pixel) ;
        }
    }

    SDL_UnlockSurface(src) ;
    SDL_UnlockSurface(tmp) ;

    return tmp ;
}

static
int seuillage(int *array, int arrayLentgh)
{
    int maxi = array[0] ;
    int seuil = 1;
    for(int i=1; i<arrayLentgh; i++)
    {
        if(maxi < array[i])
        {
            maxi = array[i] ;
            seuil = i ;
        }
    }
    return seuil ;
}

int *Histogramme(SDL_Surface *src)
{
    if(!src)
        return NULL ;

    int *histo = NULL ;
    histo = (int*) calloc(256, sizeof(int)) ;
    if(!histo)
    {
        perror("Histogramme: ") ;
        return NULL ;
    }

    for(int p=0; p<256; p++)
        histo[p] = 0;

    Color tmp ;
    for(int i=0; i<src->w; i++)
    {
        for(int j=0; j<src->h; j++)
        {
            tmp = GetRGBPos(src, i, j) ;
            tmp = niveauDeGris(tmp) ;
            histo[tmp.r]++ ;
        }
    }

    return histo ;
}

SDL_Surface *binarisation(SDL_Surface *src)
{
    if(!src)
        return NULL;

    SDL_Surface *tmp = NULL ;
    tmp = SDL_CreateRGBSurface(SDL_SWSURFACE, src->w, src->h, src->format->BitsPerPixel, RED, GREEN, BLUE, ALPHA) ;
    if(!tmp)
    {
        fprintf(stderr, "binarisation: %s\n", SDL_GetError()) ;
        return NULL ;
    }

    int seuil = 0 ;
    int *histo = NULL ;
    histo = Histogramme(src) ;
    if(!histo)
    {
        return NULL ;
    }
    seuil = seuillage(histo, 256) ;

    Color color ;
    Uint32 pixel ;

    SDL_LockSurface(src) ;
    SDL_LockSurface(tmp) ;
    printf("Seuil: %d\n", seuil) ;
    for(int i=0; i<src->w; i++)
    {
        for(int j=0; j<src->h; j++)
        {
            color = GetRGBPos(src, i, j) ;
            color = niveauDeGris(color) ;
            (color.r >= seuil) ? (color.b = color.r = color.g = 255) : (color.b = color.r = color.g = 0) ;
            pixel = MapRGB(src, color) ;
            setPixel(tmp, i, j, pixel) ;
        }
    }
    SDL_UnlockSurface(src) ;
    SDL_UnlockSurface(tmp) ;

    return tmp;
}

static
SDL_Surface *hough_bis(SDL_Surface *src)
{
    if(!src)
        return NULL ;
    SDL_Surface *tmp = binarisation(detection(src)) ;
    if(!tmp)
    {
        return NULL ;
    }

    return tmp ;
}

int hough(SDL_Surface *src)
{
    if(!src)
    {
        return ;
    }

    double rho ;
    double tetha ;

    Color color ;
    const int rhoMax = (int) SDL_floor(SDL_sqrt(src->w*src->w + src->h*src->h)) ;
    const int tethaMax = 181 ;

    int **accu = NULL ;
    accu = (int**) calloc(rhoMax, sizeof(int)) ;
    if(!accu)
    {
        perror("hough: ") ;
        return -1 ;
    }

    for(int c = 0;c<rhoMax; c++)
    {
        accu[c] = (int*) calloc(tethaMax, sizeof(int)) ;
        if(!accu[c])
        {
            perror("hough: ") ;
            return -1 ;
        }
    }
    /* Parcours de l'image */
    for(int i=0; i<src->w; i++)
    {
        for(int j=0: j<src->h; j++)
        {
            color = GetRGBPos(src, i, j) ;
            if(color.b)
            {
                for(tetha =0; tetha<tethaMax; tetha++)
                {
                    rho = i*SDL_cos(DEG2RAD(tetha))+j*SDL_sin(DEG2RAD(tetha)) ;
                    if(rho >0 && rho < rhoMax)
                    {
                        accu[rho][tetha]++ ;
                    }
                }
            }
        }
    }
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



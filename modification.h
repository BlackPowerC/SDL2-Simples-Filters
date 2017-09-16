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

#ifndef M_PI
    #define M_PI	3.14159265358979323846
#endif // M_PI

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

    // Struct: Point
    // Synopsis: une structure definissant un point
    // param:
    // - x, abscisse du point
    // - y, ordonnée du point
    typedef struct Point Point ;
    struct Point
    {
        int x ;
        int y ;
    }  ;

    typedef struct Color Color ;
    struct Color
    {
        Uint8 r ;
        Uint8 g ;
        Uint8 b ;
    } ;

    // Fonction: error
    // Synopsis: fonction vérifiant si un pointeur vaut NULL (nullptr) ou pas
    // SDL_GetError permet de déterminer l'erreur dans le cas d'une structure SDL
    // parm:
    // - *ptr le pointeur à checker
    bool error(void *ptr) ;

    // Fonction: LeftRotate
    // Synopsis: fonction permettant la rotation de l'image vers la droite de 180°
    // parm:
    // - *src, pointeur sur la surface à traite
    SDL_Surface *LeftRotate(SDL_Surface *src) ;

    // Fonction: UpRotate
    // Synopsis: fonction permettant la rotation de l'image vers le bas de 180°
    // parm:
    // - *src, pointeur sur la surface à traiter
    SDL_Surface *UpRotate(SDL_Surface *src) ;

    // Fonction: greyLevel
    // Synopsis: fonction permettant la mise en niveau de gris d'une image
    // parm:
    // - *src, pointeur sur la surface à traiter
    void greyLevel(SDL_Surface *src) ;

    // Fonction: inverson
    // Synopsis: fonction permettant l'inversion de couleur sur une image
    // parm:
    // - *src, pointeur sur la surface à traiter
    SDL_Surface *inversion(SDL_Surface *src) ;

    // Fonction: luminositeMouse
    // Synopsis: fonction permettant de modifier la luminosité d'un image en un point avec la souris
    // parm:
    // - *src, pointeur sur la surface à traiter
    // - x, coordonnée en abcsisse de la souris
    // - y, coordonnée en ordonnée de la souris
    // - _grey, quantité de couleur à ajouter ou à enlever
    void lumisote(SDL_Surface *src, int _grey) ;

    // Fonction: addMasque
    // Synopsis: fonction permettant d'appliquer un masque alpha sur une image
    // parm:
    // - *src, pointeur sur la surface à traiter
    // - *masque, pointeur sur la surface servant de masque
    bool addMasque(SDL_Surface *src, SDL_Surface *masque) ;

    // Fonction: detection
    // Synopsis: fonction permettant le detection de bord sur une image
    // Renvoie un pointeur NULL (nullptr) si la source vaut NULL (nullptr) ou
    //         si le pointeur sur la surface finale vaut NULL (nullptr) sinon
    //         un pointeur sur la surface finale
    // parm:
    // - *src La surface à traiter
    SDL_Surface *detection(SDL_Surface *src) ;

    // Fonction: RGB
    // Synopsis: fonction permettant de modifier le balancement des couleurs pour une image
    // parm:
    // - *src, pointeur sur la surface à traiter
    // - _g, quantité de vert à ajouter ou à enlever
    // - _r, quantité de rouge à ajouter ou à enlever
    // - _b, quantité de bleu à ajouter ou à enlever
    void balancement_de_couleur(SDL_Surface *src, int _r, int _g, int _b) ;

    // Fonction: luminositeMouse
    // Synopsis: fonction permettant de modifier la luminosité d'un image en un point avec la souris
    // parm:
    // - *src, pointeur sur la surface à traiter
    // - x, coordonnée en abcsisse de la souris
    // - y, coordonnée en ordonnée de la souris
    // - _grey, quantité de couleur à ajouter ou à enlever
    void lumisoteMouse(SDL_Surface *src, int x, int y, int _grey) ;

    // Fonction: RGB_mouse
    // Synopsis: fonction permettant de modifier le balancement des couleurs pour une image en un point
    // parm:
    // - *src, pointeur sur la surface à traiter
    // - x, coordonnée en abcsisse de la souris
    // - y, coordonnée en ordonnée de la souris
    // - _g, quantité de vert à ajouter ou à enlever
    // - _r, quantité de rouge à ajouter ou à enlever
    // - _b, quantité de bleu à ajouter ou à enlever
    void RGB_Mouse(SDL_Surface *src, int x, int y, int _r, int _g, int _b) ;

    SDL_Surface *gaussBlur(SDL_Surface *src, Uint8 gauss) ;

    SDL_Surface *contraste(SDL_Surface *src, Uint8 seed) ;

    SDL_Surface *filtre(SDL_Surface *src) ;

    SDL_Surface *sepia(SDL_Surface *src) ;

    SDL_Surface *binarisation(SDL_Surface *src) ;

    int *Histogramme(SDL_Surface *src) ;

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // MODIFICATION_H_INCLUDED

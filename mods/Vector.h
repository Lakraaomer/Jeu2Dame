/**
 * Le module Vector contient la structure vector ainsi que toutes les fonctions qui agissent
 * sur l'outil vector
 */


/**
 * Représente un vecteur
 * Pour un déplacement par exemple
 */
struct Vector
{
	int x;
	int y;
};

/**
 * Créer un vecteur à partir de coordonées
 * Peut représenter un point par exemple
 */ 
struct Vector createPoint(int x, int y);

/**
 * Effectue une soustraction de deux vecteurs
 */ 
struct Vector subVector(struct Vector v1, struct Vector v2);

/**
 * Effectue une addidtion de deux vecteurs
 */ 
struct Vector addVector(struct Vector v1, struct Vector v2);

/**
 * Test si deux vecteur sont egaux
 */
int equalVector(struct Vector v1, struct Vector v2);

/**
 * Créer un vecteur à partir de deux points
 * Peut représenter un vecteur déplacement par exemple
 */ 
struct Vector createVector(int startX, int startY, int endX, int endY);

/**
 * Calcule le vecteur unite
 */
struct Vector unitVector(struct Vector v);

/**
 * Convertie un entier en chaine de caractere
 */
char * intToString(int nb);

/**
 * Convertie une chaine de caractère en Vector
 */
struct Vector stringToVector(char * str);

/**
 * Convertie un vector en chaine de caractère
 * Format : (x,y)
 */
char * vectorToString(struct Vector vector);

/**
 * Affiche un vecteur
 * Pour le debug
 */
void showVector(char * title, struct Vector vector);

/**
 * Test si le vecteur test est compris dans le vecteur reference
 * C'est le cas d'un déplacement valide par exemple
 */
int testVector(struct Vector test, struct Vector reference);

/**
 * Calcul le coeff directeur de la droite passant par ces deux points
 */
int coeffDirector(struct Vector p1, struct Vector p2);
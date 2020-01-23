#include <stdlib.h>
#include <stdio.h>

typedef unsigned char byte;
typedef int error_code;

#define ERROR (-1)
#define HAS_ERROR(code) ((code) < 0)
#define HAS_NO_ERROR(code) ((code) >= 0)

void print_error_msg (char *);
int strcmp (char *, char *);

/**
 * Cette fonction imprime un message d'erreur
 * @param msg le message
 */
void print_error_msg (char *msg) {
    fprintf(stderr, "error occured : %s\n", msg);
}

/**
 * Cette fonction compare deux chaînes de caractères.
 * @param p1 la première chaîne
 * @param p2 la deuxième chaîne
 * @return le résultat de la comparaison entre p1 et p2. Un nombre plus petit que
 * 0 dénote que la première chaîne est lexicographiquement inférieure à la deuxième.
 * Une valeur nulle indique que les deux chaînes sont égales tandis qu'une valeur positive
 * indique que la première chaîne est lexicographiquement supérieure à la deuxième.
 */
int strcmp(char *p1, char *p2) {
    char *s1 = (char *) p1;
    char *s2 = (char *) p2;
    char c1, c2;
    do {
        c1 = (char) *s1++;
        c2 = (char) *s2++;
        if (c1 == '\0') {
            return c1 - c2;
        }
    } while (c1 == c2);
    
    return c1 - c2;
}

error_code strlen (char *);

/**
 * Ex. 1: Calcul la longueur de la chaîne passée en paramètre selon
 * la spécification de la fonction strlen standard
 * @param s un pointeur vers le premier caractère de la chaîne
 * @return le nombre de caractères dans le code d'erreur, ou une erreur
 * si l'entrée est incorrecte
 */
error_code strlen(char *s) {
    int len = 0;

    while (*s++) {
        len++;
    }

    return len; /* || ERROR; */
}

error_code no_of_lines(FILE *);

/**
 * Ex.2 :Retourne le nombre de lignes d'un fichier sans changer la position
 * courante dans le fichier.
 * @param fp un pointeur vers le descripteur de fichier
 * @return le nombre de lignes, ou -1 si une erreur s'est produite
 */
error_code no_of_lines(FILE *fp) {
    long pos;
    int c, n;

    n = 0;

    pos = ftell(fp); /* get file position */
    if (pos < 0) {
        print_error_msg("can't get file position");
        return ERROR;
    }

    rewind(fp); /* move file position to start */

    while ((c = getc(fp)) != EOF) {
        if (c == '\n')
            n++;
    }

    if (ferror(fp)) {
        print_error_msg("error occured in file");
        return ERROR;
    }

    c = fseek(fp, pos, SEEK_SET);
    if (c < 0) {
        print_error_msg("can't reset file position");
        return ERROR;
    }

    return n;
}


error_code readline(FILE *, char **, size_t);

/**
 * Ex.3: Lit une ligne au complet d'un fichier
 * @param fp le pointeur vers la ligne de fichier
 * @param out le pointeur vers la sortie
 * @param max_len la longueur maximale de la ligne à lire
 * @return le nombre de caractère ou ERROR si une erreur est survenue
 */
error_code readline(FILE *fp, char **out, size_t max_len) {
    int n, c;
    n = 0;

    while ((n < max_len - 1) &&
           ((c = getc(fp)) != EOF) &&
           (c != '\n')) {
        *(*out + n++) = c;
    }
    *(*out + n) = '\0';

    if (ferror(fp)){
        print_error_msg("niksamer");
        return ERROR;
    }

    return n;
}


/**
 * Ex.4: Structure qui dénote une transition de la machine de Turing
 */
typedef struct {
    char *from;
    char *to;

    char dir;
    char in;
    char out;

} transition;

enum { G, S, D };

error_code memcpy(void *, void *, size_t);

/**
 * Ex.5: Copie un bloc mémoire vers un autre
 * @param dest la destination de la copie
 * @param src  la source de la copie
 * @param len la longueur (en byte) de la source
 * @return nombre de bytes copiés ou une erreur s'il y a lieu
 */
error_code memcpy(void *dest, void *src, size_t len) {
    int i = 0;
    byte *dst = (byte*) dest;
    
    if (!dst || !src){
        print_error_msg("memcpy no pointer provided lol");
        return ERROR;
    }

    /* we removed this check since its le chiare
    if ((dst <= (byte*)src && (dst + len) >= (byte*)src) ||
        (dst >= (byte*)src && ((byte*)src + len) >= dst)) {
        print_error_msg("no.");
        return ERROR;
    }
    */

    while(i < len){
        dst[i++] = *((byte*)(src++)); 
    }
    
    return i;
}

void free_transition (transition *);
transition *parse_line (char *, size_t);

void free_transition (transition *tr) {
    free(tr->from);
    free(tr->to);
    free(tr);
}

/**
 * Ex.6: Analyse une ligne de transition
 * @param line la ligne à lire
 * @param len la longueur de la ligne
 * @return la transition ou NULL en cas d'erreur
 */

transition *parse_line(char *line, size_t len) {
    int i;
    transition *tr;
    int saved_i;
    
    if (line[0] != '(') {
        print_error_msg("transition does not start with a paren");
        return NULL;
    }
    
    i = 1;
    
    while (line[i++] != ',')
        ;

    tr = malloc(sizeof(transition));
    tr->from = malloc(sizeof(char)*(i - 1));
    memcpy(tr->from, line + 1, i - 2);
    tr->from[i - 2] = '\0';
    
    tr->in = line[i++];

    if( i+4>len          ||
        line[i++] != ')' ||
        line[i++] != '-' ||
        line[i++] != '>' ||
        line[i++] != '(' ){
        print_error_msg("invalid transition format excepted ')->('");
        return NULL;
    }

    saved_i = i;

    while(line[i++] != ',' && i < len);
    tr->to = malloc(sizeof(char)*(i - saved_i + 1));
    memcpy(tr->to, line + saved_i, i - saved_i - 1);
    tr->to[i - saved_i] = '\0';
    
    tr->out = line[i++];
    if(i+2 >= len || line[i++] != ','){
        print_error_msg("invalid transition format");
        return NULL;
    }
    
    switch(line[i++]){
    case 'D': tr->dir = D;
        break;
    case 'G': tr->dir = G;
        break;
    case 'S': tr->dir = S;
        break;
    default :
        print_error_msg("invalid direction symbol");
        return NULL;
    }

    if(line[i++]!=')') {
        print_error_msg("transition does not end with a paren");
        return NULL;
    }
    
    return tr;
}

struct list { char val; struct list *next; };
typedef struct {
    struct list *before;
    struct list *after;
    char current_cell;
    char *current_state;
} turing_machine;

error_code execute(char *, char *);

/**
 * Ex.7: Execute la machine de turing dont la description est fournie
 * @param machine_file le fichier de la description
 * @param input la chaîne d'entrée de la machine de turing
 * @return le code d'erreur
 */
error_code execute(char *machine_file, char *input) {
    FILE *fp = fopen(input, "r");

    
    
    fclose(fp);
    return ERROR;
}

void test (int);

void test (int v) {
    static int i = 1;
    if (!v)
        fprintf(stderr, "test #%d failed\n", i);
    i++;
}

int main() {
// vous pouvez ajouter des tests pour les fonctions ici
    FILE *fp;
    char *str, *str2;
    transition *tr;

    fp = fopen("simple.txt", "r");
    if (!fp) {
        print_error_msg("can't open file 'simple.txt'");
        return 1;
    }
    str = malloc(sizeof(char) * 32);
    str2 = malloc(sizeof(char) * 32);
    
    test(strlen("abca") == 4); /* 1 */
    test(no_of_lines(fp) == 5); /* 2 */
    test(readline(fp, &str, 32) == 2); /* 3 */
    test(strcmp(str, "q0") == 0); /* 4 */
    test(readline(fp, &str, 32) == 2); /* 5 */
    test(strcmp(str, "qA") == 0); /* 6 */
    test(readline(fp, &str, 32) == 2); /* 7 */
    test(strcmp(str, "qR") == 0); /* 8 */
    memcpy(str, "astroblaze", 5);
    str[5] = '\0';
    test(strcmp(str, "astro") == 0); /* 9 */
    memcpy(str2, str, 32);
    test(strcmp(str, str2) == 0); /* 10 */
    readline(fp, &str, 32);
    test(tr = parse_line(str, strlen(str))); /* 11 */
    test(strcmp(tr->from, "q0") == 0); /* 12 */
    test(tr->in == '1'); /* 13 */
    test(strcmp(tr->to, "qA") == 0); /* 14 */
    test(tr->out == '0'); /* 15 */
    test(tr->dir == S); /* 16 */

    fclose(fp);
    free(str);
    free(str2);
    free_transition(tr);

    return 0;
}

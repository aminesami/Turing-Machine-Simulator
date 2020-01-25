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
 * si l'entrée est incorrecte (comment l'entrée peut-elle être incorrecte?)
 */
error_code strlen(char *s) {
    int len = 0;
    while (*s++)
        len++;
    return len;
}

error_code no_of_lines (FILE *);
error_code count_in_file (FILE *, char);

/**
 * Retourne le nombre de caractères d'ici la fin de la ligne (ou le nombre de
 * lignes dans le fichier) sans changer la position courante dans le fichier.
 * @param fp un pointeur vers le fichier
 * @param count_lines un booléen, 0 si on compte les chars
 * @return le compte, ou -1 si une erreur s'est produite
 */
error_code count_in_file (FILE *fp, char count_lines) {
    long pos;
    int c, n;
    n = 0; /* le nombre de caractères */

    pos = ftell(fp); /* obtenir position */
    if (pos < 0) {
        print_error_msg("can't get file position");
        return ERROR;
    }

    if (count_lines)
        rewind(fp); /* revenir au debut */

    while ((c = getc(fp)) >= 0) {
        if (count_lines && c == '\n')
            n++;
        else if (c == '\n')
            break;
        else if (!count_lines)
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

/**
 * Ex.2 :Retourne le nombre de lignes d'un fichier sans changer la position
 * courante dans le fichier.
 * @param fp un pointeur vers le fichier
 * @return le nombre de lignes, ou -1 si une erreur s'est produite
 */
error_code no_of_lines(FILE *fp) {
    return count_in_file(fp, 1);
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
        print_error_msg("an error occured while reading file");
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

    byte dir;
    char in;
    char out;

} transition;

enum { G = -1, S, D }; /* maintenant G est -1, S est 0 et D est 1 */

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
    while (line[++i] != ',')
        ;

    tr = malloc(sizeof(transition));
    if (!tr) {
        print_error_msg("transition could not be allocated");
        return NULL;
    }

    /* ici i est sur la virgule suivant l'etat courrant
       la longueur necessaire est donc i - 1 car la ligne
       commence par une parenthese */
    tr->from = calloc(sizeof(char), i);
    if (!(tr->from)) {
        print_error_msg("from state string could not be allocated");
        free(tr); /* il faut rendre la memoire de tr pour ne pas la perdre */
        return NULL;
    }

    memcpy(tr->from, line + 1, i - 1);
    tr->from[i++] = '\0'; /* en incrementant i pour sauter la virgule */

    tr->in = line[i++];

    if( i+4>len          ||
        line[i++] != ')' ||
        line[i++] != '-' ||
        line[i++] != '>' ||
        line[i++] != '(' ){
        print_error_msg("invalid transition format excepted ')->('");
        free(tr->from);
        free(tr);
        return NULL;
    }

    saved_i = i--; /* saved_i est sur le premier caractère de l'etat suivant */

    while(line[++i] != ',' && i + 1 < len)
        ;

    if (i > len) {
        print_error_msg("invalid state format, excepted comma");
        free(tr->from);
        free(tr);
        return NULL;
    }

    /* i est sur la virgule */

    tr->to = calloc(sizeof(char), i - saved_i + 1);
    if (!(tr->to)) {
        print_error_msg("to state string could not be allocated");
        free(tr->from);
        free(tr);
        return NULL;
    }

    memcpy(tr->to, line + saved_i, i - saved_i);
    tr->to[i - saved_i] = '\0';

    tr->out = line[++i];
    if(i+3 >= len || line[++i] != ',' || line[i+2] != ')'){
        print_error_msg("invalid transition format, excepted 'out,dir)'");
        free_transition(tr);
        return NULL;
    }

    switch(line[++i]){
    case 'D':
        tr->dir = D;
        break;
    case 'G':
        tr->dir = G;
        break;
    case 'S':
        tr->dir = S;
        break;
    default :
        print_error_msg("invalid direction symbol, excepted G, S or D");
        free_transition(tr);
        return NULL;
    }

    return tr;
}

struct list { char val; struct list *next; };

typedef struct {
    transition **transitions;
    char *accept_state;
    char *reject_state;
    struct list *before;
    struct list *after;
    char *current_state;
    int no_of_transitions;
    char current_cell;
} turing_machine;

void move_left (turing_machine );
void move_right (turing_machine );
struct list *make_list (char, struct list *);
void free_list (struct list *);
error_code execute(char *, char *);

struct list *make_list (char val, struct list *next) {
    struct list *l = malloc(sizeof(struct list));
    l->val = val;
    l->next = next;
    return l;
}

void free_list (struct list *l) {
    struct list *next;
    while (l) {
        next = l->next;
        free(l);
        l = next;
    }
}

/**
 *`Libère la machine de turing, l'étant courant n'est pas libéré par
 * cette fonction
 * @param tm une machine de turing
 */
void free_turing_machine (turing_machine tm) {
    int i;

    for (i = 0; i < tm.no_of_transitions; i++)
        free(tm.transitions[i]);
    free(tm.transitions);

    free(tm.accept_state);
    free(tm.reject_state);
    free_list(tm.before);
    free_list(tm.after);
}

/**
 * Ex.7: Execute la machine de turing dont la description est fournie
 * @param machine_file le fichier de la description
 * @param input la chaîne d'entrée de la machine de turing
 * @return le code d'erreur
 */
error_code execute(char *machine_file, char *input) {
    char *line;
    int len;
    struct list *l; // le ruban
    turing_machine tm; /* pas necessaire de mettre la structure dans le heap */
    transition *trans;
    FILE *fp = fopen(input, "r");
    int i;

    /* count the lenght of the first line of the file */
    len = count_in_file(fp, 0);
    if (len < 0) {
        print_error_msg("could not count line lenght");
        fclose(fp);
        return ERROR;
    }

    /* the len is used to decide length of line
       the current state is set in data struct */
    line = calloc(sizeof(char) ,len + 1);
    if (!line) {
        print_error_msg("could not allocate initial state string");
        fclose(fp);
        return ERROR;
    }
    tm.current_state = line;
    readline(fp, &(tm.current_state), len);

    /* accept state is set */
    len = count_in_file(fp, 0);
    if (len < 0) {
        print_error_msg("could not count line lenght");
        fclose(fp);
        free(line);
        return ERROR;
    }

    tm.accept_state = calloc(sizeof(char) ,len + 1);
    if (!(tm.accept_state)) {
        print_error_msg("could not allocate accept state string");
        fclose(fp);
        free(line);
        return ERROR;
    }
    readline(fp, &(tm.accept_state), len);

    /* for reject state */
    len = count_in_file(fp, 0);
    if (len < 0) {
        print_error_msg("could not count line lenght");
        fclose(fp);
        free(line);
        free(tm.accept_state);
        return ERROR;
    }

    tm.reject_state = calloc(sizeof(char) ,len + 1);
    if (!(tm.reject_state)) {
        print_error_msg("could not allocate reject state string");
        fclose(fp);
        free(line);
        free(tm.accept_state);
        return ERROR;
    }
    readline(fp, &(tm.reject_state), len);

    /* here we check when is the end of the array and nbr of transitions */
    tm.no_of_transitions = no_of_lines(fp) - 3;
    tm.transitions = malloc(sizeof(transition*) * tm.no_of_transitions);
    if (!(tm.transitions)) {
        print_error_msg("could not allocate transitions pointers");
        fclose(fp);
        free(line);
        free(tm.accept_state);
        free(tm.reject_state);
        return ERROR;
    }

    /* TODO: utiliser parse_line pour remplir tm.transitions, si le string line
 est utilisé, il faut remettre tm.current_state dedans à la fin pour ne pas
       leak de memoire ensuite lors de l'execution de la machine */
    for(i=0; i<tm.no_of_transitions; i++){
        /* count the lenght of the first line of the file */
        len = count_in_file(fp, 0);
        if (len < 0) {
            print_error_msg("could not count line lenght");
            fclose(fp);
            free(line);
            free(tm.accept_state);
            free(tm.reject_state);
            while(--i>=0) free(tm.transitions[i]);
            free(tm.transitions);
            return ERROR;
        }

        /* the len is used to decide length of line
           the current state is set in data struct */
        line = calloc(sizeof(char) ,len + 1);
        if (!line) {
            print_error_msg("could not allocate initial state string");
            fclose(fp);
            free(line);
            free(tm.accept_state);
            free(tm.reject_state);
            while(--i>=0) free(tm.transitions[i]);
            free(tm.transitions);
            return ERROR;
        }
        tm.transitions[i] = parse_line(line, len);
        if (!(tm.transitions[i])) {
            print_error_msg("couldnt get transition properly");
            fclose(fp);
            free(line);
            free(tm.accept_state);
            free(tm.reject_state);
            while(--i>=0) free(tm.transitions[i]);
            free(tm.transitions);
            return ERROR;
        }
    }

    // now I have to put back to variable line the string of current-state
    line = tm.current_state;

    l = NULL; /*just in case */

    // cree le ruban mais a l'envers
    while (*input)
        l = make_list(*(input++), l);

    tm.current_cell = l->val;
    tm.after = NULL;
    // used as a temp var
    tm.before = l;
    l = l->next;
    free(tm.before);

    // revire la liste de bord
    while (l) {
        tm.after = make_list(tm.current_cell, tm.after);
        tm.current_cell = l->val;
        // used as temp var
        tm.before = l;
        l = l->next;
        free(tm.before);
    }
    // aftr
    fclose(fp);

    // 6. Exécuter la machine
    // 7. Si on atteint éventuellement un état acceptant ou rejetant, terminer l'exécution de la machine
    // pendant que la machine s'execute faut pas faire malloc sur le bfr
    while(1){
        //check what happens for currentState/currentInput
        for(i = 0; i < tm.no_of_transitions; i++){

            trans = tm.transitions[i];

            if(strcmp(tm.current_state,trans->from)==0 &&
               tm.current_cell == trans->in){

                tm.current_cell = trans->out;
                tm.current_state = trans->to;

                if(trans->dir==-1) move_left(tm);
                else if(trans->dir==1) move_right(tm);
                break;
            }
        }

        if(strcmp(tm.current_state,tm.accept_state)==0){
            return 1;
        }
        if(strcmp(tm.current_state,tm.reject_state)==0){
            return 0;
        }
    }

    /* ces 2 object doivent être libéré avant de retourner le résultat */
    free_turing_machine(tm);
    free(line);

    return ERROR;
}

void move_left (turing_machine tm){
    struct list *next;
    if(!tm.before) return;
    next = tm.before;
    tm.before = next->next;
    tm.after = make_list(tm.current_cell, tm.after);
    tm.current_cell = next->val;
    free(next);
    return;
}

void move_right (turing_machine tm){
    struct list *next;
    if(!tm.after) next = make_list('\0', NULL);
    else next = tm.after;
    tm.before = make_list(tm.current_cell, tm.before);
    tm.current_cell = next->val;
    tm.after = next->next;
    free(next);
    return;
}

int main() {
// vous pouvez ajouter des tests pour les fonctions ici
    FILE *fp;
    char *str, *str2;
    transition *tr;
    byte clean_stage = 0;

    int i = 1;
    //execute("simple.txt","0010");
#define test(v) if (!(v)) {\
        fprintf(stderr, "test #%d failed\n", i); \
        goto clean; } i++

    fp = fopen("simple.txt", "r");
    if (!fp) {
        print_error_msg("can't open file 'simple.txt'");
        return 1;
    }
    str = malloc(sizeof(char) * 32);
    str2 = malloc(sizeof(char) * 32);

    test(strlen("abca") == 4); /* 1 : strlen*/
    test(readline(fp, &str, 32) == 2); /* 2 : readline return value */
    test(strcmp(str, "q0") == 0); /* 3 : readline reading */
    test(readline(fp, &str, 32) == 2); /* 4 : readline return value #2 */
    test(strcmp(str, "qA") == 0); /* 5 readline reading #2 */
    test(no_of_lines(fp) == 5); /* 6 : no_of_lines not changing file position */
    test(readline(fp, &str, 32) == 2); /* 7 : readline return value #3 */
    test(strcmp(str, "qR") == 0); /* 8 : readline reading #3 */
    memcpy(str, "astroblaze", 5);
    str[5] = '\0';
    test(strcmp(str, "astro") == 0); /* 9 : memcpy substring */
    memcpy(str2, str, strlen(str) + 10);
    test(strcmp(str, str2) == 0); /* 10 : memcpy n over strlen */

    readline(fp, &str, 32);
    test(tr = parse_line(str, strlen(str))); /* 11 */
    clean_stage++;
    test(strcmp(tr->from, "q0") == 0); /* 12 */
    test(tr->in == '1'); /* 13 */
    test(strcmp(tr->to, "qA") == 0); /* 14 */
    test(tr->out == '0'); /* 15 */
    test(tr->dir == S); /* 16 */

 clean:
    fclose(fp);
    free(str);
    free(str2);
    if (clean_stage > 0)
        free_transition(tr);

    return 0;
}

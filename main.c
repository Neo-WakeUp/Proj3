/* **************************************************** */
/*               Jednoducha zhlukova analyza            */
/*                                                      */
/*                        Version : 1                   */
/*                                                      */
/*                     Vladimír Lovíšek                 */
/*                       December 2016                  */
/* **************************************************** */
/**
* Kostra programu pro 3. projekt IZP 2015/16
*
* Jednoducha shlukova analyza
* Complete linkage
* http://is.muni.cz/th/172767/fi_b/5739129/web/web/clsrov.html
*/
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);
    if (cap == 0)
    {
        c->obj = NULL;
        c->size = 0;
    }
    else
    {
        c->obj = malloc(cap * sizeof(struct obj_t));
        c->size = cap;
    }
    c->capacity = cap;
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
   // for (int i = 0; i < c->capacity;i++)
    {
        free(c->obj);
    }
    init_cluster(c,0);
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    if (c->size < c->capacity)
    {
        c->obj[c->size] = obj;
        c->size = c->size +1;
    }else
    if (c->size == c->capacity)
    {
        c = resize_cluster(c, c->capacity+1);
        c->obj[c->size] = obj;
        c->size = c->size +1;
    }
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

    if (c1->size + c2->size > c1->capacity)
    {
        c1 = resize_cluster(c1, c1->size + c2->size);
    }
    for(int i = 0; i < c2->size; i++)
        append_cluster(c1, c2->obj[i]);
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);

    narr--;
    int i = 0;
    clear_cluster(&carr[idx]);
    for (i = idx; i < narr; i++)
        carr[i] = carr[i+1];
    return narr;
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);
    float x = fabsf(o1->x - o2->x);
    float y = fabsf(o1->y - o2->y);
    float distance = x*x + y*y;
    return sqrtf(distance);
}

/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    int i = 0;
    int j = 0;
    float o_distance = 0;
    for (i = 0; i < c1->size; i++)
    {
        for (j = 0; j < c2->size; j++)//porovna vzdialenost c1->obj[i] so vsetkymi objektami c2
        {
            if(obj_distance(&c1->obj[i], &c2->obj[j]) > o_distance) //zapise do o_distance najvacsiu vzdialenost
                o_distance = obj_distance(&c1->obj[i], &c2->obj[j]);
        }
    }
    return o_distance;
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);
    int i = 0;
    int j = 0;
    int idx1 = -1;
    int idx2 = -1;
    float c_distance = INFINITY;//dostane hodnotu vacsiu ako navecsi mozna vzdialenost zhlukov v danom priestore
    for (i = 0; i < narr; i++)
    {
        for (j = 0; j < narr; j++)//porovna vzdialenost zhluku i so vsetky ostatnymi zhlukmi
        {
            if(cluster_distance(&carr[i], &carr[j]) < c_distance && i != j)
            {
                c_distance = cluster_distance(&carr[i], &carr[j]);
                idx1 = i;
                idx2 = j;
            }
        }

    }
    *c1 = idx1;
    *c2 = idx2;
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = a;
    const struct obj_t *o2 = b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');

        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}
/*
 Zo 'string' precita pocet objektov alebo informacie o objekte
 Vracia pocet objektov alebo nenulovu ak cita objekt, pri chybe vracia 0
*/
int read_line(char *string, int *obj_id, float *obj_x, float *obj_y)
{
    int count = 0;
    float err_check = 0;
    if(obj_id == NULL)
    {
        if((sscanf(string,"count=%d %f",&count, &err_check)) != 1 || !isdigit(string[6]))
        {
            fprintf(stderr,"Nespravny format vstupneho suboru.\n");
            return 0;
        }else
        {
            int i = 6;
            while (string[i] != EOF && string[i] != '\n' && string[i] != '\0')
            {
                if(!isdigit(string[i]) && !isspace(string[i]))
                {
                    fprintf(stderr,"Nespravny format vstupneho suboru.\n");
                    return 0;
                }
                i++;
            }
        }
        return count;
    }else
    {
        if ((sscanf(string,"%d %f %f %f", obj_id, obj_x, obj_y, &err_check) != 3))
        {
            fprintf(stderr,"Nespravny format vstupneho suboru.\n");
            return 0;
        }else
        {
            int j = 0;
            while(string[j] != EOF && string[j] != '\n' && string[j] != '\0')
            {
                if(!isdigit(string[j]) && !isspace(string[j]) && string[j] != '.')
                {
                    fprintf(stderr,"Nespravny format vstupneho suboru.\n");
                    return 0;
                }else
                if(string[j] == '.' && string[j+1] == '.')
                {
                    fprintf(stderr,"Nespravny format vstupneho suboru.\n");
                    return 0;
                }
                j++;
            }
        }
    }
    return 42;
}
/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)//TO DO osetrit fopen, fgets
{
    assert(arr != NULL);

    int obj_count = 0;
    int count = 0;
    char line[20];
    FILE *fp;
    fp = fopen(filename,"r");
    if(fp == NULL)
    {
        fprintf(stderr,"Nepodarilo sa otvorit vstupny subor.\n");
        *arr = NULL;
        return 0;
    }
    if(fgets(line, 19,fp) == NULL)
    {
        fprintf(stderr,"Chyba pri citani vstupneho suboru.\n");
        *arr = NULL;
        fclose(fp);
        return 0;
    }
    if ((count = read_line(line, NULL, NULL, NULL)) == 0)
    {
        *arr = NULL;
        return 0;
    }
    struct cluster_t *carr;
    carr = malloc(count * sizeof(struct cluster_t));//Alokuje miesto pre pole zhlukov

    for (int i = 0; i < count; i++)
    {
        init_cluster(&carr[i],1);
        if(carr[i].obj == NULL)
        {
            fprintf(stderr,"Chyba pri alokacii.\n");
            *arr = NULL;
            fclose(fp);
            break;
        }
        if(fgets(line, 19,fp) == NULL)
        {
            fprintf(stderr,"Chyba pri citani vstupneho suboru.\n");
            *arr = NULL;
            fclose(fp);
            break;
        }
        if (read_line(line, &carr[i].obj->id, &carr[i].obj->x, &carr[i].obj->y) == 0)
        {
            *arr = NULL;
            fclose(fp);
            break;
        }
        obj_count++;
    }
    if (count != obj_count)//nepodarilo sa ancitat vsetky objekty
    {
        for(int i = 0; i <= obj_count; i++)
            clear_cluster(&carr[i]);
        free(carr);
        return obj_count;
    }
    *arr = carr;
    fclose(fp);
    return obj_count;
}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}
int main(int argc, char *argv[])
{
    struct cluster_t *clusters;
    if (argc != 3)
    {
        fprintf(stderr,"Nespravne vstupne argumenty.\n");
        return 1;
    }

    int cluster_count = load_clusters(argv[1], &clusters);
    if (clusters == NULL)
        return 1;

    char *err_check;
    int cluster_target = strtol(argv[2], &err_check, 10);//Pocet zhlukov ktory chceme poslat na vystup
    if(err_check == '\0')
    {
        fprintf(stderr,"Nespravne vstupne argumenty.\n");
        return 1;
    }
    if(cluster_target == 0) cluster_target = 1;

    int c_indx1 = -1;
    int c_indx2 = -1;

    for(int i = cluster_count; i > cluster_target;i--)//Opakuje dokym nemame pozadovany pocet zhlukov
    {
        find_neighbours(clusters, cluster_count, &c_indx1, &c_indx2);
        merge_clusters(&clusters[c_indx1], &clusters[c_indx2]);
        sort_cluster(&clusters[c_indx1]);
        cluster_count = remove_cluster(clusters, cluster_count, c_indx2);
    }
    print_clusters(clusters, cluster_count);
    for (int i = 0; i < cluster_count; i++)//uvolnenie pameti allkovovanej pre objekty zhlukov
    {
        clear_cluster(&clusters[i]);
    }
    free(clusters);//uvolnenie pameti allokovanej pre pole zhlukov
    return 0;
}

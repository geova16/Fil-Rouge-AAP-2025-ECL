
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define MAX_READ_LINE 100



typedef int t_bool;    // Booléen
typedef int t_vertex;  // Sommet de graphe

// Maillon de liste chaînée
typedef struct node {
  t_vertex val;          // Valeur (sommet)
  struct node * p_next;  // Pointeur vers le maillon suivant
} t_node;
// Liste chaînée (de sommets)
typedef t_node * t_list;
// Pile (de sommets)
typedef t_list t_stack;


//////// Piles ////////
void stack_show(t_stack * ps);
t_stack * stack_new();
int stack_is_empty(t_stack * ps);
void stack_push(t_vertex e, t_stack * ps);
t_vertex stack_pop(t_stack * ps);
t_vertex stack_top(t_stack * ps);

//////// Listes chaînées ////////
t_list list_new();
t_list list_add_head(t_vertex e, t_list l);
int list_is_empty(t_list l);
void list_show(t_list l);
t_vertex list_get_first_val(t_list l);
t_list list_remove_head(t_list l);
t_list list_free(t_list l);
t_node * list_cursor_new(t_list l);
int list_cursor_at_end(t_node * lc);
int list_cursor_get_val(t_node * lc);
t_node * list_cursor_next(t_node * lc);


////////Algorithme de Kosaraju ////////

int order[1000];
int step;

void transpose_graph(t_list *g, t_list *gt, int nbr_sommets);
void enum_cfc_kosaraju(t_list * l);
void Kosaraju_1(t_list *adj, int nbr_sommets);
int Kosaraju_1_recur(t_list *adj, int x, t_bool marking[], int order[], int step);
void Kosaraju_2(t_list *adj, int order[], int nbr_sommets);
int Kosaraju_2_recur(t_list *adj, int x, t_bool marking[]);

int main(){
    // Exemple du cours 
  t_list L[4];
  for (int i = 0; i < 4; i++)
    L[i] = list_new();

  L[0] = list_add_head(2, L[0]);
  L[0] = list_add_head(1, L[0]);
  L[2] = list_add_head(3, L[2]);
  L[2] = list_add_head(1, L[2]);
  L[3] = list_add_head(2, L[3]);

  enum_cfc_kosaraju(L);
}


void enum_cfc_kosaraju(t_list * l){
  int nbr_sommets=4; // nombre de sommets du graphe à changer selon le graphe utilisé 

  Kosaraju_1(l,nbr_sommets);
  t_list gt[nbr_sommets];
  transpose_graph(l, gt, nbr_sommets);
  Kosaraju_2(gt, order, nbr_sommets);
    
}

void Kosaraju_1(t_list *l, int nbr_sommets){ // on a besoin de connaitre la taille de la liste
    t_bool marking[nbr_sommets];  
    int x; 
    step=0; 
    int i;
    for (i=0; i<nbr_sommets; i++)
        marking[i]=0; 
    for (x=0; x<nbr_sommets; x++){
        step=Kosaraju_1_recur(l, x, marking, order, step);
    }
}
int Kosaraju_1_recur(t_list *l, int x, t_bool marking[], int order[], int step){
    int y;
    if (marking[x]==0){
        marking[x]=1;
        t_node *lc = list_cursor_new(l[x]); // curseur pour parcourir les successeurs de x
        while(list_cursor_at_end(lc)!=1){ // On cherche tous les successeurs de x
            y=list_cursor_get_val(lc); // Un successeur y de x
            step=Kosaraju_1_recur(l, y, marking, order, step);
            lc = list_cursor_next(lc); // On passe au successeur suivant
        }
        order[x]=step; 
        step++;
    }
    return step;
}


void Kosaraju_2(t_list *l, int order[], int nbr_sommets){ // on a besoin de connaitre la taille de la liste
    t_bool marking[nbr_sommets];
    int x;
    int inv_order[nbr_sommets];
    int nb_scc=0;
    int i;
    
    for (i=0; i<nbr_sommets; i++)
        marking[i]=0;
    
    for (x=0; x<nbr_sommets; x++)
        inv_order[(nbr_sommets-1)-order[x]]=x;

    for (x=0; x<nbr_sommets; x++){
        if (Kosaraju_2_recur(l,inv_order[x], marking)==1) {
            nb_scc++;
            printf("\n");
        }
    }
    printf("%d composantes fortement connexes trouvées\n", nb_scc);

}

int Kosaraju_2_recur(t_list *l, int x, t_bool marking[]){
    int y; 
    if (marking[x]==1){
        return 0; 
    }
    else {
        marking[x]=1;
        printf("%d ",x);
        t_node *lc = list_cursor_new(l[x]);      // curseur pour parcourir les successeurs de x
        while(list_cursor_at_end(lc)!=1){       // On cherche tous les successeurs de x
            y=list_cursor_get_val(lc);         // Un successeur y de x
            Kosaraju_2_recur(l, y, marking);
            lc = list_cursor_next(lc);       // On passe au successeur suivant
        }
        return 1;
    }    

}

// fonction de transposition d'un graphe représenté par des listes d'adjacence
void transpose_graph(t_list *l, t_list *gt, int nbr_sommets){
  int i;

  for(i=0;i<nbr_sommets;i++) 
    gt[i]=list_new(); // initialisation des listes du graphe transposé

  for(i=0; i<nbr_sommets; i++){
    t_node *cur = list_cursor_new(l[i]); // curseur pour parcourir les successeurs de i
    while(list_cursor_at_end(cur)!=1){
      gt[list_cursor_get_val(cur)] = list_add_head(i, gt[list_cursor_get_val(cur)]); // inversion
      cur = list_cursor_next(cur); // on passe au successeur suivant
      }
    }
}



//////// Piles ////////

// Crée une nouvelle pile vide
// Utilisation : t_stack * ps = stack_new();
t_stack * stack_new() {
  t_stack * ps = malloc(sizeof(*ps));
  assert(ps != NULL);
  *ps = list_new();
  return ps;
}

// Affiche tout le contenu d'une pile (pour le débogage seulement)
void stack_show(t_stack * ps) {
  list_show(*ps);
}

// Retourne 1 si la pile *ps est vide, 0 sinon
int stack_is_empty(t_stack * ps) {
  return list_is_empty(*ps);
}

// Empile la valeur e en haut de la pile *ps
void stack_push(t_vertex e, t_stack * ps) {
  *ps = list_add_head(e, *ps);
}

// Dépile et retourne la valeur en haut de la pile *ps
t_vertex stack_pop(t_stack * ps) {
  t_vertex res = stack_top(ps);
  *ps = list_remove_head(*ps);
  return res;
}

// Retourne la valeur en haut de la pile *ps sans la dépiler
t_vertex stack_top(t_stack * ps) {
  assert(!list_is_empty(*ps));
  return list_get_first_val(*ps);
}



//////// Listes chaînées ////////

// Crée une nouvelle liste chaînée
// Utilisation : t_list l = list_new();
t_list list_new() {
  return NULL;
}

// Ajoute un maillon avec la valeur e au début de la liste l
t_list list_add_head(t_vertex e, t_list l) {
  t_node * n = malloc(sizeof(t_node));
  assert(n != NULL);
  n->val = e;
  n->p_next = l;
  return n;
}

// Retourne 1 si la liste l est vide, 0 sinon
int list_is_empty(t_list l) {
  return l == NULL;
}

// Affiche la liste l
void list_show(t_list l) {
  t_node * n = l;
  int i = 0;
  while (n != NULL) {
    printf("Maillon %d, valeur = %d\n", i++, n->val);
    n = n->p_next;
  }
}

// Retourne la valeur du premier maillon de la liste l
t_vertex list_get_first_val(t_list l) {
  assert(l != NULL);
  return l->val;
}

// Retire le premier maillon de la liste l et retourne la nouvelle tête
// Utilisation : l = list_remove_head(l);
t_list list_remove_head(t_list l) {
  t_list l2;
  assert(l != NULL);
  l2 = l->p_next;
  free(l);
  return l2;
}

// Libère la liste l de la mémoire (maillon par maillon)
// Utilisation : l = list_free(l);
t_list list_free(t_list l) {
  t_node * n = l, * nNext;
  while (n != NULL) {
    nNext = n->p_next;
    free(n);
    n = nNext;
  }
  return NULL;
}

// Retourne un curseur de liste pour parcourir l
// Utilisation : t_node * lc = list_cursor_new(l);
// Remarque : en fait, un curseur est aussi une liste chaînée !
t_node * list_cursor_new(t_list l) {
  return l;
}

// Retourne 1 si la fin de la liste a été atteinte
// Remarque : la fin d'une liste (NULL) est la même chose qu'une liste vide !
int list_cursor_at_end(t_node * lc) {
  return list_is_empty(lc);
}

// Retourne la valeur du maillon actuellement pointé par lc
// Remarque : ça revient en fait à prendre la valeur du premier maillon de lc !
int list_cursor_get_val(t_node * lc) {
  return list_get_first_val(lc);
}

// Avance le curseur vers le maillon suivant
// Utilisation : lc = list_cursor_next(lc);
t_node * list_cursor_next(t_node * lc) {
  assert(lc != NULL);
  return lc->p_next;
}







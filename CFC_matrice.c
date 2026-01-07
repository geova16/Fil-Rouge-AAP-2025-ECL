#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define MAX_READ_LINE 100

typedef int t_bool;
typedef struct {
  int size;    
  t_bool ** m; 
} t_graph;

// Variables globales 
int order[MAX_READ_LINE]; // tableau stockant l'ordre de passage des noeuds
int step;

// Prototypes
int Kosaraju_1_recur(t_graph g, int x, t_bool marking[], int order[], int step);
void Kosaraju_1(t_graph g);
int Kosaraju_2_recur(t_graph g, int x, t_bool marking[]);
void Kosaraju_2(t_graph g, int order[]);
void enum_cfc_kosaraju(t_graph g);

int main(){
   // Test de l'algorithme de Kosaraju sur l'exemple du cours
    t_graph g; 
    g.size = 4; 
    g.m = malloc(g.size * sizeof(t_bool*)); 
    for (int i=0; i<g.size; i++)
        g.m[i] = malloc(g.size * sizeof(t_bool)); 

    int mat[4][4] = {
        {0,1,1,0},
        {0,0,0,0},
        {0,1,0,1},
        {0,0,1,0}
    }; // matrice d'adjacence de l'exemple du cours

    for (int i=0; i<g.size; i++)
        for (int j=0; j<g.size; j++)
            g.m[i][j] = mat[i][j]; 

    enum_cfc_kosaraju(g);
    return 0;
}

void enum_cfc_kosaraju(t_graph g){
    Kosaraju_1(g);
    t_graph gi; // calcul du graphe transposé 
    gi.size = g.size;
    gi.m = malloc(gi.size * sizeof(t_bool*));
    for (int i = 0; i < gi.size; i++) {
        gi.m[i] = malloc(gi.size * sizeof(t_bool));
        for (int j = 0; j < g.size; j++)
            gi.m[i][j] = g.m[j][i]; // transposition
    }

    Kosaraju_2(gi, order);

}

// Parcours en profondeur 
void Kosaraju_1(t_graph g){ 
    t_bool marking[g.size]; // tableau de marquage
    int i;
    int x; // un sommet de g 
    step = 0; // compteur de l'ordre de passage

    for(i=0;i<g.size;i++) // initialisation du tableau à 0 car aucun sommet visité pour le moment 
        marking[i]=0;
    for(x=0;x<g.size;x++) // parcours de tous les sommets de g
        step = Kosaraju_1_recur(g, x, marking, order, step); // appel de la fonction récursive
}

int Kosaraju_1_recur(t_graph g, int x, t_bool marking[], int order[], int step){
    int y; // un sommet de g
    if(marking[x]==0){ // si le sommet n'a pas encore été visité
        marking[x]=1;
        for(y=0;y<g.size;y++)
            if(g.m[x][y]==1) // condition pour être successeur
                step = Kosaraju_1_recur(g, y, marking, order, step);
        order[x] = step;
        step++;
    }
    return step;
}

//Parcours en profondeur sur le graphe transposé
void Kosaraju_2(t_graph g, int order[]){
    t_bool marking[g.size]; // tableau de marquage
    int i;
    int inv_order[g.size]; // tableau de l'ordre inverse
    int nb_scc = 0; // nombre de composantes fortement connexes
    int x; // un sommet de g

    for(i=0;i<g.size;i++) 
        marking[i]=0; // initialisation du tableau à 0 car aucun sommet visité pour le moment
    
    for(x=0;x<g.size;x++)   
        inv_order[g.size-1-order[x]] = x; // calcul de l'ordre inverse


    for(x=0;x<g.size;x++){      
        if(Kosaraju_2_recur(g, inv_order[x], marking)==1){
            nb_scc++;
            printf("\n");
        }
    }
    printf("%d composantes fortement connexes trouvées\n", nb_scc);
}

int Kosaraju_2_recur(t_graph g, int x, t_bool marking[]){
    int y; // un sommet de g

    if(marking[x]==1) // si le sommet a déjà été visité
        return 0;
        
    marking[x] = 1;
    printf("%d ", x);
    for(y=0;y<g.size;y++)
        if(g.m[x][y]==1) // condition pour être successeur
        Kosaraju_2_recur(g, y, marking);
    return 1;
}

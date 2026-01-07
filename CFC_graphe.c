#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#define MAX_READ_LINE 10000

typedef int t_bool;

// Variables globales
int order[MAX_READ_LINE]; 
int step;

// Prototypes : la signature a changé car "graph.h" "renvoie" un graphe de type t_graph* et non plus comme dans l'énoncé 
int Kosaraju_1_recur(t_graph *g, int x, t_bool marking[], int order[], int step);
void Kosaraju_1(t_graph *g);
int Kosaraju_2_recur(t_graph *g, int x, t_bool marking[]);
void Kosaraju_2(t_graph *g, int order[]);
void enum_cfc_kosaraju(t_graph *g);

int main(){
    // Lecture du graphe depuis fichier
    t_graph *g = graph_read_format1(".txt", 0); // Changer le chemin du fichier selon l'emplacement. Changer la valeur: 1 matrice d'adjacence et 0 liste d'adjacence

    enum_cfc_kosaraju(g);
    return 0;
}

// Fonction principale Kosaraju
void enum_cfc_kosaraju(t_graph *g){
    int n = graph_size(g);
    Kosaraju_1(g);

    // Construction du graphe transposé
    t_graph *gi = graph_new(n, 0, 1); 
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            if(graph_has_edge(g,i,j)==1){
                graph_add_edge(gi,j,i); 
            }
        }
    }
    Kosaraju_2(gi, order);


}

// Parcours en profondeur pour l'ordre de fin
void Kosaraju_1(t_graph *g){
    int n = graph_size(g);
    t_bool marking[n];
    step=0;
    int x; 
    int i;

    for(i=0;i<n;i++)
        marking[i]=0;
    for(x=0; x<n; x++){
        step = Kosaraju_1_recur(g, x, marking, order, step);
    }
}

int Kosaraju_1_recur(t_graph *g, int x, t_bool marking[], int order[], int step){
    int n = graph_size(g);
    int y;

    if(marking[x]==0){
        marking[x] = 1;
        for(y=0; y<n; y++){
            if(graph_has_edge(g,x,y)==1){
                step = Kosaraju_1_recur(g, y, marking, order, step);
            }
        }
        order[x] = step;
        step++;
    }
    return step;
}

// Deuxième DFS sur le graphe transposé
void Kosaraju_2(t_graph *g, int order[]){
    int n = graph_size(g);
    t_bool marking[n];
    int inv_order[n];
    int nb_scc = 0;
    int i;
    int x;

    for(i=0;i<n;i++)
        marking[i] = 0;
        
    for(x=0;x<n;x++)
        inv_order[n-1-order[x]] = x;

    for(x=0;x<n;x++){
        if(Kosaraju_2_recur(g,inv_order[x],marking)==1){
            nb_scc++;
            printf("\n");
        }
    }

    printf("%d composantes fortement connexes trouvées\n", nb_scc);
}

int Kosaraju_2_recur(t_graph *g, int x, t_bool marking[]){
    int n = graph_size(g);
    int y;

    if(marking[x]==1) 
        return 0;

    marking[x] = 1;
    printf("%d ", x);
    for(y=0; y<n; y++){
        if(graph_has_edge(g,x,y)==1){
            Kosaraju_2_recur(g,y,marking);
        }
    }
    return 1;
}


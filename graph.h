#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>

typedef int t_bool;    // Booléen
typedef int t_vertex;  // Sommet du graphe

// Type de graphe opaque
typedef struct graph t_graph;

// Informations de base
t_graph * graph_new(int size, t_bool with_names, t_bool use_matrix);
void graph_free(t_graph * g);
int graph_size(const t_graph * g);
t_bool graph_has_names(const t_graph * g);
const char * graph_vertex_name(const t_graph * g, t_vertex v);
t_vertex graph_vertex_from_name(const t_graph * g, const char * name);

// Opérations sur les arêtes
void graph_add_edge(t_graph * g, t_vertex from, t_vertex to);
t_bool graph_has_edge(const t_graph * g, t_vertex from, t_vertex to);

// Parcours / graphe transposé
void graph_for_each_succ(const t_graph * g, t_vertex u, void (*f)(t_vertex v, void * ctx), void * ctx);
t_graph * graph_reverse(const t_graph * g);
void graph_show(const t_graph * g);

// Lecture de graphe (format 1 : numéros ; format 2 : noms)
t_graph * graph_read_format1_file(FILE * in, t_bool use_matrix);
t_graph * graph_read_format2_file(FILE * in, t_bool use_matrix);
t_graph * graph_read_format1(const char * filename, t_bool use_matrix);
t_graph * graph_read_format2(const char * filename, t_bool use_matrix);

#endif // GRAPH_H

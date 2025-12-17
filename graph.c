#include "graph.h"

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define MAX_READ_LINE 100

// Liste chaînée interne (pour les représentations en listes d'adjacence)
typedef struct node {
  t_vertex val;
  struct node * p_next;
} t_node;
typedef t_node * t_list;

static t_list list_add_head(t_vertex e, t_list l) {
  t_node * n = malloc(sizeof(t_node));
  assert(n != NULL);
  n->val = e;
  n->p_next = l;
  return n;
}

static t_list list_free(t_list l) {
  t_node * n = l, * nNext;
  while (n != NULL) {
    nNext = n->p_next;
    free(n);
    n = nNext;
  }
  return NULL;
}

// Définition de la structure de graphe (cachée dans ce fichier)
struct graph {
  int size;
  t_bool use_matrix; // 1 : matrice d'adjacence ; 0 : listes d'adjacence
  union {
    t_list * adj;   // Tableau de listes d'adjacence
    t_bool ** m;    // Matrice d'adjacence dynamique
  } repr;
  char ** names;    // Noms de sommets (optionnel)
};

// Fonctions utilitaires
static void trim_trailing(char * s) {
  size_t len = strlen(s);
  while (len > 0 && isspace((unsigned char)s[len - 1])) {
    s[--len] = '\0';
  }
}

static int read_line_skip_empty(FILE * f, char * buf, size_t len) {
  while (fgets(buf, (int)len, f) != NULL) {
    trim_trailing(buf);
    const char * p = buf;
    while (*p && isspace((unsigned char)*p)) {
      p++;
    }
    if (*p != '\0') {
      return 1;
    }
  }
  return 0;
}

static char * str_dup(const char * s) {
  size_t len = strlen(s) + 1;
  char * copy = malloc(len);
  assert(copy != NULL);
  memcpy(copy, s, len);
  return copy;
}

// Création / libération
t_graph * graph_new(int size, t_bool with_names, t_bool use_matrix) {
  assert(size > 0);
  t_graph * g = malloc(sizeof(*g));
  assert(g != NULL);
  g->size = size;
  g->use_matrix = use_matrix;
  if (use_matrix) {
    g->repr.m = calloc((size_t)size, sizeof(*(g->repr.m)));
    assert(g->repr.m != NULL);
    for (int i = 0; i < size; i++) {
      g->repr.m[i] = calloc((size_t)size, sizeof(*(g->repr.m[i])));
      assert(g->repr.m[i] != NULL);
    }
  } else {
    g->repr.adj = calloc((size_t)size, sizeof(*(g->repr.adj)));
    assert(g->repr.adj != NULL);
  }
  g->names = with_names ? calloc((size_t)size, sizeof(*(g->names))) : NULL;
  if (with_names) {
    assert(g->names != NULL);
  }
  return g;
}

void graph_free(t_graph * g) {
  if (g == NULL) return;
  if (g->use_matrix) {
    for (int i = 0; i < g->size; i++) {
      free(g->repr.m[i]);
    }
    free(g->repr.m);
  } else {
    for (int i = 0; i < g->size; i++) {
      g->repr.adj[i] = list_free(g->repr.adj[i]);
    }
    free(g->repr.adj);
  }

  if (g->names != NULL) {
    for (int i = 0; i < g->size; i++) {
      free(g->names[i]);
    }
  }
  free(g->names);
  free(g);
}

int graph_size(const t_graph * g) {
  return g ? g->size : 0;
}

t_bool graph_has_names(const t_graph * g) {
  return (g != NULL && g->names != NULL);
}

const char * graph_vertex_name(const t_graph * g, t_vertex v) {
  if (g == NULL || g->names == NULL) return NULL;
  if (v < 0 || v >= g->size) return NULL;
  return g->names[v];
}

t_vertex graph_vertex_from_name(const t_graph * g, const char * name) {
  if (g == NULL || g->names == NULL || name == NULL) return -1;
  for (int i = 0; i < g->size; i++) {
    if (g->names[i] != NULL && strcmp(g->names[i], name) == 0) {
      return i;
    }
  }
  return -1;
}

// Opérations sur les arêtes
t_bool graph_has_edge(const t_graph * g, t_vertex from, t_vertex to) {
  assert(g != NULL);
  assert(from >= 0 && from < g->size);
  assert(to >= 0 && to < g->size);
  if (g->use_matrix) {
    return g->repr.m[from][to] ? 1 : 0;
  } else {
    for (t_node * n = g->repr.adj[from]; n != NULL; n = n->p_next) {
      if (n->val == to) return 1;
    }
    return 0;
  }
}

void graph_add_edge(t_graph * g, t_vertex from, t_vertex to) {
  assert(g != NULL);
  assert(from >= 0 && from < g->size);
  assert(to >= 0 && to < g->size);
  if (graph_has_edge(g, from, to)) return;
  if (g->use_matrix) {
    g->repr.m[from][to] = 1;
  } else {
    g->repr.adj[from] = list_add_head(to, g->repr.adj[from]);
  }
}

// Parcours des successeurs
void graph_for_each_succ(const t_graph * g, t_vertex u, void (*f)(t_vertex v, void * ctx), void * ctx) {
  assert(g != NULL);
  assert(f != NULL);
  assert(u >= 0 && u < g->size);
  if (g->use_matrix) {
    for (int v = 0; v < g->size; v++) {
      if (g->repr.m[u][v]) {
        f(v, ctx);
      }
    }
  } else {
    for (t_node * n = g->repr.adj[u]; n != NULL; n = n->p_next) {
      f(n->val, ctx);
    }
  }
}

// Graphe transposé
struct reverse_ctx {
  t_graph * rev;
  t_vertex from;
};

static void graph_reverse_cb(t_vertex v, void * ctx) {
  struct reverse_ctx * c = ctx;
  graph_add_edge(c->rev, v, c->from);
}

t_graph * graph_reverse(const t_graph * g) {
  assert(g != NULL);
  t_bool has_names = g->names != NULL;
  t_graph * rev = graph_new(g->size, has_names, g->use_matrix);
  if (has_names) {
    for (int i = 0; i < g->size; i++) {
      rev->names[i] = g->names[i] ? str_dup(g->names[i]) : NULL;
    }
  }

  for (int u = 0; u < g->size; u++) {
    struct reverse_ctx ctx = { rev, u };
    graph_for_each_succ(g, u, graph_reverse_cb, &ctx);
  }

  return rev;
}

// Affichage façon liste d'adjacence
struct show_ctx {
  const t_graph * g;
};

static void graph_show_cb(t_vertex v, void * ctx) {
  const struct show_ctx * c = ctx;
  const char * name = graph_vertex_name(c->g, v);
  if (name) {
    printf(" %s", name);
  } else {
    printf(" %d", v);
  }
}

void graph_show(const t_graph * g) {
  assert(g != NULL);
  for (int i = 0; i < g->size; i++) {
    const char * name = graph_vertex_name(g, i);
    if (name) {
      printf("%s:", name);
    } else {
      printf("%d:", i);
    }
    struct show_ctx ctx = { g };
    graph_for_each_succ(g, i, graph_show_cb, &ctx);
    printf("\n");
  }
}

// Lecture format 1 (numéros)
t_graph * graph_read_format1_file(FILE * in, t_bool use_matrix) {
  if (in == NULL) return NULL;
  char buf[MAX_READ_LINE];
  if (!read_line_skip_empty(in, buf, sizeof(buf))) {
    return NULL;
  }

  int size = 0;
  if (sscanf(buf, "%d", &size) != 1 || size <= 0) {
    fprintf(stderr, "Format 1 : échec de lecture du nombre de sommets\n");
    return NULL;
  }

  t_graph * g = graph_new(size, 0, use_matrix);

  while (read_line_skip_empty(in, buf, sizeof(buf))) {
    int from, to;
    if (sscanf(buf, "%d %d", &from, &to) == 2) {
      if (from >= 0 && from < size && to >= 0 && to < size) {
        graph_add_edge(g, from, to);
      } else {
        fprintf(stderr, "Format 1 : arête hors limites ignorée \"%s\"\n", buf);
      }
    } else {
      fprintf(stderr, "Format 1 : ligne ignorée car illisible \"%s\"\n", buf);
    }
  }

  return g;
}

// Lecture format 2 (noms)
t_graph * graph_read_format2_file(FILE * in, t_bool use_matrix) {
  if (in == NULL) return NULL;
  char buf[MAX_READ_LINE];
  if (!read_line_skip_empty(in, buf, sizeof(buf))) {
    return NULL;
  }

  int size = 0;
  char tag = '\0';
  int read = sscanf(buf, "%d %c", &size, &tag);
  if (read < 1 || size <= 0) {
    fprintf(stderr, "Format 2 : échec de lecture du nombre de sommets\n");
    return NULL;
  }
  if (read == 2 && tag != 'n') {
    fprintf(stderr, "Format 2 : le second champ doit être 'n', lu '%c'\n", tag);
    return NULL;
  }

  t_graph * g = graph_new(size, 1, use_matrix);

  // Lecture des noms de sommets
  for (int i = 0; i < size; i++) {
    if (!read_line_skip_empty(in, buf, sizeof(buf))) {
      fprintf(stderr, "Format 2 : pas assez de noms de sommets, %d/%d lus\n", i, size);
      graph_free(g);
      return NULL;
    }
    g->names[i] = str_dup(buf);
  }

  // Lecture des arêtes (on suppose des noms sans espace, séparés par des blancs)
  while (read_line_skip_empty(in, buf, sizeof(buf))) {
    char name_from[MAX_READ_LINE], name_to[MAX_READ_LINE];
    if (sscanf(buf, "%s %s", name_from, name_to) == 2) {
      t_vertex from = graph_vertex_from_name(g, name_from);
      t_vertex to = graph_vertex_from_name(g, name_to);
      if (from >= 0 && to >= 0) {
        graph_add_edge(g, from, to);
      } else {
        fprintf(stderr, "Format 2 : arête ignorée, sommets inconnus \"%s\"\n", buf);
      }
    } else {
      fprintf(stderr, "Format 2 : ligne ignorée car illisible \"%s\"\n", buf);
    }
  }

  return g;
}

// Enveloppe avec nom de fichier
static t_graph * graph_read_with_file(const char * filename, t_bool use_matrix,
                                      t_graph * (*reader)(FILE *, t_bool)) {
  t_graph * g = NULL;
  if (filename == NULL || strcmp(filename, "-") == 0) {
    g = reader(stdin, use_matrix);
  } else {
    FILE * f = fopen(filename, "r");
    if (f == NULL) {
      fprintf(stderr, "Impossible d'ouvrir le fichier %s\n", filename);
      return NULL;
    }
    g = reader(f, use_matrix);
    fclose(f);
  }
  return g;
}

t_graph * graph_read_format1(const char * filename, t_bool use_matrix) {
  return graph_read_with_file(filename, use_matrix, graph_read_format1_file);
}

t_graph * graph_read_format2(const char * filename, t_bool use_matrix) {
  return graph_read_with_file(filename, use_matrix, graph_read_format2_file);
}

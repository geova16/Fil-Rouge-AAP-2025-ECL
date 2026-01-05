#include "graph.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOKEN_MAX 256
/*
========================
 Compilation
========================
gcc -Wall -Wextra -std=c11 -O2 fil_rouge_2.c graph.c base_fil_rouge.c \-o fil_rouge_2 -o fil_rouge_2
    -o fil_rouge_2
*/

/*
========================
 Utilisation
========================
./fil_rouge_2 -i <fichier_graphe> -start <sommet> -goal <sommet>
Options:
  -i <fichier_graphe> : fichier du graphe (sinon stdin)
  -o <fichier_sortie> : fichier de sortie (sinon stdout)
  -start <sommet>     : sommet de depart (numero ou nom)
  -goal <sommet>      : sommet d'arrivee (numero ou nom)
*/

/* ---------- Piles / listes (API prof, définie dans base_fil_rouge.c) ---------- */
typedef struct node {
  t_vertex val;
  struct node *p_next;
} t_node;

typedef t_node *t_list;
typedef t_list t_stack;

// Piles
t_stack *stack_new();
int stack_is_empty(t_stack *ps);
void stack_push(t_vertex e, t_stack *ps);
t_vertex stack_pop(t_stack *ps);
t_vertex stack_top(t_stack *ps);

// Listes / curseurs
t_list list_free(t_list l);
t_node *list_cursor_new(t_list l);
int list_cursor_at_end(t_node *lc);
int list_cursor_get_val(t_node *lc);
t_node *list_cursor_next(t_node *lc);

static void usage(const char *prog) {
  fprintf(stderr,
          "Utilisation:\n"
          "  %s [-i <fichier_graphe>] [-o <fichier_sortie>] [-start <sommet>] [-goal <sommet>]\n"
          "Options:\n"
          "  -i <fichier_graphe> : fichier du graphe (si omis, stdin)\n"
          "  -o <fichier_sortie> : fichier de sortie (si omis, stdout)\n"
          "  -start <sommet>     : sommet de depart (si omis, stdin)\n"
          "  -goal <sommet>      : sommet cible (si omis, stdin)\n",
          prog);
}

static int read_token_stdin(char *buf, size_t cap) {
  (void)cap;
  return (scanf("%255s", buf) == 1);
}

/* Détecte format 1 vs 2 via la 1ère ligne non vide du fichier.
   Retour: 1 => format1, 2 => format2. (stdin: défaut format1)
*/
static int detect_format(const char *filename) {
  if (filename == NULL || strcmp(filename, "-") == 0) return 1;

  FILE *f = fopen(filename, "r");
  if (!f) return 1;

  char line[512];
  while (fgets(line, (int)sizeof(line), f)) {
    char *p = line;
    while (*p && isspace((unsigned char)*p)) p++;
    if (*p == '\0') continue;

    int n = 0;
    char tag = '\0';
    int r = sscanf(p, "%d %c", &n, &tag);
    fclose(f);
    if (r >= 2 && (tag == 'n' || tag == 'N')) return 2;
    return 1;
  }

  fclose(f);
  return 1;
}

/* Parse sommet: si graphe nommé => essaye nom puis entier */
static int parse_vertex(const t_graph *g, const char *s, t_vertex *out) {
  if (!s || !out) return 0;

  if (graph_has_names(g)) {
    t_vertex v = graph_vertex_from_name(g, s);
    if (v >= 0) {
      *out = v;
      return 1;
    }
  }

  char *end = NULL;
  long val = strtol(s, &end, 10);
  if (end && *end == '\0') {
    if (val >= 0 && val < graph_size(g)) {
      *out = (t_vertex)val;
      return 1;
    }
  }
  return 0;
}

/* Recherche de chemin (fonction récursive)
   Retourne VRAI si un chemin de x vers y existe,
   et empile le chemin dans stack (de y vers x).
*/
struct rr_ctx {
  const t_graph *g;
  t_vertex y;
  t_bool *marking;
  t_stack *stack;
  t_bool found;
};

static t_bool Recherche_recur_f(const t_graph *g, t_vertex x, t_vertex y,
                                t_bool *marking, t_stack *stack);

static void rr_succ_cb(t_vertex w, void *p) {
  struct rr_ctx *ctx = (struct rr_ctx *)p;
  if (ctx->found) return;

  if (Recherche_recur_f(ctx->g, w, ctx->y, ctx->marking, ctx->stack)) {
    ctx->found = 1;
  }
}

/* Recherche de chemin (récursif) — compatible avec graph_for_each_succ */
static t_bool Recherche_recur_f(const t_graph *g,
                                t_vertex x,
                                t_vertex y,
                                t_bool *marking,
                                t_stack *stack) {
  if (x == y) {                // si x = y
    stack_push(x, stack);      // empiler x
    return 1;                  // VRAI
  }

  if (marking[x]) return 0;    // déjà visité
  marking[x] = 1;              // marquer x

  struct rr_ctx ctx = { g, y, marking, stack, 0 };

  // pour chaque successeur w de x
  graph_for_each_succ(g, x, rr_succ_cb, &ctx);

  if (ctx.found) {             // si trouvé
    stack_push(x, stack);      // empiler x (au retour)
    return 1;
  }

  return 0;                    // FAUX
}

static t_bool Recherche_recur(const t_graph *g, t_vertex x, t_vertex y, t_stack **out_stack) {
  int n = graph_size(g);
  t_bool *marking = calloc((size_t)n, sizeof(*marking));
  assert(marking);

  t_stack *stack = stack_new();
  t_bool ok = Recherche_recur_f(g, x, y, marking, stack);

  free(marking);

  if (ok) {
    *out_stack = stack;
  } else {
    *stack = list_free(*stack);
    free(stack);
    *out_stack = NULL;
  }
  return ok;
}


/* ---------- Affichage du chemin ---------- */
static void print_vertex(FILE *out, const t_graph *g, t_vertex v) {
  const char *name = graph_vertex_name(g, v);
  if (name) fprintf(out, "%s", name);
  else fprintf(out, "%d", v);
}

static void print_path_stack(FILE *out, const t_graph *g, t_stack *stack) {
  // La pile contient déjà [start, ..., goal] dans l'ordre (tête -> queue)
  t_node *lc = list_cursor_new(*stack);
  int first = 1;
  while (!list_cursor_at_end(lc)) {
    t_vertex v = (t_vertex)list_cursor_get_val(lc);
    if (!first) fprintf(out, " -> ");
    first = 0;
    print_vertex(out, g, v);
    lc = list_cursor_next(lc);
  }
  fprintf(out, "\n");
}

int main(int argc, char **argv) {
  const char *infile = NULL, *outfile = NULL;
  const char *start_s = NULL, *goal_s = NULL;

  for (int i = 1; i < argc; i++) {
    if      (!strcmp(argv[i], "-i")     && i + 1 < argc) infile  = argv[++i];
    else if (!strcmp(argv[i], "-o")     && i + 1 < argc) outfile = argv[++i];
    else if (!strcmp(argv[i], "-start") && i + 1 < argc) start_s = argv[++i];
    else if (!strcmp(argv[i], "-goal")  && i + 1 < argc) goal_s  = argv[++i];
    else { usage(argv[0]); return EXIT_FAILURE; }
  }

  t_bool use_matrix = 0;

  int format = detect_format(infile);
  t_graph *g;
  if (format == 2) {
    g = graph_read_format2(infile, use_matrix);
  } else {
    g = graph_read_format1(infile, use_matrix);
  }

  if (!g) { fprintf(stderr, "Erreur: lecture du graphe impossible.\n"); return EXIT_FAILURE; }

  char buf_start[TOKEN_MAX], buf_goal[TOKEN_MAX];
  if (!start_s) { if (!read_token_stdin(buf_start, sizeof buf_start)) { fprintf(stderr, "Erreur: lire start.\n"); graph_free(g); return EXIT_FAILURE; } start_s = buf_start; }
  if (!goal_s)  { if (!read_token_stdin(buf_goal,  sizeof buf_goal))  { fprintf(stderr, "Erreur: lire goal.\n");  graph_free(g); return EXIT_FAILURE; } goal_s  = buf_goal;  }

  t_vertex start, goal;
  if (!parse_vertex(g, start_s, &start)) { fprintf(stderr, "Start invalide: %s\n", start_s); graph_free(g); return EXIT_FAILURE; }
  if (!parse_vertex(g, goal_s,  &goal))  { fprintf(stderr, "Goal invalide: %s\n",  goal_s);  graph_free(g); return EXIT_FAILURE; }

  FILE *out = stdout;
  if (outfile && strcmp(outfile, "-") != 0) {
    out = fopen(outfile, "w");
    if (!out) { fprintf(stderr, "Impossible d'ouvrir %s\n", outfile); graph_free(g); return EXIT_FAILURE; }
  }

  t_stack *path = NULL;
  if (Recherche_recur(g, start, goal, &path)) {
    print_path_stack(out, g, path);
    *path = list_free(*path);
    free(path);
  } else {
    fprintf(out, "Aucun chemin trouve.\n");
  }

  if (out != stdout) fclose(out);
  graph_free(g);
  return EXIT_SUCCESS;
}

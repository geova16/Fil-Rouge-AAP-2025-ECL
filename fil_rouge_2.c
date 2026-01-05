#include "graph.h"

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOKEN_MAX 256

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
    if (r >= 2 && tag == 'n') return 2;
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

/* ---------- DFS (récursif) ---------- */
struct dfs_ctx {
  const t_graph *g;
  t_vertex goal;
  int *mark;
  t_vertex *parent;
  int found;
};

static void dfs_visit(t_vertex u, struct dfs_ctx *ctx);

struct succ_ctx {
  struct dfs_ctx *ctx;
  t_vertex u; // sommet courant (parent des successeurs)
};

static void succ_cb(t_vertex v, void *p) {
  struct succ_ctx *sc = (struct succ_ctx *)p;
  struct dfs_ctx *ctx = sc->ctx;

  if (ctx->found) return;
  if (ctx->mark[v]) return;

  ctx->parent[v] = sc->u;
  dfs_visit(v, ctx);
}

static void dfs_visit(t_vertex u, struct dfs_ctx *ctx) {
  if (ctx->found) return;
  ctx->mark[u] = 1;

  if (u == ctx->goal) {
    ctx->found = 1;
    return;
  }

  struct succ_ctx sc = { ctx, u };
  graph_for_each_succ(ctx->g, u, succ_cb, &sc);
}

/* ---------- Print path ---------- */
static void print_vertex(FILE *out, const t_graph *g, t_vertex v) {
  const char *name = graph_vertex_name(g, v);
  if (name) fprintf(out, "%s", name);
  else fprintf(out, "%d", v);
}

static void print_path(FILE *out, const t_graph *g, t_vertex start, t_vertex goal, const t_vertex *parent) {
  int n = graph_size(g);
  t_vertex *rev = malloc((size_t)n * sizeof(*rev));
  assert(rev);

  int len = 0;
  for (t_vertex cur = goal; cur != -1 && len < n; cur = parent[cur]) {
    rev[len++] = cur;
    if (cur == start) break;
  }

  if (len == 0 || rev[len - 1] != start) {
    fprintf(out, "Aucun chemin trouve.\n");
    free(rev);
    return;
  }

  for (int i = len - 1; i >= 0; --i) {
    print_vertex(out, g, rev[i]);
    if (i) fprintf(out, " -> ");
  }
  fprintf(out, "\n");
  free(rev);
}

int main(int argc, char **argv) {
  const char *infile = NULL;
  const char *outfile = NULL;
  const char *start_s = NULL;
  const char *goal_s = NULL;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) infile = argv[++i];
    else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) outfile = argv[++i];
    else if (strcmp(argv[i], "-start") == 0 && i + 1 < argc) start_s = argv[++i];
    else if (strcmp(argv[i], "-goal") == 0 && i + 1 < argc) goal_s = argv[++i];
    else {
      fprintf(stderr, "Argument non reconnu: %s\n", argv[i]);
      usage(argv[0]);
      return EXIT_FAILURE;
    }
  }

 
  t_bool use_matrix = 0;

  int format = detect_format(infile);
  t_graph *g = (format == 2) ? graph_read_format2(infile, use_matrix)
                            : graph_read_format1(infile, use_matrix);

  if (!g) {
    fprintf(stderr, "Erreur: lecture du graphe impossible.\n");
    return EXIT_FAILURE;
  }

  char buf_start[TOKEN_MAX], buf_goal[TOKEN_MAX];
  if (!start_s) {
    if (!read_token_stdin(buf_start, sizeof(buf_start))) {
      fprintf(stderr, "Erreur: impossible de lire -start depuis stdin.\n");
      graph_free(g);
      return EXIT_FAILURE;
    }
    start_s = buf_start;
  }
  if (!goal_s) {
    if (!read_token_stdin(buf_goal, sizeof(buf_goal))) {
      fprintf(stderr, "Erreur: impossible de lire -goal depuis stdin.\n");
      graph_free(g);
      return EXIT_FAILURE;
    }
    goal_s = buf_goal;
  }

  t_vertex start = -1, goal = -1;
  if (!parse_vertex(g, start_s, &start)) {
    fprintf(stderr, "Sommet de depart invalide: %s\n", start_s);
    graph_free(g);
    return EXIT_FAILURE;
  }
  if (!parse_vertex(g, goal_s, &goal)) {
    fprintf(stderr, "Sommet d'arrivee invalide: %s\n", goal_s);
    graph_free(g);
    return EXIT_FAILURE;
  }

  int n = graph_size(g);
  int *mark = calloc((size_t)n, sizeof(*mark));
  t_vertex *parent = malloc((size_t)n * sizeof(*parent));
  assert(mark && parent);
  for (int i = 0; i < n; i++) parent[i] = -1;

  struct dfs_ctx ctx = { g, goal, mark, parent, 0 };
  dfs_visit(start, &ctx);

  FILE *out = stdout;
  if (outfile && strcmp(outfile, "-") != 0) {
    out = fopen(outfile, "w");
    if (!out) {
      fprintf(stderr, "Impossible d'ouvrir le fichier de sortie %s\n", outfile);
      free(mark);
      free(parent);
      graph_free(g);
      return EXIT_FAILURE;
    }
  }

  if (ctx.found) print_path(out, g, start, goal, parent);
  else fprintf(out, "Aucun chemin trouve.\n");

  if (out != stdout) fclose(out);

  free(mark);
  free(parent);
  graph_free(g);
  return EXIT_SUCCESS;
}

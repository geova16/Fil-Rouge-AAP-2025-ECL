#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "graph.h"

/*
Compilação (duas versões exigidas pelo projeto):
  gcc -Wall -Wextra -std=c11 -O2 fil_rouge_1.c graph.c -DUSE_MATRIX=1 -o fil_rouge_1_matrix
  gcc -Wall -Wextra -std=c11 -O2 fil_rouge_1.c graph.c -DUSE_MATRIX=0 -o fil_rouge_1_list
*/

#ifndef USE_MATRIX
#define USE_MATRIX 1
#endif

#define BUF_SZ 4096

static void usage(const char *prog) {
  fprintf(stderr,
          "Usage: %s [-i <graph_file>] [-o <dot_file>]\n"
          "  -i <graph_file> : input graph (default stdin)\n"
          "  -o <dot_file>   : output DOT (default stdout)\n",
          prog);
}

static int is_blank_line(const char *s) {
  while (*s) {
    if (!isspace((unsigned char)*s)) return 0;
    s++;
  }
  return 1;
}

/* Lê a primeira linha não vazia do stream */
static int read_first_nonempty_line(FILE *in, char *buf, size_t buflen) {
  while (fgets(buf, (int)buflen, in) != NULL) {
    if (!is_blank_line(buf)) return 1;
  }
  return 0;
}

/* Detecta formato:
   - se primeira linha for "N n" (n ou N), é formato 2
   - senão, formato 1
*/
static int detect_format_from_header(const char *line) {
  int N = 0;
  char tag = '\0';
  int r = sscanf(line, " %d %c", &N, &tag);
  if (r >= 2 && (tag == 'n' || tag == 'N')) return 2;
  return 1;
}

/* Para stdin: reconstrói um stream (tmpfile) com header + resto do stdin,
   para poder passar tudo ao leitor do grafo (porque stdin não dá rewind). */
static FILE *rebuild_stream_with_header_and_rest(const char *header_line, FILE *in) {
  FILE *tmp = tmpfile();
  if (!tmp) return NULL;

  fputs(header_line, tmp);
  size_t L = strlen(header_line);
  if (L == 0 || header_line[L - 1] != '\n') fputc('\n', tmp);

  char block[8192];
  size_t nread;
  while ((nread = fread(block, 1, sizeof(block), in)) > 0) {
    fwrite(block, 1, nread, tmp);
  }

  rewind(tmp);
  return tmp;
}

/* Escape básico para nomes no DOT */
static void dot_print_escaped(FILE *out, const char *s) {
  fputc('"', out);
  for (const char *p = s; *p; p++) {
    if (*p == '"' || *p == '\\') fputc('\\', out);
    fputc(*p, out);
  }
  fputc('"', out);
}

struct dot_ctx {
  FILE *out;
  const t_graph *g;
  t_vertex from;
};

static void dot_succ_cb(t_vertex to, void *ctx_void) {
  struct dot_ctx *ctx = (struct dot_ctx*)ctx_void;

  if (graph_has_names(ctx->g)) {
    const char *a = graph_vertex_name(ctx->g, ctx->from);
    const char *b = graph_vertex_name(ctx->g, to);

    fputs("  ", ctx->out);
    dot_print_escaped(ctx->out, a ? a : "");
    fputs(" -> ", ctx->out);
    dot_print_escaped(ctx->out, b ? b : "");
    fputs(";\n", ctx->out);
  } else {
    fprintf(ctx->out, "  %d -> %d;\n", ctx->from, to);
  }
}

static void write_dot(FILE *out, const t_graph *g) {
  fprintf(out, "digraph nom_du_graphe {\n");

  struct dot_ctx ctx;
  ctx.out = out;
  ctx.g = g;

  for (t_vertex u = 0; u < graph_size(g); u++) {
    ctx.from = u;
    graph_for_each_succ(g, u, dot_succ_cb, &ctx);
  }

  fprintf(out, "}\n");
}

int main(int argc, char **argv) {
  const char *in_path = NULL;
  const char *out_path = NULL;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-i") == 0) {
      if (i + 1 >= argc) { usage(argv[0]); return 1; }
      in_path = argv[++i];
    } else if (strcmp(argv[i], "-o") == 0) {
      if (i + 1 >= argc) { usage(argv[0]); return 1; }
      out_path = argv[++i];
    } else {
      usage(argv[0]);
      return 1;
    }
  }

  FILE *in = stdin;
  FILE *out = stdout;

  if (in_path) {
    in = fopen(in_path, "r");
    if (!in) { perror("fopen -i"); return 1; }
  }
  if (out_path) {
    out = fopen(out_path, "w");
    if (!out) {
      perror("fopen -o");
      if (in != stdin) fclose(in);
      return 1;
    }
  }

  /* Lê header para detectar formato */
  char header[BUF_SZ];
  if (!read_first_nonempty_line(in, header, sizeof(header))) {
    fprintf(stderr, "Input vazio.\n");
    if (in != stdin) fclose(in);
    if (out != stdout) fclose(out);
    return 1;
  }

  int fmt = detect_format_from_header(header);
  t_graph *g = NULL;

  if (in == stdin) {
    FILE *tmp = rebuild_stream_with_header_and_rest(header, in);
    if (!tmp) {
      fprintf(stderr, "Falha ao criar tmpfile() para stdin.\n");
      if (out != stdout) fclose(out);
      return 1;
    }
    g = (fmt == 2)
        ? graph_read_format2_file(tmp, (t_bool)USE_MATRIX)
        : graph_read_format1_file(tmp, (t_bool)USE_MATRIX);
    fclose(tmp);
  } else {
    rewind(in);
    g = (fmt == 2)
        ? graph_read_format2_file(in, (t_bool)USE_MATRIX)
        : graph_read_format1_file(in, (t_bool)USE_MATRIX);
  }

  if (!g) {
    fprintf(stderr, "Falha ao ler o grafo.\n");
    if (in != stdin) fclose(in);
    if (out != stdout) fclose(out);
    return 1;
  }

  write_dot(out, g);

  graph_free(g);
  if (in != stdin) fclose(in);
  if (out != stdout) fclose(out);
  return 0;
}

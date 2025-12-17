# README – Graph Module (中文 / English / Français / Português)

## 中文
- 模块功能：提供有向图的抽象类型 `t_graph`，支持两种表示方式（邻接表 / 动态邻接矩阵），可选顶点名称；封装读图、加边、遍历后继、转置图等操作。
- 文件：`graph.h`（接口声明），`graph.c`（实现）。其他程序只需 `#include "graph.h"` 并与 `graph.c` 一起编译。
- 主要类型：
  - `t_graph*`：不透明图指针；内部结构不暴露。
  - `t_vertex`（int）：顶点编号。
  - `t_bool`（int）：布尔值（0/1）。
- 创建与释放：
  - `graph_new(int size, t_bool with_names, t_bool use_matrix)`：创建图；`with_names=1` 分配顶点名数组；`use_matrix=1` 用动态邻接矩阵，否则用邻接表。
  - `graph_free(t_graph *g)`：释放图及内部资源。
- 基础信息：
  - `graph_size(g)`：顶点数。
  - `graph_has_names(g)`：是否存在名字数组。
  - `graph_vertex_name(g, v)`：返回顶点 v 的名字（无名或越界返回 NULL）。
  - `graph_vertex_from_name(g, name)`：按名字查编号，不存在返回 -1。
- 边操作：
  - `graph_add_edge(g, from, to)`：添加有向边（自动去重）。
  - `graph_has_edge(g, from, to)`：检查有向边是否存在。
- 遍历与转置：
  - `graph_for_each_succ(g, u, callback, ctx)`：遍历 u 的所有后继，回调 `callback(v, ctx)`。算法层无需关心底层是矩阵还是邻接表。
  - `graph_reverse(g)`：返回转置图 g'（复制名字，保持同种表示）。
  - `graph_show(g)`：打印邻接关系（按邻接表风格，名字优先）。
- 读图（格式符合课堂要求）：
  - `graph_read_format1_file(FILE *in, t_bool use_matrix)`：格式1（编号）从流读取。
  - `graph_read_format2_file(FILE *in, t_bool use_matrix)`：格式2（名字）从流读取，首行接受 `N n`。
  - `graph_read_format1(const char *filename, t_bool use_matrix)` / `graph_read_format2(...)`：文件名封装；`filename` 为 NULL 或 "-" 时从 stdin 读。
- 编译示例：
  - `gcc -Wall -Wextra -std=c11 -c graph.c`
  - `gcc main.c graph.o -o app`
- 使用示例（伪代码）：
  ```c
  t_graph *g = graph_read_format2("g2.txt", 0); // 0=邻接表, 1=矩阵
  graph_for_each_succ(g, 0, my_cb, ctx);       // 遍历后继
  t_graph *gr = graph_reverse(g);              // 转置图
  graph_free(gr);
  graph_free(g);
  ```

## English
- Purpose: Directed graph ADT `t_graph` with two storage options (adjacency lists / dynamic adjacency matrix) and optional vertex names; wrapped helpers for reading, adding edges, successor traversal, transpose, etc.
- Files: `graph.h` (API), `graph.c` (implementation). Include the header and compile/link with `graph.c`.
- Key types: `t_graph*` (opaque), `t_vertex` (int), `t_bool` (int).
- Create/free: `graph_new(size, with_names, use_matrix)`; `graph_free(g)`.
- Info: `graph_size`, `graph_has_names`, `graph_vertex_name`, `graph_vertex_from_name`.
- Edges: `graph_add_edge`, `graph_has_edge`.
- Traversal/transpose: `graph_for_each_succ`, `graph_reverse`, `graph_show`.
- Reading: `graph_read_format1_file`, `graph_read_format2_file`, and filename wrappers; stdin if filename is NULL or "-".
- Compile: `gcc -Wall -Wextra -std=c11 -c graph.c` then `gcc main.c graph.o -o app`.
- Usage: load with `graph_read_formatX`, traverse via `graph_for_each_succ`, transpose with `graph_reverse`.

## Français
- Objet : TAD graphe orienté `t_graph` avec deux représentations (listes d'adjacence / matrice dynamique) et noms optionnels ; fonctions pour lecture, ajout d'arêtes, parcours des successeurs, transposé, etc.
- Fichiers : `graph.h` (API), `graph.c` (implémentation). Inclure le header et compiler/lier avec `graph.c`.
- Types clés : `t_graph*` (opaque), `t_vertex` (int), `t_bool` (int).
- Création/libération : `graph_new(taille, with_names, use_matrix)` ; `graph_free(g)`.
- Infos : `graph_size`, `graph_has_names`, `graph_vertex_name`, `graph_vertex_from_name`.
- Arêtes : `graph_add_edge`, `graph_has_edge`.
- Parcours/transposé : `graph_for_each_succ`, `graph_reverse`, `graph_show`.
- Lecture : `graph_read_format1_file`, `graph_read_format2_file`, wrappers avec nom de fichier ; stdin si nom NULL ou "-".
- Compilation : `gcc -Wall -Wextra -std=c11 -c graph.c` puis `gcc main.c graph.o -o app`.
- Usage : lire un graphe via `graph_read_formatX`, parcourir avec `graph_for_each_succ`, obtenir le transposé avec `graph_reverse`.

## Português
- Objetivo: TAD de grafo direcionado `t_graph` com duas representações (listas de adjacência / matriz dinâmica) e nomes opcionais; utilitários para ler, adicionar arestas, percorrer sucessores, transpor, etc.
- Arquivos: `graph.h` (API), `graph.c` (implementação). Inclua o header e compile/ligue com `graph.c`.
- Tipos principais: `t_graph*` (opaco), `t_vertex` (int), `t_bool` (int).
- Criar/liberar: `graph_new(tamanho, with_names, use_matrix)`; `graph_free(g)`.
- Info: `graph_size`, `graph_has_names`, `graph_vertex_name`, `graph_vertex_from_name`.
- Arestas: `graph_add_edge`, `graph_has_edge`.
- Percurso/transposto: `graph_for_each_succ`, `graph_reverse`, `graph_show`.
- Leitura: `graph_read_format1_file`, `graph_read_format2_file` e wrappers por nome de arquivo; stdin se nome for NULL ou "-".
- Compilar: `gcc -Wall -Wextra -std=c11 -c graph.c` depois `gcc main.c graph.o -o app`.
- Uso: ler com `graph_read_formatX`, percorrer via `graph_for_each_succ`, transpor com `graph_reverse`.

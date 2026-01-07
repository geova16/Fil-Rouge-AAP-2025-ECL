# README_fil_rouge_2

## 0.Compilation

### 0.1 Version Matrice d’adjacence

```c
gcc -Wall -Wextra -std=c11 -O2 fil_rouge_2.c graph.c base_fil_rouge.c -DUSE_MATRIX=1 -o fil_rouge_2_matrix
```

### 0.2 Version Listes d’adjacence

```c
gcc -Wall -Wextra -std=c11 -O2 fil_rouge_2.c graph.c base_fil_rouge.c -DUSE_MATRIX=0 -o fil_rouge_2_list
```

## 0. Utilisation

```c
./fil_rouge_2_matrix -i Fichiers/graphes/numeros/ex1.txt -start 0 -goal 3
```

# Programme 2 : Recherche d’un chemin dans un graphe orienté

## 1. Objectif

Le programme `fil_rouge_2` a pour objectif de **rechercher un chemin** dans un **graphe orienté**, entre :

- un **sommet source** `s`,
- et un **sommet destination** `t`.

Le graphe est lu depuis une entrée (fichier ou `stdin`) au **format 1** (sommets numérotés) ou au **format 2** (sommets nommés), via le module commun `graph.c / graph.h`.

Si un chemin existe, le programme :

- affiche un message indiquant que le chemin a été trouvé,
- affiche la **suite des sommets constituant le chemin**, dans l’ordre.

Sinon, il indique qu’aucun chemin n’existe entre `s` et `t`.

La recherche est implémentée à l’aide d’un **parcours en profondeur (DFS)** récursif.

## 2. Contenu du projet (fichiers)

- `fil_rouge_2.c`
   Programme principal :
  - analyse des arguments (`-i`, `-s`, `-t`),
  - lecture du graphe,
  - lancement de la recherche de chemin,
  - affichage du résultat.
- `graph.h`   : Interface du graphe (commune à tous les programmes).
- `graph.c`   : Implémentation : création/libération, ajout d’arêtes, parcours des successeurs, lecture format 1/2, etc.
- `base_fil_rouge.c`
   Fonctions utilitaires communes :gestion de la pile (`stack`), affichage

## 3. Formats d’entrée

Le module `graph.c` ignore automatiquement :

- les lignes vides,
- les lignes ne contenant que des espaces.

### 3.1 Format 1 : sommets numérotés

#### Structure :

```txt
N
u v
u v
...

```

`N` : nombre de sommets (numérotés de `0` à `N-1`)

chaque ligne `u v` représente une arête orientée `u → v`

### 3.2 Format 2 : sommets nommés

```txt
N n
nom0
nom1
...
nom(N-1)
nom_u nom_v
nom_u nom_v
...
```

## 4. Principe de l’algorithme

La recherche de chemin repose sur :

- un **parcours en profondeur récursif (DFS)**,
- un tableau de marquage (`marking`) pour éviter les cycles,
- une **pile** permettant de reconstruire le chemin.

### Étapes principales :

1. Démarrage depuis le sommet source `s`.
2. Si le sommet courant est `t`, le chemin est trouvé.
3. Sinon :
   - marquer le sommet comme visité,
   - parcourir récursivement ses successeurs.
4. Si un successeur mène à `t`, empiler les sommets du chemin.
5. Si aucun successeur ne mène à `t`, revenir en arrière (backtracking).

Le parcours des successeurs utilise le mécanisme de **callback** fourni par `graph.c`.

## 5. Sortie du programme

### 5.1 Chemin trouvé

Si un chemin existe, le programme affiche par exemple :

```
0 -> 2 -> 3
```

ou, pour un graphe à sommets nommés :

```
start -> stop
```

### 5.2 Aucun chemin

Si aucun chemin n’existe :

```
Aucun chemin trouve.
```

## 6. Arguments en ligne de commande

- `-i <graph_file>`
   Fichier d’entrée (par défaut : `stdin`)
- `-start <source>`
   Sommet source (obligatoire)
- `-goal <destination>`
   Sommet destination (obligatoire)

Si un argument est manquant ou invalide, le programme affiche un message d’aide.

## 7. Exemples complets

### 7.1 Exemple Format 1

Entrée `ex1.txt` :

```
4
0 1
0 2
2 1
2 3
3 2
```

Commande :

```
./fil_rouge_2 -i ex1.txt -start 0 -goal 3
```

Sortie :

```
0 -> 2 -> 3
```

### 7.2 Exemple Format 2

Entrée `ex2.txt` :

```
3 n
A
B
C
A B
B C
C A
```

Commande :

```
./fil_rouge_2 -i ex2.txt -start A -goal C
```

Sortie :

```
A -> B -> C

```


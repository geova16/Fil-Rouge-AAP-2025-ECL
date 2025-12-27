# Programme 1 : Conversion Graphe → DOT

## 1. Objectif
Ce programme lit un graphe orienté depuis une entrée (fichier ou `stdin`) au **format 1** (sommets numérotés) ou **format 2** (sommets nommés), puis produit une représentation au format **DOT** vers un fichier ou `stdout`.

Il s’appuie sur le module `graph.c / graph.h` qui implémente le graphe :
- soit en **matrice d’adjacence** (allocation dynamique),
- soit en **listes d’adjacence**.

Le Programme 1 doit être livré en **deux versions** (matrice et listes), conformément au sujet.

---

## 2. Contenu du projet (fichiers)
- `fil_rouge_1.c`  
  Programme principal : parse les arguments, lit le graphe via `graph.c`, puis écrit le DOT.
- `graph.h`  
  Interface (API) du graphe.
- `graph.c`  
  Implémentation : création/libération, ajout d’arêtes, parcours des successeurs, lecture format 1/2, etc.

> ⚠️ Important : Vérifiez que `graph.h` se termine correctement par :
> `#endif // GRAPH_H`
> (et **pas** `#endif ... #include "graph.h"` sur la même ligne).

---

## 3. Formats d’entrée

Le module `graph.c` ignore déjà les **lignes vides** (ou contenant seulement des espaces) lors de la lecture.

### 4.1 Format 1 : sommets numérotés
**Structure :**
```bash
N
u v
u v
...
```

- `N` : nombre de sommets (de `0` à `N-1`)
- chaque ligne `u v` : arête orientée `u -> v`

**Exemple :**
```bash
4
0 1
0 2
2 1
2 3
3 2
```

### 4.2 Format 2 : sommets nommés
**Structure :**
```bash
N n
nom0
nom1
...
nom(N-1)
nom_u nom_v
nom_u nom_v
...
```

- première ligne : `N n` (le second champ doit être `n`)
- ensuite `N` lignes : les noms des sommets (un nom par ligne)
- ensuite : les arêtes sous forme `nom_u nom_v` (noms sans espaces)

**Exemple :**
```bash
3 n
A
B
C
A B
B C
C A
```
---

## 5. Sortie : format DOT (Graphviz)

Le programme génère :
digraph nom_du_graphe {
...
}

- En **format 1** : les sommets sont imprimés en entiers (ex: `0 -> 1;`)
- En **format 2** : les sommets sont imprimés en chaînes entourées de guillemets (ex: `"A" -> "B";`)

> Remarque : L’ordre des arêtes dans le fichier DOT peut varier, surtout avec les listes d’adjacence (insertion en tête).  
> Cela ne change pas la validité du graphe.

---

## 6. Compilation (deux versions exigées)

Le comportement du programme est identique, seule la représentation interne change via la macro `USE_MATRIX`.

### 6.1 Version Matrice d’adjacence
```bash
gcc -Wall -Wextra -std=c11 -O2 fil_rouge_1.c graph.c -DUSE_MATRIX=1 -o fil_rouge_1_matrix
```
### 6.2 Version Listes d’adjacence
```bash
gcc -Wall -Wextra -std=c11 -O2 fil_rouge_1.c graph.c -DUSE_MATRIX=0 -o fil_rouge_1_list
```
## 7. Utilisation
### 7.1 Lecture depuis un fichier, écriture vers un fichier

```bash
./fil_rouge_1_matrix -i ex1.txt -o graphe.dot
```
### 7.2 Lecture depuis stdin, écriture vers stdout

```bash
./fil_rouge_1_list < ex1.txt > graphe.dot
```
### 7.3 Arguments
- `-i <graph_file>` : fichier d’entrée (défaut : `stdin`)
- `-o <dot_file>` : fichier de sortie (défaut : `stdout`)
Si un argument est manquant ou incorrect, le programme affiche une aide.

## 8. Exemple complet de `fil_rouge_1.c`

### 8.1 Exemple Format 1
Entrée ex1.txt :
```c
4
0 1
0 2
2 1
2 3
3 2
```
Commande :
```bash
./fil_rouge_1_matrix -i ex1.txt -o graphe.dot
```
Sortie graphe.dot :
```c
digraph nom_du_graphe {
  0 -> 2;
  0 -> 1;
  2 -> 3;
  2 -> 1;
  3 -> 2;
}
```

### 8.2 Exemple Format 2
Entrée ex2.txt :
```c
3 n
A
B
C
A B
B C
C A
```
Commande :
```bash
./fil_rouge_1_list -i ex2.txt -o graphe.dot
```
Sortie graphe.dot :
```c
digraph nom_du_graphe {
  "A" -> "B";
  "B" -> "C";
  "C" -> "A";
}

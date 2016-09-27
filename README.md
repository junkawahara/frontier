frontier
========

See "doc/frontier.pdf" for details (in Japanese). 

# Brief usage

## Introduction

The frontier-based search is an algorithm that constructs Zero-suppressed binary Decision Diagrams (ZDDs) representing various subgraph sets such as s-t paths, spanning trees and graph matchings. By using constructed ZDDs, we can enumerate all subgraphs, count the number of subgraphs, and sample one uniformly and randomly.

## What the program can do

* Construct ZDDs representing the following subgraphs:
  * spanning forests
  * spanning trees
  * undirected/directed s-t paths (cycles)
  * rooted forests
  * k-cuts
  * rooted k-cuts
* Reduce ZDDs (Obtain so-called "reduced and ordered ZDD")
* Count the number of subgraphs ZDD represents
* Enumerate subgraphs one by one
* Sample a subgraph uniformly and randomly
* Output ZDDs in the graphviz format

## Build and Run

### Preparation

Install GNU MP library (http://gmplib.org/) if you want to treat an integer more than 2^64.

### Build

```
./configure
make
```

For purpose of explanation, we make symbolic links as follows:

```
ln -s src/frontier/frontier frontier
ln -s src/utility/makegrid makegrid
```

```makegrid``` creates an m times n grid graph.
It can be used as an input of ```frontier```.

Example (for 2 times 3 grid graph):

```
./makegrid 2 3
```

Output:

```
2 4
1 3 5
2 6
1 5
2 4 6
3 5
```

### Example

To construct the ZDD representing the set of spanning forests on 3 times 3 grid,
run the following command:

```
./makegrid 3 3 | ./frontier -t sforest
```

The result of Standard output (stdout):

```
\#1:
2:3,4
\#2:
3:5,5
4:5,6
\#3:
5:7,8
...
```

The above output represents a ZDD. See the below section.

The result of Standard error (stderr):

```
\# of nodes of ZDD = 45
\# of solutions = 3102
```

We find that the number of nodes (before reducing) is 45, that of subgraphs is 3,102.

To reduce the ZDD, use ```-r``` option.

```
./makegrid -d 3 3 | ./frontier -t sforest -r
```

Output:

```
\# of nodes of ZDD = 45
\# of nodes of reduced ZDD = 38
\# of solutions = 3102
```

To construct a ZDD for s-t paths, use ```-t stpath``` option.
The following example is for s-t paths where s is left-top corner
and t is right-bottom corner.

```
./makegrid -d 3 3 | ./frontier -t stpath
```

The start of s-t paths (s) can be specified by ```-s``` and
the end can be specified by ```-t```.

```
./makegrid -d 3 3 | ./frontier -t stpath -s 2 -e 8
```

## Input format

### Input format for undirected graphs

```frontier``` supports the adjacent list format and the edge list format.
If ```-c``` option is specified, the edge list format is used.
Otherwise, the adjacent list format is used.

In the adjacent list format, put indices of vertices which is adjacent to vertex i
with space separators in line i.

In the edge list format, put the number of vertices in the first line.
We can omit the first line.
Then, put the source vertex index and the edge vertex index of each edge
in the second or later line.

Example of the edge list input

```
5
1 2
1 3
1 4
2 4
2 5
3 4
4 5
```

This represents the graph having 5 vertices and 7 edges. The first edge
connects vertex 1 and vertex 2, the second edge connects vertex 1 and vertex 3,
and so on.

### Input format for directed graphs

For directed graphs, only the edge list format is supported.
In each line, the first vertex is the source and the second vertex is
the destination of each directed edge.


## Output format

### ZDD

The format of an output ZDD is as follows:

```
\#1:
2:3,4
\#2:
3:5,5
4:5,6
\#3:
5:7,8
...
```

The line ```#i:``` means the beginning of the i-th edge variable.
```p:q,r``` represents that the node pointed by the 0-arc of node ```p```
is ```q```, and that by the 1-arc is ```r```.
0-terminal is the number 0, 1-terminal is the number 1,
and non-terminal is larger than 2.
The node number is represented in decimal digit, but by giving
```--hex``` option it is represented in hex form.
By giving ```-r``` option, the reduced ZDD is output.

### Drawing ZDDs by graphviz

You can draw ZDDs by graphviz.
By giving ```--print-zdd-graphviz```, the ZDD in graphviz format is output.

Example of s-t paths (the figure is output in png format):

```
./makegrid 3 | ./frontier -t stpath -n --print-zdd-graphviz - | dot -Tpng -o xxx.png
```

### Enumeration of subgraphs and Sampling

By ```--enum <filename>``` option, we can enumerate all the subgraphs. By ```--sample <filename> <n>``` option, we can sample <n> subgraphs. By specifying ```-``` for ```<filename>```, the result is output to ```stdout```.

Example of enumerating all the s-t paths:

```
./makegrid 3 | ./frontier -t stpath -n --enum -
```

Output:

```
1 3 5 6 7 8 11 12
1 3 5 8 9 12
1 3 5 10
1 4 6 7 11 12
1 4 8 10
1 4 9 12
2 3 4 5 6 10
2 3 4 5 7 9 10 11
2 6 8 10
2 6 9 12
2 7 8 9 10 11
2 7 11 12
```

One line represents one edge set. For example, ```1 3 5 6 7 8 11 12``` represents
edge set e_1, e_3,...,e_12.

Example of uniformly random sampling of s-t paths (10 paths)

```
./makegrid 3 | ./frontier -t stpath -n --sample - 20
```


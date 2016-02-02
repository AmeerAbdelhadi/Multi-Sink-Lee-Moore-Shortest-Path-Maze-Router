## Multi-Sink Lee-Moore Shortest Path Maze Router ##
## Ameer M. Abdelhadi ; ameer.abdelhadi@gmail.com ##
## The University of British Columbia (UBC)  2011 ##

## Problem Definition: ##

This is an implemention of a Multi-Sink Lee-Moore Shortest Path Maze Router.
The router reads a floorplan with list of blockages and list of wires, each with a source and a list of sinks.
For each wire, the router tries to connect all the source to all sinks.

#### Input File: ####

The router reads in a file that indicates:
* The grid size
* List of grid points that are covered by cells (obstructions)
* A list of wires, indicating, for each wire, which grid points are to be connected

An example input file is shown below:
```
12 9
18
8 2
9 2
10 2
2 4
3 4
4 4
5 4
6 4
6 5
6 6
6 7
2 8
3 8
4 8
9 5
9 6
9 7
9 8
2
2 2 8 10 2
3 8 2 6 7 9 7
```

The first line of this file (12 9) indicates the grid size to use (12x9 grids).
The second line (18) indicates the number of obstructed cells due to grids.
The grid locations that are obstructed are then listed, one location per line.
For example, line 3 indicates that grid location (8,2) is occupied by a cell and can not be used for routing.
In this example, there are 18 such obstructed cells, ending in cell (9,8).

Following the obstructed cells is the number of wires to route.
In this example, there are 2 wires that must be routed.
Then, the wires are listed, one per line. Each wire contains at least 5 numbers.
The first number indicates the number of pins in this wire
(a wire connecting a single source to a single sink would have 2 pins).
In the example, the first wire has 2 pins, while the second wire has 3 pins
(so it has one source and two sinks).
After the number of pins, the source is listed followed by all the sinks.
So in the example, the first wire’s source is (2,8) and its sink is (10,2).
The second wire’s source is (8,2) and its two sinks are (6,7) and (9,7).

#### Nets routing ordering: ####

A scattered net is more likely to block other nets, hence, it’s recommended to deal with scattered nets last.
The bounding box area of a specific net gives an indication about how the net is scattered.
Hence, nets are ordered in the routing queue by their bounding box area.

#### Multi-destination routing method: ####

As described in Lee-Moore shortest path algorithm, a wave will be expanded from a designated source until it hits any correspondent target.
The expansion path will be traced back to the source.
If the same source has another destination, a new wave will be expanded from the previously routed net,
including the source and the previously routed target. The same operation will be repeated to all destinations.

#### Trace-back method: ####

Some nets have several shortest paths, rather than one. Hence, a source could be traced back from the corresponding target in several paths.
Two trace-back methods have been implemented as follows.
Minimum turn method: avoid turns, namely, while tracing-back, keep same direction as first priority.
Direct path method: use paths that get you closer to the source in Manhattan plan,
e.g. if the source is located in the upper-right corner, use up and right direction as first priority.

#### Rip-up and reroute method: ####

A wire is unroutable if the wave expansion from his source does not hit any of his targets (assuming that some targets still unrouted).
In order to speculate which wire prevents the routability of the current wire,
a wave will be expanded from the current source and the current unrouted targets of the same source,
each separately (notice reversed expansion from targets).
If any of these wave expansions tackles only one wire, then this wire is definitely blocking and should be ripped-up.
Otherwise, the maximum wire to hit the expansion wave is chosen, since this wire is most likely to be blocking.
The blocking wire is ripped-up; the previously unroutable wire is routed first, then the ripped-up wire (order is reversed).

- - - -

## Tool Installation: ##

Using a Linux machine (supporting X11), invoke:

```
make
make clean
```

- - - -

## Tool Usage: ##

```
  maze [OPTIONS] INFILE
```
  
**Options:**
```
  -help       (also -h): print this message
  -verbose    (also -v): verbose logging
  -postscript (also -p): generate PostScript every step
  -traceback  (also -t): traceback mode, followed by one of the following
    * minturn     (also m): avoid turns, try to keep same direction (default)
    * direct      (also d): use direct paths toward source
  -stepping   (also -s): stepping mode, followed by one of the following
    * wave        (also w): wave expansion (default)
    * destination (also d): route one destination at once
    * net         (also n): route one net at once
    * routeall    (also n): route all nets at once
```
	
**infile syntax:**
```
  <FLOORPLAN COORDINATES>
  <BLOCKAGES#>
  <BLOCKAGE 1 COORDINATE>
  <BLOCKAGE 2 COORDINATE>
  ...
  <BLOCKAGE b COORDINATE>
  <NETS#>
  <NET 1 TARGETS#> <DEST. 1 COORD.> ... <DEST. t1 COORD.>
  ...
  <NET 2 TARGETS#> <DEST. 1 COORD.> ... <DEST. t2 COORD.>
  <NET n TARGETS#> <DEST. 1 COORD.> ... <DEST. tm COORD.>
```

**Examples:**
```
  maze a.infile (using default options)
  maze a.infile -verbose -postscript -stepping destination
  maze a.infile -v -p -s d (same as above)
```

- - - -

## Experimental results: ##

Testcase | Routable nets | Overall nets
-------- | ------------- | ------------
a.infile | 2             | 3
b.infile | 3             | 3
c.infile | 2             | 3
d.infile | 3             | 4
e.infile | 3             | 4
f.infile | 4             | 5
g.infile | 3             | 5
h.infile | 3             | 4
i.infile | 3             | 6
j.infile | 6             | 10
k.infile | 5             | 10


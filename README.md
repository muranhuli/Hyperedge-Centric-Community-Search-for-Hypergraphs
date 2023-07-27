# Hyperedge-Centric Community Search for Hypergraphs

## Introduction

This repository contains the source code for Hyperedge-Centric Community Search for Hypergraphs.
Below is a detailed description of the function of each directory and file.

## Experimental Dataset

The experimental datasets are obtained from http://www.cs.cornell.edu/~arb/data/

## Directory Structure

- kCoreDecomp.cpp: The source code for the k-core decomposition algorithm.
- kxCoreDecomp.cpp: The source code for the (k,s)-core decomposition algorithm.
- kxLocalSearch.cpp: The source code for the local neighbor search algorithm.
- kxIndexSearch.cpp: The source code for the Index-tree search algorithm.
- incidenthypergraph.cpp: The source code for the incident hypergraph data structure.
- compute.cpp: The source code for the computation of Density and Number of triplets.
- coreValueDistribution.py: The source code for the core value distribution.

## Running the algorithm

1. Configure the corresponding file path
2. Compile the source code and run using the following command:
```make``` and ```./xxx.out```

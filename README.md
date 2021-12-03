# Distributed Graph Coloring
This is an implementation of the algorithm proposed by Yannic Maus in the following paper:

Yannic Maus. 2021. Distributed Graph Coloring Made Easy. In Proceedings of the 33rd ACM Symposium on Parallelism in Algorithms and Architectures (SPAA '21). Association for Computing Machinery, New York, NY, USA, 362–372. DOI:https://doi.org/10.1145/3409964.3461804


[Link](https://arxiv.org/pdf/2105.05575.pdf) to the paper on arxiv

The algorithm is a round-based graph coloring algorithm, implemented for COMP5704.  [Project website](https://www.worley.dev/COMP5704/home)

## Running Information (Main program)
This program was tested with Ubuntu 18
### Prerequisites
- C++ with MPI 
- python3, matplotlib, scipy, and networkx if you wish to use the visualizer

### Running the program
Once the MPI hostname file is configured appropriately, the program can be run by navigating to the directory its located in and executing 


```./_compile``` 


then


```./_run < <input file>``` 


The program will prompt for input if inpout redirection is not supplied. Sample input files on graphs of different size are provided in /io/


Example run: 


```./_run < ./io/100.in > 100.out``` 


to redirect the output to a text file for use in the visualizer

## Running Information (provided tools)
There are two provided tools: 
- graphgenerator.py to generate input for the main algorithm \\
- graphviz.py to process output from the graph and confirm its results are correct

### Graph Generator
graphgenerator.py must be opened to change the V and E variables it contains. With these set it will generate a random graph on V vertices with E edges, possibly adding extra to ensure the graph is connected. The graph will be outputted and a V-coloring will be outputted by giving each vertex _i_ its own color _i_

### Graph Visualizer
graphviz.py takes in an output file from the main algorithm that contains the number of vertices, edges, the max degree of the graph, the amount of rounds the algorithm took, and the resulting coloring. With this information and processes the graph, ensures the given coloring is correct, and if the graph is smaller than a given threshold it creates a diagram for the graph using networkx

## Full run example
To generate an input file (assuming graphgenerator has been modified such that V=100):


```python3 ./tools/graphgenerator > ./io/100.in ``` 


To compile the distributed coloring algorithm:


```./_compile```


To run the algorithm on the generated graph and write the output to a file:


```./_run < ./io/100.in > ./io/100.out```


To process the output and obtain the visualization for small graphs:


```python3 ./tools/graphviz.py < ./io/100.out```




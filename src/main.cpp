# include <cstdlib>
# include <iostream>
# include "mpi.h"
#include <cmath>
#include <vector>
#include <algorithm>

using namespace std;

int logb(int base, int n) {
	return log(n)/log(base);
}


bool isPrime(int p) {
	for (int i = 2; i < ceil(sqrt(p)); i++) {
		if (p%i == 0) {
			return false;
		}
	}
	return true;
}

int nextPrime(int q) {
	int n = q+1;
	while (1) {
		if (isPrime(n)) {
			return n;
		}
		n++;
	}
}



int main(int argc, char *argv[])

{
  int id;
  int p;
  double wtime;
  
  MPI::Init(argc, argv); //  Initialize MPI.
  p = MPI::COMM_WORLD.Get_size(); //  Get the number of processes.
  id = MPI::COMM_WORLD.Get_rank(); //  Get the individual process ID.
  MPI_Status Stat;

  //PARAMETERS
  int VERTICES;
  int EDGES;
  int DELTA;
  int m; //typically want this O(Delta^2) for linials, 
  int f;
  int q;
  double start;
  
   //read in and broadcast parameters
   if (id == 0) {
	  start = MPI_Wtime();
	  cin >> VERTICES;
	  cin >> EDGES;
	  cin >> DELTA;
	  cin >> m;
	  //printed for output processing later
	  cout << VERTICES << endl;
	  cout << EDGES << endl; 
	  cout << DELTA << endl;
   }
   
   MPI_Bcast(&VERTICES, 1, MPI_INT, 0, MPI_COMM_WORLD);
   MPI_Bcast(&EDGES, 1, MPI_INT, 0, MPI_COMM_WORLD);
   MPI_Bcast(&DELTA, 1, MPI_INT, 0, MPI_COMM_WORLD);
   MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
   
   
  int graph[VERTICES][VERTICES] = {0};
  int colors[VERTICES] = {-1};
  int active[VERTICES];
  fill_n(active, VERTICES, 1);
  
  
  //id 0 reads in graph
  if (id == 0) {
	  int x,y;
	  //read in graphs and initial coloring
	  for (int i = 0; i < EDGES; i++) {
		cin >> x >> y;
		graph[x][y] = 1;
		graph[y][x] = 1;
		cout << x << " " << y << endl;	//printed for output processing later
	  }
	  int c;
	  for (int i = 0; i < VERTICES; i++) {
		cin >> c;
		colors[i] = c;
	  }

  }



	//SEND graph 
	for (int i = 0; i < VERTICES; i++) {
		MPI_Bcast(&graph[i], VERTICES, MPI_INT, 0, MPI_COMM_WORLD);
    }
    //SEND coloring + graph info
    MPI_Bcast(&colors, VERTICES, MPI_INT, 0, MPI_COMM_WORLD);

  //implementation currently assumes d=0 but formulas are left in with respect to d
  //Future work could involve implementing more advanced conflict checking so d-defective 
  //colorings can be properly calculated
  int d = 0;
  
  int X = 4*(ceil(DELTA/(d+1)))*ceil(logb(DELTA/(d+1), m));
  f = ceil(logb(DELTA/(d+1), m));
  
  
  // k must be chosen such that its less than X
  int k=min(5, X/2);
  
  int ROUNDS = ceil(X/k);

    
    //find next prime 
  q = nextPrime(2*f*DELTA);


	if (id == 0) {
	  cout << ROUNDS << endl;
  }
 
  
  //calculate polynomials for each input color
  int polys[m][f+1];
  int polycount = 0;
  while (polycount < m) {
	  int num = 1; //coefficient
	  for (int a = f; a >= 0; a--) {
		polys[polycount][a] = ((num+polycount)/int(pow(10,f-a)))%q;
		num++;
	  }
	  polycount++;
  }

  
  int n, nc, si;	  
  for (int round = 0; round < ROUNDS; round++) {
	  // broadcast most recent coloring
	 MPI_Bcast(&colors, VERTICES, MPI_INT, 0, MPI_COMM_WORLD);
     n = id;
     if (id == 0) {
		 int seqs[VERTICES][k] = {0};
	
		 //The main node collects the color sequences for each node and processes them
		 for (int i = 0; i < VERTICES; i++) {
			 MPI_Recv(&seqs[i], k, MPI_INT, MPI_ANY_SOURCE , i, MPI_COMM_WORLD, &Stat);
		 }
		 
		 // loop over the vertices
		 for (int v = 0; v < VERTICES; v++) {
			 int index = 0;		//track where we are in v's sequence
			 bool flag = false;
			 
			 //if this node is active, try to color it
			 if (active[v] == 1) {
				 flag = true; 
				 //find all neighbours of v
				 for (int j = 0; j < VERTICES; j++) {
					 if (index >= k) {
						 flag = false;
						 break;
					 }
					 //if v is adjacent to a permanently colored node with the same color, move up the seq
					 if (graph[v][j] == 1 && active[j] == 0 && colors[j] == seqs[v][index]) {
						 index++;
					}
				}
			 }
			 if (flag) {
				 active[v] = 0;
				 colors[v] = seqs[v][index];
			 }
		 }
		 
	 }
	 if (id != 0) {
		 //process vertices for processor n that are in the mod class VERTICES%n
		 while (n < VERTICES) {
			 n--;
			 int seq[k] = {0};
			 //try and locally color vertex n
			 nc = colors[n];
			 //generate sequence from nth polynomial
			 for (int i = 0; i < k; i ++) {
				 si = 0;
				 //calculate si by sampling the polynomial
				 for (int j = 0; j <= f; j++) {
					 si+= polys[nc][j]*int(pow(i,j));
				 }
				 seq[i] = si%q;
			 }
			
			// I know MPI_Alltoall is better, but MPI_Send + an aggregator processor seems to work better 
			 // for this case then doing an alltoall then having each node also filter its neighbours 
			 // since it needs to track the sequences from other nodes as well 

			 //send the sequence to node 0 for processing 
			 MPI_Send(&seq, k, MPI_INT, 0, n, MPI_COMM_WORLD); 
			 
			 
			 if (n + p - 1 < VERTICES) {
				int np = n+p-1;
				int seq[k] = {0};
				//try and locally color vertex n
				nc = colors[np];
				//generate sequence from nth polynomial
				for (int i = 0; i < k; i ++) {
					si = 0;
					//calculate si by sampling the polynomial
					for (int j = 0; j <= f; j++) {
						si+= polys[nc][j]*int(pow(i,j));		
					}
					seq[i] = si;
				 }

				//send the sequence to node 0 for processing 
				MPI_Send(&seq, k, MPI_INT, 0, np, MPI_COMM_WORLD); 
			 }
			 n += p;

		 }
		 
		 // handle processor i's portion of processor 0's mod class
		 
	 }
  }

  if (id == 0) {
	  for (int i=0; i < VERTICES; i++) {
		  cout << colors[i] << endl;
	  }
	  double end = MPI_Wtime();
	  cout << "Time in seconds: " << end-start << endl;
  }
  MPI::Finalize();
  return 0;
}



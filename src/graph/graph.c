/*
 * C Adjacency Matrix graph
 * @author Andre Abreu Calfa
 */
#include <stdio.h>
#include <stdlib.h>

/*
 * Adjacency Matrix Struct
 */
typedef struct adjacencyMatrix
{
	int n;
	int *adj;
	int *visited; /* bool to mark visited nodes */
}AdjacencyMatrix;

AdjacencyMatrix* createGraph(int n)
{
	AdjacencyMatrix *graph = (AdjacencyMatrix*) malloc(sizeof(AdjacencyMatrix));
	graph->n = n;
	graph->visited = (int*) malloc(n * sizeof(int));
	int * mat = (int*) malloc(n * n * sizeof(int));
	for (int i = 0; i < n; i++)
	{
		for(int j = 0; j < n; j++)
		{
			mat[i*n + j] = 0;
		}
	}
	graph->adj = mat;
	return graph;
}

/*
 * Adding Edge to Graph
 */ 
void add_edge(AdjacencyMatrix *graph, int origin, int destin)
{
	int n = graph->n;
	if( origin > n || destin > n || origin < 0 || destin < 0)
	{   
		printf("Invalid edge!\n");
	}
	else
	{
		graph->adj[origin*n + destin] = 1;
	}
}
/*
 * Print the graph
 */ 
void display(AdjacencyMatrix *graph)
{
	int i,j;
	int n = graph->n;
	for(i = 0;i < n;i++)
	{
		for(j = 0; j < n; j++)
			printf("%d  ", graph->adj[i*n + j]);
		printf("\n");
	}
}

/*
 * Main
 */ 
int main()
{
	int nodes, max_edges, origin, destin;
	printf("Enter number of nodes: ");
	scanf("%d", &nodes);
	AdjacencyMatrix *graph = createGraph(nodes);
	max_edges = nodes * (nodes - 1);
	for (int i = 0; i < max_edges; i++)
	{
		printf("Enter edge (-1 -1 to exit): ");
		scanf("%d %d", &origin, &destin);
		if((origin == -1) && (destin == -1))
			break;
		add_edge(graph, origin, destin);
	}
	display(graph);
	return 0;
}

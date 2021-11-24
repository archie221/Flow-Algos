#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef struct EDGE
{
	int y;
	int c;
	int f;
	struct EDGE *next;
}EDGE;

typedef struct VERTEX
{
	int x;
	int n;
	EDGE *p;
}VERTEX;

typedef struct GRAPH
{
	int V;
	int E;
	VERTEX *H;
}GRAPH;

GRAPH *ReadGraph(char *fname)
{
	FILE *fptr;
	fptr = fopen(fname,"r");
	if(fptr==NULL)
		printf("Error in File Creation");
	GRAPH* G;
	G = (GRAPH*)malloc(sizeof(GRAPH));
	int v,e;
	fscanf(fptr,"%d %d",&v,&e);
	G->V = v;
	G->E = e;
	VERTEX* h;
	h = (VERTEX*)malloc((v+2)*sizeof(VERTEX));
	for(int i=0;i<v;i++)
	{
		h[i].x = i+1;
		fscanf(fptr,"%d",&h[i].n);
		h[i].p = NULL;
	}
	EDGE* s[v]; 
	for(int i=0;i<v;i++)
		s[i] = NULL;
	for(int i=0;i<e;i++)
	{
		int n1,n2,c;
		fscanf(fptr,"%d %d %d",&n1,&n2,&c);
		EDGE *newedge;
		newedge = (EDGE*)malloc(sizeof(EDGE));
		newedge->y = n2;
		newedge->c = c;
		newedge->f = 0;
		newedge->next = NULL;
		if(h[n1-1].p==NULL)
		{
			s[n1-1] = newedge;
			h[n1-1].p = newedge;
		}
		else
		{
			h[n1-1].p->next = newedge;
			h[n1-1].p = h[n1-1].p->next;
		}
	}
	for(int i=0;i<v;i++)
		h[i].p = s[i];
	G->H = h;
	fclose(fptr);
	return G;
}

void PrintGraph(GRAPH *G)
{
	int v = G->V;
	VERTEX* h = G->H; 
	for(int i=0;i<v;i++)
	{
		printf("%d",h[i].x);
		EDGE* temp = h[i].p;
		while(temp!=NULL)
		{
			if(temp->y>v)
				break;
			printf("->(%d,%d,%d)",temp->y,temp->c,temp->f);
			temp = temp->next;
		}
		printf("\n");
	}
	printf("\n");
}

/*+++++++++++Extra Structures and Functions for Computing Max Flow++++++++++++*/

/*+++Queue Implementation+++*/ 

struct QNode 
{ 
    int key; 
    struct QNode* next; 
}; 
  
struct Queue 
{ 
    struct QNode *front, *rear; 
}; 
  
struct QNode* newNode(int k) 
{ 
    struct QNode* temp = (struct QNode*)malloc(sizeof(struct QNode)); 
    temp->key = k; 
    temp->next = NULL; 
    return temp; 
} 

struct Queue* createQueue() 
{ 
    struct Queue* q = (struct Queue*)malloc(sizeof(struct Queue)); 
    q->front = q->rear = NULL; 
    return q; 
} 
  
void push(struct Queue* q, int k) 
{ 
    struct QNode* temp = newNode(k); 
    if (q->rear == NULL) 
    { 
        q->front = q->rear = temp; 
        return; 
    } 
    q->rear->next = temp; 
    q->rear = temp; 
} 
  
void pop(struct Queue* q) 
{ 
    if (q->front == NULL) 
        return; 
    struct QNode* temp = q->front; 
    q->front = q->front->next; 
    if (q->front == NULL) 
        q->rear = NULL; 
    free(temp); 
} 

/*---Queue Implementation---*/ 

int min(int a, int b)
{
	if(a>b)
		return b;
	return a;
}

//Function to find the shortest path having max residual cappacity implemented using breadth first search

void bfs_res(int v, int res[v+1][v+1], int ans[v+1], int s, int t)
{
	int parent[v+1], visited[v+1], flow[v+1], level[v+1];
	int count = 1, flag = 0, lvl = 0;
	for(int i=0;i<v+1;i++)
		visited[i] = 0;
	parent[s] = s;
	level[s] = 0;
	flow[s] = INT_MAX;

	struct Queue* q = createQueue();
	push(q,s);
	visited[s] = 1;
	while(q->front!=NULL)  //bfs ends when queue becomes empty i.e. all nodes are traversed
	{
		int cnt = count;
		count = 0;
		lvl++;
		if(lvl>=v)
			break;
		for(int i=0;i<cnt;i++)
		{
			int top = q->front->key;
			pop(q);
			for(int j=1;j<v+1;j++)
			{
				if(res[top][j]>0&&visited[j]!=1)              //pushing the node in queue if not visited and has residual capacity
				{
					push(q,j);
					count++;
					visited[j] = 1;
					parent[j] = top;
					flow[j] = min(flow[top],res[top][j]);     //storing flow of node as the minimum residual capacity on the path which it is present 
					level[j] = lvl;							  //storing level on which node is traversed
					if(j==t)
						flag = 1;							  //if at a level sink is present then traversing next level is not required
				}
				else if(res[top][j]>0&&visited[j]==1)         //visiting a node again can change its parent under the below condition
				{
					if(level[top]<level[j] && min(flow[top],res[top][j])>flow[j])  //if parent exists at a lower level and residual capacity of that path is greater
					{															   //then parent is updated
						parent[j] = top;
						flow[j] = min(flow[top],res[top][j]);
					}
				}
			}
		}
		if(flag==1)
			break;
	}
	if(flag==1)    //returning answer when sink is found on a path
	{
		int k = t;
		while(parent[k]!=k)
		{
			ans[parent[k]] = k;    //storing child of nodes starting from source in answer array
			k = parent[k];
		}
		ans[0] = flow[t];
	}
	else
		ans[0] = -1;  //returning -1 when no path to sink is found
	return;
}

/*+++++++++++Extra Structures and Functions for Computing Max Flow++++++++++++*/

void ComputeMaxFlow(GRAPH *G, int s, int t)
{
	int v = G->V;
	VERTEX* h = G->H; 

	int resmatrix[v+1][v+1];
	for(int i=0;i<v+1;++i)
	{
		for(int j=0;j<v+1;j++)
		{
			resmatrix[i][j] = 0;
		}
	}
	for(int i=1;i<v+1;i++)							//creating residual matrix
	{
		EDGE* temp = h[i-1].p;
		while(temp!=NULL)
		{
			resmatrix[i][temp->y] = temp->c;
			temp = temp->next;
		}
	}

	int maxflow = 0;
	while(1)          								//running ford-fulkerson till path to sink exists
	{
		int aug_path[v+1];
		bfs_res(v,resmatrix,aug_path,s,t);          //finding shortest + max residual capacity augmenting path  
		if(aug_path[0]==-1)							//exiting when no augmenting path present
			break;
		
		maxflow += aug_path[0];

		int k = s;
		while(k!=t)										
		{
			resmatrix[k][aug_path[k]] -= aug_path[0];		//updating residual capacity matrix
			resmatrix[aug_path[k]][k] += aug_path[0];

			EDGE *temp = h[k-1].p;
			while(temp!=NULL)								//updating flow values in graph
			{
				if(temp->y==aug_path[k])
					temp->f += aug_path[0];
				temp = temp->next;
			}

			k = aug_path[k]; 
		}
	}

	printf("%d is the MAXFLOW for the given graph :)\n",maxflow);  

	return;
}

void NeedBasedFlow(GRAPH *G)
{
	int prod_need = 0, cons_need = 0;
	G->V += 2;									//creating dummy source and dummy sink by increasing no of vertices
	int v = G->V;
	VERTEX* h = G->H;
	EDGE* s[2];
	s[0] = NULL;
	s[1] = NULL;
	h[v-2].x = v-1;
	h[v-1].x = v;
	for(int i=0;i<v-2;i++)						//creating edges for dummy vertices
	{
		if(h[i].n<0)							//installing edges from dummy source to producers
		{
			int abs_need = (-1)*h[i].n;
			prod_need += abs_need;
			EDGE *newedge;
			newedge = (EDGE*)malloc(sizeof(EDGE));
			newedge->y = i+1;
			newedge->c = abs_need;
			newedge->f = 0;
			if(h[v-2].p==NULL)
			{
				s[0] = newedge;
				h[v-2].p = newedge;
			}
			else
			{
				h[v-2].p->next = newedge;
				h[v-2].p = h[v-2].p->next;
			}
		}
		else if(h[i].n>0)					   //installing edges from consumers to dummy sink
		{
			cons_need += h[i].n;
			EDGE *newedge;
			newedge = (EDGE*)malloc(sizeof(EDGE));
			newedge->y = v;
			newedge->c = h[i].n;
			newedge->f = 0;
			if(h[i].p==NULL)
				h[i].p = newedge;
			else
			{
				EDGE* temp = h[i].p;
				while(temp->next!=NULL)
					temp = temp->next;
				temp->next = newedge;	
			}
			
		}
	}
	h[v-2].p = s[0];
	h[v-1].p = s[1];

	if(prod_need!=cons_need)    //as inflow and outflow difference condition can't be maintained at each vertice 
	{
		printf("As Total Producer Need != Total Consumer Need\n");
		printf("\nNO FLOW ASSIGNMENT POSSIBLE(for the given need values)\n");
		printf("\nHence, the graph shown below has been assigned 0 as flow values\n");
		G->V -= 2;
		return;	
	}

	ComputeMaxFlow(G,v-1,v);        //computing max flow for the updated graph with dummy source and sink

	int flag = 0;
	EDGE* temp = h[v-2].p;
	while(temp!=NULL)				//checking if flow through dummy source is satisfying the producer needs or not
	{
		if(temp->c!=temp->f)
		{
			flag = 1;
			break;
		}
		temp = temp->next;
	}

	if(flag)						//if producer needs not satisfied hence inflow and outflow difference not maintained
	{
		printf("%d is the total producer need\n",prod_need);
		printf("As TOTAL PRODUCER NEED != MAXFLOW\n");
		printf("\nNO FLOW ASSIGNMENT POSSIBLE\n");
		printf("\nHence, the graph shown below is IRRELEVANT\n");
		G->V -= 2;
		return;
	}

	G->V -= 2;						//removing dummy source and dummy sink
	return;

}

int main()
{
	char file[25];
	printf("Enter the input filename(with extension):");
	scanf("%s",file);

	GRAPH *g;
	g = ReadGraph(file);
	PrintGraph(g);

	int s,t;
	printf("Enter source id from [1,%d]:",g->V);
	scanf("%d",&s);
	printf("Enter sink id from [1,%d]:",g->V);
	scanf("%d",&t);

	printf("\n<COMPUTE MAX FLOW>\n");
	ComputeMaxFlow(g,s,t);
	PrintGraph(g);

	printf("<NEED BASED FLOW>\n");
	g = ReadGraph(file);
	NeedBasedFlow(g);
	PrintGraph(g);

	return 0;
}

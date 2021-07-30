#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define N 1024

typedef struct {
    int key;
    long distance;
}GraphNode;

typedef struct Nd{
    int key;
    int color;
    long payload;
    struct Nd *sx, *dx, *p;
}TreeNode;

FILE * fp;
TreeNode * root;

void printTopK();
void saveMatrix(long ** currGraph, int d);
GraphNode * initQueue(long ** currGraph, int d);
void addToStructure(int id, long load, int kReached);   //kReached represents if TopK is full already or not yet. If it is, priority must be checked.
void quicksort(GraphNode * queue, int lo, int hi);
int partition(GraphNode * queue, int lo, int hi);
void switchNodes(GraphNode * queue, int i, int j);
void printQueue(GraphNode * queue, int d);
long dijkstraQueue(long ** currGraph, int d);
void insert(int id, long load);
void restoreRB(TreeNode * x);
void leftRotate(TreeNode * x);
void rightRotate(TreeNode * x);
void checkMax(long newValue);
TreeNode * maximum(TreeNode * curr);
void deleteMax(TreeNode * x);

int main(){

    int d, k, id = 0;
    char line[N];
    long ** currGraph;

    fp = fopen("open_tests\\input_1.txt", "r");
    //fp = stdin;

    fscanf(fp, "%d %d", &d, &k);
    currGraph = (long **) malloc(d * sizeof(long));
    for(int i=0; i<d; i++){
        currGraph[i] = (long *) malloc(d * sizeof(long));
    }

    root = NULL;

    fgets(line, N, fp);
    while(!feof(fp)) {
        if(strcmp(line, "AggiungiGrafo\n") == 0){
            saveMatrix(currGraph, d);

            long sumMinDistances = dijkstraQueue(currGraph, d);

            //printf("Result: %ld\n", sumMinDistances);

            printf("Graph %d\n", id);
            addToStructure(id, sumMinDistances, id >= k);
            id++;

        } else if(strcmp(line, "TopK\n") == 0){
            printTopK();
        }
        fgets(line, N, fp);
    }

    return 0;
}

long dijkstraQueue(long ** currGraph, int d){
    GraphNode * queue = NULL;
    int queueDim = d-1;
    long sum = 0;
    queue = initQueue(currGraph, d);

    while(queueDim != 0){
        quicksort(queue, 0, queueDim-1);
        //printQueue(queue, d);
        sum += queue->distance;
        GraphNode * u = queue;
        queue++;
        queueDim--;
        //for each v in u* check min(v.distance, u.distance+weight(u->v))
        for(int i=0; i<queueDim; i++){
            if(currGraph[u->key][queue[i].key] != 0 && currGraph[u->key][queue[i].key]+u->distance < queue[i].distance)
                queue[i].distance = currGraph[u->key][queue[i].key] + u->distance;
        }
    }

    return sum;
}

void saveMatrix(long ** currGraph, int d){
    for(int i=0; i<d; i++){
        int j = 0;
        do{
            fscanf(fp, "%ld", &currGraph[i][j++]);
        } while((char) fgetc(fp) != '\n');
    }
}

GraphNode * initQueue(long ** currGraph, int d){
    GraphNode * queue = malloc((d-1) * sizeof(GraphNode));
    for(int i=1; i<d; i++){
        queue[i-1].key = i;
        if(currGraph[0][i] == 0){
            queue[i-1].distance = (long) NULL;
        } else{
            queue[i-1].distance = (long) currGraph[0][i];
        }
    }
    return queue;
}

void quicksort(GraphNode * queue, int lo, int hi) {
    while(queue[hi].distance == 0)
        hi--;
    if (lo < hi) {
        int pivot = partition(queue, lo, hi);
        quicksort(queue, lo, pivot-1);
        quicksort(queue, pivot+1, hi);
    }
}

int partition(GraphNode *queue, int lo, int hi) {
    long pivot;
    pivot = queue[hi].distance;
    int i = lo - 1;
    for (int j=lo; j<hi; j++) {
        if (queue[j].distance <= pivot & queue[j].distance != 0) {
            i++;
            if (i != j)
                switchNodes(queue, i, j);
        }
    }
    switchNodes(queue, i+1, hi);
    return i + 1;
}

void switchNodes(GraphNode * queue, int i, int j) {
    if(i != j){
        int tempKey = queue[j].key;
        long tempDis = queue[j].distance;
        queue[j].key = queue[i].key;
        queue[j].distance = queue[i].distance;
        queue[i].key = tempKey;
        queue[i].distance = tempDis;
    }
}

void addToStructure(int id, long load, int kReached){
    /*if(kReached)
        checkMax(load);*/
    insert(id, load);
}

void insert(int id, long load){
    TreeNode * curr = root;
    TreeNode * prev = NULL;

    while(curr != NULL){
        prev = curr;
        if(id < curr->key)
            curr = curr->sx;
        else if(id >= curr->key)
            curr = curr->dx;
    }

    curr = (TreeNode *) malloc(sizeof(TreeNode));
    curr->color = 1;
    curr->key = id;
    curr->payload = load;

    if(prev == NULL){       //empty tree
        curr->dx = NULL;
        curr->sx = NULL;
        curr->p = NULL;
        curr->color = 0;
        root = curr;
    } else{
        if(prev->key < id)
            prev->dx = curr;
        else
            prev->sx = curr;
        curr->p = prev;
        restoreRB(curr);
    }
}

void restoreRB(TreeNode * x){
    TreeNode * y;
    while(x->p->color == 1){
        if(x->p == x->p->p->sx && x->p->p->dx != NULL){
            y = x->p->p->dx;
            if(y->color == 1){
                x->p->color = 0;
                y->color = 0;
                x->p->p->color = 1;
                x = x->p->p;
            }else if(x == x->p->dx){
                x = x->p;
                leftRotate(x);
            } else if(x == x->p->dx){
                x->p->color = 0;
                x->p->p->color = 1;
                rightRotate(x->p->p);
            }
        } else if(x->p->p->sx != NULL){
            y = x->p->p->sx;
            if(y->color == 1){
                x->p->color = 0;
                y->color = 0;
                x->p->p->color = 1;
                x = x->p->p;
            } else if(x == x->p->sx){
                x = x->p;
                rightRotate(x);
            } else{
                x->p->color = 0;
                x->p->p->color = 1;
                leftRotate(x->p->p);
            }
        } else  break;
    }
    while(root->p != NULL)
        root = root->p;
    root->color = 0;
}

void leftRotate(TreeNode * x){	//fatta
    TreeNode * y = x->dx;					//y punta al figlio destro del nodo X
    x->dx = y->sx;					//X punta dx al figlio sx di Y
    if(y->sx != NULL)
        y->sx->p = x;					//il figlio sx di Y riconosce X come nuovo padre
    y->p = x->p;						//Y sale di livello rconoscendo il padre di X come proprio
    if(x->p == NULL)
        root=y;						//Nel caso Y diventa radice
    else if(x == x->p->sx)				//se X è un figlio sx
        x->p->sx = y;					//Y prende il suo posto
    else
        x->p->dx = y;					//analogamente se era figlio dx
    y->sx = x;						//X scende di livello diventando figlio sx di Y
    x->p = y;							//Y diventa padre di X
}

void rightRotate(TreeNode * x){	//fatta
    TreeNode * y = x->sx;					//Y è figlio sx di X
    x->sx = y->dx;					//X punta sx al figlio dx di Y
    if(y->sx != NULL)
        y->dx->p = x;					//il figlio dx di Y riconosce X come nuovo padre
    y->p = x->p;						//Y sale di livello rconoscendo il padre di X come proprio
    if(x->p == NULL)
        root = y;						//Nel caso Y diventa radice
    else if(x == x->p->sx)				//se X è un figlio sx
        x->p->sx = y;					//Y prende il suo posto
    else
        x->p->dx = y;					//analogamente se era figlio dx
    y->dx = x;						//X scende di livello diventando figlio dx di Y
    x->p = y;							//Y diventa padre di X
}

void checkMax(long newValue){
    TreeNode * last = maximum(root);
    if(newValue < last->payload)
        deleteMax(last);
}

TreeNode * maximum(TreeNode * curr){		//fatta funziona bene
    if(curr->dx != NULL)
        return maximum(curr->dx);
    return curr;
}

void deleteMax(TreeNode * x){
    if(x == root)
        free(root);
    else if(x == x->p->dx)
        free(x->p->dx);
    else
        free(x->p->sx);
}

void printTopK(){
    printf("printTopK(): implementation needed");
}

void printQueue(GraphNode * queue, int d){
    for(int i=0; i<d-1; i++){
        printf("%d:%ld\n", queue[i].key, queue[i].distance);
    }
}
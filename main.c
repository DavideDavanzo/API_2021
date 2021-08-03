#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define N 600000
#define BLACK 0
#define RED 1

typedef struct {
    unsigned int key;
    unsigned long distance;
}GraphNode;

typedef struct Nd{
    unsigned int key;
    int color;
    unsigned long payload;
    struct Nd *sx, *dx, *p;
}TreeNode;

FILE * fp;
TreeNode * root;
TreeNode * max;

char * getLine();
void printTopK();
void saveMatrix(unsigned long ** currGraph, int d);
GraphNode * initQueue(unsigned long ** currGraph, int d);
void addToStructure(int id, unsigned long load, int kReached);   //kReached represents if TopK is full already or not yet. If it is, priority must be checked.
void quicksort(GraphNode * queue, int lo, int hi);
int partition(GraphNode * queue, int lo, int hi);
void switchNodes(GraphNode * queue, int i, int j);
void printQueue(GraphNode * queue, int d);
unsigned long dijkstraQueue(unsigned long ** currGraph, int d);
void insert(int id, unsigned long load);
void restoreRBInsert(TreeNode * x);
void restoreRBDelete(TreeNode * x);
void leftRotate(TreeNode * x);
void rightRotate(TreeNode * x);
int checkMax(unsigned long newValue);
TreeNode * maximum(TreeNode * curr);
TreeNode * minimum(TreeNode * curr);
void delete(TreeNode * x);
TreeNode * heirTo(TreeNode * x);
void printValue(TreeNode * treeNode);
int myAtoi(char * string);
unsigned long myStrtol(char * string);

int main(){

    int d, k, id = 0;
    char * line, buffer[N];
    unsigned long ** currGraph;

    fp = fopen("open_tests\\input_1.txt", "r");
    //fp = stdin;

    line = fgets(buffer, N, fp);

    d = myAtoi(line);
    while(*line != ' ')
        line++;
    line++;
    k = myAtoi(line);
    currGraph = (unsigned long **) malloc(d * sizeof(unsigned long));
    for(int i=0; i<d; i++){
        currGraph[i] = (unsigned long *) malloc(d * sizeof(unsigned long));
    }
    root = NULL;

    line = fgets(buffer, N, fp);

    while(!feof(fp)) {
        if(strcmp(line, "AggiungiGrafo\n") == 0){
            saveMatrix(currGraph, d);
            unsigned long sumMinDistances = dijkstraQueue(currGraph, d);

            //printf("%d: %lu\n", id, sumMinDistances);

            addToStructure(id, sumMinDistances, id >= k);
            id++;
            line = fgets(line, N, fp);
        } else if(strcmp(line, "TopK\n") == 0){
            printTopK();
            line = fgets(line, N, fp);
            printf("\n");
        }
    }
    return 0;
}

char * getLine(char * buffer){
    int i=0;
    do{
        buffer[i] = (char) getc_unlocked(fp);
        i++;
    }while(buffer[i-1] != '\n');
    return buffer;
}

unsigned long dijkstraQueue(unsigned long ** currGraph, int d){
    GraphNode * queue = initQueue(currGraph, d);
    GraphNode * temp = queue;
    int queueDim = d-1;
    unsigned long sum = 0;
    while(queueDim != 0){
        quicksort(queue, 0, queueDim-1);
        sum += queue->distance;
        GraphNode * u = queue;
        queue++;
        queueDim--;
        //for each v in u* check min(v.distance, u.distance+weight(u->v))
        if(u->distance != 0){
            for(int i=0; i<queueDim; i++){
                if(currGraph[u->key][queue[i].key] != 0 && (currGraph[u->key][queue[i].key]+u->distance < queue[i].distance || queue[i].distance == 0))
                    queue[i].distance = currGraph[u->key][queue[i].key] + u->distance;
            }
        }
    }
    free(temp);
    return sum;
}

void saveMatrix(unsigned long ** currGraph, int d){
    char * line, buffer[N];
    for(int i=0; i<d; i++){
        line = fgets(buffer, N, fp);
        for(int j=0; j<d; j++){
            currGraph[i][j] = myStrtol(line);
            while(*line != ',' && *line != '\n')
                line++;
            line++;
        }
    }
}

GraphNode * initQueue(unsigned long ** currGraph, int d){
    GraphNode * queue = malloc((d-1) * sizeof(GraphNode));
    for(int i=1; i<d; i++){
        queue[i-1].key = i;
        queue[i-1].distance = (unsigned long) currGraph[0][i];
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
    unsigned long pivot;
    pivot = queue[hi].distance;
    int i = lo - 1;
    for (int j=lo; j<hi; j++) {
        if (queue[j].distance <= pivot && queue[j].distance != 0) {
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
        unsigned long tempDis = queue[j].distance;
        queue[j].key = queue[i].key;
        queue[j].distance = queue[i].distance;
        queue[i].key = tempKey;
        queue[i].distance = tempDis;
    }
}

void addToStructure(int id, unsigned long load, int kReached){
    if(kReached){
        if(checkMax(load)){
            insert(id, load);
            max = maximum(root);
        }
    } else{
        insert(id, load);
        max = maximum(root);
    }
}

void insert(int id, unsigned long load){
    TreeNode * curr = root;
    TreeNode * prev = NULL;
    while(curr != NULL){
        prev = curr;
        if(load < curr->payload)
            curr = curr->sx;
        else if(load >= curr->payload)
            curr = curr->dx;
    }
    curr = (TreeNode *) malloc(sizeof(TreeNode));
    curr->color = RED;
    curr->key = id;
    curr->payload = load;
    curr->dx = NULL;
    curr->sx = NULL;
    if(prev == NULL){       //empty tree
        curr->p = NULL;
        curr->color = BLACK;
        root = curr;
    } else{
        if(prev->payload <= load)
            prev->dx = curr;
        else
            prev->sx = curr;
        curr->p = prev;
        restoreRBInsert(curr);
    }
}

void restoreRBInsert(TreeNode * x){
    TreeNode * y;
    while(x != root && x->color == RED && x->p->color == RED){
        if(x->p == x->p->p->sx){
            y = x->p->p->dx;
            if(y != NULL && y->color == RED){
                x->p->color = BLACK;
                y->color = BLACK;
                x->p->p->color = RED;
                x = x->p->p;
            } else{
                if(x == x->p->dx){
                    x = x->p;
                    leftRotate(x);
                }
                x->p->color = BLACK;
                x->p->p->color = RED;
                rightRotate(x->p->p);
            }
        } else{
            y = x->p->p->sx;
            if(y != NULL && y->color == RED){
                x->p->color = BLACK;
                y->color = BLACK;
                x->p->p->color = RED;
                x = x->p->p;
            } else{
                if(x == x->p->sx){
                    x = x->p;
                    rightRotate(x);
                }
                x->p->color = BLACK;
                x->p->p->color = RED;
                leftRotate(x->p->p);
            }
        }
    }
    root->color = BLACK;
}

void leftRotate(TreeNode * x){
    TreeNode * y = x->dx;
    x->dx = y->sx;					//X punta dx al figlio sx di Y
    if(y->sx != NULL)
        y->sx->p = x;					//il figlio sx di Y riconosce X come nuovo padre
    y->p = x->p;						//Y sale di livello rconoscendo il padre di X come proprio
    if(x->p == NULL)
        root = y;						//Nel caso Y diventa radice
    else if(x == x->p->sx)				//se X è un figlio sx
        x->p->sx = y;					//Y prende il suo posto
    else
        x->p->dx = y;					//analogamente se era figlio dx
    y->sx = x;						//X scende di livello diventando figlio sx di Y
    x->p = y;							//Y diventa padre di X
}

void rightRotate(TreeNode * x){
    TreeNode * y = x->sx;					//Y è figlio sx di X
    x->sx = y->dx;					//X punta sx al figlio dx di Y
    if(y->dx != NULL)
        y->dx->p = x;					//il figlio dx di Y riconosce X come nuovo padre
    y->p = x->p;						//Y sale di livello rconoscendo il padre di X come proprio
    if(x->p == NULL)
        root = y;						//Nel caso Y diventa radice
    else if(x == x->p->dx)				//se X è un figlio sx
        x->p->dx = y;					//Y prende il suo posto
    else
        x->p->sx = y;					//analogamente se era figlio dx
    y->dx = x;						//X scende di livello diventando figlio dx di Y
    x->p = y;							//Y diventa padre di X
}

int checkMax(unsigned long newValue){
    if(newValue < max->payload){
        delete(max);
        return 1;
    }
    return 0;
}

TreeNode * maximum(TreeNode * curr){
    if(curr->dx != NULL)
        return maximum(curr->dx);
    return curr;
}

void delete(TreeNode * x){
    TreeNode * toDelete, * subTree;
    if(x->sx == NULL || x->dx == NULL)
        toDelete = x;
    else    toDelete = heirTo(x);
    if(toDelete->sx != NULL)
        subTree = toDelete->sx;
    else    subTree = toDelete->dx;
    if(subTree != NULL)
        subTree->p = toDelete->p;
    if(toDelete->p == NULL)
        root = subTree;
    else if(toDelete == toDelete->p->sx)
        toDelete->p->sx = subTree;
    else    toDelete->p->dx = subTree;
    if(toDelete != x){
        x->key = toDelete->key;
        x->payload = toDelete->payload;
    }
    if(toDelete->color == BLACK){
        free(toDelete);
        if(subTree != NULL)
            restoreRBDelete(subTree);
    } else free(toDelete);
}

void restoreRBDelete(TreeNode * x){
    while(x != root && x->color == BLACK){
        if(x == x->p->sx){
            if(x->p->dx->color == RED){     //case 1
                x->p->color = RED;
                x->p->dx->color = BLACK;
                leftRotate(x->p);
            }
            if(x->p->dx->sx->color == BLACK && x->p->dx->color == BLACK){
                x->p->dx->color = RED;
                x = x->p;
            } else{
                if(x->p->dx->dx->color == BLACK){      //case 4
                    x->p->dx->color = RED;
                    x->p->dx->sx->color = BLACK;
                    rightRotate(x->p);
                }
                x->p->dx->color = x->p->color;
                x->p->dx->dx->color = BLACK;
                x->p->color = BLACK;
                leftRotate(x->p);
                x = root;
            }
        } else{
            if(x->p->sx->color == RED){     //case 1
                x->p->color = RED;
                x->p->sx->color = BLACK;
                rightRotate(x->p);
            }
            if(x->p->sx->color == BLACK &&x->p->sx->dx->color == BLACK){
                x->p->sx->color = RED;
                x = x->p;
            } else{
                if(x->p->sx->sx->color == BLACK){    //case 3 -> case 4
                    x->p->sx->color = RED;
                    x->p->sx->dx->color = BLACK;
                    leftRotate(x->p->sx);
                }
                x->p->sx->color = x->p->color;
                x->p->color = BLACK;
                x->p->sx->sx->color = BLACK;
                rightRotate(x->p);
                x = root;
            }
        }
    }
    x->color = BLACK;
}

TreeNode * heirTo(TreeNode * x){
    TreeNode * temp;
    if(x->dx != NULL)
        return minimum(x->dx);
    temp = x->p;
    while(temp != NULL && x == temp->dx){
        x = temp;
        temp = temp->p;
    }
    return temp;
}

TreeNode * minimum(TreeNode * curr){
    if(curr->sx != NULL)
        return minimum(curr->sx);
    return curr;
}

int myAtoi(char * string){
    int res = 0;
    for (int i=0; (string[i]>='0') && (string[i]<='9'); i++){
        res = 10 * res + (string[i] - '0');
    }
    return res;
}

unsigned long myStrtol(char * string){
    unsigned long res = 0;
    for (int i=0; (string[i]>='0') && (string[i]<='9'); i++){
        res = 10 * res + (string[i] - '0');
    }
    return res;
}

void printTopK(){
    printValue(root);
}

void printValue(TreeNode * treeNode){
    if(treeNode == NULL)
        return;
    printValue(treeNode->sx);
    if(treeNode == max)
        printf("%d", treeNode->key);
    else
        printf("%d ", treeNode->key);
    printValue(treeNode->dx);
}

void printQueue(GraphNode * queue, int dim){
    for(int i=0; i<dim; i++){
        printf("%d:%lu\n", queue[i].key, queue[i].distance);
    }
}
#include<stdio.h>
#include<stdlib.h>

#define BLACK 0
#define RED 1

typedef struct {
    int key;
    unsigned long distance;
}GraphNode;

typedef struct Nd{
    int key;
    int color;
    unsigned long payload;
    struct Nd *sx, *dx, *p;
}TreeNode;

FILE * fp;
TreeNode * root;
TreeNode * max;

void printTopK();
void saveMatrix(unsigned long ** currGraph, int d);
GraphNode * initQueue(unsigned long ** currGraph, int d, GraphNode * queue);
void addToStructure(int id, unsigned long load, int k);
unsigned long dijkstraQueue(unsigned long ** currGraph, int d, GraphNode * queue);
void insert(int id, unsigned long load);
void restoreRBInsert(TreeNode * x);
void restoreRBDelete(TreeNode * x);
void leftRotate(TreeNode * x);
void rightRotate(TreeNode * x);
int checkMax(unsigned long newValue);
TreeNode * maximum(TreeNode * curr);
TreeNode * minimum(TreeNode * curr);
void delete(TreeNode * x);
TreeNode * nextTo(TreeNode * x);
void printValue(TreeNode * treeNode);
int myAtoi();
unsigned long myStrtol();
void minFirst(GraphNode * queue, int dim);

int main(){
    int d, k, id = 0;
    unsigned long ** currGraph;

    fp = stdin;

    d = myAtoi();
    k = myAtoi();
    currGraph = (unsigned long **) malloc(d * sizeof(unsigned long));
    for(int i=0; i<d; i++){
        currGraph[i] = (unsigned long *) malloc(d * sizeof(unsigned long));
    }
    root = NULL;
    GraphNode * queue = malloc((d-1) * sizeof(GraphNode));
    char c = getc_unlocked(fp);
    while(!feof(fp)) {
        if(c == 'A'){
            while(c != '\n'){
                c = getc_unlocked(fp);
            }
            saveMatrix(currGraph, d);
            unsigned long sumMinDistances = dijkstraQueue(currGraph, d, queue);
            addToStructure(id, sumMinDistances, k);
            id++;
        } else if(c == 'T'){
            while(c != '\n'){
                c = getc_unlocked(fp);
            }
            printTopK();
            printf("\n");
        }
        c = getc_unlocked(fp);
    }
    return 0;
}

int myAtoi(){
    int res = 0;
    char c;
    while((c = getc_unlocked(fp)) != ' ' && c != '\n'){
        res = 10 * res + (c - '0');
    }
    return res;
}

unsigned long myStrtol(){
    unsigned long res = 0;
    char c;
    while((c = getc_unlocked(fp)) != ',' && c != '\n'){
        res = 10 * res + (c - '0');
    }
    return res;
}

unsigned long dijkstraQueue(unsigned long ** currGraph, int d, GraphNode * queue){
    initQueue(currGraph, d, queue);
    int queueDim = d-1;
    unsigned long sum = 0;
    while(queueDim != 0){
        minFirst(queue, queueDim);
        sum += queue->distance;
        GraphNode * u = queue;
        if(u->distance != 0){
            queue++;
            queueDim--;
            for(int i=0; i<queueDim; i++){
                if(currGraph[u->key][queue[i].key] != 0 && (currGraph[u->key][queue[i].key]+u->distance < queue[i].distance || queue[i].distance == 0))
                    queue[i].distance = currGraph[u->key][queue[i].key] + u->distance;
            }
        } else break;
    }
    return sum;
}

void minFirst(GraphNode * queue, int dim){
    GraphNode * min = queue;
    for(int i=1; i<dim; i++){
        if((queue[i].distance < min->distance || min->distance == 0) && queue[i].distance != 0)
            min = &queue[i];
    }
    if(min->distance != 0 && min != &queue[0]){
        int minKey = min->key;
        unsigned long minDis = min->distance;
        min->key = queue[0].key;
        min->distance = queue[0].distance;
        queue[0].key = minKey;
        queue[0].distance = minDis;
    }
}

void saveMatrix(unsigned long ** currGraph, int d){
    for(int i=0; i<d; i++){
        for(int j=0; j<d; j++){
            currGraph[i][j] = myStrtol();
        }
    }
}

GraphNode * initQueue(unsigned long ** currGraph, int d, GraphNode * queue){
    for(int i=1; i<d; i++){
        queue[i-1].key = i;
        queue[i-1].distance = (unsigned long) currGraph[0][i];
    }
    return queue;
}

void addToStructure(int id, unsigned long load, int k){
    if(id >= k){
        if(checkMax(load)){
            insert(id, load);
        }
    } else{
        insert(id, load);
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
    if(prev == NULL){
        curr->p = NULL;
        curr->color = BLACK;
        root = curr;
        max = root;
    } else{
        if(prev->payload <= load)
            prev->dx = curr;
        else
            prev->sx = curr;
        curr->p = prev;
        restoreRBInsert(curr);
        if(load >= max->payload)
            max = curr;
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
    x->dx = y->sx;
    if(y->sx != NULL)
        y->sx->p = x;
    y->p = x->p;
    if(x->p == NULL)
        root = y;
    else if(x == x->p->sx)
        x->p->sx = y;
    else
        x->p->dx = y;
    y->sx = x;
    x->p = y;
}

void rightRotate(TreeNode * x){
    TreeNode * y = x->sx;
    x->sx = y->dx;
    if(y->dx != NULL)
        y->dx->p = x;
    y->p = x->p;
    if(x->p == NULL)
        root = y;
    else if(x == x->p->dx)
        x->p->dx = y;
    else
        x->p->sx = y;
    y->dx = x;
    x->p = y;
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

    //max handling
    if(x->sx == NULL)
        max = x->p;
    else max = maximum(x->sx);

    //actual delete
    if(x->sx == NULL || x->dx == NULL)
        toDelete = x;
    else    toDelete = nextTo(x);
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

TreeNode * nextTo(TreeNode * x){
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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

typedef struct Node {
    int x, y;
    float cost;
    float heuristic;
    float total_cost; // Total cost = cost + heuristic
    struct Node* parent;
} Node;

typedef struct {
    Node **nodes;  // Array of node pointers
    int size;      // Current size of the priority queue
    int capacity;  // Maximum capacity of the priority queue
} PriorityQueue;

float heuristic(Node *current, Node *goal) {
    return sqrt(pow(current->x - goal->x, 2) + pow(current->y - goal->y, 2));
}

PriorityQueue* create_priority_queue(int capacity) {
    PriorityQueue *pq = (PriorityQueue *)malloc(sizeof(PriorityQueue));
    if (!pq) return NULL;

    pq->nodes = (Node **)malloc(sizeof(Node *) * capacity);
    if (!pq->nodes) {
        free(pq);
        return NULL;
    }

    pq->size = 0;
    pq->capacity = capacity;
    return pq;
}

bool resize_priority_queue(PriorityQueue *pq, int new_capacity) {
    Node **new_nodes = (Node **)realloc(pq->nodes, sizeof(Node *) * new_capacity);
    if (!new_nodes) return false;

    pq->nodes = new_nodes;
    pq->capacity = new_capacity;
    return true;
}

void heapify_up(PriorityQueue *pq, int index) {
    while (index > 0) {
        int parentIndex = (index - 1) / 2;
        if (pq->nodes[parentIndex]->total_cost > pq->nodes[index]->total_cost) {
            Node *temp = pq->nodes[parentIndex];
            pq->nodes[parentIndex] = pq->nodes[index];
            pq->nodes[index] = temp;
            index = parentIndex;
        } else {
            break;
        }
    }
}

void insert_priority_queue(PriorityQueue *pq, Node *node) {
    if (pq->size == pq->capacity && !resize_priority_queue(pq, pq->capacity * 2)) {
        printf("Failed to resize priority queue\n");
        return;
    }
    pq->nodes[pq->size] = node;
    pq->size++;
    heapify_up(pq, pq->size - 1);
}

void heapify_down(PriorityQueue *pq, int index) {
    int smallest = index;
    int leftChildIdx = 2 * index + 1;
    int rightChildIdx = 2 * index + 2;

    if (leftChildIdx < pq->size && pq->nodes[leftChildIdx]->total_cost < pq->nodes[smallest]->total_cost) {
        smallest = leftChildIdx;
    }

    if (rightChildIdx < pq->size && pq->nodes[rightChildIdx]->total_cost < pq->nodes[smallest]->total_cost) {
        smallest = rightChildIdx;
    }

    if (smallest != index) {
        Node *temp = pq->nodes[index];
        pq->nodes[index] = pq->nodes[smallest];
        pq->nodes[smallest] = temp;
        heapify_down(pq, smallest);
    }
}

Node* pop_priority_queue(PriorityQueue *pq) {
    if (pq->size == 0) return NULL;
    Node* top = pq->nodes[0];
    pq->nodes[0] = pq->nodes[pq->size - 1];
    pq->size--;
    heapify_down(pq, 0);
    return top;
}

bool is_empty_priority_queue(PriorityQueue *pq) {
    return pq->size == 0;
}

void find_neighbors(Node *current, Node **neighbors, int *neighbor_count, int grid_size) {
    *neighbor_count = 0;
    int dx[4] = {1, -1, 0, 0};
    int dy[4] = {0, 0, 1, -1};

    for (int i = 0; i < 4; ++i) {
        int newX = current->x + dx[i];
        int newY = current->y + dy[i];

        if (newX >= 0 && newX < grid_size && newY >= 0 && newY < grid_size) {
            Node *newNeighbor = (Node *)malloc(sizeof(Node));
            if (newNeighbor) {
                newNeighbor->x = newX;
                newNeighbor->y = newY;
                newNeighbor->cost = 0; // Will be set correctly when adding to open set
                newNeighbor->parent = current;
                neighbors[*neighbor_count] = newNeighbor;
                (*neighbor_count)++;
            }
        }
    }
}

void reconstruct_path(Node *current) {
    if (current == NULL) return;
    reconstruct_path(current->parent);
    printf("(%d, %d) -> ", current->x, current->y);
}

void a_star_algorithm(Node *start, Node *goal, int grid_size) {
    PriorityQueue *openSet = create_priority_queue(100);
    if (!openSet) {
        printf("Failed to create priority queue\n");
        return;
    }

    start->heuristic = heuristic(start, goal);
    start->total_cost = start->cost + start->heuristic;
    insert_priority_queue(openSet, start);

    while (!is_empty_priority_queue(openSet)) {
        Node *current = pop_priority_queue(openSet);

        if (current->x == goal->x && current->y == goal->y) {
            reconstruct_path(current);
            printf("Goal reached\n");
            break;
        }

        Node *neighbors[4];
        int neighbor_count = 0;
        find_neighbors(current, neighbors, &neighbor_count, grid_size);

        for (int i = 0; i < neighbor_count; i++) {
            Node *neighbor = neighbors[i];

            float tentative_gScore = current->cost + 1; // Assuming cost of 1 for each move
            if (tentative_gScore < neighbor->cost || neighbor->cost == 0) {
                neighbor->parent = current;
                neighbor->cost = tentative_gScore;
                neighbor->heuristic = heuristic(neighbor, goal);
                neighbor->total_cost = neighbor->cost + neighbor->heuristic;

                // This example does not include a closed set or a check to see if the neighbor is already in the open set with a lower cost.
                // This is a simplification. In a full implementation, you should add these checks to avoid processing nodes more than necessary.
                insert_priority_queue(openSet, neighbor);
            }
        }
    }

    // Freeing nodes and the priority queue not shown for brevity
}

int main() {
    Node start = {0, 0, 0, 0, 0, NULL};
    Node goal = {5, 5, 0, 0, 0, NULL};
    int grid_size = 10;

    a_star_algorithm(&start, &goal, grid_size);

    return 0;
}

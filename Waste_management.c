#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HEAP 100

typedef struct {
    int id;
    char area[30];
    int fill;       //! 0-100
    int priority;   //! smaller value => more urgent
} Request;

//?Queue 
typedef struct Node {
    Request data;
    struct Node *next;
} Node;

typedef struct {
    Node *front, *rear;
} Queue;

void initQueue(Queue *q) { q->front = q->rear = NULL; }
int isQueueEmpty(Queue *q) { return q->front == NULL; }

void enqueue(Queue *q, Request r) {
    Node *newNode = (Node*)malloc(sizeof(Node));
    newNode->data = r;
    newNode->next = NULL;
    if (q->rear == NULL) q->front = q->rear = newNode;
    else { q->rear->next = newNode; q->rear = newNode; }
    printf("Added request ID %d to queue.\n", r.id);
} 
// ?Removes the earliest request from the queue and returns it.
// ? Used when moving requests from Queue → Heap.

int dequeue(Queue *q, Request *r) {
    if (isQueueEmpty(q)) return 0;
    Node *temp = q->front;
    *r = temp->data;
    q->front = q->front->next;
    if (q->front == NULL) q->rear = NULL;
    free(temp);
    return 1;
}

void displayQueue(Queue *q) {
    if (isQueueEmpty(q)) {
        printf("Queue is empty.\n");
        return;
    }
    Node *cur = q->front;
    printf("\n-- Queue Contents --\n");
    while (cur) {
        printf("ID:%d  Area:%s  Fill:%d%%  Priority:%d\n",
               cur->data.id, cur->data.area, cur->data.fill, cur->data.priority);
        cur = cur->next;
    }
    printf("--------------------\n");
}

/* ------------------ Min Heap (Dynamic Array) ------------------ */
//?   Min Heap is used so that the bin with the **smallest priority number**
//?(meaning most urgent) is always available at index 0.
typedef struct {
    Request *arr;
    int size;
    int capacity;
} MinHeap;

void initHeap(MinHeap *h, int capacity) {
    h->arr = (Request*)malloc(capacity * sizeof(Request));
    h->size = 0;
    h->capacity = capacity;
}

void swap(Request *a, Request *b) { Request t = *a; *a = *b; *b = t; }
//!heapifyUp is used(fixes) when adding a new request to the heap.

void heapifyUp(MinHeap *h, int i) {
    while (i > 0) {
        int p = (i - 1) / 2;
        if (h->arr[p].priority <= h->arr[i].priority) break;
        swap(&h->arr[p], &h->arr[i]);
        i = p;
    }
}
//!heapifyDown is used when removing the root (the most urgent bin).
// If this new value is larger (less urgent) than one of its children,
// it swaps downward until it fits the heap rule.
void heapifyDown(MinHeap *h, int i) {
    while (1) {
        int l = 2 * i + 1, r = 2 * i + 2, s = i;
        if (l < h->size && h->arr[l].priority < h->arr[s].priority) s = l;
        if (r < h->size && h->arr[r].priority < h->arr[s].priority) s = r;
        if (s == i) break;
        swap(&h->arr[i], &h->arr[s]);
        i = s;
    }
}

void insertHeap(MinHeap *h, Request r) {
    if (h->size == h->capacity) {
        h->capacity *= 2;
        h->arr = (Request*)realloc(h->arr, h->capacity * sizeof(Request));
    }
    h->arr[h->size] = r;
    heapifyUp(h, h->size); //!
    h->size++;
}

int extractMin(MinHeap *h, Request *out) {
    if (h->size == 0) return 0;
    *out = h->arr[0];
    h->arr[0] = h->arr[h->size - 1];
    h->size--;
    heapifyDown(h, 0); //!
    return 1;
}

void displayHeap(MinHeap *h) {
    if (h->size == 0) { printf("Heap is empty.\n"); return; }
    printf("\n-- Heap (by priority) --\n");
    for (int i = 0; i < h->size; i++) {
        Request r = h->arr[i];
        printf("[%d] ID:%d  Area:%s  Fill:%d%%  Priority:%d\n",
               i, r.id, r.area, r.fill, r.priority);
    }
    printf("------------------------\n");
}

//!  Check if Bin ID already exists (in Queue or Heap)
int idExists(Queue *q, MinHeap *h, int id) {
    Node *cur = q->front;
    while (cur) {
        if (cur->data.id == id) return 1;
        cur = cur->next;
    }
    for (int i = 0; i < h->size; i++) {
        if (h->arr[i].id == id) return 1;
    }
    return 0;
}

// Admin chooses which bin to clean
void adminChooseBin(MinHeap *h) {
    if (h->size == 0) {
        printf("No bins in heap.\n");
        return;
    }
    displayHeap(h);
    int id;
    printf("Enter ID of bin to clean: ");
    scanf("%d", &id);

    int found = -1;
    for (int i = 0; i < h->size; i++) {
        if (h->arr[i].id == id) {
            found = i;
            break;
        }
    }
    if (found == -1) {
        printf("Bin ID not found.\n");
        return;
    }

    printf("Cleaning bin ID:%d (%s)... Done.\n", h->arr[found].id, h->arr[found].area);

    //? Remove it (replace with last and re-heapify)
    h->arr[found] = h->arr[h->size - 1];
    h->size--;
    heapifyDown(h, found);
}

//?MAIN
int main() {
    Queue q; 
    MinHeap h;
    initQueue(&q);
    initHeap(&h, 10);

    int role, choice;

    while (1) {
        printf("\n=== Main Menu ===\n");
        printf("1. Login as Admin\n");
        printf("2. Login as User\n");
        printf("3. Exit Program\n");
        printf("Enter choice: ");
        scanf("%d", &role);

        if (role == 1) {
            // Admin Mode
            while (1) {
                printf("\n===== Admin Menu =====\n");
                printf("1. Display Queue\n");
                printf("2. Move Queue -> Heap\n");
                printf("3. Display Heap\n");
                printf("4. Auto clean next (Extract-Min)\n");
                printf("5. Choose bin to clean (by ID)\n");
                printf("6. Logout to Main Menu\n");
                printf("Enter choice: ");
                scanf("%d", &choice);

                if (choice == 1) displayQueue(&q);
                else if (choice == 2) {
                    Request t;
                    while (dequeue(&q, &t)) insertHeap(&h, t);
                    printf("Moved all requests to heap.\n");
                }
                else if (choice == 3) displayHeap(&h);
                else if (choice == 4) {
                    Request next;
                    if (extractMin(&h, &next))
                        printf("Auto cleaned Bin ID:%d (%s)\n", next.id, next.area);
                    else printf("No bins to clean.\n");
                }
                else if (choice == 5) adminChooseBin(&h);
                else if (choice == 6) break; // go back to main menu
                else printf("Invalid choice.\n");
            }
        }

        else if (role == 2) {
            // User Mode
            while (1) {
                printf("\n===== User Menu =====\n");
                printf("1. Add Bin Request\n");
                printf("2. Display Queue\n");
                printf("3. Logout to Main Menu\n");
                printf("Enter choice: ");
                scanf("%d", &choice);

                if (choice == 1) {
                    Request r;
                    printf("Enter Bin ID: ");
                    scanf("%d", &r.id);
                    if (idExists(&q, &h, r.id)) {
                        printf("Bin ID already exists! Try another.\n");
                        continue;
                    }
                    printf("Enter Area: ");
                    scanf("%s", r.area);
                    printf("Enter Fill Level (0-100): ");
                    scanf("%d", &r.fill);
                    if (r.fill < 0) r.fill = 0;
                    if (r.fill > 100) r.fill = 100;
                    r.priority = 100 - r.fill;
                    enqueue(&q, r);
                }
                else if (choice == 2) displayQueue(&q);
                else if (choice == 3) break; // back to main menu
                else printf("Invalid choice.\n");
            }
        }

        else if (role == 3) {
            printf("Exiting program...\n");
            break;
        }

        else {
            printf("Invalid choice! Try again.\n");
        }
    }

    // !cleanup
    free(h.arr);
    Node *cur = q.front;
    while (cur) {
        Node *tmp = cur;
        cur = cur->next;
        free(tmp);
    }

    printf("Program ended.\n");
    return 0;
}
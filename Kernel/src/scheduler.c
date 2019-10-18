#include <scheduler.h>
#include <time.h>
#include <memoryManager.h>
#include <console.h>

#define MAX_PRIO 3
#define SIZE 4000

static Node *search(uint64_t pid);
static Node * newNode();
static void freeNode(Node * node);
static void cleanMem();
static uint64_t pow(int base, int exponent);

static Node *current;
static Node *address;
static int init;

uint64_t scheduler(uint64_t sp) {
    timer_handler();

    // si se mata ulyimo proceso porque termino, entonces sp que devuelvo es donde
    // ira el programa cuando no tiene procesos para correr?
    // habria que crear una especie de proceso idle
    // no puede no haber procesos

    switch(init){
        case 0: return sp; 
        case 1: init = 2; break;
        default: current->n.process.sp = sp; break;
    }
    
    // if (current->n.process.state == BLOCKED) {
    //     current->n.times = 0;
    //     current->n.next->n.process.state = RUNNING;
    //     return current->n.next->n.process.sp;
    // }
    current->n.times++;
    if(current->n.times == pow(2, MAX_PRIO - current->n.process.priority)){
        current->n.times = 0;
        current->n.process.state = READY;
        
        current = current->n.next;
        current->n.process.state = RUNNING;
        return current->n.process.sp;
    }
    return current->n.process.sp;
}

uint8_t add(Process p) {
    if (address == 0) return 0;
    if(init == 0){
        init = 1;
    }

    Node * node = newNode();
    node->n.times = 0;
    node->n.process = p;           
    if (current == 0) {
        current = node;
        node->n.next = node;
    } else {
        Node * aux;
        aux = current->n.next;
        current->n.next = node;
        node->n.next = aux;
    }
    return 0;
}

void killCurrent() {
    kill(getPid());
}

uint64_t kill(uint64_t pid) {
    /* If its the only process */
    if (current == current->n.next) {
            Node * aux = current; 
            uint64_t pid = aux->n.process.pid;
            remove(aux->n.process);
            freeNode(aux);
            current = 0;
            init = 0;
            return pid;
    }
    Node * node = current;
    do {
        if (pid == node->n.next->n.process.pid) {
            Node * aux = node->n.next; 
            uint64_t pid = aux->n.process.pid;
            node->n.next = aux->n.next;
            if (current == aux) {
                current = aux->n.next;
                init = 1;
            }
            remove(aux->n.process);
            freeNode(aux);
            return pid;
        }
        node = node->n.next;
    } while (node != current); // si el siguiente no lo vi aun
    return 0;               
}

void setPriority(uint64_t pid, uint8_t n) {
    if (n > MAX_PRIO || n < 0 ) return;
    Node * node = search(pid);
    node->n.process.priority = n;
}

void setState(uint64_t pid, states state) {
    if (state == RUNNING) return;
    Node * node = search(pid);
    /* If not the one currently running */
    if (node->n.process.pid != current->n.process.pid) {
        node->n.process.state = state;
        return;
    }
    if (state == READY) return;
    node->n.process.state = state;  
}

uint64_t getPid() {
    return current->n.process.pid;
}

Node * search(uint64_t pid) {
    if (current == 0) return 0;
    Node * node = current;
    do {
        if (pid == node->n.process.pid)
            return node;
        node = node->n.next;
    } while (node != current);
    return 0;
}

void listAll() {
    Node * node = current;
    print("\nName\tPID\tSP\tBP\tPrio\tLevel\tState\n");
    if (current == 0) {
        print("NOP\t-1\tThere is no Process in the scheduler");
        return;
    }
    do {
        Process p = node->n.process;
        char* stat = p.state==0? "Ready" : (p.state==1? "Running":"Blocked");
        char* lvl = p.context==0? "Foreground" : "Background";
        print(p.name); print("\t"); printHex(p.pid); print("\t");
        printHex(p.sp); print("\t"); printHex(p.bp); print("\t"); printHex((uint64_t)p.priority);
        print("\t"); print(lvl); print("\t"); print(stat); print("\n");
        node = node->n.next;
    } while (node != current);
    print("\n");
}

void initScheduler() {
    init = 0;
    current = 0;
    address = (Node *)malloc(SIZE);
    cleanMem();
}

/* Memory manager for the nodes */
/* Returns direction of a new Node */
static Node * newNode() {
    for (uint64_t i = 0; i < SIZE / sizeof(Node); i++) {
        if ((address+i)->n.used == 0) {
            (address+i)->n.used = 1;
            return address+i;
        }
    }
    return 0;    
}

/* Frees the Node given */
static void freeNode(Node * node) {
    node->n.used = 0;
}

/* Sets all available places to free */
static void cleanMem() {
    for (uint64_t i = 0; i < SIZE / sizeof(Node); i++)
        (address+i)->n.used = 0;    
}

static uint64_t pow(int base, int exponent) {
	int result = 1;
	for (uint64_t i = 0; i < exponent; i++){
		result = result * base;
	}
	return result;
}
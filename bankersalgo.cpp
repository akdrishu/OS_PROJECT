/* So the objective is to detect deadlock using bankers's algorithm.
 Banker's algorithm states that resource allocation should be done only if the system is in safe state.
 If the system is in unsafe state, there may be chances that there is a deadlock in the system.
 Hence our objective will be to find out if the system is in a safe state or not.
 */
#include <bits/stdc++.h>
#include <unistd.h>  //for linux
//#include "Windows.h" //for windows
#include "thread"

using namespace std;
typedef long long int ll;
const ll mxn = 1e2;
ll no_Resources, no_Processes;
ll avail[mxn], allocated[mxn][mxn], no_completedprocess = 0;
ll maxRequired[mxn][mxn], need[mxn][mxn], safeSeq[mxn];

pthread_mutex_t lock_Resources;
pthread_cond_t all_condition;

bool isSafe();

void *Code_Processing(void *arg);

void get_input();

void calculate_needmatrix();

void solve();

void Process_Execution();

int main(int argc, char **argv) {

    get_input();
    calculate_needmatrix();
    solve();
    Process_Execution();
    return 0;

}


/*Funtion to get input values of processes and resources.*/

void get_input() {
    srand(time(NULL));

    printf("\nNumber of processes? ");
    scanf("%lld", &no_Processes);

    printf("\nNumber of resources? ");
    scanf("%lld", &no_Resources);


    printf("\nCurrently Available resources (R1 R2 ...Rn)? ");
    for (ll i = 0; i < no_Resources; i++)
        scanf("%lld", &avail[i]);


    printf("\n");
    for (ll i = 0; i < no_Processes; i++) {
        printf("\nResource allocated to process %lld (R1 R2 ...)? ", i + 1);
        for (ll j = 0; j < no_Resources; j++)
            scanf("%lld", &allocated[i][j]);
    }
    printf("\n");

    // maximum required resources
    for (ll i = 0; i < no_Processes; i++) {
        printf("\nMaximum resource required by process %lld (R1 R2 ...)? ", i + 1);
        for (ll j = 0; j < no_Resources; j++)
            scanf("%lld", &maxRequired[i][j]);
    }
    printf("\n");


}


/*Function to calculate need matrix
 * need[i][j]= maxRequired[i][j] - allocated[i][j]*/

void calculate_needmatrix() {
    // calculate need matrix

    for (ll i = 0; i < no_Processes; i++) {
        for (ll j = 0; j < no_Resources; j++)
            need[i][j] = maxRequired[i][j] - allocated[i][j];
    }
}

/*Funtion to find out if safe sequence exists or not and further
 * if safe sequence exists, execute all processes one by one.*/

void solve() {
    // get safe sequence

    for (ll i = 0; i < no_Processes; i++) safeSeq[i] = -1;

    if (!isSafe()) {
        printf("\nNo safe sequence detected, hence system may/may not be in deadlock state.\n\n");
        exit(-1);
    }

    printf("\n\nSafe Sequence Found : ");
    for (ll i = 0; i < no_Processes - 1; i++) {
        printf("P%lld-->", safeSeq[i] + 1);
    }
    printf("P%lld", safeSeq[no_Processes - 1] + 1);

}

/*
 Banker's algorithm to find if system is in a safe state or not:
 1) Let Work,Finish,Safe be vectors of length m,n,n respectively.
Work is used to determine current max need
 Initialize: Work = Available
Finish[i] = 0; for i:(1 to n)

2) Find an i such that both
a) Finish[i] = 0
b) Need[i] <= Work
c) push i into Safe
if no such i exists goto step -> 4

3) Work = Work + Allocation[i]
Finish[i] = 1
goto step (2)

4) if Finish [i] = 1 for all i
then the system is in a safe state

5) Print safe.
 */


bool isSafe() {
    // get safe sequence

    ll work[no_Resources], ind = 0;
    for (ll i = 0; i < no_Resources; i++) work[i] = avail[i];

    ll finish[no_Processes] = {0};
    int count = 0;
    while (count < no_Processes) {
        bool found = false;
        for (ll i = 0; i < no_Processes; i++) {
            if (finish[i] == 0) {
                ll j;
                for (j = 0; j < no_Resources; j++)
                    if (need[i][j] > work[j])
                        break;
                if (j == no_Resources) {
                    for (ll k = 0; k < no_Resources; k++)
                        work[k] += allocated[i][k];

                    safeSeq[count++] = i;

                    finish[i] = 1;

                    found = true;
                }
            }
        }

        if (!found) {
            for (ll i = 0; i < no_Processes; i++) safeSeq[i] = -1;
            return false;
        }
    }

    return true;

}

/*Creating multiple processes outside main and applying locks
 * so that shared data be safe from concurrent access*/

void *Code_Processing(void *arg) {
    ll _insafeSeq = *((ll *) arg);

    // lock resources
    pthread_mutex_lock(&lock_Resources);

    // condition check
    while (_insafeSeq != safeSeq[no_completedprocess])
        pthread_cond_wait(&all_condition, &lock_Resources);

    // process
    printf("\n--> Process %lld", _insafeSeq + 1);
    printf("\n\tAllocated : ");
    for (ll i = 0; i < no_Resources; i++)
        printf("%lld ", allocated[_insafeSeq][i]);

    printf("\n\tNeeded    : ");
    for (ll i = 0; i < no_Resources; i++)
        printf("%lld ", need[_insafeSeq][i]);

    printf("\n\tAvailable : ");
    for (ll i = 0; i < no_Resources; i++)
        printf("%lld ", avail[i]);

    printf("\n");
    sleep(1);

    printf("\tResource Allocated!");
    printf("\n");
    sleep(1);
    printf("\tProcess Code Running...");
    printf("\n");
    sleep(5); // process code
    printf("\tProcess Code Completed...");
    printf("\n");
    sleep(1);
    printf("\tProcess Releasing Resource...");
    printf("\n");
    sleep(1);
    printf("\tResource Released!");

    for (ll i = 0; i < no_Resources; i++)
        avail[i] += allocated[_insafeSeq][i];

    printf("\n\tNow Available : ");
    for (ll i = 0; i < no_Resources; i++)
        printf("%lld ", avail[i]);
    printf("\n\n");

    sleep(1);

    // condition broadcast
    no_completedprocess++;
    pthread_cond_broadcast(&all_condition);
    pthread_mutex_unlock(&lock_Resources);
    pthread_exit(NULL);
}


/*Funtion for independant process execution one by one. */

void Process_Execution() {

    printf("\nExecuting Processes...\n\n");
    sleep(1);

    // run threads
    pthread_t processes[no_Processes];
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    ll processNumber[no_Processes];
    for (ll i = 0; i < no_Processes; i++) processNumber[i] = i;

    for (ll i = 0; i < no_Processes; i++)
        pthread_create(&processes[i], &attr, Code_Processing, (void *) (&processNumber[i]));

    for (ll i = 0; i < no_Processes; i++)
        pthread_join(processes[i], NULL);

    printf("\nAll Processes Finished\n");

}

















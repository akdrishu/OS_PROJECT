/* So the objective is to avoid deadlock using bankers's algorithm.
 Banker's algorithm states that resource allocation should be done only if the system is in safe state.
 If the system is in unsafe state, there may be chances that there is a deadlock in the system.
 Hence our objective will be to find out if the system is in a safe state or not.
 */
#include "bits/stdc++.h"
#include "thread"
#include "Windows.h"
using namespace std;
typedef long long int ll;

std::mutex mtx;
ll n, m;

void process_run(ll safe[], ll alloc[n][m], ll need[n][m], ll stored_work[n][m]) {
    static int i = 0;
    mtx.lock();
    {
        cout << "Running Process " << safe[i] << endl;
        cout << "Allocated    ";
        for (ll j = 0; j < m; j++) cout << alloc[safe[i]][j] << " ";
        cout << "\nNeeded     ";
        for (ll j = 0; j < m; j++) cout << need[safe[i]][j] << " ";
        cout << "\nAvailable  ";
        for (ll j = 0; j < m; j++) cout << stored_work[i][j] << " ";
        cout << "\nResource Allocated!\n";
        Sleep(1);
        cout << "Process running!\n";
        Sleep(5);
        cout << "Process completed!\n";
        Sleep(1);
        cout << "Process releasing resource!\n";
        Sleep(2);
        cout << "Resource released!\n";
        cout << "Now available:   ";
        i++;
        for (ll j = 0; j < m; j++) cout << stored_work[i][j] << " ";
    }

    mtx.unlock();
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



int main() {
    ll k, flag = 0;
    cout << "Give No of processes";
    cin >> n;                             // Number of processes
    cout << "\nGive no of resources";
    cin >> m;                             // Number of resources
    ll alloc[n][m], max[n][m], avail[m];
    cout << "\nProvide the allocation matrix";
    for (ll i = 0; i < n; i++) {
        for (ll j = 0; j < m; j++)
            cin >> alloc[i][j];
    }
    cout << "\nProvide the max matrix";
    for (ll i = 0; i < n; i++) {
        for (ll j = 0; j < m; j++)
            cin >> max[i][j];
    }
    cout << "\nProvide the avail list";
    for (ll i = 0; i < m; i++) cin >> avail[i];
    ll work[m], store_work[n + 1][m];
    for (ll i = 0; i < m; i++) {
        work[i] = avail[i];
        store_work[0][i] = work[i];
    }

    ll finish[n] = {0};
    ll safe[n], ind = 0;

    ll need[n][m];
    for (ll i = 0; i < n; i++) {
        for (ll j = 0; j < m; j++)
            need[i][j] = max[i][j] - alloc[i][j];
    }
    ll y = 0;
    for (k = 0; k < n; k++) {
        for (ll i = 0; i < n; i++) {
            if (finish[i] == 0) {


                for (ll j = 0; j < m; j++) {
                    if (need[i][j] > work[j]) {
                        flag = 1;
                        break;
                    }
                }

                if (flag == 0) {
                    {
                        safe[ind++] = i;

                    }
                    for (y = 0; y < m; y++) {
                        work[y] += alloc[i][y];
                        store_work[i + 1][y] = work[y];
                    }
                    finish[i] = 1;
                }
            }
        }
    }
    if (flag == 1) cout << "No safe sequence detected, hence system may/may not be in deadlock state.\n";
    else {
        cout << "\nFollowing is the SAFE Sequence" << endl;
        for (ll i = 0; i < n - 1; i++)
            cout << " P" << safe[i] << " ->";
        cout << " P" << safe[n - 1] << endl;
        cout << "Executing processes:\n";
        Sleep(1);
        std::thread threads[n](process_run, safe, alloc, need, store_work);
        for (ll i = 0; i < n; i++)
            thread::join(threads[i]);
    }
}

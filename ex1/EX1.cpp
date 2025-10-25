#include <iostream>
#include <vector>
using namespace std;

vector<int> init_state(int size){
    vector<int> state(size, 0);
    state[size / 2] = 1;
    return state;
}

void print_state(const vector<int>& state){
    for(int bit : state)
    cout <<(bit?"||":" ");
    cout << endl;
}

vector<int> evolve(const vector<int>& current, int rule){
    int n = current.size();
    vector<int> next (n,0);
    for(int i=0; i < n ; i++ ){
        int left =(i==0)? 0 : current[i-1];
        int center = current[i];
        int right = (i== n-1)? 0 : current[i+1];

        int index = (left << 2 )| (center << 1) | right;
        next[i] = (rule >> index) & 1;

    }

    return next;

}

int main() {
    int size = 31;   
    int steps = 15; 
    int rule = 30;   

    vector<int> state = init_state(size);

    cout << "Automate Cellulaire 1D - Rule " << rule << endl;
    print_state(state);

    for (int i = 0; i < steps; i++) {
        state = evolve(state, rule);
        print_state(state);
    }

    return 0;
}

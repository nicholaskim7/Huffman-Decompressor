#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <pthread.h>
#include <thread>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <algorithm>
#include <cmath>

#include "h3.h"
using namespace std;

//thread struct
template <class H>
struct pthread{
    string line;
    Node<H> *root;
    vector<pair<int, char>> info; //indexs and the char
    pthread_mutex_t *mutex;
};

//get binary code and its indexs from string from compressed file
vector<string> getCom(string line){
    vector<string> stuff;
    string s;
    stringstream ss(line);
    while(getline(ss, s, ' ')){
        stuff.push_back(s);
    }
    return stuff;
}

//thread funct
template <class H> 
void *decode(void *void_ptr){
    pthread<H> *data = (pthread<H>*)void_ptr;
    vector<string>comm = getCom(data->line);
    
    char symbol = getSymb(data->root, comm[0], 0);
    int freq = getFreq(data->root, comm[0], 0);
    
    cout<<"Symbol: "<< symbol <<", Frequency: "<< freq <<", Code: "<< comm[0]<<endl;
    
    pthread_mutex_unlock(data->mutex);
    
    for(int i = 1; i < comm.size(); i++){
        int n = stoi(comm[i]);
        data->info.push_back(make_pair(n, symbol));
    }
    return nullptr;
}

template <class H>
int getFreq(Node<H> *node, string code, int index){
    if(code.empty()){
        return 0;
    }

    if(index == code.length()){
        return node->freq;
    }

    if(code[index] == '0'){ //go left
        return getFreq(node->left, code, index+1);
    }

    else if(code[index] == '1'){  //go right
        return getFreq(node->right, code, index+1);
    }

    return 0;
}

//finding the symbol from huffman code
template <class H>
char getSymb(Node<H> *node, string code, int index){
    if(code.empty()){
        return '\0';
    }

    if(index == code.length()){
        return node->C;
    }

    if(code[index] == '0'){ //go left
        return getSymb(node->left, code, index+1);
    }

    else if(code[index] == '1'){  //go right
        return getSymb(node->right, code, index+1);
    }

    return '\0';
}

int main(int argc, char** argv){
    string in;
    string comm;
    char alpha;
    int freq;
    int nthreads=0;
    int finalSize=0;

    //initialize mutex
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, nullptr);

    //get num of symbols (first line)
    int num;
    cin >> num;
    cin.ignore();

    //get info for huffman tree
    prio_queue<char> pq;
    for(int i = 0; i < num; i++){
        getline(cin, in);
        alpha = in[0];
        freq = stoi(in.substr(2, in.size() - 1));
        pq.enqueue(alpha, freq);
        //final length of decoded mssg
        finalSize += freq;
    }

    //creating tree
    pq.helper();

    // thread vectors
    vector<pthread_t> threads;
    vector<pthread<char>*> threadZ;

    int turn = 0;

    //getting info from comm file (multiple threads)
    for(int i = 0; i < num; i++){
        getline(cin, comm);
        nthreads+=1;

        pthread<char> *thread_data = new pthread<char>();
        thread_data->root = pq.getRoot();
        //thread_data->turn = turn;
        thread_data->line = comm;
        
        
        pthread_t thread;

        pthread_mutex_lock(&mutex);
        
        pthread_create(&thread, NULL, &decode<char>, (void*)thread_data);
        threads.push_back(thread);
        threadZ.push_back(thread_data);
    }

    //join threads
    for(int i = 0; i < threads.size(); i++){
        pthread_join(threads[i], nullptr);
    }


    //pushing back symbols into pair
    vector< pair<int, char> > array;
    for (int i = 0; i < threadZ.size(); i++) {
        for (int j = 0; j < threadZ[i]->info.size(); j++) {
            array.push_back(make_pair(threadZ[i]->info[j].first,threadZ[i]->info[j].second));
        }
    }

    sort(array.begin(), array.end());

    //cout the decoded string
    cout << "Original message: ";
    for(int i = 0; i < finalSize; i++){
        cout << array[i].second;
    }
    return 0;


}

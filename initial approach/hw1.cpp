#include <iostream>
#include <queue>
#include <fstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <pthread.h>
#include <unistd.h>
#include <sstream>
#include <string>
using namespace std;


//struct with the arguments you need for the void* thread function
struct args
{
    vector<string> com;
    map<string,char> m;
    char* finalstr;
};


struct Node {
    char c;
    int freq;
    Node *left, *right;

    Node(char c, int freq) {
        this->c = c;
        this->freq = freq;
        left = right = NULL;
    }
};


struct compare {
    bool operator()(Node* l, Node* r) {
        if((l->freq == r->freq)){  
            if(l->c == r->c){   
                return l < r;  //tie breaker
            }
            return(l->c > r->c);  //compare ascii
        }
        return (l->freq > r->freq);  //compare freq
    } 
};


Node* HuffmanCodes(vector<char> data, vector<int> freq, int size) {
    Node *left, *right, *top;

    priority_queue<Node*, vector<Node*>, compare> minHeap;

    for (int i = 0; i < size; i++)
        minHeap.push(new Node(data[i], freq[i]));

    while (minHeap.size() != 1) {
        left = minHeap.top();
        minHeap.pop();
        right = minHeap.top();
        minHeap.pop();
        top = new Node(31, left->freq + right->freq);
        top->left = left;
        top->right = right;
        minHeap.push(top);
    }

    Node* root = minHeap.top();
    return root;
}


void printCodes(Node* root, string str, map<string, char> &m) {
    if (root == nullptr){
      return;
    }
    
    else if (root->c != 31){
    cout << "Symbol: " << root->c << ", Frequency: " << root->freq << ", Code: " << str << endl;
    }
    
    printCodes(root->left, str + "0", m);
    printCodes(root->right, str + "1", m);
}

map<string, char> getMap(Node* root, string str, map<string, char> &m){
    //create and return map containing code and symbol of that code
    if(root->c != 31){
        m[str] = root->c;  //create map
    }
    else{
        getMap(root->left, str + "0", m);
        getMap(root->right, str + "1", m);
    }
    return m;
}



void* decode(void *void_ptr){
    struct args *ptr = (struct args *)void_ptr;
    char currentchar = ptr->m[ptr->com[0]];
    
    for(int i = 1; i < ptr->com.size(); i++) {
        *(ptr->finalstr + stoi(ptr->com[i])) = currentchar;
    }
    return NULL;
}


int main() {
    // string input = "";  //get file names
    // string com = "";
    // cin >> input;
    // cin >> com;
    
    ifstream inputFile("input2.txt");  //opening files
    ifstream comFile("com2.txt");
    
    vector<char> alphas;
    vector<int> freq;
  
    string thing;
    char alpha;
    int num, count = 0;
    
  
    while(getline(inputFile, thing)){   //string manipulation of input file
        alpha = thing[0];
        num = stoi(thing.substr(1,2));
        alphas.push_back(alpha);
        freq.push_back(num);
    }

    
    int size = alphas.size();  //num of symbols to decode/number of posix threads
    Node* root = HuffmanCodes(alphas, freq, size);
    map<string, char> m;
    printCodes(root, "", m);
    string encoded = "";
    string c;
    
    
    for(int i = 0; i < freq.size(); i++){  //length of final string
        count+= freq[i];
    }
    
    char finalstr[count+1];
    
    for(int i = 0; i < count; i++){  //creation of lengthed string
         finalstr[i] = '0';
    }
    finalstr[count] = '\0';
    

    //assume we have struct that contains map and final string
    args* a = new args[size];
    static vector<pthread_t> threads;  //vector for the threads
    for(int iterator = 0; iterator < size; iterator++){  //for loop/numthreads
        getline(comFile, encoded);
        string code;
        string index;
        vector<string> com;
        code = encoded.substr(0,encoded.find(' '));
        a[iterator].com.push_back(code);  //push back code
        index = encoded.substr(encoded.find(' ')+1);
        stringstream ss(index);
        string inte;
        while(ss >> inte){
            a[iterator].com.push_back(inte);
        }
        
    
    
    a[iterator].m = getMap(root, c, m); //getmap
    
    a[iterator].finalstr = finalstr; 
    


    //static vector<pthread_t> threads;  //vector for the threads
    pthread_t thread;
    
    pthread_create(&thread, NULL, decode, (void*) &a[iterator]);
    
    threads.push_back(thread);
    
    //pthread_join(threads[iterator], NULL);
    
    
    } //end of for loop looping for number of threads (size)
    for(int iterator = 0; iterator < size; iterator++){
        pthread_join(threads[iterator], NULL);
    }



    cout << "Original message: " << finalstr << endl;
   
    return 0;
}
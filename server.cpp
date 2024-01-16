#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#include <string.h>
#include <map>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <queue>
using namespace std;

//used code from rincon

// struct args
// {
//     vector<string> com;
//     map<string,char> m;
//     char* finalstr;
// };


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
        if((l->freq == r->freq)){  //compare ascii
            if(l->c == r->c){
                return l < r;
            }
            return(l->c > r->c);  //tie breaker
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


// void printCodes(Node* root, string str, map<string, char> &m) {
//     if (root == nullptr){
//       return;
//     }
    
//     else if (root->c != 31){
//     cout << "Symbol: " << root->c << ", Frequency: " << root->freq << ", Code: " << str << endl;
//     }
    
//     printCodes(root->left, str + "0", m);
//     printCodes(root->right, str + "1", m);
// }

map<string, char> getMap(Node* root, string str, map<string, char> &m){
    //create and return map containing code and symbol of that code
    if(root->c != 31){
        cout << "Symbol: " << root->c << ", Frequency: " << root->freq << ", Code: " << str << endl;
        m[str] = root->c;  //create map
    }
    else{
        getMap(root->left, str + "0", m);
        getMap(root->right, str + "1", m);
    }
    return m;
}


// void* decode(void *void_ptr){
//     struct args *ptr = (struct args *)void_ptr;
//     char currentchar = ptr->m[ptr->com[0]];
    
//     for(int i = 1; i < ptr->com.size(); i++) {
//         *(ptr->finalstr + stoi(ptr->com[i])) = currentchar;
//     }
//     return NULL;
// }



void fireman(int){
    while (waitpid(-1, NULL, WNOHANG) > 0)
        continue;
}



void error(char *msg){
    perror(msg);
    exit(1);
}


int main(int argc, char *argv[]){
    vector<char> alphas;
    vector<int> freq;
    map<string, char> m;
    string thing, c;
    char alpha;
    int num, count = 0;

    while(getline(cin, thing)){   //string manipulation of input file
        alpha = thing[0];
        num = stoi(thing.substr(1,2));
        alphas.push_back(alpha);
        freq.push_back(num);
    }
    int asize = alphas.size();  //num of symbols to decode/number of posix threads
    //cout << "asize: "<< asize << endl;
    Node* root = HuffmanCodes(alphas, freq, asize);
    //printCodes(root, "", m);
    m = getMap(root, c, m);

    
    int sockfd, newsockfd, portno, clilen;

    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc < 2){
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);



    signal(SIGCHLD, fireman);
    while (true){
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
        if (fork() == 0){
            if (newsockfd < 0)
                error("new socket wont open");
            bzero(buffer, 256);
            n = read(newsockfd, buffer, 255);
            if (n < 0)
                error("ERROR reading from socket");
            string temp=buffer;
            bzero(buffer, 256);
            buffer[0] = m[temp];
            n = write(newsockfd, buffer, 255);
            if (n < 0)
                error("ERROR writing to socket");
            close(newsockfd);
            _exit(0);
        }
    }
    close(sockfd);
    return 0;
}
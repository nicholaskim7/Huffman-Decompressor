#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <bits/stdc++.h>
using namespace std;

//derived from Rincons code from class

struct threadParams{
    string subMsg;
    int numToDecode;
    int finalstrsize;
    char symbol;
    struct sockaddr_in server_addr;
    struct hostent *serv;
    int portnumber;
    //vector<string> ind;
    //string decodedmsg;
};

void *sendServerThreads(void *params){
    threadParams *args = (threadParams *)params;

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char *buffer = new char[args->numToDecode + 2]; //sending a string binary code and receiving a char

    portno = args->portnumber;
    server = args->serv;
    serv_addr=args->server_addr;

    // make a new socket per thread
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        std::cout<<"ERROR opening socket\n";
        exit(0);
    }

    //connecting to server
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
        std::cout<<"Error connecting to server\n";
        exit(0);
    }

    bzero(buffer, args->numToDecode + 2);
    args->subMsg.copy(buffer, sizeof(args->subMsg.length()));
    n = write(sockfd, buffer, args->numToDecode + 1);
    if (n < 0){
        std::cout<<"Error writing the socket\n";
        exit(0);
    }
    bzero(buffer, args->numToDecode + 2);
    n = read(sockfd, buffer, args->numToDecode + 1);
    if (n < 0){
        std::cout<<"ERROR reading from socket\n";
    }

    args->symbol = buffer[0];
    close(sockfd);
    return NULL;
}

int main(int argc, char *argv[]){
    int sockfd, portno, n, numToDecode, finalstrsize, numthreads, temp;
    std::string encoded;
    string decodedmsg;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3){
        std::cout<<"too little arguments for client.cpp\n";
        return -1;
    }
    
    //create socket
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        std::cout<<"ERROR opening socket\n";
        return -1;
    }

    //get server
    server = gethostbyname(argv[1]);
    if (server == NULL){
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }


    

    int count = 0;
    vector<string> ind;
    vector<string> cod;
    while(getline(cin, encoded)){ //recieve line from compressed file
        string code, index;
        code = encoded.substr(0,encoded.find(' '));
        index = encoded.substr(encoded.find(' ')+1);
        cod.push_back(code);
        ind.push_back(index);
    }

    
    
    for(int j = 0; j < ind.size(); j++){
        stringstream ss(ind[j]);
        string inte;
        while(ss >> inte){
            count++;
        }
    }

    decodedmsg.resize(count);


    //creating threads
    numthreads = ind.size();
    //cout << "numthreads: " << numthreads << endl;
    pthread_t *threads = new pthread_t[numthreads];
    //sending vars needed in all threads
    threadParams *var = new threadParams[numthreads];
    

    //cout << cod.size()<< endl;
    for (int i = 0; i < cod.size(); i++){  //number of symbols to decode / lines in compressed.txt
            //cout << "entering for loop " << i << endl;
            var[i].numToDecode = cod.size();
            //cout << "numtodecode "<< i << endl;
            var[i].subMsg = cod[i];
            //cout << "submsg " << i << endl;
            var[i].portnumber = portno;
            //cout << "port " << i << endl;
            var[i].serv = server;
            //cout << "server " << i << endl;
            var[i].server_addr=serv_addr;
            //cout << "adress " << i << endl;
            //exiting for loop
            //var[i].ind = ind;
            //var[i].finalstrsize = count;
            //var[i].decodedmsg = decodedmsg;

    }

    for (int i = 0; i < numthreads; i++)
        pthread_create(&threads[i], NULL, sendServerThreads, &var[i]);
    
    for (int i = 0; i < numthreads; i++)
        pthread_join(threads[i], NULL);
    
    delete[] threads;


    
    for(int j = 0; j < ind.size(); j++){
        //cout << "ind: " << ind[j] << endl;
        stringstream ss(ind[j]);
        string inte;
        while(ss >> inte){
            decodedmsg[stoi(inte)] = var[j].symbol;
        }
    }      
    
    delete[] var;
    cout << "Original message: " << decodedmsg << "\n";

    
    return 0;
}

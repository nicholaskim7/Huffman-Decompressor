#ifndef H3_H
#define H3_H

#include <iostream>
#include <sstream>
#include "math.h"
#include <cstdlib>
using namespace std;

//priority queue
template <class H>
struct Node {
    H C;
    int freq;

    Node<H> *left;
    Node<H> *right;
    Node<H> *next;
    
};

template <class H>
class prio_queue {
public:
    Node<H> *head;
    Node<H> *tail;
    // string Chars;
    // string Binarys;

    //huffmanTree initialization
    prio_queue(){
        head = nullptr;
        tail = nullptr;
        // Binarys = "";
        // Chars = "";

    }

    //first check the huffman tree if it is empty
    bool isEmpty(){
        return head == nullptr;

    }


    //enqueue
    void enqueue(H _C, int _freq) {
        Node<H> *temp = new Node<H>();
        temp->C = _C;
        temp->freq = _freq;

        temp->next = nullptr;

        if (isEmpty()) {
            head = temp;
            tail = temp;
        }

        else {
            Node<H> *cu = head;
            Node<H> *prev = nullptr;

            if(temp->freq < cu->freq) {// compare freq
                temp->next = head;
                head = temp;
            }
            else if(temp->freq == cu->freq){
                if(temp->C < cu->C){// compare ascii
                    temp->next = head;
                    head = temp;
                }
                else if(temp->C > cu->C){//tie breaker
                    while(cu != nullptr && temp->C > cu->C && temp->freq == cu->freq){
                        prev = cu;
                        cu = cu->next;
                    }
                    prev->next = temp;
                    temp->next = cu;
                    if(temp->next == nullptr){ tail = temp; }
                }
            }
            else if(temp->freq > cu->freq){
                while(cu != nullptr && temp->freq > cu->freq){
                    prev = cu;
                    cu = cu->next;
                }
                if(cu == nullptr){
                    prev->next = temp;
                    temp->next = cu;
                }
                else if(temp->C < cu->C){
                    prev->next = temp;
                    temp->next = cu;
                }
                else if(temp->C > cu->C){
                    while(cu != nullptr && temp->C > cu->C && temp->freq == cu->freq){
                        prev = cu;
                        cu = cu->next;
                    }
                    prev->next = temp;
                    temp->next = cu;
                    if(temp->next == nullptr){ tail = temp; }
                }
            }
        }
    }


    //enque node function
    void enqueueNode(Node<H> *temp){
        temp->next = nullptr;

        if (isEmpty()) {
            head = temp;
            tail = temp;
        }

        else {
            Node<H> *cu = head;
            Node<H> *prev = nullptr;
            if (temp->freq < cu->freq || (temp->freq == cu->freq  && temp->C < cu->C)) {
                temp->next = head;
                head = temp;
            }
            else {
                    while (cu != nullptr && temp->freq > cu->freq ) {// took out = of temp>freq
                        prev = cu;
                        cu = cu->next;
                    }
                    prev->next = temp;
                    temp->next = cu;
                    if (temp->next == nullptr) {tail = temp;}
            }
        }
    } 


    //return root of huffmantree
    Node<H>* getRoot() {
        if (!isEmpty())
            return head;
        return nullptr;
    }

    //counting queue size
    int size(){
        Node<H> *temp = head;
        int count =0;
        while (temp != nullptr){
            count++;
            temp = temp->next;
        }
        return count;
    }


    //dequeue
    Node<H>* dequeue(){
        if (!isEmpty()) {
            Node<H> *temp = head;
            head = head->next;
            return temp;
        }
    }


    //helper funct combines first 2 nodes and re enqueue
    void helper(){
        while(size() > 1){
            Node<char> *node1 = getRoot();
            dequeue();
            Node<char> *node2 = getRoot();
            dequeue();

            Node<H> *newNode = new Node<H>();
            newNode->freq = node1->freq + node2->freq;
            newNode->C = 31;
            newNode->left = node1;
            newNode->right = node2;

            enqueueNode(newNode);
        }

    } 


    //printing out huffman codes
    void printCodes(Node<H> *root, string str){
        if(!root){
            return;
        }

        if(root->C!=31){
            cout<<"Symbol: "<<root->C<<", Frequency: "<<root->freq<<", Code: "<<str<<endl;
            // Binarys+=str;
            // Chars+=root->C;
        }

        printCodes(root->left,str+"0");
        printCodes(root->right,str+"1");
    }
};

#endif
// Header file containing all structures used
// @Authors: jcbaker, sccoache
// CS3013 Project 3b

#ifndef SOME_GUARD
#define SOME_GUARD

//struct definitions
struct broadcasts{
    int id;
    int message;
    int recast;
    int channel;
};
typedef struct broadcasts brdcst;

struct position{
    int x;
    int y;
};
typedef struct position coord;

struct noisemakers{ 
    int channel;
    int talkTime;
    int randTime;
    double talkProb;
    coord pos;
    brdcst broadcast;
    pthread_t noiseThread;
};
typedef struct noisemakers noisemaker;

struct node{
    int id;
    int transmitTime;
    int channel; 

    unsigned int dwellDur; 
    unsigned int brdcstTime; 

    double dwellProb;
    double talkProb; 
    
    coord pos; //coordinates
    brdcst nodeMessage; //message to be broadcasted
    brdcst totalMessages[1000];
    
    pthread_t threadLord; 
    unsigned int msgcnt;
    char* logLord; //textfile name
};
typedef struct node nodeLord;

#endif
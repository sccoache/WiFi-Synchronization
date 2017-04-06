// Mutex.c
// @Authors: jcbaker, sccoache
// CS3013 Project 3b
// WiFi Synchronization Problem

#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "header.h"

#define NODE_COUNT (20)
#define NOISE_COUNT (8)

//mutex and condition variable definition
typedef pthread_mutex_t mutex;

//prototypes
void noiseCast(noisemaker* maker);
int waiting(nodeLord *node);
void *threadDecisions(void *args);
void *noisemakerDecisions(void *args);
void changeChannel(nodeLord *node);

int scanArea(int x, int y, brdcst **chan);
int tag(brdcst broadcast, int x, int y, brdcst *chan[100]);
int untag(int x, int y, brdcst *chan[100]);
int scanAreaWait(int x, int y, brdcst **chan, brdcst *data);
char *idConversion(int id);
void broadcast(nodeLord *node);

brdcst createCast(int integer, int channel);
coord createPos(int x, int y);

brdcst **tvRemote(int channel);
mutex *parentalLock(int channel);

//global variables
mutex *mutex_lock;
mutex *channel_lock1;
mutex *channel_lock6;
mutex *channel_lock11;
nodeLord *node_list;
noisemaker* noisemaker_list;
brdcst** channel1;
brdcst** channel6;
brdcst** channel11;
int nodePositions[100][100];

int main(int argc, char **argv) {
    channel1 = (brdcst**)malloc(sizeof(brdcst) * 100);
    channel6 = (brdcst**)malloc(sizeof(brdcst) * 100);
    channel11 = (brdcst**)malloc(sizeof(brdcst) * 100);

    for(int i =0; i< 100; i++){
	    channel1[i] = (brdcst*)malloc(100 * sizeof(brdcst));
        channel6[i] = (brdcst*)malloc(100 * sizeof(brdcst));
        channel11[i] = (brdcst*)malloc(100 * sizeof(brdcst));
    }

    brdcst* tempCast = (brdcst*)malloc(sizeof(brdcst));
    tempCast->id = 0;
    tempCast->message = 0;
    tempCast->channel =0;
    tempCast->recast = 0;

    for(int i = 0; i< 100; i++){
        for(int j = 0; j < 100; j++){
	       channel1[i][j].id = tempCast->id;
	       channel6[i][j].id = tempCast->id;
	       channel11[i][j].id = tempCast->id;

	       channel1[i][j].message = tempCast->message;
	       channel6[i][j].message = tempCast->message;
	       channel11[i][j].message = tempCast->id;
        }
    }

    // seed random number generation
    srand((unsigned) time(NULL));
    // create mutex lock and malloc to the default size
    mutex_lock = (mutex *) malloc(sizeof(mutex)); 

    // locks for each channel
    channel_lock1 = (mutex *) malloc(sizeof(mutex)); // channel 1
    channel_lock6 = (mutex *) malloc(sizeof(mutex)); // channel 6
    channel_lock11 = (mutex *) malloc(sizeof(mutex)); // channel 11

    // initalizing mutex locks for each channel
    pthread_mutex_init(mutex_lock, 0); // mutex lock gets initalized
    pthread_mutex_init(channel_lock1, 0); // channel 1
    pthread_mutex_init(channel_lock6, 0); // channel 6
    pthread_mutex_init(channel_lock11, 0); // channel 11

    node_list = (nodeLord *) malloc(sizeof(nodeLord) * NODE_COUNT);

    

    // loop to create nodes **NOTE to change number of nodes, change defined variable 'NODE_COUNT' value above
    for (int node_count = 0; node_count < NODE_COUNT; node_count++) {
        node_list[node_count].id = node_count + 1;
        node_list[node_count].dwellDur = (unsigned) rand() % 10 + 2;
        node_list[node_count].dwellProb = rand() % 10 / 10 + 0.1;
        node_list[node_count].transmitTime = 10;
        node_list[node_count].talkProb = (unsigned) rand() % 10 / 10 + 0.1;
        node_list[node_count].brdcstTime = 1;
        node_list[node_count].pos = createPos(rand() % 100, rand() % 100);
        node_list[node_count].channel = 1;
        node_list[node_count].nodeMessage = createCast(node_count, node_list[node_count].channel);
        node_list[node_count].logLord = idConversion(node_list[node_count].id);
        node_list[node_count].msgcnt = 0;
    }

    // loop to create a thread for each node
    for (int node_count = 0; node_count < NODE_COUNT; node_count++) {
        int makeThread = pthread_create(&node_list[node_count].threadLord, NULL, threadDecisions, (void *) &node_list[node_count]);

        if (makeThread) {
            printf("Failed to create a pthread, error number: %d\n", makeThread);
        }
    }
  

  
    noisemaker_list = (noisemaker *) malloc(sizeof(noisemaker) * NOISE_COUNT);


    for(int noisemaker_count = 0; noisemaker_count < NOISE_COUNT; noisemaker_count++){
        noisemaker_list[noisemaker_count].channel = 1;
        noisemaker_list[noisemaker_count].pos = createPos(rand() % 100, rand() % 100);
        noisemaker_list[noisemaker_count].talkProb = (unsigned) rand() % 10 / 10 + 0.1;
        noisemaker_list[noisemaker_count].talkTime = rand() % 7;
        noisemaker_list[noisemaker_count].randTime = rand() % 5;
        noisemaker_list[noisemaker_count].broadcast = createCast(noisemaker_count, noisemaker_list[noisemaker_count].channel);
   
    }

    // loop to create a thread for each noisemaker
    for (int noisemaker_count = 0; noisemaker_count < NOISE_COUNT; noisemaker_count++) {
        int makeThread = pthread_create(&noisemaker_list[noisemaker_count].noiseThread, NULL, noisemakerDecisions, (void *) &noisemaker_list[noisemaker_count]);

        if (makeThread) {
            printf("Failed to create a pthread, error number: %d\n", makeThread);
        }
    }

    // make threads for each node and then join
    for (int node_count = 0; node_count < NODE_COUNT; node_count++) {

        // use pthread_join to join all the threads @ main and then set them free
        pthread_join(node_list[node_count].threadLord, NULL); 
    }

    // make threads for each noisemaker and then join
    for (int noisemaker_count = 0; noisemaker_count < NOISE_COUNT; noisemaker_count++) {

        // use pthread_join to join all the threads @ main and then set them free
        pthread_join(noisemaker_list[noisemaker_count].noiseThread, NULL);
    }
  
    return 0;
}

// make broadcast. take in id and channel as fields, fill the rest
brdcst createCast(int integer, int channel) {
    brdcst example_brdcst;
    example_brdcst.id = integer;
    example_brdcst.message = rand() % 9999;
    example_brdcst.recast = 0;
    example_brdcst.channel = channel;
    return example_brdcst;
}

// make x,y coord. take two integers as fields
coord createPos(int x, int y) {
    if(nodePositions[x][y] != 0){
        createPos(rand() % 100, rand() % 100);
    }
  
  	nodePositions[x][y] = 1;
    coord example_brdcst;
    example_brdcst.x = x;
    example_brdcst.y = y;
    return example_brdcst;
}

/**
 * idConversion takes the 4-byte integer id and converts it to a string
 * @param id = the 4-byte id that will be converted to a string
 * @return = return the converted string
 */
char *idConversion(int id) {
    char *idConversion = malloc(sizeof(char) * 25);
    sprintf(idConversion, "%d", id);
    strcat(idConversion, "Mutex_Node_Output.txt");
    return idConversion;
}

/**
 * threadDecisions handles the potential actions a thread can peform. It handles deciding when a thread will waiting, broadcast, or change channels.
 * @return = void
 */
void *threadDecisions(void *args) {
    nodeLord *example_node = (nodeLord *) args;
    while (1) {

        // Find random time to sleep based on the globally defined const variables above
        int sleep_time = (1 + (rand() % (10 - 1)));
        sleep((unsigned int) sleep_time); // Make the thread sleep for that many seconds now

        // creates a random double with a value from 0.0 - 1.0
        double randNum = (rand() % 10) / 10; 

        // ask if the node wants to broadcast
        if (example_node->talkProb >= randNum && example_node->brdcstTime != 0){

            // check if the node wants to broadcast and the window time doesn't equal 0
            broadcast(example_node);
        }

        // check if the node wants to wait
        waiting(example_node);

        // check if the node wants to change channels
        if (example_node->dwellProb >= randNum && example_node->dwellDur == 0) {

            changeChannel(example_node);
        }

        example_node->brdcstTime = example_node->brdcstTime - 1;
        example_node->dwellDur = example_node->dwellDur - 1;
        sleep(1);
    }
}


// change the channel of the node based on a random number and the dwell duration
void changeChannel(nodeLord *node) {
    int randNum = rand() % 10;
    node->dwellDur = (unsigned) 2 + rand() % 10;
    if (randNum < 4) {
        node->channel = 1;
    } else if (randNum < 7) {
        node->channel = 6;
    } else {
        node->channel = 11;
    }
}

// the main broadcast functions
void broadcast(nodeLord *node){
    //trylock on channel lock
    if (pthread_mutex_trylock(parentalLock(node->channel)) == 0) {

        // if we hit a lock then release from the lock after the broadcast
        int rebroadcastProb = (rand() %100);

        // if the rebroadcastProb is greater than 100 and the message hasnt been seen before, rebroadcast
	if (rebroadcastProb >= 100 && node->msgcnt > 0){

	    int multipleCast = rand() % node->msgcnt; // choose to rebroadcast again based on the amount of messages seen

	    if (node->msgcnt == 1 && multipleCast == 1){
	       multipleCast = 0;
	} 
	    node->nodeMessage = node->totalMessages[multipleCast];
	    node->nodeMessage.recast = 1; // tag if the message is repeated
	    node->nodeMessage.channel = node->channel;
	}
	else{
	    node->nodeMessage = createCast(node->id, node->channel);
        }
        int isBroadcasting = 0;

        //broadcast if there is no data
        if ((scanArea(node->pos.x, node->pos.y, tvRemote(node->channel))) == 1) {
            isBroadcasting = tag(node->nodeMessage, node->pos.x, node->pos.y, tvRemote(node->channel));
	    pthread_mutex_unlock(parentalLock(node->channel));
        } 
        else{
              ; // the void space
             } 

        if (isBroadcasting) {
            sleep((unsigned) node->transmitTime);
	    pthread_mutex_lock(parentalLock(node->channel));
            untag(node->pos.x, node->pos.y, tvRemote(node->channel));
            
        }
    // release the lock with the pthread_mutex_unlock function
	pthread_mutex_unlock(parentalLock(node->channel));
    }
    return;
}

// function to get channels
brdcst **tvRemote(int channel) {
    if (channel == 1) {
        return channel1;
    } 
    else if (channel == 6) {
        return (brdcst **)channel6;
    } 
    else {
        return (brdcst **)channel11;
    }
}

// gets the channel locks
mutex *parentalLock(int channel) {
    if (channel == 1) {
        return channel_lock1;
    } 
    else if (channel == 6) {
        return channel_lock6;
    } 
    else {
        return channel_lock11;
    }
}

//tag takes the coordinate positions of a node and fills a 10 by 10 square
int tag(brdcst broadcast, int x, int y, brdcst *chan[100]) {
    int x1, x2;
    int y1, y2;

    if(x >= 5){
        x1 = x -5;
    }else{
        x1 = 0;
    }

    if(y >= 5){
        y1 = y-5;
    }else{
        y1 = 0;
    }

    if(x +5 >= 100){
        x2 = 100 -1;
    }else{
        x2 = x +5;
    }

    if(y +5 >= 100){
        y2 = 100 -1;
    }else{
        y2 = y +5;
    }
	
    for (int i = y1; i < y2; i++) {
        for (int j = x1; j < x2; j++) {
            chan[i][j] = broadcast;
        }
    }
    return 1;
}


//untag takes the coordinate coord of a node and unfills a 10x10 square
int untag(int x, int y, brdcst *chan[100]) {
    int x1, x2;
    int y1, y2;

    if(x >= 5){
        x1 = x -5;
    }else{
        x1 = 0;
    }
    
    if(y >= 5){
        y1 = y-5;
    }else{
        y1 = 0;
    }
    
    if(x +5 >= 100){
	   x2 = 100 -1;
    }
    else{
       x2 = x +5;
    }
    
    if(y +5 >= 100){
	   y2 = 100 -1;
    }
    else{
       y2 = y +5;
    }

    for (int i = y1; i < y2; i++) {
        for (int j = x1; j < x2; j++) {
            chan[i][j].id = 0;
            chan[i][j].message = 0;
        }
    }
    return 1;
}

//scanArea looks at the zone that a node would broadcast in and sees if there are conflicted nodes
int scanArea(int x, int y, brdcst **chan) {
    int isClear = 1;
    int x1, x2;
    int y1, y2;

    if(x >= 5){
        x1 = x -5;
    }else{
        x1 = 0;
    }
    
    if(y >= 5){
        y1 = y-5;
    }else{
        y1 = 0;
    }
    
    if(x +5 >= 100){
	   x2 = 100 -1;
    }
    else{
        x2 = x +5;
    }
    
    if(y +5 >= 100){
	   y2 = 100 -1;
    }
    else{
       y2 = y +5;
    }

    for (int i = y1; i < y2; i++) {
        for (int j = x1; j < x2; j++) {

            //if there is data
            if (chan[i][j].id != 0 && chan[i][j].message != 0) {
                isClear = 0;
            }
        }
     }
    return isClear;
}

//waits and prints the messages to the log when they are being recieved by the node
int waiting(nodeLord *node) {
    int alreadySeen = 0;
    brdcst *message = malloc(sizeof(brdcst));
    if (scanAreaWait(node->pos.x, node->pos.y, tvRemote(node->channel), message)) {
        for (int i = 0; i < node->msgcnt; i++) {
            if (node->totalMessages[i].id == message->id && node->totalMessages[i].message == message->message){
                alreadySeen = 1;
                break;
            }
        }

        if (alreadySeen != 1) {
            node->totalMessages[node->msgcnt].id = message->id;
            node->totalMessages[node->msgcnt].message = message->message;
	        node->totalMessages[node->msgcnt].recast = message->recast;

            FILE *fp = fopen(node->logLord, "a+");

            fprintf(fp, "Broadcasted from node: %d\n", node->totalMessages[node->msgcnt].id);
            fprintf(fp, "Message: %d\n", node->totalMessages[node->msgcnt].message);
            fprintf(fp, "Channel recieved on: %d\n", message->channel);
            fprintf(fp, "Rebroadcasted: %d\n", message->recast);
            fclose(fp);

            node->msgcnt += 1;
        }
    }

    free(message);
    return 0;
}


//scanAreaWait looks to see if there is anything broadcasting in the area it can waiting to, if there it it records
int scanAreaWait(int x, int y, brdcst **chan, brdcst *data) {
    int isClear = 0;
    int x1, x2;
    int y1, y2;

    if(x >= 5){
       x1 = x -5;
    }else{
       x1 = 0;
    }
    if(y >= 5){
       y1 = y-5;
    }else{
       y1 = 0;
    }
    if(x +5 >= 100){
	x2 = 100 -1;
    }
    else{
       x2 = x +5;
    }
    if(y +5 >= 100){
	y2 = 100 -1;
    }
    else{
       y2 = y +5;
    }
	
    for (int i = y1; i < y2; i++) {
        for (int j = x1; j < x2; j++) {
            //if there is data
            if (chan[i][j].id != 0 && chan[i][j].message != 0) {
                isClear = 1;
                data->id = chan[i][j].id;
                data->message = chan[i][j].message;
		        data->recast = chan[i][j].recast;
		        data->channel = chan[i][j].channel;
            }
        }
    }
    return isClear;
}

//broadcast function for the noisemaker
void noiseCast(noisemaker* maker) {
	int disrupting;
	if (pthread_mutex_trylock(parentalLock(maker->channel)) == 0) {
	    disrupting = tag(maker->broadcast, maker->pos.x, maker->pos.y, tvRemote(maker->channel));
	    sleep(maker->talkTime);
	    pthread_mutex_unlock(parentalLock(maker->channel));
	}
	return;
}


//thought loops for the noisemaker, because noisemakers dream of electric sheep
void* noisemakerDecisions(void* args) {
    noisemaker *a_maker = (noisemaker *) args;
    while (1) {
        int sleep_time = (5 + rand() % 10);
        sleep((unsigned int) sleep_time); // Make the thread sleep for that many seconds now
        double randNum = (rand() % 10) / 10; //Will generate a decimal number between 0.0 and 1.0
        //check if it wants to broadcast
        if (a_maker->talkProb >= randNum && a_maker->talkTime != 0) { //If we want to talk and the window time isn't 0
            noiseCast(a_maker);
        }
    }
}


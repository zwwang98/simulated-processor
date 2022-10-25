#include <Logger.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "List.h"
#include "Thread.h"
#include "thread_lock.h"

const char* readyList = NULL;

bool priorityCompare(void *v1, void *v2) {
    Thread *t1 = (Thread *) v1;
    Thread *t2 = (Thread *) v2;
    // method sortList sorts given list in ascending order if we implement the comparator it says (v1 <= v2 return true),
    // but here we want sort readyList in descending order so the first element is always of the highest priority
    // therefore, we need the opposite of the description, which is return false for v1 <= v2
    // return !(t1->priority <= t2->priority);
    return t1->priority > t2->priority;
}

Thread* createAndSetThreadToRun(const char* name,
                                void* (*func)(void*),
                                void* arg,
                                int pri) {
    Thread* ret = (Thread*)malloc(sizeof(Thread));
    ret->name = (char*)malloc(strlen(name) + 1);
    strcpy(ret->name, name);
    ret->func = func;
    ret->arg = arg;
    ret->priority = pri;
    ret->originalPriority = pri;

    createThread(ret);
    addToList(readyList, (void*)ret);

    // char line[1024];
    // sprintf(line, "now the list contains %d threads:\n", listSize(readyList));
    // for (int i = 0; i < listSize(readyList); i++) {
    //     Thread *curThread = (Thread*) listGet(readyList, i);
    //     sprintf(line, "wzw %s\n", curThread->name);
    // }
    // verboseLog(line);

    // this way whenever a new thread is added, the thread at index 0 will always be the one with the highest priority
    // sortList(readyList, priorityCompare);
    return ret;
}

void destroyThread(Thread* thread) {
    free(thread->name);
    free(thread);
}

/**
 * @brief 
 * 
 * @param currentTick 
 * @return Thread* 
 */
int getHighestPriorityThreadIndex() {
    if (listSize(readyList) == 0) {
        return -1;
    }
    // readyList contains at least one thread
    int index = 0; // highest priority thread's index so far
    Thread *thread = ((Thread*)listGet(readyList, index));
    int highestPriority = thread->priority;

    for (int i = 1; i < listSize(readyList); i++ ) {
        thread = ((Thread*)listGet(readyList, i));
        if (thread->priority > highestPriority) {
            index = i;
            highestPriority = thread->priority;
        }
    }

    return index;
}

Thread* nextThreadToRun(int currentTick) {
    char line[1024];
    if (listSize(readyList) == 0)
        return NULL;
    Thread* ret = NULL;
    do {
        // choose 0 based on the assumption that our thread list is always sorted in descending order of priority
        int threadIndex = 0;
        sprintf(line, "[nextThreadToRun] trying thread index %d\n",
                threadIndex);
        verboseLog(line);
        ret = ((Thread*)listGet(readyList, threadIndex));
        if (ret->state == TERMINATED) {
            sprintf(line, "[nextThreadToRun] thread %d was terminated\n",
                    threadIndex);
            verboseLog(line);
            removeFromList(readyList, ret);
            ret = NULL;
        }

    } while (listSize(readyList) > 0 && ret == NULL);
    return ret;
}

void initializeCallback() {
    readyList = createNewList();
}

void shutdownCallback() {
    destroyList(readyList);
}

int tickSleep(int numTicks) {}

void setMyPriority(int priority) {}

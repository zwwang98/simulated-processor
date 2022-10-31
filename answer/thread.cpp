#include <Logger.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "List.h"
#include "Map.h"
#include "Thread.h"
#include "thread_lock.h"

/**
 * @brief A list of ready-to-run threads sorted by priority in descending order.
 * invariant: readyList is sorted by priority in descending order
 */
const char* readyList = NULL;

/**
 * @brief The map stores information about asleep threads and when each thread
 * should wake up Map's key: int, tick. Map's value: thread to resume at that
 * tick. todo: what if we have more than one threads to resume at certain tick?
 * Looks like we need a list for the val.
 */
const char* wokentickToThreads = NULL;

/**
 * @brief A map whose key is lockId and the key's value is the thread holding
 * the lock. If no thread is holding the key/lock, the key's value should be
 * NULL.
 */
const char* lockToHoldingThread = NULL;

/**
 * @brief A map whose key is lockId and the key's value is a list of threads
 * waiting for the lock.
 */
const char* lockToWaitingThreads = NULL;

/**
 * @brief A map whose key is a thread and the key's value is a list of locks the
 * key is holding.
 */
const char* threadToHoldingLocks = NULL;

/**
 * @brief A map whose key is a thread and the key's value is the lock the key is
 * waiting for.
 */
const char* threadToWaitingLock = NULL;

// return true if v1->priority is greater than v2->prioirty
bool priorityCompare(void* v1, void* v2) {
    Thread* t1 = (Thread*)v1;
    Thread* t2 = (Thread*)v2;
    // method sortList sorts given list in ascending order if we implement the
    // comparator it says (v1 <= v2 return true), but here we want sort
    // readyList in descending order so the first element is always of the
    // highest priority therefore, we need the opposite of the description,
    // which is return false for v1 <= v2 return !(t1->priority <=
    // t2->priority);
    return t1->priority > t2->priority;
}

// PRECONDITION: readyList has been initialized
// POSTCONDITION: readyList is sorted by priority (descending)
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
    // sprintf(line, "now the list contains %d threads:\n",
    // listSize(readyList)); for (int i = 0; i < listSize(readyList); i++) {
    //     Thread *curThread = (Thread*) listGet(readyList, i);
    //     sprintf(line, "wzw %s\n", curThread->name);
    // }
    // verboseLog(line);

    // this way whenever a new thread is added, the thread at index 0 will
    // always be the one with the highest priority
    sortList(readyList, priorityCompare);
    return ret;
}

void destroyThread(Thread* thread) {
    free(thread->name);
    free(thread);
}

// PRECONDITION: readyList is sorted by priority in descending order
// POSTCONDITION: all terminated threads have been removed
Thread* nextThreadToRun(int currentTick) {
    char line[1024];

    sprintf(line, "[nextThreadToRun] Showing readyList...\n");
    verboseLog(line);
    for (int i = 0; i < listSize(readyList); i++) {
        Thread* t = (Thread*)listGet(readyList, i);
        sprintf(line, "[nextThreadToRun] %d - thread %s - priority %d\n", i,
                t->name, t->priority);
        verboseLog(line);
    }
    sprintf(line, "[nextThreadToRun] Showed readyList...\n");
    verboseLog(line);

    // if there is any thread to resume at currentTick,
    // extract them from wokentickToThreads and add them into readyList
    bool needResumeThread = MAP_CONTAINS(int, wokentickToThreads, currentTick);
    if (needResumeThread) {
        Thread* threadToResume =
            (Thread*)REMOVE_FROM_MAP(int, wokentickToThreads, currentTick);
        sprintf(line,
                "[nextThreadToRun] add thread %s from wokentickToThreads to "
                "readyList\n",
                threadToResume->name);
        verboseLog(line);
        addToList(readyList, threadToResume);
        sortList(readyList, priorityCompare);
    }

    sprintf(line, "[nextThreadToRun] there are %d threads in readyList\n",
            listSize(readyList));
    verboseLog(line);
    if (listSize(readyList) == 0) {
        return NULL;
    }

    Thread* ret = NULL;
    // choose 0 based on the assumption that our thread list is always sorted in
    // descending order of priority
    int threadIndex = 0;
    do {
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
        } else {
            sprintf(line, "[nextThreadToRun] here wzw %d\n", __LINE__);
            verboseLog(line);
            bool retIsBlocked = MAP_CONTAINS(Thread*, threadToWaitingLock, ret);
            sprintf(line, "[nextThreadToRun] here wzw %d\n", __LINE__);
            verboseLog(line);
            if (retIsBlocked) {
                sprintf(line, "[nextThreadToRun] thread %s is blocked wzw\n",
                        ret->name);
                verboseLog(line);
                ret = NULL;
            }
            sprintf(line, "[nextThreadToRun] here wzw %d\n", __LINE__);
            verboseLog(line);
        }

        threadIndex++;
    } while (listSize(readyList) > 0 && ret == NULL &&
             threadIndex < listSize(readyList));

    // removeFromList(readyList, (void*) ret);
    // addToList(readyList, ret);
    // sortList(readyList, priorityCompare);

    return ret;
}

// POSTCONDITION: readyList has been initialized
void initializeCallback() {
    readyList = createNewList();
    wokentickToThreads = CREATE_MAP(int);
    // The type must be exactly the same as lockId, which is const char *
    lockToHoldingThread = CREATE_MAP(const char*);
    lockToWaitingThreads = CREATE_MAP(const char*);
    threadToHoldingLocks = CREATE_MAP(Thread*);
    threadToWaitingLock = CREATE_MAP(Thread*);
}

void shutdownCallback() {
    destroyList(readyList);
}

// POSTCONDITION:
// 1. the current thread has been paused
// 2. the current thread will not be woken up until numTicks have passed
int tickSleep(int numTicks) {
    Thread* curThread = getCurrentThread();
    // PRECONDITION: the first thread in readyList must be
    // the current running thread returned by getCurrentThread()
    removeFromListAtIndex(readyList, 0);

    // 1.calculate the expected end tick
    // 2.
    int startTick = getCurrentTick();
    int endTick = startTick + numTicks;
    PUT_IN_MAP(int, wokentickToThreads, endTick, (void*)curThread);

    stopExecutingThreadForCycle();

    return startTick;
}

// POSTCONDITION: readyList is sorted by priority (descending)
void setMyPriority(int priority) {
    Thread* thread = getCurrentThread();
    thread->priority = priority;

    // With the precondition that readyList is always sorted
    // in descending order, we must sort readyList here
    sortList(readyList, priorityCompare);
}
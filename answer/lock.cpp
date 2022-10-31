#include "Lock.h"
#include <Logger.h>
#include <stddef.h>
#include "List.h"
#include "Map.h"
#include "thread_lock.h"

// See os_simulator/includes/Lock.student.h for explanations of these functions
// Not all of these functions must be implemented
// Remember to delete these three comments before grading for full marks

#pragma region Lock Callbacks

void lockCreated(const char* lockId) {
    PUT_IN_MAP(const char*, lockToHoldingThread, lockId, (void*)NULL);
    PUT_IN_MAP(const char*, lockToWaitingThreads, lockId, (void*)NULL);
}

void lockAttempted(const char* lockId, Thread* thread) {
    char line[1024];
    sprintf(line, "[lockAttempted] thread %s is attempting lock %s\n",
            thread->name, lockId);
    verboseLog(line);

    const char* waitingThreadList =
        (const char*)GET_FROM_MAP(const char*, lockToWaitingThreads, lockId);
    addToList(waitingThreadList, thread);
    sortList(waitingThreadList, priorityCompare);
    removeFromList(readyList, 0);

    // PUT_IN_MAP(Thread*, threadToWaitingLock, thread, (void*)lockId);

    Thread* holdingThread =
        (Thread*)GET_FROM_MAP(const char*, lockToHoldingThread, lockId);
    if (!holdingThread) {
        sprintf(line,
                "[lockAttempted] thread %s is attempting lock %s and the lock "
                "is not held by any thread\n",
                thread->name, lockId);
        verboseLog(line);
    } else {
        if (holdingThread->priority < thread->priority) {
            sprintf(line,
                    "[lockAttempted] thread %s is attempting lock %s and it "
                    "need to donate priority to thread %s\n",
                    thread->name, lockId, holdingThread->name);
            verboseLog(line);
            holdingThread->priority = thread->priority;
            sprintf(
                line,
                "[lockAttempted] thread %s just donated priority to thread %s. "
                "Now thread %s's priority is %d, thread %s's priority is %d\n",
                thread->name, holdingThread->name, thread->name,
                thread->priority, holdingThread->name, holdingThread->priority);
            verboseLog(line);
            bool isHoldingThreadBlocked =
                MAP_CONTAINS(Thread*, threadToWaitingLock, holdingThread);
            if (isHoldingThreadBlocked) {
                sprintf(line,
                        "[lockAttempted] holdingThread is blocked, priority "
                        "donation is required\n");
                verboseLog(line);
                const char* blockHoldingThreadLock = (const char*)GET_FROM_MAP(
                    Thread*, threadToWaitingLock, holdingThread);
                Thread* blockHoldingThreadThread = (Thread*)GET_FROM_MAP(
                    const char*, lockToHoldingThread, blockHoldingThreadLock);
                blockHoldingThreadThread->priority = thread->priority;
                sprintf(line,
                        "[lockAttempted] nested priority donatoin - thread %s "
                        "is attempting lock %s and it need to donate priority "
                        "to thread %s\n",
                        holdingThread->name, blockHoldingThreadLock,
                        blockHoldingThreadThread->name);
                verboseLog(line);
                sprintf(line,
                        "[lockAttempted] nested priority donatoin - thread %s "
                        "just donated priority to thread %s. Now thread %s's "
                        "priority is %d, thread %s's priority is %d\n",
                        holdingThread->name, blockHoldingThreadThread->name,
                        holdingThread->name, holdingThread->priority,
                        blockHoldingThreadThread->name,
                        blockHoldingThreadThread->priority);
            }
            sortList(readyList, priorityCompare);
        }
    }

    PUT_IN_MAP(Thread*, threadToWaitingLock, thread, (void*)lockId);
}

void lockAcquired(const char* lockId, Thread* thread) {
    char line[1024];
    sprintf(line, "[lockAcquired] thread %s acquired lock %s\n", thread->name,
            lockId);
    verboseLog(line);
    PUT_IN_MAP(const char*, lockToHoldingThread, lockId, (void*)thread);
    REMOVE_FROM_MAP(Thread*, threadToWaitingLock, thread);
}

void lockFailed(const char* lockId, Thread* thread) {
    char line[1024];
    sprintf(line, "[lockFailed] thread %s failed to acquire lock %s\n",
            thread->name, lockId);
    verboseLog(line);

    removeFromList(readyList, (void*)thread);
    PUT_IN_MAP(Thread*, threadToWaitingLock, thread, (void*)lockId);
    const char* waitingThreadList =
        (const char*)GET_FROM_MAP(const char*, lockToWaitingThreads, lockId);
    addToList(waitingThreadList, thread);
    sortList(waitingThreadList, priorityCompare);
}

void lockReleased(const char* lockId, Thread* thread) {
    char line[1024];
    sprintf(line, "[lockReleased] thread %s released lock %s\n", thread->name,
            lockId);
    verboseLog(line);
    thread->priority = thread->originalPriority;
    sortList(readyList, priorityCompare);
    PUT_IN_MAP(const char*, lockToHoldingThread, lockId, (void*)NULL);

    const char* waitingThreadList =
        (const char*)GET_FROM_MAP(const char*, lockToWaitingThreads, lockId);
    for (int i = 0; i < listSize(waitingThreadList); i++) {
        Thread* t = (Thread*)listGet(waitingThreadList, i);
        REMOVE_FROM_MAP(Thread*, threadToWaitingLock, t);
    }
}

#pragma endregion

#pragma region Lock Functions

Thread* getThreadHoldingLock(const char* lockId) {
    return (Thread*)GET_FROM_MAP(const char*, lockToHoldingThread, lockId);
}

#pragma endregion
#ifndef _THREAD_LOCK_H
#define _THREAD_LOCK_H

// You can use this header to share data and/or definitions between your
// lock.cpp and thread.cpp.

// For example, if you wanted to share a map between the two...

// extern const char *sharedMap;

extern const char* readyList;
extern const char* wokentickToThreads;

extern const char* lockToHoldingThread;
extern const char* lockToWaitingThreads;
extern const char* threadToHoldingLocks;
extern const char* threadToWaitingLock;

bool priorityCompare(void* v1, void* v2);

// Then in ONE OF lock.cpp or thread.cpp you would write

// const char *sharedMap = CREATE_MAP(const char*);

// Now both lock.cpp and thread.cpp may refer to sharedMap.

#endif
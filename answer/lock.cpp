#include "Lock.h"
#include <stddef.h>
#include "thread_lock.h"

// See os_simulator/includes/Lock.student.h for explanations of these functions
// Not all of these functions must be implemented
// Remember to delete these three comments before grading for full marks

#pragma region Lock Callbacks

void lockCreated(const char* lockId) {}
void lockAttempted(const char* lockId, Thread* thread) {}
void lockAcquired(const char* lockId, Thread* thread) {}
void lockFailed(const char* lockId, Thread* thread) {}
void lockReleased(const char* lockId, Thread* thread) {}

#pragma endregion

#pragma region Lock Functions

Thread* getThreadHoldingLock(const char* lockId) {
    return NULL;
}

#pragma endregion
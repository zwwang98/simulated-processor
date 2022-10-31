# Project 2 Questions

## Directions

Answer each question to the best of your ability; feel free to copy sections of
your code or link to it if it helps explain but answers such as "this line
guarantees that the highest priority thread is woken first" are not sufficient.

Your answers can be relatively short - around 100 words, and no more than 200,
should suffice per subsection.

To submit, edit this file and replace every `ANSWER_BEGIN` to `ANSWER_END` with
your answer. Your submission should not include **any** instances of
`ANSWER_BEGIN` or `ANSWER_END`; the text provided is meant to give you some tips
on how to begin answering.

## Scheduling

**Two tests have infinite loops with the scheduler you were provided. Explain
why `Running.PriorityOrder` will never terminate with the basic scheduler.**

ANSWER_BEGIN

1. Method `nextThreadToRun()` is called by `ThreadManager` every tick to decide which thread to run at the tick.
2. `readyList` stores all runnable threads
3. In the basic scheduler, the `readyList` follows FIFO rule and we always pick the first thread to run, so we always run the `Lo Pri` thread.
4. `Mi Pri` thread and `Hi Pri` thread are never scheduled, so we have a infinite loop. We start `Lo Pri` thread at tick 00001 and keep resuming `Lo Pri` thread at every following tick.

If this question doesn't make sense at all, take a look at `test_config.h`.

Turning on verbose logging and reading it will help answer this question.

ANSWER_END

## Sleep

**Briefly describe what happens in your `tickSleep(int numTicks)` function.**

ANSWER_BEGIN
1. pull current running thread `curThread` out of `readyList`
2. get current tick as the "sleep-is-supposed-to-start" tick - `startTick`
3. calculate expected end-sleep tick `endTick` by `startTick + numTicks` 
4. store the extracted thread into `sleepMap` with a key of expected end-sleep tick  
   ```C++
    PUT_IN_MAP(int, sleepMap, endTick, (void*)curThread);
   ```
   `sleepMap`'s key is the tick, `sleepMap`'s value is the thread need to resume at given tick
5. call `stopExecutingThreadForCycle()` to `PAUSE` the thread

ANSWER_END

**How does your `sleep(...)` function guarantee duration?**

ANSWER_BEGIN

What prevents a thread from waking up too early?
1. As I have explained above, an asleep thread will be moved out from `readyList` and it will never be added into `readyList` again until its expected awake tick reaches
2. In our `nextThreadToRun()`, the sheduler fetches ready-to-run threads from `readyList`. Since we do not put asleep threads in `readyList`, those asleep threads are prevented from waking up too early.

What prevents a thread from _never_ waking up?
1. When ask a thread to sleep, we extract the thread out of `readyList` and `PAUSE` it. We also store this thread with the key of its expected woken tick in `sleepMap`.
2. At each tick, we utilize `sleepMap` to check if there is any thread to be woken. If so, put them back into `readyList` so that these woken threads will eventually be scheduled.  
3. Since all woken threads are added to `readyList` again, they all will eventyally be schedulled, which means they will eventually wake up.

ANSWER_END

**Do you have to check every sleeping thread every tick? Why not?**

You shouldn't have to check every thread every tick.

You should be able to take advantage of knowing how long a thread will sleep when `sleep(...)` is called.

ANSWER_BEGIN

No. Because with the help of `sleepMap`, we only need to check if the `sleepMap` contains current tick as a key. If so, it means we need to wake up some threads.

ANSWER_END

## Locking

**How do you ensure that the highest priority thread waiting for a lock gets it
first?**

ANSWER_BEGIN

This is similar to how you should be handling `sleep(...)`ing threads.

1. The `readyList` is always sorted by priority in descending order
2. So in the `nextThreadToRun()` method, the thread with the highest priority will always get scheduled first, which means it will get the lock first.

ANSWER_END

## Priority Donation

**Describe the sequence of events before and during a priority donation.**

ANSWER_BEGIN

Your answer should talk about the order of framework functions called and how
priority donation opportunities are detected.

1. The order of framework functions called
   1. The current executing thread calls `lock(const char* lockId)` to lock a given lock `lockId`.
   2. Inside the `lock(const char* lockId)` method, we invokes `lockAttempted(const char* lockId, Thread* thread)` first so that we could know if the given lock `lockId` is held by other thread, check priority donation and also to update the map `threadToWaitingLock` if needed

2. How priority donation opportunities are detected  
   Priprity donation opportunities are detected ruring `lockAttempted(const char* lockId, Thread* thread)`. When the `thread` finds that the `lockId` is held by another thread `holdingThread`, if the `holdingThread` has lower priority than `thread`, it means a priority donation is required to allow lower priority thread `holdingThread` to have chance to release the `lockId`.

ANSWER_END

**How is nested donation handled?**

ANSWER_BEGIN

Describe a system of three threads (A, B, C) and three locks (1, 2, 3).
1. At the begining,
   | Thread | Holding Locks | Priority |Request Locks|
   |:------:|:-------------:|:--------:|:-----------:|
   |   A    |       1       |    Lo    |             |
   |   B    |       2       |    Mi    |      1      |
   |   C    |       3       |    Hi    |      2      |
2. Thread B(pri=Mi) wants lock 1 and lock 1 is held by thread A(pri=Lo). To allow thread A to release lock 1, we need to schedule thread A although it has lower priority than thread B. So we donate priority from B to A. After the donation, thread A and thread B are both of Mi priority.
3. Thread C(pri=Hi) wants lock 2 and lock 2 is held by thread B(pri=Mi). To allow thread B to realease lock 2, we need to schedule thread B although it has lower priority than thread C. So we donate priority from C to B. After the donation, thread B and thread C are both of Hi priority.

ANSWER_END

**How do you ensure that the highest priority thread waiting is called when the
lock is released?**

ANSWER_BEGIN

Apply your previous answer to your code.

1. When the lock is realeased, method `lockReleased(const char* lockId, Thread* thread)` is invoked.
   ```C++
   void lockReleased(const char* lockId, Thread* thread) {
      char line[1024];
      sprintf(line, "[lockReleased] thread %s released lock %s\n", thread->name, lockId);
      verboseLog(line);
      thread->priority = thread->originalPriority;
      sortList(readyList, priorityCompare);
      PUT_IN_MAP(const char*, lockToHoldingThread, lockId, (void*) NULL);

      const char * waitingThreadList = (const char *) GET_FROM_MAP(const char *, lockToWaitingThreads, lockId);
      for (int i = 0; i < listSize(waitingThreadList); i++) {
         Thread* t = (Thread *) listGet(waitingThreadList, i);
         REMOVE_FROM_MAP(Thread*, threadToWaitingLock, t);
      }
   }
   ```
2. Inside the method, we will remove all threads in `lockToWaitingThreads` under the key of given `lockId` from `threadToWaitingLock` map so that we will know these threads is not blocked by any lock during the `nextThreadToRun()` method.
3. In the `nextThreadToRun()` method, we choose the highest priority non-blocked thread to run. So we can ensure that the highest priority thread waiting is called when the lock is released.


ANSWER_END

### Bonus Points
1. In `Thread.h`, the comments of method `void stopExecutingThreadForCycle();`.
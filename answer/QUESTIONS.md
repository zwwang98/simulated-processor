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

??? Why with the basic scheduler, the Lo Pri Thread never teminates? I found that it has something to do with the piece of code below. It looks like we always get `status` as `false`. But why is that case?
```C++
bool status = currentThread->joinWithTimeout();
setRunningThread(idleThread);
if (InternalLogger::getLogger().isVerbose()) {
    InternalLogger::eventSink()
        << "[ThreadManager] "
        << "End of cycle for thread " << newThread->name << "\n";
    InternalLogger::getLogger().flush();
}
if (!status) {
    if (InternalLogger::getLogger().isVerbose()) {
        InternalLogger::eventSink()
            << "[ThreadManager] "
            << "Pausing thread " << newThread->name << "\n";
        InternalLogger::getLogger().flush();
    }
    currentThread->pause();
    if (InternalLogger::getLogger().isVerbose()) {
        InternalLogger::eventSink() << "[ThreadManager] "
                                    << "Successfully paused thread "
                                    << newThread->name << "\n";
        InternalLogger::getLogger().flush();
    }
} else {
    if (InternalLogger::getLogger().isVerbose()) {
        InternalLogger::eventSink()
            << "[ThreadManager] "
            << "Terminating thread " << newThread->name << "\n";
        InternalLogger::getLogger().flush();
    }
    currentThread->terminated();
}
```

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

ANSWER_END

## Priority Donation

**Describe the sequence of events before and during a priority donation.**

ANSWER_BEGIN

Your answer should talk about the order of framework functions called and how
priority donation opportunities are detected.

ANSWER_END

**How is nested donation handled?**

ANSWER_BEGIN

Describe a system of three threads (A, B, C) and three locks (1, 2, 3).

ANSWER_END

**How do you ensure that the highest priority thread waiting is called when the
lock is released?**

ANSWER_BEGIN

Apply your previous answer to your code.

ANSWER_END

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

For one point, remember to delete this section (Directions) before submitting.

## Scheduling

**Two tests have infinite loops with the scheduler you were provided. Explain
why `Running.PriorityOrder` will never terminate with the basic scheduler.**

ANSWER_BEGIN

If this question doesn't make sense at all, take a look at `test_config.h`.

Turning on verbose logging and reading it will help answer this question.

ANSWER_END

## Sleep

**Briefly describe what happens in your `sleep(...)` function.**

ANSWER_BEGIN

1. A list of
2. Numbered steps in
3. Plain English will be fine
4. Not too many steps.

ANSWER_END

**How does your `sleep(...)` function guarantee duration?**

ANSWER_BEGIN

What prevents a thread from waking up too early?

What prevents a thread from _never_ waking up?

ANSWER_END

**Do you have to check every sleeping thread every tick? Why not?**

ANSWER_BEGIN

You shouldn't have to check every thread every tick.

You should be able to take advantage of knowing how long a thread will sleep
when `sleep(...)` is called.

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

# CppTask

## About

CppTask is a lightweight C++ library that provides a *Task* class for running 
operations asynchronously with ease, using simple lambdas.

The Task class supports both lambdas with and without return values. Tasks can 
be executed asynchronously or awaited until completion. You can also create 
already-completed tasks, again with or without results. The Task class is 
thread-safe and exposes its current status through a well-defined task state.

Task execution is handled by an internal thread pool, fully managed by the 
library. You can run tasks without having to deal with low-level thread 
management yourself.

Tasks are implemented through an interface, making them easier to test and 
integrate as parameters.


## Usage

All library functionality is used with the **CppTask_Task.h** header:

```cpp
#include <libcpptask/CppTask_Task.h>
```


### Task creation

Tasks are created like any other class: define the task’s return type and pass 
a matching lambda to the constructor.

```cpp
std::string output = "Hello!";

CppTask::Task<void> task([output](){
    std::cout << output << std::endl;
});
```

Tasks can also return a result. In this case, the return type must be specified 
instead of **void**, and the lambda must return that type:

```cpp
int counter = 0;

CppTask::Task<int> task([counter](){
    return counter + 1;
});
```

> [!TIP]
> You can also return more complex types like classes or structs.

Creating a already completed task is also possible:

```cpp
// Create a task without a return value
auto voidTask = CppTask::Task<void>CompletedTask();

// Create a completed task with a specific integer result
auto intTask = CppTask::Task<int>CompletedTask(32);
```


### Running a task

A task is executed by starting it and then waiting for completion:

```cpp
// Run a task async, and then wait for the task to complete
task.RunAsync();
task.Await();

// It is also possible to combine the two
// The Run() function will start an async task and immediately wait for it
task.Run();
```

> [!IMPORTANT]
> A task can only be run once!

You can check a task’s current state using **GetState()**:

```cpp
switch (task.GetState())
{
    case CppTask::TaskState::WAITING:
        // The task is still waiting to be run
        break;
    case CppTask::TaskState::RUNNING:
        // The task is currently running, but not finished
        break;
    case CppTask::TaskState::FINISHED:
        // The task has completed
        break;
}
```

> [!TIP]
> Checking the task state can help you avoid blocking calls and decide whether 
> to skip certain tasks.

If a task returns a result, you can retrieve it once the task has finished. 
Results are stored and remain available as long as the task instance exists:

```cpp
// Get the result of a task run
// The task has to be completed
auto result = task.GetResult();

// Waiting for a result of a currently running task is also possible
// This helper function waits for the task and returns the result
auto result = task.AwaitResult();
```

### Task interface

Tasks are represented by the **ITask** interface, which helps keep components 
testable and also allows you to provide custom task implementations.

```cpp
#include <libcpptask/CppTask_ITask.h>
```

## Licence

This project is licenced under the Apache 2.0 licence. 
Please read the included LICENSE.txt for the exact terms.


## Directories

Directory | Description
--------- | -----------
include | The library headers.
src | Library source code.
test | Library test source code.
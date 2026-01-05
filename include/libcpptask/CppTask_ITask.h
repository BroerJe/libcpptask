/**
 *  Copyright (C) 2025, BroerJe.
 *  https://github.com/BroerJe
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef libcpptask_CppTask_ITask_h
#define libcpptask_CppTask_ITask_h

// STL

// External

// Project
#include "./CppTask_Exception.h"


// Namespace
namespace CppTask {

//******************************************************************************
// Task State
//******************************************************************************

/**
 *  @brief The states a task can inhabit.
 */
enum TaskState
{
    WAITING = 0,
    RUNNING = 1,
    FINISHED = 2
};

//******************************************************************************
// Task Interface
//******************************************************************************

/**
 *  @brief The task interface defines the functionality expected of a task. This
 *         interface can be used to implement special tasks, or help with the
 *         creation of test code.
 */
template <typename T>
class ITask
{
public:

    //**************************************************************************
    // Destructor
    //**************************************************************************
    
    /**
     *  @brief Default destructor.
     */
    virtual ~ITask() noexcept = default;
    
    //**************************************************************************
    // Run
    //**************************************************************************
    
    /**
     *  @brief Run the task synchronously. Running the task is only possible
     *         once. This function is thread-safe.
     */
    virtual void
    Run() = 0;

    /**
     *  @brief Run the task asynchronously. Running the task is only possible
     *         once. This function is thread-safe.
     */
    virtual void
    RunAsync() = 0;
    
    //**************************************************************************
    // Await
    //**************************************************************************

    /**
     *  @brief Wait for a task to finished. This function is thread-safe.
     */
    virtual void
    Await() const = 0;

    /**
     *  @brief Get the result of a finished task. This function is thread-safe.
     *
     *  @returns The task result.
     */
    virtual T
    GetResult() const = 0;

    /**
     *  @brief Wait for a task to finish and return the task result. This 
     *         function will return the already finished result if one exists.
     *         The same result will be returned for repeated calls. This 
     *         function is thread-safe.
     *
     *  @returns The task result.
     */
    virtual T
    AwaitResult() const = 0;

    //**************************************************************************
    // State
    //**************************************************************************
    
    /**
     *  @brief Get the current task state. This function is thread-safe.
     * 
     *  @returns The current task state.
     */
    virtual TaskState
    GetState() const = 0;

protected:

    //**************************************************************************
    // Constructor
    //**************************************************************************
    
    /**
     *  @brief Default constructor.
     */
    ITask() noexcept = default;
};

// Namespace
}

#endif /* libcpptask_CppTask_ITask_h */
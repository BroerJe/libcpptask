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

#ifndef libcpptask_CppTask_Task_h
#define libcpptask_CppTask_Task_h

// STL
#include <functional>
#include <memory>

// External

// Project
#include "./CppTask_ITask.h"


// Namespace
namespace CppTask {

//******************************************************************************
// Task Implementation
//******************************************************************************

/**
 *  @brief The task thread class is responsible for running the task content
 *         on a separate thread, storing the result and setting the task state.
 */
class TaskThread
{
    // We want everything hidden, except the existence of the class
    // This thread class is meant for use by the task and thread pool class only
    // Sadly, It is not really possible to hide the definition completely
    template<typename T> friend class Task;
    friend class ThreadPool;

public:

    //**************************************************************************
    // Destructor
    //**************************************************************************

    /**
     *  @brief Default destructor.
     */
    virtual ~TaskThread() noexcept;

private:

    //**************************************************************************
    // Constructor
    //**************************************************************************
        
    /**
     *  @brief Default constructor.
     *
     *  @param c_rFunction The function of the task thread to run.
     */
    TaskThread(const std::function<void(TaskThread& rTaskThread)>& c_rFunction);
        
    //**************************************************************************
    // Run
    //**************************************************************************
    
    /**
     *  @brief Enqueue a task thread to be run. This function is thread-safe.
     *
     *  @param pTaskThread The task thread to enqueue.
     */
    static void
    Enqueue(std::shared_ptr<TaskThread> pTaskThread);

    /**
     *  @brief Run the task thread with the given function. This function is
     *         thread-safe.
     */
    void
    Run();
        
    //**************************************************************************
    // Await
    //**************************************************************************

    /**
     *  @brief Set the result of a task. This will notify all which are
     *         waiting for the task to finish. This function is thread-safe.
     */
    void
    SetFinished();

    /**
     *  @brief Wait for the task to be finished. This function will return 
     *         instantly if the task already finished. This function is
     *         thread-safe.
     */
    void
    Await();
        
    //**************************************************************************
    // Result
    //**************************************************************************

    /**
     *  @brief Set the result of a task. This function is thread-safe.
     *
     *  @param c_pSrc The source to read the result from. The data is copied.
     *  @param size The size available to write to.
     */
    void
    SetResult(const void* c_pPtr, size_t size);

    /**
     *  @brief Retrieve the result of a task. The same result will be returned
     *         for repeated calls. This function is thread-safe.
     *
     *  @param pDst The destination to write the result to.
     *  @param size The size available to write to.
     */
    void
    GetResult(void* pPtr, size_t size);

    //**************************************************************************
    // State
    //**************************************************************************
    
    /**
     *  @brief Get the current task thread state. This function is thread-safe.
     * 
     *  @returns The current task thread state.
     */
    TaskState
    GetState();

    //**************************************************************************
    // Storage
    //**************************************************************************

    /**
     *  @brief The storage hides the actual content used by the task thread
     *         implementation.
     */
    struct Storage;

    //**************************************************************************
    // Data
    //**************************************************************************

    std::shared_ptr<Storage> m_pStorage;
};

//******************************************************************************
// Task Template
//******************************************************************************

/**
 *  @brief The task class allows to run asynchronous operations. Tasks can
 *         return a result if needed. A task can only be run once, but results
 *         can be retrieved multiple times. 
 *         
 *         Task results are stored and stay available until the last task 
 *         instance has been destroyed.
 * 
 *         Make sure to capture the variables needed by the task function
 *         correctly to prevent crashes. Tasks are unable to handle exceptions 
 *         because of lambda limitations; make sure to handle exceptions in the
 *         provided task function.
 * 
 *         Tasks will keep running, even if the last held instance goes out of 
 *         scope. There is no way to cancel a task once it has been started,
 *         unless the task function includes functionality to implement such 
 *         behaviour.
 */
template <typename T>
class Task : public ITask<T>
{
public:

    //**************************************************************************
    // Constructor / Destructor
    //**************************************************************************
    
    /**
     *  @brief Default constructor.
     *
     *  @param c_rTaskFunction The function of the task to run.
     */
    Task(const std::function<T()>& c_rTaskFunction)
    : m_pTaskThread(new TaskThread([c_rTaskFunction](TaskThread& rTaskThread){
        if constexpr (std::is_same_v<T, void>)
        {
            c_rTaskFunction();
            rTaskThread.SetFinished();
        }
        else
        {
            auto result = c_rTaskFunction();
            rTaskThread.SetResult(&result, sizeof(result));
            rTaskThread.SetFinished();
        }
    }))
    {}
    
    /**
     *  @brief Default destructor.
     */
    virtual ~Task() noexcept
    {}
    
    //**************************************************************************
    // Completed Task
    //**************************************************************************
    
    /**
     *  @brief Create an already completed task.
     *
     *  @returns The completed task.
     */
    template <typename U = T>
    static typename std::enable_if<std::is_void<U>::value, std::shared_ptr<Task<U>>>::type
    CompletedTask()
    {
        auto pTask = std::make_shared<Task<U>>([](){});
        pTask->m_pTaskThread->SetFinished();

        return pTask;
    }

    /**
     *  @brief Create an already completed task.
     *
     *  @param c_rResult The result of the completed task.
     *
     *  @returns The completed task.
     */
    template <typename U = T>
    static typename std::enable_if<!std::is_void<U>::value, std::shared_ptr<Task<U>>>::type
    CompletedTask(const U& c_Result)
    {
        // While this function shouldn't be called since we immediately
        // finish the task we still return a valid result. For this we capture
        // the task itself in the task lambda. The task is known to exist as
        // long as the lambda does, since the task owns it
        std::shared_ptr<Task<U>> pTask = std::make_shared<Task<U>>([&pTask](){
            return pTask->GetResult();
        });

        pTask->m_pTaskThread->SetResult(&c_Result, sizeof(c_Result));
        pTask->m_pTaskThread->SetFinished();

        return pTask;
    }

    //**************************************************************************
    // Run
    //**************************************************************************
    
    /**
     *  @brief Run the task synchronously. Running the task is only possible
     *         once. This function is thread-safe.
     */
    void
    Run() override
    {
        RunAsync();
        Await();
    }
    
    /**
     *  @brief Run the task asynchronously. Running the task is only possible
     *         once. This function is thread-safe.
     */
    void
    RunAsync() override
    {
        TaskThread::Enqueue(m_pTaskThread);
    }
    
    //**************************************************************************
    // Await
    //**************************************************************************

    /**
     *  @brief Wait for a task to finished. This function is thread-safe.
     */
    void
    Await() override
    {
        m_pTaskThread->Await();
    }

    /**
     *  @brief Get the result of a finished task. This function is thread-safe.
     *
     *  @returns The task result.
     */
    T
    GetResult() override
    {
        if constexpr (!std::is_same_v<T, void>)
        {
            auto size = sizeof(T);
            char pDst[size];

            m_pTaskThread->GetResult(pDst, size);

            return *(T*)pDst;
        }
    }

    /**
     *  @brief Wait for a task to finish and return the task result. This 
     *         function will return the already finished result if one exists.
     *         The same result will be returned for repeated calls. This 
     *         function is thread-safe.
     *
     *  @returns The task result.
     */
    T
    AwaitResult() override
    {
        Await();

        if constexpr (!std::is_same_v<T, void>)
        {
            return GetResult();
        }
    }

    //**************************************************************************
    // State
    //**************************************************************************
    
    /**
     *  @brief Get the current task state. This function is thread-safe.
     * 
     *  @returns The current task state.
     */
    TaskState
    GetState() override
    {
        return m_pTaskThread->GetState();
    }

private:
 
    //**************************************************************************
    // Data
    //**************************************************************************

    std::shared_ptr<TaskThread> m_pTaskThread;
};

// Namespace
}

#endif /* libcpptask_CppTask_Task_h */
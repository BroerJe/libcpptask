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

#ifndef libcpptask_CppTask_ThreadPool_h
#define libcpptask_CppTask_ThreadPool_h

// STL
#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>

// External

// Project
#include "../include/libcpptask/CppTask_Task.h"


// Namespace
namespace CppTask {

/**
 *  @brief The thread pool singleton is responsible for running individual
 *         task thread instances.
 */
class ThreadPool
{
public:

    //**************************************************************************
    // Constructor
    //**************************************************************************
    
    /**
     *  @brief Copy constructor. Disabled for this class.
     *
     *  @param c_rThreadPool ThreadPool class source.
     */
    ThreadPool(const ThreadPool& c_rThreadPool) = delete;
    
    //**************************************************************************
    // Singleton
    //**************************************************************************
    
    /**
     *  @brief Get the singleton class instance. This function is thread-safe.
     *
     *  @returns The singleton class instance.
     */
    static ThreadPool&
    Singleton();
    
    //**************************************************************************
    // Enqueue
    //**************************************************************************
    
    /**
     *  @brief Enqueue a task thread to be run on the thread pool. This function
     *         is thread-safe.
     *
     *  @param pTaskThread The task thread to run.
     */
    void
    Enqueue(std::shared_ptr<TaskThread> pTaskThread);

private:

    //**************************************************************************
    // Constructor / Destructor
    //**************************************************************************
    
    /**
     *  @brief Default constructor.
     */
    ThreadPool();

    /**
     *  @brief Default destructor.
     */
    virtual ~ThreadPool() noexcept;

    //**************************************************************************
    // Run Thread
    //**************************************************************************

    /**
     *  @brief Run a task thread.
     * 
     *  @param pInstance The class instance to update with.
     */
    static void
    RunThread(ThreadPool* pInstance) noexcept;

    //**************************************************************************
    // Variables
    //**************************************************************************
    
    std::list<std::thread> m_threads;
    std::mutex m_mutex;
    std::condition_variable m_condition;

    bool m_runThreads;

    std::list<std::shared_ptr<TaskThread>> m_taskThreads;
};

// Namespace
}

#endif /* libcpptask_CppTask_ThreadPool_h */
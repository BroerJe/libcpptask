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

// STL
#include <iostream>
#include <thread>

// External

// Project
#include "./CppTask_ThreadPool.h"


#ifdef libcpptask_THREAD_POOL_FORCED_THREAD_COUNT
    #if libcpptask_THREAD_POOL_FORCED_THREAD_COUNT <= (0)
        #error "Invalid task thread pool thread count!"
    #endif
#endif


// Namespace
namespace CppTask {

//******************************************************************************
// MARK: Constructor / Destructor
//******************************************************************************

ThreadPool::ThreadPool()
: m_runThreads(true)
{
#ifndef libcpptask_THREAD_POOL_FORCED_THREAD_COUNT
    auto threadCount = std::thread::hardware_concurrency();

    // Always keep one thread completely free, and create at least one
    // We want to keep concurrency as healthy as possible
    if (threadCount == 0)
    {
        threadCount = 1;
    }
    else if (threadCount > 1)
    {
        --threadCount;
    }

    for (size_t i = 0; i < threadCount; ++i)
#else
    for (size_t i = 0; i < libcpptask_THREAD_POOL_FORCED_THREAD_COUNT; ++i)
#endif
    {
        m_threads.emplace_back(RunThread, this);
    }
}

ThreadPool::~ThreadPool() noexcept
{
    {
        std::lock_guard<std::mutex> lockGuard(m_mutex);

        m_runThreads = false;
        m_condition.notify_all();
    }

    for (auto& rThread : m_threads)
    {
        if (rThread.joinable())
        {
            rThread.join();
        }
    }
}

//******************************************************************************
// MARK: Singleton
//******************************************************************************
    
ThreadPool&
ThreadPool::Singleton()
{
    static ThreadPool s_threadPool;
    return s_threadPool;
}
    
//******************************************************************************
// MARK: Enqueue
//******************************************************************************

void
ThreadPool::Enqueue(std::shared_ptr<TaskThread> pTaskThread)
{
    if (!pTaskThread)
    {
        throw Exception("Invalid parameters!");
    }

    std::lock_guard<std::mutex> lockGuard(m_mutex);

    if (!m_runThreads)
    {
        throw Exception("Thread pool is stopped!");
    }

    m_taskThreads.emplace_back(pTaskThread);
    m_condition.notify_one();
}

//******************************************************************************
// MARK: Run Thread
//******************************************************************************

void
ThreadPool::RunThread(ThreadPool* pInstance) noexcept
{
    std::shared_ptr<TaskThread> pTaskThread(nullptr);

    while (true)
    {
        {
            std::unique_lock<std::mutex> uniqueLock(pInstance->m_mutex);

            if (pInstance->m_taskThreads.empty())
            {
                pInstance->m_condition.wait(uniqueLock);
            }

            if (!pInstance->m_runThreads)
            {
                break;
            }
            else if (pInstance->m_taskThreads.empty())
            {
                continue;
            }

            pTaskThread.swap(pInstance->m_taskThreads.front());
            pInstance->m_taskThreads.pop_front();
        }

        try
        {
            pTaskThread->Run();
        }
        catch (const std::exception& e)
        {
#if defined(DEBUG) || defined(_DEBUG)
            std::stderr << e.what();
#endif
        }
    }
}

// Namespace
}

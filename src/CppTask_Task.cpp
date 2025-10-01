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
#include <cstring>
#include <mutex>
#include <condition_variable>

// External

// Project
#include "../include/libcpptask/CppTask_Task.h"
#include "./CppTask_ThreadPool.h"


// Namespace
namespace CppTask {

//******************************************************************************
// Implementation Storage
//******************************************************************************

struct TaskThread::Storage
{
    std::mutex m_mutex;
    std::condition_variable m_condition;

    std::function<void(TaskThread& rTaskThread)> m_function;
    TaskState m_state;
    std::shared_ptr<char[]> m_pResult;
};

//******************************************************************************
// Constructor
//******************************************************************************

TaskThread::TaskThread(const std::function<void(TaskThread& rTaskThread)>& c_rFunction)
: m_pStorage(std::make_shared<Storage>())
{
    m_pStorage->m_function = c_rFunction;
    m_pStorage->m_state = TaskState::WAITING;
    m_pStorage->m_pResult = nullptr;
}

//******************************************************************************
// Run
//******************************************************************************

void
TaskThread::Enqueue(std::shared_ptr<TaskThread> pTaskThread)
{
    std::lock_guard<std::mutex> lockGuard(pTaskThread->m_pStorage->m_mutex);

    if (pTaskThread->m_pStorage->m_state != TaskState::WAITING)
    {
        throw Exception("Attempted to enqueue a task already run before!");
    }

    ThreadPool::Singleton().Enqueue(pTaskThread);
}

void
TaskThread::Run()
{
    {
        std::lock_guard<std::mutex> lockGuard(m_pStorage->m_mutex);

        if (m_pStorage->m_state != TaskState::WAITING)
        {
            throw Exception("Attempted to run a task already run before!");
        }

        m_pStorage->m_state = TaskState::RUNNING;
    }

    // We do not need a try-catch block here, since this is an external lambda
    // We will not be able to catch something, the thread will die silently
    // before or call std::terminate
    m_pStorage->m_function(*this);
    
    // Do not set the state yet, we want to set the result first before that
    // We have the SetFinished() function to call after the result has been set
}

//******************************************************************************
// Await
//******************************************************************************

void
TaskThread::SetFinished()
{
    std::lock_guard<std::mutex> lockGuard(m_pStorage->m_mutex);
    
    if (m_pStorage->m_state != TaskState::FINISHED)
    {
        m_pStorage->m_state = TaskState::FINISHED;
        m_pStorage->m_condition.notify_all();
    }
}

void
TaskThread::Await()
{
    std::unique_lock<std::mutex> uniqueLock(m_pStorage->m_mutex);

    if (m_pStorage->m_state != TaskState::FINISHED)
    {
        m_pStorage->m_condition.wait(uniqueLock);
    }
}

//******************************************************************************
// Result
//******************************************************************************

void
TaskThread::SetResult(const void* c_pPtr, size_t size)
{
    if (c_pPtr == nullptr || size == 0)
    {
        throw Exception("Invalid parameters!");
    }

    std::lock_guard<std::mutex> lockGuard(m_pStorage->m_mutex);

    m_pStorage->m_pResult = std::shared_ptr<char[]>(new char[size]);
    std::memcpy(m_pStorage->m_pResult.get(), c_pPtr, size);
}

void
TaskThread::GetResult(void* pPtr, size_t size)
{
    if (pPtr == nullptr || size == 0)
    {
        throw Exception("Invalid parameters!");
    }

    std::lock_guard<std::mutex> lockGuard(m_pStorage->m_mutex);

    if (m_pStorage->m_pResult == nullptr)
    {
        throw Exception("No result available to return!");
    }
    
    std::memcpy(pPtr, m_pStorage->m_pResult.get(), size);
}

//******************************************************************************
// State
//******************************************************************************

TaskState
TaskThread::GetState()
{
    std::lock_guard<std::mutex> lockGuard(m_pStorage->m_mutex);
    return m_pStorage->m_state;
}

// Namespace
}

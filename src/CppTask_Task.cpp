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
#include <mutex>
#include <condition_variable>

// External

// Project
#include "../include/libcpptask/CppTask_Task.h"
#include "./CppTask_ThreadPool.h"


// Namespace
namespace CppTask {

//******************************************************************************
// MARK: Task Implementation Storage Object
//******************************************************************************

struct TaskThread::Storage
{
    mutable std::mutex m_mutex;
    mutable std::condition_variable m_condition;

    std::function<void(TaskThread& rTaskThread)> m_function;
    TaskState m_state;
    std::any m_result;
};

//******************************************************************************
// MARK: Constructor
//******************************************************************************

TaskThread::TaskThread(const std::function<void(TaskThread& rTaskThread)>& c_rFunction)
: m_pStorage(std::make_shared<Storage>())
{
    m_pStorage->m_function = c_rFunction;
    m_pStorage->m_state = TaskState::WAITING;
}

//******************************************************************************
// MARK: Run Task
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
// MARK: Await Task
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
TaskThread::Await() const
{
    std::unique_lock<std::mutex> uniqueLock(m_pStorage->m_mutex);

    if (m_pStorage->m_state != TaskState::FINISHED)
    {
        m_pStorage->m_condition.wait(uniqueLock);
    }
}

//******************************************************************************
// MARK: Task Result
//******************************************************************************

void
TaskThread::SetResult(std::any result)
{
    std::lock_guard<std::mutex> lockGuard(m_pStorage->m_mutex);

    m_pStorage->m_result = std::move(result);
}

const std::any&
TaskThread::GetResult() const
{
    std::lock_guard<std::mutex> lockGuard(m_pStorage->m_mutex);

    if (!m_pStorage->m_result.has_value())
    {
        throw Exception("No result available to return!");
    }

    return m_pStorage->m_result;
}

//******************************************************************************
// MARK: Task State
//******************************************************************************

TaskState
TaskThread::GetState() const
{
    std::lock_guard<std::mutex> lockGuard(m_pStorage->m_mutex);
    
    return m_pStorage->m_state;
}

// Namespace
}

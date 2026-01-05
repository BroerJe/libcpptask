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
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>

// External
#include <gtest/gtest.h>

// Project
#include "../../include/libcpptask/CppTask_Task.h"


/**
 *  @TODO: A lot of these tests are timer-based, which makes them less robust.
 *         Swap with helpers and similar.
 */


//******************************************************************************
// MARK: Helpers
//******************************************************************************

class TClass
{
public:

    TClass(int value)
    : m_value(value)
    {}

    int
    GetValue()
    {
        return m_value;
    }

private:

    int m_value;
};

struct TStruct
{
    int m_value;
};

//******************************************************************************
// MARK: Tests
//******************************************************************************

TEST(Task, Construct_FunctionWithReturnValue_Success)
{
    try
    {
        CppTask::Task<int> task([](){
            return 1;
        });
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, Construct_FunctionWithoutReturnValue_Success)
{
    try
    {
        CppTask::Task<void> task([](){
            int x = 1;
        });
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, Copy_FunctionWithReturnValue_Success)
{
    try
    {
        CppTask::Task<int> task([](){
            return 1;
        });

        {
            CppTask::Task<int> secondTask = task;
        }
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, Copy_FunctionWithoutReturnValue_Success)
{
    try
    {
        CppTask::Task<void> task([](){
            int x = 1;
        });

        {
            CppTask::Task<void> secondTask = task;
        }
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, Destroy_RunningFunctionWithReturnValue_Success)
{
    try
    {
        CppTask::Task<int> task([](){
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            return 1;
        });

        {
            CppTask::Task<int> secondTask = task;
            secondTask.RunAsync();

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, Destroy_RunningFunctionWithoutReturnValue_Success)
{
    try
    {
        CppTask::Task<void> task([](){
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            int x = 1;
        });

        {
            CppTask::Task<void> secondTask = task;
            secondTask.RunAsync();

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, CompletedTask_TaskWithReturnValue_ReturnsCompletedTaskWithValue)
{
    try
    {
        auto pTask = CppTask::Task<int>::CompletedTask(32);

        ASSERT_NE(pTask, nullptr);
        ASSERT_EQ(pTask->GetState(), CppTask::TaskState::FINISHED);
        ASSERT_EQ(pTask->GetResult(), 32);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, CompletedTask_TaskWithoutReturnValue_ReturnsCompletedTask)
{
    try
    {
        auto pTask = CppTask::Task<void>::CompletedTask();

        ASSERT_NE(pTask, nullptr);
        ASSERT_EQ(pTask->GetState(), CppTask::TaskState::FINISHED);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, Run_FunctionWithReturnValue_SucceedsWithCorrectStates)
{
    try
    {
        CppTask::Task<int> task([](){
            return 1;
        });

        ASSERT_EQ(task.GetState(), CppTask::TaskState::WAITING);
        
        task.Run();

        ASSERT_EQ(task.GetState(), CppTask::TaskState::FINISHED);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, Run_FunctionWithoutReturnValue_SucceedsWithCorrectStates)
{
    try
    {
        CppTask::Task<void> task([](){
            int x = 1;
        });

        ASSERT_EQ(task.GetState(), CppTask::TaskState::WAITING);
        
        task.Run();

        ASSERT_EQ(task.GetState(), CppTask::TaskState::FINISHED);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, Run_RerunFunctionWithReturnValue_SucceedsAndReturnsInstantly)
{
    try
    {
        CppTask::Task<int> task([](){
            return 1;
        });

        task.Run();

        ASSERT_EQ(task.GetState(), CppTask::TaskState::FINISHED);
        ASSERT_ANY_THROW(task.Run());
        ASSERT_EQ(task.GetState(), CppTask::TaskState::FINISHED);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, Run_RerunFunctionWithoutReturnValue_SucceedsAndReturnsInstantly)
{
    try
    {
        CppTask::Task<void> task([](){
            int x = 1;
        });

        task.Run();

        ASSERT_EQ(task.GetState(), CppTask::TaskState::FINISHED);
        ASSERT_ANY_THROW(task.Run());
        ASSERT_EQ(task.GetState(), CppTask::TaskState::FINISHED);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, RunAsync_FunctionWithReturnValue_SucceedsWithCorrectStates)
{
    try
    {
        CppTask::Task<int> task([](){
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            return 1;
        });

        ASSERT_EQ(task.GetState(), CppTask::TaskState::WAITING);
        
        task.RunAsync();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        ASSERT_EQ(task.GetState(), CppTask::TaskState::RUNNING);
        
        task.AwaitResult();

        ASSERT_EQ(task.GetState(), CppTask::TaskState::FINISHED);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, RunAsync_FunctionWithoutReturnValue_SucceedsWithCorrectStates)
{
    try
    {
        CppTask::Task<void> task([](){
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        });

        ASSERT_EQ(task.GetState(), CppTask::TaskState::WAITING);
        
        task.RunAsync();

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        ASSERT_EQ(task.GetState(), CppTask::TaskState::RUNNING);

        task.Await();

        ASSERT_EQ(task.GetState(), CppTask::TaskState::FINISHED);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, RunAsync_RunFromTwoSeparateThreads_OnlyRunsTaskOnce)
{
    try
    {
        std::mutex mutex;
        std::condition_variable condition;
        std::atomic<size_t> runCount(0);

        CppTask::Task<void> task([&](){
            runCount += 1;
        });

        auto threadFunction = [&](){
            {
                std::unique_lock<std::mutex> uniqueLock(mutex);
                condition.wait(uniqueLock);
            }

            try
            {
                task.RunAsync();
            }
            catch (...)
            {
                // Might throw here if the task was prevented to enque directly
                // Otherwise the thread pool will catch the task and skip it
                //
                // Either way, exception or no exception is fine
            }
        };

        std::thread firstThread(threadFunction);
        std::thread secondThread(threadFunction);

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        {
            std::lock_guard<std::mutex> lockGuard(mutex);
            condition.notify_all();
        }
        
        if (firstThread.joinable())
        {
            firstThread.join();
        }

        if (secondThread.joinable())
        {
            secondThread.join();
        }

        task.Await();

        ASSERT_EQ(runCount, 1);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, RunAsync_RerunFunctionWithReturnValue_SucceedsWithCorrectStates)
{
    try
    {
        CppTask::Task<int> task([](){
            return 1;
        });

        task.Run();
        
        ASSERT_EQ(task.GetResult(), 1);
        ASSERT_EQ(task.GetState(), CppTask::TaskState::FINISHED);
        ASSERT_ANY_THROW(task.RunAsync());
        ASSERT_EQ(task.AwaitResult(), 1);
        ASSERT_EQ(task.GetState(), CppTask::TaskState::FINISHED);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, RunAsync_RerunFunctionWithoutReturnValue_SucceedsWithCorrectStates)
{
    try
    {
        CppTask::Task<void> task([](){
            int x = 1;
        });

        task.Run();

        ASSERT_EQ(task.GetState(), CppTask::TaskState::FINISHED);
        
        ASSERT_ANY_THROW(task.RunAsync());

        task.Await();

        ASSERT_EQ(task.GetState(), CppTask::TaskState::FINISHED);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, GetResult_Run_ReturnsLambdaValueResult)
{
    try
    {
        CppTask::Task<int> task([](){
            return 32;
        });

        task.Run();
        auto result = task.GetResult();

        ASSERT_EQ(result, 32);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, GetResult_RunAsync_ReturnsLambdaValueResult)
{
    try
    {
        CppTask::Task<int> task([](){
            return 32;
        });

        task.RunAsync();
        task.Await();
        auto result = task.GetResult();

        ASSERT_EQ(result, 32);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, GetResult_Run_ReturnsLambdaClassResult)
{
    try
    {
        CppTask::Task<TClass> task([](){
            return TClass(32);
        });

        task.Run();
        auto result = task.GetResult();

        ASSERT_EQ(result.GetValue(), 32);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, GetResult_RunAsync_ReturnsLambdaClassResult)
{
    try
    {
        CppTask::Task<TClass> task([](){
            return TClass(32);
        });

        task.RunAsync();
        task.Await();
        auto result = task.GetResult();

        ASSERT_EQ(result.GetValue(), 32);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, GetResult_Run_ReturnsLambdaStructResult)
{
    try
    {
        CppTask::Task<TStruct> task([](){
            return TStruct { 32 };
        });

        task.Run();
        auto result = task.GetResult();

        ASSERT_EQ(result.m_value, 32);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, GetResult_RunAsync_ReturnsLambdaStructResult)
{
    try
    {
        CppTask::Task<TStruct> task([](){
            return TStruct { 32 };
        });

        task.RunAsync();
        task.Await();
        auto result = task.GetResult();

        ASSERT_EQ(result.m_value, 32);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, GetResult_Run_ReturnsLambdaPointerResult)
{
    try
    {
        CppTask::Task<int*> task([](){
            return new int(32);
        });

        task.Run();
        auto pResult = task.GetResult();

        EXPECT_NE(pResult, nullptr);

        if (pResult)
        {
            EXPECT_EQ(*pResult, 32);
            delete pResult;
        }
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, GetResult_RunAsync_ReturnsLambdaPointerResult)
{
    try
    {
        CppTask::Task<int*> task([](){
            return new int(32);
        });

        task.RunAsync();
        task.Await();
        auto pResult = task.GetResult();

        EXPECT_NE(pResult, nullptr);

        if (pResult)
        {
            EXPECT_EQ(*pResult, 32);
            delete pResult;
        }
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, AwaitResult_RunAsync_ReturnsLambdaValueResult)
{
    try
    {
        CppTask::Task<int> task([](){
            return 32;
        });

        task.RunAsync();
        auto result = task.AwaitResult();

        ASSERT_EQ(result, 32);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, AwaitResult_Run_ReturnsLambdaValueResult)
{
    try
    {
        CppTask::Task<int> task([](){
            return 32;
        });

        task.Run();
        auto result = task.AwaitResult();

        ASSERT_EQ(result, 32);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, AwaitResult_RunAsync_ReturnsLambdaClassResult)
{
    try
    {
        CppTask::Task<TClass> task([](){
            return TClass(32);
        });

        task.RunAsync();
        auto result = task.AwaitResult();

        ASSERT_EQ(result.GetValue(), 32);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, AwaitResult_Run_ReturnsLambdaClassResult)
{
    try
    {
        CppTask::Task<TClass> task([](){
            return TClass(32);
        });

        task.Run();
        auto result = task.AwaitResult();

        ASSERT_EQ(result.GetValue(), 32);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, AwaitResult_RunAsync_ReturnsLambdaStructResult)
{
    try
    {
        CppTask::Task<TStruct> task([](){
            return TStruct { 32 };
        });

        task.RunAsync();
        auto result = task.AwaitResult();

        ASSERT_EQ(result.m_value, 32);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, AwaitResult_Run_ReturnsLambdaStructResult)
{
    try
    {
        CppTask::Task<TStruct> task([](){
            return TStruct { 32 };
        });

        task.Run();
        auto result = task.AwaitResult();

        ASSERT_EQ(result.m_value, 32);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, AwaitResult_RunAsync_ReturnsLambdaPointerResult)
{
    try
    {
        CppTask::Task<int*> task([](){
            return new int(32);
        });

        task.RunAsync();
        auto pResult = task.AwaitResult();

        EXPECT_NE(pResult, nullptr);

        if (pResult)
        {
            EXPECT_EQ(*pResult, 32);
            delete pResult;
        }
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, AwaitResult_Run_ReturnsLambdaPointerResult)
{
    try
    {
        CppTask::Task<int*> task([](){
            return new int(32);
        });

        task.Run();
        auto pResult = task.AwaitResult();

        EXPECT_NE(pResult, nullptr);

        if (pResult)
        {
            EXPECT_EQ(*pResult, 32);
            delete pResult;
        }
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, GetResult_CalledTwice_ReturnsSameLambdaValueResult)
{
    try
    {
        CppTask::Task<int> task([](){
            return 32;
        });

        task.RunAsync();
        task.Await();
        auto firstResult = task.GetResult();
        auto secondResult = task.GetResult();

        ASSERT_EQ(firstResult, 32);
        ASSERT_EQ(secondResult, 32);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, AwaitResult_CalledTwice_ReturnsSameLambdaValueResult)
{
    try
    {
        CppTask::Task<int> task([](){
            return 32;
        });

        task.RunAsync();
        auto firstResult = task.AwaitResult();
        auto secondResult = task.AwaitResult();

        ASSERT_EQ(firstResult, 32);
        ASSERT_EQ(secondResult, 32);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, GetResult_CalledTwice_ReturnsSameLambdaClassResult)
{
    try
    {
        CppTask::Task<TClass> task([](){
            return TClass(32);
        });

        task.RunAsync();
        task.Await();
        auto firstResult = task.GetResult();
        auto secondResult = task.GetResult();

        ASSERT_EQ(firstResult.GetValue(), 32);
        ASSERT_EQ(secondResult.GetValue(), 32);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, AwaitResult_CalledTwice_ReturnsSameLambdaClassResult)
{
    try
    {
        CppTask::Task<TClass> task([](){
            return TClass(32);
        });

        task.RunAsync();
        auto firstResult = task.AwaitResult();
        auto secondResult = task.AwaitResult();

        ASSERT_EQ(firstResult.GetValue(), 32);
        ASSERT_EQ(secondResult.GetValue(), 32);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, GetResult_CalledTwice_ReturnsSameLambdaStructResult)
{
    try
    {
        CppTask::Task<TStruct> task([](){
            return TStruct { 32 };
        });

        task.RunAsync();
        task.Await();
        auto firstResult = task.GetResult();
        auto secondResult = task.GetResult();

        ASSERT_EQ(firstResult.m_value, 32);
        ASSERT_EQ(secondResult.m_value, 32);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, AwaitResult_CalledTwice_ReturnsSameLambdaStructResult)
{
    try
    {
        CppTask::Task<TStruct> task([](){
            return TStruct { 32 };
        });

        task.RunAsync();
        auto firstResult = task.AwaitResult();
        auto secondResult = task.AwaitResult();

        ASSERT_EQ(firstResult.m_value, 32);
        ASSERT_EQ(secondResult.m_value, 32);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, GetResult_CalledTwice_ReturnsSameLambdaPointerResult)
{
    try
    {
        CppTask::Task<int*> task([](){
            return new int(32);
        });

        task.RunAsync();
        task.Await();
        auto pFirstResult = task.GetResult();
        auto pSecondResult = task.GetResult();

        EXPECT_NE(pFirstResult, nullptr);
        EXPECT_NE(pSecondResult, nullptr);

        EXPECT_EQ(pFirstResult, pSecondResult);

        if (pFirstResult)
        {
            EXPECT_EQ(*pFirstResult, 32);
        }

        if (pSecondResult)
        {
            EXPECT_EQ(*pSecondResult, 32);
        }

        delete pFirstResult;
        // pFirstResult is pSecondResult!
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, AwaitResult_CalledTwice_ReturnsSameLambdaPointerResult)
{
    try
    {
        CppTask::Task<int*> task([](){
            return new int(32);
        });

        task.RunAsync();
        auto pFirstResult = task.AwaitResult();
        auto pSecondResult = task.AwaitResult();

        EXPECT_NE(pFirstResult, nullptr);
        EXPECT_NE(pSecondResult, nullptr);

        EXPECT_EQ(pFirstResult, pSecondResult);

        if (pFirstResult)
        {
            EXPECT_EQ(*pFirstResult, 32);
        }

        if (pSecondResult)
        {
            EXPECT_EQ(*pSecondResult, 32);
        }

        delete pFirstResult;
        // pFirstResult is pSecondResult!
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, GetResult_NotRunFunctionWithResult_Throws)
{
    // There is no reason to test the behaviour of void lambdas,
    // since those will not attempt to read the task thread result

    try
    {
        CppTask::Task<int> task([](){
            return 1;
        });

        ASSERT_ANY_THROW(task.GetResult());
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, Await_IsConstTask_Callable)
{
    try
    {
        CppTask::Task<void> task([](){
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        });

        auto useConstTask = [](const CppTask::Task<void>& c_rTask){
            c_rTask.Await();
        };

        task.RunAsync();
        useConstTask(task);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, AwaitResult_IsConstTask_Callable)
{
    try
    {
        CppTask::Task<int> task([](){
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            return 1;
        });

        int result = -1;

        auto useConstTask = [&result](const CppTask::Task<int>& c_rTask){
            result = c_rTask.AwaitResult();
        };

        task.RunAsync();
        useConstTask(task);

        ASSERT_EQ(result, 1);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, GetResult_IsConstTask_Callable)
{
    try
    {
        CppTask::Task<int> task([](){
            return 1;
        });

        int result = -1;

        auto useConstTask = [&result](const CppTask::Task<int>& c_rTask){
            result = c_rTask.GetResult();
        };

        task.Run();
        useConstTask(task);

        ASSERT_EQ(result, 1);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

TEST(Task, GetState_IsConstTask_Callable)
{
    try
    {
        CppTask::Task<int> task([](){
            return 1;
        });

        auto initialState = CppTask::TaskState::RUNNING;
        auto finalState = CppTask::TaskState::RUNNING;

        auto useConstTask = [](const CppTask::Task<int>& c_rTask, CppTask::TaskState& rState){
            rState = c_rTask.GetState();
        };

        useConstTask(task, initialState);
        task.Run();
        useConstTask(task, finalState);

        ASSERT_EQ(initialState, CppTask::TaskState::WAITING);
        ASSERT_EQ(finalState, CppTask::TaskState::FINISHED);
    }
    catch (const std::exception& e)
    {
        FAIL() << e.what();
    }
}

//******************************************************************************
// MARK: Main
//******************************************************************************

int 
main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

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

#ifndef libcpptask_CppTask_Exception_h
#define libcpptask_CppTask_Exception_h

// STL
#include <string>
#include <exception>

// External

// Project


// Namespace
namespace CppTask {

/**
 *  @brief The exception class is used by the library task content to throw
 *         identifyable exceptions.
 */
class Exception : public std::exception
{
public:
    
    //**************************************************************************
    // Constructor
    //**************************************************************************
    
    /**
     *  @brief std::string constructor.
     *
     *  @param c_rMessage The error message.
     */
    Exception(const std::string& c_rMessage) 
    : m_message(c_rMessage)
    {}
    
    /**
     *  @brief const char* constructor.
     *
     *  @param c_pMessage The error message.
     */
    Exception(const char* c_pMessage) 
    : m_message(c_pMessage != nullptr ? c_pMessage : "")
    {}
    
    //**************************************************************************
    // Getters
    //**************************************************************************
    
    /**
     *  @brief Get the const char* exception string.
     *
     *  @returns A const char* string with the error message.
     */
    inline const char* 
    what() const noexcept
    {
        return m_message.c_str();
    }
    
    /**
     *  @brief Get the std::string exception string.
     *
     *  @returns A std::string string with the error message.
     */
    inline std::string 
    what2() const noexcept
    {
        return m_message;
    }
    
private:
    
    //**************************************************************************
    // Variables
    //**************************************************************************
    
    std::string m_message;
};

// Namespace
}

#endif /* libcpptask_CppTask_Exception_h */

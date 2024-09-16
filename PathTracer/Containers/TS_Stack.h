#ifndef __TS_STACK_H
#define __TS_STACK_H

#include <stack>
#include <mutex>
#include <condition_variable>
#include <utility> 
/**
 * @brief Thread-Safe Stack Container
*/
namespace EDX {
    template <typename T> 
    class TS_Stack {
    public: 

        void Wait_And_Push(T& element) {
            m_Lock.lock(); 

            m_Stack.push(std::move(element)); 

            m_Lock.unlock(); 
        }

        bool Try_Pop(T& element) {
            if (m_Lock.try_lock()) {

                element = m_Stack.top(); 
                m_Stack.pop(); 

                m_Lock.unlock(); 
                return true; 
            }
            else {
                return false; 
            }

        }
        T& Wait_And_Pop() {
            m_Lock.lock(); 

            T elem = m_Stack.top(); 
            m_Stack.pop(); 

            m_Lock.unlock(); 

            return elem; 
        }

        uint64_t Size() const {
            uint64_t size = 0u; 
            
            m_Lock.lock(); 
            size = m_Stack.size(); 
            m_Lock.unlock(); 

            return size;
        }

        bool Empty() const {
            return (Size() == 0u); 
        }


    private:
        std::stack<T> m_Stack;
        mutable std::mutex m_Lock; 
    };
}
#endif
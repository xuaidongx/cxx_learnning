#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include <string_view>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <atomic>
#include <iostream>
#include <thread>
#include <format>

namespace log
{
    class logger
    {
        public:
            logger():m_stop{false}
            {
                m_thread = std::thread{&logger::processEntries, this};
            };
            ~logger()
            {
                m_stop = true;
                m_cv.notify_all();
                m_thread.join();
            };

            logger(const logger&)=delete;
            logger& operator=(const logger&)=delete;

            void log(std::string_view sv)
            {
                std::unique_lock lk{m_queue_mutex};
                m_queue_message.push({sv.data(), sv.size()});
                m_cv.notify_all();
            }

            void processEntries()
            {
                std::ofstream out{"./log.txt"};
                if (out.fail())
                {
                    std::cout<<"open file log.txt failed"<<std::endl;
                    return;
                }

                std::unique_lock lk{m_queue_mutex, std::defer_lock};
                while(true)
                {
                    lk.lock();
                    m_cv.wait(lk);
                    if (!m_queue_message.empty())
                    {
                        std::queue<std::string> local_message;
                        local_message.swap(m_queue_message);
                        lk.unlock();

                        while (!local_message.empty())
                        {
                            out << local_message.front() << std::endl;
                            local_message.pop();
                        }
                    }
                    
                    if (m_stop)
                    {
                        lk.lock();
                        while (!m_queue_message.empty())
                        {
                            out << m_queue_message.front() << std::endl;
                            m_queue_message.pop();
                        }
                        break;
                    }
                }
            }
        private:
            std::queue<std::string> m_queue_message;
            std::mutex m_queue_mutex;
            std::condition_variable m_cv;
            std::atomic<bool> m_stop;
            std::thread m_thread;
    };
    

    static void testing()
    {
        logger local_logger;

        auto generator_10_message{[](auto index, logger& log)
        {
            for(auto i{0}; i < 10; ++i)
            {
                log.log(std::format("log entry {} from thread {}", i, index));
            }
        }};

        std::vector<std::thread> threads;
        for(auto num{0}; num < 10; ++num)
            threads.emplace_back(generator_10_message, num, std::ref(local_logger));
        std::for_each(std::begin(threads), std::end(threads), [](auto& t){t.join();});
    
    }
} // namespace log

#endif //__LOGGER_HPP__

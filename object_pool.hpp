#ifndef __OBJECT_POOL_HPP__
#define __OBJECT_POOL_HPP__

#include <iostream>
#include <vector>
#include <numeric>
#include <memory>
#include <assert.h>
#include <array>


namespace object_pool
{
template<typename T, typename Allocator = std::allocator<T>>
class ObjectPool
{
    public:
        ObjectPool() = default;
        explicit ObjectPool(const Allocator& allocator):m_allocator(allocator){}
        virtual ~ObjectPool()
        {
            assert(m_freeObjects.size() == m_initChunkSize * (std::pow(2, m_pool.size()) - 1));

            auto chunksize{m_initChunkSize};
            for(auto& chunk : m_pool)
            {
                m_allocator.deallocate(chunk, chunksize);
                chunksize *= 2;
            }
        }

        ObjectPool(const ObjectPool&) = default;
        ObjectPool& operator=(const ObjectPool&) = default;

        ObjectPool(ObjectPool&&) = delete;
        ObjectPool& operator=(ObjectPool&&) = delete;

        template<typename... Args>
        std::shared_ptr<T> acquireObject(Args... args)
        {
            if (m_freeObjects.empty())
                addChunk();
            
            auto* freeobject = m_freeObjects.back();

            new(freeobject) T {std::forward<Args>(args)...};
            m_freeObjects.pop_back();

            return std::shared_ptr<T>{freeobject, [this](T* object)
            {
                std::destroy_at(object);
                m_freeObjects.push_back(object);
            }};
        }
    private:
        void addChunk()
        {
            auto* firstObjectPointer = m_allocator.allocate(m_newChunkSize);
            m_pool.push_back(firstObjectPointer);

            auto oldfreeobjectsize = m_freeObjects.size();
            m_freeObjects.resize(oldfreeobjectsize + m_newChunkSize);
            std::iota(m_freeObjects.begin() + oldfreeobjectsize, m_freeObjects.end(), firstObjectPointer);

            m_newChunkSize *= 2;
        }
        std::vector<T*> m_pool;
        std::vector<T*> m_freeObjects;
        static const std::size_t m_initChunkSize{5};
        std::size_t m_newChunkSize{m_initChunkSize};
        Allocator m_allocator;
};

void testing()
{
    class ExpensiveObject
    {
        public:
            ExpensiveObject(){};
            virtual ~ExpensiveObject(){};

        private:
            std::array<double, 4*1024*1024> m_data;
    };

    auto processExpensiveObject = [](ExpensiveObject* p){};

    std::cout<<"objectpool testing"<<std::endl;
    using eo_pool = ObjectPool<ExpensiveObject>;

    const std::size_t number_iterator{5000'000};
    eo_pool eopool;
    {
        auto before = std::chrono::system_clock::now();
        for(auto i{0}; i < number_iterator; ++i)
        {
            auto object = eopool.acquireObject();
            processExpensiveObject(object.get());
        }
        std::cout<<"object pool process 5000'000 spend time: " 
        << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - before).count()<<" s"<<std::endl;
    }

    {
        auto before = std::chrono::system_clock::now();
        for(auto i{0}; i < number_iterator; ++i)
        {
            auto object = new ExpensiveObject();
            processExpensiveObject(object);
            delete object;
        }
        std::cout<<"using new/delete process 5000'000 spend time: " 
        << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - before).count()<<" s"<<std::endl;
    }

}
}


#endif//__OBJECT_POOL_HPP__
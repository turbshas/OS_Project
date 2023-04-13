#ifndef _STATIC_CIRCULAR_BUFFER
#define _STATIC_CIRCULAR_BUFFER

#include <array>
#include <cstdio>
#include <cstdlib>

namespace os::utils::static_buffer
{
    namespace details
    {
        template <typename T>
        class StaticCircularBufferBase
        {
            protected:
                typedef typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type StorageType;

            private:
                // std::array<StorageType, TSize> _data;
                StorageType* _data;
                std::size_t _beginIndex;
                std::size_t _endIndex;
                const std::size_t _capacity;
                std::size_t _count;

                template <typename U>
                void StoreValue(const std::size_t index, U&& value)
                {
                    const StorageType* dest = _data + index;
                    new (dest) T(std::forward<U>(value));
                }

                std::size_t IncreaseIndex(const std::size_t from)
                {
                    return (from == (_capacity - 1)) ? 0 : (from + 1);
                }

                std::size_t DecreaseIndex(const std::size_t from)
                {
                    return (from == 0) ? (_capacity - 1) : (from - 1);
                }

            protected:
                StaticCircularBufferBase(StorageType* data, std::size_t capacity)
                    : _data(data),
                      _beginIndex(0),
                      _endIndex(0),
                      _capacity(capacity),
                      _count(0)
                {
                }
                StaticCircularBufferBase(const StaticCircularBufferBase&) = delete;
                StaticCircularBufferBase(StaticCircularBufferBase&&) = delete;
                ~StaticCircularBufferBase()
                {
                    // Storage is compile-time allocated so nothing to do.
                }
                StaticCircularBufferBase& operator=(const StaticCircularBufferBase&) = delete;
                StaticCircularBufferBase& operator=(StaticCircularBufferBase&&) = delete;

                const T* operator[](const std::size_t index) const
                {
                    if (index >= Length())
                    {
                        return nullptr;
                    }
                    const std::size_t adjustedIndex = _beginIndex + index;
                    return _data + adjustedIndex;
                }

                std::size_t Capacity() const { return _capacity; }
                bool IsEmpty() const { return _count == 0; }
                bool IsFull() const { return _count == _capacity; }
                std::size_t Length() const { return _count; }

                template <typename U>
                bool PushFront(U&& value)
                {
                    if (IsFull()) return false;

                    const std::size_t destIndex = DecreaseIndex(_beginIndex);
                    StoreValue(destIndex, std::forward<U>(value));
                    _beginIndex = destIndex;
                    _count++;
                    return true;
                }

                template <typename U>
                bool PushBack(U&& value)
                {
                    if (IsFull()) return false;

                    const std::size_t destIndex = IncreaseIndex(_endIndex);
                    StoreValue(destIndex, std::forward<U>(value));
                    _endIndex = destIndex;
                    _count++;
                    return true;
                }

                const T& PopFront()
                {
                    if (IsEmpty()) return nullptr;

                    // Get the value and call the desctructor on the stored item.
                    const T* const valuePointer = _data + _beginIndex;
                    const T value = *valuePointer;
                    valuePointer->~T();

                    // Update indices.
                    const std::size_t newIndex = IncreaseIndex(_beginIndex);
                    _beginIndex = newIndex;
                    _count--;
                    return value;
                }

                const T& PopBack()
                {
                    if (IsEmpty()) return nullptr;

                    // Get the value and call the desctructor on the stored item.
                    const T* const valuePointer = _data + _beginIndex;
                    const T value = *valuePointer;
                    valuePointer->~T();

                    // Update indices.
                    const std::size_t newIndex = DecreaseIndex(_endIndex);
                    _endIndex = newIndex;
                    _count--;
                    return value;
                }

                const T* PeekFront() const
                {
                    if (IsEmpty()) return nullptr;
                    return _data + _beginIndex;
                }

                const T* PeekBack() const
                {
                    if (IsEmpty()) return nullptr;
                    return _data + _endIndex;
                }
        };
    }

    template <typename T, std::size_t TSize>
    class StaticCircularBuffer : public details::StaticCircularBufferBase<T>
    {
        private:
            typedef details::StaticCircularBufferBase<T> Base;
            typedef typename Base::StorageType StorageType;
            std::array<StorageType, TSize> _data;

        public:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
            // _data shouldn't be initialized in member list - this causes it to zero-initialize.
            // That introduces a use of memset to zero the entire array, which is unnecessary as
            // the usage of the array is already tracked by the base class.
            StaticCircularBuffer()
                : Base(&_data[0], TSize)
            {
            }
#pragma GCC diagnostic pop

            template <typename U>
            bool PushFront(U&& value)
            {
                return Base::PushFront(std::forward<U>(value));
            }

            template <typename U>
            bool PushBack(U&& value)
            {
                return Base::PushBack(std::forward<U>(value));
            }

            const T* PopFront()
            {
                return Base::PopFront();
            }

            const T* PopBack()
            {
                return Base::PopBack();
            }

            const T* PeekFront()
            {
                return Base::PeekFront();
            }

            const T* PeekBack()
            {
                return Base::PeekBack();
            }

            std::size_t Capacity() const { return Base::Capacity(); }
            bool IsEmpty() const { return Base::IsEmpty(); }
            bool IsFull() const { return Base::IsFull(); }
            std::size_t Length() const { return Base::Length(); }
    };
}

#endif // _STATIC_CIRCULAR_BUFFER

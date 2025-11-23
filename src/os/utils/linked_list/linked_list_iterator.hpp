#ifndef _LINKED_LIST__ITERATOR
#define _LINKED_LIST__ITERATOR

#include "list_item.hpp"

namespace os::utils::linked_list
{
    template <class T>
    class Iterator
    {
        private:
            const details::empty_list_item* _list_sentinel;
            details::empty_list_item* _current_item;

        public:
            Iterator()
                : _list_sentinel(nullptr),
                  _current_item(nullptr)
            {
            }

            Iterator(const details::empty_list_item& sentinel, details::empty_list_item& start)
                : _list_sentinel(&sentinel),
                  _current_item(&start)
            {
            }

            Iterator(const Iterator& other)
                : _list_sentinel(other._list_sentinel),
                  _current_item(other._current_item)
            {
            }

            Iterator(Iterator&& other)
                : _list_sentinel(other._list_sentinel),
                  _current_item(other._current_item)
            {
                *this = other;
            }

            ~Iterator()
            {
                _list_sentinel = nullptr;
                _current_item = nullptr;
            }

            Iterator& operator=(const Iterator& other)
            {
                if (this == &other) return *this;
                _current_item = other._current_item;
                return *this;
            }

            Iterator& operator=(Iterator&& other)
            {
                if (this == &other) return *this;
                _current_item = other._current_item;
                return *this;
            }

            bool operator==(const Iterator& other)
            {
                if (_list_sentinel != other._list_sentinel) return false;
                if ((_current_item == nullptr) || (other._current_item == nullptr)) return _current_item == other._current_item;
                return (_current_item->prev == other._current_item->prev) && (_current_item->next == other._current_item->next);
            }

            bool operator!=(const Iterator& other)
            {
                return !(*this == other);
            }

            T& operator*()
            {
                return *currentItem();
            }

            const Iterator& operator++() // Pre-increment
            {
                _current_item = _current_item->next;
                return *this;
            }

            Iterator operator++(int) // Post-increment
            {
                Iterator current = *this;
                ++(*this);
                return current;
            }

            const Iterator& operator--() // Pre-increment
            {
                _current_item = _current_item->prev;
                return *this;
            }

            Iterator operator--(int) // Post-increment
            {
                Iterator current = *this;
                --(*this);
                return current;
            }

            T* currentItem() const
            {
                if (_current_item == _list_sentinel) return nullptr;
                details::list_item<T>* currentItem = reinterpret_cast<details::list_item<T>*>(_current_item);
                return &currentItem->item;
            }

            bool atBegin() const
            {
                return (_list_sentinel->next != _list_sentinel) && (_current_item == _list_sentinel->next);
            }

            bool atEnd() const
            {
                return _current_item == _list_sentinel;
            }

            void moveNext()
            {
                _current_item = _current_item->next;
            }

            void removeCurrent()
            {
                if (_current_item == _list_sentinel) return;

                const details::list_item<T>* const next_item = _current_item->next;
                delete _current_item;
                _current_item = next_item;
            }

            void insertBeforeCurrent(const T& item)
            {
                new details::list_item<T>(_current_item->prev, _current_item, item);
            }

            void insertAfterCurrent(const T& item)
            {
                new details::list_item<T>(_current_item, _current_item->next, item);
            }
    };
}

#endif /* _LINKED_LIST__ITERATOR */

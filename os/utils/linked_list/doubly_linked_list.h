#ifndef _DOUBLY_LINKED_LIST_H
#define _DOUBLY_LINKED_LIST_H

#include "linked_list_iterator.hpp"
#include "list_item.hpp"
#include <cstdio>

namespace os::utils::linked_list
{
    /// @brief Circular Doubly-linked List
    /// @tparam T Stored Data Type
    template <class T>
    class DoublyLinkedList
    {
        private:
            size_t _num_items;
            details::empty_list_item _sentinel;

        public:
            DoublyLinkedList()
                : _num_items(0),
                  _sentinel()
            {
                _sentinel.next = &_sentinel;
                _sentinel.prev = &_sentinel;
            }

            DoublyLinkedList(const DoublyLinkedList& other)
                : _num_items(0),
                  _sentinel()
            {
                for (const auto& item : other)
                {
                    pushFront(item);
                }
            }

            DoublyLinkedList(DoublyLinkedList&& other)
                : _num_items(0),
                  _sentinel()
            {
                // Transfer member data.
                _sentinel.next = other._sentinel.next;
                _sentinel.prev = other._sentinel.prev;
                _num_items = other._num_items;

                // Clear other.
                other._sentinel.next = &other._sentinel;
                other._sentinel.prev = &other._sentinel;
                other._num_items = 0;
            }

            ~DoublyLinkedList()
            {
                clear();
            }

            DoublyLinkedList& operator=(const DoublyLinkedList& other)
            {
                if (this == &other) return *this;

                // Clear current list to not leak memory.
                clear();

                for (const auto& item : other)
                {
                    pushBack(item);
                }

                return *this;
            }

            DoublyLinkedList& operator=(DoublyLinkedList&& other)
            {
                if (this == &other) return *this;

                // Clear current list to not leak memory.
                clear();

                // Transfer member data.
                _sentinel.next = other._sentinel.next;
                _sentinel.prev = other._sentinel.prev;
                _num_items = other._num_items;

                // Clear other.
                other._sentinel.next = &other._sentinel;
                other._sentinel.prev = &other._sentinel;
                other._num_items = 0;

                return *this;
            }

            void pushFront(const T& item)
            {
                new details::list_item<T>(&_sentinel, _sentinel.next, item);
                _num_items++;
            }

            void pushBack(const T& item)
            {
                new details::list_item<T>(_sentinel.prev, &_sentinel, item);
                _num_items++;
            }

            T popFront()
            {
                if (empty()) return T{};

                details::list_item<T>* li = reinterpret_cast<details::list_item<T>*>(_sentinel.next);
                T item = li->item;
                delete li;

                _num_items--;
                return item;
            }

            T popBack()
            {
                if (empty()) return T{};

                details::list_item<T>* li = reinterpret_cast<details::list_item<T>*>(_sentinel.next);
                T item = li->item;
                delete li;

                _num_items--;
                return item;
            }

            size_t size() const { return _num_items; };
            bool empty() const { return _num_items == 0; };
            void clear()
            {
                while (_sentinel.next != &_sentinel)
                {
                    delete _sentinel.next;
                }
            }

            T removeItem(const size_t index)
            {
                if (index >= _num_items) return T{};

                details::list_item<T>* li = &_sentinel;
                for (size_t i = 0; i < index; i++)
                {
                    li = li->next;
                    // If we hit the end of the list, return default.
                    if (li == &_sentinel) return T{};
                }

                T item = li->item;
                delete li;
                return item;
            }

            Iterator<T> begin() const
            {
                return Iterator<T>{_sentinel, *(_sentinel.next)};
            }

            Iterator<T> end() const
            {
                return Iterator<T>{_sentinel, *(_sentinel.prev)};
            }

            T& operator[](const size_t index) const
            {
                if (index >= _num_items) return T{};

                details::list_item<T>* li = &_sentinel;
                for (size_t i = 0; i < index; i++)
                {
                    li = li->next;
                    // If we hit the end of the list, return default.
                    if (li == &_sentinel) return T{};
                }
                return li->item;
            }
    };
}

#endif /* _DOUBLY_LINKED_LIST_H */

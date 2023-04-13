#ifndef _DOUBLY_LINKED_LIST_H
#define _DOUBLY_LINKED_LIST_H

#include "list_item.hpp"
#include <cstdio>

// TODO: fix copy constructor - need to iterate over list properly
// TODO: fix iterator - should implement actual C++ iterator to enable iterative loops https://cplusplus.com/reference/iterator/

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
            class Iterator
            {
                private:
                    details::list_item<T>* _current_item;

                public:
                    Iterator();
                    Iterator(const details::list_item<T>& start);
                    ~Iterator();
                    Iterator& operator=(const Iterator&);
                    bool operator==(const Iterator&);
                    bool operator!=(const Iterator&);
                    const Iterator& operator++(); // Pre-increment
                    Iterator operator++(int);     // Post-increment
                    const Iterator& operator--(); // Pre-increment
                    Iterator operator--(int);     // Post-increment

                    void moveNext();
                    T& currentItem() const;
                    T removeCurrent();
                    void insertBeforeCurrent(const T& item);
                    void insertAfterCurrent(const T& item);
            };

            DoublyLinkedList();
            DoublyLinkedList(const DoublyLinkedList&);
            DoublyLinkedList(DoublyLinkedList&&);
            ~DoublyLinkedList();
            DoublyLinkedList& operator=(const DoublyLinkedList&) = delete;
            DoublyLinkedList& operator=(DoublyLinkedList&&) = delete;

            void pushFront(const T& item);
            void pushBack(const T& item);
            T popFront();
            T popBack();

            size_t size() const { return _num_items; };
            bool empty() const { return _num_items == 0; };
            void clear();
            T removeItem(const size_t index);

            Iterator begin() const;
            Iterator end() const;
            T& operator[](const size_t index) const;
    };

    /*
     * Iterator definitions
     */
    template <class T>
    DoublyLinkedList<T>::Iterator::Iterator()
    {
        _current_item = &_sentinel;
    }

    template <class T>
    DoublyLinkedList<T>::Iterator::Iterator(const details::list_item<T>& start)
    {
        _current_item = &start;
    }

    template <class T>
    DoublyLinkedList<T>::Iterator::~Iterator()
    {
    }

    template <class T>
    DoublyLinkedList<T>::Iterator&
    DoublyLinkedList<T>::Iterator::operator=(const DoublyLinkedList<T>::Iterator& other)
    {
        _current_item = other._current_item;
    }

    template <class T>
    bool
    DoublyLinkedList<T>::Iterator::operator==(const DoublyLinkedList<T>::Iterator& other)
    {
        if ((_current_item == nullptr) || (other._current_item == nullptr)) return _current_item == other._current_item;
        return (_current_item->prev == other._current_item->prev) && (_current_item->next == other._current_item->next);
    }

    template <class T>
    bool
    DoublyLinkedList<T>::Iterator::operator!=(const Iterator& other)
    {
        return !(*this == other);
    }

    template <class T>
    const DoublyLinkedList<T>::Iterator&
    DoublyLinkedList<T>::Iterator::operator++()
    {
        _current_item = _current_item->next;
        return *this;
    }

    template <class T>
    DoublyLinkedList<T>::Iterator
    DoublyLinkedList<T>::Iterator::operator++(int _ignore)
    {
        Iterator current = *this;
        ++(*this);
        return current;
    }

    template <class T>
    const DoublyLinkedList<T>::Iterator&
    DoublyLinkedList<T>::Iterator::operator--()
    {
        _current_item = _current_item->prev;
        return *this;
    }

    template <class T>
    DoublyLinkedList<T>::Iterator
    DoublyLinkedList<T>::Iterator::operator--(int _ignore)
    {
        Iterator current = *this;
        --(*this);
        return current;
    }

    template <class T>
    void
    DoublyLinkedList<T>::Iterator::moveNext()
    {
        _current_item = _current_item->next;
    }

    template <class T>
    T&
    DoublyLinkedList<T>::Iterator::currentItem() const
    {
        return _current_item->item;
    }

    template <class T>
    T
    DoublyLinkedList<T>::Iterator::removeCurrent()
    {
        const details::list_item<T>* const next_item = _current_item->next;
        const T item = _current_item->item;

        delete _current_item;

        _current_item = next_item;
        return item;
    }

    template <class T>
    void
    DoublyLinkedList<T>::Iterator::insertBeforeCurrent(const T& item)
    {
        new details::list_item<T>(_current_item->prev, _current_item, item);
    }

    template <class T>
    void
    DoublyLinkedList<T>::Iterator::insertAfterCurrent(const T& item)
    {
        new details::list_item<T>(_current_item, _current_item->next, item);
    }

    /*
     * Linked list definitions
     */
    template <class T>
    DoublyLinkedList<T>::DoublyLinkedList()
        : _num_items(0),
          _sentinel()
    {
        _sentinel.next = &_sentinel;
        _sentinel.prev = &_sentinel;
    }

    template <class T>
    DoublyLinkedList<T>::~DoublyLinkedList()
    {
        clear();
    }

    template <class T>
    DoublyLinkedList<T>::DoublyLinkedList(const DoublyLinkedList& other)
    {
        for (auto item : other)
        {
            pushFront(item);
        }
    }

    template <class T>
    DoublyLinkedList<T>::DoublyLinkedList(DoublyLinkedList&& other)
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

    template <class T>
    void
    DoublyLinkedList<T>::pushFront(const T& item)
    {
        new details::list_item<T>(&_sentinel, _sentinel.next, item);
        _num_items++;
    }

    template <class T>
    void
    DoublyLinkedList<T>::pushBack(const T& item)
    {
        new details::list_item<T>(_sentinel.prev, &_sentinel, item);
        _num_items++;
    }

    template <class T>
    T
    DoublyLinkedList<T>::popFront()
    {
        details::list_item<T>* li = reinterpret_cast<details::list_item<T>*>(_sentinel.next);
        T item = li->item;
        delete li;

        _num_items--;
        return item;
    }

    template <class T>
    T
    DoublyLinkedList<T>::popBack()
    {
        details::list_item<T>* li = reinterpret_cast<details::list_item<T>*>(_sentinel.next);
        T item = li->item;
        delete li;

        _num_items--;
        return item;
    }

    template <class T>
    void
    DoublyLinkedList<T>::clear()
    {
        while (_sentinel.next != &_sentinel)
        {
            delete _sentinel.next;
        }
    }

    template <class T>
    T
    DoublyLinkedList<T>::removeItem(const size_t index)
    {
        details::list_item<T>* li = &_sentinel;
        for (size_t i = 0; i < index; i++)
        {
            li = li->next;
        }

        T item = li->item;
        delete li;
        return item;
    }

    template <class T>
    typename DoublyLinkedList<T>::Iterator
    DoublyLinkedList<T>::begin() const
    {
        Iterator iter(_sentinel);
        return iter;
    }

    template <class T>
    typename DoublyLinkedList<T>::Iterator
    DoublyLinkedList<T>::end() const
    {
        Iterator iter(_sentinel);
        return iter;
    }

    template <class T>
    T&
    DoublyLinkedList<T>::operator[](const size_t index) const
    {
        details::list_item<T>* li = &_sentinel;
        for (size_t i = 0; i < index; i++)
        {
            li = li->next;
        }
        return li->item;
    }
}

#endif /* _DOUBLY_LINKED_LIST_H */

#ifndef _DOUBLY_LINKED_LIST_H
#define _DOUBLY_LINKED_LIST_H

#include <cstdio>

// TODO: fix copy constructor - need to iterate over list properly
// TODO: fix iterator - should implement actual C++ iterator to enable iterative loops https://cplusplus.com/reference/iterator/

/// @brief Circular Doubly-linked List
/// @tparam T Stored Data Type
template <class T>
class DoublyLinkedList {
    struct list_item;

    public:
        class Iterator {
            public:
                Iterator(const list_item& start);

                void moveNext();
                T& currentItem() const;
                T removeCurrent();
                void insertBeforeCurrent(const T& item);
                void insertAfterCurrent(const T& item);

            private:
                list_item *_current_item;
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

        Iterator getIter() const;
        T& operator[](const size_t index) const;

    private:
        struct list_item {
            list_item *prev;
            list_item *next;
            T item;

            list_item();
            list_item(const list_item&) = delete;
            list_item(list_item&&) = delete;
            list_item(list_item *const prev, list_item *const next, const T& item);
            ~list_item();
            list_item& operator=(const list_item&) = delete;
            list_item& operator=(list_item&&) = delete;
        };

        size_t _num_items;
        list_item _sentinel;
};

/*
 * Iterator definitions
 */
template <class T>
DoublyLinkedList<T>::Iterator::Iterator(const list_item& start)
{
    _current_item = &start;
}

template <class T>
void DoublyLinkedList<T>::Iterator::moveNext()
{
    _current_item = _current_item->next;
}

template <class T>
T& DoublyLinkedList<T>::Iterator::currentItem() const
{
    return _current_item->item;
}

template <class T>
T DoublyLinkedList<T>::Iterator::removeCurrent()
{
    const list_item *const next_item = _current_item->next;
    const T item = _current_item->item;

    delete _current_item;

    _current_item = next_item;
    return item;
}

template <class T>
void DoublyLinkedList<T>::Iterator::insertBeforeCurrent(const T& item)
{
    new list_item(_current_item->prev, _current_item, item);
}

template <class T>
void DoublyLinkedList<T>::Iterator::insertAfterCurrent(const T& item)
{
    new list_item(_current_item, _current_item->next, item);
}

/*
 * list_item definitions
 */
template <class T>
DoublyLinkedList<T>::list_item::list_item()
    : prev(nullptr),
    next(nullptr),
    item()
{
}

template <class T>
DoublyLinkedList<T>::list_item::list_item(list_item *const prev_item, list_item *const next_item, const T& item_val)
    : prev(prev_item),
    next(next_item),
    item(item_val)
{
    prev->next = this;
    next->prev = this;
}

template <class T>
DoublyLinkedList<T>::list_item::~list_item()
{
    prev->next = next;
    next->prev = prev;
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
    for (auto item : other) {
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
void DoublyLinkedList<T>::pushFront(const T& item)
{
    new list_item(&_sentinel, _sentinel.next, item);
    _num_items++;
}

template <class T>
void DoublyLinkedList<T>::pushBack(const T& item)
{
    new list_item(_sentinel.prev, &_sentinel, item);
    _num_items++;
}

template <class T>
T DoublyLinkedList<T>::popFront()
{
    list_item *li = _sentinel.next;
    const T item = li->item;
    delete li;

    _num_items--;
    return item;
}

template <class T>
T DoublyLinkedList<T>::popBack()
{
    list_item *li = _sentinel.prev;
    const T item = li->item;
    delete li;

    _num_items--;
    return item;
}

template <class T>
void DoublyLinkedList<T>::clear()
{
    for (size_t i = 0; i < _num_items; i++) {
        delete _sentinel.next;
    }
}

template <class T>
T DoublyLinkedList<T>::removeItem(const size_t index)
{
    list_item *li = &_sentinel;
    for (size_t i = 0; i < index; i++) {
        li = li->next;
    }

    const T item = li->item;
    delete li;
    return item;
}

template <class T>
typename DoublyLinkedList<T>::Iterator DoublyLinkedList<T>::getIter() const
{
    Iterator iter(_sentinel);
    return iter;
}

template <class T>
T& DoublyLinkedList<T>::operator[](const size_t index) const
{
    list_item *li = &_sentinel;
    for (size_t i = 0; i < index; i++) {
        li = li->next;
    }
    return li->item;
}

#endif /* _DOUBLY_LINKED_LIST_H */


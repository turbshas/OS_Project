#ifndef _DOUBLY_LINKED_LIST_H
#define _DOUBLY_LINKED_LIST_H

#include <cstdio>

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
                list_item *current_item;
        };

        DoublyLinkedList();
        DoublyLinkedList(const DoublyLinkedList&) = delete;
        DoublyLinkedList(DoublyLinkedList&&) = delete;
        ~DoublyLinkedList();
        DoublyLinkedList& operator=(const DoublyLinkedList&) = delete;
        DoublyLinkedList& operator=(DoublyLinkedList&&) = delete;

        void pushFront(const T& item);
        void pushBack(const T& item);
        T popFront();
        T popBack();

        size_t size() const { return num_items; };
        bool empty() const { return num_items == 0; };
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

        size_t num_items;
        list_item sentinel;
};

/*
 * Iterator definitions
 */
template <class T>
DoublyLinkedList<T>::Iterator::Iterator(const list_item& start)
{
    current_item = &start;
}

template <class T>
void DoublyLinkedList<T>::Iterator::moveNext()
{
    current_item = current_item->next;
}

template <class T>
T& DoublyLinkedList<T>::Iterator::currentItem() const
{
    return current_item->item;
}

template <class T>
T DoublyLinkedList<T>::Iterator::removeCurrent()
{
    const list_item *const next_item = current_item->next;
    const T item = current_item->item;

    delete current_item;

    current_item = next_item;
    return item;
}

template <class T>
void DoublyLinkedList<T>::Iterator::insertBeforeCurrent(const T& item)
{
    new list_item(current_item->prev, current_item, item);
}

template <class T>
void DoublyLinkedList<T>::Iterator::insertAfterCurrent(const T& item)
{
    new list_item(current_item, current_item->next, item);
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
    : num_items(0),
    sentinel()
{ }

//DoublyLinkedList::DoublyLinkedList(const DoublyLinkedList& other) {}
//DoublyLinkedList::DoublyLinkedList(DoublyLinkedList&& other) {}

template <class T>
DoublyLinkedList<T>::~DoublyLinkedList()
{
    clear();
}

template <class T>
void DoublyLinkedList<T>::pushFront(const T& item)
{
    new list_item(&sentinel, sentinel.next, item);
    num_items++;
}

template <class T>
void DoublyLinkedList<T>::pushBack(const T& item)
{
    new list_item(sentinel.prev, &sentinel, item);
    num_items++;
}

template <class T>
T DoublyLinkedList<T>::popFront()
{
    list_item *li = sentinel.next;
    const T item = li->item;
    delete li;

    num_items--;
    return item;
}

template <class T>
T DoublyLinkedList<T>::popBack()
{
    list_item *li = sentinel.prev;
    const T item = li->item;
    delete li;

    num_items--;
    return item;
}

template <class T>
void DoublyLinkedList<T>::clear()
{
    for (size_t i = 0; i < num_items; i++) {
        delete sentinel.next;
    }
}

template <class T>
T DoublyLinkedList<T>::removeItem(const size_t index)
{
    list_item *li = &sentinel;
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
    Iterator iter(sentinel);
    return iter;
}

template <class T>
T& DoublyLinkedList<T>::operator[](const size_t index) const
{
    list_item *li = &sentinel;
    for (size_t i = 0; i < index; i++) {
        li = li->next;
    }
    return li->item;
}

#endif /* _DOUBLY_LINKED_LIST_H */


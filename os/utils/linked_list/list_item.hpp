#ifndef _LINKED_LIST_DETAILS__LIST_ITEM
#define _LINKED_LIST_DETAILS__LIST_ITEM

namespace os::utils::linked_list::details
{
    class empty_list_item
    {
        public:
            empty_list_item* prev;
            empty_list_item* next;

            empty_list_item()
                : prev(this),
                  next(this)
            {
            }

            empty_list_item(empty_list_item* const prev_item, empty_list_item* const next_item)
                : prev(prev_item),
                  next(next_item)
            {
            }

            empty_list_item(const empty_list_item&) = delete;
            empty_list_item(empty_list_item&&) = delete;

            ~empty_list_item()
            {
                prev->next = next;
                next->prev = prev;
            }

            empty_list_item& operator=(const empty_list_item&) = delete;
            empty_list_item& operator=(empty_list_item&&) = delete;
    };

    template <class T>
    class list_item : public empty_list_item
    {
        public:
            T item;

            list_item() = delete;
            list_item(const list_item&) = delete;
            list_item(list_item&&) = delete;

            list_item(empty_list_item* const prev_item, empty_list_item* const next_item, const T& item_val)
                : empty_list_item(prev_item, next_item),
                  item(item_val)
            {
                prev->next = this;
                next->prev = this;
            }

            ~list_item()
            {
                // Do nothing.
            }

            list_item& operator=(const list_item&) = delete;
            list_item& operator=(list_item&&) = delete;
    };
}

#endif /* _LINKED_LIST_DETAILS__LIST_ITEM */

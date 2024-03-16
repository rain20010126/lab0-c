#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (head == NULL)
        return NULL;

    INIT_LIST_HEAD(head);
    return head;
}


/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, head, list)
        q_release_element(entry);
    free(head);
}



/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *node = malloc(sizeof(element_t));
    if (!node) {
        free(node);
        return false;
    }

    // 獲得字串字數並使用strncpy將字串複製過去配置好的記憶體空間
    node->value = malloc((strlen(s) + 1) * sizeof(char));
    if (!node->value) {
        free(node);
        return false;
    }
    strncpy(node->value, s, strlen(s) + 1);

    list_add(&node->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *node = malloc(sizeof(element_t));
    if (!node) {
        free(node);
        return false;
    }

    // 獲得字串字數並使用strncpy將字串複製過去配置好的記憶體空間
    node->value = malloc((strlen(s) + 1) * sizeof(char));
    if (!node->value) {
        free(node);
        return false;
    }
    strncpy(node->value, s, strlen(s) + 1);

    list_add_tail(&node->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *node = list_first_entry(head, element_t, list);

    list_del(head->next);

    if (sp) {
        strncpy(sp, node->value, bufsize);
        sp[bufsize - 1] = '\0';
    }

    return node;
}


/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    return q_remove_head(head->prev->prev, sp, bufsize);
}



/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    const struct list_head *tail = head->prev;
    struct list_head *fast = head->next, *slow = fast;
    while (fast != tail && fast->next != tail) {
        fast = fast->next->next;
        slow = slow->next;
    }

    list_del(slow);
    q_release_element(container_of(slow, element_t, list));
    return true;
}


/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;

    struct list_head *current = head->next, *next = NULL;
    bool remove_cur = false;

    while (current != head && current->next != head) {
        next = current->next;

        while (next != head &&
               strcmp(list_entry(next, element_t, list)->value,
                      list_entry(current, element_t, list)->value) == 0) {
            remove_cur = true;
            list_del(next);
            q_release_element(list_entry(next, element_t, list));
            next = current->next;
        }

        current = current->next;

        if (remove_cur) {
            struct list_head *del = current->prev;
            list_del(del);
            q_release_element(list_entry(del, element_t, list));
            remove_cur = false;
        }
    }
    return true;
}


/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *n = head->next;
    while (n != head && n->next != head) {
        list_move(n, n->next);
        n = n->next;
    }
}



/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    struct list_head *first = head->next;
    struct list_head *tail = head->prev;
    while (tail->prev != head) {
        list_move(first, tail);
        first = head->next;
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head))
        return;

    int size = q_size(head);
    struct list_head *tail = head;
    struct list_head *this_term_head = head;

    while (size >= k) {
        size = size - k;

        for (int i = 0; i < k && tail->next; i++) {
            tail = tail->next;
        }
        struct list_head *first = this_term_head->next;
        struct list_head *next_term_head = first;
        while (tail->prev != this_term_head) {
            list_move(first, tail);
            first = this_term_head->next;
        }
        tail = next_term_head;
        this_term_head = next_term_head;
    }
}


struct list_head *merge_two_nodes(struct list_head *L, struct list_head *R)
{
    if (!L && !R) {
        return NULL;
    }

    struct list_head h;
    struct list_head *head = &h;

    while (L && R) {
        if (strcmp(list_entry(L, element_t, list)->value,
                   list_entry(R, element_t, list)->value) <= 0) {
            head->next = L;
            L = L->next;
        } else {
            head->next = R;
            R = R->next;
        }
        head = head->next;
    }

    // Link the remaining elements to the headㄝㄝ
    if (L) {
        head->next = L;
    }
    if (R) {
        head->next = R;
    }

    return h.next;
}


struct list_head *merge_divide(struct list_head *head)
{
    if (!head || !head->next)
        return head;
    struct list_head *rabbit = head, *turtle = head;

    while (rabbit && rabbit->next) {
        rabbit = rabbit->next->next;
        turtle = turtle->next;
    }

    turtle->prev->next = NULL;

    struct list_head *L = merge_divide(head);
    struct list_head *R = merge_divide(turtle);

    return merge_two_nodes(L, R);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return;

    head->prev->next = NULL;
    head->next = merge_divide(head->next);

    struct list_head *before = head, *after = head->next;
    for (; after != NULL; after = after->next) {
        after->prev = before;
        before = after;
    }
    before->next = head;
    head->prev = before;
}


int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;
    struct list_head *ptr = head->prev;
    while (ptr != head && ptr->prev != head) {
        int compare = strcmp(list_entry(ptr, element_t, list)->value,
                             list_entry(ptr->prev, element_t, list)->value);
        if (compare >= 0) {
            element_t *entry = container_of(ptr->prev, element_t, list);
            list_del(ptr->prev);
            q_release_element(entry);
        }

        else
            ptr = ptr->prev;
    }
    return q_size(head);
}



/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;

    int size = 0;
    struct list_head *first = list_entry(head->next, queue_contex_t, chain)->q;
    size += q_size(first);
    first->prev->next = NULL;

    for (struct list_head *list = head->next->next; list != head;
         list = list->next) {
        struct list_head *node = list_entry(list, queue_contex_t, chain)->q;
        size += q_size(node);
        node->prev->next = NULL;
        merge_two_nodes(first->next, node->next);
        INIT_LIST_HEAD(node);
    }

    struct list_head *before = first, *after = first->next;
    for (; after != NULL; after = after->next) {
        after->prev = before;
        before = after;
    }
    before->next = first;
    first->prev = before;

    return size;
}

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


    while (current != head && current->next != head) {
        next = current->next;
        bool remove_cur = false;
        element_t *entry;

        while (!strcmp(list_entry(next, element_t, list)->value,
                       list_entry(current, element_t, list)->value)) {
            remove_cur = true;
            list_del(next);
            entry = container_of(next, element_t, list);
            q_release_element(entry);
            next = current->next;
        }

        current = current->next;
        if (remove_cur) {
            struct list_head *del = current->prev;
            list_del(del);
            entry = container_of(del, element_t, list);
            q_release_element(entry);
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
    int size = q_size(head);
    struct list_head *tail = head->next;
    struct list_head *first = NULL;
    struct list_head *next_term_head = head;

    while (size > k) {
        size = size - k;

        for (int i = 0; i < k; i++) {
            tail = tail->next;
        }
        first = next_term_head->next;
        next_term_head = first;
        while (tail->prev != head && tail->prev != next_term_head) {
            list_move(first, tail);
            first = next_term_head->next;
        }
        tail = next_term_head->next;
    }
}



/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}


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
        int cmp = strcmp(list_entry(ptr, element_t, list)->value,
                         list_entry(ptr->prev, element_t, list)->value);
        if (cmp >= 0) {
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
    return 0;
}

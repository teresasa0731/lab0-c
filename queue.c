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
    struct list_head *new = malloc(sizeof(struct list_head));
    if (!new)
        return NULL;
    INIT_LIST_HEAD(new);
    return new;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    element_t *cur, *next;
    list_for_each_entry_safe (cur, next, head, list)
        q_release_element(cur);

    free(head);
}


/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new_node = malloc(sizeof(element_t));
    // malloc failure
    if (!new_node)
        return false;

    new_node->value = malloc(strlen(s) + 1);

    // duplicate string failure
    if (!new_node->value) {
        free(new_node);
        return false;
    }

    if (strncpy(new_node->value, s, strlen(s) + 1) == NULL) {
        free(new_node->value);
        free(new_node);
        return false;
    }

    list_add(&new_node->list, head);
    return true;
}


/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new_node = malloc(sizeof(element_t));
    // malloc failure
    if (!new_node)
        return false;

    new_node->value = malloc(strlen(s) + 1);

    // duplicate string failure
    if (!new_node->value) {
        free(new_node);
        return false;
    }

    if (strncpy(new_node->value, s, strlen(s) + 1) == NULL) {
        free(new_node->value);
        free(new_node);
        return false;
    }

    list_add_tail(&new_node->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *rm_node = list_first_entry(head, element_t, list);
    list_del(&rm_node->list);

    if (sp) {
        size_t str_length = strnlen(rm_node->value, bufsize - 1);
        strncpy(sp, rm_node->value, str_length);
        sp[str_length] = '\0';  // Ensure null-termination of the copied string
    }
    return rm_node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *rm_node = list_last_entry(head, element_t, list);
    list_del(&rm_node->list);

    if (sp) {
        size_t str_length = strnlen(rm_node->value, bufsize - 1);
        strncpy(sp, rm_node->value, str_length);
        sp[str_length] = '\0';  // Ensure null-termination of the copied string
    }
    return rm_node;
}


/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    int size = 0;
    struct list_head *it;
    list_for_each (it, head)
        size++;
    return size;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    struct list_head *slow, *fast = head->next->next;
    list_for_each (slow, head) {
        if (fast == head || fast->next == head)
            break;
        fast = fast->next->next;
    }
    element_t *del_element = list_entry(slow, element_t, list);
    list_del(slow);
    free(del_element->value);
    free(del_element);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}

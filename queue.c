#include <stdint.h>
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
        q_release_element(new_node);
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
        q_release_element(new_node);
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
        strncpy(sp, rm_node->value, bufsize);
        sp[bufsize - 1] = '\0';  // Ensure null-termination of the copied string
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
        strncpy(sp, rm_node->value, bufsize);
        sp[bufsize - 1] = '\0';  // Ensure null-termination of the copied string
    }
    return rm_node;
}


/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

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

    struct list_head *front = head->next, *back = head->prev;
    while (front != back && front->next != back) {
        front = front->next;
        back = back->prev;
    }
    list_del(front);
    q_release_element(container_of(front, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    bool found = false;
    element_t *cur, *next;
    list_for_each_entry_safe (cur, next, head, list) {
        if (&next->list == head) {
            if (found) {
                list_del(&cur->list);
                q_release_element(cur);
            }
            return true;
        }

        if (strcmp(cur->value, next->value) == 0) {
            list_del(&cur->list);
            q_release_element(cur);
            found = true;
        } else {
            if (found) {
                list_del(&cur->list);
                q_release_element(cur);
                found = false;
            }
        }
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head)
        return;
    struct list_head *node;
    list_for_each (node, head) {
        if (node->next == head) {
            return;
        }
        list_move(node, node->next);
    }
    return;
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        node->next = node->prev;
        node->prev = safe;
    }
    node->next = node->prev;
    node->prev = safe;
    return;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head)
        return;

    int group = q_size(head) / k;
    struct list_head *cut;

    LIST_HEAD(tmp);
    LIST_HEAD(new_head);

    while (group) {
        int cnt = k;
        list_for_each (cut, head) {
            if (cnt)
                break;
            cnt--;
        }
        list_cut_position(&tmp, head, cut->prev);
        q_reverse(&tmp);
        list_splice_tail_init(&tmp, &new_head);
        group--;
    }
    list_splice_init(&new_head, head);
}

static struct list_head *mergeSortedList(struct list_head *L,
                                         struct list_head *R)
{
    struct list_head *head = NULL;
    struct list_head **ptr = &head;

    while (L && R) {
        element_t *left = list_entry(L, element_t, list);
        element_t *right = list_entry(R, element_t, list);

        if (strcmp(left->value, right->value) > 0) {
            *ptr = R;
            R = R->next;
        } else {
            *ptr = L;
            L = L->next;
        }
        ptr = &(*ptr)->next;
    }
    *ptr = (struct list_head *) ((uintptr_t) R | (uintptr_t) L);
    return head;
}

static struct list_head *mergeSort(struct list_head *head)
{
    if (!head || !head->next)
        return head;

    struct list_head *mid = head, *fast = head;

    while (fast && fast->next) {
        mid = mid->next;
        fast = fast->next->next;
    }
    mid->prev->next = NULL;

    return mergeSortedList(mergeSort(head), mergeSort(mid));
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || !head->next)
        return;

    struct list_head *tmp_prev = head, *it = head->next;

    head->prev->next = NULL;
    head->next = mergeSort(head->next);

    // reconstruct double-linked list
    while (it) {
        it->prev = tmp_prev;
        tmp_prev = it;
        it = it->next;
    }
    tmp_prev->next = head;
    head->prev = tmp_prev;

    if (descend)
        q_reverse(head);
}

/* Import Linux kernel list sort */

typedef int (*list_cmp_func_t)(const struct list_head *,
                               const struct list_head *);


static struct list_head *merge(list_cmp_func_t cmp,
                               struct list_head *a,
                               struct list_head *b)
{
    struct list_head *head = NULL, **tail = &head;

    for (;;) {
        if (cmp(a, b) <= 0) {
            *tail = a;
            tail = &a->next;
            a = a->next;
            if (!a) {
                *tail = b;
                break;
            }
        } else {
            *tail = b;
            tail = &b->next;
            b = b->next;
            if (!b) {
                *tail = a;
                break;
            }
        }
    }
    return head;
}

static void merge_final(list_cmp_func_t cmp,
                        struct list_head *head,
                        struct list_head *a,
                        struct list_head *b)
{
    struct list_head *tail = head;
    unsigned char count = 0;
    for (;;) {
        if (cmp(a, b) <= 0) {
            tail->next = a;
            a->prev = tail;
            tail = a;
            a = a->next;
            if (!a)
                break;
        } else {
            tail->next = b;
            b->prev = tail;
            tail = b;
            b = b->next;
            if (!b) {
                b = a;
                break;
            }
        }
    }

    tail->next = b;
    do {
        if (__glibc_unlikely(!++count))
            cmp(b, b);
        b->prev = tail;
        tail = b;
        b = b->next;
    } while (b);

    tail->next = head;
    head->prev = tail;
}


void list_sort(struct list_head *head, list_cmp_func_t cmp)
{
    struct list_head *list = head->next, *pending = NULL;
    size_t count = 0;

    if (list == head->prev)
        return;

    head->prev->next = NULL;

    do {
        size_t bits;
        struct list_head **tail = &pending;
        for (bits = count; bits & 1; bits >>= 1)
            tail = &(*tail)->prev;

        // for GNU C Library
        if (__glibc_likely(bits)) {
            struct list_head *a = *tail, *b = a->prev;

            a = merge(cmp, b, a);
            a->prev = b->prev;
            *tail = a;
        }

        list->prev = pending;
        pending = list;
        list = list->next;
        pending->next = NULL;
        count++;
    } while (list);

    list = pending;
    pending = pending->prev;
    for (;;) {
        struct list_head *next = pending->prev;

        if (!next)
            break;
        list = merge(cmp, pending, list);
        pending = next;
    }
    merge_final(cmp, head, pending, list);
}

/* End of linux kernel list sort */

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    element_t *back = list_entry(head->prev, element_t, list);
    element_t *front = list_entry(head->prev->prev, element_t, list);

    while (front->list.prev != head) {
        if (strcmp(back->value, front->value) > 0) {
            // back value > front value (strictly ascend): both move ahead one
            // entry
            front = list_entry(front->list.prev, element_t, list);
            back = list_entry(back->list.prev, element_t, list);
        } else {
            list_del(&front->list);
            free(front->value);
            free(front);
            front = list_entry(back->list.prev, element_t, list);
        }
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    element_t *back = list_entry(head->prev, element_t, list);
    element_t *front = list_entry(head->prev->prev, element_t, list);

    while (front->list.prev != head) {
        if (strcmp(back->value, front->value) < 0) {
            front = list_entry(front->list.prev, element_t, list);
            back = list_entry(back->list.prev, element_t, list);
        } else {
            list_del(&front->list);
            free(front->value);
            free(front);
            front = list_entry(back->list.prev, element_t, list);
        }
    }
    return q_size(head);
}

int mergeTwoLists(struct list_head *l1_head, struct list_head *l2_head)
{
    int l1_len = q_size(l1_head);
    int l2_len = q_size(l2_head);

    struct list_head tmp;
    INIT_LIST_HEAD(&tmp);

    while (l1_len > 0 && l2_len > 0) {
        element_t *l1_entry = list_entry(l1_head->next, element_t, list);
        element_t *l2_entry = list_entry(l2_head->next, element_t, list);

        if (strcmp(l1_entry->value, l2_entry->value) > 0) {
            list_move_tail(l2_head->next, &tmp);
            l2_len--;
        } else {
            list_move_tail(l1_head->next, &tmp);
            l1_len--;
        }
    }

    list_splice_tail(l2_len > 0 ? l2_head : l1_head, &tmp);
    INIT_LIST_HEAD(l2_len > 0 ? l2_head : l1_head);

    list_splice_tail(&tmp, l1_head);
    return q_size(l1_head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return 0;

    struct list_head *first = head->next;
    struct list_head *next = first->next;
    int size = 0;

    queue_contex_t *first_queue = list_entry(first, queue_contex_t, chain);

    for (int i = 1; i < q_size(head); i++) {
        queue_contex_t *next_queue = list_entry(next, queue_contex_t, chain);
        size = mergeTwoLists(first_queue->q, next_queue->q);
        next = next->next;
    }
    return size;
}

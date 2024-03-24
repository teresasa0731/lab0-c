#include "queue.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list_sort.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

#define sortVer 1


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
    /*malloc failure*/
    if (!new_node)
        return false;

    new_node->value = malloc(strlen(s) + 1);

    /*duplicate string failure*/
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
    /* malloc failure*/
    if (!new_node)
        return false;

    new_node->value = malloc(strlen(s) + 1);

    /*duplicate string failure*/
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
        sp[bufsize - 1] =
            '\0'; /* Ensure null-termination of the copied string*/
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
        sp[bufsize - 1] = '\0'; /*Ensure null-termination of the copied string*/
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
        int cnt = 0;
        list_for_each (cut, head) {
            if (cnt >= k)
                break;
            cnt++;
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
    struct list_head *head = L, *prev = NULL;
    struct list_head **ptr = &head;

    while (L && R) {
        if (strcmp(list_entry(L, element_t, list)->value,
                   list_entry(R, element_t, list)->value) > 0) {
            *ptr = R;
            (*ptr)->prev = prev;
            prev = R;
            R = R->next;
        } else {
            *ptr = L;
            (*ptr)->prev = prev;
            prev = L;
            L = L->next;
        }
        ptr = &(*ptr)->next;
    }

    *ptr = (struct list_head *) ((uintptr_t) R | (uintptr_t) L);
    (*ptr)->prev = prev;
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

void merge_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || !head->next)
        return;

    /*cut circular list*/
    head->prev->next = NULL;
    head->next = mergeSort(head->next);

    /*econstruct double-linked list*/
    head->next->prev = head;
    struct list_head *tail = head->next;
    while (tail->next)
        tail = tail->next;

    head->prev = tail;
    tail->next = head;

    if (descend)
        q_reverse(head);
}

int cmp(void *priv, const struct list_head *a, const struct list_head *b)
{
    bool descend = *(bool *) priv;
    char *a_val = list_entry(a, element_t, list)->value;
    char *b_val = list_entry(b, element_t, list)->value;
    return descend ? strcmp(b_val, a_val) : strcmp(a_val, b_val);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
#if (sortVer == 1)
    list_sort(&descend, head, cmp);
#else
    merge_sort(head, descend);
#endif
}



/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    element_t *back = list_entry(head->prev, element_t, list);
    element_t *front = list_entry(head->prev->prev, element_t, list);

    while (&front->list != head) {
        if (strcmp(back->value, front->value) > 0) {
            /*back value > front value (strictly ascend): both move ahead one*/
            /*entry*/
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

    while (&front->list != head) {
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

/* for merging two list */
void mergeTwoList(struct list_head *l1, struct list_head *l2, bool descend)
{
    if (!l1 || !l2)
        return;

    LIST_HEAD(tmp);

    while (!list_empty(l1) && !list_empty(l2)) {
        element_t *l1_entry = list_first_entry(l1, element_t, list);
        element_t *l2_entry = list_first_entry(l2, element_t, list);
        element_t *tmp_entry;
        if (descend)
            tmp_entry = strcmp(l1_entry->value, l2_entry->value) > 0 ? l1_entry
                                                                     : l2_entry;
        else
            tmp_entry = strcmp(l1_entry->value, l2_entry->value) < 0 ? l1_entry
                                                                     : l2_entry;

        list_move_tail(&tmp_entry->list, &tmp);
    }

    list_splice_tail_init(l1, &tmp);
    list_splice_tail_init(l2, &tmp);
    list_splice(&tmp, l1);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return 0;

    if (list_is_singular(head))
        return q_size(list_first_entry(head, queue_contex_t, chain)->q);

    int size = q_size(head);
    int iter = (size & 1) ? (size >> 1) + 1 : size >> 1;

    for (int i = 0; i < iter; i++) {
        queue_contex_t *l1 = list_first_entry(head, queue_contex_t, chain);
        queue_contex_t *l2 = list_entry(l1->chain.next, queue_contex_t, chain);

        while (!list_empty(l1->q) && !list_empty(l2->q)) {
            mergeTwoList(l1->q, l2->q, descend);
            list_move_tail(&l2->chain, head);

            /*move to next pair of queues*/
            l1 = list_entry(l1->chain.next, queue_contex_t, chain);
            l2 = list_entry(l1->chain.next, queue_contex_t, chain);
        }
    }
    return q_size(head);
}
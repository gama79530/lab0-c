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
    struct list_head *head =
        (struct list_head *) malloc(sizeof(struct list_head));
    if (head)
        INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    element_t *entry = NULL, *safe = NULL;
    list_for_each_entry_safe (entry, safe, head, list) {
        if (entry->value)
            free(entry->value);
        free(entry);
    }
    free(head);
}

/* for q_insert_head and q_insert_tail */
#define q_insert(head, s, add_func)                             \
    if (!head)                                                  \
        return false;                                           \
    element_t *entry = (element_t *) malloc(sizeof(element_t)); \
    if (!entry)                                                 \
        return false;                                           \
    entry->value = strdup(s);                                   \
    if (!entry->value) {                                        \
        free(entry);                                            \
        return false;                                           \
    }                                                           \
    add_func(&entry->list, head);                               \
    return true;

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    q_insert(head, s, list_add);
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    q_insert(head, s, list_add_tail);
}

/* for q_remove_head and q_remove_tail */
#define q_remove(head, sp, bufsize, entry_getter)           \
    if (!head || list_empty(head))                          \
        return NULL;                                        \
    element_t *entry = entry_getter(head, element_t, list); \
    list_del(&entry->list);                                 \
    if (sp && entry->value) {                               \
        strncpy(sp, entry->value, bufsize - 1);             \
        sp[bufsize - 1] = 0;                                \
    }                                                       \
    return entry;

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    q_remove(head, sp, bufsize, list_first_entry);
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    q_remove(head, sp, bufsize, list_last_entry);
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;
    int len = 0;
    struct list_head *node;
    list_for_each (node, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;
    struct list_head *forward = head->next, *backward = head->prev;
    while (forward != backward && forward->next != backward) {
        forward = forward->next;
        backward = backward->prev;
    }
    list_del(backward);
    element_t *entry = list_entry(backward, element_t, list);
    if (entry->value)
        free(entry->value);
    free(entry);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head))
        return false;
    element_t *entry = NULL, *safe = NULL, *tmp_entry = NULL;
    bool do_del = false;
    list_for_each_entry_safe (entry, safe, head, list) {
        // cppcheck-suppress knownConditionTrueFalse
        if (!tmp_entry || strcmp(entry->value, tmp_entry->value)) {
            if (do_del) {
                list_del(&tmp_entry->list);
                free(tmp_entry->value);
                free(tmp_entry);
                do_del = false;
            }
            tmp_entry = entry;
        } else {
            list_del(&entry->list);
            free(entry->value);
            free(entry);
            do_del = true;
        }
    }
    if (do_del) {
        list_del(&tmp_entry->list);
        free(tmp_entry->value);
        free(tmp_entry);
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head))
        return;
    LIST_HEAD(tmp_head);
    list_splice_init(head, &tmp_head);
    struct list_head *node, *safe, *tmp_node = NULL;
    list_for_each_safe (node, safe, &tmp_head) {
        list_del(node);
        if (tmp_node) {
            list_add_tail(node, head);
            list_add_tail(tmp_node, head);
            tmp_node = NULL;
        } else {
            tmp_node = node;
        }
    }
    if (tmp_node)
        list_add_tail(tmp_node, head);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head)
        list_move(node, head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head))
        return;
    struct list_head *node = NULL, *safe = NULL;
    LIST_HEAD(tmp_head);
    LIST_HEAD(rev_head);
    list_splice_init(head, &tmp_head);
    int count = 0;
    list_for_each_safe (node, safe, &tmp_head) {
        if (++count == k) {
            list_cut_position(&rev_head, &tmp_head, node);
            q_reverse(&rev_head);
            list_splice_tail_init(&rev_head, head);
            count = 0;
        }
    }
    if (count)
        list_splice_tail(&tmp_head, head);
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    LIST_HEAD(tmp_head_1);
    LIST_HEAD(tmp_head_2);
    list_splice_init(head, &tmp_head_1);
    struct list_head *forward = tmp_head_1.next, *backward = tmp_head_1.prev;
    while (forward != backward && forward->next != backward) {
        forward = forward->next;
        backward = backward->prev;
    }
    list_cut_position(&tmp_head_2, &tmp_head_1, forward);
    q_sort(&tmp_head_1, descend);
    q_sort(&tmp_head_2, descend);
    while (!list_empty(&tmp_head_1) && !list_empty(&tmp_head_2)) {
        element_t *e_1 = list_first_entry(&tmp_head_1, element_t, list);
        element_t *e_2 = list_first_entry(&tmp_head_2, element_t, list);
        element_t *e = (descend && strcmp(e_1->value, e_2->value) > 0) ||
                               (!descend && strcmp(e_1->value, e_2->value) < 0)
                           ? e_1
                           : e_2;
        list_move_tail(&e->list, head);
    }
    struct list_head *tmp_head =
        list_empty(&tmp_head_1) ? &tmp_head_2 : &tmp_head_1;
    list_splice_tail_init(tmp_head, head);
}

/* for q_ascend and q_descend */
#define q_ascend_descend(head, stop_cond)                           \
    int ret = 0;                                                    \
    if (!head || list_empty(head))                                  \
        return ret;                                                 \
    LIST_HEAD(tmp_head);                                            \
    list_splice_init(head, &tmp_head);                              \
    element_t *entry = NULL, *safe = NULL, *tmp_entry = NULL;       \
    list_for_each_entry_safe (entry, safe, &tmp_head, list) {       \
        ret++;                                                      \
        while (!list_empty(head)) {                                 \
            tmp_entry = list_last_entry(head, element_t, list);     \
            if (strcmp(entry->value, tmp_entry->value) stop_cond 0) \
                break;                                              \
            list_del(&tmp_entry->list);                             \
            free(tmp_entry->value);                                 \
            free(tmp_entry);                                        \
            ret--;                                                  \
        }                                                           \
        list_add_tail(&entry->list, head);                          \
    }                                                               \
    return ret;

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    q_ascend_descend(head, >=);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    q_ascend_descend(head, <=);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;
    queue_contex_t *qc = list_first_entry(head, queue_contex_t, chain);
    if (list_is_singular(head))
        return q_size(qc->q);
    int offset = 1;
    LIST_HEAD(tmp_q_head);
    int chain_size = 0;
    bool first_loop = true;
    while (first_loop || offset < chain_size) {
        int count = 1;
        queue_contex_t *entry_qc_1 = NULL, *entry_qc_2 = NULL;
        list_for_each_entry (entry_qc_2, head, chain) {
            chain_size += first_loop;
            // cppcheck-suppress knownConditionTrueFalse
            if (--count)
                continue;
            // cppcheck-suppress knownConditionTrueFalse
            if (entry_qc_1) {
                struct list_head *q_head_1 = entry_qc_1->q;
                struct list_head *q_head_2 = entry_qc_2->q;
                while (!list_empty(q_head_1) && !list_empty(q_head_2)) {
                    element_t *e_1 =
                        list_first_entry(q_head_1, element_t, list);
                    element_t *e_2 =
                        list_first_entry(q_head_2, element_t, list);
                    element_t *e =
                        (descend && strcmp(e_1->value, e_2->value) > 0) ||
                                (!descend && strcmp(e_1->value, e_2->value) < 0)
                            ? e_1
                            : e_2;
                    list_move_tail(&e->list, &tmp_q_head);
                }
                if (list_empty(q_head_1))
                    list_splice_init(q_head_2, q_head_1);
                list_splice_init(&tmp_q_head, q_head_1);
            } else {
                entry_qc_1 = entry_qc_2;
            }
            count = offset;
        }
        first_loop = false;
        offset <<= 1;
    }
    return q_size(qc->q);
}

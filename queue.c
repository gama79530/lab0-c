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

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *entry = (element_t *) malloc(sizeof(element_t));
    if (!entry)
        return false;
    entry->value = strdup(s);
    if (!entry->value) {
        free(entry);
        return false;
    }
    list_add(&entry->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *entry = (element_t *) malloc(sizeof(element_t));
    if (!entry)
        return false;
    entry->value = strdup(s);
    if (!entry->value) {
        free(entry);
        return false;
    }
    list_add_tail(&entry->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *entry = list_first_entry(head, element_t, list);
    list_del(&entry->list);
    if (sp && entry->value) {
        strncpy(sp, entry->value, bufsize - 1);
        sp[bufsize - 1] = 0;
    }
    return entry;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *entry = list_last_entry(head, element_t, list);
    list_del(&entry->list);
    if (sp && entry->value) {
        strncpy(sp, entry->value, bufsize - 1);
        sp[bufsize - 1] = 0;
    }
    return entry;
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
    list_for_each_safe (node, safe, head) {
        list_del(node);
        list_add(node, head);
    }
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

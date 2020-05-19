#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"
#include "strnatcmp.h"
/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q;
    if (!(q = malloc(sizeof(queue_t)))) {
        return NULL;
    }
    q->head = q->tail = NULL;
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (q_status(q) == NOTHING) {
        return;
    }
    while (q_remove_head(q, NULL, 0)) {
        ;
    }
    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    list_ele_t *newh;
    /* if the queue is NULL OR malloc() failed, return false */
    if (q_status(q) == NOTHING || !(newh = malloc(sizeof(list_ele_t)))) {
        return false;
    }

    if (!(newh->value = malloc(strlen(s) + 1))) {
        free(newh);
        return false;
    }
    strncpy(newh->value, s, strlen(s) + 1);

    if (q_status(q) == EMPTY) {
        q->tail = newh;
    }
    newh->next = q->head;
    q->head = newh;
    q->size++;
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    list_ele_t *newt;
    /* if the queue is NULL OR malloc() failed, return false */
    if (q_status(q) == NOTHING || !(newt = malloc(sizeof(list_ele_t)))) {
        return false;
    }

    if (!(newt->value = malloc(strlen(s) + 1))) {
        free(newt);
        return false;
    }
    strncpy(newt->value, s, strlen(s) + 1);

    if (q_status(q) == EMPTY) {
        q->head = newt;
    } else {
        q->tail->next = newt;
    }
    q->tail = newt;
    newt->next = NULL;
    q->size++;
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (q_status(q) == NOTHING || q_status(q) == EMPTY) {
        return false;
    }

    list_ele_t *beRemoved = q->head;
    if (sp) {
        strncpy(sp, beRemoved->value, bufsize);

        /* strncpy truncate problem, need to assign '\0' manually*/
        sp[bufsize - 1] = '\0';
    }

    q->head = q->head->next;
    if (q_status(q) == EMPTY) {
        q->tail = NULL;
    }
    free(beRemoved->value);
    free(beRemoved);
    q->size--;

    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    if (q_status(q) == NOTHING || q_status(q) == EMPTY) {
        return 0;
    }
    return q->size;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (q_status(q) == NOTHING || q_status(q) == EMPTY) {
        return;
    }
    list_ele_t *newt = q->head;
    list_ele_t *next = NULL;
    list_ele_t *prev = NULL;
    while (q->head) {
        next = q->head->next;
        q->head->next = prev;

        /* move to next element */
        prev = q->head;
        q->head = next;
    }
    q->head = prev;
    q->tail = newt;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (q_status(q) == NOTHING || q_status(q) == EMPTY ||
        (q->head && !q->head->next)) {
        return;
    }
    q->head = mergeSort(q->head);
    /*list_ele_t *i = q->head;
    for (; i->next != NULL; i = i->next) {
        ;
    }
    q->tail = i;*/
}

int q_status(const queue_t *q)
{
    if (!q) {
        return NOTHING;
    } else {
        return (!q->head) ? EMPTY : OTHERS;
    }
}

int compare(const list_ele_t *a, const list_ele_t *b)
{
    // Origin comparision
    // return strcmp(a->value, b->value);

    // New one, using natural sort
    return strnatcmp(a->value, b->value);
}

list_ele_t *merge(list_ele_t *l1, list_ele_t *l2)
{
    list_ele_t t = {.value = "\0", .next = NULL};
    list_ele_t *tmp = &t;

    list_ele_t *q = tmp;
    while (l1 && l2) {
        if (compare(l1, l2) < 0) {
            tmp->next = l1;
            tmp = tmp->next;
            l1 = l1->next;
        } else {
            tmp->next = l2;
            tmp = tmp->next;
            l2 = l2->next;
        }
    }

    if (l1) {
        tmp->next = l1;
    }
    if (l2) {
        tmp->next = l2;
    }

    list_ele_t *head = q->next;

    return head;
}


list_ele_t *mergeSort(list_ele_t *head)
{
    if (!head || !head->next) {
        return head;
    }

    list_ele_t *fast = head->next;
    list_ele_t *slow = head;

    // split list
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    fast = slow->next;
    slow->next = NULL;

    list_ele_t *l1 = mergeSort(head);
    list_ele_t *l2 = mergeSort(fast);

    return merge(l1, l2);
}

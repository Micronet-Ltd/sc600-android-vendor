/******************************************************************************
 * Generic list routines.
 *
 * Copyright (c) 2003-2007 InnoPath Software, Inc. All rights reserved.
 *
 * Legal Statements
 *
 * THE SOFTWARE AND ANY MATERIALS PROVIDED BY INNOPATH ARE PROVIDED ON AN "AS
 * IS" BASIS WITHOUT ANY REPRESENTATIONS, WARRANTIES OR CONDITIONS OF ANY KIND,
 * WHETHER EXPRESS, IMPLIED, STATUTORY, OUT OF A COURSE OF DEALING OR USAGE,
 * TRADE OR OTHERWISE INCLUDING ANY IMPLIED WARRANTIES OR CONDITIONS OF
 * MERCHANTABILITY, MERCHANTABLE QUALITY, FITNESS FOR ANY PARTICULAR PURPOSE
 * OR USE, TITLE, OR NON-INFRINGEMENT. INNOPATH DOES NOT REPRESENT OR WARRANT
 * THAT THE SOFTWARE OR ANY MATERIALS WILL BE FREE OF DEFECTS, UNINTERRUPTED,
 * ACCURATE, COMPLETE, CURRENT, STABLE, BUG-FREE, ERROR-FREE, OR AVAILABLE AT
 * ANY TIME.
 *
 * TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL INNOPATH BE LIABLE
 * FOR ANY DIRECT, INDIRECT, CONSEQUENTIAL, INCIDENTAL, SPECIAL, RELIANCE,
 * PUNITIVE OR OTHER DAMAGES OR EXPENSES OF ANY KIND, INCLUDING BUT NOT LIMITED
 * TO ANY LOSS OF PROFITS OR REVENUE, LOST SAVINGS, INTERRUPTED BUSINESS, LOST
 * BUSINESS, LOST BUSINESS OPPORTUNITIES, LOST DATA, LOST GOODWILL, LOSS FROM
 * WORK STOPPAGE, COSTS OF OVERHEAD, COSTS OF COVER, LOSS OF ANTICIPATED
 * BENEFITS HEREUNDER, ARISING OUT OF OR RELATED TO THIS AGREEMENT, HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, EVEN IF INNOPATH HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES, AND NOTWITHSTANDING THE FAILURE OF THE
 * ESSENTIAL PURPOSE OF ANY LIMITED REMEDY STATED HEREIN.
 *****************************************************************************/

#ifndef __IPTH_IPTH_LIST_H__
#define __IPTH_IPTH_LIST_H__

#include "ipth_hal_config.h"

/**
 * \file
 *
 * This file defines several routines and macros for generic double-linked
 * list manipulations. Most operations are mirrored after Linux kernel
 * list operations.
 *
 */

/*
 * USAGE through example:
 *
 * - If you want to link a C structure (called container structure),
 *   add ipth_LinkNode inside the structure (called "link node member")
 *   You can add multiple link nodes if the same container structure can
 *   appear in multiple lists at the same time.
 *
 *  struct foo {
 *	char name;
 *	ipth_LinkNode link;
 *  };
 *
 * - To declare and define a static list head
 *
 *  ipth_ListHead fooListHead = IPTH_LIST_HEAD_INIT_VALUE(fooListHead);
 *
 * - To add a struct foo data, x, to the list, to the beginning or to the end
 *
 *  ipth_ListInsert(&fooListHead, &x.link);
 *  ipth_ListAppend(&fooListHead, &x.link);
 *
 * - To check to see if a node is in the list,
 *
 *  ipth_ListContainNode(&fooListHead, &x.link);
 *
 * - To remove a node from the list,
 *
 *  ipth_ListRemove(&fooListHead, &x.link);
 *
 * - There are multiple ways to iterate through the list.
 *
 *  Approach #1:
 *  struct foo *p;
 *  IPTH_LIST_ITERATE_ENTRY(p, &fooListHead, struct foo, link) {
 *	... do things with p
 *  }
 *
 * Approach #2:
 *  ipth_LinkNode *p;
 *  struct foo *q;
 *  IPTH_LIST_ITERATE(p, &fooListHead) {
 *	q = IPTH_LIST_ENTRY(p, struct foo, link);
 *	... do things with q
 *
 * Use the xxxx_SAFE() version for removal-safe iterations.
 *
 * Use the xxx_REVERSE_ITERATE_xxx() version for reverse iteration.
 *
 * - You can also split a list or concatnate two lists into one.
 *
 * - See the unit test (test/common/list/list_test.c) for more usage examples.
 */

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * A macro that converts from ListNode inside a container structure
     * back to the container itself.
     */
#define IPTH_LIST_ENTRY(node, type, member) \
	( (type *) ( (char *)(node) - offsetof(type,member) ) )

    /**
     * ListNode structure that should be defined in container structure to be
     * linked into lists.
     */
    typedef struct ipth_ListNode {
        struct ipth_ListNode *prev;
        struct ipth_ListNode *next;
    } ipth_ListNode;

    /**
     * Data type for list head variable (which happens to be same as ListNode)
     */
    typedef struct ipth_ListNode ipth_ListHead;

    /* the string list data structure */
    typedef struct {
        char *str;
        ipth_ListNode link;
    } ipth_stringListNode;

    /**
     * ListHead initial value for static value definition. For example,
     *
     * static ipth_ListHead myListHead = IPTH_LIST_HEAD_INIT_VALUE(myListHead);
     */
#define IPTH_LIST_HEAD_INIT_VALUE(name) {&(name), &(name)}

    /**
     * ListNode initial value.
     */
#define IPTH_LIST_NODE_INIT_VALUE() { NULL, NULL}

    /**
     * Run-time verion of initializing a list head.
     */
    void ipth_ListHeadInit(ipth_ListHead *h);

    /**
     * Run-time version of initializing a list node.
     */
    void ipth_ListNodeInit(ipth_ListNode *n);

    /**
     * Check to see if a list is empty.
     */
    int ipth_ListEmpty(ipth_ListHead *h);

    /**
     * Return list length.
     */
    int ipth_ListLength(ipth_ListHead *h);

    /**
     * Check to see if a node is contained in a list.
     */
    int ipth_ListContainNode(ipth_ListHead *h, ipth_ListNode *n);

    /**
     * Get the first list node in the list. Return NULL if list is empty.
     */
    ipth_ListNode * ipth_ListGetFirst(ipth_ListHead *h);

    /**
     * Convenience macro to get first entry.
     */
#define IPTH_LIST_GET_FIRST_ENTRY(h, type, member) \
	(ipth_ListGetFirst(h) ? IPTH_LIST_ENTRY(ipth_ListGetFirst(h), type, member) : NULL)

    /**
     * Get the last list node in the list. Return NULL if list is empty.
     */
    ipth_ListNode * ipth_ListGetLast(ipth_ListHead *h);

    /**
     * Convenience macro to get first entry.
     */
#define IPTH_LIST_GET_LAST_ENTRY(h, type, member) \
	(ipth_ListGetLast(h) ? IPTH_LIST_ENTRY(ipth_ListGetLast(h), type, member) : NULL)

    /**
     * Add a new node and make it the last in the list.
     */
    void ipth_ListAppend(ipth_ListHead *list, ipth_ListNode *newNode);

    /**
     * Add a new node and make it the first in the list.
     */
    void ipth_ListInsert(ipth_ListHead *list, ipth_ListNode *newNode);

    /**
     * Remove a node from a list. If h is unknown, it can be NULL.
     * if h is not NULL, validate to make sure n is in the list headed by h.
     */
    void ipth_ListRemove(ipth_ListHead *h, ipth_ListNode *n);

    /**
     * Remove all nodes from a list.
     */
    void ipth_ListRemoveAll(ipth_ListHead *h);

    /**
     * Move a node in the list to the first position. The node must be in
     * the list.
     */
    void ipth_ListMoveToFirst(ipth_ListHead *h, ipth_ListNode *n);

    /**
     * Move a node in the list to the tail position. The node must be in
     * the list.
     */
    void ipth_ListMoveToLast(ipth_ListHead *h, ipth_ListNode *n);

    /**
     * Split a list into two. The specified node will the first node in the
     * second list.
     */
    void ipth_ListSplit(ipth_ListHead *l1, ipth_ListNode *n, ipth_ListHead *l2);

    /**
     * Concatnate two lists into one. h1 will hold the new list. And h2
     * will be initialized and empty.
     */
    void ipth_ListConcatenate(ipth_ListHead *h1, ipth_ListHead *h2);

    /**
     * Check to see if a list is properly linked.
     */
    int ipth_ListValid(ipth_ListHead *h);

    /**
     * Macro to iterate through a list using ListNode pointer.
     */
#define IPTH_LIST_ITERATE(ptr, head) \
	for ((ptr)=(head)->next; (ptr) != (head); (ptr) = (ptr)->next)

    /**
     * Macro to iterate through a list using ListNode pointer that is
     * safe for node removel.
     */
#define IPTH_LIST_ITERATE_SAFE(ptr, head, tempPtr) \
	for ((ptr)=(head)->next, (tempPtr)=(ptr)->next; \
		 (ptr) != (head); \
		 (ptr)=(tempPtr), (tempPtr)=(ptr)->next)

    /**
     * Macro to iterate through a list using container pinter
     */
#define IPTH_LIST_ITERATE_ENTRY(ptr, head, type, member) \
	for ((ptr)=IPTH_LIST_ENTRY((head)->next, type, member); \
		 &(ptr)->member != (head); \
		 (ptr)=IPTH_LIST_ENTRY((ptr)->member.next, type, member))

    /**
     * Macro to iterate through a list using container pointer that is safe
     * for node removal.
     */
#define IPTH_LIST_ITERATE_ENTRY_SAFE(ptr, head, type, member, tempPtr) \
	for ((ptr)=IPTH_LIST_ENTRY((head)->next, type, member), \
			(tempPtr)=IPTH_LIST_ENTRY((ptr)->member.next, type, member); \
		 &(ptr)->member != (head); \
		 (ptr)=(tempPtr), (tempPtr)=IPTH_LIST_ENTRY((ptr)->member.next, type, member))

    /**
     * Macro to iterate through a list using ListNode pointer in reverse order.
     */
#define IPTH_LIST_REVERSE_ITERATE(ptr, head) \
	for ((ptr)=(head)->prev; (ptr) != (head); (ptr) = (ptr)->prev)

    /**
     * Reversely iterate through a list using ListNode pointer that is
     * safe for node removel.
     */
#define IPTH_LIST_REVERSE_ITERATE_SAFE(ptr, head, tempPtr) \
	for ((ptr)=(head)->prev, (tempPtr)=(ptr)->prev; \
		 (ptr) != (head); \
		 (ptr)=(tempPtr), (tempPtr)=(ptr)->prev)

    /**
     * Reversely iterate through a list using container pinter
     */
#define IPTH_LIST_REVERSE_ITERATE_ENTRY(ptr, head, type, member) \
	for ((ptr)=IPTH_LIST_ENTRY((head)->prev, type, member); \
		 &(ptr)->member != (head); \
		 (ptr)=IPTH_LIST_ENTRY((ptr)->member.prev, type, member))

    /**
     * Reversely iterate through a list using container pointer that is safe
     * for node removal.
     */
#define IPTH_LIST_REVERSE_ITERATE_ENTRY_SAFE(ptr, head, type, member, tempPtr) \
	for ((ptr)=IPTH_LIST_ENTRY((head)->prev, type, member), \
			(tempPtr)=IPTH_LIST_ENTRY((ptr)->member.prev, type, member); \
		 &(ptr)->member != (head); \
		 (ptr)=(tempPtr), (tempPtr)=IPTH_LIST_ENTRY((ptr)->member.prev, type, member))

#define IPTH_LIST_MOVE_H2H(dest, source) \
	ipth_ListMovePos2Pos(dest, 1, source, 1)

#define IPTH_LIST_MOVE_H2T(dest, source) \
	ipth_ListMovePos2Pos(dest, 0, source, 1)

#define IPTH_LIST_MOVE_T2H(dest, source) \
	ipth_ListMovePos2Pos(dest, 1, source, 0)

#define IPTH_LIST_MOVE_T2T(dest, source) \
	ipth_ListMovePos2Pos(dest, 0, source, 0)
    /**
     * Internal function to move a list element form one list to another.
     * This function should not be called directly. Please use the appropriate macro
     * instead. Returns a pointer to the moved element or NULL if nothing moved.
     */
    ipth_ListNode * ipth_ListMovePos2Pos(ipth_ListHead *dest, int destPos,
                                         ipth_ListHead *source, int sourcePos);


    /**
     * free a string list.
     *
     * \param[in] list		string list head.
     *
     * \return	Error code.
     */
    int ipth_freeStrList(ipth_ListHead *list);


#ifdef __cplusplus
}
#endif

#endif /* __IPTH_IPTH_LIST_H__ */

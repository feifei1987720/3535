#ifndef __MY_LIST_H__
#define __MY_LIST_H__

/* This file is from Linux Kernel (include/linux/list.h)
 * and modified by simply removing hardware prefetching of list items.
 * Here by copyright, credits attributed to wherever they belong.
 * Kulesh Shanmugasundaram (kulesh [squiggly] isis.poly.edu)
 */

/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions (__xxx) are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */

struct kernel_list_head {
	struct kernel_list_head *next, *prev;
};
/**
 * kernel_list_entry  get the struct for this entry
 * @ptr:    the &struct kernel_list_head pointer.
 * @type:    the type of the struct this is embedded in.
 * @member:    the name of the list_struct within the struct.
 */
#define kernel_list_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

/**
 * list_for_each    -    iterate over a list
 * @pos:    the &struct kernel_list_head to use as a loop counter.
 * @head:    the head for your list.
 */
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); \
			pos = pos->next)
/**
 * list_for_each_prev    -    iterate over a list backwards
 * @pos:    the &struct kernel_list_head to use as a loop counter.
 * @head:    the head for your list.
 */
#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); \
			pos = pos->prev)

/**
 * list_for_each_safe    -    iterate over a list safe against removal of list entry
 * @pos:    the &struct kernel_list_head to use as a loop counter.
 * @n:        another &struct kernel_list_head to use as temporary storage
 * @head:    the head for your list.
 */
#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head);\
			pos = n, n = pos->next)


/**
 * list_for_each_entry    -    iterate over list of given type
 * @pos:    the type * to use as a loop counter.
 * @head:    the head for your list.
 * @member:    the name of the list_struct within the struct.
 */
#define list_for_each_entry(pos, head, member)                \
	for (pos = kernel_list_entry((head)->next,typeof(*pos), member); \
			&pos->member != (head);                                  \
			pos = kernel_list_entry(pos->member.next, typeof(*pos), member))

/**
 * list_for_each_entry_safe  iterate over list of given type safe against removal of list entry
 * @pos:    the type * to use as a loop counter.
 * @n:        another type * to use as temporary storage
 * @head:    the head for your list.
 * @member:    the name of the list_struct within the struct.
 */
#define list_for_each_entry_safe(pos, n, head, member)            \
	for (pos = kernel_list_entry((head)->next, typeof(*pos), member),    \
			n = kernel_list_entry(pos->member.next, typeof(*pos), member);    \
			&pos->member != (head);                     \
			pos = n, n = kernel_list_entry(n->member.next, typeof(*n), member))

#endif//__LIST_H__


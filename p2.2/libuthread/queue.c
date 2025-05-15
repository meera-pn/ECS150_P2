#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

struct node {
	void *data;
	struct node *next;
};

struct queue {
	/* TODO Phase 1 */
	struct node *head;
	struct node *tail;
	int len;
};

queue_t queue_create(void)
{
	/* TODO Phase 1 */
	queue_t queue = malloc(sizeof(struct queue));
	queue->head = queue->tail = NULL;
	queue->len = 0;
	return queue;
}

int queue_destroy(queue_t queue)
{
	/* TODO Phase 1 */
	if (!queue || queue->len > 0){
		return -1;
	}
	struct node *current = queue->head;
	while(current){
		struct node *ph = current;
		current = current->next;
		free(ph);
	}
	free(queue);
	return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	/* TODO Phase 1 */
	if(!queue || !data){
		return -1;
	}
	struct node *new_node = malloc(sizeof(struct node));
	new_node->data = data;
	new_node->next = NULL;
	if (queue->tail){
		queue->tail->next = new_node;
	}
	else{
		queue->head = new_node;
	}
	queue->tail = new_node;
	queue->len++;
	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	/* TODO Phase 1 */
	if(!queue || !data || queue->len == 0){
		return -1;
	}
	struct node *prev_head = queue->head;
	*data = prev_head->data;
	queue->head = prev_head->next;

	if(!queue->head){
		queue->tail = NULL;
	}

	free(prev_head);
	queue->len--;
	return 0;
}

int queue_delete(queue_t queue, void *data){
	/* TODO Phase 1 */
	if(!queue || !data){
		return -1;
	}
	struct node *current = queue->head;
	struct node *prev = NULL;

	while(current){
		if(current->data == data){
			if(prev == NULL){
				queue->head = current->next;
				if(current == queue->tail){
					queue->tail = NULL;
				}
			}
			else{
				prev->next = current->next;
				if(current == queue->tail){
					queue->tail = prev;
				}
			}
			free(current);
			queue->len--;
			return 0;
		}
		prev = current;
		current = current->next;
	}
	return -1;
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	/* TODO Phase 1 */
	if(!queue || !func){
		return -1;
	}
	struct node *current = queue->head;
	struct node *next;

	while(current){
		next = current->next;
		func(queue, current->data);
		current = next;
	}
	return 0;
}

int queue_length(queue_t queue)
{
	/* TODO Phase 1 */
	if (!queue){
		return -1;
	}
	return queue->len;
}


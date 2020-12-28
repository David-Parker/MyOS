#include <kernel/lib/ds.h>
#include <kernel/util/debug.h>

void linked_list_init(linked_list_t* list)
{
    list->count = 0;
    list->head = NULL;
}

void linked_list_add_back(linked_list_t* list, linked_list_node_t* toAdd)
{
    kassert(list != NULL && toAdd != NULL);

    if (list->head == NULL)
    {
        linked_list_add_empty(list, toAdd);
    }
    else
    {
        linked_list_add_before(list->head, toAdd);
    }

    list->count++;
}

void linked_list_add_front(linked_list_t* list, linked_list_node_t* toAdd)
{
    kassert(list != NULL && toAdd != NULL);

    if (list->head == NULL)
    {
        linked_list_add_empty(list, toAdd);
    }
    else
    {
        linked_list_add_before(list->head, toAdd);
        list->head = toAdd;
    }

    list->count++;
}

void linked_list_remove(linked_list_t* list, linked_list_node_t* toRemove)
{
    kassert(list != NULL && toRemove != NULL);

    if (toRemove->next == toRemove)
    {
        kassert(list->count == 1 && list->head == toRemove);
        list->head = NULL;
    }
    else
    {
        toRemove->next->prev = toRemove->prev;
        toRemove->prev->next = toRemove->next;
        if (list->head == toRemove)
        {
            list->head = toRemove->next;
        }
    }

    list->count--;
}

static void linked_list_add_before(linked_list_node_t* node, linked_list_node_t* toAdd)
{
    toAdd->next = node;
    toAdd->prev = node->prev;
    node->prev->next = toAdd;
    node->prev = toAdd;
}

static void linked_list_add_empty(linked_list_t* list, linked_list_node_t* toAdd)
{
    list->head = toAdd;
    list->head->next = toAdd;
    list->head->prev = toAdd;
}

void ring_buffer_init(ring_buffer_t* ring, size_t* buffer, int buf_len, int ring_len)
{
    kassert(ring != NULL);
    kassert(buffer != NULL);
    kassert(buf_len > 0);
    kassert(buf_len >= ring_len);

    ring->buffer = buffer;
    ring->buf_len = buf_len;
    ring->ring_len = ring_len;
    ring->idx_start = 0;
    ring->idx_end = 0;
    ring->total_push = 0;
}

size_t ring_buffer_push(ring_buffer_t* ring, size_t elem)
{
    kassert(ring != NULL);

    size_t old_elem = 0;

    if (ring->total_push >= ring->buf_len)
    {
        // We are overwriting existing values
        old_elem = ring->buffer[ring->idx_end];
    }

    ring->buffer[ring->idx_end] = elem;
    ring->total_push++;
    ring->idx_end++;

    int dist = modulo(ring->idx_end - ring->idx_start, ring->buf_len);

    if (dist > ring->ring_len)
        ring->idx_start++;
    
    if (ring->idx_end == ring->buf_len)
    {
        // wrap
        ring->idx_end = 0;
    }

    if (ring->idx_start == ring->buf_len)
    {
        // wrap
        ring->idx_start = 0;
    }

    return old_elem;
}

size_t ring_buffer_get(ring_buffer_t* ring, int idx)
{
    kassert(ring != NULL);

    return ring->buffer[modulo(idx, ring->buf_len)];
}

size_t ring_buffer_get_last(ring_buffer_t* ring)
{
    kassert(ring != NULL);

    int idx = modulo(ring->idx_end - 1, ring->buf_len);

    return ring->buffer[idx];
}

int ring_buffer_next(ring_buffer_t* ring, int read_idx)
{
    kassert(read_idx <= ring->buf_len);
    kassert(read_idx >= 0);

    read_idx = modulo(read_idx + 1, ring->buf_len);

    if (read_idx == ring->idx_end)
    {
        return -1;
    }
    else
    {
        return read_idx;
    }
}

int ring_buffer_prev(ring_buffer_t* ring, int read_idx)
{
    kassert(read_idx <= ring->buf_len);
    kassert(read_idx >= 0);

    read_idx = modulo(read_idx - 1, ring->buf_len);

    if (read_idx == ring->idx_end)
    {
        return -1;
    }
    else
    {
        return read_idx;
    }
}
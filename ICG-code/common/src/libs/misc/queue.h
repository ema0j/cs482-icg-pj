// 
// Class Queue 
//

#ifndef _QUEUE_H_
#define _QUEUE_H_

// Include ---------------------------------------------------------------

#include "compilerOptions.h"
#include <assert.h>

// Declaration -----------------------------------------------------------

template<class T>
struct QueueItem {
    T               data;
    QueueItem       *next;
    QueueItem       *prev;

    QueueItem() {
        next = prev = 0;
    }
};

// implementation using sentinel
template<class T>
class Queue {

// Static data ------------------------------------------------------

// Data -------------------------------------------------------------
protected:
    QueueItem<T>     *_sentinel;

public:

// Public methods ---------------------------------------------------
	Queue();

	~Queue();

    // Add elements to head and tail
    void AddHead(const T& e);
    void AddTail(const T& e);

    // Empty
    bool IsEmpty() const;

    // Clean
    void Clear();

    // Get head and tail
    T& Head();
    T& Tail();

    // Remove head and tail
    T RemoveHead();
    T RemoveTail();

    // Item operations
    void RemoveItem(QueueItem<T>* item); // does not delete item

protected:
};

// Inline functions ------------------------------------------------------

template<class T>
inline Queue<T>::Queue() {
    _sentinel = new QueueItem<T>();
    _sentinel->next = _sentinel;
    _sentinel->prev = _sentinel;
}

template<class T>
inline Queue<T>::~Queue() {
    Clear();
}

template<class T>
inline void Queue<T>::Clear() {
    QueueItem<T>* cur = _sentinel->next;
    while(cur != _sentinel) {
        RemoveItem(cur);
        QueueItem<T>* td = cur;
        cur = cur->next;
        delete td;
    }
}

template<class T>
inline bool Queue<T>::IsEmpty() const {
    return _sentinel->next == _sentinel;
}

template<class T>
inline void Queue<T>::AddHead(const T& e) {
    QueueItem<T>* item = new QueueItem<T>();
    item->data = e;
    item->next = _sentinel->next;
    _sentinel->next->prev = item;
    _sentinel->next = item;
    item->prev = _sentinel;
}

template<class T>
inline void Queue<T>::AddTail(const T& e) {
    QueueItem<T>* item = new QueueItem<T>();
    item->data = e;
    item->prev = _sentinel->prev;
    _sentinel->prev->next = item;
    _sentinel->prev = item;
    item->next = _sentinel;
}

template<class T>
inline T& Queue<T>::Head() {
    return _sentinel->next->data;
}

template<class T>
inline T& Queue<T>::Tail() {
    return _sentinel->prev->data;
}

template<class T>
inline T Queue<T>::RemoveHead() {
    QueueItem<T>* item = _sentinel->next;
    T ret = item->data;
    RemoveItem(item);
    delete item;
    return ret;
}

template<class T>
inline T Queue<T>::RemoveTail() {
    QueueItem<T>* item = _sentinel->prev;
    T ret = item->data;
    RemoveItem(item);
    delete item;
    return ret;
}

template<class T>
inline void Queue<T>::RemoveItem(QueueItem<T>* item) {
    item->prev->next = item->next;
    item->next->prev = item->prev;
}

#endif // _QUEUE_H_

/*
* List.h
* Copyright 2022 Aleksandr Orlov
* avo2000@mail.ru
* created: 2022-10-23 23:56:48
*/

#ifndef _id_1b4b7acf_0498_41fc_a238_44a32ca6dd72
#define _id_1b4b7acf_0498_41fc_a238_44a32ca6dd72

/*
    Author: Einar Arnason
    email: einsiarna@gmail.com
    A lightweight linked list implementation,
    meant for microcontrollers.
    Usage and further info:
    https://github.com/EinarArnason
*/

#if __cplusplus <= 199711L
#define nullptr NULL
#endif

template <typename T>
class LinkedList {
 public:
  class Node {
   public:
    T item;
    Node* next;

    Node() { next = nullptr; }
    ~Node() { next = nullptr; }
  };
  
 private:
  Node* _head;
  Node* _tail;
  unsigned int count;

 public:
  LinkedList() {
    this->_head = nullptr;
    this->_tail = nullptr;
    this->count = 0;
  }

  ~LinkedList() {
    for (Node* node = _head; node != nullptr; node = _head) {
      _head = node->next;
      delete node;
    }
  }

  Node* head() { return _head; }
  Node* tail() { return _tail; }

  /*
    Push an item to the list.
    Returns false if memory is
    full, or true if the item
    was added to list.
  */
  bool push_back(T item) {

    Node* node = new Node;
    if (node == nullptr) {
      return false;
    }

    node->item = item;

    if (_head == nullptr) {
      _head = node;
      _tail = node;
      count++;

      return true;
    }

    _tail->next = node;
    _tail = node;
    count++;

    return true;
  }

  /*
    Pop the front of the list.
    Because exceptions are not
    usually implemented for
    microcontrollers, if list
    is empty, a dummy item is
    returned.
  */
  T pop_front() {
    if ((count == 0) || (_head == nullptr)) {
      return T();
    }

    Node* node = _head;
    _head = node->next;
    T item = node->item;
    delete node;
    node = nullptr;

    if (_head == nullptr) {
      _tail = nullptr;
    }

    count--;
    return item;
  }
  
  bool remove( const T& item) {
    bool rc = false;
    for ( Node *p = _head, *prev = nullptr; p != nullptr;) {
      if ( p->item == item ) {
        if ( prev == nullptr ) {
          // remove first item of the list
          _head = nullptr;
          delete p;
          p = nullptr;
        }
        else {
          // remove item in the middle or tail of the list
          prev->next = p->next;
          if ( p == _tail )
            _tail = prev;
          delete p;
          p = prev->next;
        }
        rc = true;
      }
      else {
        prev = p;
        p = p->next;
      }
    }
    return rc;
  }

  /*
    Returns true if the list
    is empty, false otherwise.
  */
  bool isEmpty() { return _head == nullptr; }

  /*
    Returns the number of items
    currently in the list.
  */
  unsigned int itemCount() { return count; }

  /*
    Returns the size of the
    list item in bytes.
  */
  unsigned int itemSize() { return sizeof(Node); }
  /*
    Get the item in the front
    of the list.
    Because exceptions are not
    usually implemented for
    microcontrollers, if list
    is empty, a dummy item is
    returned.
  */
  T getHead() {
    if ((count == 0) || (_head == nullptr)) {
      return T();
    }

    T item = _head->item;
    return item;
  }

  /*
    Get the item in the back
    of the list.
    Because exceptions are not
    usually implemented for
    microcontrollers, if list
    is empty, a dummy item is
    returned.
  */
  T getTail() {
    if ((count == 0) || (_head == nullptr)) {
      return T();
    }

    T item = _tail->item;
    return item;
  }

  T* getHeadPtr() {
    if ((count == 0) || (_head == nullptr)) {
      return nullptr;
    }

    return &(_head->item);
  }

  T* getTailPtr() {
    if ((count == 0) || (_head == nullptr)) {
      return nullptr;
    }

    return &(_tail->item);
  }

  /*
    Depricated functions
  */

  // Depricated, use getHead() instead
  T front() { return getHead(); }
  // Depricated, use itemCount() instead
  unsigned int item_count() { return itemCount(); }
  // Depricated, use itemSize() instead
  unsigned int item_size() { return itemSize(); }
};

#endif /* _id_1b4b7acf_0498_41fc_a238_44a32ca6dd72 */

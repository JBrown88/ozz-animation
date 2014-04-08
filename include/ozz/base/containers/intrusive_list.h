//============================================================================//
//                                                                            //
// ozz-animation, 3d skeletal animation libraries and tools.                  //
// https://code.google.com/p/ozz-animation/                                   //
//                                                                            //
//----------------------------------------------------------------------------//
//                                                                            //
// Copyright (c) 2012-2014 Guillaume Blanc                                    //
//                                                                            //
// This software is provided 'as-is', without any express or implied          //
// warranty. In no event will the authors be held liable for any damages      //
// arising from the use of this software.                                     //
//                                                                            //
// Permission is granted to anyone to use this software for any purpose,      //
// including commercial applications, and to alter it and redistribute it     //
// freely, subject to the following restrictions:                             //
//                                                                            //
// 1. The origin of this software must not be misrepresented; you must not    //
// claim that you wrote the original software. If you use this software       //
// in a product, an acknowledgment in the product documentation would be      //
// appreciated but is not required.                                           //
//                                                                            //
// 2. Altered source versions must be plainly marked as such, and must not be //
// misrepresented as being the original software.                             //
//                                                                            //
// 3. This notice may not be removed or altered from any source               //
// distribution.                                                              //
//                                                                            //
//============================================================================//

#ifndef OZZ_OZZ_BASE_CONTAINERS_INTRUSIVE_LIST_H_
#define OZZ_OZZ_BASE_CONTAINERS_INTRUSIVE_LIST_H_

#include "ozz/base/platform.h"

#include <cstddef>
#include <cassert>
#include <iterator>

// REDEBUG should be used to test IntrusiveList internal integrity.
// It can slow down performance significantly.

// Automatically enables REDEBUGing if OZZ_HAS_REDEBUG_ALL is defined.
#if OZZ_HAS_REDEBUG_ALL
#define OZZ_HAS_CONTAINER_LIST_REDEBUG 1
#endif  // OZZ_HAS_REDEBUG_ALL

#if OZZ_HAS_CONTAINER_LIST_REDEBUG
#define OZZ_IF_HAS_LIST_REDEBUG(...) __VA_ARGS__
#else  // OZZ_HAS_CONTAINER_LIST_REDEBUG
#define OZZ_IF_HAS_LIST_REDEBUG(...)
#endif  // OZZ_HAS_CONTAINER_LIST_REDEBUG

namespace ozz {
namespace containers {

// Forward declares the IntrusiveList. See class definition for the detailed
// documentation.
template<typename, typename> class IntrusiveList;

// Enters the internal namespace that encloses private implementation details.
namespace internal {

// Forward declares IntrusiveNodeList.
class IntrusiveNodeList;

// Defines the node class that's linked by the IntrusiveListImpl.
// This is an internal class as the user's node class must inherit from
// IntrusiveList<>::Hook (which inherit from Node).
class Node {
 public:

  // Unlinks *this node from its current list.
  // This function must be called on a linked node.
  void unlink();

  // Test if *this node is linked in a list.
  // This function is not able to test for a particular list.
  bool is_linked() const {
    OZZ_IF_HAS_LIST_REDEBUG(assert(redebug_check_integrity());)
    return prev_ != this;
  }

#ifndef NDEBUG
  // Test if *this node is linked in _list.
  // This function is only available for debug purpose.
  // It tests the same thing is_linked does, but allows to test
  // which particular list links *this node. Will return false if the node is
  // linked in another list.
  bool debug_is_linked_in(const IntrusiveNodeList& _list) const {
    OZZ_IF_HAS_LIST_REDEBUG(assert(redebug_check_integrity());)
    return &_list == list_;
  }
#endif  // NDEBUG

 protected:

  // Constructs an unlinked node.
  Node()
    OZZ_IF_DEBUG(: list_(NULL)) {
    prev_ = this;
    next_ = this;
  }

  // Destructs the node, no check is done as they depend on the LinkMode.
  ~Node() {
  }

 private:

  // The node class can be publicly used by the internal layers.
  friend class IntrusiveNodeList;
  template <typename, typename> friend class IntrusiveListIterator;

  // Pushes (inserts) *this node before _node.
  // *this node must be unlinked and _node must be linked.
  void insert(Node* _where);

#if OZZ_HAS_CONTAINER_LIST_REDEBUG
  // Internal function that tests for node integrity.
  bool redebug_check_integrity() const;
#endif  // OZZ_HAS_CONTAINER_LIST_REDEBUG

#ifndef NDEBUG
  // Tests if *this node is the end node of a list.
  // This function is only available for debug purpose.
  // end_ is the first member of the list, which allows to compare *this
  // address with list_.
  bool debug_is_end_node() const {
    return list_ == reinterpret_cast<IntrusiveNodeList const*>(this);
  }
#endif  // NDEBUG

  // Disallow copy and assignation
  Node(const Node&);
  void operator = (const Node&);

  // prev_ and next_ points to *this node if *this is NOT linked.
  Node* prev_;  // Pointer to the previous node in the list.
  Node* next_;  // Pointer to the next node in the list.

  // Pointer to the list_ that references this node, used for debugging only.
  OZZ_IF_DEBUG(IntrusiveNodeList* list_;)
};

// Implements non template algorithms of the IntrusiveList class.
// This class is based on Node type only, but still implement IntrusiveList
// public algorithms.
class IntrusiveNodeList {
 public:

  // Constructs an empty list.
  IntrusiveNodeList() {
    OZZ_IF_DEBUG(end_.list_ = this;)
  }

  // Destructs a list. Assertions are done in the templates class as they
  // depend on the LinkMode argument.
  ~IntrusiveNodeList() {
  }

  // Removes all the elements from the list iteratively.
  // This function has an O(n) complexity.
  void clear();

  // Returns true if the list contains no element.
  bool empty() const { return end_.next_ == &end_; }

  // Reverses the order of elements in the list.
  // All iterators remain valid and continue to point to the same elements.
  // This function is linear time.
  void reverse();

  // Swaps the contents of two lists.
  // This function as O(1) complexity (except in debug builds) as opposed to
  // the std::list implementation.
  void swap(IntrusiveNodeList& _list);

  // Returns the size of the list.
  // This function is NOT constant time but linear O(n). If you wish to test
  // whether a list is empty, you should write l.empty() rather than
  // l.size() == 0.
  std::size_t size() const;

 protected:

  // The type used to counts the number of elements in a list.
  typedef std::size_t size_type;

  // Returns the first node of the list if it is not empty, end node otherwise.
  Node& begin_node() { return *end_.next_; }
  const Node& begin_node() const { return *end_.next_; }

  // Returns the last node of the list if it is not empty, end node otherwise.
  Node& last_node() { return *end_.prev_; }
  const Node& last_node() const { return *end_.prev_; }

  // Returns the end node of the list.
  Node& end_node() { return end_; }
  const Node& end_node() const { return end_; }

  // Links _node at the front of the list, ie: just after end node.
  void link_front(Node* _node) { _node->insert(end_.next_); }

  // Links _node at the back of the list, ie: just before end node.
  void link_back(Node* _node) { _node->insert(&end_); }

  // Inserts _node before _where.
  void insert_(Node* _node, Node* _where) { _node->insert(_where); }

#ifndef NDEBUG
  // Tests if the range [_begin, end_[ is valid, ie: _begin <= _end.
  // The range invalidity is triggered if _begin and _end are not in the same
  // list, are not linked, or if the end node of the list is traversed while
  // iterating from _begin to _end. Unfortunately it makes the algorithm O(n).
  bool debug_is_range_valid(const Node& _begin, const Node& _end) {
    if (!_begin.debug_is_linked_in(*this) || !_end.debug_is_linked_in(*this)) {
      return false;
    }
    const Node* node = &_begin;
    while (node != &_end) {
      if (node == &_begin.list_->end_) {
        return false;
      }
      node = node->next_;
    }
    return true;
  }
#endif  // NDEBUG

  // Implements splice algorithm.
  void splice_(Node* _where, Node* _first, Node* _end);

  // Implements erase algorithm.
  void erase_(Node* _begin, Node* _end);

  // Implements equality test using _pred functor.
  template<typename _Pred>
  bool is_equal_(IntrusiveNodeList const& _list, _Pred _pred) const;

  // Implements "less than" test using _pred functor.
  template<typename _Pred>
  bool is_less_(IntrusiveNodeList const& _list, _Pred _pred) const;

  // Implements merge algorithm using _pred functor.
  template<typename _Pred>
  void merge_(IntrusiveNodeList* _list, _Pred _pred);

  // Implements sort algorithm using _pred functor.
  template<typename _Pred>
  void sort_(_Pred _pred);

  // Implements merge algorithm using _pred functor.
  template<typename _Pred>
  bool is_ordered_(_Pred _pred) const;

  // Implements remove_if algorithm using _pred functor.
  template<typename _Pred>
  void remove_if_(_Pred _pred);

#if OZZ_HAS_CONTAINER_LIST_REDEBUG
  // Internal function that tests for list integrity.
  // Iterates through all nodes and test their integrity as well.
  bool redebug_check_integrity() const;
#endif  // OZZ_HAS_CONTAINER_LIST_REDEBUG

 private:

  // Base iterator can access end_ for debug purpose
  template<typename, typename> friend class IntrusiveListIterator;

  // The node that is used to link the first and last elements of the list,
  // in order to create a circular list.
  // This node is the one returned by the end() function.
  Node end_;
};

// Declares the trait configuration of mutable iterators.
template <typename _List>
struct MutableCfg {
  typedef typename _List::pointer pointer;
  typedef typename _List::reference reference;
  typedef Node ListNode;
  typedef typename _List::Hook Hook;
  enum { kReverse = 0 };
};

// Declares the trait configuration of const iterators.
template <typename _List>
struct ConstCfg {
  typedef typename _List::const_pointer pointer;
  typedef typename _List::const_reference reference;
  typedef const Node ListNode;
  typedef const typename _List::Hook Hook;
  enum { kReverse = 0 };
};

// Declares the trait configuration of mutable reverse iterators.
template <typename _List>
struct MutableReverseCfg {
  typedef typename _List::pointer pointer;
  typedef typename _List::reference reference;
  typedef Node ListNode;
  typedef typename _List::Hook Hook;
  enum { kReverse = 1 };
};

// Declares the trait configuration of const reverse iterators.
template <typename _List>
struct ConstReverseCfg {
  typedef typename _List::const_pointer pointer;
  typedef typename _List::const_reference reference;
  typedef const Node ListNode;
  typedef const typename _List::Hook Hook;
  enum { kReverse = 1 };
};

// Implements the IntrusiveList bidirectional iterator.
// The _Config template argument is a trait that configures the iterator for
// const/mutable and forward/reverse iteration orders.
template <typename _List, typename _Config>
class IntrusiveListIterator {
 public:

  // Defines iterator types as required by std::
  typedef std::bidirectional_iterator_tag iterator_category;
  typedef typename _List::value_type value_type;
  typedef typename _List::difference_type difference_type;
  typedef typename _Config::pointer pointer;
  typedef typename _Config::reference reference;
  typedef typename _Config::ListNode ListNode;

  // Constructs an iterator pointing _node.
  // _node can be NULL which creates a default un-dereferencable iterator.
  explicit IntrusiveListIterator(ListNode* _node = NULL) :
    node_(_node) {
    assert((!_node || _node->list_) &&
           "Cannot build an iterator from a node that's unlinked");
  }

  ~IntrusiveListIterator() {
  }

  explicit IntrusiveListIterator(const IntrusiveListIterator& _it) :
    node_(_it.node_) {
  }

  // Constructs an iterator from an iterator with a different config, like
  // forward/reverse, const/mutable variations.
  // Disallowed conversions, like const to mutable, do not compile.
  template<typename _OConfig>
  IntrusiveListIterator(IntrusiveListIterator<_List, _OConfig> const& _it) :
    node_(_it.node_) {
  }

  // Compares two iterators with different configurations.
  template<typename _OConfig>
  bool operator == (IntrusiveListIterator<_List, _OConfig> const& _it) const {
    assert(node_ && _it.node_ && node_->list_ == _it.node_->list_ &&
           "List iterators incompatible");
    return node_ == _it.node_;
  }

  // Compares two iterators with different configurations.
  template<typename _OConfig>
  bool operator != (IntrusiveListIterator<_List, _OConfig> const& _it) const {
    assert(node_ && _it.node_ && node_->list_ == _it.node_->list_ &&
           "List iterators incompatible");
    return node_ != _it.node_;
  }

  // Dereferences the object pointed by *this iterator.
  // *this must be a valid iterator: initialized and not end().
  reference operator* () const {
    assert(node_ && !node_->debug_is_end_node() &&
           "List iterator not dereferencable");
    return static_cast<reference>(
      static_cast<typename _Config::Hook&>(*node_));
  }

  // Pre-increments iterator to the next object. The direction depends on
  // iterator configuration (forward or reverse).
  // *this must be a valid iterator: initialized and not end().
  inline IntrusiveListIterator operator ++() {
    assert(node_ && !node_->debug_is_end_node() &&
           "List iterator is already on list boundaries");
    node_ = _Config::kReverse? node_->prev_:node_->next_;
    return *this;
  }

  // Pre-decrements iterator to the next object. The direction depends on
  // iterator configuration (forward or reverse).
  // *this must be a valid iterator: initialized and not end().
  inline IntrusiveListIterator operator --() {
    assert(node_ &&
           node_ != (_Config::kReverse?node_->list_->end_.prev_:
                                       node_->list_->end_.next_) &&
            "List iterator is already on list boundaries");
    node_ = _Config::kReverse? node_->next_:node_->prev_;
    return *this;
  }

  // Post-increments iterator to the next object. The direction depends on
  // iterator configuration (forward or reverse).
  // *this must be a valid iterator: initialized and not end().
  // DO NOT use the post-increment function if the returned value is ignored.
  IntrusiveListIterator operator ++(int) {  // NOLINT unnamed argument
    const IntrusiveListIterator old(*this);
    ++(*this);
    return old;
  }

  // Post-decrements iterator to the next object. The direction depends on
  // iterator configuration (forward or reverse).
  // *this must be a valid iterator: initialized and not end().
  // DO NOT use the post-decrement function if the returned value is ignored.
  IntrusiveListIterator operator --(int) {  // NOLINT unnamed argument
    const IntrusiveListIterator old(*this);
    --(*this);
    return old;
  }

 private:

  // Grants the right to IntrusiveList to access node() function.
  template<typename, typename> friend class ozz::containers::IntrusiveList;

  // Get the node currently pointed by the iterator.
  // *this iterator must be initialized, but can point a list end node.
  Node& node() const {
    assert(node_ && "Iterator isn't initialized");
    return *node_;
  }

  // Other iterator specialization can access each other
  template<typename, typename> friend class IntrusiveListIterator;

  // The list Node designated by *this iterator, which can be the end Node of a
  // list. A default iterator has a NULL designated Node.
  ListNode* node_;
};
}  // internal

// Enumerate all the link modes that can be used.
struct LinkMode {
  enum Value {
    kSafe,  // RECOMMENDED default mode.
            // Hooks and lists can not be deleted while they are linked.
            // Programming errors that can corrupt the list are detected:
            // - pushing a hook twice in a list.
            // - popping an unlinked hook.
            // - deleting a linked hook.
            // - deleting a list that still contains hooks.
            // This is the default and preferred mode as the rules above
            // give a lot of guarantees to the user, often even about its own
            // algorithm consistency.
    kAuto,  // Does the same checks as kSafe. Automatically unlink all hooks
            // when the list is destroyed. Automatically unlink a hook when it
            // is destroyed also. BE CAREFUL that the containers can silently
            // be modified (without any containers function call), which can
            // easily lead to thread unsafe code.
    kUnsafe,  // NOT RECOMMENDED.
              // Behaves exactly as kSafe mode, but does not assert for
              // deletion of a linked hook or a non empty list.
              // This mode is unsafe as deleting a linked hook or a non empty
              // list leads to corrupt data (dangling pointers). This is useful
              // when the user knows that all the data (hooks + list) are going
              // to be erased and that neither the list or any hook of the list
              // will be accessed. This mode is NOT RECOMMENDED, but still
              // allows to remove a O(n) algorithm (release all hooks) in some
              // rare cases where a list is not by nature empty (or relatively
              // small) at destruction time: .
  };
};

// Holds the options for the IntrusiveList containers.
// _Unique is never used in the code, but differentiates the type of multiple
// IntrusiveList at compile time. This is useful in order to store the same
// hook in more than one list (differentiated by their type thus) at the same
// time.
// _LinkMode is a value of LinkMode enumeration.
template<LinkMode::Value _LinkMode = LinkMode::kSafe, int _Unique = 0>
struct Option {
  static const LinkMode::Value kLinkMode = _LinkMode;
};

template <typename _Ty, typename _Option = Option<> >
class IntrusiveList : public internal::IntrusiveNodeList {
 public:

  class Hook : public internal::Node {
   protected:
    Hook() {
    }
    ~Hook() {
      if (void(0), _Option::kLinkMode == LinkMode::kAuto && is_linked()) {
        unlink();
      }
      assert((_Option::kLinkMode == LinkMode::kUnsafe || !is_linked()) &&
             "Node is still linked");
    }
   private:
    Hook(const Hook&);
    void operator = (const Hook&);
  };

  typedef _Ty value_type;  // The type of object, T, stored in the list.
  typedef _Ty* pointer;  // Pointer to T.
  typedef _Ty const* const_pointer;  // Const pointer to T.
  typedef _Ty& reference;  // Reference to T.
  typedef _Ty const& const_reference;  // Const reference to T.
  typedef internal::IntrusiveNodeList::size_type size_type;  // A type that
                                // counts the number of elements in a list.
  typedef std::ptrdiff_t difference_type;  // A type that provides the
                                           // difference between two iterators.

  typedef internal::IntrusiveListIterator< IntrusiveList,
                                  internal::MutableCfg<IntrusiveList> >
            iterator;  // Iterator used to iterate through a list;
  typedef internal::IntrusiveListIterator< IntrusiveList,
                                  internal::ConstCfg<IntrusiveList> >
            const_iterator;  // Const iterator used to iterate through a list.
  typedef internal::IntrusiveListIterator< IntrusiveList,
                                  internal::MutableReverseCfg<IntrusiveList> >
            reverse_iterator;  // Iterator used to iterate backwards through
                               // a list.
  typedef internal::IntrusiveListIterator< IntrusiveList,
                                  internal::ConstReverseCfg<IntrusiveList> >
            const_reverse_iterator;  // Const iterator used to iterate
                                     // backwards through a list.

  // Constructs an empty list.
  IntrusiveList() {
  }

  // Destructs a list that must be empty if link mode is not kUnsafe, otherwise
  // an assertion is thrown.
  ~IntrusiveList() {
    if (void(0), _Option::kLinkMode == LinkMode::kAuto) {
      clear();
    }
    assert(_Option::kLinkMode == LinkMode::kUnsafe || empty());
  }

  // Inserts an unlinked element at the beginning of the list.
  void push_front(reference _val) {
    link_front(static_cast<Hook*>(&_val));
  }

  // Inserts an unlinked element at the end of the list.
  void push_back(reference _val) {
    link_back(static_cast<Hook*>(&_val));
  }

  // Removes the first element of the list and returns its reference.
  // Compared to the std::list, this function can return the reference as
  // pop_front does not delete the element.
  // This function asserts if list is empty.
  reference pop_front() {
    assert(!empty() && "Invalid function on an empty list");
    internal::Node& node = begin_node();
    node.unlink();
    OZZ_IF_HAS_LIST_REDEBUG(assert(redebug_check_integrity());)
    return static_cast<reference>(static_cast<Hook&>(node));
  }

  // Removes the last element of the list and returns its reference.
  // Compared to the std::list, this function can return the reference as
  // pop_back does not delete the element.
  // This function asserts if list is empty.
  reference pop_back() {
    assert(!empty() && "Invalid function on an empty list");
    internal::Node& node = last_node();
    node.unlink();
    OZZ_IF_HAS_LIST_REDEBUG(assert(redebug_check_integrity());)
    return static_cast<reference>(static_cast<Hook&>(node));
  }

  // Returns the a reference to the first element.
  // This function asserts if list is empty.
  reference front() {
    assert(!empty() && "Invalid function on an empty list");
    return static_cast<reference>(static_cast<Hook&>(begin_node()));
  }

  // Returns the a const reference to the first element.
  // This function asserts if list is empty.
  const_reference front() const {
    assert(!empty() && "Invalid function on an empty list");
    return static_cast<const_reference>(
      static_cast<const Hook&>(begin_node()));
  }

  // Returns the a reference to the last element.
  // This function asserts if list is empty.
  reference back() {
    assert(!empty() && "Invalid function on an empty list");
    return static_cast<reference>(static_cast<Hook&>(last_node()));
  }

  // Returns the a const reference to the last element.
  // This function asserts if list is empty.
  const_reference back() const {
    assert(!empty() && "Invalid function on an empty list");
    return static_cast<const_reference>(static_cast<const Hook&>(last_node()));
  }

  // Returns an iterator pointing to the beginning of the list.
  iterator begin() { return iterator(&begin_node()); }

  // Returns a const_iterator pointing to the beginning of the list.
  const_iterator begin() const { return const_iterator(&begin_node()); }

  // Returns an iterator pointing to the end of the list.
  // The returned iterator can not be dereferenced.
  iterator end() { return iterator(&end_node()); }

  // Returns a const_iterator pointing to the end of the list.
  // The returned iterator can not be dereferenced.
  const_iterator end() const { return const_iterator(&end_node()); }

  // Returns a reverse_iterator pointing to the beginning of the reversed list.
  // The returned iterator can not be dereferenced.
  reverse_iterator rbegin() { return reverse_iterator(&last_node()); }

  // Returns a const_reverse_iterator pointing to the beginning of the reversed
  // list. The returned iterator can not be dereferenced.
  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(&last_node());
  }

  // Returns a reverse_iterator pointing to the end of the reversed list.
  reverse_iterator rend() { return reverse_iterator(&end_node()); }

  // Returns a const_reverse_iterator pointing to the end of the reversed list.
  const_reverse_iterator rend() const {
    return const_reverse_iterator(&end_node());
  }

  // Removes _val element from the list with a O(1) complexity.
  // The relative order of elements is unchanged, and iterators to elements
  // that are not removed remain valid.
  // This functions asserts if _val is not element of the list.
  void remove(reference _val) {
    Hook& hook = static_cast<Hook&>(_val);
    assert(hook.debug_is_linked_in(*this) && "The node is linked by this list");
    hook.unlink();
    OZZ_IF_HAS_LIST_REDEBUG(assert(redebug_check_integrity());)
  }

  // Removes all elements such that _pred() is true, with an O(n) complexity.
  // The relative order of elements that are not removed is unchanged.
  // Iterators to elements that are not removed remain valid.
  template <typename _Pred>
  void remove_if(_Pred _pred) {
    remove_if_(UnnaryPredFw<_Pred>(_pred));
  }

  // Erases element at _where and returns an iterator that designates the first
  // element remaining beyond the element removed.
  // _where must be a valid iterator.
  // ::remove should be preferred as it avoid creating and returning an
  // iterator.
  iterator erase(iterator _where) {
    internal::Node& where_node = _where.node();
    assert(where_node.debug_is_linked_in(*this) &&
           "The node is linked by this list");
    ++_where;  // Offset the iterator to return before modifying the list
    where_node.unlink();
    return _where;
  }

  // Erases elements in range [_begin, _end[, and returns an iterator that
  // designates the first element remaining beyond the element removed.
  // _first and _end iterators must be a valid.
  iterator erase(iterator const& _begin, iterator const& _end) {
    internal::Node& begin_node = _begin.node();
    internal::Node& end_node = _end.node();
    erase_(&begin_node, &end_node);
    return _end;  // _end is still a valid iterator
  }

  // Insert _val before _where.
  // Compared to std::list, this function does not return an iterator as
  // IntrusiveLisrt iterators can be constructed in O(1) directly from _val.
  void insert(iterator const& _where, reference _val) {
    // Dereference iterator to ensure its validity
    insert_(static_cast<Hook*>(&_val), &_where.node());
    OZZ_IF_HAS_LIST_REDEBUG(assert(redebug_check_integrity());)
  }

  // All of the elements of _list are inserted before _where and removed from
  // _list.
  // This function is constant time.
  void splice(iterator _where, IntrusiveList& _list) {  // NOLINT conforms with std::list API
    if (this != &_list && !_list.empty()) {
      splice_(&_where.node(), &_list.begin_node(), &_list.end_node());
    }
  }

  // The elements _what from _list is inserted before _where and removed from
  // _list.
  // This function is constant time.
  void splice(iterator _where, IntrusiveList& _list,  // NOLINT conforms with std::list API
              iterator _what) {
    reference val = static_cast<reference>(static_cast<Hook&>(_what.node()));
    _list.remove(val);
    insert(_where, val);
  }

  // All of the elements in the range [_begin, _end[ are inserted before
  // _where and removed from _list.
  // This function is constant time.
  void splice(iterator _where, IntrusiveList& _list,  // NOLINT conforms with std::list API
              iterator _begin, iterator _end) {
    internal::Node* where_node = &_where.node();
    internal::Node* begin_node = &_begin.node();
    internal::Node* end_node = &_end.node();
    if (begin_node != end_node && (this != &_list || where_node != end_node)) {
      splice_(where_node, begin_node, end_node);
    }
  }

  // Removes all of _list's elements and inserts them in order into *this.
  // _Pred must be a comparison function that induces a strict weak ordering
  // (as defined in the LessThan Comparable requirements) on objects of type
  // _Ty, and both *this and _list must be sorted according to that ordering.
  // The merge is stable; that is, if an element from *this is equivalent to
  // one from x, then the element from *this will precede the one from x.
  // This function is linear time and performs at most:
  // size() + _list.size() - 1 applications of _Pred.
  template<typename _Pred>
  void merge(IntrusiveList& _list, _Pred _pred) {  // NOLINT conforms with std::list API
    merge_(&_list, BinaryPredFw<_Pred>(_pred));
  }

  // Removes all of _list's elements and inserts them in order into *this.
  // Both *this and x must be sorted according to operator<.
  // The merge is stable; that is, if an element from *this is equivalent to
  // one from x, then the element from *this will precede the one from x.
  // All iterators to elements in *this and x remain valid.
  // This function is linear time and performs at most
  // size() + _list.size() - 1 comparisons.
  void merge(IntrusiveList& _list) {  // NOLINT conforms with std::list API
    merge_(&_list, LessTester());
  }

  // Sorts the list *this according to Comp.
  // Comp must be a comparison function that induces a strict weak ordering
  // (as defined in the LessThan Comparable requirements on objects of type T.
  // The sort is stable, that is, the relative order of equivalent elements is
  // preserved.
  // The number of comparisons is approximately n.log(n).
  template<class _Pred>
  void sort(_Pred _pred) { sort_(BinaryPredFw<_Pred>(_pred)); }

  // Sorts *this according to operator<.
  // The sort is stable, that is, the relative order of equivalent elements is
  // preserved.
  // The number of comparisons is approximately n.log(n).
  void sort() { sort_(LessTester()); }

  // Tests two lists for equality according to operator==.
  bool operator == (IntrusiveList const& _list) const {
    return is_equal_(_list, EqualTester());
  }

  // Tests two lists for inequality according to operator==.
  bool operator != (IntrusiveList const& _list) const {
    return !(*this == _list);
  }

  // Lexicographical "less" comparison according to operator<.
  bool operator < (IntrusiveList const& _list) const {
    return is_less_(_list, LessTester());
  }

  // Lexicographical "less or equal" comparison according to operator<.
  bool operator <= (IntrusiveList const& _list) const {
    return !(_list < *this);
  }

  // Lexicographical "greater" comparison according to operator<.
  bool operator > (IntrusiveList const& _list) const {
    return _list < *this;
  }

  // Lexicographical "greater or equal" comparison according to operator<.
  bool operator >= (IntrusiveList const& _list) const {
    return !(*this < _list);
  }

 private:

  // Internal function that tests the order of the list according to _Pred.
  template<typename _Pred>
  bool is_ordered(_Pred _pred) const {
    return is_ordered_(BinaryPredFw<_Pred>(_pred));
  }

  // Helper binary functor that converts the nodes in argument to
  // a value_type that are given as arguments to _pred.
  template<typename _Pred>
  struct BinaryPredFw {
    explicit BinaryPredFw(_Pred _pred) : pred_(_pred) {
    }
    bool operator()(const internal::Node& _left,
                    const internal::Node& _right) {
      const_reference left = static_cast<const_reference>(
        static_cast<const Hook&>(_left));
      const_reference right = static_cast<const_reference>(
        static_cast<const Hook&>(_right));
      return pred_(left, right);
    }
    _Pred pred_;
  };

  // Helper unary functor that converts the node in argument to
  // a value_type that is given as an argument to _pred.
  template<typename _Pred>
  struct UnnaryPredFw {
    explicit UnnaryPredFw(_Pred _pred) : pred_(_pred) {
    }
    bool operator()(const internal::Node& _node) {
      const_reference val = static_cast<const_reference>(
        static_cast<const Hook&>(_node));
      return pred_(val);
    }
    _Pred pred_;
  };

  // Compares 2 nodes according to the value_type operator ==.
  struct EqualTester {
    bool operator()(const internal::Node& _left,
                    const internal::Node& _right) {
      const_reference left = static_cast<const_reference>(
         static_cast<const Hook&>(_left));
      const_reference right = static_cast<const_reference>(
        static_cast<const Hook&>(_right));
      return left == right;
    }
  };

  // Compares 2 nodes according to the value_type operator <.
  struct LessTester {
    bool operator()(const internal::Node& _left,
                    const internal::Node& _right) {
      const_reference left = static_cast<const_reference>(
         static_cast<const Hook&>(_left));
      const_reference right = static_cast<const_reference>(
        static_cast<const Hook&>(_right));
      return left < right;
    }
  };

  // Disallow copy and assignment
  IntrusiveList(const IntrusiveList&);
  void operator =(const IntrusiveList&);
};

namespace internal {

// Connect the next and previous nodes together, resets internal linked state.
inline void Node::unlink() {
  assert(is_linked() && "This node is not linked");
  assert(!debug_is_end_node() && "The end_ node cannot be unlinked");

  next_->prev_ = prev_;  // Reconnect prev and next nodes
  prev_->next_ = next_;

  // Reset this node to the NOT linked state
  prev_ = this;
  next_ = this;

  OZZ_IF_DEBUG(list_ = NULL;)
  OZZ_IF_HAS_LIST_REDEBUG(assert(redebug_check_integrity());)
}

inline void Node::insert(Node* _where) {
  assert(!is_linked() && _where->list_ &&  // Cannot test _where->is_linked
                                            // as end_ would return false.
         "*this node must be unlinked and _node must be linked");
  assert(!debug_is_end_node() && "The end_ node cannot be linked");

  prev_ = _where->prev_;  // Connect the previous of *this node
  _where->prev_->next_ = this;

  next_ = _where;  // Connect the next of *this node
  _where->prev_ = this;

  OZZ_IF_DEBUG(list_ = _where->list_;)
  OZZ_IF_HAS_LIST_REDEBUG(assert(redebug_check_integrity());)
}

#if OZZ_HAS_CONTAINER_LIST_REDEBUG
// Internal function that tests for node integrity.
inline bool Node::redebug_check_integrity() const {
  // Both should be equal to this, or none.
  // If the node is linked, then list_ member must NOT be NULL.
  return  (prev_ != this && next_ != this OZZ_IF_DEBUG(&& list_ != NULL)) ||
          (prev_ == this && next_ == this OZZ_IF_DEBUG(&& list_ == NULL));
}
#endif  // OZZ_HAS_CONTAINER_LIST_REDEBUG

// Iterates through all elements to unlink them from the list.
inline void IntrusiveNodeList::clear() {
  while (end_.next_ != &end_) {
    end_.next_->unlink();
  }
}

// Iterates through all elements in range [_begin, _end[ to unlink them from
// the list.
inline void IntrusiveNodeList::erase_(Node* _begin, Node* _end) {
  assert(debug_is_range_valid(*_begin, *_end) && "Invalid iterator range");
  while (_begin != _end) {
    internal::Node* next_node = _begin->next_;
    _begin->unlink();
    _begin = next_node;
  }
  OZZ_IF_HAS_LIST_REDEBUG(assert(redebug_check_integrity());)
}

// Loops and inserts the first element in front of the original last one,
// until the last one is reached.
inline void IntrusiveNodeList::reverse() {
  Node* const last = end_.prev_;
  while (end_.next_ != last) {
    Node* node = end_.next_;
    node->unlink();
    node->insert(last->next_);
  }
  OZZ_IF_HAS_LIST_REDEBUG(assert(redebug_check_integrity());)
}

// Loops and counts the number of elements, excluding the end_ node.
inline std::size_t IntrusiveNodeList::size() const {
  std::size_t size = 0;
  for (const Node* node = end_.next_;
       node != &end_;
       node = node->next_, ++size) {
  }
  return size;
}

// Takes advantage of the intrusive property to swap end_ nodes without
// iterating through all nodes.
// This makes this implementation O(1) rather than O(n).
// In debug build though, every node between of the two lists must be traversed
// to reset their list_ member.
inline void IntrusiveNodeList::swap(IntrusiveNodeList& _list) {
  // Don't use std::swap to avoid including <algorithm> in a h file.
  // Also std::swap does a branch for nothing when dealing with pointers.
#define SWAP_PTR_(_a, _b) {\
  Node* temp = _a;\
  _a = _b;\
  _b = temp;\
  }

  SWAP_PTR_(_list.end_.prev_->next_, end_.prev_->next_);
  SWAP_PTR_(_list.end_.prev_, end_.prev_);
  SWAP_PTR_(_list.end_.next_->prev_, end_.next_->prev_);
  SWAP_PTR_(_list.end_.next_, end_.next_);
#undef SWAP_PTR_

#ifndef NDEBUG
  // Reset node internal list_ pointer
  Node* node = end_.next_;
  while (node != &end_) {
    node->list_ = this;
    node = node->next_;
  }
  node = _list.end_.next_;
  while (node != &_list.end_) {
    node->list_ = &_list;
    node = node->next_;
}
#endif  // NDEBUG

  OZZ_IF_HAS_LIST_REDEBUG(assert(redebug_check_integrity());)
  OZZ_IF_HAS_LIST_REDEBUG(assert(_list.redebug_check_integrity());)
}

#if OZZ_HAS_CONTAINER_LIST_REDEBUG
// Walk through the list to check that its a finite loop.
// Also tests every node's integrity.
inline bool IntrusiveNodeList::redebug_check_integrity() const {
  const Node* node = end_.next_;
  while (node != &end_) {
    if (!node->redebug_check_integrity()) {
      return false;
    }
    node = node->next_;
  }
  return true;
}
#endif  // OZZ_HAS_CONTAINER_LIST_REDEBUG

// Takes advantage of the intrusive property to splice nodes without iterating.
// This makes this implementation O(1) rather than O(n).
// In debug build though, every node between _first and _end must be traversed
// to reset their list_ member.
inline void IntrusiveNodeList::splice_(Node* _where,
                                       Node* _first, Node* _end) {
  assert(_where->list_ == this && "_where is not a member of *this list");
  assert(_first->list_ &&
         _first->list_->debug_is_range_valid(*_first, *_end) &&
         "Invalid iterator range");
  assert(_first != _end);

  // Keep a pointer to the last node as _end->prev_ is modified early
  Node* last = _end->prev_;

  // De-link _first and last from its original list
  _first->prev_->next_ = _end;
  _end->prev_ = _first->prev_;

  // Re-link _first
  _first->prev_ = _where->prev_;
  _where->prev_->next_ = _first;

  // Re-link _end
  _where->prev_ = last;
  last->next_ = _where;

#ifndef NDEBUG
  // Reset node internal list_ pointer, for all the inserted nodes
  Node* node = _first;
  while (node != _where) {
    node->list_ = this;
    node = node->next_;
  }
#endif  // NDEBUG

  OZZ_IF_HAS_LIST_REDEBUG(assert(redebug_check_integrity());)
  OZZ_IF_HAS_LIST_REDEBUG(assert(_end->list_->redebug_check_integrity());)
}

template<typename _Pred>
inline bool IntrusiveNodeList::is_equal_(IntrusiveNodeList const& _list,
                                         _Pred _pred) const {
  const internal::Node* left_node = end_.next_;
  const internal::Node* right_node = _list.end_.next_;
  while (left_node != &end_ && right_node != &_list.end_) {
    if (!_pred(*left_node, *right_node)) {
      return false;
    }
    left_node = left_node->next_;
    right_node = right_node->next_;
  }
  // Finally returns true if the two lists have the same sizes
  return left_node == &end_ && right_node == &_list.end_;
}

template<typename _Pred>
inline bool IntrusiveNodeList::is_less_(IntrusiveNodeList const& _list,
                                        _Pred _pred) const {
  const internal::Node* left_node = end_.next_;
  const internal::Node* right_node = _list.end_.next_;
  while (left_node != &end_ && right_node != &_list.end_) {
    if (_pred(*left_node, *right_node)) {
      return true;
    } else if (_pred(*right_node, *left_node)) {
      return false;
    }
    left_node = left_node->next_;
    right_node = right_node->next_;
  }
  // Finally returns true if "this" list has less elements
  return left_node == &end_ && right_node != &_list.end_;
}

// Tries to splice more than one element at a time, as the intrusive policy
// allow splicing of n consecutive nodes in O(1) complexity.
template<typename _Pred>
inline void IntrusiveNodeList::merge_(IntrusiveNodeList* _list,
                                      _Pred _pred) {
  assert(is_ordered_(_pred) && "This list must be ordered");
  if (this == _list) {
    return;
  }
  assert(_list->is_ordered_(_pred) && "The list in argument must be ordered");

  internal::Node* node = end_.next_;
  internal::Node* to_insert_begin = _list->end_.next_;

  while (node != &end_ && to_insert_begin != &_list->end_) {
    if (_pred(*node, *to_insert_begin)) {
      node = node->next_;
    } else {  // Try to find consecutive nodes satisfying _pred
      internal::Node* to_insert_end = to_insert_begin->next_;
      while (to_insert_end != &_list->end_) {
        if (_pred(*node, *to_insert_end)) {
          break;
        }
        to_insert_end = to_insert_end->next_;
      }
      splice_(node, to_insert_begin, to_insert_end);
      to_insert_begin = to_insert_end;
    }
  }

  if (to_insert_begin != &_list->end_) {  // Appends the rest of _list
    OZZ_IF_HAS_LIST_REDEBUG(assert(node == &end_);)
    splice_(&end_, to_insert_begin, &_list->end_);
  }

  OZZ_IF_HAS_LIST_REDEBUG(assert(_list->empty() &&
                                 redebug_check_integrity() &&
                                 is_ordered_(_pred));)
}

// Iterate and test predicate _pred for every node.
template<typename _Pred>
inline void IntrusiveNodeList::remove_if_(_Pred _pred) {
  internal::Node* node = end_.next_;
  while (node != &end_) {
    internal::Node* next_node = node->next_;
    if (_pred(*node)) {
      node->unlink();
    }
    node = next_node;
  }
  OZZ_IF_HAS_LIST_REDEBUG(assert(redebug_check_integrity());)
}

// Bin sort algorithm, takes advantage of O(1) complexity of swap and splice.
template<typename _Pred>
inline void IntrusiveNodeList::sort_(_Pred _pred) {
  // It's worth sorting if there is more than one element
  if (end_.next_->next_ == &end_) {
    return;
  }
  const int kMaxBins = 25;
  IntrusiveNodeList bin_lists[kMaxBins + 1];
  IntrusiveNodeList temp_list;
  int used_bins = 0;
  while (!empty()) {
    // Inserts the front node at the front of temp_list
    internal::Node* node = end_.next_;
    node->unlink();
    temp_list.link_front(node);

    int bin = 0;
    for (; bin < used_bins && !bin_lists[bin].empty(); ++bin) {
      // Merges into ever larger bins
      bin_lists[bin].merge_(&temp_list, _pred);
      bin_lists[bin].swap(temp_list);
    }

    if (bin == kMaxBins) {  // No more bin, merge in the last one
      bin_lists[kMaxBins - 1].merge_(&temp_list, _pred);
    } else {  // Spills to new bin, while they last
      bin_lists[bin].swap(temp_list);
      if (bin == used_bins) {
        used_bins++;
      }
    }
  }

  for (int bin = 1; bin < used_bins; ++bin) {  // Merge up every bin
    bin_lists[bin].merge_(&bin_lists[bin - 1], _pred);
  }

  if (used_bins != 0) {  // Result is in last bin
    IntrusiveNodeList& last_bin = bin_lists[used_bins - 1];
    splice_(end_.next_, last_bin.end_.next_, &last_bin.end_);
  }

  OZZ_IF_HAS_LIST_REDEBUG(assert(is_ordered_(_pred));)
}

// Loops and tests if _Pred is true for all nodes
template<typename _Pred>
inline bool IntrusiveNodeList::is_ordered_(_Pred _pred) const {
  const internal::Node* next_node = end_.next_->next_;
  while (next_node != &end_) {
    if (!_pred(*next_node->prev_, *next_node)) {
      return false;
    }
    next_node = next_node->next_;
  }
  return true;
}
}  // internal
}  // containers
}  // ozz

// Specialization of the std::swap algorithm for the IntusiveList class.
// Does not need to be implemented in std namespace thanks to ADL.
template <typename _Ty, typename _Option>
inline void swap(ozz::containers::IntrusiveList<_Ty, _Option>& _left, // NOLINT Don't want to #include <algorithm>
                 ozz::containers::IntrusiveList<_Ty, _Option>& _right) {
  _left.swap(_right);
}

// Undefines local macros
#undef OZZ_IF_DEBUG
#undef OZZ_IF_NDEBUG
#undef OZZ_HAS_CONTAINER_LIST_REDEBUG
#undef OZZ_IF_HAS_LIST_REDEBUG
#endif  // OZZ_OZZ_BASE_CONTAINERS_INTRUSIVE_LIST_H_

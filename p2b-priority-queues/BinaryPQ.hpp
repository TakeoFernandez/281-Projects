// Project identifier: 43DE0E0C4C76BFAA6D8C2F5AEAE0518A9C262F4E

#ifndef BINARYPQ_H
#define BINARYPQ_H
#include <algorithm>
#include "Eecs281PQ.hpp"


/*
A binary heap is defined as a binary tree with two additional constraints:[3]

Shape property: a binary heap is a complete binary tree; that is, all levels 
of the tree, except possibly the last one (deepest) are fully filled, and, if 
the last level of the tree is not complete, the nodes of that level are filled 
from left to right.

Heap property: the key stored in each node is either greater than or equal to 
(≥) or less than or equal to (≤) the keys in the node's children, according to 
some total order.
*/

// A specialized version of the priority queue ADT implemented as a binary heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class BinaryPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:

    // Translate 1-based indexing into a 0-based vector
    TYPE &getElement(std::size_t i) {
      return data[i - 1];
    }  // getElement()

    const TYPE &getElement(std::size_t i) const {
      return data[i - 1];
    }  // getElement()

    // Description: Construct an empty PQ with an optional comparison functor.
    // Runtime: O(1)
    explicit BinaryPQ(COMP_FUNCTOR comp = COMP_FUNCTOR())
        : BaseClass { comp } {
    }  // BinaryPQ


    // Description: Construct a PQ out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template<typename InputIterator>
    BinaryPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR())
        : BaseClass { comp }, data {start,end} {
            updatePriorities();
    }  // BinaryPQ

    

    // Description: Destructor doesn't need any code, the data vector will
    //              be destroyed automatically.
    virtual ~BinaryPQ() = default;


    // Description: Copy constructors don't need any code, the data vector
    //              will be copied automatically.
    BinaryPQ(const BinaryPQ &) = default;
    BinaryPQ(BinaryPQ &&) noexcept = default;


    // Description: Copy assignment operators don't need any code, the data
    //              vector will be copied automatically.
    BinaryPQ &operator=(const BinaryPQ &) = default;
    BinaryPQ &operator=(BinaryPQ &&) noexcept = default;


    // Description: Assumes that all elements inside the heap are out of order and
    //              'rebuilds' the heap by fixing the heap invariant.
    // Runtime: O(n)
    virtual void updatePriorities() {
        for(size_t i = data.size() - 1; 0 < i; --i){
            fixDown(i);
        }
    }  // updatePriorities()


    // Description: Add a new element to the PQ.
    // Runtime: O(log(n))
    virtual void push(const TYPE &val) {
        data.push_back(val);        //insert element at bottom
        fixUp(data.size());         //call fixUp on bottem element
    }  // push()


    // Description: Remove the most extreme (defined by 'compare') element
    //              from the PQ.
    // Note: We will not run tests on your code that would require it to pop
    // an element when the PQ is empty. Though you are welcome to if you are
    // familiar with them, you do not need to use exceptions in this project.
    // Runtime: O(log(n))
    virtual void pop() {
        getElement(1) = data.back();    //set root node to end node
        data.pop_back();                //remove the last node
        fixDown(1);                     //call fix down starting from root node
    }  // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the PQ. This should be a reference for speed. It MUST
    //              be const because we cannot allow it to be modified, as
    //              that might make it no longer be the most extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const {
        return getElement(1);
    }  // top()


    // Description: Get the number of elements in the PQ.
    // Runtime: O(1)
    [[nodiscard]] virtual std::size_t size() const {
        return data.size();
    }  // size()


    // Description: Return true if the PQ is empty.
    // Runtime: O(1)
    [[nodiscard]] virtual bool empty() const {
        return data.size() == 0 ;
    }  // empty()


private:
    // Note: This vector *must* be used for your PQ implementation.
    std::vector<TYPE> data;

    //Time complexity: O(log n)
    void fixDown(size_t k){
        while(2 * k <= data.size()){
            size_t j = 2 * k;
            if(j < data.size() && this->compare(getElement(j), getElement(j+1))){
                ++j;
            }//if right child bigger than left child choose right child
            if(this->compare(getElement(j), getElement(k))){
                break;
            }  //if heap fixed stop
            std::swap(getElement(k), getElement(j));
            k = j;
        }
    }

    //Time complexity: O(log n)
    void fixUp(size_t k){
        while(k > 1 && this->compare(getElement(k/2), getElement(k))){
            std::swap(getElement(k), getElement(k/2));
            k/=2;
        }
    }

};  // BinaryPQ


#endif  // BINARYPQ_H

// Project identifier: 43DE0E0C4C76BFAA6D8C2F5AEAE0518A9C262F4E

#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include <deque>
#include <utility>

#include "Eecs281PQ.hpp"

// A specialized version of the priority queue ADT implemented as a pairing
// heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
    public:
        // Description: Custom constructor that creates a node containing
        //              the given value.
        explicit Node(const TYPE &val)
            : elt { val } {}

        // Description: Allows access to the element at that Node's position.
        //              There are two versions, getElt() and a dereference
        //              operator, use whichever one seems more natural to you.
        // Runtime: O(1) - this has been provided for you.
        const TYPE &getElt() const { return elt; }
        const TYPE &operator*() const { return elt; }

        // The following line allows you to access any private data
        // members of this Node class from within the PairingPQ class.
        // (ie: myNode.elt is a legal statement in PairingPQ's add_node()
        // function).
        friend PairingPQ;

    private:
        TYPE elt;
        Node *child = nullptr;
        Node *sibling = nullptr;
        Node *parent = nullptr;
    };  // Node


    // Description: Construct an empty pairing heap with an optional
    //              comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR())
        : BaseClass { comp },root{nullptr},count{0}{
    }  // PairingPQ()


    // Description: Construct a pairing heap out of an iterator range with an
    //              optional comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR())
        : BaseClass { comp },root{nullptr},count{0} {
            while(start != end){
                push(*start);
                ++start;
            }
    }  // PairingPQ()
 

    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ &other)
        : BaseClass { other.compare },root{nullptr},count{0} {
        if(other.empty()){
            return;
        }   //if PQ empty then nothing to copy
        std::deque<Node*> nodes;
        nodes.push_back(other.root);
        while(!nodes.empty()){
            Node* curr = nodes.front(); //get next elt
            nodes.pop_front();
            if(curr->child){
                nodes.push_back(curr->child);
            }//if curr has a child
            if(curr->sibling){
                nodes.push_back(curr->sibling);
            }//if curr has a sibling
            push(curr->getElt());//copy

        }
    }  // PairingPQ()


    // Description : Copy assignment operator.
    // Runtime: O(n)
    PairingPQ &operator=(const PairingPQ &rhs) {
        PairingPQ temp(rhs);
        std::swap(count,temp.count);
        std::swap(root,temp.root);
        return *this;
    }  // operator=()


    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() {
        if(!root){
            return;
        }   //if PQ empty then nothing to delete
        std::deque<Node*> nodes;
        nodes.push_back(root);
        while(!nodes.empty()){
            Node* curr = nodes.front(); //get next elt
            nodes.pop_front();
            if(curr->child != nullptr){
                nodes.push_back(curr->child);
            }//if curr has a child
            if(curr->sibling !=nullptr){
                nodes.push_back(curr->sibling);
            }//if curr has a sibling
            delete curr;//delete current elt
        }
        root = nullptr; //set root to nullptr
    }  // ~PairingPQ()


    // Description: Move constructor and assignment operators don't need any
    //              code, the members will be reused automatically.
    PairingPQ(PairingPQ &&) noexcept = default;
    PairingPQ &operator=(PairingPQ &&) noexcept = default;


    // Description: Assumes that all elements inside the pairing heap are out
    //              of order and 'rebuilds' the pairing heap by fixing the
    //              pairing heap invariant.  You CANNOT delete 'old' nodes
    //              and create new ones!
    // Runtime: O(n)
    virtual void updatePriorities() {
        if(count < 2){
            return;
        }   //if PQ is empty or a single element then there are no nodes to update
        std::deque<Node*> nodes;
        nodes.push_back(root);
        while(!nodes.empty()){
            Node* curr = nodes.front(); //get next elt
            nodes.pop_front();
            if(curr->child){
                nodes.push_back(curr->child);
            }//if curr has a child
            if(curr->sibling){
                nodes.push_back(curr->sibling);
            }//if curr has a sibling
            curr->child = nullptr;
            curr->sibling = nullptr;
            curr->parent = nullptr;// break all connections
            if(curr != root){
                root = meld(root, curr);   //call meld on the root and curr now that curr has no connections
            }   //to deal with the issue of melding root to curr when they are the same
        }
    }  // updatePriorities()
    

    // Description: Add a new element to the pairing heap. This is already
    //              done. You should implement push functionality entirely
    //              in the addNode() function, and this function calls
    //              addNode().
    // Runtime: O(1)
    virtual void push(const TYPE &val) { addNode(val); }  // push()


    // Description: Remove the most extreme (defined by 'compare') element
    //              from the pairing heap.
    // Note: We will not run tests on your code that would require it to pop
    // an element when the pairing heap is empty. Though you are welcome to
    // if you are familiar with them, you do not need to use exceptions in
    // this project.
    virtual void pop() {
        if(empty()){return;}   //if after removing root there are no more elements, set root to null and return
        Node* oldRoot = root;
        std::deque<Node*> nodes;
        Node* p = root->child;  //store a pointer to child of old root
        --count;
        delete oldRoot;//remove the root node
        if(count == 0){
            root = nullptr;
            return;
        }

        while(p != nullptr){
            Node* next = p->sibling;
            p->parent = nullptr;
            p->sibling = nullptr;   //break connections
            nodes.push_back(p);
            p = next;   //move to next sibling
        }   //collects all the children of root and adds them to our node list

        std::deque<Node*> paired;
        while (nodes.size() > 1) {
            Node* a = nodes.front(); nodes.pop_front();
            Node* b = nodes.front(); nodes.pop_front();
            paired.push_back(meld(a, b));
        }   //iterate over nodes list

        if (!nodes.empty()) {
            paired.push_back(nodes.front());
        }   //if there was only 1 node to begin with

        root = paired.back();
        paired.pop_back();

        while (!paired.empty()) {
            root = meld(paired.back(), root);
            paired.pop_back();
        }
    }  // pop()

    // Description: Return the most extreme (defined by 'compare') element of
    //              the pairing heap. This should be a reference for speed.
    //              It MUST be const because we cannot allow it to be
    //              modified, as that might make it no longer be the most
    //              extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const {
        return root->getElt();
    }  // top()


    // Description: Get the number of elements in the pairing heap.
    // Runtime: O(1)
    [[nodiscard]] virtual std::size_t size() const {
        return count;
    }  // size()

    // Description: Return true if the pairing heap is empty.
    // Runtime: O(1)
    [[nodiscard]] virtual bool empty() const {
        return count == 0;
    }  // empty()


    // Description: Updates the priority of an element already in the pairing
    //              heap by replacing the element refered to by the Node with
    //              new_value.  Must maintain pairing heap invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more
    //              extreme (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    void updateElt(Node *node, const TYPE &new_value) {
        node->elt = new_value;   //update the element
        if(node == root){
            return;
        }   //if we change root do nothing
        Node* p = node->parent;
        if(p->child == node){
            p->child = node->sibling; //set nodes parents child to be nodes sibling

        } //if we are the leftmost child
        else{
            Node* curr = p->child; //will be set to the sibling before target node
            while(curr->sibling != node){
                curr = curr ->sibling;
            }   //move right until we reach the sibling before node
            curr->sibling = node->sibling;  //make the node previous to target to point to target nodes sibling
        }
        node->parent = nullptr;
        node->sibling = nullptr;
        root = meld(root, node);
    }  // updateElt()


    // Description: Add a new element to the pairing heap. Returns a Node*
    //              corresponding to the newly added element.
    // Runtime: O(1)
    // NOTE: Whenever you create a node, and thus return a Node *, you must
    //       be sure to never move or copy/delete that node in the future,
    //       until it is eliminated by the user calling pop(). Remember this
    //       when you implement updateElt() and updatePriorities().
    Node *addNode(const TYPE &val) {
        Node* toAdd = new Node(val);
        if(empty()){
            ++count;
            root = toAdd;
        }   //if empty
        else{
            root = meld(root,toAdd);
            ++count;
        }
        return toAdd;
    }  // addNode()

    Node* getLeftMostChild(){
        return root->child;
    }


private:
    Node* meld(Node* r1, Node* r2){
        if(this->compare(r1->getElt(), r2->getElt())){
            r1->parent = r2;
            r1->sibling = r2->child;
            r2->child = r1;
            return r2;
        }
        else{
            r2->parent = r1;
            r2->sibling = r1->child;
            r1->child = r2;
            return r1;
        }
    }

    // NOTE: For member variables, you are only allowed to add a "root
    //       pointer" and a "count" of the number of nodes. Anything else
    //       (such as a deque) should be declared inside of member functions
    //       as needed.
    Node *root; //must not have a parent or siblings
    uint32_t count;
};


#endif  // PAIRINGPQ_H

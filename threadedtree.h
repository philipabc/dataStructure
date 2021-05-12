#include <iostream>
#include <iomanip>
using namespace std;


template <class T>
class ThreadedTree {
    enum Color { RED, BLACK };
    struct Node {
        T data_{};
        Node* parent_{}, * left_{}, * right_{};
        bool leftThread_{}, rightThread_{};
        bool color_{};

        Node(const T& data = T{}, Node* parent = nullptr, Node* left = nullptr, Node* right = nullptr, bool leftThread = 1, bool rightThread = 1, bool color = 0) {
            data_ = data;
            parent_ = parent;
            left_ = left;
            right_ = right;
            leftThread_ = leftThread;
            rightThread_ = rightThread;
            color_ = color;
        }

    };
    Node* root_{};
    size_t size_{};
    Node* maxNode{};
    Node* minNode{};


    void assign(Node*& node, const T& data = T{}, Node* parent = nullptr, Node* left = nullptr, Node* right = nullptr, bool leftThread = 1, bool rightThread = 1, bool color = 0) {
        node->data_ = data;
        node->parent_ = parent;
        node->left_ = left;
        node->right_ = right;
        node->leftThread_ = leftThread;
        node->rightThread_ = rightThread;
        node->color_ = color;
    }
    Node* findMin(Node* current) const {   //O(logn)
        if (current) {
            while (current->left_ && !current->leftThread_) {
                current = current->left_;
            }
        }
        return current;
    }

    Node* findMax(Node* current) const {   //O(logn)
        if (current) {
            while (current->right_ && !current->rightThread_) {
                current = current->right_;
            }
        }
        return current;
    }

    Node* findPrev_ThreadTree(Node* current) const {   //O(logn)
        if (current) {
            if (!current->leftThread_ && current->left_) {
                //findMax(current->left_);
                current = current->left_;
                while (current->right_ && !current->rightThread_) {
                    current = current->right_;
                }
            }
            else current = current->left_;
        }
        else { //think as the iterator is a loop, now it is at end()
            if (root_) {
                //current = findMax(root_);
                current = maxNode;
            }
            else {
                cout << "The tree does not exist.";
            }
        }
        return current;
    }

    Node* findNext_ThreadTree(Node* current) const {   //O(logn)
        if (current) {
            if (!current->rightThread_ && current->right_) {
                //findMin(current->right_);
                current = current->right_;
                while (current->left_ && !current->leftThread_) {
                    current = current->left_;
                }
            }
            else current = current->right_;
        }
        else { //think as the iterator is a loop, now it is at end()
            if (root_) {
                //current = findMin(root_);
                current = minNode;
            }
            else {
                cout << "The tree does not exist.";
            }
        }
        return current;
    }

    Node* find(Node* root, const T& key) const {   //O(logn)
        Node* curr=root;
        if (curr) {
            if (key == curr->data_)  return curr;
            if (key < root->data_) {
                if (!root->leftThread_) curr = find(root->left_, key);
                else return nullptr;
            }
            else {
                if (!root->rightThread_) curr = find(root->right_, key);
                else return nullptr;
            }

        }
        return curr;
   }


    void destroy(Node* subroot) {   //O(n)
        if (subroot != nullptr) {
            if (!subroot->leftThread_) destroy(subroot->left_);
            if (!subroot->rightThread_) destroy(subroot->right_);
            delete subroot;
        }
    }


public:
    class const_iterator {
        friend class ThreadedTree;

    protected:
        Node* curr_{};
        const ThreadedTree<T>* threadedTree_{};

    public:
        const_iterator() {
        }

        const_iterator(const ThreadedTree* threadedTree, Node* curr) : threadedTree_{ threadedTree }, curr_{ curr }{
        }

        const_iterator operator++(int) {   //O(logn)
            const_iterator temp = *this;
            curr_ = this->threadedTree_->findNext_ThreadTree(curr_);
            return temp;
        }

        const_iterator operator--(int) {   //O(logn)
            const_iterator temp = *this;
            curr_ = this->threadedTree_->findPrev_ThreadTree(curr_);
            return temp;
        }


        const_iterator operator++() {   //O(logn)
            curr_ = this->threadedTree_->findNext_ThreadTree(curr_);
            return *this;

        }

        const_iterator operator--() {   //O(logn)
            curr_ = this->threadedTree_->findPrev_ThreadTree(curr_);
            return *this;
        }

        const T& operator*() const {
            return curr_->data_;
        }

        bool operator==(const const_iterator& rhs) const {
            return (curr_ == rhs.curr_ && threadedTree_ == rhs.threadedTree_);
        }

        bool operator!=(const const_iterator& rhs) const {
            return !(*this == rhs);
        }


    };
    class iterator :public const_iterator {
        friend class ThreadedTree;
    public:
        iterator() :const_iterator() {
        }

        iterator(const ThreadedTree* threadedTree, Node* curr) :const_iterator(threadedTree, curr) {
        }

        const T& operator*() const {
            return this->curr_->data_;
        }

        T& operator*() {
            return this->curr_->data_;
        }

        iterator operator++(int) {   //O(logn)
            iterator temp = *this;
            this->curr_ = this->threadedTree_->findNext_ThreadTree(this->curr_);
            return temp;
        }

        iterator operator--(int) {   //O(logn)
            iterator temp = *this;
            this->curr_ = this->threadedTree_->findPrev_ThreadTree(this->curr_);
            return temp;
        }

        iterator operator++() {   //O(logn)
            this->curr_ = this->threadedTree_->findNext_ThreadTree(this->curr_);
            return *this;
        }

        iterator operator--() {   //O(logn)
            this->curr_ = this->threadedTree_->findPrev_ThreadTree(this->curr_);
            return *this;
        }

        T* operator->() {
            return &this->curr_->data_;
        }

        const T* operator->() const {
            return &this->curr_->data_;
        }

    };

    ThreadedTree() {
    }
    void print() {};

    Node* ThreadInsert(const T& data) {
        Node* curr = root_;
        Node* parent = nullptr;
        Node* newNode{};

        while (curr != nullptr) {
            parent = curr;
            if (data == curr->data_) return curr;
            if (data < curr->data_) {
                if (!curr->leftThread_)
                    curr = curr->left_;
                else    break;
            }
            else {
                if (!curr->rightThread_)  curr = curr->right_;
                else  break;
            }
        }


        if (parent == nullptr) {
            newNode = new Node(data, parent);
            maxNode = minNode = root_ = newNode;
        }
        else if (data < parent->data_) {
            newNode = new Node(data, parent, parent->left_, parent);
            parent->left_ = newNode;
            parent->leftThread_ = 0;

        }
        else {
            newNode = new Node(data, parent, parent, parent->right_);
            parent->right_ = newNode;
            parent->rightThread_ = 0;
        }
        size_++;
        if (maxNode->data_ < newNode->data_) maxNode = newNode;
        if (newNode->data_ < minNode->data_) minNode = newNode;
        return newNode;
    }

    void rotate(Node*& grand_parent, Node*& root, Node*& left, Node*& right,
        Node* leftRight, bool rightThread,
        Node* rightLeft, bool leftThread) {

        assign(root, root->data_, nullptr, left, right, 0, 0, 1);
        if (grand_parent->parent_ == nullptr) {
            root_ = root;
            root->parent_ = nullptr;
        }
        else {
            root->parent_ = grand_parent->parent_;
            if (grand_parent->parent_->left_ == grand_parent) {
                grand_parent->parent_->left_ = root;
            }
            else {
                grand_parent->parent_->right_ = root;
            }
        }

        assign(left, left->data_, root, left->left_, leftRight, left->leftThread_, rightThread, 1);

        assign(right, right->data_, root, rightLeft, right->right_, leftThread, right->rightThread_, 1);

        if (!rightThread) {
            leftRight->parent_ = left;
            left->right_ = leftRight;
        }
        else {
            left->right_ = root;
        }

        if (!leftThread) {
            rightLeft->parent_ = right;
            right->left_ = rightLeft;
        }
        else {
            right->left_ = root;
        }
    }


    // This function fixes violations caused by BST insertion for a red black tree
    void maintainRBT(Node*& root, Node*& curr)
    {
        Node* parent_curr = nullptr;
        Node* grand_parent_curr = nullptr;
        Node* uncle_node{};

        while ((curr != root) && (curr->color_ != BLACK) && (curr->parent_->color_ == RED)) {
            parent_curr = curr->parent_;
            grand_parent_curr = curr->parent_->parent_;

            /* Case : A - / left case */
            if (parent_curr == grand_parent_curr->left_) {
                uncle_node = grand_parent_curr->right_;

                /* Case : 1*/
                if (uncle_node != nullptr && uncle_node->color_ == RED) {
                    grand_parent_curr->color_ = RED;
                    parent_curr->color_ = BLACK;
                    uncle_node->color_ = BLACK;
                    curr = grand_parent_curr;
                }
                else {
                    /* Case : 2 - \ left right case */
                    if (curr == parent_curr->right_) {
                        rotate(grand_parent_curr, curr, parent_curr, grand_parent_curr,
                            curr->left_, curr->leftThread_,
                            curr->right_, curr->rightThread_);
                    }

                    /* Case : 3 - /  left left case */
                    else {
                        rotate(grand_parent_curr, parent_curr, curr, grand_parent_curr,
                            curr->right_, curr->rightThread_,
                            parent_curr->right_, parent_curr->rightThread_);
                        curr = parent_curr;
                    }
                }
            }

            /* Case : B - \ right case */
            else {
                uncle_node = grand_parent_curr->left_;
                /* Case : 1*/
                if ((uncle_node != nullptr) && (uncle_node->color_ == RED)) {
                    grand_parent_curr->color_ = RED;
                    parent_curr->color_ = BLACK;
                    uncle_node->color_ = BLACK;
                    curr = grand_parent_curr;
                }
                else {
                    /* Case : 2 - / right left case */
                    if (curr == parent_curr->left_) {
                        rotate(grand_parent_curr, curr, grand_parent_curr, parent_curr,
                            curr->left_, curr->leftThread_,
                            curr->right_, curr->rightThread_);
                    }

                    /* Case : 3 - \ right right case */
                    else {
                        rotate(grand_parent_curr, parent_curr, grand_parent_curr, curr,
                            parent_curr->left_, parent_curr->leftThread_,
                            curr->left_, curr->leftThread_);
                        curr = parent_curr;
                    }
                }
            }

        }

        root->color_ = BLACK;
    }

    // Function to insert a new node with given data for a red black tree
    void insert(const T& data)
    {
        Node* curr = ThreadInsert(data);

        maintainRBT(root_, curr);
    }

    iterator find(const T& key) {

        return iterator(this, find(root_, key));
    }
    const_iterator find(const T& key) const {

        return const_iterator(this, find(root_, key));
    }
    iterator begin() {   //O(1)
        return iterator(this, minNode);
    }
    iterator end() {   //O(1)
        return iterator(this, nullptr);
    }
    const_iterator cbegin()const {   //O(1)
        return const_iterator(this, minNode);
    }
    const_iterator cend() const {   //O(1)
        return const_iterator(this, nullptr);
    }

    int size() const {   //O(1)
        return size_;
    }
    bool empty() const {   //O(1)
        return root_ == nullptr;
    }
    ~ThreadedTree() {
        destroy(root_);
    }
};


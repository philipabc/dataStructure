
#include<algorithm>

template <typename T>

//sentinel
class RecentList {
	struct Node {
		T data_;
		Node* nx_;
		Node* pr_;
		Node(const T& data = T{}, Node* nx = nullptr, Node* pr = nullptr) {
			data_ = data;
			nx_ = nx;
			pr_ = pr;
		}
	};
	Node* front_;
	Node* back_;
public:
	class const_iterator {
		friend class RecentList<T>;
	protected:
		const RecentList<T>* myList_;
		Node* curr_;
		const_iterator(Node* curr, const RecentList<T>* theList) : curr_(curr), myList_(theList) {}
	public:
		//Default constructor
		const_iterator() {
			myList_ = nullptr;
			curr_ = nullptr;
		}
		//prefix
		const_iterator operator++() {
			curr_ = curr_->nx_;
			return *this;
		}
		//postfix
		const_iterator operator++(int) {
			const_iterator old = *this;
			curr_ = curr_->nx_;
			return old;
		}
		//prefix
		const_iterator operator--() {
			curr_ = curr_->pr_;
			return *this;
		}
		//postfix
		const_iterator operator--(int) {
			const_iterator old = *this;
			curr_ = curr_->pr_;
			return old;
		}
		//Compares rhs and current object to see if they refer to the same node
		bool operator==(const_iterator rhs) const {
			bool rc = false;
			if (myList_ == rhs.myList_ && curr_ == rhs.curr_) {
				rc = true;
			}
			return rc;
		}
		//Compares rhs and current object to see if they DO NOT refer to the same node
		bool operator!=(const_iterator rhs) {
			return !(*this == rhs);
		}
		//returns a reference to data in the node referred to by the iterator.
		const T& operator*()const {
			return curr_->data_;
		}
	};
	class iterator : public const_iterator {
		friend class RecentList<T>;

	public:
		iterator() {}

		iterator(Node* curr, const RecentList<T>* theList) : const_iterator(curr, theList) {}

		//prefix
		iterator operator++() {
			this->curr_ = this->curr_->nx_;
			return *this;
		}
		//postfix
		iterator operator++(int) {
			iterator old = *this;
			this->curr_ = this->curr_->nx_;
			return old;
		}
		//prefix
		iterator operator--() {
			this->curr_ = this->curr_->pr_;
			return *this;
		}
		//postfix
		iterator operator--(int) {
			iterator old = *this;
			this->curr_ = this->curr_->pr_;
			return old;
		}
		//returns a reference to data in the node referred to by the iterator.
		T& operator*() {
			return this->curr_->data_;
		}
		//returns a const reference to data in the node referred to by the iterator.
		const T& operator*()const {
			return const_iterator::operator*();
		}
	};
	void initialize();
	RecentList();
	~RecentList();
	RecentList(const RecentList& rhs);
	RecentList& operator=(const RecentList& rhs);
	RecentList(RecentList&& rhs);
	RecentList& operator=(RecentList&& rhs);
	//returns the appropriate iterator to the first node in the list
	iterator begin() { return iterator(front_->nx_, this); }
	//returns the appropriate iterator to the node after the last node in the list
	iterator end() { return iterator(back_, this); }
	//returns the appropriate iterator to the first node in the list
	const_iterator cbegin() const { return const_iterator(front_->nx_, this); }
	//returns the appropriate iterator to the node after the last node in the list
	const_iterator cend() const { return const_iterator(back_, this); }
	void insert(const T& data);
	iterator search(const T& data);
	iterator erase(iterator it);
	iterator erase(iterator first, iterator last);
	bool empty() const;
	int size() const;
};

//Creates an empty list and initializes the member variables to safe empty state (sentinel)
template <typename T>
void RecentList<T>::initialize() {
	front_ = new Node();
	back_ = new Node();
	front_->nx_ = back_;
	back_->pr_ = front_;
}
//Default Constructor
template <typename T>
RecentList<T>::RecentList() {
	initialize();
}

//destructor for linked list. All nodes are deallocated
template <typename T>
RecentList<T>::~RecentList() {
	erase(begin(), end());
	delete front_;
	delete back_;
}

//copy constructor
template <typename T>
RecentList<T>::RecentList(const RecentList& rhs) {
	initialize();
	*this = rhs;
}

//assignment operator.
template <typename T>
RecentList<T>& RecentList<T>::operator=(const RecentList& rhs) {
	if (this != &rhs) {
		erase(begin(), end());
		if (!rhs.empty()) {
			for (const_iterator it = rhs.cbegin(); it != rhs.cend(); it++) {
				Node* nn = new Node(it.curr_->data_, back_, back_->pr_);
				back_->pr_->nx_ = nn;
				back_->pr_ = nn;
			}
		}
	}
	return *this;
}

//move constructor
template <typename T>
RecentList<T>::RecentList(RecentList&& rhs) {
	initialize();
	*this = std::move(rhs);
}

//move operator
template <typename T>
RecentList<T>& RecentList<T>::operator=(RecentList&& rhs) {
	if (this != &rhs) {
		std::swap(rhs.front_, this->front_);
		std::swap(rhs.back_, this->back_);
	}
	return *this;
}

//creates new node holding data at the front of the linked list.
template <typename T>
void RecentList<T>::insert(const T& data) {
	Node* nn = new Node(data, front_->nx_, front_);
	front_->nx_->pr_ = nn;
	front_->nx_ = nn;
}

/*search accepts as argument a key of the unknown data type T.
The function finds and returns an iterator to the node containing a matching key.
It will also adjust the linked list and found node by moving the found node to the front of the list.
Function returns an iterator to the node if found, end() if it is not. */
template <typename T>
typename RecentList<T>::iterator RecentList<T>::search(const T& data) {
	for (iterator it = begin(); it != end(); it++) {
		if (*it == data) {
			if (it.curr_->pr_ == front_) {
				return it;
			}
			it.curr_->pr_->nx_ = it.curr_->nx_;
			it.curr_->nx_->pr_ = it.curr_->pr_;
			it.curr_->pr_ = front_;
			it.curr_->nx_ = front_->nx_;
			front_->nx_->pr_ = it.curr_;
			front_->nx_ = it.curr_;
			return it;
		}
	}
	return end();
}

//This function removes a node matches iterator it returns node that follows the removed node
template <typename T>
typename RecentList<T>::iterator RecentList<T>::erase(iterator it) {
	Node* temp = it.curr_;
	iterator rc(temp->nx_, this);
	temp->pr_->nx_ = temp->nx_;
	temp->nx_->pr_ = temp->pr_;
	delete temp;

	return rc;
}

//This function removes all nodes from first to last, including first but not including last. function returns node after the last removed node
template <typename T>
typename RecentList<T>::iterator RecentList<T>::erase(iterator first, iterator last) {
	for (iterator it = first; it != last;) {
		it = erase(it);
	}
	return last;
}

//This function checks if the list is empty.
template <typename T>
bool RecentList<T>::empty() const {
	return (front_->nx_ == back_);
}

//This function returns the number of nodes in the list.
template <typename T>
int RecentList<T>::size() const {
	int rc = 0;
	for (const_iterator it = cbegin(); it != cend(); it++) {
		rc++;
	}
	return rc;
}

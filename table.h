
#include <string>
#include <utility>
#include <functional>
#include "mylist.h"
using namespace std;


template <class TYPE>
class Table {
public:
	Table() {}
	virtual void update(const std::string& key, const TYPE& value) = 0;
	virtual bool remove(const std::string& key) = 0;
	virtual bool find(const std::string& key, TYPE& value) = 0;
	virtual int numRecords() const = 0;
	virtual bool isEmpty() const = 0;
	virtual int capacity() const = 0;
	virtual ~Table() {}
};

template <class TYPE>
class SimpleTable :public Table<TYPE> {

	struct Record {
		TYPE data_;
		std::string key_;
		Record(const std::string& key, const TYPE& data) {
			key_ = key;
			data_ = data;
		}

	};


	Record* (*records_);   //the table
	int capacity_;       //capacity of the array

	void grow() {
		Record** tmp = new Record * [capacity_ * 2 + 1];
		for (int i = 0; i < capacity_; i++) {
			tmp[i] = records_[i];
		}
		for (int i = capacity_; i < capacity_ * 2 + 1; i++) {
			tmp[i] = nullptr;
		}
		delete[] records_;
		records_ = tmp;
		capacity_ *= 2;
	}
public:
	SimpleTable(int capacity);
	SimpleTable(const SimpleTable& rhs);
	SimpleTable(SimpleTable&& rhs);
	virtual void update(const std::string& key, const TYPE& value);
	virtual bool remove(const std::string& key);
	virtual bool find(const std::string& key, TYPE& value);
	virtual const SimpleTable& operator=(const SimpleTable& rhs);
	virtual const SimpleTable& operator=(SimpleTable&& rhs);
	virtual ~SimpleTable();
	virtual bool isEmpty() const { return numRecords() == 0; }
	virtual int numRecords() const;
	virtual int capacity() const { return capacity_; }

};

template <class TYPE>
int SimpleTable<TYPE>::numRecords() const {
	int rc = 0;
	for (int i = 0; records_[i] != nullptr; i++) {
		rc++;
	}
	return rc;
}



template <class TYPE>
SimpleTable<TYPE>::SimpleTable(int capacity) : Table<TYPE>() {
	records_ = new Record * [capacity + 1];
	capacity_ = capacity;
	for (int i = 0; i < capacity_ + 1; i++) {
		records_[i] = nullptr;
	}
}

template <class TYPE>
SimpleTable<TYPE>::SimpleTable(const SimpleTable<TYPE>& rhs) {
	records_ = new Record * [rhs.capacity_ + 1];
	capacity_ = rhs.capacity_;
	for (int i = 0; i < capacity_ + 1; i++) {
		records_[i] = nullptr;
	}
	for (int i = 0; i < rhs.numRecords(); i++) {
		update(rhs.records_[i]->key_, rhs.records_[i]->data_);
	}
}
template <class TYPE>
SimpleTable<TYPE>::SimpleTable(SimpleTable<TYPE>&& rhs) {
	capacity_ = rhs.capacity_;
	records_ = rhs.records_;
	rhs.records_ = nullptr;
	rhs.capacity_ = 0;
}

template <class TYPE>
void SimpleTable<TYPE>::update(const std::string& key, const TYPE& value) {
	int idx = -1;

	for (int i = 0; i < numRecords(); i++) {
		if (records_[i]->key_ == key) {
			idx = i;
		}
	}
	if (idx == -1) {
		if (numRecords() == capacity_) {
			grow();
		}
		records_[numRecords()] = new Record(key, value);
		for (int i = numRecords() - 1; i > 0 && records_[i]->key_ < records_[i - 1]->key_; i--) {
			Record* tmp = records_[i];
			records_[i] = records_[i - 1];
			records_[i - 1] = tmp;
		}

	}
	else {
		records_[idx]->data_ = value;
	}

}

template <class TYPE>
bool SimpleTable<TYPE>::remove(const std::string& key) {
	int idx = -1;
	for (int i = 0; i < numRecords(); i++) {
		if (records_[i]->key_ == key) {
			idx = i;
		}
	}
	if (idx != -1) {
		int size = numRecords();
		delete records_[idx];
		for (int i = idx; i < size - 1; i++) {
			records_[i] = records_[i + 1];
		}
		records_[size - 1] = nullptr;
		return true;
	}
	else {
		return false;
	}
}

template <class TYPE>
bool SimpleTable<TYPE>::find(const std::string& key, TYPE& value) {
	int idx = -1;
	for (int i = 0; i < numRecords(); i++) {
		if (records_[i]->key_ == key) {
			idx = i;
		}
	}
	if (idx == -1)
		return false;
	else {
		value = records_[idx]->data_;
		return true;
	}
}

template <class TYPE>
const SimpleTable<TYPE>& SimpleTable<TYPE>::operator=(const SimpleTable<TYPE>& rhs) {
	if (this != &rhs) {
		if (records_) {
			while (numRecords() != 0) {
				remove(records_[0]->key_);
			}
			delete[] records_;
		}
		records_ = new Record * [rhs.capacity_ + 1];
		capacity_ = rhs.capacity_;
		for (int i = 0; i < capacity_; i++) {
			records_[i] = nullptr;
		}
		for (int i = 0; i < rhs.numRecords(); i++) {
			update(rhs.records_[i]->key_, rhs.records_[i]->data_);
		}

	}
	return *this;
}
template <class TYPE>
const SimpleTable<TYPE>& SimpleTable<TYPE>::operator=(SimpleTable<TYPE>&& rhs) {
	if (records_) {
		while (numRecords() != 0) {
			remove(records_[0]->key_);
		}
		delete[] records_;
	}
	records_ = rhs.records_;
	capacity_ = rhs.capacity_;
	rhs.records_ = nullptr;
	rhs.capacity_ = 0;

	return *this;
}
template <class TYPE>
SimpleTable<TYPE>::~SimpleTable() {
	if (records_) {
		int sz = numRecords();
		for (int i = 0; i < sz; i++) {
			remove(records_[0]->key_);
		}
		delete[] records_;
	}
}


template <class TYPE>
class ChainingTable :public Table<TYPE> {

	struct Record {
		TYPE data_{};
		std::string key_{};
		Record() {}
		Record(const std::string& key, const TYPE& data) {
			key_ = key;
			data_ = data;
		}
		bool operator== (const Record& record) const {
			return key_ == record.key_;
		}
	};
	RecentList<Record>** records_{};   //the table
	int capacity_{};       //capacity of the array
	double maxLoadFactor_{};
	int numOfRecords_{};
	std::hash<std::string> hashFunction;

	void grow() {
		RecentList<Record>** tmp = new RecentList<Record>*[capacity_ * 2];
		capacity_ *= 2;
		size_t hashValue{};
		int idx{};
		for (int i = 0; i < capacity_; i++) {
			tmp[i] = new RecentList<Record>();
		}
		for (int i = 0; i < capacity_ / 2; i++) {
			for (auto it = records_[i]->begin(); it != records_[i]->end(); it++) {
				hashValue = hashFunction((*it).key_);
				idx = hashValue % capacity_;
				tmp[idx]->insert(*it);     //O(1)
			}
		}
		for (int i = 0; i < capacity_ / 2; i++) {
			delete records_[i];
		}
		delete[] records_;
		records_ = tmp;
	}

public:
	ChainingTable(int maxExpected, double maxLoadFactor);
	ChainingTable(const ChainingTable& other);
	ChainingTable(ChainingTable&& other);
	virtual void update(const std::string& key, const TYPE& value);
	virtual bool remove(const std::string& key);
	virtual bool find(const std::string& key, TYPE& value);
	virtual const ChainingTable& operator=(const ChainingTable& other);
	virtual const ChainingTable& operator=(ChainingTable&& other);
	virtual ~ChainingTable();
	virtual bool isEmpty() const;
	virtual int numRecords() const;
	virtual int capacity() const;

};

//Constructor
template <class TYPE>
ChainingTable<TYPE>::ChainingTable(int capacity, double maxLoadFactor) : Table<TYPE>() {
	capacity_ = capacity;
	maxLoadFactor_ = maxLoadFactor;
	numOfRecords_ = 0;
	records_ = new RecentList<Record>*[capacity_];
	for (int i = 0; i < capacity_; i++) {
		records_[i] = new RecentList<Record>();
	}
}

//copy constructor
template <class TYPE>
ChainingTable<TYPE>::ChainingTable(const ChainingTable<TYPE>& other) {
	*this = other;
}

//move constructor
template <class TYPE>
ChainingTable<TYPE>::ChainingTable(ChainingTable<TYPE>&& other) {
	*this = std::move(other);
}

//This function is passed a key-value pair. If your table already has a record with a matching key, the record's value is replaced by the value passed to this function. 
//If no record exists, a record with key-value pair is added.
//If the update() causes the load factor to go above the maxLoadFactor for the table, 
//this function must reallocate the number of slots in the table to be double the current number of slots before the new key - value pair gets passed in.
template <class TYPE>
void ChainingTable<TYPE>::update(const std::string& key, const TYPE& value) {
	size_t hashIndex = hashFunction(key) % capacity_;
	TYPE temp;
	if (find(key, temp)) {
		(*(records_[hashIndex]->begin())).data_ = value;
	}
	else {
		hashIndex = hashFunction(key) % capacity_;
		Record newRecord = *(new Record(key, value));
		records_[hashIndex]->insert(newRecord);
		numOfRecords_++;
		if ( numOfRecords_ > maxLoadFactor_ * capacity_) {
			grow();
		}
	}

}

//This function is passed a key. If your table contains a record with a matching key, the record (both the key and the value) is removed from the table
template <class TYPE>
bool ChainingTable<TYPE>::remove(const std::string& key) {
	size_t hashIndex = hashFunction(key) % capacity_;
	TYPE temp;
	if (find(key, temp)) {
		records_[hashIndex]->erase(records_[hashIndex]->begin());
		numOfRecords_--;
		return true;
	}
	return false;
}

//This function is passed a key and a reference for passing back a found value. If your table contains a record with a matching key, the function returns true, and passes back the value from the record.
//If it does not find a record with a matching key, function returns false.
template <class TYPE>
bool ChainingTable<TYPE>::find(const std::string& key, TYPE& value) {
	size_t hashIndex = hashFunction(key) % capacity_;
	Record passedRecord = Record(key, value);
	if (records_[hashIndex]->search(passedRecord) == records_[hashIndex]->end()) {
		return false;
	}
	value = (*records_[hashIndex]->begin()).data_;
	return true;
}

//Copy assignment
template <class TYPE>
const ChainingTable<TYPE>& ChainingTable<TYPE>::operator=(const ChainingTable<TYPE>& other) {
	if (this != &other)
	{
		maxLoadFactor_ = other.maxLoadFactor_;
		numOfRecords_ = other.numOfRecords_;

		for (int i = 0; i < capacity_; i++)
		{
			if (records_[i])
				delete records_[i];
		}
		delete[] records_;

		capacity_ = other.capacity_;
		records_ = new  RecentList<Record> *[capacity_] {};
		for (int i = 0; i < capacity_; i++)
		{
			records_[i] = new RecentList<Record>(*other.records_[i]);
		}
	}

	return *this;
}

//Move assignment
template <class TYPE>
const ChainingTable<TYPE>& ChainingTable<TYPE>::operator=(ChainingTable<TYPE>&& other) {
	swap(records_, other.records_);
	swap(capacity_, other.capacity_);
	swap(maxLoadFactor_, other.maxLoadFactor_);
	swap(numOfRecords_, other.numOfRecords_);
	return *this;
}

//Destructor
template <class TYPE>
ChainingTable<TYPE>::~ChainingTable() {
	for (int i = 0; i < capacity_; i++) {
		delete records_[i];
	}
	delete[] records_;
}

//This function returns true if table is empty, false otherwise
template <class TYPE>
bool ChainingTable<TYPE>::isEmpty() const {
	return !numOfRecords_;
}

//This function returns number of records in the table
template <class TYPE>
int ChainingTable<TYPE>::numRecords() const {
	return numOfRecords_;
}

//This function returns the number of slots in the table
template <class TYPE>
int ChainingTable<TYPE>::capacity() const {
	return capacity_;
}


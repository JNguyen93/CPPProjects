#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <cstdint>
#include <stdexcept>
#include <utility>
#include <stdint.h>
#include <iterator>

//Utility gives std::rel_ops which will fill in relational
//iterator operations so long as you provide the
//operators discussed in class.  In any case, ensure that
//all operations listed in this website are legal for your
//iterators:
//http://www.cplusplus.com/reference/iterator/RandomAccessIterator/
using namespace std::rel_ops;
using std::cout;

namespace epl{

class invalid_iterator {
	public:
	enum SeverityLevel {SEVERE,MODERATE,MILD,WARNING};
	SeverityLevel level;	

	invalid_iterator(SeverityLevel level = SEVERE){ this->level = level; }
	virtual const char* what() const {
    switch(level){
      case WARNING:   return "Warning"; // not used in Spring 2015
      case MILD:      return "Mild";
      case MODERATE:  return "Moderate";
      case SEVERE:    return "Severe";
      default:        return "ERROR"; // should not be used
    	}
	}
};

template <typename T>
class vector{
	friend class iterator;
	friend class const_iterator;
	private:
		std::allocator<T> allocator;
		T* storage;
		T* front;
		uint32_t fcapacity;
		uint32_t bcapacity;
		uint32_t len;
		uint32_t storage_size;
		uint32_t revision;
	public:
		vector(void){
			storage = (T*)operator new(sizeof(T) * 8);
			fcapacity = 0;
			bcapacity = 8;
			len = 0;
			storage_size = 8;
			front = storage;
			revision = 0;
		}

		explicit vector(uint64_t n){
			if(n > 0){
				storage = new T[n];
				fcapacity = 0;
				bcapacity = 0;
				len = n;
				storage_size = n;
				front = storage;
				revision = 0;
			}
			else if(n == 0){
				storage = (T*)operator new(sizeof(T) * 8);
				storage_size = 8;
				fcapacity = 0;
				bcapacity = 8;
				len = 0;
				front = storage;
				revision = 0;
			}
			else{
				throw std::out_of_range{"Negative Index!"};
			}
		}

		vector(std::initializer_list<T> temp){
			storage = (T*)operator new(sizeof(T) * 8);
			int counter = 0;
			for (auto b = temp.begin() ; b != temp.end(); ++b){
				storage[counter] = *b;
				counter++;
			}
			storage_size = 8;
			fcapacity = 0;
			bcapacity = 8 - temp.size();
			len = temp.size();
			front = storage;
			revision = 0;
		}

		template <typename it>
		vector(it b, it e){
			copy(*b.pvector);
		}

		vector(const vector& ptr){
			copy(ptr);
		}

		~vector(void) {
			destroy();
		}

		vector(vector&& that){
			move(std::move(that));
		}

		vector& operator= (const vector& rhs){
			if (this != &rhs) {
				destroy();
				copy(rhs);
				this->revision++;
			}
			return *this;
		}

		vector& operator= (vector&& rhs){
			move(std::move(rhs));
			this->revision++;
			return *this;
		}

		uint64_t size(void) const {
			return len;
		}

		T& operator[](uint64_t k){
			if (k >= storage_size) {
				throw std::out_of_range{"index out of range"};
			}
			return front[k];
		}

		const T& operator[](uint64_t k) const {
			if (k >= storage_size) {
				throw std::out_of_range{"index out of range"};
			}
			return front[k];
		}

		void push_back(const T& data){
			if(bcapacity > 0){
				new(front + len)T(data);
				bcapacity--;
				len++;
				revision++;
			}
			else{
				bcapacity = bcapacity + storage_size;
				storage_size *= 2;
				T* temp = allocator.allocate(storage_size);
				for(uint32_t i = 0; i < len; i++){
					new(temp + i)T(std::move(front[i]));
				}
				fcapacity = 0;
				storage = temp;
				new(storage + len)T(data);
				temp = nullptr;
				front = storage;
				bcapacity--;
				len++;
				revision++;
			}
		}

		void push_back(T&& data){
			if(bcapacity > 0){
				new(front + len)T(std::move(data));
				bcapacity--;
				len++;
				revision++;
			}
			else{
				bcapacity = bcapacity + storage_size;
				storage_size *= 2;
				T* temp = allocator.allocate(storage_size);
				for(uint32_t i = 0; i < len; i++){
					new(temp + i)T(std::move(front[i]));
				}
				fcapacity = 0;
				storage = temp;
				temp = nullptr;
				new(storage + len)T(std::move(data));
				front = storage;
				bcapacity--;
				len++;
				revision++;
				}
		}

		void push_front(const T& data){
			if(fcapacity > 0){
				new(front - 1)T(data);
				front--;
				fcapacity--;
				len++;
				revision++;
			}
			else{
				if(bcapacity > 0){
					T* temp = allocator.allocate(storage_size);
					new(temp)T(data);
					for(uint32_t i = 0; i < len; i++){
						int k = i + 1;
						new(temp + k)T(std::move(front[i]));
					}
					storage = temp;
					temp = nullptr;
					front = storage;
					fcapacity = 0;
					bcapacity--;
					len++;
					revision++;
				}
				else{
					bcapacity = bcapacity + storage_size;
					storage_size *= 2;
					T* temp = allocator.allocate(storage_size);
					new(temp)T(data);
					for(uint32_t i = 0; i < len; i++){
						int k = i + 1;
						new(temp + k)T(std::move(front[i]));
					}
					fcapacity = 0;
					storage = temp;
					temp = nullptr;
					front = storage;
					bcapacity--;
					len++;
					revision++;
				}
			}
		}

		void push_front(T&& data){
			if(fcapacity > 0){
				new(front - 1)T(std::move(data));
				front--;
				fcapacity--;
				len++;
				revision++;
			}
			else{
				if(bcapacity > 0){
					T* temp = allocator.allocate(storage_size);
					new(temp)T(std::move(data));
					for(uint32_t i = 0; i < len; i++){
						int k = i + 1;
						new(temp + k)T(std::move(front[i]));
					}
					storage = temp;
					temp = nullptr;
					front = storage;
					fcapacity = 0;
					bcapacity--;
					len++;
					revision++;
				}
				else{
					bcapacity = bcapacity + storage_size;
					storage_size *= 2;
					T* temp = allocator.allocate(storage_size);
					new(temp)T(std::move(data));
					for(uint32_t i = 0; i < len; i++){
						int k = i + 1;
						new(temp + k)T(std::move(front[i]));
					}
					fcapacity = 0;
					storage = temp;
					temp = nullptr;
					front = storage;
					bcapacity--;
					len++;
					revision++;
				}
			}
		}

		void pop_back(){
			if(len == 0){
				throw std::out_of_range("No elements in vector.");
			}
			else{
				front[len-1].~T();
				bcapacity++;
				len--;
				revision++;
			}
		}

		void pop_front(){
			if(len == 0){
				throw std::out_of_range("No elements in vector.");
			}
			else{
				front[0].~T();
				front++;
				fcapacity++;
				len--;
				revision++;
			}
		}

		class iterator : std::iterator<std::random_access_iterator_tag, T>{
			friend class vector;
			private:
				T* position = nullptr;
				vector<T>* pvector = nullptr;
				uint32_t version = -1;
			public:
				iterator(void) {}
				iterator(vector<T>* v, T* p){
					position = p;
					pvector = v;
					version = v->revision;
				}
				iterator(vector<T>* v){
					pvector = v;
					version = v->revision;
					position = v->front;
				}

				iterator(const iterator& it){
					if(pvector != nullptr){
						position = it.position;
						pvector = it.pvector;
						version = it.version;
					}
					else{
						position = nullptr;
						pvector = nullptr;
						version = -1;
					}

				}
				T& operator*(void) {
					if(version == pvector->revision){
						return *position;
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

				T& operator[](uint64_t k){
					if(version == pvector->revision){
						if (k >= pvector->storage_size) {
							throw std::out_of_range{"index out of range"};
						}
						return position[k];
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

				iterator& operator+(uint32_t n){
					if(version == pvector->revision){
						this->position = position + n;
						return *this;
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

				iterator& operator-(uint32_t n){
					if(version == pvector->revision){
						this->position = position - n;
						return *this;
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

				iterator& operator-(iterator& that){
					if(version == pvector->revision){
						uint64_t diff = this->position - that.position;
						this->position = this->position - diff;
						return *this;
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}


				iterator& operator=(const iterator& rhs){
					if(this != &rhs){
						this->position = rhs.position;
						this->version = rhs.version;
						this->pvector = rhs.pvector;
					}
					return *this;
				}

				iterator& operator+= (uint32_t n){
					if(version == pvector->revision){
						this->position = position + n;
						return *this;
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

				iterator& operator-= (uint32_t n){
					if(version == pvector->revision){
						this->position = position - n;
						return *this;
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

				iterator& operator++(void) {
					if(version == pvector->revision){
						position = ++position;
						return *this;
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

				iterator operator++(int) {
					if(version == pvector->revision){
						iterator t{*this}; // copy myself
						this->operator++(); // increment myself (pre-increment)
						return t; // return the copy of my old value
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

				bool operator==(const iterator& rhs) const {
					if(version == pvector->revision){
						return this->position == rhs.position;
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

				bool operator!=(const iterator& rhs) const {
					if(version == pvector->revision){
						return ! (*this == rhs);
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

				bool operator<(const iterator& rhs) const{
					if(version == pvector->revision){
						return this->position < rhs.position;
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

				bool operator>(const iterator& rhs) const{
					if(version == pvector->revision){
						return this->position > rhs.position;
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

				bool operator<=(const iterator& rhs) const{
					if(version == pvector->revision){
						return this->position <= rhs.position;
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

				bool operator>=(const iterator& rhs) const{
					if(version == pvector->revision){
						return this->position >= rhs.position;
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

				iterator& operator--(void) {
					if(version == pvector->revision){
						position = --position;
						return *this;
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

				iterator operator--(int) {
					if(version == pvector->revision){
						iterator t{*this};
						this->operator--();
						return t;
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

			};

		class const_iterator : public iterator{
			private:
				const T* position = nullptr;
				const vector<T>* pvector = nullptr;
				uint32_t version = -1;
			public:
				const_iterator(void) {}
				const_iterator(const vector<T>* v, const T* p){
					position = p;
					pvector = v;
					version = v->revision;
				}
				const_iterator(const vector<T>* v){
					pvector = v;
					position = v->front;
					version = v->revision;
				}
				const T& operator*(void) {
					if(version == pvector->revision){
						return *position;
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

				const T& operator[](uint64_t k){
					if(version == pvector->revision){
						if (k >= pvector->storage_size) {
							throw std::out_of_range{"index out of range"};
						}
						return position[k];
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

				const_iterator& operator+(uint32_t n){
					if(version == pvector->revision){
						this->position = position + n;
						return *this;
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

				const_iterator& operator-(uint32_t n){
					if(version == pvector->revision){
						this->position = position - n;
						return *this;
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
						}
					}

				const_iterator& operator-(const_iterator& that){
					if(version == pvector->revision){
						uint64_t diff = this->position - that.position;
						this->position = this->position - diff;
						return *this;
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

				const_iterator& operator=(const const_iterator& rhs){
					if(this != &rhs){
						this->position = rhs.position;
						this->version = rhs.version;
						this->pvector = rhs.pvector;
					}
					return *this;
				}

				const_iterator& operator+= (uint32_t n){
					if(version == pvector->revision){
						this->position = position + n;
						return *this;
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

				const_iterator& operator-= (uint32_t n){
					if(version == pvector->revision){
						this->position = position - n;
						return *this;
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

				const_iterator& operator++(void) {
					if(version == pvector->revision){
						position = ++position;
						return *this;
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

				const_iterator operator++(int) {
					if(version == pvector->revision){
						const_iterator t{*this}; // copy myself
						this->operator++(); // increment myself (pre-increment)
						return t; // return the copy of my old value
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

				bool operator==(const const_iterator& rhs) const {
					if(version == pvector->revision){
						return this->position == rhs.position;
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

				bool operator!=(const const_iterator& rhs) const {
					if(version == pvector->revision){
						return ! (*this == rhs);
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

				bool operator<(const const_iterator& rhs) const{
					if(version == pvector->revision){
						return this->position < rhs.position;
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

				bool operator>(const const_iterator& rhs) const{
					if(version == pvector->revision){
						return this->position > rhs.position;
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

				bool operator<=(const const_iterator& rhs) const{
					if(version == pvector->revision){
						return this->position <= rhs.position;
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

				bool operator>=(const const_iterator& rhs) const{
					if(version == pvector->revision){
						return this->position >= rhs.position;
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

				const_iterator& operator--(void) {
					if(version == pvector->revision){
						position = --position;
						return *this;
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}

				const_iterator operator--(int) {
					if(version == pvector->revision){
						const_iterator t{*this};
						this->operator--();
						return t;
					}
					else{
						throw epl::invalid_iterator(epl::invalid_iterator::MODERATE);
					}
				}
			};

			iterator begin(void) {
				return iterator(this);
			}

			iterator end(void) {
				iterator result(this, (front + len));
				return result;
			}

			const_iterator begin(void) const {
				return const_iterator(this);
			}

			const_iterator end(void) const {
				const_iterator result(this, (front + len));
				return result;
			}

	private:
		void copy(const vector& that) {
			len = that.len;
			if (that.storage) {
				fcapacity = that.fcapacity;
				bcapacity = that.bcapacity;
				storage_size = that.storage_size;
				storage = allocator.allocate(storage_size);
				for (uint32_t k = 0; k < len; k += 1) {
					new(storage + k)T(that.storage[k]);
				}
				front = storage;
			}
			else {
				storage = nullptr;
				fcapacity = 0;
				bcapacity = 0;
				storage_size = 0;
				front = storage;
			}
		}

		void destroy(void) {
			for(int i = 0; i < len; ++i){
				front[i].~T();
			}
			allocator.deallocate(storage, storage_size);
			front = nullptr;
		}

		void move(vector&& that){
			fcapacity = that.fcapacity;
			bcapacity = that.bcapacity;
			storage_size = that.storage_size;
			len = that.len;
			storage = that.storage;
			front = storage;
			that.storage = nullptr;
			that.fcapacity = 0;
			that.bcapacity = 0;
			that.storage_size = 0;
			that.len = 0;
			that.front = nullptr;
		}

	};

} //namespace epl

#endif

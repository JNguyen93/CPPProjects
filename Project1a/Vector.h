// Vector.h -- header file for Vector data structure project

#pragma once
#ifndef _Vector_h
#define _Vector_h

#include <cstdint>
#include <stdexcept>
#include <iostream>

namespace epl{

	template <typename T>
	class vector {
	private:
		std::allocator<T> allocator;
		T* storage;
		T* front;
		uint32_t fcapacity;
		uint32_t bcapacity;
		uint32_t len;
		uint32_t storage_size;

	public:
		vector(void){
			storage = new T[8];
			fcapacity = 0;
			bcapacity = 8;
			len = 0;
			storage_size = 8;
			front = storage;
		}

		explicit vector(uint64_t n){
			if(n > 0){
				storage = new T[n];
				fcapacity = 0;
				bcapacity = n;
				len = n;
				storage_size = n;
				front = storage;
			}
			else if(n == 0){
				storage = new T[8];
				storage_size = 8;
				fcapacity = 0;
				bcapacity = 8;
				len = 0;
				front = storage;
			}
			else{
				throw std::out_of_range{"Negative Index!"};
			}
		}

		vector(const vector& ptr){
			copy(ptr);
		}

		~vector(void) {
			destroy();
		}

		vector(vector&& that){
			move(that);
		}

		vector& operator= (const vector& rhs){
			if (this != &rhs) {
				destroy();
				copy(rhs);
			}
			return *this;
		}

		vector& operator= (vector&& rhs){
			move(rhs);
			return *this;
		}

		uint64_t size(void) const {
			return len;
		}

		T& operator[](uint64_t k){
			if (k >= storage_size) {
				throw std::out_of_range{"index out of range whut"};
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
			T elem(data);
			if(bcapacity > 0){
				front[len] = elem;
				bcapacity--;
				len++;
			}
			else{
				bcapacity = bcapacity + storage_size;
				storage_size *= 2;
				T* temp = allocator.allocate(storage_size);
				for(uint32_t i = 0; i < len; i++){
					temp[i] = storage[i];
				}
				fcapacity = 0;
				storage = temp;
				storage[len] = elem;
				front = storage;
				bcapacity--;
				len++;
			}
		}

		void push_back(T&& data){
			T elem(data);
			if(bcapacity > 0){
				front[len] = elem;
				bcapacity--;
				len++;
			}
			else{
				bcapacity = bcapacity + storage_size;
				storage_size *= 2;
				T* temp = allocator.allocate(storage_size);
				for(uint32_t i = 0; i < len; i++){
					temp[i] = storage[i];
				}
				fcapacity = 0;
				storage = temp;
				storage[len] = elem;
				front = storage;
				bcapacity--;
				len++;
				}
		}

		void push_front(const T& data){
			T elem(data);
			if(fcapacity > 0){
				*(front - 1) = elem;
				fcapacity--;
				len++;
			}
			else{
				fcapacity = fcapacity + (storage_size / 2);
				storage_size *= 3/2;
				T* temp = allocator.allocate(storage_size);
				temp[fcapacity - 1] = elem;
				for(uint32_t i = 0; i < len; i++){
					temp[i + fcapacity] = storage[i];
				}
				storage = temp;
				storage[len] = data;
				front = &storage[fcapacity - 1];
				fcapacity--;
				len++;
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
					}
				}

	private:
		void copy(const vector& that) {
			len = that.len;
				if (that.storage) {
					fcapacity = that.fcapacity;
					bcapacity = that.bcapacity;
					storage_size = that.storage_size;
					storage = allocator.allocate(storage_size);
					for (uint32_t k = 0; k < storage_size; k += 1) {
						storage[k] = that.storage[k];
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
			delete[] storage;
		}

		void move(vector&& that){
			fcapacity = that.fcapacity;
			bcapacity = that.bcapacity;
			storage_size = that.storage_size;
			len = that.len;
			storage = that.storage;
			that.storage = nullptr;
			that.fcapacity = 0;
			that.bcapacity = 0;
			that.storage_size = 0;
			that.len = 0;
		}

	};

} //namespace epl

#endif /* _Vector_h */

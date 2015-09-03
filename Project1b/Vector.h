// Vector.h -- header file for Vector data structure project

#pragma once
#ifndef _Vector_h
#define _Vector_h

#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <stdint.h>

using std::cout;

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
			storage = (T*)operator new(sizeof(T) * 8);
			fcapacity = 0;
			bcapacity = 8;
			len = 0;
			storage_size = 8;
			front = storage;
			cout << "Constructor(Void)!\n";
		}

		explicit vector(uint64_t n){
			if(n > 0){
				storage = new T[n];
				fcapacity = 0;
				bcapacity = 0;
				len = n;
				storage_size = n;
				front = storage;
				cout << "Constructor(n)\n";
			}
			else if(n == 0){
				storage = (T*)operator new(sizeof(T) * 8);
				storage_size = 8;
				fcapacity = 0;
				bcapacity = 8;
				len = 0;
				front = storage;
				cout << "Constructor(n)\n";
			}
			else{
				throw std::out_of_range{"Negative Index!"};
			}
		}

		vector(const vector& ptr){
			copy(ptr);
			cout << "Copy constructor!\n";
		}

		~vector(void) {
			destroy();
			cout << "Destructor!\n";
		}

		vector(vector&& that){
			move(std::move(that));
			cout << "Move constructor!\n";
		}

		vector& operator= (const vector& rhs){
			if (this != &rhs) {
				destroy();
				copy(rhs);
			}
			cout << "Copy assignment!\n";
			return *this;
		}

		vector& operator= (vector&& rhs){
			move(std::move(rhs));
			cout << "Move assignment!\n";
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
			}
			else{
				bcapacity = bcapacity + storage_size;
				storage_size *= 2;
				T* temp = allocator.allocate(storage_size);
				for(uint32_t i = 0; i < len; i++){
					new(temp + i)T(std::move(front[i]));
					front[i].~T();
				}
				fcapacity = 0;
				storage = temp;
				new(storage + len)T(data);
				temp = nullptr;
				front = storage;
				bcapacity--;
				len++;
			}
		}

		void push_back(T&& data){
			if(bcapacity > 0){
				cout << "Pushing...\n";
				new(front + len)T(std::move(data));
				cout << "Pushed!!\n";
				bcapacity--;
				len++;
			}
			else{
				bcapacity = bcapacity + storage_size;
				storage_size *= 2;
				T* temp = allocator.allocate(storage_size);
				for(uint32_t i = 0; i < len; i++){
					new(temp + i)T(std::move(front[i]));
					front[i].~T();
				}
				//allocator.deallocate(storage, storage_size/2);
				fcapacity = 0;
				storage = temp;
				temp = nullptr;
				cout << "Pushing...\n";
				new(storage + len)T(std::move(data));
				cout << "Pushed!!\n";
				front = storage;
				bcapacity--;
				len++;
				}
		}

		void push_front(const T& data){
			if(fcapacity > 0){
				new(front - 1)T(data);
				fcapacity--;
				len++;
			}
			else{
				/*fcapacity = fcapacity + (storage_size / 2);
				storage_size *= 3/2;
				T* temp = allocator.allocate(storage_size);
				temp[fcapacity - 1] = elem;
				for(uint32_t i = 0; i < len; i++){
					temp[i + fcapacity] = storage[i];
				}
				storage = temp;
				storage[len] = data;
				front = storage[fcapacity - 1];
				fcapacity--;
				len++;*/
				if(bcapacity > 0){
					T* temp = allocator.allocate(storage_size);
					new(temp)T(data);
					for(uint32_t i = 0; i < len; i++){
						new(temp + (i + i))T(std::move(front[i]));
						front[i].~T();
					}
					storage = temp;
					temp = nullptr;
					front = storage;
					fcapacity = 0;
					bcapacity--;
					len++;
				}
				else{
					bcapacity = bcapacity + storage_size;
					storage_size *= 2;
					T* temp = allocator.allocate(storage_size);
					new(temp)T(data);
					for(uint32_t i = 0; i < len; i++){
						new(temp + (i + 1))T(std::move(front[i]));
						front[i].~T();
					}
					fcapacity = 0;
					storage = temp;
					temp = nullptr;
					front = storage;
					bcapacity--;
					len++;
				}
			}
		}

		void push_front(T&& data){
			if(fcapacity > 0){
				new(front - 1)T(std::move(data));
				fcapacity--;
				len++;
			}
			else{
				/*fcapacity = fcapacity + (storage_size / 2);
				storage_size *= 3/2;
				T* temp = allocator.allocate(storage_size);
				temp[fcapacity - 1] = elem;
				for(uint32_t i = 0; i < len; i++){
				temp[i + fcapacity] = storage[i];
				}
				storage = temp;
				storage[len] = data;
				front = storage[fcapacity - 1];
				fcapacity--;
				len++;*/
				if(bcapacity > 0){
					T* temp = allocator.allocate(storage_size);
					new(temp)T(std::move(data));
					for(uint32_t i = 0; i < len; i++){
						new(temp + (i + i))T(std::move(front[i]));
						front[i].~T();
					}
					storage = temp;
					temp = nullptr;
					front = storage;
					fcapacity = 0;
					bcapacity--;
					len++;
				}
				else{
					bcapacity = bcapacity + storage_size;
					storage_size *= 2;
					T* temp = allocator.allocate(storage_size);
					new(temp)T(std::move(data));
					for(uint32_t i = 0; i < len; i++){
						new(temp + (i + 1))T(std::move(front[i]));
						front[i].~T();
					}
					fcapacity = 0;
					storage = temp;
					temp = nullptr;
					front = storage;
					bcapacity--;
					len++;
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
			/*storage = allocator.allocate(storage_size);
			for(int k = 0; k < len; k++){
				new(storage + k)T(std::move(that.storage[k]));
				that.storage[k].~T();
			}*/
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

#endif /* _Vector_h */

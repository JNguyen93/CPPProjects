// Valarray.h

/* Put your solution in this file, we expect to be able to use
 * your epl::valarray class by simply saying #include "Valarray.h"
 *
 * We will #include "Vector.h" to get the epl::vector<T> class 
 * before we #include "Valarray.h". You are encouraged to test
 * and develop your class using std::vector<T> as the base class
 * for your epl::valarray<T>
 * you are required to submit your project with epl::vector<T>
 * as the base class for your epl::valarray<T>
 */

#include <utility>
#include <stdlib.h>
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <complex>
#include <string>
#include <vector>

#ifndef _Valarray_h
#define _Valarray_h

/*using std::vector; // during development and testing*/
using epl::vector; // after submission

template <typename T>
struct rank;

template <> struct rank<int> { static constexpr int value = 1; };
template <> struct rank<float> { static constexpr int value = 2; };
template <> struct rank<double> { static constexpr int value = 3; };

template <typename T>
struct rank<std::complex<T>> { static constexpr int value = rank<T>::value; };

template <uint32_t r>
struct SType;

template <> struct SType<1> { using type = int;  };
template <> struct SType<2> { using type = float; };
template <> struct SType<3> { using type = double; };

template <typename T>
struct is_complex : public std::false_type {};
template <typename T>
struct is_complex<std::complex<T>> : public std::true_type {};

template <bool p, typename T>
struct ctype;
template <typename T>
struct ctype<true, T> { using type = std::complex<T>; };
template <typename T>
struct ctype<false, T> { using type = T; };

template <typename T>
struct is_scalar;

template <> struct is_scalar<int> { using type = int;  };
template <> struct is_scalar<float> { using type = float; };
template <> struct is_scalar<double> { using type = double; };
template <typename T>struct is_scalar<std::complex<T>> : public std::true_type {};


template <typename T1, typename T2>
struct choose_type {
	static constexpr int t1_rank = rank<T1>::value;
	static constexpr int t2_rank = rank<T2>::value;
	static constexpr int max_rank = (t1_rank < t2_rank) ? t2_rank : t1_rank;

	using my_stype = typename SType<max_rank>::type;

	static constexpr bool t1_comp = is_complex<T1>::value;
	static constexpr bool t2_comp = is_complex<T2>::value;
	static constexpr  bool my_comp = t1_comp || t2_comp;

	using type = typename ctype<my_comp, my_stype>::type;
};

namespace epl{

	template<typename T>
	struct scalar {
		T val;
		scalar(T _v) : val(_v) {}

		T operator[](uint64_t index) const {
			return val;
		}

		uint64_t size() const {
			return -1;
		}
	};

	template <typename T>
	class valarray : public epl::vector<T>{
	public:
		valarray(void) : epl::vector<T>(){}

		valarray(std::initializer_list<T> list) : epl::vector<T>(list){}

		explicit valarray(uint64_t n): epl::vector<T>(n){}

		valarray(T temp) : epl::vector<T>(){
			this->push_back(temp);
		}
		valarray(const valarray<T>& rhs) {
			for(int i = 0; i < rhs.size(); i++) {
				this->push_back((T)rhs[i]);
			}
		}

		valarray<T>& operator=(const valarray<T> that) {
			if(this != &that) {
				int total_size = this->size();
				int temp = 0;
				while(temp != total_size) {
					++temp;
					this->pop_back();
				}
				for(int i = 0; i < that.size(); ++i) {
					this->push_back(that[i]);
				}
		    }
			return *this;
		}

		valarray<T>& operator=(uint64_t n){
			for(int k = 0; k < this->size(); k++){
				this->operator[](k) = n;
			}
			return *this;
		}

		valarray<T>& operator-(){
			for(int k = 0; k < this->size(); k++){
				this->operator[](k) = -(this->operator[](k));
			}
			return *this;
		}

		template <typename operand>
		T accumulate(operand op){
			T temp;
			for(int i = 0; i < this->size(); ++i){
				temp = op(temp, this->operator[](i));
			}
			return temp;
		}

		T sum(){
			return accumulate(std::plus<T>());
		}
	};

	template <typename T>
	struct to_ref{
		using type = T;
	};

	template <typename T>
	struct to_ref<valarray<T>>{
		using type = valarray<T>&;
	};

	template <typename T>
	struct nested;

	template <typename T>
	struct nested<valarray<T>>{
		using type =T ;
	};

	template <typename T>
	using Ref = typename to_ref<T>::type;

	template <typename T1, typename T2>
		using choose = typename choose_type<T1, T2>::type;

	template <typename left, typename right, typename BinaryOperator>
	class proxy{
		const Ref<left> l;
		const Ref<right> r;
		const BinaryOperator op;

		proxy(const Ref<left> _l, const Ref<right> _r, BinaryOperator _op) :
			l(_l), r(_r), op(_op) {};

		choose<Ref<left>, Ref<right>> operator[](uint64_t index){
			auto leftvalue = choose<Ref<left>, Ref<right>> (this->l[index]);
			auto rightvalue = choose<Ref<left>, Ref<right>> (this->r[index]);
			return this->op(leftvalue.getValue(), rightvalue.getValue());
		}
	};

	/*template <typename T1, typename T2>
	valarray<proxy<T1, T2, std::plus<choose<Ref<T1>,Ref<T2>>>>> operator+(const valarray<T1>& left, const valarray<T2>& right){
		return valarray<proxy<valarray<T1>&, valarray<T2>&, std::plus<choose<Ref<T1>, Ref<T2>>>>>
				(proxy<valarray<T1>&, valarray<T2>&, std::plus<choose<Ref<T1>, Ref<T2>>>>
						(left, right, std::plus<choose<Ref<T1>,Ref<T2>>>()));
	}*/

	template <typename T1, typename T2>
	valarray<choose<T1, T2>> operator+(const valarray<T1>& left, const valarray<T2>& right){
		valarray<choose<T1, T2>> temp = valarray<choose<T1, T2>>(std::min(left.size(), right.size()));
		for(int k = 0; k < std::min(left.size(), right.size()); k++){
			temp[k] = (choose<T1, T2>)left[k] + (choose<T1, T2>)right[k];
		}
		return temp;
	}

	template <typename T1, typename T2, typename T3 = typename is_scalar<T1>::type>
	valarray<choose<T1, T2>> operator+(T1 value, const valarray<T2>& right){
		valarray<choose<T1, T2>> temp = valarray<choose<T1, T2>>(right.size());
		for(int k = 0; k < right.size(); k++){
			temp[k] = scalar<T1>(value)[k] + right[k];
		}
		return temp;
	}

	template <typename T1, typename T2, typename T3 = typename is_scalar<T2>::type>
	valarray<choose<T1, T2>> operator+(const valarray<T1>& left, T2 value){
		valarray<choose<T1, T2>> temp = valarray<choose<T1, T2>>(left.size());
		for(int k = 0; k < left.size(); k++){
			temp[k] = left[k] + scalar<T1>(value)[k];
		}
		return temp;
	}

	template <typename T1, typename T2>
	valarray<choose<T1, T2>> operator-(const valarray<T1>& left, const valarray<T2>& right){
		valarray<choose<T1, T2>> temp = valarray<choose<T1, T2>>(std::min(left.size(), right.size()));
		for(int k = 0; k < std::min(left.size(), right.size()); k++){
			temp[k] = (choose<T1, T2>)left[k] - (choose<T1, T2>)right[k];
		}
		return temp;
	}

	template <typename T1, typename T2, typename T3 = typename is_scalar<T1>::type>
	valarray<choose<T1, T2>> operator-(T1 value, const valarray<T2>& right){
		valarray<choose<T1, T2>> temp = valarray<choose<T1, T2>>(right.size());
		for(int k = 0; k < right.size(); k++){
			temp[k] = scalar<T1>(value)[k] - right[k];
		}
		return temp;
	}

	template <typename T1, typename T2, typename T3 = typename is_scalar<T2>::type>
	valarray<choose<T1, T2>> operator-(const valarray<T1>& left, T2 value){
		valarray<choose<T1, T2>> temp = valarray<choose<T1, T2>>(left.size());
		for(int k = 0; k < left.size(); k++){
			temp[k] = left[k] - scalar<T1>(value)[k];
		}
		return temp;
	}

	template <typename T1, typename T2>
	valarray<choose<T1, T2>> operator*(const valarray<T1>& left, const valarray<T2>& right){
		valarray<choose<T1, T2>> temp = valarray<choose<T1, T2>>(std::min(left.size(), right.size()));
		for(int k = 0; k < std::min(left.size(), right.size()); k++){
			temp[k] = (choose<T1, T2>)left[k] * (choose<T1, T2>)right[k];
		}
		return temp;
	}

	template <typename T1, typename T2, typename T3 = typename is_scalar<T1>::type>
	valarray<choose<T1, T2>> operator*(T1 value, const valarray<T2>& right){
		valarray<choose<T1, T2>> temp = valarray<choose<T1, T2>>(right.size());
		for(int k = 0; k < right.size(); k++){
			temp[k] = scalar<T1>(value)[k] * right[k];
		}
		return temp;
	}

	template <typename T1, typename T2, typename T3 = typename is_scalar<T2>::type>
	valarray<choose<T1, T2>> operator*(const valarray<T1>& left, T2 value){
		valarray<choose<T1, T2>> temp = valarray<choose<T1, T2>>(left.size());
		for(int k = 0; k < left.size(); k++){
			temp[k] = left[k] * scalar<T1>(value)[k];
		}
		return temp;
	}

	template <typename T1, typename T2>
	valarray<choose<T1, T2>> operator/(const valarray<T1>& left, const valarray<T2>& right){
		valarray<choose<T1, T2>> temp = valarray<choose<T1, T2>>(std::min(left.size(), right.size()));
		for(int k = 0; k < std::min(left.size(), right.size()); k++){
			temp[k] = (choose<T1, T2>)left[k] / (choose<T1, T2>)right[k];
		}
		return temp;
	}

	template <typename T1, typename T2, typename T3 = typename is_scalar<T1>::type>
	valarray<choose<T1, T2>> operator/(T1 value, const valarray<T2>& right){
		valarray<choose<T1, T2>> temp = valarray<choose<T1, T2>>(right.size());
		for(int k = 0; k < right.size(); k++){
			temp[k] = scalar<T1>(value)[k] / right[k];
		}
		return temp;
	}

	template <typename T1, typename T2, typename T3 = typename is_scalar<T2>::type>
	valarray<choose<T1, T2>> operator/(const valarray<T1>& left, T2 value){
		valarray<choose<T1, T2>> temp = valarray<choose<T1, T2>>(left.size());
		for(int k = 0; k < left.size(); k++){
			temp[k] = left[k] / scalar<T1>(value)[k];
		}
		return temp;
	}

	template <typename T>
	std::ostream& operator<<(std::ostream& os, const valarray<T>& ar){
		std::string comma = "";
		for(int i = 0; i < ar.size(); i++){
			os << comma << ar[i];
			comma = ", ";
		}
		os << std::endl;
		return os;
	}

}

#endif /* _Valarray_h */


#include <cstdint>
#include <iostream>
#include <stdexcept>

#include "Vector.h"

using std::cout;
using std::endl;
using std::begin;
using std::end;

using epl::vector;

int main(void){
	/*vector<int> x;
	cout << "x size = " << x.size() << "\n";
	x.push_back(42);
	cout << "Pushed 42 to x!\n";
	cout << "x size = " << x.size() << "\n";
	cout << "x[0] = " << x[0] << "\n";
	cout << "x[0] to 10!\n";
	x[0] = 10;
	cout << "x[0] = " << x[0] << "\n";
	x.pop_back();
	cout << "Popped back of x!\n";
	cout << "Size = " << x.size() << "\n";
	cout << "x[0] = " << x[0] << "\n";
	x.push_back(30);
	cout << "Pushed 30 to x!\n";
	cout << "x size = " << x.size() << "\n";
	cout << "x[0] = " << x[0] << "\n";
	x.pop_front();
	cout << "Popped front of x!\n";
	cout << "Size = " << x.size() << "\n";
	cout << "x[0] = " << x[0] << "\n";
	x.push_back(42);
	cout << "Pushed 42 to back of x!\n";
	cout << "x size = " << x.size() << "\n";
	cout << "x[0] = " << x[0] << "\n";

	for (int k = 0; k < 10; k += 1) {
		cout << "Push " << k << "\n";
		x.push_back(k);
		cout << "Pushed " << k << "\n";
		cout << "Pushing " << k << "\n";
	    x.push_front(k);
		cout << "Pushed " << k << "\n";
		for(int i = 0; i < x.size(); i++){
				cout << x[i] << " ";
		}
		cout << "\n";
	}

	for(int i = 0; i < x.size(); i++){
		cout << x[i] << " ";
	}*/

	const vector<int> x{ 1, 2, 3 };
	auto it = begin(x);
	auto it2 = end(x);
	it2 - it2;
	cout << *it2 << "\n";
	it2 = end(x);
	//*it = 4;
	cout << *it << "\n";
	it += 2;
	cout << *it << "\n";
	it -= 2;
	cout << *it << "\n";
	it + 2;
	cout << *it << "\n";
	it - 2;
	cout << *it << "\n";
	++it;
	cout << *it << "\n";
	it++;
	cout << *it << "\n";
	cout << *it << "\n";
	--it;
	cout << *it << "\n";
	it--;
	cout << *it << "\n";
	if(it < it2){
		cout << "Works1\n";
	}
	if(!(it > it2)){
		cout << "Works2\n";
	}
	if(!(it == it2)){
		cout << "Works3\n";
	}
	if(it != it2){
		cout << "Works4\n";
	}
	if(it <= it){
		cout << "Works5\n";
	}
	if(it >= it){
		cout << "Works6\n";
	}
	if(it <= it2){
		cout << "Works7\n";
	}
	if(!(it >= it2)){
		cout << "Works8\n";
	}
	cout << it[0] << "\n";
	cout << it[1] << "\n";
	cout << it[2] << "\n";

	for(int i = 0; i < x.size(); i++){
		cout << x[i] << " ";
	}
	cout << "\n";

	/*vector<int32_t> x(10);

	    int32_t k = 0;
	    for (auto& v : x) {
	        v = k++;
	    }

	    const vector<int32_t>& y = x;
	    int32_t s = 0;
	    for (const auto& v : y) {
	        s += v;
	    }
	    cout << s << "\n";*/

	/*vector<uint32_t> x(10);
	    auto it = x.begin();
	    x.push_front(42);
	    try {
	        *it;
	        cout << "Failed.\n";
	    } catch (epl::invalid_iterator exc) {
	    	cout << exc.level << "\n";
	    } catch (...) {
	        cout << "Failed wrong exception.\n";
	    }*/

	/*vector<uint32_t> x {0, 1 ,2};

	vector<uint32_t> y(x.begin(), x.end());
	for(int i = 0; i < x.size(); i++){
			cout << x[i] << " ";
		}
	cout << "\n";
	for(int i = 0; i < y.size(); i++){
		cout << y[i] << " ";
	}
	cout << "\n";*/
}

#include <cstdint>
#include <iostream>
#include <stdexcept>

#include "Vector.h"

using std::cout;
using std::endl;

using epl::vector;

int main(void){
	vector<int> x;
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

	for (int k = 0; k < 10; k += 1) {
		x.push_back(k);
	    x.push_front(k);
	}

	for(int i = 0; i < x.size(); i++){
		cout << x[i] << " ";
	}

}

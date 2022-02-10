#include <iostream>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <cstdlib> //std::system
#include <sstream>
using namespace std;
using namespace boost::interprocess;

//thực hiện ghi vào shared memory rồi đọc ra màn hình
int main()
{
	// delete SHM if exists
	shared_memory_object::remove("my_shm");
	// create a new SHM object and allocate space
	managed_shared_memory managed_shm(open_or_create, "my_shm", 1024);

	// write into SHM
	// Type: int, Name: my_int, Value: 99
	int *i = managed_shm.construct<int>("my_int")(99);
	cout << "Write into shared memory: "<< *i << '\n';

	// write into SHM
	// Type: std::string, Name: my_string, Value: "Hello World"
	string *sz = managed_shm.construct<string>("my_string")("Hello World");
	cout << "Write into shared memory: "<< *sz << '\n' << '\n';

	// read INT from SHM
	//Tries to find a previously created object. Returns a pointer to the object and the
	//count (if it is not an array, returns 1). If not present, the returned pointer is 0
	pair<int*, size_t> pInt = managed_shm.find<int>("my_int");

	if (pInt.first) 
		cout << "Read from shared memory: "<< *pInt.first << '\n';
	else 
		cout << "my_int not found" << '\n';

	// read STRING from SHM
	pair<string*, size_t> pString = managed_shm.find<string>("my_string");

	if (pString.first)
		cout << "Read from shared memory: "<< *pString.first <<'\n';	
	else 
		cout << "my_string not found" << '\n';

	managed_shm.destroy<int>("my_int");
	managed_shm.destroy<string>("my_string");
	// delete SHM if exists
	shared_memory_object::remove("my_shm");

	return 0;
}
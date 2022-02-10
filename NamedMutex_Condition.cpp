#include <iostream>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <cstdlib> //std::system
#include <sstream>
#include <windows.h>
using namespace std;
using namespace boost::interprocess;

//server sẽ thay đổi giá trị sau 2s, đợi client đọc vào xong mới tiếp tục thay đổi
//client sẽ đợi server giải phóng lock bằng wait(lock) rồi đọc giá trị, xong báo cho bên server biết và tiếp tục chờ lượt kế
//sau khi đọc xong giá trị cuối cùng client thông báo cho server để nó giải phóng

//Lưu ý: sd condition chỉ là một cách báo hiệu chứ không làm thay đổi lock => chỉ phù hợp để trao đổi qua lại luân phiên
int main(int argc, char *argv[])
{
    try
    {
        if (argc == 1)
        {
            cout << "Short of argument!" << endl;
            return 0;
        }
        string opt = argv[1];

        // create a new SHM object and allocate space
        managed_shared_memory managed_shm(open_or_create, "my_shm", 1024);
        named_mutex mtx(open_or_create, "my_mutex");
        named_condition cond(open_or_create, "my_cond");
        scoped_lock<named_mutex> lock(mtx);

        if (opt == "server")
        {
            int *i = managed_shm.construct<int>("my_int")(1);
            cout << "Write into shared memory: " << endl;
            cout << "Address of i: " << i << endl;

            for (int j = 1; j < 6; j++)
            {
                *i = j;
                cout << *i << " ";
                cond.notify_all(); //thông báo giải phóng
                cond.wait(lock); //giải phóng khoá lock
                Sleep(2000);
            }

            managed_shm.destroy<int>("my_int");       
            mtx.~named_mutex();
            cond.~named_condition();      
            // delete SHM if exists
            shared_memory_object::remove("my_shm");
            named_mutex::remove("my_mutex");
            named_condition::remove("my_cond");
        }
        else //if client
        {
            pair<int *, size_t> pInt;
            cout << "Read from shared memory: " << endl;
            pInt = managed_shm.find<int>("my_int");
            if (!pInt.first)
                cout << "Can't find my_int" << endl;
            else
                cout << "Address of i: " << pInt.first << endl;
                while (1)
                {
                    cout << *pInt.first << " ";    
                    //Sleep(1000);  
                    if (*pInt.first == 5)
                    {
                        cond.notify_all(); //nếu hết rồi thì thông báo bên server để nó giải phóng
                        break;
                    }              
                    cond.notify_all();
                    cond.wait(lock);                               
                    pInt = managed_shm.find<int>("my_int");
                }     
            managed_shm.destroy<int>("my_int");
            mtx.~named_mutex();
            cond.~named_condition(); 
            // delete SHM if exists
            shared_memory_object::remove("my_shm");
            named_mutex::remove("my_mutex");
            named_condition::remove("my_cond");
        }          
    }
    catch (interprocess_exception &e)
    {
        cout << e.what() << '\n';
    }
    return 0;
}
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
//Sử dụng khoá lock để khoá vùng nhớ khi server ghi dữ liệu, sau đó thông báo cho bên kia biết đã ghi rồi và mở lock
//client chỉ việc chờ tới khi server mở lock để vô lấy
//riêng giá trị đầu tiên thì server phải chờ cho client chạy rồi mới được qua tiếp
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

        if (opt == "server") //trường hợp lần chạy trước bị lỗi và chương trình bị ngắt ngang khi chưa xoá vùng nhớ
        {
            shared_memory_object::remove("my_shm");
            named_mutex::remove("my_mutex");
            named_condition::remove("my_cond");
        }

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
                if (!lock.owns()) //khoá lại khi ghi dữ liệu, ghi xong thì thông báo cho bên kia biết và mở khoá ra
                    lock.lock();
                *i = j;
                cout << *i << " ";
                cond.notify_all(); //thông báo giải phóng
                if (*i == 1) //chỉ chờ client khi bắt đầu giá trị đầu tiên, các giá trị sau không chờ nữa
                    cond.wait(lock); //giải phóng khoá lock
                else if (lock.owns())
                    lock.unlock();
                //cond.wait(lock);
                Sleep(500);
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
                    if (*pInt.first == 5)
                        break;

                    if (*pInt.first == 1)
                        cond.notify_all(); //giá trị đầu tiên thông báo cho bên server để nó giải phóng lệnh wait(lock)
                    cond.wait(lock);
                    //pInt = managed_shm.find<int>("my_int");
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
        cout << e.get_error_code() << " " << e.what() << '\n';
    }
    return 0;
}
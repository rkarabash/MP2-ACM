#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <string>
#include <fstream>
#include <condition_variable>
#include <ctime>
using namespace std;
mutex check, rent, print;
bool is_notified = false;
condition_variable avaibility;
int guest_id, pr_time, num_guests = 0, ids = 1;
// Логирование происходящего
void log(string message) {
    print.lock();
    cout << "[Time " + to_string((clock() - pr_time) / CLOCKS_PER_SEC) + "] " + message << endl;
    print.unlock();
}

// Метод приема гостей отеля
void enter_hotel() {

    while (true)
    {
        unique_lock<mutex> locker(check);
        while (!is_notified)
            avaibility.wait(locker);
        log("(+) Hotel meets Guest #" + to_string(guest_id));
        num_guests++;
        is_notified = false;
    }
}

// Попытка гостя заселиться в отель
void try_rent(int id) {
    {
        unique_lock<mutex> lock(rent);
        log("(?) Guest #" + to_string(id) + " is trying to rent room... ");
        this_thread::sleep_for(chrono::milliseconds(100));
        bool flag = true;
        while (num_guests >= 30)
        {
            if (flag)
            {
                log("(!) Oh no! Hotel has no free rooms! Guest #" + to_string(id) + " must waiting outside... ");
                flag = false;
            }
        }
        guest_id = id;
        avaibility.notify_one();
        is_notified = true;
    }
    {
        unique_lock<mutex> lock();
        srand(time(nullptr));
        this_thread::sleep_for(chrono::milliseconds((rand() % 10 + 1) * 1000));
        log("(-) Guest #" + to_string(id) + " leave hotel... ");
        num_guests--;
    }
}
int main()
{
    cout << "Karabash Radimir BSE195" << endl;
    cout << "Variant 13. The hotel problem. There are 30 rooms in the hotel, hotel clients rent a room for one night, if there are nofree rooms in the hotel, clients settle for the night near the hotel and wait until any room is available. Create a multi-threaded application that simulates hotel operations." << endl;
    cout << "Enter the number of iterations. One iteration equals 300 guests checking into a hotel on one day:" << endl;
    int n, k = 0;
    cin >> n;
    if (n <= 0 || n > 100)
    {
        cout << "Invalid number of iterations! This number must be more than 0 and less or equal then 100!";
        return 0;
    }
    pr_time = clock();
    thread hotel(enter_hotel);
    vector<thread> threads;
    while (k < n)
    {
        threads.clear();
        log("Day #" + to_string(k + 1) + " started...");
        for (size_t i = 0; i < 300; i++)
        {
            threads.push_back(thread(try_rent, ids++));
        }
        for (size_t i = 0; i < threads.size(); i++)
        {
            threads[i].join();
        }

        k++;
        log("Day #" + to_string(k) + " came to an end...");
    }
    hotel.detach();
    cout << "The hotel served " + to_string(--ids) + " clients!" << endl;
    return 0;
}


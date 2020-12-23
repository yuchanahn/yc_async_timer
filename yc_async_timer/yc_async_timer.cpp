#include <iostream>

#include "yc_async_timer.hpp"


#include <iostream> 
#include <future> 
#include <functional>
#include <thread>
#include <coroutine>
#include <chrono>

using namespace std;


future<void> async_timer(float n, std::function<void()> f) {
	float dt = 0;
	while (true)
	{
		int ms = (n * 1000.f);
		bool flg = false;
		co_await std::async([&] { 
			std::this_thread::sleep_for(std::chrono::milliseconds(ms));
			flg = true;
		});
		
		if (flg)
		{
			break;
		}
	}
	co_return f();
} 


int main() { 
	cout << "Start" << endl; 
	auto f = async_timer(25.f, [] { printf("25ÃÊ Áö³²\n"); });
	while (!f._Is_ready()) { 
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		cout << "Main!" << endl; 
	} 
	cout << endl; 
	return 0; 
}


#include "yc_async_timer.hpp"

#include <stdio.h>
#include <time.h>
#include <iostream>

int main() {
	using namespace std;

	cout << "Start" << endl;
	bool stop = false;

	for (int i = 1; i < 1000; i++)
	{
		async_timer(i * 0.1f, [&, i] {
			cout << endl << i * 0.1f <<" sec over";
			if (i == 99) stop = true;
		});
	}

	cout << "timer set!" << endl;

	while (!stop) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	cout << endl;
	return 0;
}
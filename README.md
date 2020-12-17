## async timer in c++ 20

```cpp
#include "yc_async_timer.hpp"

#include <iostream>

int main()
{
	bool s = false;
	yc_timer::timer_event([] { printf("0.001sec...\n"); }, 0.001f);
	yc_timer::timer_event([] { printf("1sec...\n"); }, 1.f);
	yc_timer::timer_event([] { printf("2sec...\n"); }, 2.f);
	yc_timer::timer_event([] { printf("2sec...\n"); }, 2.f);
	yc_timer::timer_event([&s] { printf("2sec...\n");  s = true; }, 2.f);
	yc_timer::timer_event([&s] { printf("3sec...\n"); }, 3.f);

	while (!s);
	yc_timer::shutdown_timer();
}
```

/std:c++latest
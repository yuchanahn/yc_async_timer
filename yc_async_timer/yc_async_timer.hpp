#pragma once
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <stdexcept>
#include <coroutine>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <atomic>
#include <concurrent_unordered_map.h>

using namespace std;


struct job_info_t
{
	float t;
	float dt = 0.f;
	std::chrono::system_clock::time_point start_t;
	std::function<void()> f;
	bool active = true;
};

static concurrency::concurrent_unordered_map <size_t, job_info_t> jobs;
static std::atomic_int64_t offset = 0;

static std::thread timer_thread = std::thread([] {
	while (true) {
		static std::chrono::system_clock::time_point* lastT = nullptr;
		if (lastT == nullptr)
		{
			lastT = new std::chrono::system_clock::time_point;
			*lastT = std::chrono::system_clock::now();
		}
		else {
			auto this_t = std::chrono::system_clock::now();
			for (auto& i : jobs) {
				if (i.second.active) {
					if(i.second.dt == 0.f) 
						i.second.dt = ((std::chrono::duration<float>)(this_t - i.second.start_t)).count();
					else {
						i.second.dt += ((std::chrono::duration<float>)(this_t - *lastT)).count();
					}
					if (i.second.dt >= i.second.t) {
						i.second.f();
					}
				}
			}
		}
		*lastT = std::chrono::system_clock::now();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
});

struct task {

	struct promise_type {
		task get_return_object() {
			return task{
				nullptr,
				0.f,
				(std::function<void()>&)[] {}
			};
		}
		std::suspend_never initial_suspend() { return {}; }
		std::suspend_never final_suspend() noexcept { return {}; }
		void return_void() {}
		void unhandled_exception() {}
	};

	bool await_ready() { return false; }
	void await_suspend(std::coroutine_handle<> h) {
		//printf("%lld,", offset.load());
		size_t idx = -1;
		for (auto& i : jobs) {
			if (!i.second.active) idx = i.first;
		}
		idx = idx == -1 ? offset++ : idx;
		jobs[idx] = job_info_t{
			.t = _ms,
			.dt = 0.f,
			.start_t = std::chrono::system_clock::now(),
			.f = [func = &_func, p = _p, idx, h] { 
				jobs[idx].active = false; 
				p->set_value(*func);
				h.resume();
			},
			.active = true
		};
	}
	void await_resume() {}
	std::promise<std::function<void()>>* _p = nullptr;
	float _ms = 0.f;
	std::function<void()>& _func;
	task(
		std::promise<std::function<void()>>* p, 
		float ms, 
		std::function<void()>& func
		) : _p(p), _ms(ms), _func(func){ }
};

task async_timer(float n, std::function<void()> f) {
	std::promise<std::function<void()>> f_;
	auto ftr = f_.get_future();
	co_await task(&f_, n, f);
	if (ftr._Is_ready()) {
		ftr.get()();
	}
}
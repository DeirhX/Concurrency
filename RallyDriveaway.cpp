#include <vector>
#include <iostream>
#include <semaphore>
#include <print>
#include <mutex>
#include <thread>
#include <concepts>
#include <ranges>
#include <algorithm>

class drive_away
{
public:
    void call_obama() 
    {
        {
            std::unique_lock lock{ on_call };
            std::println("Obama calling.");
            democrats_on_call++;
            dispatch();
        }
		democrats.acquire();
        std::println("Obama is driving away");
    }
    void call_trump() 
    {
        {
            std::unique_lock lock{ on_call };
            std::println("Trump calling.");
            republicans_on_call++;
            dispatch();
        }
        republicans.acquire();
        std::println("Trump is driving away");
    }
private:
    void dispatch()
    {
        print_status("Before:");
        if (democrats_on_call >= 4)
        {
            democrats.release(4);
            democrats_on_call -= 4;
            std::println("Democrats are driving away");
        }
        else if (republicans_on_call >= 4)
		{
			republicans.release(4);
            republicans_on_call -= 4;
			std::println("Republicans are driving away");
		}
        else if (democrats_on_call >= 2 && republicans_on_call >= 2)
		{
			democrats.release(2);
			republicans.release(2);
            democrats_on_call -= 2;
            republicans_on_call -= 2;
			std::println("Democrats and Republicans are driving away");
		}
        print_status("After:");
    }

    void print_status(std::string_view preface)
	{
		std::println("{} Democrats: {}, Republicans: {}", preface, democrats_on_call.load(std::memory_order_relaxed), republicans_on_call.load(std::memory_order_relaxed));
	}

    std::counting_semaphore<std::numeric_limits<int>::max()> democrats{ 0 };
    std::counting_semaphore<std::numeric_limits<int>::max()> republicans{ 0 };
    
    std::mutex on_call;
    std::atomic<int> democrats_on_call = 0;
    std::atomic<int> republicans_on_call = 0;
};

std::vector<std::jthread> queue(std::invocable auto&& operation, int amount)
{
    std::vector<std::jthread> threads;
    for (int i = 0; i < amount; ++i)
    {
        threads.emplace_back([operation]() {
            operation();
        });
    }
    return threads;
}

int main()
{
    auto uber = drive_away{};
    std::println("Hello Fools!");
    auto threads = queue([&] {uber.call_trump(); }, 10);
    std::ranges::move(queue([&] {uber.call_obama(); }, 10), std::back_inserter(threads));
    std::ranges::move(queue([&] {uber.call_trump(); }, 1), std::back_inserter(threads));
    std::ranges::move(queue([&] {uber.call_obama(); }, 1), std::back_inserter(threads));
    std::ranges::move(queue([&] {uber.call_trump(); }, 1), std::back_inserter(threads));
    std::ranges::move(queue([&] {uber.call_obama(); }, 1), std::back_inserter(threads));
    return 0;
}

#include <chrono>
#include <thread>
#include <vector>
#include <cstdio>

#define DELAY 10ms
#define COUNT 1000
#define EPS 1.ms

int main() {
  using namespace std::chrono_literals;
  printf("======== Start of Timer Benchmark ========\n");
  if (COUNT <= 0) {
    printf("error: incorrect number of intervals %u\n", COUNT);
    printf("========= End of Timer Benchmark =========\n");
    return 1;
  }
  std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>> Intervals;
  Intervals.resize(COUNT + 1);
  for (auto I{0u}; I < COUNT; ++I) {
    Intervals[I] = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(DELAY);
  }
  Intervals[COUNT] = std::chrono::high_resolution_clock::now();
  auto NumberOfExceeded{0u};
  using TP = std::chrono::duration<double, std::milli>;
  std::pair<TP, unsigned> MinInterval{TP::max(), 0u};
  std::pair<TP, unsigned> MaxInterval{TP::min(), 0u};
  TP AverageInterval{0ms};
  for (auto I{1u}; I <= COUNT; ++I) {
    auto Interval{Intervals[I] - Intervals[I - 1]};
    if (Interval > (DELAY + EPS))
      ++NumberOfExceeded;
    if (MinInterval.first > Interval) {
      MinInterval.first = Interval;
      MinInterval.second = I;
    }
    if (MaxInterval.first < Interval) {
      MaxInterval.first = Interval;
      MaxInterval.second = I;
    }
    AverageInterval += Interval;
  }
  AverageInterval /= COUNT;

  printf("Number of intervals:            %10d\n", COUNT);
  printf("Duration of an interval:        %10lld\n", DELAY.count());
  printf("Measure of inaccuracy (msec):   %10.3f\n", EPS.count());
  printf("Number of exceeded intervals    %10u\n", NumberOfExceeded);
  printf("Minimum duration (msec):        %10.3f\n", MinInterval.first.count());
  printf("Minimum duration at:            %10u\n", MinInterval.second);
  printf("Maximum duration (msec):        %10.3f\n", MaxInterval.first.count());
  printf("Maximum duration at:            %10u\n", MaxInterval.second);
  printf("Average duration (msec)         %10.3f\n", AverageInterval.count());
  printf("========= End of Timer Benchmark =========\n");
  return 0;
}

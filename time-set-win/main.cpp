//===--- time_set_win.c ------ Windows Timer Benchmark ----------*- C++ -*-===//
//
// The benchmark checks the accuracy of OS Windows timer set with timeSetEven().
//
//----------------------------------------------------------------------------//

#include <windows.h>
#include <vector>
#include <stdio.h>

#define DELAY 10  // msec
#define EPS 1000  // usec
#define COUNT 10000

HANDLE EndEvent = NULL;
unsigned Count = 0;

std::vector<LARGE_INTEGER> Intervals;

void timeCallback(UINT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR) {
  QueryPerformanceCounter(&Intervals[++Count]);
  if (Count < COUNT)
    timeSetEvent(DELAY, 1, &timeCallback, 0, TIME_ONESHOT);
  else
    SetEvent(EndEvent);
}

int main() {
  printf("======== Start of Timer Benchmark ========\n");
  if (COUNT <= 0) {
    printf("error: incorrect number of intervals %u\n", COUNT);
    printf("========= End of Timer Benchmark =========\n");
    return 1;
  }
  EndEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("EndEvent"));
  if (EndEvent == NULL) {
    printf("error: unable to create event handler\n");
    printf("========= End of Timer Benchmark =========\n");
    return 2;
  }
  Count = 0;
  Intervals.resize(COUNT + 1);
  QueryPerformanceCounter(&Intervals[Count]);
  timeSetEvent(DELAY, 1, &timeCallback, 0, TIME_ONESHOT);
  if (WaitForSingleObject(EndEvent, INFINITE) != WAIT_OBJECT_0) {
    printf("error: wait for event error\n");
    printf("========= End of Timer Benchmark =========\n");
    return 3;
  }
  CloseHandle(EndEvent);
  LARGE_INTEGER TimeFreauency;
  QueryPerformanceFrequency(&TimeFreauency);
  unsigned NumberOfExceeded = 0;
  std::pair<LONGLONG, unsigned> MinInterval(2 * DELAY * 1000, 0u);
  std::pair<LONGLONG, unsigned> MaxInterval(0, 0u);
  double AverageInterval = 0;
  for (unsigned I = 1u; I <= COUNT; ++I) {
    LONGLONG Interval =
        ((Intervals[I].QuadPart - Intervals[I - 1].QuadPart) * 1000000.) /
        TimeFreauency.QuadPart;
    if (Interval > (DELAY * 1000) + EPS)
      ++NumberOfExceeded;
    if (MinInterval.first > Interval) {
      MinInterval.first = Interval;
      MinInterval.second = I;
    }
    if (MaxInterval.first < Interval) {
      MaxInterval.first = Interval;
      MaxInterval.second = I;
    }
    AverageInterval += Interval / 1000. ;
  }
  AverageInterval /= COUNT;
  printf("Number of intervals:            %10d\n", COUNT);
  printf("Duration of an interval:        %10d\n", DELAY);
  printf("Measure of inaccuracy (msec):   %10.3f\n", EPS / 1000.);
  printf("Number of exceeded intervals    %10u\n", NumberOfExceeded);
  printf("Minimum duration (msec):        %10.3f\n", MinInterval.first / 1000.);
  printf("Minimum duration at:            %10u\n", MinInterval.second);
  printf("Maximum duration (msec):        %10.3f\n", MaxInterval.first / 1000.);
  printf("Maximum duration at:            %10u\n", MaxInterval.second);
  printf("Average duration (msec)         %10.3f\n", AverageInterval);
  printf("========= End of Timer Benchmark =========\n");
  return 0;
}

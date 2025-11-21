#pragma once
#include <chrono>
class CustomStopwatch {
public://Public Functions
	/// <summary>
	/// Sets the stopwatch start point to NOW
	/// </summary>
	void start();

	/// <summary>
	/// Sets the stopwatch end point to NOW
	/// </summary>
	void stop();

	/// <summary>
	/// Return the duration from the start point to the end point in microseconds
	/// </summary>
	/// <returns></returns>
	std::chrono::microseconds getDurationMicro();

	/// <summary>
	/// Return the duration from the start point to the end point in milliseconds
	/// </summary>
	/// <returns></returns>
	std::chrono::milliseconds getDurationMilli();
private://variables
	std::chrono::high_resolution_clock::time_point startPoint;
	std::chrono::high_resolution_clock::time_point endPoint;
};
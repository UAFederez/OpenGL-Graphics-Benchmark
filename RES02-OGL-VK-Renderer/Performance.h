/* Performance Statistics */
#pragma once 

#include <chrono>
#include <vector>
#include <iostream>
#include <fstream>

namespace pstat
{
	using timep_t = std::chrono::high_resolution_clock::time_point;
	using delta_t = std::chrono::duration<double>;
	using stats_t = std::vector<double>;

	double GetAverage(const stats_t&);

	void WriteCSV(const std::string&, const stats_t&, const stats_t&);

	timep_t	getCurrentTime();
	delta_t	getTimeDuration(const timep_t&, const timep_t&);
}

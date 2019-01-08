/* Performance Statistics */
#ifndef RES_PERFORMANCE_H
#define RES_PERFORMANCE_H

#include <chrono>
#include <vector>
#include <iostream>
#include <iostream>
#include <fstream>

namespace pstat
{
	using point_t		= std::chrono::high_resolution_clock::time_point;
	using duration_t	= std::chrono::duration<double>;
	using statlog_t		= std::vector<double>;

	double GetAverage(const statlog_t& dataset);

	void WriteCSV(const std::string &filename, const statlog_t &fps_stat, const statlog_t &mspf_stat);

	point_t		getCurrentTime();
	duration_t	getTimeDuration(const point_t& t1, const point_t& t2);
};

#endif

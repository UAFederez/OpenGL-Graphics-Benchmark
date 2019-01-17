#include "Performance.h"

pstat::timep_t pstat::getCurrentTime()
{
	return std::chrono::high_resolution_clock::now();
}

pstat::delta_t pstat::getTimeDuration(const timep_t& t1, const timep_t& t2)
{
	return std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t2);
}

void pstat::WriteCSV(const std::string &filename, const stats_t &fps_stat, const stats_t &mspf_stat)
{	
	std::cout << "Writing results to file" << std::endl;
	
	std::ofstream output_csv;

	output_csv.open(("results/" + filename + ".csv"));

	if (! output_csv) throw std::runtime_error("Error: unable to open file");

	output_csv << "second,fps,mspf" << std::endl;
	
	for (size_t i = 0; i != fps_stat.size(); i++)
		output_csv << i << ',' << fps_stat[i] << ',' << mspf_stat[i] << std::endl;

	output_csv.close();

	std::cout << "CSV file successfully written" << std::endl;
}

double pstat::GetAverage(const stats_t& dataset)
{
	double average = 0.0;
	const unsigned offset = 5;

	if (dataset.size() <= offset) return 0;

	for (auto it = dataset.begin() + offset; it != dataset.end(); ++it)
		average += *it / (dataset.size() - offset);

	return average;
}
#pragma once
#pragma once
#include <vector>
#include <fstream>
#include <sstream> 
#include <filesystem>

namespace fs = std::filesystem;

static inline void read_3d_fields(std::ifstream& read)
{
	std::string str;
	std::string substr;
	std::stringstream ss;
	std::getline(read, str);
	std::getline(read, str);


	int Nx_ = Nx + 1;
	int Ny_ = Ny + 1;
	int Nz_ = Nz + 1;

	double vmax = 0;
	double pm = 0;
	for (int i = 0; i < Nx_; i++) {
		for (int j = 0; j < Ny_; j++) {
			for (int k = 0; k < Nz_; k++) {

				int l = i + Nx_ * j + Nx_ * Ny_ * k;

				ss.str("");
				ss.clear();
				std::getline(read, str);
				ss << str;
				ss >> substr; // x
				ss >> substr; // y
				ss >> substr; // z
				ss >> substr; vx[l] = atof(substr.c_str());
				ss >> substr; vy[l] = atof(substr.c_str());
				ss >> substr; vz[l] = atof(substr.c_str());
				double v = sqrt(vx[l] * vx[l] + vy[l] * vy[l] + vz[l] * vz[l]);
				if (v > vmax) vmax = v;

				if (vy[l] > VyColorMax) VyColorMax = vy[l];
				if (vy[l] < VyColorMin) VyColorMin = vy[l];
			}
		}
	}


	if (vmax < 1e-10) vmax = 1;
	for (int i = 0; i < Nx_; i++) {
		for (int j = 0; j < Ny_; j++) {
			for (int k = 0; k < Nz_; k++) {
				int l = i + off * j + off2 * k;
				vx[l] /= vmax;
				vy[l] /= vmax;
				vz[l] /= vmax;
			}
		}
	}
}



struct SimpleFileIterator
{
	std::vector<fs::path> files;
	size_t currentFileIndex = 0;
	SimpleFileIterator(const std::string& path = "testfolder") {
		if (!fs::exists(path) || !fs::is_directory(path)) {
			std::cout << "folder not found" << std::endl;
			return;
		}

		for (const auto& entry : fs::directory_iterator(path)) {
			if (fs::is_regular_file(entry.path()))
			{
				files.push_back(entry.path());
			}
		}
		std::cout << "N files = " << files.size() << std::endl;
	}

	std::ifstream getNextFile(int incr = 1)
	{
		currentFileIndex += incr;
		if (currentFileIndex >= files.size())
		{
			return std::ifstream();
		}
		std::cout << files[currentFileIndex].filename() << std::endl;
		return std::ifstream(files[currentFileIndex]);
	}

	// Прочитать конкретный файл по индексу
	std::ifstream openFile(size_t index) {
		if (index < files.size())
		{
			//cout << files[index].filename() << endl;
			return std::ifstream(files[index]);
		}
		return std::ifstream();
	}

	size_t getFileCount() const {
		return files.size();
	}

	void reset() {
		currentFileIndex = 0;
	}
};


//SimpleFileIterator FILES("Z:\\Projects\\CUDA\\ternary_3d\\CudaRuntime4\\all");
SimpleFileIterator FILES("Z:\\Projects\\CUDA\\ternary_3d\\CudaRuntime4\\all");

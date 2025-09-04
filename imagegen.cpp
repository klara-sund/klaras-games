#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[]) {
	if (argc < 4) {
		std::cerr << "Usage: " << argv[0] << " <width> <height> <color>" << std::endl;
		return 1;
	}

	int width = std::stoi(argv[1]);
	int height = std::stoi(argv[2]);
	std::string hexColor = argv[3];

	if (hexColor.length() != 6) {
		std::cerr << "Error, Color must be a 6-digit hex value (e.g., FF0000)" << std::endl;
		return 1;
	}

	int r = std::stoi(hexColor.substr(0, 2), nullptr, 16);
	int g = std::stoi(hexColor.substr(2, 2), nullptr, 16);
	int b = std::stoi(hexColor.substr(4, 2), nullptr, 16);

	std::ofstream outfile("output.ppm");

	//Write ppm header
	outfile << "P3" << std::endl;
	outfile << width << " " << height << std::endl;
	outfile << "255" << std::endl;

	//Write the pixel data
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < height; ++x) {
			outfile << r << " " << g << " " << b << " ";
		}
		outfile << std::endl;
	}

	outfile.close();

	std::cout << "Generated output.ppm" << std::endl;

	return 0;
}


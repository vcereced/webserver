#ifndef FILEPARSE_HPP
# define FILEPARSE_HPP

# include <fstream>
# include <set>
# include <vector>
# include <string>
# include <sstream>
# include <iostream>
# include <unistd.h>
# include <map>

# define RED "\033[31m"
# define GREEN "\033[32m"
# define YELLOW "\033[33m"
# define BLUE "\033[34m"
# define MAGENTA "\033[35m"
# define RESET "\033[0m"

#include "webserv.hpp"

class FileParse {
public:
	// // Constructor por defecto
	FileParse(void) {}

	void loadConfigFromFile(const std::string filename);
	void showConfig(void);

	//getter
	t_http &getStruct(void);

private:
	t_http configData;
};

#endif  // FileParse_HPP 




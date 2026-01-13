#include <string>
#include <map>

enum struct HTTP_Method{
	GET,
	POST
};

struct HTTP_Request {
	HTTP_Method Method;
	std::string Url;
	std::map<std::string, int> Variables;
};

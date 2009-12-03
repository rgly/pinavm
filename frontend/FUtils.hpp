#ifndef _FUTILS_HPP
#define _FUTILS_HPP

#include <vector>
#include <string>

class FUtils {
public:
	static std::string tutos[];
		
	template < class Element >
	inline static void
	deleteVector(std::vector < Element > *vec) {
		typename std::vector < Element >::iterator it;
		while (!vec->empty()) {
			Element e = *vec->begin();
			vec->erase(vec->begin());
			delete e;
		}
		//    delete vec; } static std::string tutos[];
	}
};
	

#endif

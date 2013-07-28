#include <algorithm>
#include <array>
#include <cstdint>
#include <deque>
#include <list>
#include <memory>
#include <vector>
#include <random>
#include <iostream>
#include <fstream>

#include <boost/timer.hpp>

#include "moving_vector.hpp"

namespace {
using value_type = uint32_t;

value_type number() {
	static std::random_device rd;
	static std::mt19937 random_engine(rd());
	static std::uniform_int_distribution<value_type> distribution(0, std::numeric_limits<value_type>::max());
	return distribution(random_engine);
}

class Class {
	public:
		Class() {
			x[0] = number();
		}
		uint32_t value() const {
			return x[0];
		}
		inline friend bool operator<=(Class const & lhs, Class const & rhs) {
			return lhs.x[0] <= rhs.x[0];
		}
	private:
		// ARRAY_SIZE is defined by the compiler
		std::array<value_type, ARRAY_SIZE> x;
};

template<typename Container>
void add(Container & container, Class const & value) {
	auto const it = std::find_if(std::begin(container), std::end(container), [&](Class const & c) {
		return value <= c;
	});
	container.emplace(it, value);
}

// Do something with the result
template<typename Container>
void insert_to_file(Container const & container) {
	std::fstream file("file.txt");
	for (auto const & c : container) {
		file << c.value() << '\n';
	}
}

template<typename Container>
void f(std::vector<Class> const & values) {
	Container container;
	for (auto const & value : values) {
		add(container, value);
	}
	insert_to_file(container);
}

}	// namespace

int main(int argc, char ** argv) {
	std::size_t const size = (argc == 1) ? 1 : std::stoul(argv[1]);
	// Default constructor of Class fills in values here
	std::vector<Class> const values_to_be_copied(size);
	#if defined VECTOR
	typedef std::vector<Class> Container;
	#elif defined LIST
	typedef std::list<Class> Container;
	#elif defined DEQUE
	typedef std::deque<Class>Container;
	#elif defined MOVING_VECTOR
	typedef smart_pointer::moving_vector<Class> Container;
	#endif
	boost::timer timer;
	f<Container>(values_to_be_copied);
	std::cerr << "Finished in " << timer.elapsed() << " seconds.\n";
}

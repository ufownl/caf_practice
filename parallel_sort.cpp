#include <caf/all.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <stdlib.h>
#include <time.h>

template <class _it>
caf::behavior partition(caf::event_based_actor* self) {
	return {
		[self] (_it l, _it r) {
			using std::swap;

			_it pivot = r - 1;
			_it p = l, q = r - 2;
			while (p <= q) {
				if (*p < *pivot) ++p;
				else swap(*p, *(q--));
			}

			swap(*(q + 1), *pivot);

			self->quit();

			return q + 1;
		}
	};
}

template <class _it>
void parallel_sort(caf::event_based_actor* self, _it l, _it r, size_t part_size) {
	if (r - l < part_size) {
		std::sort(l, r);
		return;
	}

	auto partition_actor = self->spawn<caf::linked>(partition<_it>);
	self->sync_send(partition_actor, l, r).then(
		[self, l, r, part_size] (_it m) {
			self->spawn<caf::linked>(parallel_sort<_it>, l, m, part_size);
			self->spawn<caf::linked>(parallel_sort<_it>, m + 1, r, part_size);
		}
	);
}

caf::optional<long> as_long(const std::string& str) {
	return stoul(str);
}

int main(int argc, char* argv[]) {
	long total_size = 0;
	long part_size = 0;

	caf::message_builder(argv + 1, argv + argc).apply({
		on(as_long, as_long) >> [&total_size, &part_size] (long n, long m) {
			total_size = n;
			part_size = m;
		},
		caf::others >> [] {
			std::cerr << "Usage: parallel_sort <total_size> <part_size>" << std::endl;
		}
	});

	if (total_size > 0 && part_size > 0) {
		srand(time(nullptr));

		std::vector<long> v;
		v.reserve(total_size);
		for (long i = 0; i < total_size; ++i)
			v.push_back(rand());

		if (total_size > 100) {
			auto w = v;
			auto t = std::chrono::high_resolution_clock::now();
			std::sort(w.begin(), w.end());
			std::cout << "std::sort duration: " << std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - t).count() << std::endl;
		}

		auto t = std::chrono::high_resolution_clock::now();
		caf::spawn(parallel_sort<decltype(v)::iterator>, v.begin(), v.end(), part_size);
		caf::await_all_actors_done();
		std::cout << "parallel_sort duration: " << std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - t).count() << std::endl;

		if (total_size <= 100) {
			for (auto n: v)
				std::cout << n << std::endl;
		} else {
			std::cout << "Too many numbers!" << std::endl;

			for (size_t i = 1; i < v.size(); ++i) {
				if (v[i - 1] > v[i])
					std::cerr << "ERROR!!!" << std::endl;
			}
		}
	}

	caf::shutdown();

	return 0;
}

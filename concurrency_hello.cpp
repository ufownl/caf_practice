#include <caf/all.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include "util.hpp"

caf::behavior mirror_worker(caf::event_based_actor* self) {
	return {
		[self] (const std::string& what) {
			for (int i = 0; i < 5; ++i) {
				caf::aout(self) << "thread[" << std::this_thread::get_id() << "] actor[" << self->address() <<  "] tick[" << i << "]: " << what << std::endl;
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}

			self->quit();

			return std::string(what.rbegin(), what.rend());
		}
	};
}

caf::behavior mirror(caf::event_based_actor* self, size_t& worker_cnt) {
	self->trap_exit(true);

	return {
		[self, &worker_cnt] (const std::string&) {
			auto worker = self->spawn<caf::linked>(mirror_worker);
			self->forward_to(worker);
			++worker_cnt;
		},
		[self, &worker_cnt] (const caf::exit_msg& msg) {
			caf::aout(self) << "thread[" << std::this_thread::get_id() << "] actor[" << self->address() <<  "]: worker_actor[" << msg.source << "] exit" << std::endl;

			if (--worker_cnt == 0)
				self->quit();
		},
		caf::others >> [] {
			return "This is default handler.";
		}
	};
}

void hello_world(caf::event_based_actor* self, const caf::actor& buddy) {
	for (int i = 0; i < 5; ++i) {
		self->sync_send(buddy, "Hello, world!").then(
			[self] (const std::string& what) {
				caf::aout(self) << "thread[" << std::this_thread::get_id() << "] actor[" << self->address() << "]: " << what << std::endl;
			}
		);
	}
}

int main() {
	size_t worker_cnt = 0;
	auto mirror_actor = caf::spawn(mirror, std::ref(worker_cnt));
	caf::spawn(hello_world, mirror_actor);
	caf::await_all_actors_done();
	caf::shutdown();
	return 0;
}

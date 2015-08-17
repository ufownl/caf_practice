#include <caf/all.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include "util.hpp"

using mirror_type = caf::typed_actor<caf::replies_to<std::string>::with<std::string> >;

mirror_type::behavior_type mirror(mirror_type::pointer self)
{
	return {
		[self] (const std::string& what)
		{
			caf::aout(self) << "thread[" << std::this_thread::get_id() << "] actor[" << self->address() <<  "]: " << what << std::endl;

			self->quit();

			return std::string(what.rbegin(), what.rend());
		}
	};
}

void hello_world(caf::event_based_actor* self, const mirror_type& buddy)
{
	self->sync_send(buddy, "Hello, world!").then([self] (const std::string& what)
			{
				caf::aout(self) << "thread[" << std::this_thread::get_id() << "] actor[" << self->address() << "]: " << what << std::endl;
			});
}

int main()
{
	auto mirror_actor = caf::spawn(mirror);
	caf::spawn(hello_world, mirror_actor);
	caf::await_all_actors_done();
	caf::shutdown();
	return 0;
}

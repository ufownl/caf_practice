#include <caf/all.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>

caf::behavior mirror(caf::event_based_actor* self)
{
	return {
		[self] (const std::string& what)
		{
			std::stringstream ss;
			ss << std::this_thread::get_id();
			caf::aout(self) << "thread[" << ss.str() << "] actor[" << self->address().id() <<  "]: " << what << std::endl;

			self->quit();

			return std::string(what.rbegin(), what.rend());
		},
		caf::others >> []
		{
			return "This is default handler.";
		}
	};
}

void hello_world(caf::event_based_actor* self, const caf::actor& buddy)
{
	self->sync_send(buddy, "Hello, world!").then([self] (const std::string& what)
			{
				std::stringstream ss;
				ss << std::this_thread::get_id();
				caf::aout(self) << "thread[" << ss.str() << "] actor[" << self->address().id() << "]: " << what << std::endl;
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

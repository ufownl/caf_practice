#include <caf/all.hpp>
#include <iostream>
#include <thread>
#include "util.hpp"

caf::behavior mirror(caf::event_based_actor* self)
{
	return {
		[self] (const std::string& what)
		{
			caf::aout(self) << "thread[" << std::this_thread::get_id() << "] actor[" << self->address() <<  "]: " << what << std::endl;

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

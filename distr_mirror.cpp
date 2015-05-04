#include <caf/all.hpp>
#include <caf/io/all.hpp>
#include <iostream>
#include <thread>
#include "util.hpp"

caf::behavior mirror_worker(caf::event_based_actor* self)
{
	return {
		[self] (const std::string& what)
		{
			for (int i = 0; i < 5; ++i)
			{
				caf::aout(self) << "thread[" << std::this_thread::get_id() << "] actor[" << self->address() <<  "] tick[" << i << "]: " << what << std::endl;
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}

			self->quit();

			return std::to_string(rand()) + std::string(what.rbegin(), what.rend());
		}
	};
}

caf::behavior mirror(caf::event_based_actor* self)
{
	self->trap_exit(true);

	return {
		[self] (const std::string&)
		{
			auto worker = self->spawn<caf::linked>(mirror_worker);
			self->forward_to(worker);
		},
		[self] (const caf::exit_msg& msg)
		{
			caf::aout(self) << "thread[" << std::this_thread::get_id() << "] actor[" << self->address() <<  "]: worker_actor[" << msg.source << "] exit" << std::endl;
		},
		caf::others >> []
		{
			return "This is default handler.";
		}
	};
}

caf::optional<uint16_t> as_uint16(const std::string& str)
{
	return static_cast<uint16_t>(stoul(str));
}

int main(int argc, char* argv[])
{
	caf::message_builder(argv + 1, argv + argc).apply({
				on(as_uint16) >> [] (uint16_t port)
				{
					auto mirror_actor = caf::spawn(mirror);
					port = caf::io::publish(mirror_actor, port, nullptr, true);
					std::cout << "Mirror_actor has published on port[" << port << "]." << std::endl;
				},
				caf::others >> []
				{
					std::cerr << "Usage: distr_mirror <port>" << std::endl;
				}
			});
	caf::await_all_actors_done();
	caf::shutdown();
	return 0;
}

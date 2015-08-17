#include "distr_typed_common.hpp"

mirror_type::behavior_type mirror(mirror_type::pointer self) {
	self->trap_exit(true);

	return {
		[self] (const std::string& what) {
			caf::aout(self) << "thread[" << std::this_thread::get_id() << "] actor[" << self->address() <<  "]: " << what << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
			return std::string(what.rbegin(), what.rend());
		},
	};
}

caf::optional<uint16_t> as_uint16(const std::string& str) {
	return static_cast<uint16_t>(stoul(str));
}

int main(int argc, char* argv[])
{
	caf::message_builder(argv + 1, argv + argc).apply({
		on(as_uint16) >> [] (uint16_t port) {
			auto mirror_actor = caf::spawn(mirror);
			port = caf::io::typed_publish(mirror_actor, port, nullptr, true);
			std::cout << "typed_mirror_actor has published on port[" << port << "]." << std::endl;
		},
		caf::others >> [] {
			std::cerr << "Usage: distr_mirror <port>" << std::endl;
		}
	});
	caf::await_all_actors_done();
	caf::shutdown();
	return 0;
}

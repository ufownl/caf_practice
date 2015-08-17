#include <caf/all.hpp>
#include <caf/io/all.hpp>
#include <iostream>

caf::behavior printer(caf::event_based_actor* self) {
	caf::aout(self) << "printer_actor[" << self->address() << "]" << std::endl;

	return {
		[self] (const std::string& msg) {
			caf::aout(self) << "actor[" << self->address() << "]: " << msg << std::endl;
		}
	};
}

void hello(caf::event_based_actor* self, uint16_t port) {
	caf::aout(self) << "hello_actor[" << self->address() << "]" << std::endl;

	auto g = caf::group::get("local", "printer");
	self->spawn_in_group<caf::linked>(g, printer);
	self->spawn_in_group<caf::linked>(g, printer);
	self->spawn_in_group<caf::linked>(g, printer);

	port = caf::io::publish_local_groups(port);
	caf::aout(self) << "Local groups have published on port[" << port << "]." << std::endl;
}

caf::optional<uint16_t> as_uint16(const std::string& str) {
	return static_cast<uint16_t>(stoul(str));
}

int main(int argc, char* argv[]) {
	caf::message_builder(argv + 1, argv + argc).apply({
		on(as_uint16) >> [] (uint16_t port) {
			caf::spawn(hello, port);
		},
		caf::others >> [] {
			std::cerr << "Usage: distr_printer_group <port>" << std::endl;
		}
	});
	caf::await_all_actors_done();
	caf::shutdown();
	return 0;
}

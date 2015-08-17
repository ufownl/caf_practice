#include <caf/all.hpp>
#include <caf/io/all.hpp>
#include <iostream>
#include <thread>
#include <chrono>

void hello(caf::event_based_actor* self, const std::string& host, uint16_t port) {
	caf::aout(self) << "hello_actor[" << self->address() << "]" << std::endl;

	auto g = caf::io::remote_group("printer", host, port);

	for (int i = 0; i < 5; ++i) {
		self->send(g, "Hello, world!");
		caf::aout(self) << "Message has been sent." << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

caf::optional<uint16_t> as_uint16(const std::string& str) {
	return static_cast<uint16_t>(stoul(str));
}

int main(int argc, char* argv[]) {
	caf::message_builder(argv + 1, argv + argc).apply({
		on(caf::val<std::string>, as_uint16) >> [] (const std::string& host, uint16_t port) {
			caf::spawn(hello, host, port);
		},
		caf::others >> [] {
			std::cerr << "Usage: distr_hello_group <host> <port>" << std::endl;
		}
	});
	caf::await_all_actors_done();
	caf::shutdown();
	return 0;
}

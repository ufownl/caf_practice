#include "distr_typed_common.hpp"

void hello_world(caf::event_based_actor* self, const mirror_type& buddy) {
	caf::aout(self) << "hello_world_actor[" << self->address() << "]" << std::endl;

	for (int i = 0; i < 5; ++i) {
		self->sync_send(buddy, "Hello, world!").then(
			[self] (const std::string& what) {
				caf::aout(self) << "thread[" << std::this_thread::get_id() << "] actor[" << self->address() << "]: " << what << std::endl;
			}
		);
	}
}

using hello_atom = caf::atom_constant<caf::atom("hello")>;
using hello_client_type = caf::typed_actor<caf::replies_to<hello_atom>::with<void> >;

hello_client_type::behavior_type hello_client(hello_client_type::pointer self, const std::string& host, uint16_t port) {
	caf::aout(self) << "hello_client_actor[" << self->address() << "]" << std::endl;

	self->trap_exit(true);
	self->send(self, hello_atom::value);

	return {
		[self, host, port] (hello_atom) {
			caf::aout(self) << "Connecting and sending \"Hello, world!\"." << std::endl;

			try {
				auto mirror_actor = caf::io::typed_remote_actor<mirror_type>(host, port);
				self->monitor(mirror_actor);

				self->spawn<caf::linked>(hello_world, mirror_actor);
			} catch (const caf::network_error& e) {
				caf::aout(self) << e.what() << std::endl;
				self->send(self, hello_atom::value);
			}
		},
		[self] (const caf::down_msg& msg) {
			caf::aout(self) << "actor[" << msg.source << "] down_msg: " << caf::exit_reason::as_string(msg.reason) << std::endl;

			if (msg.reason == caf::exit_reason::remote_link_unreachable)
				self->send(self, hello_atom::value);
		},
		[self] (const caf::exit_msg& msg) {
			caf::aout(self) << "actor[" << msg.source << "] exit_msg: " << caf::exit_reason::as_string(msg.reason) << std::endl;

			if (msg.reason != caf::exit_reason::unhandled_sync_failure)
				self->quit(msg.reason);
		}
	};
}

caf::optional<uint16_t> as_uint16(const std::string& str) {
	return static_cast<uint16_t>(stoul(str));
}

int main(int argc, char* argv[]) {
	caf::message_builder(argv + 1, argv + argc).apply({
		on(caf::val<std::string>, as_uint16) >> [] (const std::string& host, uint16_t port) {
			caf::spawn(hello_client, host, port);
		},
		caf::others >> [] {
			std::cerr << "Usage: distr_hello <host> <port>" << std::endl;
		}
	});
	caf::await_all_actors_done();
	caf::shutdown();
	return 0;
}

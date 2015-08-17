#include <caf/all.hpp>
#include <iostream>

caf::behavior printer(caf::event_based_actor* self) {
	caf::aout(self) << "printer_actor[" << self->address() << "]" << std::endl;

	auto g = caf::group::get("local", "printer");
	self->join(g);
	self->send(g, "RangerUFO");

	return {
		[self] (const std::string& msg) {
			caf::aout(self) << "actor[" << self->address() << "]: " << msg << std::endl;
		}
	};
}

void hello(caf::event_based_actor* self) {
	caf::aout(self) << "hello_actor[" << self->address() << "]" << std::endl;

	auto g = caf::group::anonymous();
	self->spawn_in_group<caf::linked>(g, printer);
	self->spawn_in_group<caf::linked>(g, printer);
	self->spawn_in_group<caf::linked>(g, printer);

	self->send(g, "Hello, world!");

	getchar();
	self->quit(caf::exit_reason::user_shutdown);
}

int main() {
	caf::spawn(hello);
	caf::await_all_actors_done();
	caf::shutdown();
	return 0;
}

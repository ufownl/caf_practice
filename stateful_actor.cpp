#include <caf/all.hpp>

using add_atom = caf::atom_constant<caf::atom("add")>;
using get_atom = caf::atom_constant<caf::atom("get")>;

using test_actor = caf::typed_actor<caf::reacts_to<add_atom, int>,
									caf::replies_to<get_atom>::with<int>>;

test_actor::behavior_type test(test_actor::stateful_pointer<int> self) {
	std::cout << self->name() << std::endl;
	return {
		[=] (add_atom, int x) {
			self->state += x;
		},
		[=] (get_atom) {
			return self->state;
		}
	};
}

int main() {
	auto t = caf::spawn(test);
	for (int i = 0; i < 5; ++i) {
		caf::anon_send(t, add_atom::value, i + 1);
	}
	{
		caf::scoped_actor s;
		s->sync_send(t, get_atom::value).await(
			[] (int value) {
				std::cout << value << std::endl;
			}
		);
	}
	caf::await_all_actors_done();
	caf::shutdown();
	return 0;
}

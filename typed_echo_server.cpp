#include <caf/all.hpp>
#include <caf/io/all.hpp>
#include <iostream>
#include <string>

using connection = caf::io::experimental::minimal_client;

connection::behavior_type conn_worker(connection::broker_pointer self, caf::io::connection_handle conn)
{
	self->configure_read(conn, caf::io::receive_policy::at_most(4096));
	return {
		[self] (const caf::io::new_data_msg& msg)
		{
			caf::aout(self) << "connection[" << self->address() << "]: " << std::string(msg.buf.begin(), msg.buf.end()) << std::endl;

			self->write(msg.handle, msg.buf.size(), &msg.buf.front());
		},
		[self] (const caf::io::connection_closed_msg&)
		{
			caf::aout(self) << "connection close: " << self->address() << std::endl;

			self->quit();
		}
	};
}

using acceptor = caf::io::experimental::minimal_server;

acceptor::behavior_type echo_server(acceptor::broker_pointer self)
{
	return {
		[self] (const caf::io::new_connection_msg& new_conn)
		{
			auto worker = self->fork(conn_worker, new_conn.handle);
			caf::aout(self) << "new connection: " << worker.address() << std::endl;
		},
		[] (const caf::io::new_data_msg&) {},
		[] (const caf::io::connection_closed_msg&) {},
		[] (const caf::io::acceptor_closed_msg&) {}
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
					std::cout << "echo_server startup" << std::endl;
					caf::io::spawn_io_server(echo_server, port);
				},
				caf::others >> []
				{
					std::cerr << "Usage: echo_server <port>" << std::endl;
				}
			});
	caf::await_all_actors_done();
	caf::shutdown();
	return 0;
}

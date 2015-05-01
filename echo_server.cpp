#include <caf/all.hpp>
#include <caf/io/all.hpp>
#include <iostream>
#include <string>

caf::behavior conn_worker(caf::io::broker* self, caf::io::connection_handle conn)
{
	self->configure_read(conn, caf::io::receive_policy::at_most(4096));
	return {
		[self] (const caf::io::new_data_msg& msg)
		{
			caf::aout(self) << "connection[" << self->address().id() << "]: " << std::string(msg.buf.begin(), msg.buf.end()) << std::endl;

			self->write(msg.handle, msg.buf.size(), &msg.buf.front());
		},
		[self] (const caf::io::connection_closed_msg&)
		{
			caf::aout(self) << "connection close: " << self->address().id() << std::endl;

			self->quit();
		},
		caf::others >> [self]
		{
			caf::aout(self) << "unexpected: " << caf::to_string(self->current_message()) << std::endl;
		}
	};
}

caf::behavior echo_server(caf::io::broker* self)
{
	return {
		[self] (const caf::io::new_connection_msg& new_conn)
		{
			auto worker = self->fork(conn_worker, new_conn.handle);
			self->link_to(worker);

			caf::aout(self) << "new connection: " << worker.address().id() << std::endl;
		},
		caf::others >> [self]
		{
			caf::aout(self) << "unexpected: " << caf::to_string(self->current_message()) << std::endl;
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
				on("-p", as_uint16) >> [] (uint16_t port)
				{
					std::cout << "echo_server startup" << std::endl;
					spawn_io_server(echo_server, port);
				},
				caf::others >> []
				{
					std::cerr << "Usage: echo_server -p <port>" << std::endl;
				}
			});
	caf::await_all_actors_done();
	caf::shutdown();
	return 0;
}

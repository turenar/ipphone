#include "ippp.hxx"
#include <ipp/protocol/message/message.hxx>

namespace ipp {
	namespace protocol {
		ippp::ippp(network::socket_connection con) : _con(std::move(con)) {}

		ippp::~ippp() {}

		void ippp::keep_alive() {
			_packet.write(message::pack(message::keep_alive{}))
					.send(_con);
		}
		void ippp::connect() {
			_packet.write(message::pack(message::connect{IPPP_PROTOCOL_NAME, protocol_version, protocol_revision}))
					.send(_con);
		}

		void ippp::disconnect() {
			_packet.write(message::pack(message::disconnect{}))
					.send(_con);
		}
	}
}

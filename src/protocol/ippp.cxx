#include "ippp.hxx"
#include <protocol/message/message.hxx>

namespace ipp {
	namespace protocol {
		ippp::ippp(network::socket_connection&& con) : _con(std::move(con)) {}

		ippp::~ippp() {}

		void ippp::connect() {
			_packet.write(message::pack(message::connect{protocol_version, protocol_revision}))
					.send(_con);
		}

		void ippp::disconnect() {
			_packet.write(message::pack(message::disconnect{}))
					.send(_con);
		}
	}
}

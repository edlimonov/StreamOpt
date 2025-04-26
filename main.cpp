#include <iostream>
#include <string>
#include "root_certificates.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http;   // from <boost/beast/http.hpp>
namespace net = boost::asio;    // from <boost/asio.hpp>
namespace ssl = net::ssl;       // from <boost/asio/ssl.hpp>
using tcp = net::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

using StreamOpt = std::optional<ssl::stream<beast::tcp_stream>>;

StreamOpt setup_ssl_stream(net::io_context& ioc, const std::string& host, const std::string& port) {
    try {
        ssl::context ctx(ssl::context::tlsv12_client);
        load_root_certificates(ctx);
        ctx.set_verify_mode(ssl::verify_peer);

        tcp::resolver resolver(ioc);
        auto const results = resolver.resolve(host, port);

        ssl::stream<beast::tcp_stream> stream(ioc, ctx);
        beast::get_lowest_layer(stream).connect(results);

        if (!SSL_set_tlsext_host_name(stream.native_handle(), host.c_str())) {
            beast::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
            throw beast::system_error{ec};
        }

        stream.handshake(ssl::stream_base::client);

        return stream;
    } catch (std::exception const& e) {
        std::cerr << "Exception during stream setup: " << e.what() << std::endl;
        return std::nullopt;
    }
}

int main(){

  net::io_context context;
  
  const std::string host = "api.bybit.com";
  const std::string port = "443";

  StreamOpt stream = setup_ssl_stream(context, host, port);

  if (stream) {
    // here you can use the stream
  }

}

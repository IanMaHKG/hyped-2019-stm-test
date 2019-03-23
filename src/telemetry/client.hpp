#ifndef TELEMETRY_CLIENT_HPP_
#define TELEMETRY_CLIENT_HPP_

#include "types/message.pb.h"
#include "utils/logger.hpp"
#include <string>
#include <google/protobuf/io/zero_copy_stream.h>

namespace hyped {

using utils::Logger;

namespace client {

    constexpr auto port = "9090";
    // constexpr auto server_ip = "localhost";
    constexpr auto server_ip = "192.168.1.50";

    class Client {
        public:
            Client(Logger& log);
            ~Client();
            bool sendData(protoTypes::TestMessage message);
            bool receiveData();
        private:
            int sockfd;
            Logger& log_;
            google::protobuf::io::ZeroCopyInputStream* socketStream;  // member variable bc we need to keep reading from same stream (buffered)
    };

}  // namespace client
}  // namespace hyped

#endif

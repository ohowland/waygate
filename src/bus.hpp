#ifndef CAN_READER_H_
#define CAN_READER_H_

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "frame.hpp"
#include "signals.hpp"

namespace can {

class Reader{
public:
    Reader();
};

class Writer{
public:
    Writer();
};

class SocketHandler{
public:
    SocketHandler(const std::string& t_iface);
    ~SocketHandler();
    
    auto recv() const -> Frame;
    auto send(const Frame) -> int;
    auto close() -> int;
    auto open(std::string t_iface) -> int;
    auto bind(int t_fd, std::string t_iface) -> int;

private:    
    const int m_fd;
};

class Bus {
public:
    Bus(const std::string& t_iface);

    auto read() -> Frame; 
    auto write(Frame);
private:
    const std::shared_ptr<SocketHandler> m_socket_handler;
};

auto nameToIndex(int t_fd, std::string t_iface) -> int;

}



#endif
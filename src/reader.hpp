#ifndef CAN_READER_H_
#define CAN_READER_H_

#include <iostream>
#include <string>
#include <vector>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "frame.hpp"

namespace can {


class Reader {
public:
    Reader(const std::string& t_iface);
    ~Reader();
        
    auto recv() const -> Frame;
private:    
    auto close() -> int;
    auto open(std::string t_iface) -> int;
    auto bind(int t_fd, std::string t_iface) -> int;

    const int m_fd;
};

auto nameToIndex(int t_fd, std::string t_iface) -> int;

}

#endif
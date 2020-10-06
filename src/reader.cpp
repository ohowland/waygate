#include "reader.hpp"

#include <string>
#include <string.h>

#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "frame.hpp"

namespace can {

Reader::Reader(const std::string& t_ifname)
: m_fd(open(t_ifname))
{
    if (m_fd < 0) {
        std::cout << "failed to open socket" << std::endl;
        throw;
    }
}

Reader::~Reader() 
{
    int r = close();
    if (r < 0) {
        std::cerr << "failed to close socket" << std::endl;
    }
}

auto Reader::close() -> int {
    std::cout << "closing socket (fd: " << m_fd << ")" << std::endl;
    return ::close(m_fd);
}

auto Reader::open(std::string iface) -> int {
    int fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (fd < 0) {
        std::cerr << "failed to open socket" << std::endl;
        return fd;
    }
    return bind(fd, iface); 
}

auto Reader::bind(int t_fd, std::string t_iface) -> int {

    auto index = nameToIndex(t_fd, t_iface);
    if (index < 0) {
        return -1;
    }

    struct sockaddr_can addr;
    addr.can_ifindex = index;
    addr.can_family = AF_CAN;
    
    int r = ::bind(t_fd, (struct sockaddr *)&addr, sizeof(addr));
    if (r < 0) {
        std::cerr << "failed to bind to socket" << std::endl;
        return r;
    }
    
    return t_fd;
}


auto Reader::recv() const -> Frame {
    struct can_frame frame;

    auto n = ::read(m_fd, &frame, CAN_MTU);
    if (n < 0) {
        std::cerr << "can socket read error" << std::endl;
        return Frame();
    }

    return Frame(frame.can_id, frame.can_dlc, frame.data);
}

auto nameToIndex(int t_fd, std::string t_iface) -> int {
    struct ifreq ifr;

    strcpy(ifr.ifr_name, t_iface.c_str());
    ioctl(t_fd, SIOCGIFINDEX, &ifr, IFNAMSIZ - 1);
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';
    ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name);
    if (ifr.ifr_ifindex == 0) {
        std::cerr << t_iface << " not found" << std::endl;
        return -1;
    }

    return ifr.ifr_ifindex;
}

}
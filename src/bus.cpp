#include "bus.hpp"

#include <string>
#include <string.h>
#include <memory>

#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "frame.hpp"

namespace can {

SocketHandler::SocketHandler(const std::string& t_ifname)
: m_fd(open(t_ifname))
{
    if (m_fd < 0) {
        std::cout << "failed to open socket" << std::endl;
        throw;
    }
}

SocketHandler::~SocketHandler() 
{
    int r = close();
    if (r < 0) {
        std::cerr << "failed to close socket" << std::endl;
    }
}

auto SocketHandler::close() -> int {
    std::cout << "closing socket (fd: " << m_fd << ")" << std::endl;
    return ::close(m_fd);
}

auto SocketHandler::open(std::string iface) -> int {
    int fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (fd < 0) {
        std::cerr << "failed to open socket" << std::endl;
        return fd;
    }
    return bind(fd, iface); 
}

auto SocketHandler::bind(int t_fd, std::string t_iface) -> int {

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


auto SocketHandler::recv() const -> Frame {
    struct can_frame frame;

    auto n = ::read(m_fd, &frame, CAN_MTU);
    if (n < 0) {
        std::cerr << "can socket read error" << std::endl;
        return Frame();
    }

    return Frame(frame.can_id, frame.can_dlc, frame.data);
}

auto SocketHandler::send(const Frame t_frame) -> int {
    struct can_frame frame;
    frame.can_id = t_frame.id();
    
    auto t_data = t_frame.data();
    auto n = t_data.size();
    frame.can_dlc = static_cast<uint8_t>(n);

    std::copy(t_data.begin(), t_data.end(), frame.data);
    
    auto r = ::write(m_fd, &frame, sizeof(frame));
    return r;
}

auto SocketHandler::addFilters(std::vector<Filter> t_filters) -> int {
    struct can_filter rfilter[t_filters.size()];
    int i = 0;
    for (auto it = t_filters.begin(); it != t_filters.end(); it++) {
        rfilter[i].can_id = it->m_id;
        rfilter[i].can_mask = it->m_mask;
        i++;
    }

    auto r = setsockopt(m_fd, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));
    return r;
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

Bus::Bus(const std::string& t_iface, const std::vector<Filter> t_filters)
: m_socket_handler(std::make_shared<SocketHandler>(t_iface))
{ 
    m_socket_handler->addFilters(t_filters); 
}

auto Bus::read() -> Frame {
    return m_socket_handler->recv();
}

auto Bus::write(const Frame t_frame) -> int {
    return m_socket_handler->send(t_frame);
}

}
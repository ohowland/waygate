#include "waygate.hpp"

#include <string>
#include <iostream>
#include <memory>

int main(int arc, char* argv[]) {
    fprintf(stdout, "%s version %d.%d\n",argv[0], 0, 1);

    auto sig1 = can::Signal("test1", 0, 2, 1);
    auto sig2 = can::Signal("test2", 2, 1, 1);
    auto msg = can::Message(123, can::LittleEndian)
        .addSignal(sig1)
        .addSignal(sig2);
    
    for (auto signal : msg.signals()) {
        std::cout << signal.name() << " -> " << signal.start() << std::endl;
    }

    auto bus = std::make_unique<can::Reader>("vcan0");
    auto r = msg.decode(bus->recv());

    for (auto const& [key, val] : r) {
        std::cout << key << ": " << val << std::endl;
    }
}
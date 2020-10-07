#include "waygate.hpp"

#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <map>

int main(int arc, char* argv[]) {
    fprintf(stdout, "%s version %d.%d\n",argv[0], 0, 1);

    auto sig1 = can::Signal("test1", 0, 2, 0.3);
    auto sig2 = can::Signal("test2", 2, 1, 10);
    auto sig3 = can::Signal("test3", 3, 1, 5.5);
    auto sigs = std::vector<can::Signal>({sig1, sig2, sig3});
    auto msg1 = can::Message("test_message", 291, sigs);

    auto msgs = std::vector<can::Message>({msg1});
    auto msg_handler = can::MessageHandler(msgs);

    auto filters = std::vector<can::Filter>();
    for (auto& msg : msgs) {
        filters.push_back(msg);
    }

    auto bus = std::make_unique<can::Bus>("vcan0", filters);

    while (1) {
        auto rframe = bus->read();
        auto r = msg_handler.decode(rframe);

        for (auto const& [key, val] : r) {
            std::cout << key << ": " << val << std::endl;
        }

        auto new_data = std::map<std::string, double>({{"test1", 3.0}});

        auto wframe = msg_handler.encode(new_data);
        auto w = bus->write(wframe);
    }
}
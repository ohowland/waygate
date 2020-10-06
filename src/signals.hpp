#ifndef CAN_SIGNALS_H_
#define CAN_SIGNALS_H_

#include <vector>
#include <map>
#include <string>

#include "frame.hpp"

namespace can {

enum Endianness {
    LittleEndian,
    BigEndian,
};
    
class Signal {
public:
    Signal(std::string t_name, uint8_t t_start, uint8_t t_size, double t_scale);

    auto name() const -> std::string { return m_name; }
    auto start() const -> int { return m_start; }
    auto size() const -> int { return m_size; }
    auto scale() const -> double { return m_scale; }

    auto parse_from(std::vector<uint8_t>, const Endianness) const -> std::pair<std::string, double>;
private:
    
    const std::string m_name;
    const int m_start;
    const int m_size;
    const double m_scale;
};

class Message {
public:
    Message(uint32_t t_name);
    Message(uint32_t t_name, Endianness t_endian);

    auto id() -> uint32_t { return m_id; }
    auto signals() -> std::vector<Signal> { return m_signals; }

    auto addSignal(Signal t_sig) -> Message&;
    auto decode(const Frame t_frame) const -> std::map<std::string, double>;
    // auto decode(std::vector<uint8_t> t_payload) -> std::map<std::string, double>;
private:

    const uint32_t m_id;
    const Endianness m_endian;
    std::vector<Signal> m_signals;
};


}
#endif
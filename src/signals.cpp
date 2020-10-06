#include "signals.hpp"
#include "frame.hpp"

#include <arpa/inet.h>

namespace can {

Signal::Signal(std::string t_name, uint8_t t_start, uint8_t t_size, double t_scale)
: m_name(t_name),
  m_start(static_cast<int>(t_start)),
  m_size(static_cast<int>(t_size)),
  m_scale(t_scale)
{ }

auto Signal::parse_from(std::vector<uint8_t> t_data, const Endianness t_endian) const -> std::pair<std::string, double> {
    
    int raw_val = 0;
    auto i = m_size - 1;
    for (auto it = t_data.begin() + m_start; it != t_data.begin() + m_start + m_size; it++) {
        raw_val = raw_val | *it << (i*8);
        i--;
    }

    auto scaled_val = static_cast<double>(raw_val) * m_scale; 

    return std::pair<std::string, double>(m_name, static_cast<double>(scaled_val));
}

auto compare_start_index(const Signal i, const Signal j) -> bool { return i.start() < j.start(); }


Message::Message(uint32_t t_id, Endianness t_endian)
: m_id(t_id),
  m_endian(t_endian),
  m_signals()
{ }

// define iterator for message signals: deliver signals ordered by ascending start bit.

auto Message::addSignal(Signal t_sig) -> Message& {
    m_signals.push_back(t_sig);
    return *this;
}

auto Message::decode(const Frame t_frame) const -> std::map<std::string, double> {
    std::map<std::string, double> decoded;

    auto const payload = t_frame.data();
    for (auto const& signal : m_signals) {
        auto v = signal.parse_from(payload, m_endian);
        decoded.insert(v);
    }

    return decoded;
}

}
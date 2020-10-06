#include "signals.hpp"
#include "frame.hpp"

#include <algorithm>

namespace can {

Signal::Signal(std::string t_name, uint8_t t_start, uint8_t t_size, double t_scale)
: m_name(t_name),
  m_start(static_cast<int>(t_start)),
  m_size(static_cast<int>(t_size)),
  m_scale(t_scale)
{ }

auto Signal::parse_from(const std::vector<uint8_t> t_data, const Endianness t_endian) const -> std::pair<std::string, double> {

    auto begin = t_data.begin() + m_start;
    auto end = t_data.begin() + m_start + m_size;

    int value = 0;
    switch (t_endian) {
    case LittleEndian:
        auto offset = 0;
        for (auto it = begin; it != end; it++) {
            value += *it << offset*8;
            offset++;
        }
        break;
    /*
    case BigEndian:
        auto offset = m_size;
        for reverse iterator
        break;
    */
    }

    return std::pair<std::string, double>(m_name, static_cast<double>(value));
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
    //std::sort(m_signals.begin(), m_signals.end(), compare_start_index);
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
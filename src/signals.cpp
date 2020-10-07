#include "signals.hpp"
#include "frame.hpp"

#include <map>
#include <vector>

#include <iostream>

namespace can {

Signal::Signal(std::string t_name, uint8_t t_start, uint8_t t_size, double t_scale)
: m_name(t_name),
  m_start(static_cast<int>(t_start)),
  m_size(static_cast<int>(t_size)),
  m_scale(t_scale)
{ }

auto Signal::parse_from(const std::vector<uint8_t> t_data) const -> double {
    
    int raw_val = 0;
    auto i = m_size - 1;
    for (auto it = t_data.begin() + m_start; it != t_data.begin() + m_start + m_size; it++) {
        raw_val = raw_val | *it << (i*8);
        i--;
    }

    return static_cast<double>(raw_val) * m_scale; 
}

Message::Message(const std::string& t_name, const uint32_t t_id, const std::vector<Signal> t_sigs)
: m_name(t_name),
  m_id(t_id),
  m_signals(t_sigs)
{ }

auto Message::decode(const Frame t_frame) const -> std::map<std::string, double> {
    std::map<std::string, double> decoded;

    auto const payload = t_frame.data();
    for (auto const& signal : m_signals) {
        auto scaled_val = signal.parse_from(payload);
        decoded.insert(std::pair<std::string, double>(signal.name(), scaled_val));
    }

    return decoded;
}

auto msg_vec_to_map_ids(std::vector<Message> t_msgs) -> std::map<uint32_t, Message> {
    std::map<uint32_t, Message> msg_map;
    for (auto msg : t_msgs) {
        msg_map.emplace(std::pair<uint32_t, Message>(msg.id(), msg));
    }
    return msg_map;
}

auto msg_vec_to_map_names(std::vector<Message> t_msgs) -> std::map<std::string, Message> {
    std::map<std::string, Message> msg_map;
    for (auto msg : t_msgs) {
        msg_map.emplace(std::pair<std::string, Message>(msg.name(), msg));
    }
    return msg_map;
}

MessageHandler::MessageHandler(std::vector<Message> t_msgs)
: m_msg_ids(msg_vec_to_map_ids(t_msgs)),
  m_msg_names(msg_vec_to_map_names(t_msgs))
{ }

auto MessageHandler::decode(const Frame t_frame) const -> std::map<std::string, double> {
    auto msg = m_msg_ids.at(t_frame.id());
    return msg.decode(t_frame);
}

// want to map(signal name, -> signal value) to return can frame.
// frames generally have multi signals, so not sure if want to save other values
// or to enforce rewire.

auto MessageHandler::encode(const std::map<std::string, double> t_data) -> Frame {
    for (auto const& [key, val] : t_data) {
        auto msg = m_msg_names.at(key);
    }
    return Frame();
}


}
#ifndef CAN_FRAME_H_
#define CAN_FRAME_H_

#include <ctime> 

namespace can {

class Frame {
public:
    Frame()
    : m_timestamp(std::time(0)),
      m_id(0),
      m_len(0),
      m_data()
    { }

    Frame(const uint32_t t_id, const uint8_t t_len, const unsigned char t_data[])
    : m_timestamp(std::time(0)),
      m_id(t_id),
      m_len(t_len),
      m_data(t_data, t_data + t_len)
    { }

    auto timestamp() const -> std::time_t { return m_timestamp; }
    auto id() const -> uint32_t { return m_id; }
    auto data() const -> std::vector<uint8_t> { return m_data; }
private:
    const std::time_t m_timestamp; 
    const uint32_t m_id;
    const uint8_t m_len;
    const std::vector<uint8_t> m_data;
};

}
#endif 
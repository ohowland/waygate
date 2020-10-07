#include "filter.hpp"
#include "signals.hpp"

namespace can {

Filter::Filter(uint32_t t_id, uint32_t t_mask)
: m_id(t_id),
  m_mask(t_mask)
{ }

Filter::Filter(Message t_msg)
: m_id(t_msg.id()),
  m_mask(CAN_SFF_MASK)
{ }

}
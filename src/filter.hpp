#ifndef CAN_FILTER_H_
#define CAN_FILTER_H_

#include "signals.hpp"

#include <linux/can.h>
#include <linux/can/raw.h>

namespace can {

struct Filter {
    Filter(uint32_t t_id, uint32_t t_mask);
    Filter(Message t_msg);

  const uint32_t m_id;
  const uint32_t m_mask;
};

}

#endif
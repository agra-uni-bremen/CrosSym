#pragma once

#include "registry.hpp"

#include <sysc/communication/sc_clock.h>
#include <sysc/communication/sc_signal.h>
#include <sysc/communication/sc_signal_ports.h>
#include <sysc/communication/sc_writer_policy.h>

#include <sysc/datatypes/bit/sc_bv_base.h>
#include <sysc/datatypes/int/sc_nbdefs.h>
#include <sysc/datatypes/int/sc_uint.h>

#include <sysc/kernel/sc_module.h>
#include <sysc/kernel/sc_time.h>
#include <sysc/kernel/sc_event.h>

#include <tlm_core/tlm_2/tlm_generic_payload/tlm_gp.h>
#include <tlm_utils/simple_target_socket.h>

using namespace sc_dt;
using namespace sc_core;

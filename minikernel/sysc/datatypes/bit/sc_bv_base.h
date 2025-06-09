#pragma once

#include <sysc/datatypes/int/sc_nbdefs.h>

namespace sc_dt {

	class sc_bv_base {

	public:
		explicit sc_bv_base(int) : m_data(nullptr){}

	protected:
		sc_digit* m_data; // data array
	};
}

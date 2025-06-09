#pragma once

namespace sc_core {

	enum sc_writer_policy {
		SC_ONE_WRITER = 0,
		SC_MANY_WRITERS = 1
	};

	const sc_writer_policy SC_DEFAULT_WRITER_POLICY = SC_ONE_WRITER;
}

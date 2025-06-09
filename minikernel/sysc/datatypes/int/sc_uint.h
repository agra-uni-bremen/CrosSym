#pragma once

namespace sc_dt
{

// classes defined in this module
	template <int W> class sc_uint;


	template <int W>
	class sc_uint
	{
		int data; // TODO "int" is currently fine, not necessarily always
	public:
		sc_uint() : data(0) {}
		sc_uint(const int a) : data(a) {}

		sc_uint<W>& operator = ( const int a )
		{
			data = a;
			return *this;
		}

		operator int() const {
			return data;
		}

		bool operator <= ( const int a ) {
			return (data <= a); // TODO richtigrum?
		}

		bool operator >= ( const int a ) {
			return (data >= a);
		}
	};

} // namespace sc_dt

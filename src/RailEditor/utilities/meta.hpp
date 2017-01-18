#pragma once

#include <cstdlib>

namespace utilities {
	template <bool val1, bool... vals>
	struct true_count {
		constexpr static std::size_t value = std::size_t(val1) + true_count<vals...>::value;
	};

	template <bool val>
	struct true_count<val> {
		constexpr static std::size_t value = val;
	};

	template <bool... vals>
	struct conjuction {
		constexpr static bool value = true_count<vals...>::value == sizeof...(vals);
	};

	template <bool... vals>
	struct disjunction {
		constexpr static bool value = true_count<vals...>::value >= 1;
	};

	template <class A, class... Types>
	struct has_one {
		constexpr static bool value =
		    true_count<std::is_same<typename std::remove_reference<A>::type,
		                            typename std::remove_reference<Types>::type>::value...>::value == 1;
	};

	template <class A, class... Types>
	struct has_no_more_than_one {
		constexpr static bool value =
		    true_count<std::is_same<typename std::remove_reference<A>::type,
		                            typename std::remove_reference<Types>::type>::value...>::value <= 1;
	};

	template <class A, class... Types>
	struct has_none {
		constexpr static bool value =
		    true_count<std::is_same<typename std::remove_reference<A>::type,
		                            typename std::remove_reference<Types>::type>::value...>::value == 0;
	};
}

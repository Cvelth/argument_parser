#include "argument_parser.hpp"
#include "argument_tester.hpp"

template <typename type>
class constructible_from_type {
public:
	explicit constexpr constructible_from_type(type const &v) : value_(v) {}
	constexpr bool operator==(constructible_from_type<type> const &o) const {
		return value_ == o.value_;
	}

protected:
	type value_;
};
using constructible_from_int		 = constructible_from_type<int>;
using constructible_from_bool		 = constructible_from_type<bool>;
using constructible_from_double		 = constructible_from_type<double>;
using constructible_from_string		 = constructible_from_type<std::string>;
using constructible_from_string_view = constructible_from_type<std::string_view>;
class constructible_from_char_star : public constructible_from_type<char const *> {
public:
	using constructible_from_type<char const *>::constructible_from_type;
	bool operator==(constructible_from_char_star const &o) const {
		return !std::strcmp(value_, o.value_);
	}
};

TEST_CASE("Arguments object with a single <-integer> value.") {
	using namespace ap;
	arguments test_arguments{value{"integer"}};

	SUBCASE("No arguments") {
		// ./executable
		argument_tester(parse_helper(test_arguments, {}))
			.no_errors()
			.no_warnings()
			.argument_is_equal("integer", false)
			.argument_is_not_equal("integer", 0)
			.argument_is_not_equal("integer", 0u)
			.argument_is_not_equal("integer", (short) 0)
			.argument_is_not_equal("integer", (unsigned short) 0)
			.argument_is_not_equal("integer", 0l)
			.argument_is_not_equal("integer", 0ul)
			.argument_is_not_equal("integer", 0ll)
			.argument_is_not_equal("integer", 0ull)
			.argument_is_not_equal("integer", 0.0f)
			.argument_is_not_equal("integer", 0.0)
			.argument_is_not_equal("integer", 0.0l)
			.argument_is_not_equal("integer", '\0')
			.argument_is_not_equal("integer", (unsigned char) 0)
			.argument_is_not_equal("integer", (signed char) 0)
			.argument_is_equal("integer", "")
			.argument_is_equal("integer", std::string{})
			.argument_is_equal("integer", std::string_view{})
			.argument_is_not_equal("integer", constructible_from_int{0})
			.argument_is_equal("integer", constructible_from_bool{false})
			.argument_is_not_equal("integer", constructible_from_double{0.0})
			.argument_is_equal("integer", constructible_from_string{std::string{}})
			.argument_is_equal("integer", constructible_from_string_view{std::string_view{}})
			.argument_is_equal("integer", constructible_from_char_star{""});
	}
}
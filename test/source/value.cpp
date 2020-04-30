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

TEST_CASE("Arguments object with a single <-value> option.") {
	using namespace ap;
	arguments test_arguments{value{"value"}};

	SUBCASE("Passing no arguments") {
		// ./executable
		argument_tester(parse_helper(test_arguments, {}))
			.no_errors()
			.no_warnings()

			.argument_is_convertible_to<bool>("value")
			.argument_is_equal("value", false)

			.argument_is_not_convertible_to<signed int>("value")
			.argument_is_not_convertible_to<unsigned int>("value")
			.argument_is_not_convertible_to<signed short>("value")
			.argument_is_not_convertible_to<unsigned short>("value")
			.argument_is_not_convertible_to<signed long>("value")
			.argument_is_not_convertible_to<unsigned long>("value")
			.argument_is_not_convertible_to<signed long long>("value")
			.argument_is_not_convertible_to<unsigned long long>("value")

			.argument_is_not_convertible_to<float>("value")
			.argument_is_not_convertible_to<double>("value")
			.argument_is_not_convertible_to<long double>("value")

			.argument_is_not_convertible_to<unsigned char>("value")
			.argument_is_not_convertible_to<signed char>("value")

			.argument_is_convertible_to<char const *>("value")
			.argument_is_not_convertible_to<char *>("value")  // by design
			.argument_is_convertible_to<std::string>("value")
			.argument_is_convertible_to<std::string_view>("value")
			.argument_is_equal("value", "")
			.argument_is_equal("value", std::string{})
			.argument_is_equal("value", std::string_view{})

			.argument_is_convertible_to<constructible_from_bool>("value")
			.argument_is_convertible_to<constructible_from_string>("value")
			.argument_is_convertible_to<constructible_from_string_view>("value")
			.argument_is_convertible_to<constructible_from_char_star>("value")
			.argument_is_equal("value", constructible_from_bool{false})
			.argument_is_equal("value", constructible_from_string{std::string{}})
			.argument_is_equal("value", constructible_from_string_view{std::string_view{}})
			.argument_is_equal("value", constructible_from_char_star{""})

			.argument_is_not_convertible_to<constructible_from_int>("value")
			.argument_is_not_convertible_to<constructible_from_double>("value");
	}

	SUBCASE("Passing a single string argument") {
		// ./executable -value="hello world"
		argument_tester(parse_helper(test_arguments, {"-value=\"hello world\""}))
			.no_errors()
			.no_warnings()

			.argument_is_convertible_to<bool>("value")
			.argument_is_equal("value", true)

			.argument_is_not_convertible_to<signed int>("value")
			.argument_is_not_convertible_to<unsigned int>("value")
			.argument_is_not_convertible_to<signed short>("value")
			.argument_is_not_convertible_to<unsigned short>("value")
			.argument_is_not_convertible_to<signed long>("value")
			.argument_is_not_convertible_to<unsigned long>("value")
			.argument_is_not_convertible_to<signed long long>("value")
			.argument_is_not_convertible_to<unsigned long long>("value")

			.argument_is_not_convertible_to<float>("value")
			.argument_is_not_convertible_to<double>("value")
			.argument_is_not_convertible_to<long double>("value")

			.argument_is_not_convertible_to<unsigned char>("value")
			.argument_is_not_convertible_to<signed char>("value")

			.argument_is_convertible_to<char const *>("value")
			.argument_is_not_convertible_to<char *>("value")  // by design
			.argument_is_convertible_to<std::string>("value")
			.argument_is_convertible_to<std::string_view>("value")
			.argument_is_equal("value", "hello world")
			.argument_is_equal("value", std::string{"hello world"})
			.argument_is_equal("value", std::string_view{"hello world"})

			.argument_is_convertible_to<constructible_from_bool>("value")
			.argument_is_convertible_to<constructible_from_string>("value")
			.argument_is_convertible_to<constructible_from_string_view>("value")
			.argument_is_convertible_to<constructible_from_char_star>("value")
			.argument_is_equal("value", constructible_from_bool{true})
			.argument_is_equal("value", constructible_from_string{std::string{"hello world"}})
			.argument_is_equal("value",
							   constructible_from_string_view{std::string_view{"hello world"}})
			.argument_is_equal("value", constructible_from_char_star{"hello world"})

			.argument_is_not_convertible_to<constructible_from_int>("value")
			.argument_is_not_convertible_to<constructible_from_double>("value");
	}
}
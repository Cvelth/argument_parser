#include "argument_parser.hpp"
#include "argument_tester.hpp"

TEST_CASE("Arguments object with a single <-counter> argument.") {
	using namespace ap;
	arguments test_arguments{counter{"counter", "-counter", "c", "x"}};

	SUBCASE("Passing no arguments") {
		// ./executable
		ap::argument_tester(parse_helper(test_arguments, {}))
			.no_errors()
			.no_warnings()

			.argument_is_convertible_to<bool>("counter")
			.argument_is_equal("counter", false)

			.argument_is_convertible_to<signed int>("counter")
			.argument_is_convertible_to<unsigned int>("counter")
			.argument_is_convertible_to<signed short>("counter")
			.argument_is_convertible_to<unsigned short>("counter")
			.argument_is_convertible_to<signed long>("counter")
			.argument_is_convertible_to<unsigned long>("counter")
			.argument_is_convertible_to<signed long long>("counter")
			.argument_is_convertible_to<unsigned long long>("counter")
			.argument_is_equal("counter", 0)
			.argument_is_equal("counter", 0u)
			.argument_is_equal("counter", (signed short) 0)
			.argument_is_equal("counter", (unsigned short) 0)
			.argument_is_equal("counter", 0l)
			.argument_is_equal("counter", 0ul)
			.argument_is_equal("counter", 0ll)
			.argument_is_equal("counter", 0ull)

			.argument_is_convertible_to<float>("counter")
			.argument_is_convertible_to<double>("counter")
			.argument_is_convertible_to<long double>("counter")
			.argument_is_equal("counter", 0.f)
			.argument_is_equal("counter", 0.0)
			.argument_is_equal("counter", 0.l)

			.argument_is_convertible_to<signed char>("counter")
			.argument_is_convertible_to<unsigned char>("counter")
			.argument_is_equal("counter", (signed char) 0)
			.argument_is_equal("counter", (unsigned char) 0)

			.argument_is_not_convertible_to<char const *>("counter")
			.argument_is_not_convertible_to<char *>("counter")
			.argument_is_not_convertible_to<std::string>("counter")
			.argument_is_not_convertible_to<std::string_view>("counter")

			.argument_is_convertible_to<constructible_from_bool>("counter")
			.argument_is_equal("counter", constructible_from_bool{false})

			.argument_is_convertible_to<constructible_from_int>("counter")
			.argument_is_convertible_to<constructible_from_double>("counter")
			.argument_is_equal("counter", constructible_from_int{0})
			.argument_is_equal("counter", constructible_from_double{0.0})

			.argument_is_not_convertible_to<constructible_from_string>("counter")
			.argument_is_not_convertible_to<constructible_from_string_view>("counter")
			.argument_is_not_convertible_to<constructible_from_char_star>("counter");
	};

	SUBCASE("Repeating the call 7 times") {
		// ./executable -counter -counter --counter -x -c -c -x
		ap::argument_tester(parse_helper(test_arguments, {"-counter", "-counter", "--counter", "-x",
														  "-c", "-c", "-x"}))
			.no_errors()
			.no_warnings()

			.argument_is_convertible_to<bool>("counter")
			.argument_is_equal("counter", true)

			.argument_is_convertible_to<signed int>("counter")
			.argument_is_convertible_to<unsigned int>("counter")
			.argument_is_convertible_to<signed short>("counter")
			.argument_is_convertible_to<unsigned short>("counter")
			.argument_is_convertible_to<signed long>("counter")
			.argument_is_convertible_to<unsigned long>("counter")
			.argument_is_convertible_to<signed long long>("counter")
			.argument_is_convertible_to<unsigned long long>("counter")
			.argument_is_equal("counter", 7)
			.argument_is_equal("counter", 7u)
			.argument_is_equal("counter", (signed short) 7)
			.argument_is_equal("counter", (unsigned short) 7)
			.argument_is_equal("counter", 7l)
			.argument_is_equal("counter", 7ul)
			.argument_is_equal("counter", 7ll)
			.argument_is_equal("counter", 7ull)

			.argument_is_convertible_to<float>("counter")
			.argument_is_convertible_to<double>("counter")
			.argument_is_convertible_to<long double>("counter")
			.argument_is_equal("counter", 7.f)
			.argument_is_equal("counter", 7.0)
			.argument_is_equal("counter", 7.l)

			.argument_is_convertible_to<signed char>("counter")
			.argument_is_convertible_to<unsigned char>("counter")
			.argument_is_equal("counter", (signed char) 7)
			.argument_is_equal("counter", (unsigned char) 7)

			.argument_is_not_convertible_to<char const *>("counter")
			.argument_is_not_convertible_to<char *>("counter")
			.argument_is_not_convertible_to<std::string>("counter")
			.argument_is_not_convertible_to<std::string_view>("counter")

			.argument_is_convertible_to<constructible_from_bool>("counter")
			.argument_is_equal("counter", constructible_from_bool{true})

			.argument_is_convertible_to<constructible_from_int>("counter")
			.argument_is_convertible_to<constructible_from_double>("counter")
			.argument_is_equal("counter", constructible_from_int{7})
			.argument_is_equal("counter", constructible_from_double{7.0})

			.argument_is_not_convertible_to<constructible_from_string>("counter")
			.argument_is_not_convertible_to<constructible_from_string_view>("counter")
			.argument_is_not_convertible_to<constructible_from_char_star>("counter");
	};
}

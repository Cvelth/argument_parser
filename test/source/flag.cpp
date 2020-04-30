#include "argument_parser.hpp"
#include "argument_tester.hpp"

TEST_CASE("Arguments object with a single <-help> flag.") {
	using namespace ap;
	arguments test_arguments{flag{"help", "-help", "h"}};

	SUBCASE("Passing no arguments") {
		// ./executable
		argument_tester(parse_helper(test_arguments, {}))
			.errors({})
			.warnings({})
			.argument_is_equal("help", false);
	}

	SUBCASE("Passing a single unknown argument") {
		// ./executable -x
		argument_tester(parse_helper(test_arguments, {"-x"}))
			.errors({"Unknown argument: <-x>.\n"})
			.warnings({})
			.argument_is_equal("help", false);
	}

	SUBCASE("Passing a single unknown value") {
		// ./executable xx
		argument_tester(parse_helper(test_arguments, {"xx"}))
			.errors({})
			.warnings({"Value \"xx\" was ignored.\n"})
			.argument_is_equal("help", false);
	}

	SUBCASE("Passing a single <-help> argument ") {
		// ./executable -help
		argument_tester(parse_helper(test_arguments, {"-help"}))
			.errors({})
			.warnings({})
			.argument_is_equal("help", true);
	}

	SUBCASE("Passing a single <--help> argument ") {
		// ./executable --help
		argument_tester(parse_helper(test_arguments, {"--help"}))
			.errors({})
			.warnings({})
			.argument_is_equal("help", true);
	}

	SUBCASE("Passing a single <---help> argument ") {
		// ./executable ---help
		argument_tester(parse_helper(test_arguments, {"---help"}))
			.errors({"Unknown argument: <---help>.\n"})
			.warnings({})
			.argument_is_equal("help", false);
	}

	SUBCASE("Passing a single <help> argument ") {
		// ./executable help
		argument_tester(parse_helper(test_arguments, {"help"}))
			.errors({})
			.warnings({"Value \"help\" was ignored.\n"})
			.argument_is_equal("help", false);
	}

	SUBCASE("Passing a single <h> argument ") {
		// ./executable h
		argument_tester(parse_helper(test_arguments, {"h"}))
			.errors({})
			.warnings({"Value \"h\" was ignored.\n"})
			.argument_is_equal("help", false);
	}

	SUBCASE("Passing a single <-h> argument ") {
		// ./executable -h
		argument_tester(parse_helper(test_arguments, {"-h"}))
			.errors({})
			.warnings({})
			.argument_is_equal("help", true);
	}

	SUBCASE("Passing a single <--h> argument ") {
		// ./executable --h
		argument_tester(parse_helper(test_arguments, {"--h"}))
			.errors({"Unknown argument: <--h>.\n"})
			.warnings({})
			.argument_is_equal("help", false);
	}

	SUBCASE("Passing a single non-existing argument") {
		// ./executable
		argument_tester(parse_helper(test_arguments, {}))
			.errors({})
			.warnings({})
			.argument_is_equal("help", false)
			.argument_does_not_exist("x");
	}
}

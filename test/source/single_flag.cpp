#include "argument_parser.hpp"
#include "argument_tester.hpp"

TEST_CASE("Arguments object with a single <-help> flag.") {
	using namespace ap;
	arguments test_arguments{flag{"help", "-help", "h"}};

	SUBCASE("No arguments") {
		// ./executable
		argument_tester(parse_helper(test_arguments, {}))
			.errors({})
			.warnings({})
			.argument("help", false);
	}

	SUBCASE("Unknown argument") {
		// ./executable -x
		argument_tester(parse_helper(test_arguments, {"-x"}))
			.errors({"Unknown argument: <-x>.\n"})
			.warnings({})
			.argument("help", false);
	}

	SUBCASE("Unknown value") {
		// ./executable xx
		argument_tester(parse_helper(test_arguments, {"xx"}))
			.errors({})
			.warnings({"Value \"xx\" was ignored.\n"})
			.argument("help", false);
	}

	SUBCASE("<-help> argument ") {
		// ./executable -help
		argument_tester(parse_helper(test_arguments, {"-help"}))
			.errors({})
			.warnings({})
			.argument("help", true);
	}

	SUBCASE("<--help> argument ") {
		// ./executable --help
		argument_tester(parse_helper(test_arguments, {"--help"}))
			.errors({})
			.warnings({})
			.argument("help", true);
	}

	SUBCASE("<---help> argument ") {
		// ./executable ---help
		argument_tester(parse_helper(test_arguments, {"---help"}))
			.errors({"Unknown argument: <---help>.\n"})
			.warnings({})
			.argument("help", false);
	}

	SUBCASE("<help> argument ") {
		// ./executable help
		argument_tester(parse_helper(test_arguments, {"help"}))
			.errors({})
			.warnings({"Value \"help\" was ignored.\n"})
			.argument("help", false);
	}

	SUBCASE("<h> argument ") {
		// ./executable h
		argument_tester(parse_helper(test_arguments, {"h"}))
			.errors({})
			.warnings({"Value \"h\" was ignored.\n"})
			.argument("help", false);
	}

	SUBCASE("<-h> argument ") {
		// ./executable -h
		argument_tester(parse_helper(test_arguments, {"-h"}))
			.errors({})
			.warnings({})
			.argument("help", true);
	}

	SUBCASE("<--h> argument ") {
		// ./executable --h
		argument_tester(parse_helper(test_arguments, {"--h"}))
			.errors({"Unknown argument: <--h>.\n"})
			.warnings({})
			.argument("help", false);
	}

	SUBCASE("Non-existing argument") {
		// ./executable
		argument_tester(parse_helper(test_arguments, {}))
			.errors({})
			.warnings({})
			.argument("help", false)
			.argument("x", ap::fail);
	}
}

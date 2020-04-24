#pragma once
#include <doctest/doctest.h>

namespace ap {
	class argument_tester;

	class fail_t {
		friend argument_tester;

	public:
		explicit constexpr fail_t() : value_(true) {}
		constexpr operator bool() const { return value_; }

	private:
		explicit constexpr fail_t(bool value) : value_(value) {}

	private:
		const bool value_;
	};

	class argument_tester {
	public:
		argument_tester(ap::parsing_results const &results) : results_(results) {}

		argument_tester &errors(std::vector<std::string> expected_errors) {
			CHECK_EQ(results_.errors(), expected_errors);
			return *this;
		}
		argument_tester &warnings(std::vector<std::string> expected_warnings) {
			CHECK_EQ(results_.warnings(), expected_warnings);
			return *this;
		}

		argument_tester &argument(char const *name, fail_t fail_ = fail_t(false)) {
			auto temp = results_.get(name);
			if (fail_)
				CHECK_FALSE(temp);
			else
				CHECK(temp);
			return *this;
		}
		template <typename T>
		argument_tester &argument(char const *name, T const &value, fail_t fail_ = fail_t(false)) {
			auto temp = results_.get(name);
			REQUIRE(temp);

			if (fail_)
				CHECK_NE(*temp, value);
			else
				CHECK_EQ(*temp, value);
			return *this;
		}

		argument_tester &operator()(char const *name, fail_t fail_ = fail_t(false)) {
			return argument(name, fail_);
		}
		template <typename T>
		argument_tester &operator()(char const *name, T const &value,
									fail_t fail_ = fail_t(false)) {
			return argument(name, value, fail_);
		}

	protected:
		ap::parsing_results const &results_;
	};

	ap::parsing_results parse_helper(ap::arguments &arguments,
									 std::initializer_list<std::string> &&values) {
		std::vector<std::string> local{std::move(values)};
		std::string first = "argument_tester";

		int argc	= static_cast<int>(local.size() + 1);
		char **argv = new char *[local.size() + 2];

		argv[0] = first.data();
		for (size_t i = 1; auto &value : local) {
			argv[i] = value.data();
			i++;
		}
		argv[argc] = nullptr;

		auto out = arguments.parse(argc, argv);

		delete[] argv;

		return out;
	}

	inline constexpr fail_t fail;
}  // namespace ap
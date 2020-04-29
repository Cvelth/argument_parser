#pragma once
#include <algorithm>
#include <cmath>
#include <doctest/doctest.h>

#ifdef __GNUG__
	#include <cstdlib>
	#include <cxxabi.h>
	#include <memory>

namespace ap::detail {
	inline std::string demangle(const char *name) {
		int status = -1;
		std::unique_ptr<char, void (*)(void *)> res{abi::__cxa_demangle(name, NULL, NULL, &status),
													std::free};
		return (status == 0) ? res.get() : name;
	}
#else
namespace ap::detail {
	inline std::string demangle(const char *name) { return name; }
#endif
	template <typename T>
	inline std::string get_typename() {
		return demangle(typeid(T).name());
	}
}  // namespace ap::detail

namespace ap {
	class argument_tester {
	public:
		inline argument_tester(ap::parsing_results const &results) : results_(results) {}

		inline argument_tester &errors(std::vector<std::string> expected_errors) {
			INFO("while checking errors with: " + results_.error_string());
			CHECK_EQ(results_.errors(), expected_errors);
			return *this;
		}
		inline argument_tester &warnings(std::vector<std::string> expected_warnings) {
			INFO("while checking warnings with: " + results_.warning_string());
			CHECK_EQ(results_.warnings(), expected_warnings);
			return *this;
		}

		inline argument_tester &no_errors() { return errors({}); }
		inline argument_tester &no_warnings() { return warnings({}); }

		inline argument_tester &argument_exists(char const *name) {
			CHECK(results_.get(name));
			return *this;
		}
		inline argument_tester &argument_does_not_exist(char const *name) {
			CHECK_FALSE(results_.get(name));
			return *this;
		}

		template <typename T>
		inline argument_tester &argument_is_convertible_to(char const *name) {
			INFO("while checking whether argument <-" << name << "> is convertible to a type: `"
													  << detail::get_typename<T>() << "`");
			auto result = results_.get(name);
			REQUIRE(result);
			CHECK(result->get<T>());

			return *this;
		}
		template <typename T>
		inline argument_tester &argument_is_not_convertible_to(char const *name) {
			INFO("while checking whether argument <-" << name << "> is not convertible to a type: `"
													  << detail::get_typename<T>() << "`");
			auto result = results_.get(name);
			REQUIRE(result);
			CHECK_FALSE(result->get<T>());

			return *this;
		}

		inline argument_tester &argument_is_equal(char const *name, char const *value) {
			return argument_is_equal<char const *>(name, value);
		}
		inline argument_tester &argument_is_not_equal(char const *name, char const *value) {
			return argument_is_not_equal<char const *>(name, value);
		}
		template <typename T>
		inline argument_tester &argument_is_equal(char const *name, T const &value) {
			INFO("while checking whether argument <-" << name << "> is equal with a value of type `"
													  << detail::get_typename<T>()
													  << "`: " << value);
			auto result = results_.get(name);
			REQUIRE(result);
			auto result_value = result->get<T>();
			REQUIRE(result_value);
			CHECK(check_equality(*result_value, value));

			return *this;
		}
		template <typename T>
		inline argument_tester &argument_is_not_equal(char const *name, T const &value) {
			INFO("while checking whether argument <-"
				 << name << "> is not equal with a value of type `" << detail::get_typename<T>()
				 << "`: " << value);
			auto result = results_.get(name);
			REQUIRE(result);
			auto result_value = result->get<T>();
			REQUIRE(result_value);
			CHECK_FALSE(check_equality(*result_value, value));

			return *this;
		}

		inline argument_tester &operator()() {
			no_warnings();
			return no_errors();
		}
		inline argument_tester &operator()(char const *name) { return argument_exists(name); }
		template <typename T>
		inline argument_tester &operator()(char const *name, T const &value) {
			return argument_is_equal(name, value);
		}

	protected:
		template <typename T>
		inline bool check_equality(T const &lhs, T const &rhs) {
			if constexpr (std::is_same_v<T, char const *> || std::is_same_v<T, char *>)
				return !std::strcmp(lhs, rhs);
			else if constexpr (std::is_floating_point_v<T>)
				return std::fabs(lhs - rhs) <= std::numeric_limits<T>::epsilon() *
												   (1.0 + std::max(std::fabs(lhs), std::fabs(rhs)));
			else
				return lhs == rhs;
		}

	protected:
		ap::parsing_results const &results_;
	};

	inline ap::parsing_results parse_helper(ap::arguments &arguments,
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
}  // namespace ap
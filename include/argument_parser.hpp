#pragma once
#include <charconv>
#include <cstring>
#include <functional>
#include <memory>
#include <numeric>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <typeinfo>
#include <vector>

// temporary solution. Should be removed after FP charconv is implemented by the compilers.
#ifndef _MSC_VER
	#include <sstream>
#endif

namespace ap {
	class arguments;
	namespace detail {
		class argument_visitor;
		class result;

		class argument {
			friend arguments;
			friend result;

		public:
			[[nodiscard]] inline operator bool() const noexcept { return was_activated; }

		protected:
			template <typename... Ts>
			inline explicit argument(
				std::optional<std::function<void()>> const &no_value_callable,
				std::optional<std::function<void(std::string_view const &)>> const &value_callable,
				std::optional<std::function<void(argument_visitor const &)>> const &visitor_callable,
				std::string const &t, Ts const &... ts)
				: name(t), aliases{t, ts...}, no_value_callable(no_value_callable),
				  value_callable(value_callable),
				  visitor_callable(visitor_callable ? visitor_callable
													: [](auto const &v) { return v(); }),
				  was_activated(false) {}

		protected:
			[[nodiscard]] inline bool is_callable() const noexcept {
				return no_value_callable.has_value();
			}
			[[nodiscard]] inline bool is_value_callable() const noexcept {
				return value_callable.has_value();
			}

			inline bool try_call() noexcept {
				if (no_value_callable) {
					(*no_value_callable)();
					was_activated = true;
					return true;
				} else
					return false;
			}
			inline bool try_call(std::string_view const &sv) noexcept {
				if (value_callable) {
					(*value_callable)(sv);
					was_activated = true;
					return true;
				} else
					return false;
			}

		public:
			const std::string name;
			const std::set<std::string> aliases;

		private:
			const std::optional<std::function<void()>> no_value_callable;
			const std::optional<std::function<void(std::string_view)>> value_callable;
			const std::optional<std::function<void(argument_visitor const &)>> visitor_callable;

			bool was_activated;
		};

		class argument_visitor {
		public:
			inline argument_visitor(
				std::optional<std::function<void()>> const &_default	  = std::nullopt,
				std::optional<std::function<void(size_t)>> const &counter = std::nullopt,
				std::optional<std::function<void(std::string_view const &value)>> const &value =
					std::nullopt) noexcept
				: default_(_default), counter_(counter), value_(value) {}
			inline argument_visitor(
				std::optional<std::function<void()>> const &_default = std::nullopt,
				std::optional<std::function<void(std::string_view const &value)>> const &value =
					std::nullopt,
				std::optional<std::function<void(size_t)>> const &counter = std::nullopt) noexcept
				: default_(_default), counter_(counter), value_(value) {}

			inline argument_visitor(
				std::optional<std::function<void(size_t)>> const &counter = std::nullopt,
				std::optional<std::function<void()>> const &_default	  = std::nullopt,
				std::optional<std::function<void(std::string_view const &value)>> const &value =
					std::nullopt) noexcept
				: default_(_default), counter_(counter), value_(value) {}
			inline argument_visitor(
				std::optional<std::function<void(size_t)>> const &counter = std::nullopt,
				std::optional<std::function<void(std::string_view const &value)>> const &value =
					std::nullopt,
				std::optional<std::function<void()>> const &_default = std::nullopt) noexcept
				: default_(_default), counter_(counter), value_(value) {}

			inline argument_visitor(
				std::optional<std::function<void(std::string_view const &value)>> const &value =
					std::nullopt,
				std::optional<std::function<void()>> const &_default	  = std::nullopt,
				std::optional<std::function<void(size_t)>> const &counter = std::nullopt) noexcept
				: default_(_default), counter_(counter), value_(value) {}
			inline argument_visitor(
				std::optional<std::function<void(std::string_view const &value)>> const &value =
					std::nullopt,
				std::optional<std::function<void(size_t)>> const &counter = std::nullopt,
				std::optional<std::function<void()>> const &_default	  = std::nullopt) noexcept
				: default_(_default), counter_(counter), value_(value) {}

			inline void operator()() const noexcept {
				if (default_) (*default_)();
			}
			inline void operator()(size_t value) const noexcept {
				if (counter_) (*counter_)(value);
			}
			inline void operator()(std::string_view const &value) const noexcept {
				if (value_) (*value_)(value);
			}

		private:
			std::optional<std::function<void()>> default_;
			std::optional<std::function<void(size_t)>> counter_;
			std::optional<std::function<void(std::string_view const &value)>> value_;
		};

		class result {
		public:
			inline explicit result(argument const &argument) noexcept : argument_(argument) {}

			template <typename T>
			inline std::optional<T> get() const;

			template <typename T>
			inline T as() const;

			template <typename T>
			inline T value_or(T value) const;

			inline operator bool() const noexcept { return argument_; }

		protected:
			argument const &argument_;
		};

		class callable : public argument {
		public:
			template <typename... Ts>
			inline static std::unique_ptr<argument> create(
				std::function<void()> const &no_value_callable,
				std::function<void(std::string_view const &)> const &value_callable,
				std::string const &t, Ts const &... ts) noexcept {
				return std::unique_ptr<argument>(
					new callable(no_value_callable, value_callable, std::nullopt, t, ts...));
			}

		protected:
			using argument::argument;
		};

		class flag : public argument {
		public:
			template <typename... Ts>
			inline static std::unique_ptr<argument> create(std::string const &t,
														   Ts const &... ts) noexcept {
				return std::unique_ptr<argument>(new flag(t, ts...));
			}

		protected:
			template <typename... Ts>
			inline explicit flag(std::string const &t, Ts const &... ts) noexcept
				: argument([]() {}, std::nullopt, std::nullopt, t, ts...) {}
		};

		class value : public argument {
			friend result;

		public:
			template <typename... Ts>
			inline static std::unique_ptr<argument> create(std::string const &t,
														   Ts const &... ts) noexcept {
				return std::unique_ptr<argument>(new value(t, ts...));
			}

		protected:
			template <typename... Ts>
			inline explicit value(std::string const &t, Ts const &... ts) noexcept
				: argument(
					  std::nullopt, [this](auto v) { value_ = v; },
					  [this](auto const &v) { return v(value_); }, t, ts...),
				  value_("") {}

		protected:
			std::string value_;
		};

		class positional : public value {
			friend result;

		public:
			template <typename... Ts>
			inline static std::unique_ptr<argument> create(std::string const &t,
														   Ts const &... ts) noexcept {
				return std::unique_ptr<argument>(new positional(t, ts...));
			}

		protected:
			using value::value;
		};

		class counter : public argument {
			friend result;

		public:
			template <typename... Ts>
			inline static std::unique_ptr<argument> create(std::string const &t,
														   Ts const &... ts) noexcept {
				return std::unique_ptr<argument>(new counter(t, ts...));
			}

		protected:
			template <typename... Ts>
			inline explicit counter(std::string const &t, Ts const &... ts) noexcept
				: argument([this]() { ++value_; }, std::nullopt,
						   [this](auto const &v) { return v(value_); }, t, ts...),
				  value_(0u) {}

		protected:
			std::size_t value_;
		};
	}  // namespace detail

	template <typename... Ts>
	inline static std::unique_ptr<detail::argument> callable(std::string const &t,
															 Ts const &... ts) noexcept {
		return detail::callable::create(std::nullopt, std::nullopt, t, ts...);
	}

	template <typename... Ts>
	inline static std::unique_ptr<detail::argument> callable(
		std::function<void()> const &no_value_callable, std::string const &t,
		Ts const &... ts) noexcept {
		return detail::callable::create(no_value_callable, std::nullopt, t, ts...);
	}

	template <typename... Ts>
	inline static std::unique_ptr<detail::argument> callable(
		std::function<void(std::string_view const &)> const &value_callable, std::string const &t,
		Ts const &... ts) noexcept {
		return detail::callable::create(std::nullopt, value_callable, t, ts...);
	}

	template <typename... Ts>
	inline static std::unique_ptr<detail::argument> callable(
		std::function<void()> const &no_value_callable,
		std::function<void(std::string_view const &)> const &value_callable, std::string const &t,
		Ts const &... ts) noexcept {
		return detail::callable::create(no_value_callable, value_callable, t, ts...);
	}

	template <typename... Ts>
	inline static std::unique_ptr<detail::argument> callable(
		std::function<void(std::string_view const &)> const &value_callable,
		std::function<void()> const &no_value_callable, std::string const &t,
		Ts const &... ts) noexcept {
		return detail::callable::create(no_value_callable, value_callable, t, ts...);
	}

	template <typename... Ts>
	inline static std::unique_ptr<detail::argument> flag(std::string const &t,
														 Ts const &... ts) noexcept {
		return detail::flag::create(t, ts...);
	}
	template <typename... Ts>
	inline static std::unique_ptr<detail::argument> value(std::string const &t,
														  Ts const &... ts) noexcept {
		return detail::value::create(t, ts...);
	}
	template <typename... Ts>
	inline static std::unique_ptr<detail::argument> positional(std::string const &t,
															   Ts const &... ts) noexcept {
		return detail::positional::create(t, ts...);
	}
	template <typename... Ts>
	inline static std::unique_ptr<detail::argument> counter(std::string const &t,
															Ts const &... ts) noexcept {
		return detail::counter::create(t, ts...);
	}

	class parsing_results {
		friend arguments;

	public:
		[[nodiscard]] inline operator bool() const noexcept;
		inline std::string error_string() const;
		inline std::string warning_string() const;
		inline std::string what() const { return error_string(); }
		inline std::string verbose() const { return error_string() + warning_string(); }

		inline std::vector<std::string> const &errors() const noexcept;
		inline std::vector<std::string> const &warnings() const noexcept;

		inline std::optional<detail::result> get(std::string_view sv) const;
		inline std::optional<detail::result> get(std::string s) const {
			return get(std::string_view(s));
		}
		inline std::optional<detail::result> get(char const *s) const {
			return get(std::string_view(s));
		}

		inline detail::result operator[](std::string_view sv) const;
		inline detail::result operator[](std::string s) const {
			return operator[](std::string_view(s));
		}
		inline detail::result operator[](char const *s) const {
			return operator[](std::string_view(s));
		}

	protected:
		inline explicit parsing_results(arguments const &arguments) noexcept
			: arguments_(arguments) {}

	protected:
		arguments const &arguments_;

	private:
	};

	class arguments {
		friend parsing_results;

	public:
		template <typename... Ts>
		inline explicit arguments(Ts &&... as) : was_parsed(false) {
			add(std::move(as)...);
		}

		arguments(arguments const &) = delete;
		arguments(arguments &&)		 = default;

		inline void add(std::unique_ptr<detail::argument> &&a) {
			if (was_parsed)
				errors_.emplace_back("A new argument was added after parsing."
									 "Undefined behaviour.\n");
			arguments_.emplace_back(std::move(a));
		}
		template <typename... Ts>
		inline void add(std::unique_ptr<detail::argument> &&a, Ts &&... as) {
			add(std::move(a));
			add(std::move(as)...);
		}

		inline void add(std::unique_ptr<detail::positional> &&a) {
			if (was_parsed)
				errors_.emplace_back("A new argument was added after parsing."
									 "Undefined behaviour.\n");
			positional_.emplace_back(std::move(a));
		}
		template <typename... Ts>
		inline void add(std::unique_ptr<detail::positional> &&a, Ts &&... as) {
			add(std::move(a));
			add(std::move(as)...);
		}

		inline parsing_results parse(int argc, char **argv) {
			parsing_results out(*this);
			if (was_parsed)
				errors_.emplace_back("Argument parsing is requested more than once."
									 "Undefined behaviour.\n");

			auto current_positional	 = positional_.begin();
			detail::argument *active = nullptr;

			for (int i = 1; i < argc; i++) {
				std::string_view current = argv[i];
				if (current.size())
					if (is_argument(current))
						if (auto pos = current.find_first_of('='); pos != std::string::npos) {
							parse_argument(current.substr(1, pos - 1), active);
							parse_value(current.substr(pos + 1), active,
										std::forward_as_tuple(current_positional,
															  current_positional ==
																  positional_.end()));
						} else
							parse_argument(current.substr(1), active);
					else
						parse_value(current, active,
									std::forward_as_tuple(current_positional,
														  current_positional == positional_.end()));
				else
					warnings_.emplace_back("Empty argument (with index = " + std::to_string(i) +
										   ") was encountered.\n");
			}

			if (active && !active->try_call()) {
				if (active->is_value_callable()) {
					errors_.emplace_back("Argument <-" + active->name +
										 "> expects a value, but none was given instead.\n");
				} else
					errors_.emplace_back("Argument <-" + active->name + "> is unexpected.\n");
			}
			for (; current_positional != positional_.end(); ++current_positional)
				if (!*current_positional)
					warnings_.emplace_back("No value was provided for positional argument <" +
										   (*current_positional)->name + ">.\n");

			was_parsed = true;
			return out;
		}

	protected:
		inline detail::argument *find(std::string_view const &name) const {
			auto predicate = [&name](auto const &value) {
#if defined(_MSC_VER) || (defined(__GNUG__) && !defined(__clang__))
				return value->aliases.contains(std::string(name));
#else
				// temporary solution. Should be removed after std::set<T>::contains is implemented
				// by the clang (and others).
				return value->aliases.find(std::string(name)) != value->aliases.end();
#endif
			};

			if (auto argument_it = std::find_if(arguments_.begin(), arguments_.end(), predicate);
				argument_it != arguments_.end()) {
				return argument_it->get();
			} else if (auto positional_it = std::find_if(positional_.begin(), positional_.end(),
														 predicate);
					   positional_it != positional_.end()) {
				return positional_it->get();
			} else
				return nullptr;
		}

		inline void parse_argument(std::string_view const &sv, detail::argument *&active) {
			if (auto current = find(sv)) {
				if (active && !active->try_call()) {
					if (active->is_value_callable()) {
						errors_.emplace_back("Argument <-" + active->name +
											 "> expects a value, but \"-" + std::string(sv) +
											 "\" was given instead.\n");
					} else
						errors_.emplace_back("Argument <-" + active->name + "> is unexpected.\n");
				}
				if (current->is_value_callable())
					active = current;
				else if (!current->try_call())
					errors_.emplace_back("Argument <-" + current->name + "> is unexpected.\n");
			} else
				errors_.emplace_back("Unknown argument: <-" + std::string(sv) + ">.\n");
		};

		template <typename Iterator>
		inline void parse_value(std::string_view const &sv, detail::argument *&active,
								Iterator &&current_positional) {
			if (sv.size() >= 2 && sv.front() == '"' && sv.back() == '"')
				return parse_value(sv.substr(1, sv.size() - 2), active, current_positional);

			if (active) {
				if (!active->try_call(sv))
					errors_.emplace_back("Argument <-" + active->name +
										 "> doesn't expect a value, but \"" + std::string(sv) +
										 "\" was given.\n");
				active = nullptr;
			} else if (auto &&[iterator, is_last] = current_positional; !is_last)
				if ((*iterator)->try_call(sv))
					++iterator;
				else
					errors_.emplace_back("Argument <-" + (*iterator)->name +
										 "> doesn't expect a value, but \"" + std::string(sv) +
										 "\" was given.\n");
			else
				warnings_.emplace_back("Value \"" + std::string(sv) + "\" was ignored.\n");
		}

		inline bool is_argument(std::string_view const &current) {
			return (current.size() > 1) && (current[0] == '-') &&
				   ((current[1] > 'A' && current[1] < 'Z') ||
					(current[1] > 'a' && current[1] < 'z') || current[1] == '-' ||
					current[1] == '_');
		}

	private:
		std::vector<std::unique_ptr<detail::argument>> arguments_;
		std::vector<std::unique_ptr<detail::argument>> positional_;
		std::vector<std::string> errors_;
		std::vector<std::string> warnings_;
		bool was_parsed;
	};

	inline parsing_results parse(arguments &args, int argc, char **argv) {
		return args.parse(argc, argv);
	}

	class access_error : public std::invalid_argument {
		using std::invalid_argument::invalid_argument;
	};
}  // namespace ap

inline ap::parsing_results::operator bool() const noexcept {
	return arguments_.was_parsed && arguments_.errors_.empty();
}
inline std::string ap::parsing_results::error_string() const {
	return std::accumulate(errors().begin(), errors().end(), std::string{});
}
inline std::string ap::parsing_results::warning_string() const {
	return std::accumulate(warnings().begin(), warnings().end(), std::string{});
}
inline std::vector<std::string> const &ap::parsing_results::errors() const noexcept {
	return arguments_.errors_;
}
inline std::vector<std::string> const &ap::parsing_results::warnings() const noexcept {
	return arguments_.warnings_;
}
inline std::optional<ap::detail::result> ap::parsing_results::get(std::string_view sv) const {
	if (auto argument = arguments_.find(sv); argument)
		return detail::result(*argument);
	else
		return std::nullopt;
}
inline ap::detail::result ap::parsing_results::operator[](std::string_view sv) const {
	if (auto out = get(sv); out)
		return *out;
	else
		throw access_error("Unknown argument: <-" + std::string(sv) + ">.");
}

namespace ap::detail {
	template <typename T>
	inline std::optional<T> to_type(std::string_view const &value) noexcept {
		T temp;
#ifndef _MSC_VER
		// temporary solution. Should be removed after FP charconv is implemented by the compilers.
		if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>) {
#endif

			auto &&[ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), temp);
			if (ec == std::errc() && ptr == value.data() + value.size())
				return temp;
			else
				return std::nullopt;

#ifndef _MSC_VER
		} else {
			if (value.empty()) return std::nullopt;
			std::istringstream s(value.data());
			s >> temp;
			if (s.eof())
				return temp;
			else
				return std::nullopt;
		}
#endif
	}

	namespace type_traits {
		namespace detail {
			template <typename T>
			class only_convertible_to {
			public:
				template <typename S, typename = std::enable_if_t<std::is_same_v<T, S>>>
				operator S() const {
					return S{};
				}
			};
		}  // namespace detail

		template <typename T, typename... Args>
		struct is_no_conversion_constructible
			: std::is_constructible<T, detail::only_convertible_to<Args>...> {};

		template <typename T, typename... Args>
		constexpr auto is_no_conversion_constructible_v =
			is_no_conversion_constructible<T, Args...>::value;
	}  // namespace type_traits
}  // namespace ap::detail

#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable : 4244)
	#pragma warning(disable : 4267)
#elif defined(__clang__)
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wunused-variable"
	#pragma clang diagnostic ignored "-Wunused-lambda-capture"
#elif defined(__GNUG__)
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

template <typename T>
inline std::optional<T> ap::detail::result::get() const {
	if constexpr (std::is_same_v<T, bool>)
		return (bool) argument_;
	else {
		std::optional<T> out = std::nullopt;

		auto valueless = [/* &out */]() { /*out = std::nullopt; */ };

		auto value = [&out, this]([[maybe_unused]] std::string_view const &value) {
			if constexpr (type_traits::is_no_conversion_constructible_v<T, std::string_view>)
				out = T{value};
			else if constexpr (type_traits::is_no_conversion_constructible_v<T, std::string>)
				out = T{std::string(value)};
			else if constexpr (type_traits::is_no_conversion_constructible_v<T, char const *>)
				out = T{value.data()};
			else if constexpr (std::is_arithmetic_v<T> && !std::is_same_v<T, bool>) {
				if (auto converted = detail::to_type<T>(value); converted)
					out = static_cast<T>(*converted);
			} else if constexpr (type_traits::is_no_conversion_constructible_v<T, signed char>) {
				if (auto converted = detail::to_type<signed char>(value); converted)
					out = static_cast<T>(*converted);
			} else if constexpr (type_traits::is_no_conversion_constructible_v<T, unsigned char>) {
				if (auto converted = detail::to_type<unsigned char>(value); converted)
					out = static_cast<T>(*converted);
			} else if constexpr (type_traits::is_no_conversion_constructible_v<T, signed short>) {
				if (auto converted = detail::to_type<signed short>(value); converted)
					out = static_cast<T>(*converted);
			} else if constexpr (type_traits::is_no_conversion_constructible_v<T, unsigned short>) {
				if (auto converted = detail::to_type<unsigned short>(value); converted)
					out = static_cast<T>(*converted);
			} else if constexpr (type_traits::is_no_conversion_constructible_v<T, signed int>) {
				if (auto converted = detail::to_type<signed int>(value); converted)
					out = static_cast<T>(*converted);
			} else if constexpr (type_traits::is_no_conversion_constructible_v<T, unsigned int>) {
				if (auto converted = detail::to_type<unsigned int>(value); converted)
					out = static_cast<T>(*converted);
			} else if constexpr (type_traits::is_no_conversion_constructible_v<T, signed long>) {
				if (auto converted = detail::to_type<signed long>(value); converted)
					out = static_cast<T>(*converted);
			} else if constexpr (type_traits::is_no_conversion_constructible_v<T, unsigned long>) {
				if (auto converted = detail::to_type<unsigned long>(value); converted)
					out = static_cast<T>(*converted);
			} else if constexpr (type_traits::is_no_conversion_constructible_v<T, signed long long>) {
				if (auto converted = detail::to_type<signed long long>(value); converted)
					out = static_cast<T>(*converted);
			} else if constexpr (type_traits::is_no_conversion_constructible_v<
									 T, unsigned long long>) {
				if (auto converted = detail::to_type<unsigned long long>(value); converted)
					out = static_cast<T>(*converted);
			} else if constexpr (type_traits::is_no_conversion_constructible_v<T, float>) {
				if (auto converted = detail::to_type<float>(value); converted)
					out = static_cast<T>(*converted);
			} else if constexpr (type_traits::is_no_conversion_constructible_v<T, double>) {
				if (auto converted = detail::to_type<double>(value); converted)
					out = static_cast<T>(*converted);
			} else if constexpr (type_traits::is_no_conversion_constructible_v<T, long double>) {
				if (auto converted = detail::to_type<long double>(value); converted)
					out = static_cast<T>(*converted);
			} else if constexpr (type_traits::is_no_conversion_constructible_v<T, bool>)
				out = static_cast<T>((bool) argument_);
			// else
			// out = std::nullopt;
		};

		auto counter = [&out, this]([[maybe_unused]] size_t value) {
			if constexpr (std::is_constructible_v<T, size_t> || std::is_convertible_v<size_t, T>)
				out = (T) + value;
			else if constexpr (std::is_constructible_v<T, bool> || std::is_convertible_v<bool, T>)
				out = (T)(bool) argument_;
			// else
			// out = std::nullopt;
		};

		if (argument_.visitor_callable)
			(*argument_.visitor_callable)(argument_visitor(valueless, value, counter));
		// else
		// out = std::nullopt;
		return out;
	}
}
#if defined(_MSC_VER)
	#pragma warning(pop)
#elif defined(__clang__)
	#pragma clang diagnostic pop
#elif defined(__GNUG__)
	#pragma GCC diagnostic pop
#endif

template <typename T>
inline T ap::detail::result::as() const {
	if (auto out = get<T>(); out)
		return *out;
	else
		throw access_error("Argument <-" + std::string(argument_.name) +
						   "> cannot be converted to the requested type.");
}
template <typename T>
inline T ap::detail::result::value_or(T value) const {
	if (auto out = get<T>(); out)
		return *out;
	else
		return value;
}
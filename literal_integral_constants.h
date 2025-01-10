#ifndef LITERAL_INTEGRAL_CONSTANTS_H
#define LITERAL_INTEGRAL_CONSTANTS_H

//____________________________________________________________________________
// 
//      user defined literal suffixes for std::integral_constant
//____________________________________________________________________________

namespace literal_integral_constants{


//literal_char_args_parser
template<class double_type = double>
struct literal_char_args_parser
{
private:


	//_____________________________________________________________________________________________
	struct integer_overflow
	{
		constexpr integer_overflow() {}

		template <class T>
		constexpr integer_overflow(T n) {}

		template <class T>
		constexpr auto operator + (T const& e) const {
			return *this;
		}
		template <class T>
		constexpr auto operator * (T const& e) const {
			return *this;
		}
		template <class T>
		constexpr auto operator / (T const& e) const {
			return *this;
		}
		template <class T>
		constexpr auto operator - (T const& e) const {
			return *this;
		}
	};

	//_____________________________________________________________________________________________
	template<int N>
	static inline constexpr double_type pos_power_of_ten() {
		return  10 * pos_power_of_ten<N - 1>();
	}
	template<>
	static inline constexpr double_type pos_power_of_ten<0>() {
		return 1;
	}

	//_____________________________________________________________________________________________
	static constexpr int char2num(char ch)
	{
		return ((ch - '0') < 10 && (ch - '0') > -1) ? (ch - '0')
			: ((ch - 'A') < 7 && (ch - 'A') > -1) ? (10 + (ch - 'A'))
			: ((ch - 'a') < 7 && (ch - 'a') > -1) ? (10 + (ch - 'a'))
			: throw std::overflow_error("bad character in literal");
	}

	template<int NBASE>
	struct number_base
	{
		//_____________________________________________________________________________________________

		template<double_type V, int VExp, char Arg1, char ... Args>
		struct do_neg_exponent
		{
			static constexpr auto res =
				do_neg_exponent<
				V, NBASE* VExp + char2num(Arg1), Args...>::res;
		};
		template<double_type V, int VExp, char Arg1>
		struct do_neg_exponent<V, VExp, Arg1>//terminating
		{
			static constexpr auto res =
				double_type(V) / pos_power_of_ten <
				NBASE* VExp + char2num(Arg1)>();
		};

		//_____________________________________________________________________________________________

		template<double_type V, int VExp, char Arg1, char ... Args>
		struct do_exponent
		{
			static constexpr auto res =
				do_exponent<
				V, NBASE* VExp + char2num(Arg1), Args...>::res;
		};
		template<double_type V, int VExp, char Arg1>
		struct do_exponent<V, VExp, Arg1> //terminating
		{
			static constexpr auto res =
				double_type(V) * pos_power_of_ten <
				NBASE* VExp + char2num(Arg1)>();
		};
		template<double_type V, int VExp, char ... Args>
		struct do_exponent<V, VExp, '-', Args...>
		{
			static constexpr auto res =
				do_neg_exponent<V, VExp, Args...>::res;
		};
		template<double_type V, int VExp, char ... Args>
		struct do_exponent<V, VExp, '+', Args...>
		{
			static constexpr auto res = do_exponent<V, VExp, Args...>::res;
		};

		//_____________________________________________________________________________________________




		template<double_type V, auto D, char Arg1, char ... Args>
		struct double_from_args_past_point
		{
			static constexpr auto res =
				double_from_args_past_point<
				V + (double_type)char2num(Arg1) / D, D* NBASE, Args...
				>::res;
		};
		
		template<double_type V, auto D, char Arg1>
		struct double_from_args_past_point<V, D, Arg1>
		{
			static constexpr auto res = V + (double_type)char2num(Arg1) / D;
		};

		//_______________________________________________________________
		//	Found exponent - so read remaing chars with do_exponent
		template<double_type V, auto D, char ... Args>
		struct double_from_args_past_point<V, D, 'e', Args...>
		{
			static constexpr auto res = do_exponent<V, 0, Args...>::res;
		};
		template<double_type V, auto D, char ... Args>
		struct double_from_args_past_point<V, D, 'E', Args...>
		{
			static constexpr auto res = do_exponent<V, 0, Args...>::res;
		};




		//_____________________________________________________________________________________________
		template<class T, T V, class WiderT = double_type>
		struct next_type_needed
		{
			static constexpr double_type next_val = (double_type)NBASE * (double_type)V;
			using int_type = std::conditional_t <
				(next_val > INT_MAX), long long, T>;

			using type = std::conditional_t <
				(next_val > LLONG_MAX), WiderT, int_type>;		};

		template<class T, T V, class WiderT = double_type>
		using next_type_needed_t = next_type_needed<T, V, WiderT>::type;


		//____________________________________________________________________________
		// 
		//				number_from_args<T, V, char ... Args>::res
		// 
		//  Reads digits forming a number until it encounters '.' an exponent or the end
		//	Each pass multiplies V, the result so far, by the number base and then
		//  adds the value it has just read.
		//____________________________________________________________________________

		//General case_____________________________________
		template<class T, T V, char Arg1, char ... Args>
		struct number_from_args //General case
		{
			//This will widen the type T for the next value when neccesary
			using type = next_type_needed_t<T, V>;

			static constexpr auto res =
				number_from_args<
				type, (type)NBASE* (type)V + (type)char2num(Arg1),
				Args...>::res;
		};
		//Reading the last char__________________________________
		template<class T, T V, char Arg1>
		struct number_from_args<T, V, Arg1>
		{
			using type = next_type_needed_t<T, V, integer_overflow>;

			static constexpr auto res =
				(type)NBASE * (type)V + (type)char2num(Arg1);
		};
		//Found decimal point_______________________________________
		template<class T, T V, char ... Args>
		struct number_from_args<T, V, '.', Args...>
		{
			static constexpr auto res =
				double_from_args_past_point<(double_type)V, (double_type)NBASE, Args...>::res;
		};
		template<class T, T V>
		struct number_from_args<T, V, '.'> //Found decimal point as last char
		{
			static constexpr auto res = (double_type)V;
		};
		//Found expononent________________________________________________
		template<class T, T V, char ... Args>
		struct number_from_args<T, V, 'e', Args...>
		{
			static constexpr auto res =
				do_exponent<(double_type)V, 0, Args...>::res;
		};
		template<class T, T V, char ... Args>
		struct number_from_args<T, V, 'E', Args...>
		{
			static constexpr auto res =
				do_exponent<(double_type)V, 0, Args...>::res;
		};
	};


	

	//____________________________________________________________________________
	// 
	//					 read_base_and_parse<Args...>::res
	//	Checks if first two chars signify Hex or Binary before selecting
	//	the number base and passing the remaing chars to number_from_args<Args...>
	//____________________________________________________________________________

	//Default Base 10____________________________________________
	template<char ... Args>
	struct read_base_and_parse
	{
		static constexpr auto res =
			number_base<10>::template number_from_args<int, 0, Args...>::res;
	};
	/*template<char ... Args>
	struct read_base_and_parse<'-' , Args...>
	{
		static constexpr auto res =
			-number_base<10>::template number_from_args<int, 0, Args...>::res;
	};*/

	// Ox - Hexadecimal_______________________________________________
	template<char ... Args>
	struct read_base_and_parse<'0', 'x', Args...>
	{
		static constexpr auto res =
			number_base<16>::template number_from_args<int, 0, Args...>::res;
	};
	template<char ... Args>
	struct read_base_and_parse<'0', 'X', Args...>
	{
		static constexpr auto res =
			number_base<16>::template number_from_args<int, 0, Args...>::res;
	};

	// Ob - Binary___________________________________________________________
	template<char ... Args>
	struct read_base_and_parse<'0', 'b', Args...>
	{
		static constexpr auto res =
			number_base<2>::template number_from_args<int, 0, Args...>::res;
	};
	template<char ... Args>
	struct read_base_and_parse<'0', 'B', Args...>
	{
		static constexpr auto res =
			number_base<2>::template number_from_args<int, 0, Args...>::res;
	};

	
public:
	//____________________________________________________________________________
	// 
	//					auto check_ok(value)
	//		Checks if an integer_overflow has been returned. 
	//		This is the place to fail the compilation if it has.
	//____________________________________________________________________________
	template<class T>
	static constexpr auto check_ok(T value)
	{
		return true;
	}
	static auto check_ok(integer_overflow value)
	{
		//throw std::overflow_error("integer literal is too long");
		return false;
	}
	//____________________________________________________________________________
	// 
	//					 auto get_number<Args...>()
	// The public function that reads the char ... Args and returns a number
	//____________________________________________________________________________
	template<char ... Args>
	static constexpr auto get_number()
	{
		return read_base_and_parse<Args...>::res;
	}
};

//____________________________________________________________________________
// 
//      user defined literal suffixes for value type
//____________________________________________________________________________

//General case - no type suffixes
template<char ... Args>
inline constexpr auto operator "" __() {
	constexpr auto res = literal_char_args_parser<>::get_number<Args...>();
	return (literal_char_args_parser<>::check_ok(res)) ? 
		std::integral_constant <std::remove_const_t<decltype(res)>, res>()
		: throw std::overflow_error("integer literal is too long");
}

// unsigned suffix - lower case and upper case
template<char ... Args>
inline constexpr auto operator "" __u() {

	constexpr auto num = literal_char_args_parser<>::get_number<Args...>();
	return (literal_char_args_parser<>::check_ok(num)) ? (std::is_integral_v<decltype(num)>) ?
		std::integral_constant < std::remove_const_t<std::make_unsigned_t<decltype(num)>>, num>()
		: throw std::overflow_error("floating point cannot be unsigned")
	: throw std::overflow_error("integer literal is too long");
}
template<char ... Args>
inline constexpr auto operator "" __U() {
	return operator ""__u<Args...>();
}

// float suffix - lower case and upper case
template<char ... Args>
inline constexpr auto operator "" __f() {

	constexpr auto num = literal_char_args_parser<>::get_number<Args...>();
	return (!std::is_integral_v<decltype(num)>) ?
		std::integral_constant <float, (float)num>()
		: throw std::overflow_error("integral type cannot be float");
}
template<char ... Args>
inline constexpr auto operator "" __F() {
	return operator ""__f<Args...>();
}
// long long int suffix - lower case and upper case
template<char ... Args>
inline constexpr auto operator "" __ll() {

	constexpr auto num = literal_char_args_parser<>::get_number<Args...>();
	return (std::is_integral_v<decltype(num)>) ?
		std::integral_constant <long long, num>()
		: throw std::overflow_error("floating point cannot be long long");
}
template<char ... Args>
inline constexpr auto operator "" __LL() {
	return operator ""__ll<Args...>();
}

// unsigned long long int suffix - lower case and upper case and reversed
template<char ... Args>
inline constexpr auto operator "" __ull() {

	constexpr auto num = literal_char_args_parser<>::get_number<Args...>();
	return (std::is_integral_v<decltype(num)>) ?
		std::integral_constant <unsigned long long, num>()
		: throw std::overflow_error("floating point cannot be unsigned");
}
template<char ... Args>
inline constexpr auto operator "" __ULL() {
	return operator ""__ull<Args...>();
}
template<char ... Args>
inline constexpr auto operator "" __llu() {
	return operator ""__ull<Args...>();
}
template<char ... Args>
inline constexpr auto operator "" __LLU() {
	return operator ""__ull<Args...>();
}
// long double suffix - lower case and upper case
template<char ... Args>
inline constexpr auto operator "" __l() {

	constexpr auto num = literal_char_args_parser<long double>::get_number<Args...>();
	return (!std::is_integral_v<decltype(num)>) ?
		std::integral_constant < std::remove_const_t<decltype(num)>, num>()
		: throw std::overflow_error("integral type cannot be long double");
}
template<char ... Args>
inline constexpr auto operator "" __L() {
	return operator ""__l<Args...>();
}

//Unary minus operator for std::integral_constant to embrace negative literals
template< class T, T v >
constexpr auto operator-(std::integral_constant<T, v>const& in) {
	return std::integral_constant<T, -v>();
}

} //end namespace literal_integral_constants

#endif //PTR_TO_UNIQUE_H
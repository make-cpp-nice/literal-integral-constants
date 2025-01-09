# literal-integral-constants
Converts literal constants into std::integral_constant using a double underscore suffix, e.g. 4__

Declaring compile time constants as  std::integral_constant makes them more amenable to compile time optimisations. Functions can be written to recognise that a  std::integral_constant parameter is a compile time constant and perform compile time optimisations with its type defined value.

For instance here is a function for bounds checked access to a plain array

///c++
template<class T,size_t N>
auto in_array_at(T(&array)[N], size_t index)
{
	if (index < N)
		return  array[index];
	throw std::out_of_range("bad index");
}
///

It takes an integer index and checks it at run-time, each time the function is called.

In the case that the index being passed is a compile time constant it should be possible to do the bounds check during compilation instead, because all of the information is there. However the only way the function can know it is being passed a compile time constant is if that information is indicated in the type being passed and the only way it can use it is if its value is also encoded in its type.  std::integral_constant<type, value> is a type that does just that.

So we can add an overload that takes a std::integral_constant and does the bounds check during compilation

template<class T, size_t N, class IntType, IntType I>
inline auto in_array_at(T(&array)[N], std::integral_constant<const IntType, I> index)
{
	using check_in_range = std::enable_if_t < (size_t)I < N >;
	return array[I];
}

To invoke it we have to pass our constant index as a  std::integral_constant as follows

in_array_at(AnArray, std::integral_constant<int, 4>());

This a very verbose way of saying that we are passing 4 as a compile time constant.

Literal integral constants, as provided here, allow you to write the same thing as follows:

in_array_at(AnArray, 4__); //will check index during compilation.
 

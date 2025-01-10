# literal-integral-constants
Converts literal constants into ```std::integral_constant``` using a double underscore suffix, e.g. ```4__```

Declaring compile time constants as ```std::integral_constant``` makes them more amenable to compile time optimisations. Functions can be written to recognise that a ```std::integral_constant``` parameter is a compile time constant and perform compile time optimisations with its type defined value.

For instance here is a function for bounds checked access to a plain array

```c++
template<class T,size_t N>
auto in_array_at(T(&array)[N], size_t index)
{
	if (index < N)
		return  array[index];
	throw std::out_of_range("bad index");
}
```
It takes an integer index and checks it at run-time, each time the function is called.

In the case that the index being passed is a compile time constant it should be possible to do the bounds check during compilation instead, because all of the information is there. However the only way the function can know it is being passed a compile time constant is if that information is indicated in the type being passed and the only way it can use it is if its value is also encoded in its type.  ```std::integral_constant<type, value>``` is a type that does just that.

So we can add an overload that takes a ```std::integral_constant``` and does the bounds check during compilation
```C++
template<class T, size_t N, class IntType, IntType I>
inline auto in_array_at(T(&array)[N], std::integral_constant<const IntType, I> index)
{
	using check_in_range = std::enable_if_t < (size_t)I < N >;
	return array[I];
}
```
To invoke it we have to pass our constant index as a  ```std::integral_constant``` as follows
```C++
in_array_at(AnArray, std::integral_constant<int, 4>());
```
This a very verbose way of saying that we are passing ```4``` as a compile time constant.

Literal integral constants, as provided here, allow you to write the same thing as follows:
```C++
in_array_at(AnArray, 4__); //will check index during compilation.
``` 
# Specification
To use, download [literal_integral_constants.h](https://github.com/make-cpp-nice/literal-integral-constants/blob/main/literal_integral_constants.h) and add
```C++
#include  "literal_integral_constants.h" 
using namespace literal_integral_constants;
```
A literal constant with two trailing underscores will be read as a ```std::integral_constant```
e.g. 
+ ```4__``` will be read as ```std::integral_constant<int, 4>()```
+ ```1234567891234__```  will be read as ```std::integral_constant<long long, 4>()```

If the literal contains a decimal point it will be interpreted as floating point 
+ ```3.5__``` will be read as ```std::integral_constant<double, 3.5>()```

Type forcing suffixes are applied after the trailing underscores
e.g.
+ ```4__u``` will be read as ```std::integral_constant<unsigned int, 4>()```
+ ```1234567891234__u```  will be read as ```std::integral_constant<unsigned long long, 4>()```
+ ```4__ll``` will be read as ```std::integral_constant<long long, 4>()```
+ ```4__ull``` will be read as ```std::integral_constant<unsigned long long, 4>()```
+ ```3.5__l``` will be read as ```std::integral_constant<long double, 3.5>()```
+ ```3.5__f``` will be read as ```std::integral_constant<float, 3.5>()```

Number base prefixes are applied at the beginning of the literal constant
e.g. 
+ ```0b101__``` will be read as ```std::integral_constant<int, 5>()```
+ ```0xFF__``` will be read as ```std::integral_constant<int, 255>()```

A free function provides a unary minus operator for ```std::integral_constant``` so that a negative prefix produces a  ```std::integral_constant``` with a negative value.
e.g.
+ ```-4__``` will be read as ```std::integral_constant<int, -4>()```

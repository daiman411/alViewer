#ifndef _SIMPLE_TYPETRAITS_H_
#define _SIMPLE_TYPETRAITS_H_

namespace simple_typetraits
{
    // is integral
    template<typename T> struct is_integral{ enum { value = false }; };
    template<> struct is_integral<char>{ enum { value = true }; };
    template<> struct is_integral<short>{ enum { value = true }; };
    template<> struct is_integral<int>{ enum { value = true }; };
    template<> struct is_integral<long>{ enum { value = true }; };
    template<> struct is_integral<unsigned char>{ enum { value = true }; };
    template<> struct is_integral<unsigned short>{ enum { value = true }; };
    template<> struct is_integral<unsigned int>{ enum { value = true }; };
    template<> struct is_integral<unsigned long>{ enum { value = true }; };
    template<typename T> struct is_integral<const T>{ enum { value = is_integral<T>::value }; };

    // is float
    template<typename T> struct is_float{ enum { value = false }; };
    template<> struct is_float<float>{ enum { value = true }; };
    template<> struct is_float<double>{ enum { value = true }; };
    template<typename T> struct is_float<const T>{ enum { value = is_float<T>::value }; };

    // is arithmetic
    template<typename T> struct is_arithmetic{ enum { value = is_integral<T>::value || is_float<T>::value }; };

    // is pointer
    template<typename T> struct is_pointer{ enum { value = false }; };
    template<typename T> struct is_pointer<T*>{ enum { value = true }; };

    // is reference
    template<typename T> struct is_reference{ enum { value = false }; };
    template<typename T> struct is_reference<T&>{ enum { value = true }; };

    // add reference
    template<typename T> struct add_reference{ typedef T& type; };
    template<typename T> struct add_reference<T&>{ typedef T& type; };

    // add const
    template<typename T> struct add_const{ typedef const T type; };
    template<typename T> struct add_const<const T>{ typedef const T type; };

    // is same
    template<typename T, typename U> struct is_same{ enum { value = false }; };
    template<typename T> struct is_same<T, T>{ enum { value = true }; };

    // select type
    template<bool C, typename T, typename F> struct select_type{ typedef typename T::type type; };
    template<typename T, typename F> struct select_type<false, T, F>{ typedef typename F::type type; };

    // identiry
    template<typename T> struct identity{ typedef T type; };

    template<typename T> struct const_reference{ typedef const T& type; };

    // param type
    template<typename T> struct param_type : public select_type<
        is_arithmetic<T>::value || is_pointer<T>::value || is_reference<T>::value,
        identity<T>, const_reference<T>
    >{ };    

} // namespace simple_typetraits

#endif // _SIMPLE_TYPETRAITS_H_
#include <iostream>
#include <cstdint>
#include <string>
#include <vector>
#include <list>
#include <tuple>
#include <type_traits>
#include <utility>
#include <cstddef>
#include <bit>
#include <concepts>

//#define CONCEPTS //Осовременивание с использованием концептов - Я вкурсе что никогда не надо использовать макросы)

// Реализация для целочисленных типов
#ifdef CONCEPTS
template<std::integral T>
#else
template<typename T, typename std::enable_if_t<std::is_integral_v<T>, int> = 0>
#endif
void print_ip(const T& value)
{
    const auto* bytes = std::bit_cast<const std::byte*>(&value);
    for (int i = sizeof(T) - 1; i >= 0; --i) {
        std::cout << std::to_integer<int>(bytes[i]);
        if (i > 0)
            std::cout << ".";
    }
    std::cout << std::endl;
}

//Реализация для строк
#ifdef CONCEPTS
template<std::same_as<std::string> T>
#else
template<typename T, typename std::enable_if_t<std::is_same_v<T, std::string>, int> = 0>
#endif
void print_ip(const T& value)
{
    std::cout << value << std::endl;
}

// Реализация для контейнеров (std::vector и std::list)
#ifdef CONCEPTS
template<typename T>
concept IpContainer = requires(T container)
{
    typename T::value_type; //Контейнер должен иметь value_type
    { container.begin() } -> std::input_iterator; //Контейнер должен поддерживать итераторы
    { container.end() } -> std::input_iterator;
} && !std::is_same_v<T, std::string>;

template<IpContainer T>
#else
template<typename T, typename std::enable_if_t<
    std::is_same_v<T, std::vector<typename T::value_type>> ||
    std::is_same_v<T, std::list<typename T::value_type>>, int> = 0>
#endif
void print_ip(const T & container)
{
    for (auto it = container.begin(); it != container.end(); ++it) {
        std::cout << *it;
        if (std::next(it) != container.end())
            std::cout << ".";
    }
    std::cout << std::endl;
}

// Универсальная реализация для кортежей произвольной длины
#ifdef CONCEPTS
template <typename... Args>
concept AllSame = (std::same_as<Args, std::tuple_element_t<0, std::tuple<Args...>>> && ...);

template <typename... Args>
    requires AllSame<Args...>
#else
template <typename... Args>
inline constexpr auto are_all_same_v = std::conjunction_v<std::is_same<std::tuple_element_t<0, std::tuple<Args...>>, Args>...>;

template <typename... Args, typename = std::enable_if_t<are_all_same_v<Args...>>>
#endif
void print_ip(const std::tuple<Args...>& tuple)
{
    std::apply([](const auto&... args) {
        size_t n = 0;
        ((std::cout << args << (++n == sizeof...(args) ? "" : ".")), ...);
        }, tuple);
    std::cout << std::endl;
}

int main()
{
    print_ip(int8_t{ -1 }); // 255
    print_ip(int16_t{ 0 }); // 0.0
    print_ip(int32_t{ 2130706433 }); // 127.0.0.1
    print_ip(int64_t{ 8875824491850138409 });// 123.45.67.89.101.112.131.41
    print_ip(std::string{ "Hello, World!" }); // Hello, World!
    print_ip(std::vector<int>{100, 200, 300, 400}); // 100.200.300.400
    print_ip(std::list<short>{400, 300, 200, 100}); // 400.300.200.100
    print_ip(std::make_tuple(123, 456, 789, 0)); // 123.456.789.0

    //Дополнительные варианты отработки ошибок
    //print_ip(std::deque<short>{400, 300, 200, 100}); // 400.300.200.100
    //print_ip(std::make_tuple(123, 456, 789, 0, 11, 22)); // 123.456.789.0.11.22
    //print_ip(std::make_tuple(123, 456, 789, 0, std::string(""), 8)); //ERROR
    //print_ip(std::make_tuple(123, "", 789, 0, 1, 8)); //ERROR
    //print_ip(std::make_tuple("12a3", "456", "789", "0", "1", "8")); // 12a3.456.789.0.1.8
    //print_ip(std::make_tuple("123", "456", "", "0", "", 789)); //ERROR
    //print_ip(std::make_tuple("123", "456", 789, "0", "", "8")); //ERROR
}
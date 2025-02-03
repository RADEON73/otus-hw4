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

//#define CONCEPTS_ON //Осовременивание с использованием концептов;

#ifndef CONCEPTS_ON
/// Шаблонная реализация функции печати для целочисленных типов
template<typename T, typename std::enable_if_t<std::is_integral_v<T>, int> = 0>
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

/// Шаблонная реализация функции печати для строк
template<typename T, typename std::enable_if_t<std::is_same_v<T, std::string>, int> = 0>
void print_ip(const T& value)
{
    std::cout << value << std::endl;
}

/// Шаблонная реализация функции печати для контейнеров (std::vector и std::list)
template<typename T, typename std::enable_if_t<
    std::is_same_v<T, std::vector<typename T::value_type>> ||
    std::is_same_v<T, std::list<typename T::value_type>>, int> = 0>
void print_ip(const T& container)
{
    for (auto it = container.begin(); it != container.end(); ++it) {
        std::cout << *it;
        if (std::next(it) != container.end())
            std::cout << ".";
    }
    std::cout << std::endl;
}

/// Вспомогательная переменная для проверки идентичности типов при реализации функции печати кортежей
template <typename... Args>
inline constexpr auto are_all_same_v = std::conjunction_v<std::is_same<std::tuple_element_t<0, std::tuple<Args...>>, Args>...>;

/// Шаблонная реализация функции печати для кортежей произвольной длины
template <typename... Args, typename = std::enable_if_t<are_all_same_v<Args...>>>
void print_ip(const std::tuple<Args...>& tuple)
{
    std::apply([](const auto&... args) {
        size_t n = 0;
        ((std::cout << args << (++n == sizeof...(args) ? "" : ".")), ...);
        }, tuple);
    std::cout << std::endl;
}
#else
/// Реализация для целочисленных типов
template<std::integral T>
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

///Реализация для строк
template<std::same_as<std::string> T>
void print_ip(const T& value)
{
    std::cout << value << std::endl;
}

/// Реализация для контейнеров (std::vector и std::list)
template<typename T>
concept IpContainer = requires(T container)
{
    typename T::value_type;
    { std::begin(container) } -> std::input_iterator;
    { std::end(container) } -> std::input_iterator;
} && !std::is_same_v<T, std::string>;
template<IpContainer T>
void print_ip(const T& container)
{
    for (auto it = std::begin(container); it != std::end(container); ++it) {
        std::cout << *it;
        if (std::next(it) != std::end(container))
            std::cout << ".";
    }
    std::cout << std::endl;
}

/// Универсальная реализация для кортежей произвольной длины
template <typename... Args>
concept AllSame = (std::same_as<Args, std::tuple_element_t<0, std::tuple<Args...>>> && ...);
template <typename... Args>
    requires AllSame<Args...>
void print_ip(const std::tuple<Args...>& tuple)
{
    std::apply([](const auto&... args) {
        size_t n = 0;
        ((std::cout << args << (++n == sizeof...(args) ? "" : ".")), ...);
        }, tuple);
    std::cout << std::endl;
}
#endif

/// Главная функция программы
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
}


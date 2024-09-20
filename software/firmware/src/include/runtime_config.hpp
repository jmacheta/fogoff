#ifndef RUNTIME_CONFIG_HPP_
#define RUNTIME_CONFIG_HPP_


#include "error_codes.hpp"

#include <expected>

#include <cstdint>
#include <span>
#include <string_view>


class Configuration {
  public:
    template<typename T> [[nodiscard]] static auto get(std::string_view key) noexcept -> std::expected<T, error_code>;

    template<typename T> static auto set(std::string_view key, T const& value) noexcept -> std::expected<void, error_code>;

  private:
    [[nodiscard]] static auto do_get(uint16_t id, std::span<std::byte> data) noexcept -> std::expected<void, error_code>;
    [[nodiscard]] static auto do_set(uint16_t id, std::span<std::byte const> data) noexcept -> std::expected<void, error_code>;
};


template<typename T> auto Configuration::get(std::string_view key) noexcept -> std::expected<T, error_code> {
    auto id = static_cast<uint16_t>(std::hash<std::string_view>{}(key));
    T    value;
    auto value_span = std::span(&value, 1);

    auto succeeded = do_get(id, std::as_writable_bytes(value_span));

    if (!succeeded) {
        return std::unexpected(succeeded.error());
    }

    return value;
}


template<typename T> auto Configuration::set(std::string_view key, T const& value) noexcept -> std::expected<void, error_code> {
    auto id         = static_cast<uint16_t>(std::hash<std::string_view>{}(key));
    auto value_span = std::span(&value, 1);

    return do_set(id, std::as_bytes(value_span));
}

#endif
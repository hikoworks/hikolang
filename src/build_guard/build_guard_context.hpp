
#pragma once

#include "build_guard_types.hpp"
#include "build_guard_value.hpp"
#include "utility/semantic_version.hpp"
#include "utility/fqname.hpp"

namespace hk {

enum class build_type_t {
    debug,
    release,
};

enum class kernel_type {
    linux,
    windows,
    darwin
};

enum class platform_type {
    terminal,
    service,
    kernel,
    desktop,
    phone,
    pad,
    tv,
    watch,
    vision,
    audio,
    car,
};

enum class cpu_arch {
    x86,
    arm
}

enum class {
    target,
    host
};

class build_guard_context {
public:
    build_guard_context(build_guard_context const&) = delete;
    build_guard_context(build_guard_context&&) = delete;
    build_guard_context& operator=(build_guard_context const&) = delete;
    build_guard_context& operator=(build_guard_context&&) = delete;
    build_guard_context()

    [[nodiscard]] build_guard_value get(fqname const &name);

    void set_custom(fqname const& name, build_guard_value const& value);

private:
    [[nodiscard]] semantic_version lang_version();
    [[nodiscard]] std::vector<std::string> lang_features();
    [[nodiscard]] std::string compiler_name();
    [[nodiscard]] semantic_version compiler_version();
    [[nodiscard]] std::vector<std::string> compiler_features();
    [[nodiscard]] build_type_t build_type();
    [[nodiscard]] std::string os_name();
    [[nodiscard]] std::string os_full_name();
    [[nodiscard]] semantic_version os_version();
    [[nodiscard]] kernel_type os_kernel();
    [[nodiscard]] semantic_version os_kernel_version();
    [[nodiscard]] std::vector<platform_type> os_platform();
    [[nodiscard]] std::vector<std::string> os_features();
    [[nodiscard]] cpu_arch cpu_arch();
    [[nodiscard]] semantic_version cpu_arch_version();
    [[nodiscard]] std::endian cpu_endian();
    [[nodiscard]] long long cpu_width();
    [[nodiscard]] long long cpu_limb_width();
    [[nodiscard]] std::vector<std::string> cpu_features();
    [[nodiscard]] std::vector<std::string> cpu_optionals();
};





#### os.name : string

  | value             | description                                            |
  |:----------------- |:------------------------------------------------------ |
  | "Windows"         | The Microsoft Windows desktop Operating System         |
  | "Linux"           | The Linux desktop Operating System                     |
  | "Android"         | Operating System for Android mobile phones             |
  | "Android TV"      | Operating System for Android TVs and settop-boxes      |
  | "Wear OS"         | Operating System for Android watches                   |
  | "Meta Horizon OS" | Operating System for Android VR/AR headset             |
  | "macOS"           | The Mac OS X, OS X & macOS desktop operating system.   |
  | "iOS"             | Operating System for iPhone.                           |
  | "iPadOS"          | Operating System for iPad.                             |
  | "watchOS"         | Operating System for iWatch.                           |
  | "tvOS"            | Operating System for Apple TV.                         |
  | "audioOS"         | Operating System for HomePod.                          |
  | "visionOS"        | Operating System for Vision Pro.                       |
  | "bridgeOS"        | Apple's embedded Operating System.                     |


#### target.\*, this.\*

Using the `target.*` and `this.*` constants you can specifically check the
`build`, `os`, `cpu` constants for the target machine and this machine
seperately.

This is needed when cross compiling applications, combined with the fact that
during compilation part of the code is executed on the current (this) machine.


}

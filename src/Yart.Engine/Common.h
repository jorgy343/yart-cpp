#pragma once

#include <cmath>
#include <concepts>
#include <limits>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#define force_inline __forceinline

#ifdef USE_DOUBLE
using real = double;
#else
using real = float;
#endif

#ifdef USE_DOUBLE
constexpr real NormalBump{0.001};
constexpr real Epsilon{0.0001};
#else
constexpr real NormalBump{0.05};
constexpr real Epsilon{0.001};
#endif

template <typename T>
concept real_number = std::same_as<float, T> || std::same_as<double, T>;

template <typename T>
concept any_number = std::integral<T> || real_number<T>;

namespace vcl
{
    class Vec8f;
    class Vec4d;
}

using RealVec = std::conditional_t<std::same_as<real, float>, vcl::Vec8f, vcl::Vec4d>;
constexpr size_t RealVecElements = std::same_as<real, float> ? 8 : 4;

namespace Yart
{
    constexpr real Pi = real{3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132};
    constexpr real TwoPi = real{6.2831853071795864769252867665590057683943387987502116419498891846156328125724179972560696506842341359642961730265};
    constexpr real FourPi = real{12.566370614359172953850573533118011536788677597500423283899778369231265625144835994512139301368468271928592346053};
    constexpr real EightPi = real{25.132741228718345907701147066236023073577355195000846567799556738};

    constexpr real OneOverPi = real{0.3183098861837906715377675267450287240689192914809128974953346881177935952684530701802276055325061719121456854535};
    constexpr real OneOverTwoPi = real{0.1591549430918953357688837633725143620344596457404564487476673440588967976342265350901138027662530859560728427267};
    constexpr real OneOverFourPi = real{0.0795774715459476678844418816862571810172298228702282243738336720294483988171132675450569013831265429780364213633};
    constexpr real OneOverEightPi = real{0.0397887357729738339422209408431285905086149114351141121869168360};
}
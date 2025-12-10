#include "utility/fqname.hpp"
#include "test_utilities/paths.hpp"
#include <hikotest/hikotest.hpp>

TEST_SUITE(fqname_suite) 
{
    TEST_CASE(lexically_normal) {
        REQUIRE(hk::fqname{""}.lexically_normal() == hk::fqname{""});
        REQUIRE(hk::fqname{"a"}.lexically_normal() == hk::fqname{"a"});
        REQUIRE(hk::fqname{".a"}.lexically_normal() == hk::fqname{".a"});
        REQUIRE(hk::fqname{"..a"}.lexically_normal() == hk::fqname{"..a"});
        REQUIRE(hk::fqname{"...a"}.lexically_normal() == hk::fqname{"...a"});

        REQUIRE(hk::fqname{"a.b"}.lexically_normal() == hk::fqname{"a.b"});
        REQUIRE(hk::fqname{".a.b"}.lexically_normal() == hk::fqname{".a.b"});
        REQUIRE(hk::fqname{"..a.b"}.lexically_normal() == hk::fqname{"..a.b"});
        REQUIRE(hk::fqname{"...a.b"}.lexically_normal() == hk::fqname{"...a.b"});

        REQUIRE(hk::fqname{"a.b.c"}.lexically_normal() == hk::fqname{"a.b.c"});
        REQUIRE(hk::fqname{".a.b.c"}.lexically_normal() == hk::fqname{".a.b.c"});
        REQUIRE(hk::fqname{"..a.b.c"}.lexically_normal() == hk::fqname{"..a.b.c"});
        REQUIRE(hk::fqname{"...a.b.c"}.lexically_normal() == hk::fqname{"...a.b.c"});

        REQUIRE(hk::fqname{"a.b..c"}.lexically_normal() == hk::fqname{"a.c"});
        REQUIRE(hk::fqname{".a.b..c"}.lexically_normal() == hk::fqname{".a.c"});
        REQUIRE(hk::fqname{"..a.b..c"}.lexically_normal() == hk::fqname{"..a.c"});
        REQUIRE(hk::fqname{"...a.b..c"}.lexically_normal() == hk::fqname{"...a.c"});

        REQUIRE(hk::fqname{"a.b...c"}.lexically_normal() == hk::fqname{"c"});
        REQUIRE(hk::fqname{".a.b...c"}.lexically_normal() == hk::fqname{"..c"});
        REQUIRE(hk::fqname{"..a.b...c"}.lexically_normal() == hk::fqname{"...c"});
        REQUIRE(hk::fqname{"...a.b...c"}.lexically_normal() == hk::fqname{"....c"});
    }
};

#include "utility/fqname.hpp"
#include "test_utilities/paths.hpp"
#include <hikotest/hikotest.hpp>

TEST_SUITE(fqname_suite) 
{
    TEST_CASE(lexically_normal) {
        REQUIRE(hk::fqname{""}.lexically_normal() == "");
        REQUIRE(hk::fqname{"a"}.lexically_normal() == "a");
        REQUIRE(hk::fqname{".a"}.lexically_normal() == ".a");
        REQUIRE(hk::fqname{"..a"}.lexically_normal() == "..a");
        REQUIRE(hk::fqname{"...a"}.lexically_normal() == "...a");

        REQUIRE(hk::fqname{"a.b"}.lexically_normal() == "a.b");
        REQUIRE(hk::fqname{".a.b"}.lexically_normal() == ".a.b");
        REQUIRE(hk::fqname{"..a.b"}.lexically_normal() == "..a.b");
        REQUIRE(hk::fqname{"...a.b"}.lexically_normal() == "...a.b");

        REQUIRE(hk::fqname{"a.b.c"}.lexically_normal() == "a.b.c");
        REQUIRE(hk::fqname{".a.b.c"}.lexically_normal() == ".a.b.c");
        REQUIRE(hk::fqname{"..a.b.c"}.lexically_normal() == "..a.b.c");
        REQUIRE(hk::fqname{"...a.b.c"}.lexically_normal() == "...a.b.c");

        REQUIRE(hk::fqname{"a.b..c"}.lexically_normal() == "a.c");
        REQUIRE(hk::fqname{".a.b..c"}.lexically_normal() == ".a.c");
        REQUIRE(hk::fqname{"..a.b..c"}.lexically_normal() == "..a.c");
        REQUIRE(hk::fqname{"...a.b..c"}.lexically_normal() == "...a.c");

        REQUIRE(hk::fqname{"a.b...c"}.lexically_normal() == "c");
        REQUIRE(hk::fqname{".a.b...c"}.lexically_normal() == ".c");
        REQUIRE(hk::fqname{"..a.b...c"}.lexically_normal() == "..c");
        REQUIRE(hk::fqname{"...a.b...c"}.lexically_normal() == "...c");

        REQUIRE(hk::fqname{"a.b....c"}.lexically_normal() == "..c");
        // Absolute paths remain absolute.
        REQUIRE(hk::fqname{".a.b....c"}.lexically_normal() == ".c");
        REQUIRE(hk::fqname{"..a.b....c"}.lexically_normal() == "...c");
        REQUIRE(hk::fqname{"...a.b....c"}.lexically_normal() == "....c");

        REQUIRE(hk::fqname{"a.b.....c"}.lexically_normal() == "...c");
        // Absolute paths remain absolute.
        REQUIRE(hk::fqname{".a.b.....c"}.lexically_normal() == ".c");
        REQUIRE(hk::fqname{"..a.b.....c"}.lexically_normal() == "....c");
        REQUIRE(hk::fqname{"...a.b.....c"}.lexically_normal() == ".....c");
    }

    TEST_CASE(concat) {
        REQUIRE(hk::fqname{".x.y"} / "" == ".x.y");
        REQUIRE(hk::fqname{".x.y"} / "a" == ".x.y.a");
        REQUIRE(hk::fqname{".x.y"} / ".a" == ".a");
        REQUIRE(hk::fqname{".x.y"} / "..a" == ".x.y..a");
        REQUIRE(hk::fqname{".x.y"} / "...a" == ".x.y...a");

        REQUIRE(hk::fqname{".x.y"} / "a.b" == ".x.y.a.b");
        REQUIRE(hk::fqname{".x.y"} / ".a.b" == ".a.b");
        REQUIRE(hk::fqname{".x.y"} / "..a.b" == ".x.y..a.b");
        REQUIRE(hk::fqname{".x.y"} / "...a.b" == ".x.y...a.b");

        REQUIRE(hk::fqname{".x.y"} / "a.b.c" == ".x.y.a.b.c");
        REQUIRE(hk::fqname{".x.y"} / ".a.b.c" == ".a.b.c");
        REQUIRE(hk::fqname{".x.y"} / "..a.b.c" == ".x.y..a.b.c");
        REQUIRE(hk::fqname{".x.y"} / "...a.b.c" == ".x.y...a.b.c");

        REQUIRE(hk::fqname{".x.y"} / "a.b..c" == ".x.y.a.b..c");
        REQUIRE(hk::fqname{".x.y"} / ".a.b..c" == ".a.b..c");
        REQUIRE(hk::fqname{".x.y"} / "..a.b..c" == ".x.y..a.b..c");
        REQUIRE(hk::fqname{".x.y"} / "...a.b..c" == ".x.y...a.b..c");

        REQUIRE(hk::fqname{".x.y"} / "a.b...c" == ".x.y.a.b...c");
        REQUIRE(hk::fqname{".x.y"} / ".a.b...c" == ".a.b...c");
        REQUIRE(hk::fqname{".x.y"} / "..a.b...c" == ".x.y..a.b...c");
        REQUIRE(hk::fqname{".x.y"} / "...a.b...c" == ".x.y...a.b...c");

        REQUIRE(hk::fqname{".x.y"} / "a.b....c" == ".x.y.a.b....c");
        REQUIRE(hk::fqname{".x.y"} / ".a.b....c" == ".a.b....c");
        REQUIRE(hk::fqname{".x.y"} / "..a.b....c" == ".x.y..a.b....c");
        REQUIRE(hk::fqname{".x.y"} / "...a.b....c" == ".x.y...a.b....c");

        REQUIRE(hk::fqname{".x.y"} / "a.b.....c" == ".x.y.a.b.....c");
        REQUIRE(hk::fqname{".x.y"} / ".a.b.....c" == ".a.b.....c");
        REQUIRE(hk::fqname{".x.y"} / "..a.b.....c" == ".x.y..a.b.....c");
        REQUIRE(hk::fqname{".x.y"} / "...a.b.....c" == ".x.y...a.b.....c");
    }

    TEST_CASE(lexically_absolute) {
        REQUIRE(hk::fqname{""}.lexically_absolute(".x.y") == ".x.y");
        REQUIRE(hk::fqname{"a"}.lexically_absolute(".x.y") == ".x.y.a");
        REQUIRE(hk::fqname{".a"}.lexically_absolute(".x.y") == ".a");
        REQUIRE(hk::fqname{"..a"}.lexically_absolute(".x.y") == ".x.a");
        REQUIRE(hk::fqname{"...a"}.lexically_absolute(".x.y") == ".a");

        REQUIRE(hk::fqname{"a.b"}.lexically_absolute(".x.y") == ".x.y.a.b");
        REQUIRE(hk::fqname{".a.b"}.lexically_absolute(".x.y") == ".a.b");
        REQUIRE(hk::fqname{"..a.b"}.lexically_absolute(".x.y") == ".x.a.b");
        REQUIRE(hk::fqname{"...a.b"}.lexically_absolute(".x.y") == ".a.b");

        REQUIRE(hk::fqname{"a.b.c"}.lexically_absolute(".x.y") == ".x.y.a.b.c");
        REQUIRE(hk::fqname{".a.b.c"}.lexically_absolute(".x.y") == ".a.b.c");
        REQUIRE(hk::fqname{"..a.b.c"}.lexically_absolute(".x.y") == ".x.a.b.c");
        REQUIRE(hk::fqname{"...a.b.c"}.lexically_absolute(".x.y") == ".a.b.c");

        REQUIRE(hk::fqname{"a.b..c"}.lexically_absolute(".x.y") == ".x.y.a.c");
        REQUIRE(hk::fqname{".a.b..c"}.lexically_absolute(".x.y") == ".a.c");
        REQUIRE(hk::fqname{"..a.b..c"}.lexically_absolute(".x.y") == ".x.a.c");
        REQUIRE(hk::fqname{"...a.b..c"}.lexically_absolute(".x.y") == ".a.c");

        REQUIRE(hk::fqname{"a.b...c"}.lexically_absolute(".x.y") == ".x.y.c");
        REQUIRE(hk::fqname{".a.b...c"}.lexically_absolute(".x.y") == ".c");
        REQUIRE(hk::fqname{"..a.b...c"}.lexically_absolute(".x.y") == ".x.c");
        REQUIRE(hk::fqname{"...a.b...c"}.lexically_absolute(".x.y") == ".c");

        REQUIRE(hk::fqname{"a.b....c"}.lexically_absolute(".x.y") == ".x.c");
        REQUIRE(hk::fqname{".a.b....c"}.lexically_absolute(".x.y") == ".c");
        REQUIRE(hk::fqname{"..a.b....c"}.lexically_absolute(".x.y") == ".c");
        REQUIRE(hk::fqname{"...a.b....c"}.lexically_absolute(".x.y") == ".c");

        REQUIRE(hk::fqname{"a.b.....c"}.lexically_absolute(".x.y") == ".c");
        REQUIRE(hk::fqname{".a.b.....c"}.lexically_absolute(".x.y") == ".c");
        REQUIRE(hk::fqname{"..a.b.....c"}.lexically_absolute(".x.y") == ".c");
        REQUIRE(hk::fqname{"...a.b.....c"}.lexically_absolute(".x.y") == ".c");
    }
};

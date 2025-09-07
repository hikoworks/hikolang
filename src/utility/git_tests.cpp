
#include "git.hpp"
#include "path.hpp"
#include <hikotest/hikotest.hpp>

TEST_SUITE(git_suite) {
TEST_CASE(git_list)
{
    auto optional_list = hk::git_list("https://github.com/hikogui/hikolang-test-a.git");
    REQUIRE(static_cast<bool>(optional_list));
    auto list = std::move(optional_list).value();
    REQUIRE(not list.empty());
    auto it = list.find("main");
    REQUIRE(it != list.end());
    it = list.find("v1.0.0");
    REQUIRE(it != list.end());
}

TEST_CASE(git_clone)
{
    using namespace std::literals;

    auto const tmp_dir = hk::scoped_temporary_directory("git_clone");
    auto const git_url = "https://github.com/hikogui/hikolang-test-a.git"s;
    auto const git_rev = "main"s;

    auto const r = hk::git_clone(git_url, git_rev, tmp_dir.path());
    REQUIRE(r == hk::git_error::ok);
}

TEST_CASE(git_fetch_and_update_tag)
{
    using namespace std::literals;

    auto const tmp_dir = hk::scoped_temporary_directory("git_fetch_and_update_tag");
    auto const git_url = "https://github.com/hikogui/hikolang-test-a.git"s;
    auto const git_rev = "v1.0.0"s;

    // You can only directly clone a branch, specifying a tag will clone
    // the branch.
    auto const r1 = hk::git_clone(git_url, git_rev, tmp_dir.path());
    REQUIRE(r1 == hk::git_error::ok);

    auto const r2 = hk::git_fetch_and_update(git_url, git_rev, tmp_dir.path(), hk::git_checkout_flags::fresh_clone);
    REQUIRE(r2 == hk::git_error::ok);
}

TEST_CASE(git_fetch_and_update_commit)
{
    using namespace std::literals;

    auto const tmp_dir = hk::scoped_temporary_directory("git_fetch_and_update_commit");
    auto const git_url = "https://github.com/hikogui/hikolang-test-a.git"s;
    auto const git_rev = "e5e6a25"s; // v1.0.0

    // You can only directly clone a branch, specifying a tag will clone
    // the branch.
    auto const r1 = hk::git_clone(git_url, git_rev, tmp_dir.path());
    REQUIRE(r1 == hk::git_error::ok);

    auto const r2 = hk::git_fetch_and_update(git_url, git_rev, tmp_dir.path(), hk::git_checkout_flags::fresh_clone);
    REQUIRE(r2 == hk::git_error::ok);
}

TEST_CASE(git_checkout_or_clone)
{
    using namespace std::literals;

    auto const tmp_dir = hk::scoped_temporary_directory("git_checkout_or_clone");
    auto const git_url = "https://github.com/hikogui/hikolang-test-a.git"s;
    auto const git_rev1 = "v1.0.0"s;
    auto const git_rev2 = "v1.1.0"s;

    // High-level checkout or clone. In this case clone.
    auto const r1 = hk::git_checkout_or_clone(git_url, git_rev1, tmp_dir.path());
    REQUIRE(r1 == hk::git_error::ok);

    // Create a directory and a file to see if it is properly cleaned before
    // the checkout.
    std::filesystem::create_directory(tmp_dir.path() / "foo");
    REQUIRE(std::filesystem::exists(tmp_dir.path() / "foo"));
    std::filesystem::copy_file(tmp_dir.path() / "LICENSE", tmp_dir.path() / "foo" / "foo.txt");
    REQUIRE(std::filesystem::exists(tmp_dir.path() / "foo" / "foo.txt"));

    // High-level checkout or clone. In this case nothing happens
    auto const r2 = hk::git_checkout_or_clone(git_url, git_rev1, tmp_dir.path());
    REQUIRE(r2 == hk::git_error::ok);

    // Since no checkout has performed keep the other files in tact.
    REQUIRE(std::filesystem::exists(tmp_dir.path() / "foo"));
    REQUIRE(std::filesystem::exists(tmp_dir.path() / "foo" / "foo.txt"));

    // High-level checkout or clone. In this case clean and checkout.
    auto const r3 = hk::git_checkout_or_clone(git_url, git_rev2, tmp_dir.path());
    REQUIRE(r3 == hk::git_error::ok);

    // Now files must have been cleaned.
    REQUIRE(not std::filesystem::exists(tmp_dir.path() / "foo"));
}

TEST_CASE(git_checkout_or_clone_conflict)
{
    using namespace std::literals;

    auto const tmp_dir = hk::scoped_temporary_directory("git_checkout_or_clone_conflict");
    auto const git_url = "https://github.com/hikogui/hikolang-test-a.git"s;
    auto const git_rev1 = "v1.0.0"s;
    auto const git_rev2 = "v1.1.0"s;

    // High-level checkout or clone. In this case clone.
    auto const r1 = hk::git_checkout_or_clone(git_url, git_rev1, tmp_dir.path());
    REQUIRE(r1 == hk::git_error::ok);

    // Replace the contents of a file in the repository.
    std::filesystem::copy_file(
        tmp_dir.path() / "LICENSE", tmp_dir.path() / "a.hkm", std::filesystem::copy_options::overwrite_existing);

    // High-level checkout or clone. In this case nothing happens
    auto const r2 = hk::git_checkout_or_clone(git_url, git_rev1, tmp_dir.path());
    REQUIRE(r2 == hk::git_error::ok);

    // High-level checkout or clone. In this case clean and checkout.
    auto const r3 = hk::git_checkout_or_clone(git_url, git_rev2, tmp_dir.path());
    // a.hkm was modified and conflicts with a new checkout.
    REQUIRE(r3 == hk::git_error::conflict);
}
}; // TEST_SUITE(git_suite)

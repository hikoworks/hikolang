
namespace hl {

class module {
public:
    /** Path to the module file.
     *
     * This is the canonical path to the module file. This is done
     * so that we can match module files parsed from different locations.
     */
    std::filesystem::path path;

}

}
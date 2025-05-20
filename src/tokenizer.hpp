

namespace hl  {

constexpr token parse_token(std::string_view::const_iterator& it,
                            std::string_view::const_iterator end) {
    auto start = it;
    while (it != end && !std::isspace(*it)) {
        ++it;
    }
    return token(start, it);
}

template<std::invocable<hl::token> F>
constexpr void tokenize(std::string_view text, F&& f) {
    auto it = text.begin();
    auto end = text.end();
    while (it != end) {
        auto token = hiko::token(it, end);
        if (token.is_end()) {
            break;
        }
        f(token);
        it = token.end();
    }

}

}


#include "file_ifstream.hpp"
#include <cassert>
#include <format>

namespace hk {

file_ifstream::file_ifstream(std::filesystem::path path)
    : file(), _path(std::move(path))
{
}

[[nodiscard]] std::size_t file_ifstream::read(std::size_t position, std::span<char> buffer)
{
    if (buffer.size() == 0) {
        return 0; // No data to read
    }

    open();

    if (_file_stream.eof()) {
        // If the end of the file has been reached, we return 0.
        return 0;
    }

    _file_stream.seekg(position);
    if (_file_stream.bad()) {
        // If the file stream is in a bad state, we close it and return 0.
        // This can happen if the file was not opened successfully or if it
        // was closed while we were trying to read from it.
        close();
        throw std::runtime_error{std::format("Failed to seek in file stream: {}", _path.string())};

    } else if (_file_stream.eof()) {
        // If the end of the file has been reached, we return 0.
        return 0;

    } else if (_file_stream.fail()) {
        // This may still have happened because of end-of-file. But the error
        // is recoverable. Force seek to the end of the file.
        _file_stream.clear(); // Clear the fail state.
        _file_stream.seekg(0, std::ios::end);
        if (_file_stream.fail() or _file_stream.bad()) {
            // If we cannot seek to the end of the file, we close it and throw an error.
            close();
            throw std::runtime_error{std::format("Failed to seek to end of file stream: {}", _path.string())};
        }
    }

    _file_stream.read(buffer.data(), buffer.size());
    // It is important to read gcount() directly after read() due to bugs in libc++.
    auto const r = _file_stream.gcount();
    if (_file_stream.bad()) {
        // If the file stream is in a bad state, we close it and throw an error.
        close();
        throw std::runtime_error{std::format("Failed to read from file stream: {}", _path.string())};

    } else if (_file_stream.eof()) {
        return r;

    } else if (_file_stream.fail()) {
        close();
        throw std::runtime_error{std::format("Failed to read from file stream: {}", _path.string())};
    }

    return r;
}

void file_ifstream::open()
{
    if (_file_stream.is_open()) {
        return;
    }

    _file_stream.open(_path, std::ios::binary);
    if (_file_stream.fail() or _file_stream.bad()) {
        // If the file stream could not be opened, we throw an exception.
        // This is important to ensure that the file is not left in an
        // inconsistent state.
        throw std::runtime_error{std::format("Failed to open file stream: {}", _path.string())};
    }
}

void file_ifstream::close() noexcept
{
    if (not _file_stream.is_open()) {
        return; // Nothing to close
    }

    _file_stream.close();
    _file_stream.clear();
}


} // namespace hk
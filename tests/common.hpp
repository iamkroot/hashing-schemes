#ifndef COMMON_HPP
#define COMMON_HPP

#include <filesystem>

#define private public

#include "DiskManager.hpp"

#undef private

class DiskManagerFixture {
private:

    static std::string get_temp_db_path() {
        auto path = std::filesystem::temp_directory_path();
        path /= "temp.db";
        return path;
    }

protected:
    const std::string path = get_temp_db_path();
    DiskManager dm;

    DiskManagerFixture() : dm(DiskManager(path)) {}

public:
    virtual ~DiskManagerFixture() {
        std::filesystem::remove(path);
    }

};

#endif //COMMON_HPP

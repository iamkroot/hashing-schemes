#ifndef COMMON_HPP
#define COMMON_HPP
#include <filesystem>
#include "DiskManager.hpp"

class DiskManagerFixture {
private:

    static std::string get_temp_db_path() {
        auto path = std::filesystem::temp_directory_path();
        path += "/temp.db";
        return path.string();
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

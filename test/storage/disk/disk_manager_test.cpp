
#include "storage/disk/disk_manager.h"

#include <cstring>
#include <filesystem>
#include <gtest/gtest.h>

#include "common/config.h"

namespace leichtdb {
    static std::filesystem::path db_file_name("test.leicht");

    class DiskManagerTest : public ::testing::Test {
    protected:
        void SetUp() override {
            remove(db_file_name);
        }

        void TearDown() override {
            remove(db_file_name);
        }
    };

    TEST_F(DiskManagerTest, ReadWritePageTest) {
        char data[LEICHT_DB_PAGE_SIZE];
        char buf[LEICHT_DB_PAGE_SIZE];
        std::strncpy(data, "God does not throw dice", sizeof(data));
        auto dm = DiskManager(db_file_name);

        dm.WritePage(1, data);
        dm.ReadPage(1, buf);
        EXPECT_EQ(std::memcmp(data, buf, sizeof(buf)), 0);

        dm.ReadPage(0, buf);
        EXPECT_NE(std::memcmp(buf, data, sizeof(buf)), 1);

    }
}

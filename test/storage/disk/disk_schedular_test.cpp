

#include "storage/disk/disk_schedular.h"

#include <cstring>
#include <filesystem>
#include <future>  // NOLINT
#include <iostream>
#include <memory>

#include "common/config.h"
#include "gtest/gtest.h"
#include "storage/disk/disk_manager.h"


namespace leichtdb {

    static std::filesystem::path db_file_name("test.leicht");

    class DiskSchedulerTest : public ::testing::Test {
    protected:
        void SetUp() override {
            remove(db_file_name);
        }

        void TearDown() override {
            remove(db_file_name);
        }
    };


    TEST_F(DiskSchedulerTest, ScheduleWriteReadPageTest) {
        char buf[LEICHT_DB_PAGE_SIZE] = {0};
        char data[LEICHT_DB_PAGE_SIZE] = {0};

        auto dm = DiskManager(db_file_name);
        auto disk_scheduler = std::make_unique<DiskScheduler>(&dm);

        std::strncpy(data, "A test string.", sizeof(data));
        std::cout << data << std::endl;

        auto promise1 = disk_scheduler->CreatePromise();
        auto future1 = promise1.get_future();
        auto promise2 = disk_scheduler->CreatePromise();
        auto future2 = promise2.get_future();

        DiskRequest r1{/*is_write=*/true, data, /*page_id=*/0, std::move(promise1)};
        std::vector<DiskRequest> requests1;
        requests1.push_back(std::move(r1));
        disk_scheduler->Schedule(requests1);

        DiskRequest r2{/*is_write=*/false, buf, /*page_id=*/0, std::move(promise2)};
        std::vector<DiskRequest> requests2;
        requests2.push_back(std::move(r2));
        disk_scheduler->Schedule(requests2);

        ASSERT_TRUE(future1.get());
        ASSERT_TRUE(future2.get());
        ASSERT_EQ(std::memcmp(buf, data, sizeof(buf)), 0);

        disk_scheduler = nullptr;  // Call the DiskScheduler destructor to finish all scheduled jobs.
    }

}

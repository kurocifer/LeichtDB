#pragma once
#include <filesystem>
#include <fstream>
#include <mutex>
#include <unordered_map>
#include <vector>

#include "common/config.h"

namespace leichtdb {

    class DiskManager {
    public:
        explicit DiskManager(const std::filesystem::path &db_file);

        ~DiskManager();

        /**
         * Read a page from database file
         *
         * @param pid id of the page
         * @param page_data  output buffer
         */
        void ReadPage(page_id_t pid, char *page_data);

        /**
         * Write a page to database file
         *
         * @param pid id of page
         * @param page_data raw page bytes
         */
        void WritePage(page_id_t pid, const char *page_data);

        /**
         * Delete a page from database file
         *
         * @param pid id of page
         */
        void DeletePage(page_id_t pid);

    protected:
        /**
         * Allocate a new page on disk
         *
         * @return Returns offset of allocated page
         */
        auto AllocatePage() -> size_t;

    private:

        auto GetPageOffset(const page_id_t pid) -> size_t;

        std::fstream db_io_;
        std::filesystem::path db_file_name_;

        // Records the offset of each page in the db (pid -> offset)
        std::pmr::unordered_map<page_id_t, size_t> pages_;
        // Records fre slots in teh db file
        std::vector<size_t> free_slots_;

        std::mutex db_io_latch_;

        size_t page_capacity_{DEFAULT_FILE_PAGE_CAPACITY};

    };

}

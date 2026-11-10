
#include "storage/disk/disk_manager.h"

#include <cstring>
#include <iostream>

namespace leichtdb {
    DiskManager::DiskManager(const std::filesystem::path &db_file) : db_file_name_(db_file) {

        std::scoped_lock scoped_db_io_latch(db_io_latch_);
        db_io_.open(db_file_name_, std::ios::binary | std::ios::in | std::ios::out);
        if (!db_io_.is_open()) {
            // directory or file doesn't exist

            db_io_.clear();
            db_io_.open(db_file_name_, std::ios::binary | std::ios::trunc | std::ios::in | std::ios::out);

            if (!db_io_.is_open()) {
                throw std::runtime_error("Could not open database file");
            }
        }

        std::filesystem::resize_file(db_file, (page_capacity_ + 2) * LEICHT_DB_PAGE_SIZE);
    }

    void DiskManager::ReadPage(page_id_t pid, char *page_data) {
        std::scoped_lock scoped_db_io_latch(db_io_latch_);
        size_t offset = GetPageOffset(pid);

        // set read cursor to page start
        db_io_.seekg(offset);
        db_io_.read(page_data, LEICHT_DB_PAGE_SIZE);

        if (db_io_.bad()) {
            std::cerr << "I/O error while reading page " << pid << std::endl;
            return;
        }

        // ensure read was complete
        auto read_count = db_io_.gcount();
        if (read_count < LEICHT_DB_PAGE_SIZE) {
            std::cerr << "I/O error: could not read full page into buffer for page " << pid << std::endl;
            memset(page_data, 0, LEICHT_DB_PAGE_SIZE - read_count);
        }
    }

    void DiskManager::WritePage(page_id_t pid, const char *page_data) {
        std::scoped_lock scoped_db_io_latch(db_io_latch_);

        size_t offset = GetPageOffset(pid);

        db_io_.seekg(offset);
        db_io_.write(page_data, LEICHT_DB_PAGE_SIZE);

        if (db_io_.bad()) {
            std::cerr << "I/O error while writing to page " << pid << std::endl;
            return;
        }

        db_io_.flush();
    }

    void DiskManager::DeletePage(page_id_t pid) {
        std::scoped_lock scoped_db_io_latch(db_io_latch_);

        if (pages_.find(pid) == pages_.end()) return;

        size_t offset = pages_[pid];
        free_slots_.push_back(offset);
        pages_.erase(pid);
    }

    auto DiskManager::AllocatePage() -> size_t {
        if (!free_slots_.empty()) {
            auto offset = free_slots_.back();
            free_slots_.pop_back();

            return offset;
        }

        // check if the new slot will have space in the file
        // + 2 taking into consideration the header and catalog pages
        if (pages_.size() + 2 >= page_capacity_) {
            page_capacity_ *= 2;

            std::filesystem::resize_file(db_file_name_, (page_capacity_ + 2) * LEICHT_DB_PAGE_SIZE);
        }

        return pages_.size() * LEICHT_DB_PAGE_SIZE;
    }

    auto DiskManager::GetPageOffset(const page_id_t pid) -> size_t {
        if (pages_.find(pid) != pages_.end()) {
            return pages_[pid];
        }

        size_t offset = AllocatePage();
        pages_[pid] = offset;
        return offset;
    }
}

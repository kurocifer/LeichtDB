#include "storage/disk/disk_schedular.h"

namespace leichtdb {

    DiskScheduler::DiskScheduler(DiskManager *disk_manager) : disk_manager_(disk_manager){
        background_thread_.emplace([&] { StartWorkerThread(); });
    }

    DiskScheduler::~DiskScheduler() {
        request_queue_.Put(std::nullopt);
        if (background_thread_.has_value()) {
            background_thread_->join();
        }
    }

    void DiskScheduler::Schedule(std::vector<DiskRequest> &requests) {
        for (auto& request : requests) {
            request_queue_.Put(std::move(request));
        }
    }

    void DiskScheduler::StartWorkerThread() {
        while (true) {
            auto request = request_queue_.Get();

            if (request == std::nullopt) {
                return;
            }

            if (request->is_write_) {
                disk_manager_->WritePage(request->pid_, request->data_);
            } else {
                disk_manager_->ReadPage(request->pid_, request->data_);
            }

            request->callback_.set_value(true);
        }
    }
}

#pragma once
#include <future>
#include <optional>

#include "common/config.h"
#include "disk_manager.h"
#include "common/channel.h"

namespace leichtdb {

    struct DiskRequest {
        bool is_write_;
        char *data_;
        page_id_t pid_; // page id
        std::promise<bool> callback_; // call back to signal completion of a request execution
    };

    class DiskScheduler {
    public:
        explicit DiskScheduler(DiskManager *disk_manager);
        ~DiskScheduler();

        void Schedule(std::vector<DiskRequest> &requests);

        void StartWorkerThread();

        using DiskSchedularPromise = std::promise<bool>;

        auto CreatePromise() -> DiskSchedularPromise{ return {};}

    private:
        DiskManager *disk_manager_;
        Channel<std::optional<DiskRequest>> request_queue_;
        std::optional<std::thread> background_thread_;
    };
}

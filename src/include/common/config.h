#pragma once
#include <bits/types.h>

namespace leichtdb {

    static constexpr int LEICHT_DB_PAGE_SIZE = 4096;
    static constexpr int DEFAULT_FILE_PAGE_CAPACITY = 16; // The default number of pages a file ca hold

    using page_id_t = __uint32_t;

}

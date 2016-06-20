#include "log.h"

namespace Log {

    static Logger* g_logger = 0;

    Logger* logger() {
        if (!g_logger) {
            g_logger = new Logger();
        }
            return g_logger;
        }
}

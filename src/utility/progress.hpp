
#pragma once

#include "log.hpp"

namespace hk {

enum class progress_state {
    /** Progress has not started yet.
     */
    idle,

    /** Currently progressing.
     */
    running,

    /** Progress has finished successfully.
     */
    success,

    /** Progress has failed.
     */
    failed,
};

void set_progress(float ratio);

void set_progress(std::string_view message);

/** Set the progress state.
 * 
 * When set to:
 *  - Idle shows:     [             ] Message.
 *  - Running shows:  [=========    ] Message.
 *  - Running success [===SUCCESS===] Message.
 *  - Running failed  [     FAIL    ] Message.
 */
void set_progress(progress_state state);


}
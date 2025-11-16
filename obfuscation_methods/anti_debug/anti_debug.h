#ifndef ANTI_DEBUG_H
#define ANTI_DEBUG_H

#include <cstddef>

namespace anti_debug {

bool is_debugger_present();
bool has_preload();
unsigned long checksum_self(unsigned long* out_size);

// Extra anti-debug
bool is_debugger_present_ptrace();
bool parent_process_suspicious();
bool env_suspicious();
bool loaded_objects_suspicious();
unsigned long text_segments_checksum(unsigned long* out_size);

}

#endif // ANTI_DEBUG_H

#pragma once

#include "seqnum.h"

#ifdef DEF_SEQNUM_GV
thread_local SequenceNum g_seqnum;

SequenceNum getSeqnum() {
    return g_seqnum;
}
#else
extern thread_local SequenceNum g_seqnum;
#endif
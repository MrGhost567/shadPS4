// DONT MERGE dumb hack

#pragma once

#include "common/types.h"

struct SequenceNum {
    enum QueueType { acb, dcb, ccb };

    QueueType type;
    u64 frames_submitted;
    u32 seq0;
    u32 seq1;

    SequenceNum(QueueType type_, u64 frames_submitted_, u32 seq_0_, u32 seq_1_)
        : type(type_), frames_submitted(frames_submitted_), seq0(seq_0_), seq1(seq_1_) {}

    SequenceNum() : SequenceNum(QueueType::acb, 777, 777, 777) {}
};
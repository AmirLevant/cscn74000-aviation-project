#pragma once
#include <stdint.h>
#include <string.h>
#include <chrono>

#define MAX_RECEIVE_SIZE 65565

uint32_t get_timestamp()
{
    const auto p1 = std::chrono::system_clock::now();

    return static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::seconds>(
        p1.time_since_epoch()).count());
}

enum InteractionType : uint8_t {Telemetry, Request, Response};
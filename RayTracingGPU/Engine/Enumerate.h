#pragma once

#include <Utils/VulkanException.h>

#include <vector>

namespace Engine
{

template <class TValue>
void GetEnumerateVector(VkResult(enumerate)(uint32_t*, TValue*), std::vector<TValue>& vector)
{
    uint32_t count = 0;
    VK_CHECK(enumerate(&count, nullptr));

    vector.resize(count);
    VK_CHECK(enumerate(&count, vector.data()));
}

template <class THandle, class TValue>
void GetEnumerateVector(THandle handle, void(enumerate)(THandle, uint32_t*, TValue*), std::vector<TValue>& vector)
{
    uint32_t count = 0;
    enumerate(handle, &count, nullptr);

    vector.resize(count);
    enumerate(handle, &count, vector.data());
}

template <class THandle, class TValue>
void GetEnumerateVector(THandle handle, VkResult(enumerate)(THandle, uint32_t*, TValue*), std::vector<TValue>& vector)
{
    uint32_t count = 0;
    VK_CHECK(enumerate(handle, &count, nullptr));

    vector.resize(count);
    VK_CHECK(enumerate(handle, &count, vector.data()));
}

template <class THandle1, class THandle2, class TValue>
void GetEnumerateVector(THandle1 handle1, THandle2 handle2, VkResult(enumerate)(THandle1, THandle2, uint32_t*, TValue*), std::vector<TValue>& vector)
{
    uint32_t count = 0;
    VK_CHECK(enumerate(handle1, handle2, &count, nullptr));

    vector.resize(count);
    VK_CHECK(enumerate(handle1, handle2, &count, vector.data()));
}

template <class TValue>
std::vector<TValue> GetEnumerateVector(VkResult(enumerate)(uint32_t*, TValue*))
{
    std::vector<TValue> initial;
    GetEnumerateVector(enumerate, initial);
    return initial;
}

template <class THandle, class TValue>
std::vector<TValue> GetEnumerateVector(THandle handle, void(enumerate)(THandle, uint32_t*, TValue*))
{
    std::vector<TValue> initial;
    GetEnumerateVector(handle, enumerate, initial);
    return initial;
}

template <class THandle, class TValue>
std::vector<TValue> GetEnumerateVector(THandle handle, VkResult(enumerate)(THandle, uint32_t*, TValue*))
{
    std::vector<TValue> initial;
    GetEnumerateVector(handle, enumerate, initial);
    return initial;
}

template <class THandle1, class THandle2, class TValue>
std::vector<TValue> GetEnumerateVector(THandle1 handle1, THandle2 handle2, VkResult(enumerate)(THandle1, THandle2, uint32_t*, TValue*))
{
    std::vector<TValue> initial;
    GetEnumerateVector(handle1, handle2, enumerate, initial);
    return initial;
}

}    // namespace Engine

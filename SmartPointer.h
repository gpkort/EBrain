#include <memory>
#include <new>
#include "esp_heap_caps.h"

template <typename T>
struct PsramDeleter {    
    void operator()(T* ptr) const {
        if (ptr) {
            ptr->~T(); // Call the destructor explicitly
            heap_caps_free(ptr); // Free the capability-allocated block
        }
    }
};

template <typename T>
using unique_psram_ptr = std::unique_ptr<T, PsramDeleter<T>>;
template <typename T>
using shared_psram_ptr = std::shared_ptr<T>;

template <typename T, typename... Args>
unique_psram_ptr<T> make_unique_psram(Args&&... args) {
    void* raw_mem = heap_caps_malloc(sizeof(T), MALLOC_CAP_SPIRAM);
    if (!raw_mem) {
        throw std::bad_alloc();
    }
    
    T* obj = ::new (raw_mem) T(std::forward<Args>(args)...);
    
    return unique_psram_ptr<T>(obj);
}

template <typename T>
using shared_psram_ptr = std::shared_ptr<T>;
template <typename T, typename... Args>
shared_psram_ptr<T> make_shared_psram(Args&&... args) {
    void* raw_mem = heap_caps_malloc(sizeof(T), MALLOC_CAP_SPIRAM);
    if (!raw_mem) {
        throw std::bad_alloc();
    }
    
    T* obj = ::new (raw_mem) T(std::forward<Args>(args)...);
    
    return shared_psram_ptr<T>(obj, PsramDeleter<T>{});
}

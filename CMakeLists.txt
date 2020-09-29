project(TR)
cmake_minimum_required(VERSION 2.8.12)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(adjacency_list src/adjacency_list.cpp)
target_include_directories(adjacency_list PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)

add_library(pruned_path_labeling src/pruned_path_labeling.cpp)
target_include_directories(pruned_path_labeling PUBLIC include/)
target_link_libraries(pruned_path_labeling adjacency_list)

add_library(interval_label src/interval_label.cpp)
target_include_directories(interval_label PUBLIC include/)
target_link_libraries(interval_label adjacency_list)

add_library(bloom_filter_label src/bloom_filter_label.cpp)
target_include_directories(bloom_filter_label PUBLIC include/)
target_link_libraries(bloom_filter_label adjacency_list)

add_library(bloom_filter_plus src/bloom_filter_plus.cpp)
target_include_directories(bloom_filter_plus PUBLIC include/)
target_link_libraries(bloom_filter_plus adjacency_list bloom_filter_label interval_label)

add_library(transitivity_reduction src/transitivity_reduction.cpp)
target_include_directories(transitivity_reduction PUBLIC include/)
target_link_libraries(transitivity_reduction adjacency_list bloom_filter_plus interval_label)

add_subdirectory(test)

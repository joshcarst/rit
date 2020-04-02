rit_add_executable(sort
  SOURCES
    sort.cpp
)

target_link_libraries(sort
  rit::numerical_sorting
)
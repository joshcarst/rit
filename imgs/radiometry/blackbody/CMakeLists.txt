rit_add_library(radiometry_blackbody
  SOURCES
    Blackbody.cpp
  HEADERS
    Blackbody.h
)

target_link_libraries(radiometry_blackbody
  PUBLIC
    rit::plot
    opencv_core
)
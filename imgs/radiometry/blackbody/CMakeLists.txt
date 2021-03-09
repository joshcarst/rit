rit_add_library(radiometry_blackbody
  HEADERS
    Blackbody.h
)

target_link_libraries(radiometry_blackbody
  PUBLIC
    rit::plot
    opencv_core
)

set_target_properties(radiometry_blackbody
  PROPERTIES
    LINKER_LANGUAGE CXX
)
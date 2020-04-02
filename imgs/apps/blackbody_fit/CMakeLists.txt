rit_add_executable(blackbody_fit
  SOURCES
    blackbody_fit.cpp
)

target_link_libraries(blackbody_fit
  rit::radiometry_blackbody
  rit::radiometry_blackbody_fit
  rit::plot
  opencv_core
)
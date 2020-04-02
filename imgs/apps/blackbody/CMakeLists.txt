rit_add_executable(blackbody
  SOURCES
    blackbody_radiance.cpp
)

target_link_libraries(blackbody
  rit::radiometry_blackbody
)
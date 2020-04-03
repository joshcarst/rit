rit_add_executable(blackbody_temperature
  SOURCES
    blackbody_temperature.cpp
)

target_link_libraries(blackbody_temperature
  rit::radiometry_blackbody_temperature
)
cmake_minimum_required(VERSION 3.10)

project(obs-fdo-notifications-dnd)

find_package(LibObs) # libobs-dev
find_package(DBus1) # libdbus-1-dev

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

add_library(fdonotificationsdnd MODULE FdoNotificationsDnd.cpp)

target_link_libraries(fdonotificationsdnd
    libobs
    dbus-1
)

# install without "lib" prefix
set_target_properties(fdonotificationsdnd PROPERTIES PREFIX "")

install(TARGETS fdonotificationsdnd
        DESTINATION ${LIBOBS_PLUGIN_DESTINATION})

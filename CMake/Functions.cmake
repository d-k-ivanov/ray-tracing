# ###############################################################################
# Functions
# ###############################################################################

# ###############################################################################
# Print all CMake environment variables to console
# ###############################################################################
function(cmake_list_all_variables)
    MESSAGE("+++> -------------------- VARIABLES ------------------- ")
    get_cmake_property(_variableNames VARIABLES)
    list(REMOVE_DUPLICATES _variableNames)
    list(SORT _variableNames)

    foreach(_variableName ${_variableNames})
        message("+++> ${_variableName}=${${_variableName}}")
    endforeach()

    MESSAGE("+++> -------------------------------------------------- ")
endfunction()

# ###############################################################################
# Set Time and Date variables
# ###############################################################################
function(init_time_and_date_variables)
    string(TIMESTAMP DATE_DAY "%d" UTC)
    string(TIMESTAMP DATE_MONTH "%m" UTC)
    string(TIMESTAMP DATE_YEAR "%Y" UTC)
    string(TIMESTAMP TIME_HOUR "%H" UTC)
    string(TIMESTAMP TIME_MINUTE "%M" UTC)
    string(TIMESTAMP TIME_SECOND "%S" UTC)
    set(CMAKE_DATE_DAY ${DATE_DAY} PARENT_SCOPE)
    set(CMAKE_DATE_MONTH ${DATE_MONTH} PARENT_SCOPE)
    set(CMAKE_DATE_YEAR ${DATE_YEAR} PARENT_SCOPE)
    set(CMAKE_TIME_HOUR ${TIME_HOUR} PARENT_SCOPE)
    set(CMAKE_TIME_MINUTE ${TIME_MINUTE} PARENT_SCOPE)
    set(CMAKE_TIME_SECOND ${TIME_SECOND} PARENT_SCOPE)
endfunction()

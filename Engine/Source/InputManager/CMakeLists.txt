# get all files
file(GLOB
	PRIVATE_FILES
	private/*.cpp
	private/*.h
)
file(GLOB
	PUBLIC_FILES
	public/*.h
)

# Add library
include(GenerateExportHeader)
add_library(InputManager SHARED)
generate_export_header(InputManager EXPORT_FILE_NAME export.h EXPORT_MACRO_NAME INPUT_MANAGER_API)

target_compile_definitions(InputManager
	PRIVATE
		InputManager_EXPORTS
		UNICODE)

# Set Sources
target_sources(InputManager
	PRIVATE ${PRIVATE_FILES}
	PRIVATE ${PUBLIC_FILES}
)

# include dir
target_include_directories(InputManager 
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/private
)

target_link_libraries(InputManager PRIVATE
	Math
	Core
)

target_precompile_headers(InputManager
	PRIVATE
		<unordered_map>
		<vector>
		<string>
		<cassert>
		<format>
		<functional>
		<thread>
		<mutex>
		<condition_variable>
		<tuple>
		<utility>
		<cstdint>
		<thread>
		<limits>
		<chrono>
)
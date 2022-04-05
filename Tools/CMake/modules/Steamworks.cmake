include_guard(GLOBAL)

if (EXISTS "${SDK_DIR}/Steamworks")
	if (WIN32)
		add_library(Steamworks SHARED IMPORTED GLOBAL)
		set_target_properties(Steamworks PROPERTIES IMPORTED_LOCATION "${SDK_DIR}/Steamworks/redistributable_bin/win64/steam_api64.dll")
		set_target_properties(Steamworks PROPERTIES IMPORTED_IMPLIB "${SDK_DIR}/Steamworks/redistributable_bin/win64/steam_api64.lib")
		set_target_properties(Steamworks PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${SDK_DIR}/Steamworks/public")

		deploy_runtime_files("${SDK_DIR}/Steamworks/redistributable_bin/win64/steam_api64.dll")
	elseif (APPLE)
		add_library(Steamworks SHARED IMPORTED GLOBAL)
		set_target_properties(Steamworks PROPERTIES IMPORTED_LOCATION "${SDK_DIR}/Steamworks/redistributable_bin/osx32/libsteam_api.dylib")
		set_target_properties(Steamworks PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${SDK_DIR}/Steamworks/public")
		deploy_runtime_files("${SDK_DIR}/Steamworks/redistributable_bin/osx32/libsteam_api.dylib")
	elseif (LINUX64)
		add_library(Steamworks SHARED IMPORTED GLOBAL)
		set_target_properties(Steamworks PROPERTIES IMPORTED_LOCATION "${SDK_DIR}/Steamworks/redistributable_bin/linux64/libsteam_api.so")
		set_target_properties(Steamworks PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${SDK_DIR}/Steamworks/public")

		deploy_runtime_files("${SDK_DIR}/Steamworks/redistributable_bin/linux64/libsteam_api.so")
	endif()

	if (TARGET Steamworks)
		set_target_properties(Steamworks PROPERTIES INTERFACE_COMPILE_DEFINITIONS "USE_STEAM")
	endif()
endif()

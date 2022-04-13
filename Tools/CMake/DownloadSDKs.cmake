if (NOT EXISTS "${CRYENGINE_DIR}/Tools/branch_bootstrap/bootstrap.exe")
    if (OPTION_ENGINE OR NOT PROJECT_BUILD)
        set(WEB_PATH      "https://s3.cryengine.com/cryengine-content/cryengine-57-lts/sdk")
        set(SDK_ARCHIVE   "CRYENGINE_v5.7.0_SDKs.zip")

        if(EXISTS "${CRYENGINE_DIR}/${SDK_ARCHIVE}")
            message(STATUS "Using pre-downloaded SDKs: ${SDK_ARCHIVE}")
        else()
            message(STATUS "Downloading SDKs...")
            file(DOWNLOAD "${WEB_PATH}/${SDK_ARCHIVE}"
                "${CRYENGINE_DIR}/${SDK_ARCHIVE}" SHOW_PROGRESS)
		    set(FORCE_EXTRACT_SDKS 1)
        endif()

	    if (NOT EXISTS "${SDK_DIR}" OR DEFINED FORCE_EXTRACT_SDKS)
            message("Extracting ${SDK_ARCHIVE} to ${SDK_DIR}/...")
            file(MAKE_DIRECTORY "${SDK_DIR}")
	    	message("Extracting ${SDK_ARCHIVE} to ${SDK_DIR}/...")
            execute_process(COMMAND "${CMAKE_COMMAND}" "-E" "tar" "xzkmf" "${CRYENGINE_DIR}/${SDK_ARCHIVE}" WORKING_DIRECTORY "${SDK_DIR}")
            message("Download and extraction of SDKs completed.")
        endif()
	endif()
endif()


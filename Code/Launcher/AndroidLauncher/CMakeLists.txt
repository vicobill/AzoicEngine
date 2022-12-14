#START-FILE-LIST
# File List auto generated by waf2cmake.py, do not modify manually.
set( SOURCE_FILES
	AndroidManifest.xml
	Main.cpp
)
source_group("Source Files" FILES ${SOURCE_FILES})

set( SOURCE_FILES_C
	../../SDKs/SDL2/src/src/main/android/SDL_android_main.c
)
source_group("C Source Files" FILES ${SOURCE_FILES_C})

# SDL*.java files sourced from SDL 2.0.8; some modifications applied to SDLActivity.java

set( SOURCE_FILES_JAVA
	SDL.java
	SDLActivity.java
	SDLAudioManager.java
	SDLControllerManager.java
	CryEngineActivity.java
)
source_group("Java Files" FILES ${SOURCE_FILES_JAVA})

if(ANDROID)

	set( CFilesForAndroidLauncher_SOURCES
		${SOURCE_FILES_C}
	)
	# All .c files will be compiled by a dedicated static library because CMake with Nvidia Nsight does not support mixing C/C++ in one project
	#CryEngineStaticModule(CFilesForAndroidLauncher SOLUTION_FOLDER "Launcher")
	#target_link_libraries(${THIS_PROJECT} PRIVATE SDL2)
	add_library(CFilesForAndroidLauncher OBJECT ${CFilesForAndroidLauncher_SOURCES})
	
	set( AndroidLauncher_SOURCES
		${SOURCE_FILES}
		${SOURCE_FILES_JAVA}
	)

	target_include_directories(CFilesForAndroidLauncher PRIVATE "${SDK_DIR}/SDL2/android/include")
	target_compile_definitions(CFilesForAndroidLauncher PRIVATE USE_SDL2)
	
	#CryLauncher(AndroidLauncher SOLUTION_FOLDER Launcher)
	
	set(THIS_PROJECT AndroidLauncher)
	project(AndroidLauncher)
	add_library(AndroidLauncher SHARED ${AndroidLauncher_SOURCES} $<TARGET_OBJECTS:CFilesForAndroidLauncher>)
	set_solution_folder("Launcher" AndroidLauncher)
	SET_PLATFORM_TARGET_PROPERTIES(AndroidLauncher)

	configure_android_build(APP_NAME "CRYENGINE SDK" PACKAGE com.crytek.cryengine PERMISSIONS WRITE_EXTERNAL_STORAGE READ_EXTERNAL_STORAGE INTERNET DEBUGGABLE VERSION_CODE 1 VERSION_NAME 1.0)
	configure_android_launcher(AndroidLauncher)

	if(OPTION_STATIC_LINKING)
		target_compile_definitions(${THIS_PROJECT} PRIVATE _LIB -DCRY_IS_MONOLITHIC_BUILD)
	endif()

	target_compile_definitions(${THIS_PROJECT} PRIVATE 
		CRY_IS_APPLICATION
		ANDROID_NDK 
		ANDROID 
		DISABLE_IMPORTGL 
		__ANDROID__ 
		__ARM_EABI__ 
		__ARM_ARCH_5__ 
		__ARM_ARCH_5T__ 
		__ARM_ARCH_5E__ 
		__ARM_ARCH_5TE__ 
		__ARM_ARCH_7__ 
		__ARM_ARCH_7A__)

	set_property(TARGET ${THIS_PROJECT} APPEND_STRING PROPERTY C_FLAGS "-g2 -gdwarf-2 -fpic -ffunction-sections -funwind-tables -fno-omit-frame-pointer -fno-strict-aliasing")
	set_property(TARGET ${THIS_PROJECT} APPEND_STRING PROPERTY CXX_FLAGS "-g2 -gdwarf-2 -fpic -ffunction-sections -funwind-tables -fno-omit-frame-pointer -fno-strict-aliasing")
	
	set_libpath_flag()
	set_property(TARGET ${THIS_PROJECT} APPEND_STRING PROPERTY LINK_FLAGS "-Wl,--allow-multiple-definition -Wl,--no-undefined -Wl,-z,relro -Wl,-z,now -Wl,-z,noexecstack -shared -Wl,-soname,\"libAndroidLauncher.so\"")
	
	set_property(TARGET ${THIS_PROJECT} PROPERTY ANDROID_JAVA_SOURCE_DIR .)
	
	target_link_libraries(${THIS_PROJECT} PRIVATE m log c android)
	target_link_libraries(${THIS_PROJECT} PRIVATE SDL2)

	if(${CMAKE_GENERATOR} MATCHES "Visual Studio")
		# HACK: Bypass incorrect handling of linker flags in Nvidia NSight Tegra MSBuild toolchain
		# In order for the linker to not throw away all of the engine code because it doesn't know it's being used,
		# we rely on the linker flag --whole-archive to preserve the full contents of each module.
		# CMake specifies that target_link_libraries will accept linker flags just fine, so normally, we just surround our modules with that linker flag.
		# Unfortunately, the NSight Tegra toolchain will indiscriminately prepend -l to every item in the list,
		# including the flag, which mans the linker can't understand the command-line.
		# To bypass this and get a working build, we need to just set a raw list of linker flags, which involves handling dependencies, etc. ourselves.

		macro(link_dependencies proj)
			string(SUBSTRING "${proj}" 0 1 firstChar)
			if(firstChar STREQUAL "-") #linker flag
				set_property(TARGET ${THIS_PROJECT} APPEND_STRING PROPERTY LINK_FLAGS " ${proj}")
			else()
				if(TARGET ${proj}) # CMake target (engine module, imported library, etc)
					get_property(libdir TARGET ${proj} PROPERTY BINARY_DIR)
					get_property(outname TARGET ${proj} PROPERTY OUTPUT_NAME)
					if(NOT outname)
						set(outname ${proj})
					endif()
					string(SUBSTRING "${outname}" 0 3 prefix)
					get_property(location TARGET ${proj} PROPERTY IMPORTED_LOCATION)
					if(location)
						# we already have the location, just use that
					elseif(prefix STREQUAL "lib")
						# already has a lib prefix, so keep it
						set(location "\"${libdir}/$(Configuration)/${outname}.a\"")
					else()
						# add lib prefix
						set(location "\"${libdir}/$(Configuration)/lib${outname}.a\"")
					endif()
					set_property(TARGET ${THIS_PROJECT} APPEND_STRING PROPERTY LINK_FLAGS " ${location}")
					get_property(extradeps TARGET ${proj} PROPERTY LINK_LIBRARIES)
					foreach(dep ${extradeps})
						list(FIND processList ${dep} temp)
						if (TARGET ${dep} OR temp EQUAL -1)
							list(APPEND processList ${dep})
						endif()
					endforeach()
				else() # external library
					# Handle debug/optimized flags from target_link_libraries
					string(REGEX REPLACE "\\$<\\$<NOT:\\$<OR:\\$<CONFIG:DEBUG>,\\$<CONFIG:PROFILE>>>:(.*)>" "\\1" release_configs_library ${proj})
					string(REGEX REPLACE "\\$<\\$<OR:\\$<CONFIG:DEBUG>,\\$<CONFIG:PROFILE>>:(.*)>" "\\1" debug_configs_library ${proj})
					string(REGEX MATCH "[\\\\\\/]" is_path ${proj})
					if (NOT "${debug_configs_library}" STREQUAL "${proj}") # debug flag was applied to library
						string(REPLACE "$<CONFIG>" "$(Configuration)" debug_configs_library ${debug_configs_library})
						if(is_path)
							set_property(TARGET ${THIS_PROJECT} APPEND_STRING PROPERTY LINK_FLAGS_DEBUG " ${debug_configs_library}")
							set_property(TARGET ${THIS_PROJECT} APPEND_STRING PROPERTY LINK_FLAGS_PROFILE " ${debug_configs_library}")
						else()
							set_property(TARGET ${THIS_PROJECT} APPEND_STRING PROPERTY LINK_FLAGS_DEBUG " -l${debug_configs_library}")
							set_property(TARGET ${THIS_PROJECT} APPEND_STRING PROPERTY LINK_FLAGS_PROFILE " -l${debug_configs_library}")							
						endif()
					elseif (NOT "${release_configs_library}" STREQUAL "${proj}") # optimized flag was applied to library
						string(REPLACE "$<CONFIG>" "$(Configuration)" release_configs_library ${release_configs_library})
						if(is_path)
							set_property(TARGET ${THIS_PROJECT} APPEND_STRING PROPERTY LINK_FLAGS_RELEASE " ${release_configs_library}")
						else()
							set_property(TARGET ${THIS_PROJECT} APPEND_STRING PROPERTY LINK_FLAGS_RELEASE " -l${release_configs_library}")
						endif()
					else() # library used in all configurations
						string(REPLACE "$<CONFIG>" "$(Configuration)" temp ${proj})
						if(is_path)
							set_property(TARGET ${THIS_PROJECT} APPEND_STRING PROPERTY LINK_FLAGS " ${temp}")
						else()
							set_property(TARGET ${THIS_PROJECT} APPEND_STRING PROPERTY LINK_FLAGS " -l${temp}")
						endif()
					endif()
				endif()
			endif()
		endmacro()

		add_dependencies(${THIS_PROJECT} ${MODULES} ${GAME_MODULES})
		wrap_whole_archive(${THIS_PROJECT} WRAPPED_MODULES MODULES)
		wrap_whole_archive(${THIS_PROJECT} WRAPPED_GAME_MODULES GAME_MODULES)
		set(processList ${WRAPPED_MODULES} ${WRAPPED_GAME_MODULES})
		set(c 0)
		while(TRUE)
			list(GET processList ${c} p)		
			link_dependencies(${p})
			math(EXPR c "${c}+1")
			list(LENGTH processList length)
			if(NOT c LESS length)
				break()
			endif()
		endwhile()
	else()
		wrap_whole_archive(${THIS_PROJECT} WRAPPED_MODULES MODULES)
		wrap_whole_archive(${THIS_PROJECT} WRAPPED_GAME_MODULES GAME_MODULES)
		target_link_libraries(${THIS_PROJECT} PRIVATE ${WRAPPED_MODULES} ${WRAPPED_GAME_MODULES})
	endif()
	
	if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	target_compile_options(${THIS_PROJECT} PRIVATE
		-Wno-unused-variable
		-Wno-unused-function
		-Wno-unused-value
		-Wno-unused-private-field
		-Wno-unknown-pragmas
		-Wno-parentheses
		-Wno-format
		-Wno-empty-body
	)
	endif()
else()
	CryFileContainer(AndroidLauncher SOLUTION_FOLDER "Launcher")
endif()



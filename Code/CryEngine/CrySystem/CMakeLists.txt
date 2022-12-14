if(EXISTS ${SDK_DIR}/Scaleform)
	set(HAS_SCALEFORM TRUE)	
	add_subdirectory(Scaleform)
endif()

#START-FILE-LIST
# File List auto generated by waf2cmake.py, do not modify manually.

start_sources()
# -- Scaleform/scaleform.waf_files --
sources_platform(ALL AND HAS_SCALEFORM)
add_sources("NoUberFile"
	PROJECTS CrySystem
	SOURCE_GROUP "Scaleform"
		"Scaleform/ScaleformHelper.cpp"
		"Scaleform/ScaleformHelper.h"
		"Scaleform/ScaleformRecording.h"
		"Scaleform/ScaleformRecording.cpp"
)

add_sources("Scaleform_uber.cpp"
	PROJECTS CrySystem
	SOURCE_GROUP "Scaleform"
		"Scaleform/FlashPlayerInstance.cpp"
		"Scaleform/GAllocatorCryMem.cpp"
		"Scaleform/GFileCryPak.cpp"
		"Scaleform/GFxVideoSoundCrySound.cpp"
		"Scaleform/GFxVideoWrapper.cpp"
		"Scaleform/SharedResources.cpp"
		"Scaleform/SharedStates.cpp"
		"Scaleform/FlashPlayerInstance.h"
		"Scaleform/GAllocatorCryMem.h"
		"Scaleform/GMemorySTLAlloc.h"
		"Scaleform/GFileCryPak.h"
		"Scaleform/GFxVideoSoundCrySound.h"
		"Scaleform/GFxVideoWrapper.h"
		"Scaleform/SharedResources.h"
		"Scaleform/SharedStates.h"
		"Scaleform/GImeHelper.h"
		"Scaleform/GImeHelper.cpp"
		"Scaleform/GTexture_Impl.h"
		"Scaleform/GTexture_Impl.cpp"
		"Scaleform/GImageInfo_Impl.h"
		"Scaleform/GImageInfo_Impl.cpp"
)

# -- crysystem_sce.waf_files --
sources_platform(ORBIS)
add_sources("CrySystem_uber_2.cpp"
	PROJECTS CrySystem
	SOURCE_GROUP "MemoryManager"
		"CryMemoryManager_sce.h"
)

add_sources("CrySystem_uber_6.cpp"
	PROJECTS CrySystem
	SOURCE_GROUP "Threading"
		"CryThreadUtil_sce.h"
)

# -- crysystem_win32.waf_files --
sources_platform(DURANGO WINDOWS)
add_sources("CrySystem_uber_6.cpp"
	PROJECTS CrySystem
	SOURCE_GROUP "Threading"
		"CryThreadUtil_win32.h"
)

# -- crysystem_orbis.waf_files --
sources_platform(ORBIS)
add_sources("CrySystem_uber_0.cpp"
	PROJECTS CrySystem
	SOURCE_GROUP "Source Files"
		"MemReplay_Orbis.cpp"
)

add_sources("CrySystem_uber_1.cpp"
	PROJECTS CrySystem
	SOURCE_GROUP "Source Files"
		"OrbisDebugCallStack.cpp"
)

add_sources("CrySystem_uber_4.cpp"
	PROJECTS CrySystem
	SOURCE_GROUP "Profiling"
		"Profiling/PlatformProfiler_orbis.cpp"
)

add_sources("CrySystem_uber_7.cpp"
	PROJECTS CrySystem
	SOURCE_GROUP "PlatformOS"
		"PlatformOS/PlatformOS_Orbis.cpp"
		"PlatformOS/PlatformOS_Orbis.h"
		"PlatformOS/PlatformOSDialog_Sce.cpp"
		"PlatformOS/PlatformOSDialog_Sce.h"
		"PlatformOS/PlatformOSUserManager_Sce.cpp"
		"PlatformOS/PlatformOSUserManager_Sce.h"
)

# -- crysystem_posix.waf_files --
sources_platform(ANDROID APPLE LINUX ORBIS WINDOWS)
add_sources("CrySystem_uber_6.cpp"
	PROJECTS CrySystem
	SOURCE_GROUP "Threading"
		"CryThreadUtil_posix.h"
)

# -- crysystem.waf_files --
sources_platform(ALL)
add_sources("CrySystem_uber_0.cpp"
	PROJECTS CrySystem
	SOURCE_GROUP "Header Files"
		"SystemInit.h"
		"Statistics/LocalMemoryUsage.h"
		"XML/ReadWriteXMLSink.h"
		"Serialization/ArchiveHost.h"
		"PlatformOS/PatternMatcher.h"
		"BudgetingSystem.h"
		"CmdLine.h"
		"CmdLineArg.h"
		"ConsoleBatchFile.h"
		"ConsoleHelpGen.h"
		"CPUDetect.h"
		"CryPak.h"
		"CryPakHandleCache.h"
		"CrySizerImpl.h"
		"CrySizerStats.h"
		"CryWaterMark.h"
		"DebugCallStack.h"
		"GeneralMemoryHeap.h"
		"HardwareMouse.h"
		"HotUpdate.h"
		"IDebugCallStack.h"
		"Log.h"
		"NotificationNetwork.h"
		"PakVars.h"
		"resource.h"
		"SimpleStringPool.h"
		"System.h"
		"SystemCFG.h"
		"SystemEventDispatcher.h"
		"Timer.h"
		"Validator.h"
		"XConsole.h"
		"XConsoleVariable.h"
		"BootProfiler.h"
		"ManualFrameStep.h"
	SOURCE_GROUP "Source Files"
		"AutoDetectCPUTestSuit.cpp"
		"AutoDetectSpec.cpp"
		"AVI_Reader.cpp"
		"BudgetingSystem.cpp"
		"ClientHandler.cpp"
		"CmdLine.cpp"
		"CmdLineArg.cpp"
		"CompressedFile.cpp"
		"ConsoleBatchFile.cpp"
		"ConsoleHelpGen.cpp"
		"CPUDetect.cpp"
		"CryArchive.cpp"
		"CryAsyncMemcpy.cpp"
		"CryPak.cpp"
		"CrySizerImpl.cpp"
		"CrySizerStats.cpp"
		"DebugCallStack.cpp"
		"DllMain.cpp"
		"FileIOWrapper.cpp"
		"GeneralMemoryHeap.cpp"
		"HandlerBase.cpp"
		"HardwareMouse.cpp"
		"IDebugCallStack.cpp"
		"JiraClient.cpp"
		"AsyncPakManager.cpp"
		"Log.cpp"
		"MemReplay.cpp"
		"BootProfiler.cpp"
		"ManualFrameStep.cpp"
)

add_sources("CrySystem_uber_1.cpp"
	PROJECTS CrySystem
	SOURCE_GROUP "Source Files"
		"SimpleStringPool.cpp"
		"SystemRender.cpp"
		"NotificationNetwork.cpp"
		"PhysRenderer.cpp"
		"ResourceManager.cpp"
		"ServerHandler.cpp"
		"ServerThrottle.cpp"
		"SyncLock.cpp"
		"System.cpp"
		"SystemCFG.cpp"
		"SystemEventDispatcher.cpp"
		"SystemInit.cpp"
		"SystemScheduler.cpp"
		"SystemWin32.cpp"
		"Timer.cpp"
		"UnixConsole.cpp"
		"WindowsConsole.cpp"
		"XConsole.cpp"
		"XConsoleVariable.cpp"
		"AutoDetectCPUTestSuit.h"
		"AutoDetectSpec.h"
		"AVI_Reader.h"
		"ClientHandler.h"
		"CryArchive.h"
		"DxDiag.h"
		"FileIOWrapper.h"
		"HandlerBase.h"
		"JiraClient.h"
		"AsyncPakManager.h"
		"PhysRenderer.h"
		"ResourceManager.h"
		"ServerHandler.h"
		"ServerThrottle.h"
		"SyncLock.h"
		"SystemScheduler.h"
		"UnixConsole.h"
		"WindowsConsole.h"
)

add_sources("CrySystem_uber_2.cpp"
	PROJECTS CrySystem
	SOURCE_GROUP "MemoryManager"
		"DefragAllocator.cpp"
		"MemoryAddressRange.cpp"
		"PageMappingHeap.cpp"
		"CryMemoryManager.cpp"
		"CustomMemoryHeap.h"
		"CustomMemoryHeap.cpp"
		"MemoryManager.cpp"
		"MTSafeAllocator.cpp"
		"DefragAllocator.h"
		"MemoryAddressRange.h"
		"PageMappingHeap.h"
		"MemoryManager.h"
		"MemReplay.h"
		"MTSafeAllocator.h"
	SOURCE_GROUP "Resource Files"
		"crash_face.bmp"
	SOURCE_GROUP "StreamEngine"
		"StreamEngine/StreamAsyncFileRequest.cpp"
		"StreamEngine/StreamAsyncFileRequest_Jobs.cpp"
		"StreamEngine/StreamEngine.cpp"
		"StreamEngine/StreamIOThread.cpp"
		"StreamEngine/StreamReadStream.cpp"
		"StreamEngine/StreamAsyncFileRequest.h"
		"StreamEngine/StreamEngine.h"
		"StreamEngine/StreamIOThread.h"
		"StreamEngine/StreamReadStream.h"
)

add_sources("CrySystem_uber_3.cpp"
	PROJECTS CrySystem
	SOURCE_GROUP "XML"
		"XML/SerializeXMLReader.cpp"
		"XML/SerializeXMLWriter.cpp"
		"XML/xml.cpp"
		"XML/XMLBinaryNode.cpp"
		"XML/XMLBinaryReader.cpp"
		"XML/XMLBinaryWriter.cpp"
		"XML/XMLPatcher.cpp"
		"XML/XmlUtils.cpp"
		"XML/SerializeXMLReader.h"
		"XML/SerializeXMLWriter.h"
		"XML/xml.h"
		"XML/XMLPatcher.h"
		"XML/xml_string.h"
		"XML/XMLBinaryNode.h"
		"XML/XMLBinaryReader.h"
		"XML/XMLBinaryWriter.h"
		"XML/XmlUtils.h"
	SOURCE_GROUP "XML\\\\XMLSink"
		"XML/ReadXMLSink.cpp"
		"XML/WriteXMLSource.cpp"
	SOURCE_GROUP "ZipDir"
		"ZipEncrypt.cpp"
		"ZipDirCache.cpp"
		"ZipDirCacheFactory.cpp"
		"ZipDirCacheRW.cpp"
		"ZipDirFind.cpp"
		"ZipDirFindRW.cpp"
		"ZipDirList.cpp"
		"ZipDirStructures.cpp"
		"ZipDirTree.cpp"
		"ZipEncrypt.h"
		"ZipDir.h"
		"ZipDirCache.h"
		"ZipDirCacheFactory.h"
		"ZipDirCacheRW.h"
		"ZipDirFind.h"
		"ZipDirFindRW.h"
		"ZipDirList.h"
		"ZipDirStructures.h"
		"ZipDirTree.h"
		"ZipFileFormat.h"
		"ZipFileFormat_info.h"
)

add_sources("CrySystem_uber_4.cpp"
	PROJECTS CrySystem
	SOURCE_GROUP "HTTPDownloader"
		"DownloadManager.cpp"
		"HTTPDownloader.cpp"
		"DownloadManager.h"
		"HTTPDownloader.h"
	SOURCE_GROUP "Localization"
		"LocalizedStringManager.cpp"
		"LocalizedStringManager.h"
	SOURCE_GROUP "Profiling"
		"DiskProfiler.cpp"
		"DiskProfilerWindowsSpecific.cpp"
		"DiskProfilerWindowsSpecific.h"
		"Profiling/NullProfiler.cpp"
		"Profiling/CryProfilingSystem.cpp"
		"Profiling/ProfilingRenderer.cpp"
		"Profiling/CryBrofiler.cpp"
		"Profiling/CryProfilingSystemSharedImpl.cpp"
		"PerfHUD.cpp"
		"DiskProfiler.h"
		"Profiling/NullProfiler.h"
		"Profiling/ProfilingRenderer.h"
		"Profiling/CryProfilingSystem.h"
		"Profiling/CryBrofiler.h"
		"Profiling/PlatformProfiler.h"
		"Profiling/CryProfilingSystemSharedImpl.h"
		"PerfHUD.h"
		"Watchdog.h"
		"Watchdog.cpp"
	SOURCE_GROUP "Windows"
		"WindowsErrorReporting.cpp"
)

add_sources("CrySystem_uber_5.cpp"
	PROJECTS CrySystem
	SOURCE_GROUP "Source Files"
		"ImeManager.h"
		"ImeManager.cpp"
	SOURCE_GROUP "TestSystem"
		"ExcelExport.cpp"
		"Statistics.cpp"
		"TestSystem.cpp"
		"UnitTestExcelReporter.cpp"
		"VisRegTest.cpp"
		"ExcelExport.h"
		"Statistics.h"
		"TestSystem.h"
		"UnitTestExcelReporter.h"
		"VisRegTest.h"
	SOURCE_GROUP "Interprocess"
		"Interprocess/StatsAgent.cpp"
		"Interprocess/StatsAgent.h"
		"Interprocess/StatsAgentPipe.cpp"
		"Interprocess/StatsAgentPipe.h"
)

add_sources("CrySystem_uber_6.cpp"
	PROJECTS CrySystem
	SOURCE_GROUP "ExtensionSystem"
		"ExtensionSystem/CryPluginManager.cpp"
		"ExtensionSystem/CryPluginManager.h"
		"ExtensionSystem/CryFactoryRegistryImpl.cpp"
		"ExtensionSystem/CryFactoryRegistryImpl.h"
	SOURCE_GROUP "ExtensionSystem\\\\TestCases"
		"ExtensionSystem/TestCases/TestExtensions.cpp"
		"ExtensionSystem/TestCases/TestExtensions.h"
	SOURCE_GROUP "MiniGUI"
		"MiniGUI/DrawContext.cpp"
		"MiniGUI/MiniButton.cpp"
		"MiniGUI/MiniGUI.cpp"
		"MiniGUI/MiniInfoBox.cpp"
		"MiniGUI/MiniMenu.cpp"
		"MiniGUI/MiniTable.cpp"
		"MiniGUI/DrawContext.h"
		"MiniGUI/MiniButton.h"
		"MiniGUI/MiniGUI.h"
		"MiniGUI/MiniInfoBox.h"
		"MiniGUI/MiniMenu.h"
		"MiniGUI/MiniTable.h"
	SOURCE_GROUP "NullImplementation"
		"NullImplementation/NULLAudioSystems.h"
		"NullImplementation/NullLiveCreate.h"
		"NullImplementation/NullInput.h"
		"NullImplementation/NullScaleformHelper.h"
		"NullImplementation/NullResponseSystem.h"
		"NullImplementation/NULLRenderAuxGeom.h"
		"NullImplementation/NULLRenderAuxGeom.cpp"
	SOURCE_GROUP "Threading"
		"ThreadInfo.cpp"
		"ThreadInfo.h"
		"ThreadConfigManager.h"
		"ThreadConfigManager.cpp"
		"SystemThreading.cpp"
)

add_sources("CrySystem_uber_7.cpp"
	PROJECTS CrySystem
	SOURCE_GROUP "CodeCoverage"
		"CodeCoverage/CodeCheckpointMgr.cpp"
		"CodeCoverage/CodeCheckpointMgr.h"
	SOURCE_GROUP "PlatformOS"
		"PlatformOS/SaveReaderWriter_CryPak.cpp"
		"PlatformOS/SaveReaderWriter_Memory.cpp"
		"PlatformOS/SaveReaderWriter_CryPak.h"
		"PlatformOS/SaveReaderWriter_Memory.h"
	SOURCE_GROUP "Statistics"
		"Statistics/LocalMemoryUsage.cpp"
	SOURCE_GROUP "Statoscope"
		"Statoscope.cpp"
		"StatoscopeStreamingIntervalGroup.cpp"
		"StatoscopeTextureStreamingIntervalGroup.cpp"
		"StatoscopeStreamingIntervalGroup.h"
		"StatoscopeTextureStreamingIntervalGroup.h"
		"Statoscope.h"
	SOURCE_GROUP "ZLibCompressor"
		"ZLibCompressor.cpp"
		"ZLibCompressor.h"
)

add_sources("CrySystem_uber_8.cpp"
	PROJECTS CrySystem
	SOURCE_GROUP "HuffmanEncoding"
		"Huffman.cpp"
		"Huffman.h"
	SOURCE_GROUP "JobManager"
		"JobManager/JobManager.cpp"
		"JobManager/JobManager.h"
		"JobManager/JobStructs.h"
		"JobManager/JobSystemTests.cpp"
	SOURCE_GROUP "JobManager\\\\BlockingBackend"
		"JobManager/BlockingBackend/BlockingBackEnd.cpp"
		"JobManager/BlockingBackend/BlockingBackEnd.h"
	SOURCE_GROUP "JobManager\\\\ThreadBackEnd"
		"JobManager/PCBackEnd/ThreadBackEnd.cpp"
		"JobManager/PCBackEnd/ThreadBackEnd.h"
	SOURCE_GROUP "OverloadSceneManager"
		"OverloadSceneManager/OverloadSceneManager.cpp"
		"OverloadSceneManager/OverloadSceneManager.h"
)

add_sources("CrySystem_uber_9.cpp"
	PROJECTS CrySystem
	SOURCE_GROUP "LZ4Decompressor"
		"LZ4Decompressor.h"
		"LZ4Decompressor.cpp"
	SOURCE_GROUP "ProjectManager"
		"ProjectManager/ProjectManager.h"
		"ProjectManager/ProjectManager.cpp"
	SOURCE_GROUP "RemoteConsole"
		"RemoteConsole/RemoteConsole.cpp"
		"RemoteConsole/RemoteConsole.h"
	SOURCE_GROUP "Serialization"
		"Serialization/ArchiveHost.cpp"
		"Serialization/Token.h"
		"Serialization/XmlIArchive.cpp"
		"Serialization/XmlIArchive.h"
		"Serialization/XmlOArchive.cpp"
		"Serialization/XmlOArchive.h"
	SOURCE_GROUP "Services"
		"ServiceNetwork.cpp"
		"ServiceNetwork.h"
		"RemoteCommand.cpp"
		"RemoteCommand.h"
		"RemoteCommandHelpers.cpp"
		"RemoteCommandHelpers.h"
		"RemoteCommandServer.cpp"
		"RemoteCommandClient.cpp"
	SOURCE_GROUP "Stroboscope"
		"Stroboscope/Stroboscope.cpp"
		"Stroboscope/Stroboscope.h"
	SOURCE_GROUP "VR"
		"HMDManager.h"
		"HMDManager.cpp"
		"HMDCVars.h"
		"HMDCVars.cpp"
	SOURCE_GROUP "ZLibDecompressor"
		"ZLibDecompressor.h"
		"ZLibDecompressor.cpp"
)

add_sources("NoUberFile"
	PROJECTS CrySystem
	SOURCE_GROUP "Mac"
		"SystemUtilsApple.h"
		"SystemUtilsApple.mm"
	SOURCE_GROUP "Root"
		"CrashHandler.rc"
		"StdAfx.cpp"
		"StdAfx.h"
	SOURCE_GROUP "Source Files"
		"CryDLMalloc.c"
)

# -- crysystem_durango.waf_files --
sources_platform(DURANGO)
add_sources("CrySystem_uber_1.cpp"
	PROJECTS CrySystem
	SOURCE_GROUP "Header Files"
		"DurangoDebugCallstack.h"
	SOURCE_GROUP "Source Files"
		"DurangoDebugCallstack.cpp"
)

add_sources("CrySystem_uber_4.cpp"
	PROJECTS CrySystem
	SOURCE_GROUP "Profiling"
		"Profiling/PlatformProfiler_durango.cpp"
)

add_sources("CrySystem_uber_7.cpp"
	PROJECTS CrySystem
	SOURCE_GROUP "PlatformOS"
		"PlatformOS/PlatformOSUserManager_Durango.h"
		"PlatformOS/PlatformOSUserManager_Durango.cpp"
		"PlatformOS/StreamingInstall_Durango.h"
		"PlatformOS/StreamingInstall_Durango.cpp"
		"PlatformOS/SaveReaderWriter_Durango.h"
		"PlatformOS/SaveReaderWriter_Durango.cpp"
		"PlatformOS/PlatformOS_Durango.cpp"
		"PlatformOS/PlatformOS_Durango.h"
)

sources_platform(WINDOWS)
add_sources("CrySystem_uber_4.cpp"
	PROJECTS CrySystem
	SOURCE_GROUP "Profiling"
		"Profiling/PlatformProfiler_win.cpp"
		"Profiling/PixForWindows.cpp"
		"Profiling/PixForWindows.h"
)

# -- platformos.waf_files --
sources_platform(ANDROID APPLE LINUX WINDOWS)
add_sources("NoUberFile"
	PROJECTS CrySystem
	SOURCE_GROUP "PlatformOS"
		"PlatformOS/PlatformOS_PC.cpp"
		"PlatformOS/PlatformOS_PC.h"
)

end_sources()
#END-FILE-LIST

MACRO(setup_crysystem)
	if (WINDOWS)
		target_link_libraries(${THIS_PROJECT} PRIVATE debug Detours)
	endif ()

	target_link_libraries( ${THIS_PROJECT} PRIVATE zlib expat lz4 md5 )
	target_link_libraries(${THIS_PROJECT} PUBLIC CryScaleformInterface)
	if (NOT DURANGO)
		target_link_libraries( ${THIS_PROJECT} PRIVATE tomcrypt )
	endif()
	
	if (WINDOWS)
		set_property(TARGET ${THIS_PROJECT} APPEND_STRING PROPERTY LINK_FLAGS " /NODEFAULTLIB:libcmt.lib /NODEFAULTLIB:libcpmt.lib")
		target_link_libraries(${THIS_PROJECT} PRIVATE wininet Shell32 Ole32 Gdi32)
		target_link_libraries(${THIS_PROJECT} PRIVATE WinPixEventRuntime)
	elseif (LINUX)
		target_link_libraries( ${THIS_PROJECT} PRIVATE ncursesw m )
	elseif (APPLE)
		target_link_libraries( ${THIS_PROJECT} PRIVATE ncurses m )
	elseif (ORBIS)
        target_link_libraries( ${THIS_PROJECT} PRIVATE
            SceIme_stub_weak
            SceConvertKeycode_stub_weak
            SceMsgDialog_stub_weak
            SceErrorDialog_stub_weak
            SceNpCommerce_stub_weak
            SceWebBrowserDialog_stub_weak
            SceImeDialog_stub_weak
            SceSaveData_stub_weak
            SceSaveDataDialog_stub_weak
            SceUserService_stub_weak
            ScePlayGo_stub_weak
            SceAppContent_stub_weak
            SceSigninDialog_stub_weak
            SceSystemService_stub_weak
        )
        target_link_libraries( ${THIS_PROJECT} PRIVATE $<$<NOT:$<CONFIG:Release>>:SceRazorCpu_stub_weak>)
        target_link_libraries( ${THIS_PROJECT} PRIVATE
            $<$<NOT:$<CONFIG:Release>>:SceDbg_stub_weak>
            SceDbgPlayGo_stub_weak
            SceNpProfileDialog_stub_weak
        )
	elseif (DURANGO)
		set_property(TARGET ${THIS_PROJECT} APPEND PROPERTY COMPILE_FLAGS "\"/AI${DURANGO_PLATFORM_SDK}/ExtensionSDKs/Xbox Services API/8.0/References/CommonConfiguration/neutral\"")
		target_link_libraries( ${THIS_PROJECT} PRIVATE uuid acphal )
	elseif (ANDROID)
		target_include_directories( ${THIS_PROJECT} PRIVATE "${SDK_DIR}/SDL2/include/linux" "${SDK_DIR}/SDL2/include/SDL" )
	endif()
	
	if (OPTION_ENABLE_CRASHRPT)
		target_compile_definitions(${THIS_PROJECT} PRIVATE CRY_USE_CRASHRPT=1)
		target_link_libraries( ${THIS_PROJECT} PRIVATE crashrpt )
	endif()

	if (OPTION_ENABLE_BROFILER AND OPTION_ENGINE)
		target_compile_definitions(${THIS_PROJECT} PRIVATE USE_BROFILER)
		target_include_directories( ${THIS_PROJECT} PRIVATE "${SDK_DIR}/Brofiler")
		target_link_libraries( ${THIS_PROJECT} PRIVATE "${SDK_DIR}/Brofiler/ProfilerCore64.lib")
	endif()

	if (ORBIS OR CMAKE_CXX_COMPILER_ID MATCHES "[Cc]lang")
		target_compile_options(${THIS_PROJECT} PRIVATE
			-Wno-unknown-pragmas #See usages of raw #pragma warning
			-Wno-unused-private-field #See CScopedWireFrameMode
			-Wno-multichar #CryNetwork uses multichar constants such as 'bool'
			-Wno-unused-function # See ScaleformRecording and CrySocks_sce
			-Wno-invalid-offsetof #Used in stream engine
			-Wno-address-of-packed-member #See ZipFileFormat
			-Wno-range-loop-construct
			-Wno-deprecated-declarations # ftime() in Log.cpp
			-Wno-misleading-indentation
		)
	elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		target_compile_options(${THIS_PROJECT} PRIVATE
			-Wno-unused-function
			-Wno-unused-value
			-Wno-array-bounds
			-Wno-sign-compare
			-Wno-ignored-attributes
			-Wno-int-in-bool-context
			-Wno-unused-but-set-variable
			-Wno-misleading-indentation
			-Wno-unknown-pragmas
			-Wno-unused-const-variable
			-Wno-multichar
			-Wno-return-type
			-Wno-strict-aliasing
			-Wno-maybe-uninitialized
			-Wno-unused-result
			-Wno-char-subscripts
			-Wno-parentheses
			-Wno-comment
			-Wno-address
			-Wno-format
			-Wno-switch
			-Wno-address-of-packed-member
			-Wno-deprecated-declarations # ftime() in Log.cpp
		)
	elseif (MSVC)
		target_compile_options(${THIS_PROJECT} PRIVATE
			/we5038 #data member 'member1' will be initialized after data member 'member2' data member 'member' will be initialized after base class 'base_class'
		)
	endif()
ENDMACRO()

if (OPTION_ENABLE_CRASHRPT)
	add_subdirectory( "${CRY_LIBS_DIR}/crashrpt" "${CMAKE_CURRENT_BINARY_DIR}/crashrpt")
endif()

CryEngineModule(CrySystem PCH "StdAfx.cpp" SOLUTION_FOLDER "CryEngine")
setup_crysystem()

if (OPTION_UNIT_TEST AND NOT OPTION_STATIC_LINKING) #Currently an additional static lib is only used by unit testing under non-monolithic mode
	CryEngineModule(CrySystemLib PCH "StdAfx.cpp" SOLUTION_FOLDER "CryEngine" FORCE_STATIC)
	setup_crysystem()
endif()

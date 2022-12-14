start_sources()

sources_platform(ALL)
add_sources("CryGamePlatform_uber_0.cpp"
	SOURCE_GROUP "Interface"
		"../../Interface/IGamePlatform.h"
		"../../Interface/IPlatformAccount.h"
		"../../Interface/IPlatformAccountManager.h"
		"../../Interface/IPlatformAchievement.h"
		"../../Interface/IPlatformBase.h"
		"../../Interface/IPlatformLeaderboards.h"
		"../../Interface/IPlatformLobby.h"
		"../../Interface/IPlatformMatchmaking.h"
		"../../Interface/IPlatformNetworking.h"
		"../../Interface/IPlatformRemoteStorage.h"
		"../../Interface/IPlatformServer.h"
		"../../Interface/IPlatformService.h"
		"../../Interface/IPlatformStatistics.h"
		"../../Interface/IPlatformStreamInstall.h"
		"../../Interface/IPlatformUser.h"
		"../../Interface/IPlatformUserGeneratedContent.h"
		"../../Interface/PlatformTypes.h"
		"../../Interface/PlatformIdentifier.h"
		"../../Interface/PlatformUserIdentifier.h"
	SOURCE_GROUP "Root"
		"User.h"
		"User.cpp"
		"GamePlatform.h"
		"GamePlatform.cpp"
		"PlatformAccountManager.h"
		"PlatformAccountManager.inl"
		"resource.h"
		"StdAfx.cpp"
		"StdAfx.h"
		"CryGamePlatform.natvis"
)

add_sources("NoUberFile"
	SOURCE_GROUP "Root"
		"StdAfx.cpp"
)

end_sources()

CryEngineModule(CryGamePlatform PCH "StdAfx.cpp" SOLUTION_FOLDER "CryPlugins/GamePlatform")

target_include_directories(${THIS_PROJECT}
	PUBLIC
		"../../Interface"
		"../../Module/Core"
	PRIVATE
		"${CRYENGINE_DIR}/Code/CryEngine/CryCommon"
)

if (ORBIS OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	target_compile_options(${THIS_PROJECT} PRIVATE
		-Wno-reorder
		-Wno-parentheses
		-Wno-unknown-pragmas
		-Wno-format
		-Wno-switch
		-Wno-multichar
		-Wno-unused-private-field
		-Wno-empty-body
		-Wno-unused-value
		-Wno-unused-function
	)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(${THIS_PROJECT} PRIVATE
		-Wno-unused-function
		-Wno-unused-value
		-Wno-sign-compare
		-Wno-ignored-attributes
		-Wno-int-in-bool-context
		-Wno-unused-but-set-variable
		-Wno-misleading-indentation
		-Wno-strict-aliasing
		-Wno-maybe-uninitialized
		-Wno-unknown-pragmas
		-Wno-unused-const-variable
		-Wno-multichar
		-Wno-char-subscripts
		-Wno-parentheses
		-Wno-comment
		-Wno-address
		-Wno-format
		-Wno-switch
	)
endif()

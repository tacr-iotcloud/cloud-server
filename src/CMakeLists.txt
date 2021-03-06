cmake_minimum_required (VERSION 2.8.11)
project (ui-server C CXX)

include(TargetLink)

find_library (POCO_FOUNDATION PocoFoundation)
find_library (POCO_SSL PocoNetSSL)
find_library (POCO_CRYPTO PocoCrypto)
find_library (POCO_UTIL PocoUtil)
find_library (POCO_NET PocoNet)
find_library (POCO_JSON PocoJSON)
find_library (POCO_XML PocoXML)
find_library (POCO_DATA PocoData)
find_library (POCO_SQLITE PocoDataSQLite)
find_library (POCO_ODBC PocoDataODBC)
find_library (POCO_CASSANDRA PocoDataCassandra)
find_library (ICU_I18N icui18n)
find_library (ICU_UC icuuc)
find_library (JWT jwt)
find_library (JANSSON jansson)
find_library (ZEROMQ zmq)

set(LIBS
	${POCO_FOUNDATION}
	${POCO_SSL}
	${POCO_CRYPTO}
	${POCO_UTIL}
	${POCO_NET}
	${POCO_JSON}
	${POCO_XML}
	${POCO_DATA}
	${ICU_I18N}
	${ICU_UC}
)

find_package(Lua "5.1" EXACT REQUIRED)
message(STATUS "to solve issues with Lua version, pass your -DLUA_LIBRARY=/usr/lib64/liblua5.1.so")

add_definitions(-std=c++11)

file(GLOB SOURCES
	${PROJECT_SOURCE_DIR}/baf/EventPublisher.cpp
	${PROJECT_SOURCE_DIR}/baf/GatewayWatcher.cpp
	${PROJECT_SOURCE_DIR}/baf/PublishingWatcher.cpp
	${PROJECT_SOURCE_DIR}/cassandra/CassandraStartup.cpp
	${PROJECT_SOURCE_DIR}/dao/AbstractConfigurationQueryLoader.cpp
	${PROJECT_SOURCE_DIR}/dao/AbstractGatewayDao.cpp
	${PROJECT_SOURCE_DIR}/dao/ConnectorLoader.cpp
	${PROJECT_SOURCE_DIR}/dao/ControlDao.cpp
	${PROJECT_SOURCE_DIR}/dao/FilesystemQueryLoader.cpp
	${PROJECT_SOURCE_DIR}/dao/FCMTokenDao.cpp
	${PROJECT_SOURCE_DIR}/dao/FCMRecipientsDao.cpp
	${PROJECT_SOURCE_DIR}/dao/UserDao.cpp
	${PROJECT_SOURCE_DIR}/dao/DeviceDao.cpp
	${PROJECT_SOURCE_DIR}/dao/DevicePropertyDao.cpp
	${PROJECT_SOURCE_DIR}/dao/FCMTokenDao.cpp
	${PROJECT_SOURCE_DIR}/dao/GatewayDao.cpp
	${PROJECT_SOURCE_DIR}/dao/GatewayMessageDao.cpp
	${PROJECT_SOURCE_DIR}/dao/GatewayStatusDao.cpp
	${PROJECT_SOURCE_DIR}/dao/IdentityDao.cpp
	${PROJECT_SOURCE_DIR}/dao/IniFileQueryLoader.cpp
	${PROJECT_SOURCE_DIR}/dao/LocationDao.cpp
	${PROJECT_SOURCE_DIR}/dao/LoggingQueryProfiler.cpp
	${PROJECT_SOURCE_DIR}/dao/Query.cpp
	${PROJECT_SOURCE_DIR}/dao/QueryLoader.cpp
	${PROJECT_SOURCE_DIR}/dao/QueryProfiler.cpp
	${PROJECT_SOURCE_DIR}/dao/RoleInGatewayDao.cpp
	${PROJECT_SOURCE_DIR}/dao/SensorHistoryDao.cpp
	${PROJECT_SOURCE_DIR}/dao/VerifiedIdentityDao.cpp
	${PROJECT_SOURCE_DIR}/fcm/FCMClient.cpp
	${PROJECT_SOURCE_DIR}/fcm/FCMMessage.cpp
	${PROJECT_SOURCE_DIR}/fcm/FCMReceiverConfigurable.cpp
	${PROJECT_SOURCE_DIR}/fcm/FCMResponse.cpp
	${PROJECT_SOURCE_DIR}/fcm/FCMSender.cpp
	${PROJECT_SOURCE_DIR}/fcm/LegacyFCMClient.cpp
	${PROJECT_SOURCE_DIR}/jwt/JWToken.cpp
	${PROJECT_SOURCE_DIR}/l10n/HTTPLocaleExtractor.cpp
	${PROJECT_SOURCE_DIR}/l10n/IcuLocaleImpl.cpp
	${PROJECT_SOURCE_DIR}/l10n/IcuLocaleManager.cpp
	${PROJECT_SOURCE_DIR}/l10n/IcuTimeZoneImpl.cpp
	${PROJECT_SOURCE_DIR}/l10n/IcuTimeZoneProvider.cpp
	${PROJECT_SOURCE_DIR}/l10n/IcuTranslator.cpp
	${PROJECT_SOURCE_DIR}/l10n/TimeZone.cpp
	${PROJECT_SOURCE_DIR}/l10n/TimeZoneImpl.cpp
	${PROJECT_SOURCE_DIR}/l10n/TimeZoneProvider.cpp
	${PROJECT_SOURCE_DIR}/model/AbstractModule.cpp
	${PROJECT_SOURCE_DIR}/model/BitmapInfo.cpp
	${PROJECT_SOURCE_DIR}/model/Control.cpp
	${PROJECT_SOURCE_DIR}/model/DeviceInfo.cpp
	${PROJECT_SOURCE_DIR}/model/EnumInfo.cpp
	${PROJECT_SOURCE_DIR}/model/FCMToken.cpp
	${PROJECT_SOURCE_DIR}/model/Gateway.cpp
	${PROJECT_SOURCE_DIR}/model/GatewayMessage.cpp
	${PROJECT_SOURCE_DIR}/model/GatewayScan.cpp
	${PROJECT_SOURCE_DIR}/model/Identity.cpp
	${PROJECT_SOURCE_DIR}/model/ModuleInfo.cpp
	${PROJECT_SOURCE_DIR}/model/VerifiedIdentity.cpp
	${PROJECT_SOURCE_DIR}/model/RoleInGateway.cpp
	${PROJECT_SOURCE_DIR}/model/LegacyRoleInGateway.cpp
	${PROJECT_SOURCE_DIR}/model/LegacyGateway.cpp
	${PROJECT_SOURCE_DIR}/model/Device.cpp
	${PROJECT_SOURCE_DIR}/model/DeviceExtended.cpp
	${PROJECT_SOURCE_DIR}/model/DeviceProperty.cpp
	${PROJECT_SOURCE_DIR}/model/DeviceStatus.cpp
	${PROJECT_SOURCE_DIR}/model/DeviceWithData.cpp
	${PROJECT_SOURCE_DIR}/model/GatewayStatus.cpp
	${PROJECT_SOURCE_DIR}/model/Location.cpp
	${PROJECT_SOURCE_DIR}/model/Sensor.cpp
	${PROJECT_SOURCE_DIR}/model/ValueAt.cpp
	${PROJECT_SOURCE_DIR}/model/ModuleValue.cpp
	${PROJECT_SOURCE_DIR}/model/SubtypeInfo.cpp
	${PROJECT_SOURCE_DIR}/model/TypeInfo.cpp
	${PROJECT_SOURCE_DIR}/model/User.cpp
	${PROJECT_SOURCE_DIR}/net/SocketReadyBlocker.cpp
	${PROJECT_SOURCE_DIR}/pg/PostgreStartup.cpp
	${PROJECT_SOURCE_DIR}/policy/ControlAccessPolicy.cpp
	${PROJECT_SOURCE_DIR}/policy/FCMTokenAccessPolicy.cpp
	${PROJECT_SOURCE_DIR}/policy/GatewayAccessPolicy.cpp
	${PROJECT_SOURCE_DIR}/policy/GatewayMessageAccessPolicy.cpp
	${PROJECT_SOURCE_DIR}/policy/GatewayRateLimiter.cpp
	${PROJECT_SOURCE_DIR}/policy/IdentityAccessPolicy.cpp
	${PROJECT_SOURCE_DIR}/policy/LocationAccessPolicy.cpp
	${PROJECT_SOURCE_DIR}/policy/NoGatewayRateLimiter.cpp
	${PROJECT_SOURCE_DIR}/policy/SensorAccessPolicy.cpp
	${PROJECT_SOURCE_DIR}/policy/DeviceAccessPolicy.cpp
	${PROJECT_SOURCE_DIR}/policy/RoleAccessPolicy.cpp
	${PROJECT_SOURCE_DIR}/policy/RuledSensorHistoryAccessPolicy.cpp
	${PROJECT_SOURCE_DIR}/policy/SensorHistoryAccessPolicy.cpp
	${PROJECT_SOURCE_DIR}/policy/SensorHistoryRules.cpp
	${PROJECT_SOURCE_DIR}/policy/SlidingWindowRateLimiter.cpp
	${PROJECT_SOURCE_DIR}/policy/DefaultAccessPolicy.cpp
	${PROJECT_SOURCE_DIR}/policy/PolicyContext.cpp
	${PROJECT_SOURCE_DIR}/policy/UserPolicyContext.cpp
	${PROJECT_SOURCE_DIR}/provider/AccessTokenCredentials.cpp
	${PROJECT_SOURCE_DIR}/provider/ApiKeyCredentials.cpp
	${PROJECT_SOURCE_DIR}/provider/ApiKeyAuthProvider.cpp
	${PROJECT_SOURCE_DIR}/provider/AuthProvider.cpp
	${PROJECT_SOURCE_DIR}/provider/DeviceInfoProvider.cpp
	${PROJECT_SOURCE_DIR}/provider/PermitAuthProvider.cpp
	${PROJECT_SOURCE_DIR}/provider/RandomProvider.cpp
	${PROJECT_SOURCE_DIR}/provider/SubtypeInfoProvider.cpp
	${PROJECT_SOURCE_DIR}/provider/TypeInfoProvider.cpp
	${PROJECT_SOURCE_DIR}/server/AbstractServer.cpp
	${PROJECT_SOURCE_DIR}/server/AuthRestHandler.cpp
	${PROJECT_SOURCE_DIR}/server/CORSFilter.cpp
	${PROJECT_SOURCE_DIR}/server/FailingTCPServerConnection.cpp
	${PROJECT_SOURCE_DIR}/server/HTTPFilter.cpp
	${PROJECT_SOURCE_DIR}/server/HTTPFilterChain.cpp
	${PROJECT_SOURCE_DIR}/server/HTTPCustomHeadersFilter.cpp
	${PROJECT_SOURCE_DIR}/server/ServerListener.cpp
	${PROJECT_SOURCE_DIR}/server/Session.cpp
	${PROJECT_SOURCE_DIR}/server/SessionCache.cpp
	${PROJECT_SOURCE_DIR}/server/SessionManager.cpp
	${PROJECT_SOURCE_DIR}/server/SessionManagerImpl.cpp
	${PROJECT_SOURCE_DIR}/server/SessionVerifier.cpp
	${PROJECT_SOURCE_DIR}/server/SessionVerifierImpl.cpp
	${PROJECT_SOURCE_DIR}/server/AccessLevel.cpp
	${PROJECT_SOURCE_DIR}/server/XmlRequestHandler.cpp
	${PROJECT_SOURCE_DIR}/server/SocketServer.cpp
	${PROJECT_SOURCE_DIR}/server/WebSocketConnection.cpp
	${PROJECT_SOURCE_DIR}/server/WebSocketRequestHandler.cpp
	${PROJECT_SOURCE_DIR}/server/WebSocketServer.cpp
	${PROJECT_SOURCE_DIR}/service/ControlService.cpp
	${PROJECT_SOURCE_DIR}/service/ControlServiceImpl.cpp
	${PROJECT_SOURCE_DIR}/service/UserService.cpp
	${PROJECT_SOURCE_DIR}/service/UserServiceImpl.cpp
	${PROJECT_SOURCE_DIR}/service/AuthService.cpp
	${PROJECT_SOURCE_DIR}/service/AuthServiceImpl.cpp
	${PROJECT_SOURCE_DIR}/service/DeviceService.cpp
	${PROJECT_SOURCE_DIR}/service/DeviceServiceImpl.cpp
	${PROJECT_SOURCE_DIR}/service/FCMTokenService.cpp
	${PROJECT_SOURCE_DIR}/service/FCMTokenServiceImpl.cpp
	${PROJECT_SOURCE_DIR}/service/GWSDeviceService.cpp
	${PROJECT_SOURCE_DIR}/service/GatewayService.cpp
	${PROJECT_SOURCE_DIR}/service/GatewayServiceImpl.cpp
	${PROJECT_SOURCE_DIR}/service/GatewayMessageService.cpp
	${PROJECT_SOURCE_DIR}/service/GatewayMessageServiceImpl.cpp
	${PROJECT_SOURCE_DIR}/service/GWSGatewayService.cpp
	${PROJECT_SOURCE_DIR}/service/GWSSensorHistoryService.cpp
	${PROJECT_SOURCE_DIR}/service/IdentityListener.cpp
	${PROJECT_SOURCE_DIR}/service/LocationService.cpp
	${PROJECT_SOURCE_DIR}/service/LocationServiceImpl.cpp
	${PROJECT_SOURCE_DIR}/service/SensorService.cpp
	${PROJECT_SOURCE_DIR}/service/SensorServiceImpl.cpp
	${PROJECT_SOURCE_DIR}/service/IdentityService.cpp
	${PROJECT_SOURCE_DIR}/service/IdentityServiceImpl.cpp
	${PROJECT_SOURCE_DIR}/service/RoleService.cpp
	${PROJECT_SOURCE_DIR}/service/RoleServiceImpl.cpp
	${PROJECT_SOURCE_DIR}/service/SensorHistoryService.cpp
	${PROJECT_SOURCE_DIR}/service/SensorHistoryServiceImpl.cpp
	${PROJECT_SOURCE_DIR}/transaction/NoTransactionFactory.cpp
	${PROJECT_SOURCE_DIR}/transaction/ThreadLocalTransactionManager.cpp
	${PROJECT_SOURCE_DIR}/transaction/Transactional.cpp
	${PROJECT_SOURCE_DIR}/transaction/TransactionManager.cpp
	${PROJECT_SOURCE_DIR}/util/BitmapsSAXHandler.cpp
	${PROJECT_SOURCE_DIR}/util/DevicesSAXHandler.cpp
	${PROJECT_SOURCE_DIR}/util/DeviceMatchingTester.cpp
	${PROJECT_SOURCE_DIR}/util/DevicePropertyTool.cpp
	${PROJECT_SOURCE_DIR}/util/EnumsSAXHandler.cpp
	${PROJECT_SOURCE_DIR}/util/HTTPClient.cpp
	${PROJECT_SOURCE_DIR}/util/PocoStatementIntruder.cpp
	${PROJECT_SOURCE_DIR}/util/Preprocessor.cpp
	${PROJECT_SOURCE_DIR}/util/PreprocessorChain.cpp
	${PROJECT_SOURCE_DIR}/util/Sanitize.cpp
	${PROJECT_SOURCE_DIR}/util/Startup.cpp
	${PROJECT_SOURCE_DIR}/util/StartupSecurityChecker.cpp
	${PROJECT_SOURCE_DIR}/util/SQLPreprocessor.cpp
	${PROJECT_SOURCE_DIR}/util/TypesSAXHandler.cpp
	${PROJECT_SOURCE_DIR}/util/UnknownEvaluator.cpp
	${PROJECT_SOURCE_DIR}/rest/BuiltinRestHandler.cpp
	${PROJECT_SOURCE_DIR}/rest/CPSPRestHandler.cpp
	${PROJECT_SOURCE_DIR}/rest/JSONRestHandler.cpp
	${PROJECT_SOURCE_DIR}/rest/MappedRestAction.cpp
	${PROJECT_SOURCE_DIR}/rest/PocoContext.cpp
	${PROJECT_SOURCE_DIR}/rest/PocoRestRequestHandler.cpp
	${PROJECT_SOURCE_DIR}/rest/PocoRestServer.cpp
	${PROJECT_SOURCE_DIR}/rest/RestAction.cpp
	${PROJECT_SOURCE_DIR}/rest/RestFlow.cpp
	${PROJECT_SOURCE_DIR}/rest/RestHandler.cpp
	${PROJECT_SOURCE_DIR}/rest/RestLinker.cpp
	${PROJECT_SOURCE_DIR}/rest/RestRouter.cpp
	${PROJECT_SOURCE_DIR}/rest/UriPattern.cpp
)

file(GLOB DAO_POCO_SOURCES
	${PROJECT_SOURCE_DIR}/dao/poco/PocoAbstractDao.cpp
	${PROJECT_SOURCE_DIR}/dao/poco/PocoDaoManager.cpp
	${PROJECT_SOURCE_DIR}/dao/poco/PocoDBInitializer.cpp
	${PROJECT_SOURCE_DIR}/dao/poco/PocoGlobDBInitializer.cpp
	${PROJECT_SOURCE_DIR}/dao/poco/PocoPerQueryScriptExecutor.cpp
	${PROJECT_SOURCE_DIR}/dao/poco/PocoScriptExecutor.cpp
	${PROJECT_SOURCE_DIR}/dao/poco/PocoSimpleDBInitializer.cpp
	${PROJECT_SOURCE_DIR}/dao/poco/PocoStatementInfo.cpp
	${PROJECT_SOURCE_DIR}/dao/poco/PocoSQLControlDao.cpp
	${PROJECT_SOURCE_DIR}/dao/poco/PocoSQLDeviceDao.cpp
	${PROJECT_SOURCE_DIR}/dao/poco/PocoSQLDevicePropertyDao.cpp
	${PROJECT_SOURCE_DIR}/dao/poco/PocoSQLFCMRecipientsDao.cpp
	${PROJECT_SOURCE_DIR}/dao/poco/PocoSQLFCMTokenDao.cpp
	${PROJECT_SOURCE_DIR}/dao/poco/PocoSQLGatewayDao.cpp
	${PROJECT_SOURCE_DIR}/dao/poco/PocoSQLGatewayMessageDao.cpp
	${PROJECT_SOURCE_DIR}/dao/poco/PocoSQLGatewayStatusDao.cpp
	${PROJECT_SOURCE_DIR}/dao/poco/PocoSQLIdentityDao.cpp
	${PROJECT_SOURCE_DIR}/dao/poco/PocoSQLLocationDao.cpp
	${PROJECT_SOURCE_DIR}/dao/poco/PocoSQLRoleInGatewayDao.cpp
	${PROJECT_SOURCE_DIR}/dao/poco/PocoSQLSensorHistoryDao.cpp
	${PROJECT_SOURCE_DIR}/dao/poco/PocoSQLUserDao.cpp
	${PROJECT_SOURCE_DIR}/dao/poco/PocoSQLVerifiedIdentityDao.cpp
	${PROJECT_SOURCE_DIR}/dao/poco/PocoTransactionFactory.cpp
	${PROJECT_SOURCE_DIR}/dao/poco/PocoTransactionImpl.cpp
	${PROJECT_SOURCE_DIR}/dao/poco/StatementExceptionRethrower.cpp
)

add_library(BeeeOnServerDaoPoco ${DAO_POCO_SOURCES})

file(GLOB SQITCH_SOURCES
	${PROJECT_SOURCE_DIR}/sqitch/PocoSqitchInitializer.cpp
	${PROJECT_SOURCE_DIR}/sqitch/SqitchChange.cpp
	${PROJECT_SOURCE_DIR}/sqitch/SqitchEntry.cpp
	${PROJECT_SOURCE_DIR}/sqitch/SqitchInitializer.cpp
	${PROJECT_SOURCE_DIR}/sqitch/SqitchLine.cpp
	${PROJECT_SOURCE_DIR}/sqitch/SqitchPlan.cpp
	${PROJECT_SOURCE_DIR}/sqitch/SqitchPlanParser.cpp
	${PROJECT_SOURCE_DIR}/sqitch/SqitchPragma.cpp
	${PROJECT_SOURCE_DIR}/sqitch/SqitchRef.cpp
	${PROJECT_SOURCE_DIR}/sqitch/SqitchTag.cpp
)

add_library(BeeeOnServerSqitch ${SQITCH_SOURCES})

file(GLOB OAUTH_SOURCES
	${PROJECT_SOURCE_DIR}/oauth2/AbstractOAuth2Provider.cpp
	${PROJECT_SOURCE_DIR}/oauth2/FacebookOAuth2Provider.cpp
	${PROJECT_SOURCE_DIR}/oauth2/GenericCodeExchanger.cpp
	${PROJECT_SOURCE_DIR}/oauth2/GoogleOAuth2Provider.cpp
	${PROJECT_SOURCE_DIR}/oauth2/OAuth2CodeExchanger.cpp
	${PROJECT_SOURCE_DIR}/oauth2/OAuth2UserFetcher.cpp
	${PROJECT_SOURCE_DIR}/provider/OAuth2AuthProvider.cpp
	${PROJECT_SOURCE_DIR}/provider/FacebookAuthProvider.cpp
	${PROJECT_SOURCE_DIR}/provider/GoogleAuthProvider.cpp
)

add_library(BeeeOnServerOAuth ${OAUTH_SOURCES})

if (LUA_FOUND)
	include_directories(${LUA_INCLUDE_DIR})

	file(GLOB LUA_SOURCES
		${PROJECT_SOURCE_DIR}/util/LuaEngine.cpp
		${PROJECT_SOURCE_DIR}/util/LuaPreprocessor.cpp
	)
	list(APPEND SOURCES ${LUA_SOURCES})
	list(APPEND LIBS ${LUA_LIBRARIES})
else()
	message(STATUS "Lua support is disabled due to unmet dependencies")
endif()

if (JWT AND JANSSON)
	file(GLOB JWT_SOURCES
		${PROJECT_SOURCE_DIR}/jwt/JWTDecoder.cpp
		${PROJECT_SOURCE_DIR}/jwt/JWTEncoder.cpp
		${PROJECT_SOURCE_DIR}/oauth2/OpenIDUserDecoder.cpp
		${PROJECT_SOURCE_DIR}/server/JWTSessionManager.cpp
	)
	list(APPEND SOURCES ${JWT_SOURCES})
	list(APPEND LIBS ${JANSSON})
	list(APPEND LIBS ${JWT})
else()
	message(STATUS "JWT support is disabled due to unmet dependencies")
endif()

if (ZEROMQ)
	file (GLOB ZEROMQ_SOURCES
		${PROJECT_SOURCE_DIR}/zeromq/ZeroMQContext.cpp
		${PROJECT_SOURCE_DIR}/zeromq/ZeroMQPublisher.cpp
		${PROJECT_SOURCE_DIR}/zeromq/ZeroMQSender.cpp
	)
	list(APPEND SOURCES ${ZEROMQ_SOURCES})
	list(APPEND LIBS ${ZEROMQ})
else()
	message(STATUS "ZeroMQ support is disabled due to unmet dependencies")
endif()

file(GLOB GWS_SOURCES
	${PROJECT_SOURCE_DIR}/gws/ControlChangeHandler.cpp
	${PROJECT_SOURCE_DIR}/gws/DeviceListener.cpp
	${PROJECT_SOURCE_DIR}/gws/DevicePairHandler.cpp
	${PROJECT_SOURCE_DIR}/gws/DeviceUnpairHandler.cpp
	${PROJECT_SOURCE_DIR}/gws/GatewayCommunicator.cpp
	${PROJECT_SOURCE_DIR}/gws/GatewayConnection.cpp
	${PROJECT_SOURCE_DIR}/gws/GatewayListener.cpp
	${PROJECT_SOURCE_DIR}/gws/GatewayPeerVerifier.cpp
	${PROJECT_SOURCE_DIR}/gws/GatewayScanController.cpp
	${PROJECT_SOURCE_DIR}/gws/GWMessageHandler.cpp
	${PROJECT_SOURCE_DIR}/gws/GWMessageHandlerImpl.cpp
	${PROJECT_SOURCE_DIR}/gws/GWRequestHandler.cpp
	${PROJECT_SOURCE_DIR}/gws/GWResponseExpectedQueue.cpp
	${PROJECT_SOURCE_DIR}/gws/RelaxedGatewayPeerVerifier.cpp
	${PROJECT_SOURCE_DIR}/gws/RPCForwarder.cpp
	${PROJECT_SOURCE_DIR}/gws/SensorDataListener.cpp
	${PROJECT_SOURCE_DIR}/gws/SocketGatewayPeerVerifierFactory.cpp
	${PROJECT_SOURCE_DIR}/gws/X509GatewayPeerVerifier.cpp
	${PROJECT_SOURCE_DIR}/rpc/RPCConnector.cpp
	${PROJECT_SOURCE_DIR}/rpc/SocketRPCConnector.cpp
	${PROJECT_SOURCE_DIR}/rpc/GatewayRPC.cpp
	${PROJECT_SOURCE_DIR}/rpc/GatewayRPCHandler.cpp
	${PROJECT_SOURCE_DIR}/rpc/GatewayRPCResult.cpp
	${PROJECT_SOURCE_DIR}/rpc/GatewayRPCUnpairResult.cpp
	${PROJECT_SOURCE_DIR}/rpc/AsyncGatewayRPC.cpp
	${PROJECT_SOURCE_DIR}/rpc/FakeGatewayRPC.cpp
)

add_library(BeeeOnServerGWS ${GWS_SOURCES})

file(GLOB RESTUI_SOURCES
	restui/Serializing.cpp
	restui/JSONDeviceDeserializer.cpp
	restui/JSONDevicePropertyDeserializer.cpp
	restui/JSONGatewayDeserializer.cpp
	restui/JSONLocationDeserializer.cpp
	restui/JSONRoleInGatewayDeserializer.cpp
	restui/ControlRestHandler.cpp
	restui/DeviceRestHandler.cpp
	restui/DevicePropertiesRestHandler.cpp
	restui/FCMTokenRestHandler.cpp
	restui/LocationRestHandler.cpp
	restui/OAuth2CallbackRestHandler.cpp
	restui/ProfileRestHandler.cpp
	restui/RestValueConsumer.cpp
	restui/RoleRestHandler.cpp
	restui/SensorRestHandler.cpp
	restui/TimeZoneRestHandler.cpp
	restui/TypeRestHandler.cpp
	restui/GatewayRestHandler.cpp
	restui/MessageRestHandler.cpp
)

add_library(BeeeOnServerRestUI ${RESTUI_SOURCES})

find_program(CPSPC cpspc)

file(GLOB WEB_TESTER_INDEX
	${PROJECT_SOURCE_DIR}/tester/index.cpsp
)

file(GLOB WEB_TESTER_INDEX_DEPS
	${PROJECT_SOURCE_DIR}/tester/login.cpsp
	${PROJECT_SOURCE_DIR}/tester/profile.cpsp
	${PROJECT_SOURCE_DIR}/tester/timezones.cpsp
	${PROJECT_SOURCE_DIR}/tester/types.cpsp
	${PROJECT_SOURCE_DIR}/tester/gateway.cpsp
	${PROJECT_SOURCE_DIR}/tester/messages.cpsp
	${PROJECT_SOURCE_DIR}/tester/devices.cpsp
	${PROJECT_SOURCE_DIR}/tester/sensors.cpsp
	${PROJECT_SOURCE_DIR}/tester/notifications.cpsp
)

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/index.cpp" "${CMAKE_CURRENT_BINARY_DIR}/index.h"
	COMMAND ${CPSPC} -o "${CMAKE_CURRENT_BINARY_DIR}" "${WEB_TESTER_INDEX}"
	MAIN_DEPENDENCY "${WEB_TESTER_INDEX}"
	DEPENDS "${WEB_TESTER_INDEX_DEPS}"
	WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
	COMMENT "preprocessing ${WEB_TESTER_INDEX}"
	VERBATIM)

file(GLOB WEB_TESTER_FCMSW
	${PROJECT_SOURCE_DIR}/tester/fcmsw.cpsp
)

set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/index.cpp" PROPERTIES GENERATED 1)
set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/index.h" PROPERTIES GENERATED 1)
list(APPEND SOURCES "${CMAKE_CURRENT_BINARY_DIR}/index.cpp")

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/fcmsw.cpp" "${CMAKE_CURRENT_BINARY_DIR}/fcmsw.h"
	COMMAND ${CPSPC} -o "${CMAKE_CURRENT_BINARY_DIR}" "${WEB_TESTER_FCMSW}"
	MAIN_DEPENDENCY "${WEB_TESTER_FCMSW}"
	WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
	COMMENT "preprocessing ${WEB_TESTER_FCMSW}"
	VERBATIM)

set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/fcmsw.cpp" PROPERTIES GENERATED 1)
set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/fcmsw.h" PROPERTIES GENERATED 1)
list(APPEND SOURCES "${CMAKE_CURRENT_BINARY_DIR}/fcmsw.cpp")

include_directories(
	${PROJECT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/base/src
	${CMAKE_CURRENT_BINARY_DIR}/tester
)

add_library(BeeeOnServer SHARED ${SOURCES})

file(GLOB MAIN_CPP main.cpp)
configure_file(${MAIN_CPP} ${CMAKE_CURRENT_BINARY_DIR}/main.cpp)

if (POCO_SQLITE)
	add_library(BeeeOnServerSQLiteConnector SHARED
		dao/poco/PocoSQLiteConnectorLoader.cpp)
	target_link_libraries(BeeeOnServerSQLiteConnector ${POCO_SQLITE})
endif()

if (POCO_ODBC)
	add_library(BeeeOnServerODBCConnector SHARED
		dao/poco/PocoODBCConnectorLoader.cpp
		dao/poco/ODBCStatementExceptionRethrower.cpp)
	target_link_libraries(BeeeOnServerODBCConnector ${POCO_ODBC})
endif()

if (POCO_CASSANDRA)
	add_library(BeeeOnServerCassandraConnector SHARED
		dao/poco/PocoCassandraConnectorLoader.cpp)
else()
	message(STATUS "Cassandra support is disabled due to unmet dependencies")
endif()

add_executable(beeeon-server
	${CMAKE_CURRENT_BINARY_DIR}/main.cpp
	${DB_CONNECTOR_SOURCES}
)

list(APPEND CORE_LIBS BeeeOnBase)
list(APPEND CORE_LIBS BeeeOnServer)
list(APPEND CORE_LIBS BeeeOnServerRestUI)
list(APPEND CORE_LIBS BeeeOnServerDaoPoco)
list(APPEND CORE_LIBS BeeeOnServerSqitch)
list(APPEND CORE_LIBS BeeeOnServerGWS)
list(APPEND CORE_LIBS BeeeOnServerOAuth)

TargetLink(beeeon-server "${CORE_LIBS}" "${LIBS}")

install(TARGETS beeeon-server BeeeOnServer
	COMPONENT core
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
	ARCHIVE DESTINATION "${AR_INSTALL_DIR}")

if (POCO_SQLITE)
install(TARGETS BeeeOnServerSQLiteConnector
	COMPONENT core
	LIBRARY DESTINATION "${LIB_INSTALL_DIR}")
endif()

if (POCO_ODBC)
install(TARGETS BeeeOnServerODBCConnector
	COMPONENT core
	LIBRARY DESTINATION "${LIB_INSTALL_DIR}")
endif()

if (POCO_CASSANDRA)
install(TARGETS BeeeOnServerCassandraConnector
	COMPONENT cassandra
	LIBRARY DESTINATION "${LIB_INSTALL_DIR}")
endif()

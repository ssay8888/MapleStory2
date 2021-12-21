pushd %~dp0
protoc.exe -I=./ --cpp_out=./ ./center_login_enum.proto
protoc.exe -I=./ --cpp_out=./ ./center_login_struct.proto
protoc.exe -I=./ --cpp_out=./ ./center_login_protocol.proto

GenPackets.exe --path=./center_login_protocol.proto --output=CenterLoginClientPacketHandler --createfilename=center_login_client_packet_handler --recv=CenterLoginClient --send=CenterLoginServer
GenPackets.exe --path=./center_login_protocol.proto --output=CenterLoginServerPacketHandler --createfilename=center_login_server_packet_handler --recv=CenterLoginServer --send=CenterLoginClient

IF ERRORLEVEL 1 PAUSE

XCOPY /Y center_login_enum.pb.h "../../../Server/LoginServer/protocol"
XCOPY /Y center_login_enum.pb.cc "../../../Server/LoginServer/protocol"
XCOPY /Y center_login_struct.pb.h "../../../Server/LoginServer/protocol"
XCOPY /Y center_login_struct.pb.cc "../../../Server/LoginServer/protocol"
XCOPY /Y center_login_protocol.pb.h "../../../Server/LoginServer/protocol"
XCOPY /Y center_login_protocol.pb.cc "../../../Server/LoginServer/protocol"
XCOPY /Y center_login_client_packet_handler.h "../../../Server/LoginServer/center_server_session"

XCOPY /Y center_login_enum.pb.h "../../../Server/GameServer/protocol"
XCOPY /Y center_login_enum.pb.cc "../../../Server/GameServer/protocol"
XCOPY /Y center_login_struct.pb.h "../../../Server/GameServer/protocol"
XCOPY /Y center_login_struct.pb.cc "../../../Server/GameServer/protocol"
XCOPY /Y center_login_protocol.pb.h "../../../Server/GameServer/protocol"
XCOPY /Y center_login_protocol.pb.cc "../../../Server/GameServer/protocol"
XCOPY /Y center_login_server_packet_handler.h "../../../Server/GameServer/center_client_session"

DEL /Q /F *.pb.h
DEL /Q /F *.pb.cc
DEL /Q /F *.h

PAUSE
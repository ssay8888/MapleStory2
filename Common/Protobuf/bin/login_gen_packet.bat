pushd %~dp0
protoc.exe -I=./ --cpp_out=./ ./login_enum.proto
protoc.exe -I=./ --cpp_out=./ ./login_struct.proto
protoc.exe -I=./ --cpp_out=./ ./login_protocol.proto

GenPackets.exe --path=./login_protocol.proto --output=LoginClientPacketHandler --createfilename=login_client_packet_handler --recv=LoginClient --send=LoginServer
GenPackets.exe --path=./login_protocol.proto --output=LoginServerPacketHandler --createfilename=login_server_packet_handler --recv=LoginServer --send=LoginClient

IF ERRORLEVEL 1 PAUSE

XCOPY /Y login_enum.pb.h "../../../Server/LoginServer/protocol"
XCOPY /Y login_enum.pb.cc "../../../Server/LoginServer/protocol"
XCOPY /Y login_struct.pb.h "../../../Server/LoginServer/protocol"
XCOPY /Y login_struct.pb.cc "../../../Server/LoginServer/protocol"
XCOPY /Y login_protocol.pb.h "../../../Server/LoginServer/protocol"
XCOPY /Y login_protocol.pb.cc "../../../Server/LoginServer/protocol"
XCOPY /Y login_client_packet_handler.h "../../../Server/LoginServer/login_session"

XCOPY /Y login_enum.pb.h "../../../Client/MapleStory2/protocol"
XCOPY /Y login_enum.pb.cc "../../../Client/MapleStory2/protocol"
XCOPY /Y login_struct.pb.h "../../../Client/MapleStory2/protocol"
XCOPY /Y login_struct.pb.cc "../../../Client/MapleStory2/protocol"
XCOPY /Y login_protocol.pb.h "../../../Client/MapleStory2/protocol"
XCOPY /Y login_protocol.pb.cc "../../../Client/MapleStory2/protocol"
XCOPY /Y login_server_packet_handler.h "../../../Client/MapleStory2/src/network"

DEL /Q /F *.pb.h
DEL /Q /F *.pb.cc
DEL /Q /F *.h

PAUSE
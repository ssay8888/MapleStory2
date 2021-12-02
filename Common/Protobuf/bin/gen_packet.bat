pushd %~dp0
protoc.exe -I=./ --cpp_out=./ ./enum.proto
protoc.exe -I=./ --cpp_out=./ ./struct.proto
protoc.exe -I=./ --cpp_out=./ ./protocol.proto

GenPackets.exe --path=./protocol.proto --output=ClientPacketHandler --createfilename=client_packet_handler --recv=Client --send=Server
GenPackets.exe --path=./protocol.proto --output=ServerPacketHandler --createfilename=server_packet_handler --recv=Server --send=Client

IF ERRORLEVEL 1 PAUSE

XCOPY /Y enum.pb.h "../../../Server/GameServer/protocol"
XCOPY /Y e.pb.cc "../../../Server/GameServer/protocol"
XCOPY /Y struct.pb.h "../../../Server/GameServer/protocol"
XCOPY /Y struct.pb.cc "../../../Server/GameServer/protocol"
XCOPY /Y protocol.pb.h "../../../Server/GameServer/protocol"
XCOPY /Y protocol.pb.cc "../../../Server/GameServer/protocol"
XCOPY /Y client_packet_handler.h "../../../Server/GameServer/game_session"

XCOPY /Y enum.pb.h "../../../Client/MapleStory2/protocol"
XCOPY /Y enum.pb.cc "../../../Client/MapleStory2/protocol"
XCOPY /Y struct.pb.h "../../../Client/MapleStory2/protocol"
XCOPY /Y struct.pb.cc "../../../Client/MapleStory2/protocol"
XCOPY /Y protocol.pb.h "../../../Client/MapleStory2/protocol"
XCOPY /Y protocol.pb.cc "../../../Client/MapleStory2/protocol"
XCOPY /Y server_packet_handler.h "../../../Client/MapleStory2/src/network"

DEL /Q /F *.pb.h
DEL /Q /F *.pb.cc
DEL /Q /F *.h

PAUSE
pushd %~dp0
protoc.exe -I=./ --cpp_out=./ ./game_enum.proto
protoc.exe -I=./ --cpp_out=./ ./game_struct.proto
protoc.exe -I=./ --cpp_out=./ ./game_protocol.proto

GenPackets.exe --path=./game_protocol.proto --output=GameClientPacketHandler --createfilename=game_client_packet_handler --recv=GameClient --send=GameServer --pktid=2000
GenPackets.exe --path=./game_protocol.proto --output=GameServerPacketHandler --createfilename=game_server_packet_handler --recv=GameServer --send=GameClient --pktid=2000

IF ERRORLEVEL 1 PAUSE

XCOPY /Y game_enum.pb.h "../../../Server/GameServer/protocol"
XCOPY /Y game_enum.pb.cc "../../../Server/GameServer/protocol"
XCOPY /Y game_struct.pb.h "../../../Server/GameServer/protocol"
XCOPY /Y game_struct.pb.cc "../../../Server/GameServer/protocol"
XCOPY /Y game_protocol.pb.h "../../../Server/GameServer/protocol"
XCOPY /Y game_protocol.pb.cc "../../../Server/GameServer/protocol"
XCOPY /Y game_client_packet_handler.h "../../../Server/GameServer/game_session"

XCOPY /Y game_enum.pb.h "../../../Client/MapleStory2/protocol"
XCOPY /Y game_enum.pb.cc "../../../Client/MapleStory2/protocol"
XCOPY /Y game_struct.pb.h "../../../Client/MapleStory2/protocol"
XCOPY /Y game_struct.pb.cc "../../../Client/MapleStory2/protocol"
XCOPY /Y game_protocol.pb.h "../../../Client/MapleStory2/protocol"
XCOPY /Y game_protocol.pb.cc "../../../Client/MapleStory2/protocol"
XCOPY /Y game_server_packet_handler.h "../../../Client/MapleStory2/src/network"


DEL /Q /F *.pb.h
DEL /Q /F *.pb.cc
DEL /Q /F *.h

PAUSE
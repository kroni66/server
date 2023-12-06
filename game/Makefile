CC = g++9

GAME_VERSION := $(shell cat ../../__REVISION__)

GccMajorVersion := $(shell expr `$(CC) -dumpversion | cut -f1 -d.`)
GccMinorVersion := $(shell expr `$(CC) -dumpversion | cut -f2 -d.`)
GccMinorEGT8 := $(shell expr $(GccMinorVersion) \>= 8)

INCDIR =
LIBDIR =
BINDIR = ..
OBJDIR = .obj
$(shell if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi)

## LIST OF CONSTANTS BEGIN
ENABLE_GOOGLE_TEST = 0
ENABLE_HSHIELD_SYSTEM = 0
ENABLE_XTRAP_SYSTEM = 0
ENABLE_LUA_5_VERSION = 0
ENABLE_GCC_AUTODEPEND = 1
ENABLE_STATIC = 1
## LIST OF CONSTANTS END

# Depend Path File
ifneq ($(ENABLE_GCC_AUTODEPEND), 1)
DEPFILE = Depend
endif

# Standard Libraries
LIBS = -lm -lmd

# Project Flags
CFLAGS  = -m32 -g -O0 -Wall -O2 -pipe -fexceptions -fno-strict-aliasing -pthread -D_THREAD_SAFE -DNDEBUG
CFLAGS += -Wno-deprecated-declarations -Wno-nonnull-compare -Wno-deprecated-declarations -Wno-array-bounds -Wno-address
CFLAGS += -Wno-int-in-bool-context -Wno-format-truncation -Wno-stringop-truncation -Wno-sign-compare
CXXFLAGS = -std=c++17 -Wl,-rpath=/usr/local/lib/gcc9

ifeq ($(ENABLE_STATIC), 1)
CFLAGS += -static
endif

# Version defines
CFLAGS += -D__USER__=\"$(USER)\" -D__HOSTNAME__=\"$(HOSTNAME)\" -D__PWD__=\"$(PWD)\" -D__GAME_VERSION__=\"$(GAME_VERSION)\"

# Boost
INCDIR += -I../../../Extern/include/boost

# DevIL
INCDIR += -I../../../Extern/include/IL
LIBS += ../../../Extern/lib/libIL.a\
		../../../Extern/lib/libjasper.a\
		../../../Extern/lib/libpng.a\
		../../../Extern/lib/libtiff.a\
		../../../Extern/lib/libjbig.a\
		../../../Extern/lib/libmng.a\
		/usr/lib/liblzma.a\
		../../../Extern/lib/liblcms.a\
		../../../Extern/lib/libjpeg.a

# MySQL
ifeq ($(BSD_VERSION), 7)
INCDIR += -I../../libmysql/7.x-5.1.35
LIBDIR += -L../../libmysql/7.x-5.1.35
else
INCDIR += -I../../libmysql/5.x-5.1.35
LIBDIR += -L../../libmysql/5.x-5.1.35
endif

LIBS += -lmysqlclient -lz

# Miscellaneous external libraries
INCDIR += -I../../../Extern/include
LIBDIR += -L../../../Extern/lib
# LIBS += -lcryptopp -lgtest
LIBS += -lcryptopp

# GTest
ifeq ($(ENABLE_GOOGLE_TEST), 1)
LIBS += /usr/local/lib/libgtest.a
CFLAGS += -DENABLE_GOOGLE_TEST
endif

#CFLAGS += -fsanitize=address -fsanitize-recover=address -fno-omit-frame-pointer

# OpenSSL
#LIBS += -lssl -lcrypto
# LIBS += /usr/lib/libssl.a

# Project Library
INCDIR += -I../../liblua/5.0/include
INCDIR += -I/usr/local/include

# Project Libraries
INCDIR += -I../../../Extern/include
INCDIR += -I/usr/local/include
LIBDIR += -L/usr/local/lib

LIBDIR += -L../../libthecore/lib -L../../libpoly -L../../libsql -L../../libgame/lib -L../../liblua/5.0/lib -L../../libserverkey
LIBDIR += -L/usr/local/lib
LIBS += -lthecore -lpoly -llua -llualib -lsql -lgame

# PROJECT_SRC_FILES BEGIN
MAINCPP = main.cpp

CFILE	= minilzo.c

CPPFILE = attr_transfer.cpp\
		  BattleArena.cpp FSM.cpp MarkConvert.cpp MarkImage.cpp MarkManager.cpp OXEvent.cpp ani.cpp\
		  arena.cpp banword.cpp battle.cpp blend_item.cpp buffer_manager.cpp building.cpp\
		  char.cpp char2.cpp char_affect.cpp char_battle.cpp char_change_empire.cpp char_horse.cpp char_item.cpp char_manager.cpp\
		  char_quickslot.cpp char_resist.cpp char_skill.cpp char_state.cpp PetSystem.cpp cmd.cpp cmd_emotion.cpp cmd_general.cpp\
		  cmd_gm.cpp cmd_oxevent.cpp config.cpp constants.cpp crc32.cpp db.cpp desc.cpp\
		  desc_client.cpp desc_manager.cpp desc_p2p.cpp dev_log.cpp dungeon.cpp empire_text_convert.cpp entity.cpp\
		  entity_view.cpp event.cpp event_queue.cpp exchange.cpp file_loader.cpp fishing.cpp gm.cpp guild.cpp\
		  guild_manager.cpp guild_war.cpp horse_rider.cpp horsename_manager.cpp input.cpp input_auth.cpp input_db.cpp\
		  input_login.cpp input_main.cpp input_p2p.cpp input_teen.cpp input_udp.cpp ip_ban.cpp\
		  item.cpp item_addon.cpp item_attribute.cpp item_manager.cpp item_manager_idrange.cpp locale.cpp\
		  locale_service.cpp log.cpp login_data.cpp lzo_manager.cpp marriage.cpp\
		  messenger_manager.cpp mining.cpp mob_manager.cpp motion.cpp over9refine.cpp p2p.cpp packet_info.cpp\
		  party.cpp pcbang.cpp polymorph.cpp priv_manager.cpp pvp.cpp\
		  questevent.cpp questlua.cpp questlua_affect.cpp questlua_arena.cpp questlua_ba.cpp questlua_building.cpp\
		  questlua_danceevent.cpp questlua_dungeon.cpp questlua_game.cpp questlua_global.cpp\
		  questlua_guild.cpp questlua_horse.cpp questlua_pet.cpp questlua_item.cpp questlua_marriage.cpp\
		  questlua_npc.cpp questlua_oxevent.cpp questlua_party.cpp questlua_pc.cpp\
		  questlua_quest.cpp questlua_target.cpp questmanager.cpp questnpc.cpp questpc.cpp\
		  refine.cpp regen.cpp safebox.cpp sectree.cpp sectree_manager.cpp sequence.cpp shop.cpp\
		  skill.cpp start_position.cpp target.cpp text_file_loader.cpp trigger.cpp utils.cpp vector.cpp war_map.cpp\
		  wedding.cpp version.cpp map_location.cpp\
		  skill_power.cpp affect.cpp\
		  FileMonitor_FreeBSD.cpp cipher.cpp\
		  buff_on_attributes.cpp dragon_soul_table.cpp DragonSoul.cpp\
		  group_text_parse_tree.cpp char_dragonsoul.cpp questlua_dragonsoul.cpp\
		  shop_manager.cpp shopEx.cpp item_manager_read_tables.cpp shutdown_manager.cpp new_switchbot.cpp MountSystem.cpp\
		  cuberenewal.cpp char_gaya.cpp\
		  new_offlineshop_manager.cpp new_offlineshop.cpp new_offlineshop_safebox.cpp battle_pass.cpp\
		  New_PetSystem.cpp questlua_petnew.cpp whisper_admin.cpp\
		  char_item2.cpp cmd_general2.cpp char_fishing.cpp\
		  hwidmanager.cpp cmd_gm2.cpp itemshop.cpp attendance.cpp\
		  
		  #unused-file-compilation
		  #cube.cpp
		  
		  
# PROJECT_SRC_FILES END

# PROJECT_OBJ_FILES BEGIN
COBJS	= $(CFILE:%.c=$(OBJDIR)/%.o)
CPPOBJS	= $(CPPFILE:%.cpp=$(OBJDIR)/%.o)
ifeq ($(ENABLE_GCC_AUTODEPEND), 1)
CDEPS	= $(COBJS:%.o=%.d)
CPPDEPS	= $(CPPOBJS:%.o=%.d)
endif

MAINOBJ = $(MAINCPP:%.cpp=$(OBJDIR)/%.o)
ifeq ($(ENABLE_GCC_AUTODEPEND), 1)
MAINDEPS = $(MAINOBJ:%.o=%.d)
endif

# PROJECT_OBJ_FILES END

# Target Paths
MAIN_TARGET = $(BINDIR)/game

default: $(MAIN_TARGET)
test: $(TEST_TARGET)

$(OBJDIR)/%.o: %.c
	@echo compiling $<
	@$(CC) $(CFLAGS) $(INCDIR) -c $< -o $@
ifeq ($(ENABLE_GCC_AUTODEPEND), 1)
	@$(CC) -MM -MG -MP $(CFLAGS) $(INCDIR) -c $< -o $(OBJDIR)/$*.d
	@sed -i '' -e's/$*.o:/$(OBJDIR)\/$*.o:/g' $(OBJDIR)/$*.d
endif

$(OBJDIR)/%.o: %.cpp
	@echo compiling $<
	@$(CC) $(CFLAGS) $(CXXFLAGS) $(INCDIR) -c $< -o $@
ifeq ($(ENABLE_GCC_AUTODEPEND), 1)
	@$(CC) -MM -MG -MP $(CFLAGS) $(CXXFLAGS) $(INCDIR) -c $< -o $(OBJDIR)/$*.d
	@sed -i '' -e's/$*.o:/$(OBJDIR)\/$*.o:/g' $(OBJDIR)/$*.d
endif

limit_time:
	@echo update limit time
	@python update_limit_time.py

$(MAIN_TARGET): $(CPPOBJS) $(COBJS) $(MAINOBJ)
	@echo linking $(MAIN_TARGET)
	@$(CC) $(CFLAGS) $(CXXFLAGS) $(LIBDIR) $(COBJS) $(CPPOBJS) $(MAINOBJ) $(LIBS) -o $(MAIN_TARGET)

dump:
	@$(CC) -dM -E - < /dev/null > .nope.nope

symlink:
	@ln -fs game_r$(GAME_VERSION) $(BINDIR)/game_symlink

strip:
	@cp $(MAIN_TARGET) $(BINDIR)/game_r
	@strip $(BINDIR)/game_r

clean:
	@echo cleaning $(MAIN_TARGET) $(OBJDIR)
	@rm -f $(COBJS) $(CPPOBJS) $(MAINOBJ)
ifeq ($(ENABLE_GCC_AUTODEPEND), 1)
	@rm -f $(CDEPS) $(CPPDEPS) $(MAINDEPS)
endif
	@rm -f $(BINDIR)/game_r* $(BINDIR)/test_r* $(BINDIR)/conv

dep:
ifeq ($(ENABLE_GCC_AUTODEPEND), 1)
	@echo "Note: gcc autodepend is autodetected, so target dep skipped"
else
	makedepend -f $(DEPFILE) $(INCDIR) -I/usr/include/c++/3.3 -I/usr/include/c++/4.2 -p$(OBJDIR)/ $(CPPFILE) $(CFILE) $(MAINCPP) $(TESTCPP) 2> /dev/null > $(DEPFILE)
endif

# AUTO_DEPEND_CHECK BEGIN
ifeq ($(ENABLE_GCC_AUTODEPEND), 1)
sinclude $(CDEPS)
sinclude $(CPPDEPS)
sinclude $(MAINDEPS)
else
sinclude $(DEPFILE)
endif
# AUTO_DEPEND_CHECK END

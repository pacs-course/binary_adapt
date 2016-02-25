#COMPILER FLAGS
CXX = g++ 
OPTIMIZE_OPS = -O3
CXXFLAGS = $(OPTIMIZE_OPS) -pthread -march=native -Wall -Wextra -std=c++11 $(INCLUDE_FLAGS) $(LIB_FLAGS)
INCLUDE_FLAGS = -I$(INCLUDE_DIR) -I$(SRC_DIR)
LIB_HMI = -lncurses++ -lcurses -lmenu -lform -lpanel
LIB_BOOST_FLAGS = -lboost_program_options
LIB_FLAGS = $(LIB_HMI) $(LIB_BOOST_FLAGS) -lhtmlcxx -Wl,-rpath,/usr/local/lib

#DIRECTORY TREE
OBJ_DIR = obj
INCLUDE_DIR = include
SRC_DIR = src
BIN_DIR = bin


OBJ_DIRS = $(OBJ_DIR)
BIN_DIRS = $(BIN_DIR)
DIRS = $(OBJ_DIRS) $(BIN_DIRS)



#SOURCES
COMMON_SRC = \
	     $(SRC_DIR)/base_player.cpp \
	     $(SRC_DIR)/calendar.cpp \
	     $(SRC_DIR)/game.cpp \
	     $(SRC_DIR)/nbaDB.cpp \
	     $(SRC_DIR)/league.cpp \
	     $(SRC_DIR)/league_mgr.cpp \
	     $(SRC_DIR)/league_updater.cpp \
	     $(SRC_DIR)/page_tree.cpp \
	     $(SRC_DIR)/player.cpp \
	     $(SRC_DIR)/roster.cpp \
	     $(SRC_DIR)/roster_checks_factory.cpp \
	     $(SRC_DIR)/scorer.cpp \
	     $(SRC_DIR)/sendmail.cpp \
	     $(SRC_DIR)/standings.cpp \
	     $(SRC_DIR)/team_checks_factory.cpp \
	     $(SRC_DIR)/team_validator.cpp \
	     $(SRC_DIR)/team_validator_factory.cpp \
	     $(SRC_DIR)/utils.cpp


#INCLUDES
DEPS = $(INCLUDE_DIR)/*.h

.PRECIOUS : %.dep

#%.dep : %.cpp
	    #$(CXX) $< $(CXXFLAGS) -MM -MF temp.dep
	    #sed -e"s/^\(.*\.o\)/$(OBJ_DIR)\/\/\1/" temp.dep > $@
	    #rm temp.dep

# $(call program-variables,variable-prefix,file-list)
define program-variables
    $(eval $1_APP = $1)
    $(eval $1_MAIN = $1App.cpp)
    $(eval $1_EXE = $(BIN_DIR)/$($1_APP))
    $(eval $1_SRC = $(SRC_DIR)/$($1_MAIN) $(foreach src,$(filter %.cpp,$2),$(SRC_DIR)/$(src)) $(COMMON_SRC))
    $(eval $1_INCLUDE = $(filter %.h,$2))
    $(eval $1_OBJ_tmp = $(subst .cpp,.o,$($1_SRC)))
    $(eval $1_OBJ = $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/$($1_DIR)/%,$($1_OBJ_tmp)))
    $(eval EXECUTABLES += $($1_EXE))

    $(shell touch $($1_OBJ:.o=.dep))
    $(eval include $($1_OBJ:.o=.dep))
    $($1_OBJ): $($1_INCLUDE)

    $(OBJ_DIR)/$($1_DIR)/%.o : $(SRC_DIR)/%.cpp
	    $(CXX) $(CXXFLAGS) -c -o $$@ $$<
	    $(CXX) $$< $$(CXXFLAGS) -MM -MF temp.dep
	    sed -e"s/^\(.*\.o\)/$(OBJ_DIR)\/\/\1/" temp.dep > $(OBJ_DIR)/$($1_DIR)/$$*.dep
	    rm temp.dep

    $($1_EXE): $($1_OBJ)
	    $(CXX) $($1_OBJ) -o $($1_EXE) $(CXXFLAGS)
endef


all:

$(eval $(call program-variables,acceptTeam,\
				))

$(eval $(call program-variables,dailyReport,\
				))

$(eval $(call program-variables,fantaMgr,\
				calendar_follower_wnd.cpp \
				curses_hmi.cpp \
				fanta_mgr.cpp \
				match_follower.cpp \
				match_follower_wnd.cpp \
				paged_output_window.cpp \
				roster_availability_wnd.cpp \
				rosters_wnd.cpp \
				))

$(eval $(call program-variables,nbaCrawler,\
				nbaCrawler.cpp \
				))

$(eval $(call program-variables,leagueMgr,\
				))

$(eval $(call program-variables,leagueUpdater,\
				))

$(eval $(call program-variables,rosterCost,\
				))

$(eval $(call program-variables,rtwrldCrawler,\
				rtwrldCrawler.cpp \
				))

$(eval $(call program-variables,teamVerifier,\
				))




###################################
#  sezione generazione programmi  #
###################################
.PHONY:all
all: dir_tree $(EXECUTABLES) #depend

.PHONY:debug
debug: OPTIMIZE_OPS = -g -DDEBUG #-DCOMPATIBILITY_MODE
debug: all

.PHONY:dir_tree
dir_tree: $(DIRS)

$(DIRS) :
	@if [ ! -e $@ ] ; then \
		echo creating $@ \
		; mkdir $@ \
	; fi

.PHONY:depend
depend : makefile.dependencies

makefile.dependencies : $(SRC_DIR)/*.dep
	cat $(SRC_DIR)/*.dep > makefile.dependencies


#######################
#  sezione logistica  #
#######################
.PHONY:backup
backup :
	@rsync -a --del --exclude='players/scores' --exclude='*stcz*' --exclude='bin' --exclude='obj' --out-format="%n%L" /home/francesco/Desktop/nbaCrawl/nbaCrawlApp/  /mnt/hgfs/Shared/nbaCrawlApp_Backup

.PHONY:local_backup
local_backup :
	@tar -czf ../backup/`date +"%Y%m%d-%H%M%S"`.tar.gz .

.PHONY:full_backup
full_backup : local_backup backup

.PHONY:clean
clean:
	@echo -n removing executables...
	@echo -n .
	@rm -f $(EXECUTABLES) 
	@echo  "done!"
	@echo -n removing object files...
	@echo -n .
	@find . -name "*.o" -delete
	@echo  "done!"
	@echo -n removing dependency files...
	@echo -n .
	@find . -name "*.dep" -delete
	@echo  "done!"
	@echo -n removing temporary files...
	@echo -n .
	@find . -name "*~" -delete
	@echo -n .
	@find . -name "*stcz*" -delete
	@echo  "done!"


############################
#  sezione scarico scores  #
############################
last :
	@./scripts/golastday.awk

today :
	@./scripts/gotoday.awk

alldays :
	@./scripts/go.sh

missingdays : 
	@./scripts/update_scores.sh



##########################################
#  sezione generazione file di utilita'  #
##########################################
lists : playerlist coachlist gamestats

#playerlist : players/players.list
playerlist :
	@./scripts/gen_players_list.sh

coachlist : players/coaches.dunkest

gamestats : calendar/game.stats


players/players.list : ./scores
	@./scripts/gen_players_list.sh

players/coaches.dunkest : players/players.list
	@./scripts/gendunkestcoaches.sh

calendar/game.stats : ./scores
	@./scripts/gengamestats.sh




########################
#  sezione fantaleghe  #
########################
notify : $(BIN_DIR)/leagueUpdater
	@$(BIN_DIR)/leagueUpdater




##########################
#  sezione reportistica  #
##########################
day_report : update
	@./bin/dailyReport > ./reports/daily_report.`date +"%Y%m%d"` 2>&1

signals :
	@./scripts/gencompletepscores.awk ./players/players.list




#########################
#  sezione statistiche  #
#########################
avg : playerlist all_avg

all_avg : ./scores
	@./scripts/average_player_scores.awk -v REF_LEN=100 ./players/players.list > all_avg
	@sed -e "s/^\(.*==> .*avg=\)\([0-9,\.]*\)/\2	\1\2/" all_avg > temp
	@sort -n -r temp > all_avg
	@rm temp
	@./scripts/remaining_performers.sh > remaining_performers

hall_of_fame : ./scores
	@./scripts/hall_of_fame.sh

stats : hall_of_fame avg


live :	today gamestats

update : missingdays gamestats

full :	all stats backup


#-include makefile.dependencies

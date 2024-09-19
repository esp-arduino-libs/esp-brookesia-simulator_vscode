#
# Makefile
# WARNING: relies on invocation setting current working directory to Makefile location
# This is done in .vscode/task.json
#

# Detect the operating system
UNAME_S := $(shell uname -s)

# select underlaying LCGL display driver (SDL2 || X11)
# LV_DRIVER			:= X11
LV_DRIVER			:= SDL2

PROJECT 			?= esp-brookesia-simulator_vscode
C_SRC_EXT	  		:= c
CXX_SRC_EXT			:= cpp
C_OBJ_EXT			:= o
CXX_OBJ_EXT			:= opp
ifeq ($(UNAME_S), Darwin)  	# macOS
	MAKEFLAGS 		:= -j $(shell sysctl -n hw.ncpu)
	CC  			:= arch -x86_64 gcc
	CXX 			:= arch -x86_64 g++
else ifeq ($(UNAME_S), Linux)  # Linux
	MAKEFLAGS 		:= -j $(shell nproc)
	CC  			:= gcc
	CXX 			:= g++
endif

WORKING_DIR			:= ./build
BUILD_DIR			:= $(WORKING_DIR)/obj
BIN_DIR				:= $(WORKING_DIR)/bin

MAIN_DIR 			:= ./main
COMPONENTS_DIR 		:= ./components
LVGL_DIR 			:= ${COMPONENTS_DIR}/lvgl
LVGL_DRIVER_DIR 	:= ${COMPONENTS_DIR}/lv_drivers
ESP_BROOKESIA_DIR 	:= ${COMPONENTS_DIR}/esp-brookesia
ESP_BROOKESIA_STYLESHEET_DIR 	:= ${COMPONENTS_DIR}/esp-brookesia-stylesheet
ESP_BROOKESIA_APP_DIR 			:= ${COMPONENTS_DIR}/esp-brookesia-app

SRC_INCLUDE_DIRS 	:= ${MAIN_DIR} ${LVGL_DIR} ${LVGL_DRIVER_DIR} ${ESP_BROOKESIA_DIR}/src ${ESP_BROOKESIA_APP_DIR}
SRC_EXCLUDE_DIRS 	:= ${LVGL_DIR}/tests ${LVGL_DIR}/demos ${LVGL_DIR}/examples ${LVGL_DIR}/env_support
INC_DIRS 			:= ${SRC_INCLUDE_DIRS} ${COMPONENTS_DIR}

ifeq ($(UNAME_S), Darwin)  	# macOS
C_WARNINGS 			:= 	-Wall -Wextra \
						-Wshadow -Wundef -Wmissing-prototypes \
						-Wno-unused-function -Wno-error=strict-prototypes -Wpointer-arith -fno-strict-aliasing \
						-Wno-error=cpp -Wuninitialized -Wno-unused-parameter -Wno-missing-field-initializers \
						-Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default \
						-Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers -Wno-error=pedantic \
						-Wno-sign-compare -Wno-error=missing-prototypes -Wdeprecated  \
						-Wempty-body -Wshift-negative-value \
						-Wtype-limits -Wsizeof-pointer-memaccess -Wpointer-arith
CXX_WARNINGS 		:= 	-Wall -Wextra \
						-Wshadow -Wundef \
						-Wno-unused-function -Wno-error=strict-prototypes -Wpointer-arith -fno-strict-aliasing \
						-Wno-error=cpp -Wuninitialized -Wno-unused-parameter -Wno-missing-field-initializers \
						-Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default \
						-Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers -Wno-error=pedantic \
						-Wno-sign-compare -Wno-error=missing-prototypes -Wdeprecated  \
						-Wempty-body -Wshift-negative-value \
						-Wtype-limits -Wsizeof-pointer-memaccess -Wpointer-arith
else ifeq ($(UNAME_S), Linux)  # Linux
C_WARNINGS 			:= 	-Wall -Wextra \
						-Wshadow -Wundef -Wmaybe-uninitialized -Wmissing-prototypes -Wno-discarded-qualifiers \
						-Wno-unused-function -Wno-error=strict-prototypes -Wpointer-arith -fno-strict-aliasing \
						-Wno-error=cpp -Wuninitialized -Wno-unused-parameter -Wno-missing-field-initializers \
						-Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default \
						-Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers -Wno-error=pedantic \
						-Wno-sign-compare -Wno-error=missing-prototypes -Wclobbered -Wdeprecated  \
						-Wempty-body -Wshift-negative-value -Wstack-usage=2048 \
						-Wtype-limits -Wsizeof-pointer-memaccess -Wpointer-arith
CXX_WARNINGS 		:= 	-Wall -Wextra \
						-Wshadow -Wundef -Wmaybe-uninitialized \
						-Wno-unused-function -Wno-error=strict-prototypes -Wpointer-arith -fno-strict-aliasing \
						-Wno-error=cpp -Wuninitialized -Wno-unused-parameter -Wno-missing-field-initializers \
						-Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wno-switch-default \
						-Wreturn-type -Wmultichar -Wformat-security -Wno-ignored-qualifiers -Wno-error=pedantic \
						-Wno-sign-compare -Wno-error=missing-prototypes -Wclobbered -Wdeprecated  \
						-Wempty-body -Wshift-negative-value -Wstack-usage=2048 \
						-Wtype-limits -Wsizeof-pointer-memaccess -Wpointer-arith
endif

CFLAGS 			:= -O0 -g -std=c11 $(C_WARNINGS)
CXX_FLAGS   	:= -O0 -g -std=c++14 $(CXX_WARNINGS)

# simulator library define
ifeq  "$(LV_DRIVER)" "SDL2"
LV_DRIVER_USE	:= USE_SDL
else
LV_DRIVER_USE	:= USE_$(LV_DRIVER)
endif

# Add simulator defines to allow modification of source
DEFINES				:= -D SIMULATOR=1 -D LV_BUILD_TEST=0 -D $(LV_DRIVER_USE) -D ESP_BROOKESIA_KCONFIG_IGNORE

# Include simulator inc folder first so lv_conf.h from custom UI can be used instead
INC 				:= $(patsubst %,-I%,$(INC_DIRS)) #-I/usr/include/freetype2 -L/usr/local/lib
LDLIBS	 			:= -l$(LV_DRIVER) -lpthread -lm #-lfreetype -lavformat -lavcodec -lavutil -lswscale -lm -lz
BIN 				:= $(BIN_DIR)/${PROJECT}

C_COMPILE			= $(CC) $(CFLAGS) $(INC) $(DEFINES)
CXX_COMPILE			= $(CXX) $(CXX_FLAGS) $(INC) $(DEFINES)

# Automatically include all source files
SRCS	:= 	$(shell find $(SRC_INCLUDE_DIRS) -type f \( -name '*.${C_SRC_EXT}' -o -name '*.${CXX_SRC_EXT}' \) \
			$(foreach dir,$(SRC_EXCLUDE_DIRS),-not -path '$(dir)/*'))
OBJECTS	:= 	$(patsubst %.${C_SRC_EXT},$(BUILD_DIR)/%.${C_OBJ_EXT},$(filter %.${C_SRC_EXT},$(SRCS))) \
			$(patsubst %.${CXX_SRC_EXT},$(BUILD_DIR)/%.${CXX_OBJ_EXT},$(filter %.${CXX_SRC_EXT},$(SRCS)))

EXTRA_FILES		:= Makefile $(shell find $(INC_DIRS) -type f \( -name "*.h" -o -name "*.hpp" \))

$(BUILD_DIR)/%.${C_OBJ_EXT}: %.${C_SRC_EXT} ${EXTRA_FILES}
	@echo 'Building project file: $<'
	@mkdir -p $(dir $@)
	@$(C_COMPILE) -c -o $@ $<

$(BUILD_DIR)/%.${CXX_OBJ_EXT}: %.${CXX_SRC_EXT} ${EXTRA_FILES}
	@echo 'Building project file: $<'
	@mkdir -p $(dir $@)
	@$(CXX_COMPILE) -c -o $@ $<

$(BUILD_DIR)/%.${C_OBJ_EXT}: ${ESP_BROOKESIA_DIR}/%.${C_SRC_EXT} ${EXTRA_FILES}
	@echo 'Building esp-brookesia file: $<'
	@mkdir -p $(dir $@)
	@$(C_COMPILE) -c -o $@ $<

$(BUILD_DIR)/%.${CXX_OBJ_EXT}: ${ESP_BROOKESIA_DIR}/%.${CXX_SRC_EXT} ${EXTRA_FILES}
	@echo 'Building esp-brookesia file: $<'
	@mkdir -p $(dir $@)
	@$(CXX_COMPILE) -c -o $@ $<

$(BIN): $(OBJECTS)
	@echo 'Linking bin: $@'
	@mkdir -p $(BIN_DIR)
	@$(CXX) -o $@ $^ $(LD_FLAGS) ${LDLIBS}

clean:
	rm -rf $(WORKING_DIR)

install: ${BIN}
	install -d ${DESTDIR}/usr/lib/${PROJECT}/bin
	install $< ${DESTDIR}/usr/lib/${PROJECT}/bin/

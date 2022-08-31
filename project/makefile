MODELS_DIR := models
OBJ_CONVERTER_DIR := obj_converter
3D_ENGINE_DIR := 3d_engine
OBJ_CONVERTER_DATA_DIR := $(OBJ_CONVERTER_DIR)/data
3D_ENGINE_DATA_DIR := $(3D_ENGINE_DIR)/data
OBJ_FILE := $(MODELS_DIR)/$(model)/$(model).obj
MTL_FILE := $(MODELS_DIR)/$(model)/$(model).mtl
LGT_FILE = $(MODELS_DIR)/$(model)/$(model).lgt 
MODEL_DEPEND := $(model).lastconvert
DATA_INCLUDE := data_structures.h
DATA_FILE := data.h
DATA_SRC := $(OBJ_CONVERTER_DATA_DIR)/$(DATA_FILE)
EXECUTABLE := $(3D_ENGINE_DIR)/out/$(3D_ENGINE_DIR).bin

_MAKE_DATA_DIRS_ := $(shell mkdir -p $(OBJ_CONVERTER_DATA_DIR) \
									 $(3D_ENGINE_DATA_DIR))

.PHONY: view converter convert build clean

converter:
	@echo "Build: converter"
	@cd $(OBJ_CONVERTER_DIR) && make release && cd ..
	@echo "Converter builded."

convert: converter
	@echo "Convert: $(notdir $(OBJ_FILE)) $(notdir $(MTL_FILE)) $(notdir $(LGT_FILE))"
	@cd $(OBJ_CONVERTER_DIR) \
		&& make convert file_obj=../$(OBJ_FILE) \
						file_mtl=../$(MTL_FILE) \
						file_lgt=../$(LGT_FILE) \
						file_inc=$(DATA_INCLUDE) \
						file_dst=$(DATA_FILE) \
		&& mv $(DATA_FILE) ../$(DATA_SRC) \
		&& cd ..
	@echo "Image converted."

$(MODEL_DEPEND): $(OBJ_FILE) $(MTL_FILE) $(LGT_FILE)
	@rm -f *.lastconvert
	@make convert
	@touch $(MODEL_DEPEND)

build: $(MODEL_DEPEND)
	@cp $(DATA_SRC) $(3D_ENGINE_DATA_DIR)
	@echo "Build: 3D Engine"
	@cd $(3D_ENGINE_DIR) && make all && cd ..

copy:
	@echo "Copying to a device: $(notdir $(EXECUTABLE))"
	@-sudo umount $(device)
	@sudo rm -rf $(target)1
	@sudo mkdir -p $(target)1
	@-sudo mount $(device) $(target)1
	@sudo cp $(EXECUTABLE) $(target)1
	@-sudo umount $(device)
	@sudo rm -rf $(target)1
	@echo "Copy completed."

demo: build
	make copy

view: $(MODEL_DEPEND)
	@cp $(DATA_SRC) $(3D_ENGINE_DATA_DIR)
	@echo "Build: 3D Engine"
	@cd $(3D_ENGINE_DIR) && make all flag=view && cd ..
	make copy

measuring_time: $(MODEL_DEPEND)
	@cp $(DATA_SRC) $(3D_ENGINE_DATA_DIR)
	@echo "Build: 3D Engine"
	@cd $(3D_ENGINE_DIR) && make all flag=measuring_time && cd ..
	make copy

measuring_memory: $(MODEL_DEPEND)
	@cp $(DATA_SRC) $(3D_ENGINE_DATA_DIR)
	@echo "Build: 3D Engine"
	@cd $(3D_ENGINE_DIR) && make all flag=measuring_memory && cd ..
	make copy

analyze:
	cppcheck --enable=all --language=c++ $(OBJ_CONVERTER_DIR)/src/*.cpp $(3D_ENGINE_DIR)/src/*.cpp

clean:
	@cd $(OBJ_CONVERTER_DIR) && make clean && cd ..
	@cd $(3D_ENGINE_DIR) && make clean && cd ..
	@rm -rf $(OBJ_CONVERTER_DATA_DIR) $(3D_ENGINE_DATA_DIR) *.lastconvert
	@echo "Project cleared."

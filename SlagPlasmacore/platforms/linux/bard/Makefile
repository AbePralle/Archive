BARD_SRC = ../../../libraries/native/bard
VM_FLAGS = -O3 -c -w -fno-strict-aliasing -DBARD_VM -I $(BARD_SRC)
XC_FLAGS = -O3 -c -w -fno-strict-aliasing -DBARD_XC -I $(BARD_SRC)
BUILD_DIR = ../build/bard

VM_OFILES = $(BUILD_DIR)/vm/bard_runtime.o \
            $(BUILD_DIR)/vm/bard_mm.o \
            $(BUILD_DIR)/vm/bard_stdlib.o \
            $(BUILD_DIR)/vm/bard_vm.o

XC_OFILES = $(BUILD_DIR)/xc/bard_runtime.o \
            $(BUILD_DIR)/xc/bard_mm.o \
            $(BUILD_DIR)/xc/bard_stdlib.o \
            $(BUILD_DIR)/xc/bard_xc.o

bard: $(BUILD_DIR) $(BUILD_DIR)/bard_vm.a $(BUILD_DIR)/bard_xc.a

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)/vm
	mkdir -p $(BUILD_DIR)/xc
	
$(BUILD_DIR)/bard_vm.a: $(VM_OFILES)
	ar rcs $(BUILD_DIR)/bard_vm.a $(VM_OFILES)
 
$(BUILD_DIR)/vm/bard_runtime.o: $(BARD_SRC)/bard_runtime.cpp
	g++ $(VM_FLAGS) $(BARD_SRC)/bard_runtime.cpp -o $(BUILD_DIR)/vm/bard_runtime.o

$(BUILD_DIR)/vm/bard_mm.o: $(BARD_SRC)/bard_mm.cpp
	g++ $(VM_FLAGS) $(BARD_SRC)/bard_mm.cpp -o $(BUILD_DIR)/vm/bard_mm.o

$(BUILD_DIR)/vm/bard_stdlib.o: $(BARD_SRC)/bard_stdlib.cpp
	g++ $(VM_FLAGS) $(BARD_SRC)/bard_stdlib.cpp -o $(BUILD_DIR)/vm/bard_stdlib.o

$(BUILD_DIR)/vm/bard_vm.o: $(BARD_SRC)/bard_vm.cpp
	g++ $(VM_FLAGS) $(BARD_SRC)/bard_vm.cpp -o $(BUILD_DIR)/vm/bard_vm.o

$(BUILD_DIR)/vm/bard_xc.o: $(BARD_SRC)/bard_xc.cpp
	g++ $(VM_FLAGS) $(BARD_SRC)/bard_vm.cpp -o $(BUILD_DIR)/vm/bard_vm.o


$(BUILD_DIR)/bard_xc.a: $(XC_OFILES)
	ar rcs $(BUILD_DIR)/bard_xc.a $(XC_OFILES)
 
$(BUILD_DIR)/xc/bard_runtime.o: $(BARD_SRC)/bard_runtime.cpp
	g++ $(XC_FLAGS) $(BARD_SRC)/bard_runtime.cpp -o $(BUILD_DIR)/xc/bard_runtime.o

$(BUILD_DIR)/xc/bard_mm.o: $(BARD_SRC)/bard_mm.cpp
	g++ $(XC_FLAGS) $(BARD_SRC)/bard_mm.cpp -o $(BUILD_DIR)/xc/bard_mm.o

$(BUILD_DIR)/xc/bard_stdlib.o: $(BARD_SRC)/bard_stdlib.cpp
	g++ $(XC_FLAGS) $(BARD_SRC)/bard_stdlib.cpp -o $(BUILD_DIR)/xc/bard_stdlib.o

$(BUILD_DIR)/xc/bard_xc.o: $(BARD_SRC)/bard_xc.cpp
	g++ $(XC_FLAGS) $(BARD_SRC)/bard_xc.cpp -o $(BUILD_DIR)/xc/bard_xc.o


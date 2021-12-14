# LIBEXT2FS FOR PLAYSTATION 2
# Copyright (c) 2021 André Guilherme Mendes da luz bastos based on libsmb2 makefile

IOP_CFLAGS += -Wall -Os -I. -I../include -I../include/extfs

EE_CFLAGS += -Wall -Os -I. -I../include -I../include/extfs

IOP_LIB = libext2fs.a

EE_LIB = libext2fs.a

IOP_OBJS = source/ext2_err.o source/hashmap.o source/rbtree.o 

EE_OBJS = source/ext2_err.o source/hashmap.o source/rbtree.o

IOP: $(IOP_LIB) 
ifeq ($(PS2DEV),)
	@echo "$PS2DEV ENVIROMENT is not set. Could not install libext2fs."
	@exit 1
endif
	@echo Copying...
	@cp -frv include/ext2.h $(PS2SDK)/iop/include/
	@cp -f $(IOP_LIB) $(PS2SDK)/iop/lib
	@echo Done!

EE: $(EE_LIB)
ifeq ($(PS2DEV),)
	@echo "$PS2DEV ENVIROMENT is not set. Could not install libntfs."
	@exit 1
endif
	@echo Copying...
	@cp -frv include/EE/ext2.h $(PS2SDK)/ee/include/
	@cp -f $(EE_LIB) $(PS2SDK)/ee/lib
	@echo Done!
	
clean:
	@rm -f -r $(EE_LIB) $(EE_OBJS) 
	@rm -f -r $(IOP_LIB) $(IOP_OBJS) $(IOP_OBJS_DIR)

install: IOP EE clean


include $(PS2SDK)/Defs.make
include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.iopglobal
include $(PS2SDK)/samples/Makefile.eeglobal 


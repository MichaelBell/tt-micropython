HOVALAAG_ASM_MOD_DIR := $(USERMOD_DIR)

# Add all C files to SRC_USERMOD.
SRC_USERMOD += $(HOVALAAG_ASM_MOD_DIR)/hovalaag_module.c

# We can add our module folder to include paths if needed
# This is not actually needed in this example.
CFLAGS_USERMOD += -I$(HOVALAAG_ASM_MOD_DIR)
CEXAMPLE_MOD_DIR := $(USERMOD_DIR)

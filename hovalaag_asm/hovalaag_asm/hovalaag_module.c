// Include MicroPython API.
#include "py/runtime.h"

#include "py/objstr.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "hovalaag_assembler.h"

// This is the function which will be called from Python as cexample.add_ints(a, b).
STATIC mp_obj_t hovalaag_assemble(mp_obj_t text_obj) {
    if (!mp_obj_is_type(text_obj, &mp_type_tuple)) {
        mp_raise_ValueError("hovalaag_assemble(): text should be tuple of strings");
    }
    
    // Extract the ints from the micropython input objects.
    mp_obj_tuple_t* text = (mp_obj_tuple_t*)MP_OBJ_TO_PTR(text_obj);
    char** strings = m_new(char *, text->len);
    char** strings_copy = m_new(char *, text->len);

    for (int i = 0; i < text->len; ++i) {
        GET_STR_DATA_LEN(text->items[i], str, len)
        strings[i] = m_malloc(len+1);
        strings_copy[i] = strings[i];
        memcpy(strings[i], str, len+1);
    }

    vls_assemble(strings, text->len);

    for (int i = 0; i < text->len; ++i) {
        m_free(strings_copy[i]);
    }
    m_del(char*, strings, text->len);

    mp_obj_t program_obj = mp_obj_new_list(hovalaag_num_instructions, NULL);
    mp_obj_list_t* program = (mp_obj_list_t*)MP_OBJ_TO_PTR(program_obj);

    for (int i = 0; i < hovalaag_num_instructions; ++i)
    {
        program->items[i] = mp_obj_new_int(hovalaag_program[i]);
    }

    return program_obj;
}
// Define a Python reference to the function above.
STATIC MP_DEFINE_CONST_FUN_OBJ_1(hovalaag_assemble_obj, hovalaag_assemble);

// Define all properties of the module.
// Table entries are key/value pairs of the attribute name (a string)
// and the MicroPython object reference.
// All identifiers and strings are written as MP_QSTR_xxx and will be
// optimized to word-sized integers by the build system (interned strings).
STATIC const mp_rom_map_elem_t example_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_hovalaag_asm) },
    { MP_ROM_QSTR(MP_QSTR_assemble), MP_ROM_PTR(&hovalaag_assemble_obj) },
};
STATIC MP_DEFINE_CONST_DICT(example_module_globals, example_module_globals_table);

// Define module object.
const mp_obj_module_t hovalaag_asm_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&example_module_globals,
};

// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_hovalaag_asm, hovalaag_asm_cmodule);

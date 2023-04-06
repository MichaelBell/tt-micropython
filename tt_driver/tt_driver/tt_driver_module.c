// Include MicroPython API.
#include "py/runtime.h"

#include "py/objstr.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "tt.h"

static void ensure_initialized() {
    static bool is_init = false;
    if (!is_init) {
        tt_pio_init();
        is_init = true;
    }
}

// This is the function which will be called from Python as tt_driver.send_byte()
STATIC mp_obj_t send_byte(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_b, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_latch, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_scan, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    ensure_initialized();
    int res = tt_send_byte(args[0].u_int, args[1].u_bool, args[2].u_bool);

    return mp_obj_new_int(res);
}

STATIC mp_obj_t send_byte_repeat(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_b, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_repeats, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_read, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_latch, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_scan, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    ensure_initialized();
    int res = tt_send_byte_repeat(args[0].u_int, args[1].u_int, args[2].u_bool, args[3].u_bool, args[4].u_bool);

    return mp_obj_new_int(res);
}

STATIC mp_obj_t send_bytes(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_data, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_read, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_latch, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_scan, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint8_t data[256];
    int len = 0;
    if (mp_obj_is_type(args[0].u_obj, &mp_type_tuple)) {
        mp_obj_tuple_t* data_tuple = (mp_obj_tuple_t*)MP_OBJ_TO_PTR(args[0].u_obj);
        len = data_tuple->len;
        if (len > 256) len = 256;
        for (int i = 0; i < len; ++i) {
            data[i] = mp_obj_get_int(data_tuple->items[i]);
        }
    } else if (mp_obj_is_type(args[0].u_obj, &mp_type_list)) {
        mp_obj_list_t* data_list = (mp_obj_list_t*)MP_OBJ_TO_PTR(args[0].u_obj);
        len = data_list->len;
        if (len > 256) len = 256;
        for (int i = 0; i < len; ++i) {
            data[i] = mp_obj_get_int(data_list->items[i]);
        }
    } else {
        mp_raise_ValueError("send_bytes(): data should be tuple or list of ints");
    }

    ensure_initialized();
    int res = tt_send_bytes(data, len, args[1].u_bool, args[2].u_bool, args[3].u_bool);

    return mp_obj_new_int(res);
}

// Define a Python reference to the function above.
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(send_byte_obj, 1, send_byte);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(send_byte_repeat_obj, 2, send_byte_repeat);
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(send_bytes_obj, 1, send_bytes);

// Define all properties of the module.
// Table entries are key/value pairs of the attribute name (a string)
// and the MicroPython object reference.
// All identifiers and strings are written as MP_QSTR_xxx and will be
// optimized to word-sized integers by the build system (interned strings).
STATIC const mp_rom_map_elem_t tt_driver_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_tt_driver) },
    { MP_ROM_QSTR(MP_QSTR_send_byte), MP_ROM_PTR(&send_byte_obj) },
    { MP_ROM_QSTR(MP_QSTR_send_byte_repeat), MP_ROM_PTR(&send_byte_repeat_obj) },
    { MP_ROM_QSTR(MP_QSTR_send_bytes), MP_ROM_PTR(&send_bytes_obj) },
};
STATIC MP_DEFINE_CONST_DICT(tt_driver_module_globals, tt_driver_module_globals_table);

// Define module object.
const mp_obj_module_t tt_driver_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&tt_driver_module_globals,
};

// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_tt_driver, tt_driver_cmodule);

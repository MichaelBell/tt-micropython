#pragma once

#define MAX_INSTRUCTIONS 256

extern uint32_t hovalaag_program[MAX_INSTRUCTIONS];
extern int hovalaag_num_instructions;
extern char **hovalaag_labels;

void vls_assemble(char **lines, int len);
